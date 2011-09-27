#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>

#include <XmlTempString.h>
#include <xercesDomUtil.h>

#include "AuroraNode.h"
#include "QBox.h"
#include "HAUnit.h"

#include "AggregateQBox.h"
#include "AuroraJoinQBox.h"
#include "BSortQBox.h"
#include "FilterQBox.h"
#include "FilterArrayQBox.h"
#include "JoinQBox.h"
#include "LockQBox.h"
#include "MapQBox.h"
#include "RandomDropQBox.h"
#include "RevisionAggregateQBox.h"
#include "RevisionFilterQBox.h"
#include "RevisionMapQBox.h"
#include "SControlQBox.h"
#include "SJoinQBox.h"
#include "SOutputQBox.h"
#include "SUnionQBox.h"
#include "UnionQBox.h"
#include "UnlockQBox.h"
#include "WaitForQBox.h"
#include "WindowDropQBox.h"

#include "CPQBox.h"
#include "SweeperQBox.h"


BOREALIS_NAMESPACE_BEGIN


// Look at other serial defines in Object.cc
NMSTL_SERIAL_DEFINE(QueueState, 8);


////////////////////////////////////////////////////////////////////////////////
//
// Create a new instance of a box.
//
QBox* QBox::instantiate(const  string  &type)
                  throw(AuroraBadEntityException)
{
    QBox  *box;
//..............................................................................


    box = instantiate_udb(type);

    if (box == NULL)
    {   box = instantiate_bdb(type);
    }
    
    if (box == NULL)
    {   box = instantiate_array(type);
    }

    if (box == NULL)
    {   if (type == "aurorajoin")             box = new AuroraJoinQBox;
        else if (type == "aggregate")         box = new AggregateQBox;
        else if (type == "bsort")             box = new BSortQBox;
        else if (type == "filter")            box = new FilterQBox;
        else if (type == "filter_array")      box = new FilterArrayQBox;
        else if (type == "join")              box = new JoinQBox;
        else if (type == "lock")              box = new LockQBox;
        else if (type == "map")               box = new MapQBox;
        else if (type == "random_drop")       box = new RandomDropQBox;
        else if (type == "revisionaggregate") box = new RevisionAggregateQBox;
        else if (type == "revisionfilter")    box = new RevisionFilterQBox;
        else if (type == "revisionmap")       box = new RevisionMapQBox;
        else if (type == "scontrol")          box = new SControlQBox;
        else if (type == "sjoin")             box = new SJoinQBox;
        else if (type == "soutput")           box = new SOutputQBox;
        else if (type == "sunion")            box = new SUnionQBox;
        else if (type == "union")             box = new UnionQBox;
        else if (type == "unlock")            box = new UnlockQBox;
        else if (type == "waitfor")           box = new WaitForQBox;
        else if (type == "window_drop")       box = new WindowDropQBox;
        else if (type == "connectionpoint")   box = new CPQBox;
        else if (type == "sweeper")           box = new SweeperQBox;
        else
        {   Throw(AuroraBadEntityException, "Unknown box:  " + type);
        }
    }

    return(box);
}



////////////////////////////////////////////////////////////////////////////////
//
string QBox::get_box_type(const DOMElement *element)
                    throw(AuroraBadEntityException)
{
//..............................................................................


    string tag_name = to_string(element->getTagName());

    if (tag_name == "cpoint")
        return "__cpoint";

    if (tag_name != "box")
        Throw(AuroraBadEntityException, "Expected <box> but got <" + tag_name + ">");

    return xml_attribute(element, "type", ATTR_NON_EMPTY);
}



QBox::QBox() :
    _state(CONSTRUCTED),
    _backup(false),
    _passive(false)
{}


QBox::~QBox()
{
    /***************************** fix this up???
    for (unsigned int i = 0; i < _inputs.size(); ++i)
    {   if (_inputs[i])
        {   _inputs[i]->removeDestPort( QBoxInputPort( this, i ));
        }
    }

    for (unsigned int i = 0; i < _outputs.size(); ++i)
    {   if (_outputs[i])
        {   _outputs[i]->setSourcePort(QBoxOutputPort());
        }
    }
    **********************************/
}

string QBox::as_string() const
{
    return "QBox{name=" + get_name() + "; type=" + getType()
    + "; params=" + to_string(*(_local_box->get_box_parameter_map())) + "}";
}

// When unpacking the state of a queue, we either want to add the tuples
// or clear the queue and replace the tuples. To do the latter, we must
// explicitly clear the content of the queue
void QBox::clear_queue(ptr<QueueState> qs)
{
    for (unsigned int i = 0; i < _inputs.size(); ++i)
    {
        INFO << "Attempting to clear queue!";
        TupleQueue::DeqIterator deq = _inq[i]->deq_iterator();
        while (deq.avail())
        { ++deq;
        }
    }

}

Time QBox::get_expected_capture_cost() 
{
    return Time::secs(1);
};



Time QBox::get_expected_paste_cost()
{
    return Time::secs(1);
};



void QBox::unpack_queue(ptr<QueueState> qs)
{
    set_pending_queue_state(qs);
    for (unsigned int i = 0; i < _inputs.size(); ++i)
    {
        INFO << "Attempting to unpack queue!";
        unpack_queue(*(_inputs[i]), _inq[i]->enq_iterator());
    }
}



void QBox::unpack_queue(Stream& stream, TupleQueue::EnqIterator enq)
{
    int notify_after_nb_tuples = 1000;

    if ( _pending_queue_state )
    {
        QueueState& packed_queues = *(_pending_queue_state);

        if (packed_queues.contains_stream(stream.get_name()))
        {
            string stream_name = stream.get_name();
            unsigned int tuple_size = stream.get_tuple_description().get_size();
            unsigned int num_tuples = packed_queues.number_of_tuples(stream_name, tuple_size);

            DEBUG << "Unpacking tuples for stream " << stream_name;

            string tuple_batch(tuple_size * num_tuples, ' ');
            if ( tuple_batch.capacity() < (tuple_size * num_tuples) )
            {
                WARN << "Unable to allocate temporary storage for batch unpacking, "
                     << "capacity requested: " << tuple_size * num_tuples
                     << " capacity obtained: " << tuple_batch.capacity();
                return;
            }

            if (!packed_queues.get_tuple_batch_from_queue((char*) tuple_batch.data(), stream_name, tuple_size, num_tuples))
            {
                WARN << "Queue deserialization of stream " << stream_name << " failed.";
                return;
            }

            const char* tuple = tuple_batch.data();
            unsigned int i = 0;
            for(; i < num_tuples; i++)
            {
                memcpy(enq.tuple(), tuple, tuple_size);

                Tuple t = Tuple::factory(tuple);
                if ( t.get_tuple_type() == TupleType::TENTATIVE)
                {
                    WARN << "Unpacked a tentative tuple " << t.get_tuple_stime();
                } 

                tuple += tuple_size;
                if ( (i % notify_after_nb_tuples) == 0 )
                { DEBUG << "Unpacked tuple " << i << " for stream: " << stream_name;
                }
            }

            DEBUG << "Unpacked " << i << " tuples for stream: " << stream_name;
        }
    }
}



void QBox::setup(AuroraNode& node, CatalogBox* box) throw (AuroraException)
{
    assert(_state == CONSTRUCTED);

    _node = &node;

    _local_box = box;

    // all boxes are uninvertible by default, set to true by individual boxes
    _invertible = false;
    _revision_processing_mode = NONE;
    
    // local box ids for downstream revision processing
    _local_boxid = node.lookup_box_id( box->get_box_name() );
    assert( _local_boxid != -1 );

    try
    {
        setup_impl();
        _state = SETUP;
    } catch (AuroraException &e)
    {
        _state = DEAD;
        Throw(AuroraException,
              "Exception while setting up box " + get_name() + ": " + e);
    } catch (exception&)
    {
        _state = DEAD;
        throw;
    }

    // -- tatbul
    //_stats_file.set(new ofstream(string("/tmp/"+get_name()+".txt").c_str()));
}



////////////////////////////////////////////////////////////////////////////////
//
// Init the box.  The box must have been setup but not inited.
//
void QBox::init(PagePool& pool) throw (AuroraException)
{
//..............................................................................


    assert(_state == SETUP);

    // Make sure that the number of input/output streams is
    // correct (might also want to check that the descriptions
    // match up)

    if ( _local_box->get_box_in()->size() != _inputs.size())
    {   Throw(AuroraBadEntityException,
              "Number of input streams in \"" + get_name() + "\" differs from number of input descriptions");
    }

    if ( _local_box->get_box_out()->size() != _outputs.size())
    {   Throw( AuroraBadEntityException,
               "Number of output streams in \"" + get_name() + "\" ("
                  + _outputs.size()
                  + ") differs from number of output descriptions ("
                  + _local_box->get_box_out()->size() + ")");
    }

    // Make sure there are no empty items in inputs or outputs
    if (find(_inputs.begin(), _inputs.end(), (Stream*)0) != _inputs.end())
    {    Throw(AuroraBadEntityException,
              "Not all input streams in \"" + get_name() + "\" have been set");
    }

    if (find(_outputs.begin(), _outputs.end(), (Stream*)0) != _outputs.end())
    {   Throw(AuroraBadEntityException,
              "Not all output streams in \"" + get_name() + "\" have been set");
    }

    // Set up _inq and tie to input streams
    for (unsigned int i = 0; i < _inputs.size(); ++i)
    {
        // Get the update_queue flag, type, and key from the catalog 
        // and create the queues accordingly.
        // -- tatbul
        //
        DEBUG << "Creating TupleQueue for box " << get_name() 
              << ", input port " << i;
        if (_local_box->is_update_queue(i))
        {
            DEBUG << "update_queue_option = true";
            DEBUG << "queue_type = " 
                  << _local_box->get_in_queue_type(i);
            DEBUG << "key_field = " 
                  << (_local_box->get_in_queue_key(i)).as_string();
        }
        else 
        {
            DEBUG << "update_queue_option = false";
        }
        ptr<TupleQueue> q(new TupleQueue(
                            pool,
                            _inputs[i]->get_tuple_description().get_size(),
                            _local_box->is_update_queue(i),
                            _local_box->get_in_queue_type(i),
                            (_local_box->get_in_queue_key(i)).as_string()));
        q->set_stream(_inputs[i]);

        // Add any state for this queue.
        DEBUG << "Attempting to unpack queue!";
        unpack_queue(*(_inputs[i]), q->enq_iterator());
        DEBUG << "Finished unpacking queue!";

        if ( _pending_queue_state )
        {
            _pending_queue_state.reset();
            DEBUG << "Reset pending state!";
        }

        _inq.push_back(q);

    //        NOTICE << "fix:  addDestPort";
        ///_inputs[i]->addDestPort(QBoxInputPort(this, i));
    }

    try
    {   init_impl();
        _state = INITED;
    }
    catch (exception&)
    {   _state = DEAD;
        throw;
    }

    DEBUG << "Attempting to unpack box state!";

    if ( _pending_box_state )
    {
        INFO << _pending_box_state;
        unpack_state(_pending_box_state);
        _pending_box_state.reset();
    }

    DEBUG << "Finished unpacking box state!";
}


void QBox::set_output(unsigned int i, Stream *desc)
{
    assert(_state == CONSTRUCTED || _state == SETUP);
    assert(i < 1000);

    if (_outputs.size() <= i)
    {   _outputs.resize(i + 1);
    }

    _outputs[i] = desc;

    //    NOTICE << "fix:  setSourcePort";
    //desc->setSourcePort(QBoxOutputPort(this, i));
}



ptr<LockSet> QBox::get_lock_set(string name)
{
    return _node->get_lock_set(name);
}



string QBox::get_revised_val( Tuple &t, CatalogSchema &s ) const
{
    const SchemaField *field = s.get_schema_field( s.index_of_field( "revised_value" ) );
    const char  *data = (char *)t.get_data() + field->get_offset();

    string str( data, field->get_size() );
    return( str );
}

void QBox::set_revised_val( Tuple &t, CatalogSchema &s, string data )
{
    const int i = s.index_of_field( "revised_value" );
    const SchemaField *field = s.get_schema_field( i );
    char  *revised = (char  *)t.get_data() + field->get_offset();

    memset( revised, '\0', field->get_size() );
    data.copy( revised, data.size() );
}


void QBox::set_ha_unit(ptr<HAUnit> ha_unit)
{
    _ha_unit = ha_unit;
}

ptr<HAUnit> QBox::get_ha_unit()
{
    return _ha_unit;
}

bool QBox::is_backup()
{
    return _backup;
}

void QBox::set_backup()
{
    _backup = true;
}

bool QBox::is_passive()
{
    return _passive;
}

void QBox::activate()
{
    _passive = false;
}

void QBox::deactivate()
{
    _passive = true;
}

double QBox::get_staleness() {
    if (_inq.size() == 0)
        return 0;
    double staleness = 0;
    for (unsigned int i = 0; i < _inq.size(); i++) {
        staleness += _inq[i]->get_staleness();
    }
    return staleness/_inq.size();
}

uint8  *QBox::retrieve_historical(uint8  *notification )
{
  uint8  *hist_compare = NULL;
  uint8  *hist;

  for( vector<ptr<QBox> >::iterator i = _sweeper_box.begin(); i != _sweeper_box.end(); ++i )
    {
      ptr<SweeperQBox> sweeper = (*i).static_cast_to<SweeperQBox>();
      hist = sweeper->notify(notification);

      if( hist == NULL )
    {
      sweeper->remove_request(notification);
    }
      else
    {
      if( hist_compare == NULL ) //keep the first one, though they all should be the same
        {
          hist_compare = hist;
        }
    }
    }
  return( hist_compare );
}



void QBox::send_to_sweeper(uint8  *data)
{
  vector<uint8 *> wrapper(1, data);
  DEBUG << _sweeper_box.size() << " sweepers for box " << get_name();
  
  if( Tuple::factory(data).get_tuple_type() == TupleType::REQUEST )
    {
      for( vector<ptr<QBox> >::iterator i = _sweeper_box.begin(); i != _sweeper_box.end(); ++i )
    {
      (*i).static_cast_to<SweeperQBox>()->request_queue()->enqueue_multiple(wrapper);
      (*i).static_cast_to<SweeperQBox>()->request_queue()->notify_enq();
      DEBUG << "Enqueued request into input of " << (*i)->get_name();
    }
    }
    else if( Tuple::factory(data).get_tuple_type() == TupleType::HISTORICAL )
    {
      for( vector<ptr<QBox> >::iterator i = _sweeper_box.begin(); i != _sweeper_box.end(); ++i )
    {
      (*i).static_cast_to<SweeperQBox>()->historical_queue()->enqueue_multiple(wrapper);
      (*i).static_cast_to<SweeperQBox>()->historical_queue()->notify_enq();
      DEBUG << "Enqueued historical into input of " << (*i)->get_name();
    }
    }
    else
    {
      DEBUG << "No support for sending TupleType::" << Tuple::factory(data).get_tuple_type().as_string() << " to sweepers";
    }
}

BOREALIS_NAMESPACE_END

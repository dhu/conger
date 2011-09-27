
#include <algorithm>

#include "Stream.h"
#include "QBox.h"
#include "AuroraNode.h"
#include "xercesDomUtil.h"
#include "Expression.h"
//#include "CPoint.h"
#include "CPQBox.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
Stream::Stream(string name, AuroraNode &node) throw( AuroraException ) :
    _node(node),
    _cpu_stats(node.get_cpu_stats()),
    _buf(0),
    _num_tuples_enqueued(0)
    //_cpoint(0)
{
//..............................................................................

    _local_stream = _node.get_local_stream( Name(name) );

    if ( !_local_stream ) // if the stream is not in the local catalog
    { 
        Throw( AuroraException,"Stream " + name + 
                               " is not in the local catalog." );
    }
    CatalogSchema* schema = _local_stream->get_stream_schema();
    if ( !schema ) // if the stream is not in the local catalog
    {
        Throw( AuroraException,"Stream " + name + 
                               " doesn't have its schema." );
    }

    DEBUG << "Stream:: " << *_local_stream;

    _desc = TupleDescription(schema);

    _buf = malloc( _desc.get_size() );

    // For now, let's assume that all tuple buffers are append-only queues. 
    // -- tatbul 
    //
    _tuple_buffer = new TupleQueue(node.get_page_pool(), _desc.get_size());
    _buffering = false;
}


Stream::~Stream()
{
    free(_buf);

    // I am afraid to do this yet, since there are some strange and
    // unexplained dependencies between things that use page pools
    // and page pools themselves when freeing memory.
    //  free( _tuple_buffer );
}

void Stream::add_queue(TupleQueue* q)
{
    _queues.push_back(q);
    //enqFromCPoint(q);
}

void Stream::remove_queue( TupleQueue  *q )
{
    // yna -- why would we remove all queues to the end?
    // I don't think we're keeping indexes to the vector
    //_queues.erase(remove(_queues.begin(), _queues.end(), q), _queues.end());
    remove(_queues.begin(), _queues.end(), q);
}

/***************************** fix this up???
void Stream::addDestPort( QBoxInputPort port )
{
    _dest_ports.push_back(port);
    //enqFromCPint(port.getBox()->getInputQueue(port.getPort()));
}

void Stream::removeDestPort( QBoxInputPort port )
{
    _dest_ports.erase( remove( _dest_ports.begin(),
                               _dest_ports.end(), port ),
                       _dest_ports.end() );
}

void Stream::setSourcePort( QBoxOutputPort  port )
{
    _source_port = port;
}
**********************************/

string Stream::as_string() const
{
    return "Stream{name=" + _local_stream->get_stream_name_string()
            + "; schema=" + _local_stream->get_schema_name()
            + "; " + _queues.size() + " dest queues and "
            + _local_stream->get_sink_port()->size() + " dest boxes}";
}

// we are not going to perform locking on this, because we make no
// guarantees regarding which tuples will be buffered and which packed.
void Stream::begin_buffering()
{
    DEBUG << " Stream " << get_name() << " is now buffering \n";
    _buffering = true;
}

// next time a tuple is enqueued, the contents of the tuple buffer will
// be dumped into the output.
void Stream::stop_buffering()
{
    DEBUG << " Stream " << get_name() << " has stopped buffering \n";
    _buffering = false;
}


void Stream::enqueue_tuple()
{
    //NOTICE << "Stream " << getName() << " enqueuing tuple: " << getTupleDescription().tupleToString(_buf);

    // not sure how buffering corresponds with enqueued tuples.
    // for now I'll allow counting once before tuple is buffered.
    ++_num_tuples_enqueued;

    if ( _buffering )
    {
        DEBUG << "Stream " << _local_stream->get_stream_name()
              << " buffering tuple: " 
              << get_tuple_description().tuple_to_string(_buf);

        TupleQueue::EnqIterator e = (_tuple_buffer)->enq_iterator();
        memcpy(e.tuple(), _buf, _desc.get_size());
        ++e;

        return;
    }
    else if ( _tuple_buffer->size() > 0 )
    {
        // dump queue contents into output (if any)
        TupleQueue::DeqIterator e = (_tuple_buffer)->deq_iterator();
        while ( e.avail() )
        {
            push_tuple( e.tuple() );
            ++e;

            DEBUG << " Dump tuples to all outputs ";
        }
    }

    /*
    for (vector<TupleQueue*>::iterator i = _queues.begin(); 
         i != _queues.end(); ++i)
    {
        DEBUG << " - to queue " << *i;
        TupleQueue::EnqIterator e = (*i)->enq_iterator();
        memcpy(e.tuple(), _buf, _desc.getSize());
        ++e;
    }

    for (vector<QBoxInputPort>::iterator i = _dest_ports.begin(); 
         i != _dest_ports.end(); ++i)
    {
        DEBUG << " - to " << i->getBox()->getName() << ", port " 
              << i->getPort();
        TupleQueue *q = i->getBox()->getInputQueue( i->getPort() );
        TupleQueue::EnqIterator e = q->enq_iterator();
        memcpy(e.tuple(), _buf, _desc.getSize());
        ++e;
    }
    */

    push_tuple( _buf ); //common expression elimination
}

void Stream::push_tuple( const void *_buf )
{

    for ( vector<TupleQueue*>::iterator i = _queues.begin();
          i != _queues.end(); ++i )
    {
      (*i)->_lock.lock(); // lock first
      DEBUG << " - to queue " << *i << " with TD " << _desc;
      TupleQueue::EnqIterator e = (*i)->enq_iterator();
      memcpy(e.tuple(), _buf, _desc.get_size());
      ++e;
      (*i)->_lock.unlock(); // done ... unlock
    }

    for (vector<BoxPort>::iterator i = _local_stream->get_sink_port()->begin();
         i != _local_stream->get_sink_port()->end(); ++i)
    {
        DEBUG << _local_stream->get_stream_name();
        CatalogBox* box = i->get_port_box();
        if (box)
        {
            DEBUG << " - to "  << box->get_box_name()
                  << ", port " << i->get_port() << ", bytes " 
                  << _desc.get_size();
            DEBUG << " Stream BYTES " << to_hex_string(_buf, _desc.get_size());

            ptr<QBox> sink_box = 
                            _node.get_box( box->get_box_name().as_string() );
            if ( sink_box )
            {
                if (sink_box->is_passive())
                {
//                    NOTICE << sink_box << " is passive: we skip this one";
                    continue;
                }
                TupleQueue *q = sink_box->get_input_queue( i->get_port() );
                if ( q )
                {
                    q->_lock.lock(); // lock first 
                                     // (CPView::replay() is in this queue)
                    TupleQueue::EnqIterator e = q->enq_iterator();
                    memcpy(e.tuple(), _buf, _desc.get_size());
                    ++e;
                    q->_lock.unlock();  // queue done ... unlock 

                } 
                else
                {
                    WARN << "Box " << box->get_box_name()
                         << " has no TupleQueue for port " << i->get_port();
                }
            } 
            else
            {
                WARN << "pushTuple failed: box " << box->get_box_name()
                     << " not found.";
            }
        } 
        else
        {
            WARN << "Invalid port " << i->get_port();
            break;
        }
    }

    // Only do CP when the CP is scheduled now
    /*
    if (_cp)
    {
      DEBUG << "Will enqueue in " << _cp;
      // memcopy the tuple
      char *tuple = new char[_desc.get_size()];
      memcpy(tuple, _buf, _desc.get_size());
      _cp->enqueue(tuple);
    }
    */
}

/*
void Stream::enqFromCPoint(TupleQueue *q)
{
    if (_cpoint)
    {
        _cpoint->enqueueInto(*q);
    }
} 
*/

void Stream::notify_enq()
{
    // yna -- we shouldn't attempt to run boxes while we're buffering.
    if ( !_buffering )
    {
        for ( vector<TupleQueue*>::iterator i = _queues.begin();
              i != _queues.end(); ++i )
        {
            (*i)->notify_enq();
        }

        for (vector<BoxPort>::iterator 
             i = _local_stream->get_sink_port()->begin();
             i != _local_stream->get_sink_port()->end(); ++i)
        {
            ptr<QBox> sink_box = _node.get_box(
                                i->get_port_box()->get_box_name().as_string());
            if ( sink_box )
            {
                sink_box->get_input_queue( i->get_port() )->notify_enq();
            }
            else
            {
                WARN << "Catalog/engine inconsistency: box "
                     << i->get_port_box()->get_box_name() << " not found.";
            }
        }
    }
}

string Stream::as_xml( schema_representation  rep ) const
{
    string out;

    out << "<stream name=\""
        << xml_escape( _local_stream->get_stream_name_string() ) << "\"";

    ///if ( !_desc.getName().empty() )
    if ( _local_stream->is_input_stream() )
    {   
        out << " schema=\"" << xml_escape( _local_stream->get_schema_name() )
            << "\"";
    }

    if ((rep == FULL_SCHEMA) ||
        ((rep == FULL_SCHEMA_IF_ANONYMOUS) && (_desc.get_name().empty())))
    {
        out << ">" << _desc.as_xml() << "</stream>";
    }
    else
    {   
        out << "/>";
    }

    return( out );
}

void Stream::set_cp(ptr<CPQBox> cp)
{
    //DEBUG << "Setting CP on Stream " << this->_local_stream->get_stream_name()
    //      << " with " << cp.as_string();

    _cp = cp;
}

ptr<CPQBox> Stream::get_cp()
{
    return _cp;
}

BOREALIS_NAMESPACE_END

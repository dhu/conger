#include <dlfcn.h>
#include <dirent.h>
#include <fstream>

#include "LockHolder.h"
#include "AuroraNode.h"
#include "CPQBox.h"
#include "CPView.h"
#include "NodeType.h"
#include "SweeperQBox.h"
#include "DataPath.h"
#include "DynamicArray.h"


BOREALIS_NAMESPACE_BEGIN

static const int AURORA_PAGES_MIN     = 100;
static const int AURORA_PAGES_DEFAULT = 4000;

static const int AURORA_PAGE_SIZE_MIN     = 1024;
static const int AURORA_PAGE_SIZE_DEFAULT = 4096;



////////////////////////////////////////////////////////////////////////////////
//
// Scheduler tasks.
//
void AuroraNode::modify_network_task::run()
{
//..............................................................................


    if ( _deleted_topology._boxes.size() > 0 )
    {
        _deleted_topology._schemas.clear();
        _deleted_topology._streams.clear();
        _deleted_topology._tables.clear();
        _deleted_topology._boxes.clear();

        // remove stats olga

        if (_box_ids.size()!=0)
        { // boxes
            _sm.delete_statistics("Cost", _box_ids);
            _node._scheduler->delete_scheduler_statistics("Cost", _box_ids);
            _sm.delete_statistics("InputRate", _box_ids);
            _node._scheduler->delete_scheduler_statistics("InputRate", _box_ids);
        }

        if (_instream_ids.size()!=0)
        { // input streams
            _sm.delete_statistics("QLength", _instream_ids);
            _node._scheduler->delete_scheduler_statistics("QLength",
                                                          _instream_ids);
        }

        // --tatbul
        if (_anystream_ids.size()!=0)
        { // any streams
            _sm.delete_statistics("StreamRate", _anystream_ids);
            _node._scheduler->delete_scheduler_statistics("StreamRate",
                                                          _anystream_ids);
        }
        // --tatbul

        if (_outstream_ids.size()!=0)
        { // output streams
            _sm.delete_statistics("TupleNum", _outstream_ids);
            _sm.delete_statistics("OutLatency", _outstream_ids);
            _sm.delete_statistics("OutLatencySqr", _outstream_ids);
        }

        if (_selectivity_ids.size()!=0)
        { //selectivities
            _sm.delete_statistics("Selectivity", _selectivity_ids);
            _node._scheduler->delete_scheduler_statistics("Selectivity", _selectivity_ids);
        }

        if (_stream_ids.size()!=0)
        {
            _sm.delete_statistics("StreamRate", _stream_ids);
        }

        return;
    }
    else
    {
        vector<string>  boxNames;

        INFO << "Running ModifyNetworkTask; new entities:\n"
             << indent(_new_topology.repr(), 4);

        _node._topology.copy_from( _new_topology );

        AuroraNode::BoxMap::iterator box_it = _new_topology._boxes.begin();
        AuroraNode::BoxMap::iterator box_end = _new_topology._boxes.end();

        for ( ; box_it != box_end; ++box_it )
        {
            CatalogBox* local_box = _node.get_local_box(Name(box_it->first));

            if ( local_box )
            {
                CatalogStream::StreamStar::iterator in_it = local_box->get_box_in()->begin();
                CatalogStream::StreamStar::iterator in_end = local_box->get_box_in()->end();

                for ( ; in_it != in_end;++in_it)
                {
                    AuroraNode::DeqContexts::iterator deq_ctxt_it = _node._deqs.begin();
                    AuroraNode::DeqContexts::iterator deq_ctxt_end = _node._deqs.end();
                    IDVect instreams;
                    IDVect anystreams; // -- tatbul

                    for (; deq_ctxt_it != deq_ctxt_end; ++deq_ctxt_it)
                    {
                        DEBUG << "Found input stream: " << (*in_it)->get_stream_name_string() ;

                        if (!(*in_it)->is_output_stream())
                        {   // -olga to avoid deleting input streams we subscribe to
                            if ((*deq_ctxt_it)->_outlatency_sum.find((*in_it)->get_stream_name_string())!= (*deq_ctxt_it)->_outlatency_sum.end())
                            {
                                (*deq_ctxt_it)->_outlatency_sum.erase((*in_it)->get_stream_name_string());
                                (*deq_ctxt_it)->_outlatency_sumsqr.erase((*in_it)->get_stream_name_string());
                                (*deq_ctxt_it)->_tuplenum.erase((*in_it)->get_stream_name_string());
                            }
                        }
                    }

                    if (!(*in_it)->is_output_stream())
                    {   // -olga to avoid deleting input streams we subscribe to
                        instreams.push_back( (*in_it)->get_stream_name_string());
                    }

                    anystreams.push_back((*in_it)->get_stream_name_string());
                    // -- tatbul

                    if (instreams.size()!=0)
                    {
                        _sm.delete_statistics("TupleNum", instreams);
                        _sm.delete_statistics("OutLatency", instreams);
                        _sm.delete_statistics("OutLatencySqr", instreams);
                    }
                }
            }
        }

        // Write locks should not be used! implemented in the addTSStats
        // boolean is just a hack, for the lack of removeTSStats at the moment.

        _node._monitor.modify_run_statistics(_node, _instream_ids,
                   _anystream_ids, _outstream_ids,  _selectivity_ids,
                   _stream_ids, _box_ids, _sm);

        INFO << "Resulting topology: " << _node._topology.repr();

        if ( _cond )
        {
            LockHolder hold(*_lock);
            // Init the boxes
            for (AuroraNode::BoxMap::const_iterator
                 i = _new_topology._boxes.begin();
                 i != _new_topology._boxes.end();
                 ++i)
            {
                i->second->init(*(_node._page_pool));

                boxNames.push_back(i->second->get_name());
            }

            // install box ports in the new catalog.
            _node.install_box_ports(_new_topology._boxes);
            _node.swap_box_ports(_port_swaps);

            //_node._scheduler->topologyChanged();
            _node._scheduler->update_listeners( boxNames, true );
            _cond->signal();
        }

        DEBUG << "~~~~~~~~~~~~~~~~~~ STARTING TO DO SWEEPER; TQ STUFF!";
        for (map<string, ptr<QBox> >::iterator
             s = _new_topology._boxes.begin();
             s != _new_topology._boxes.end(); ++s)
        {
            if (s->second->getType() == "sweeper")
            {
                ptr<SweeperQBox> sweeper =
                            s->second.static_cast_to<SweeperQBox>();
                set<ptr<QBox> > qboxes = _node.get_boxes();
                for (set<ptr<QBox> >::iterator b = qboxes.begin();
                     b != qboxes.end(); ++b )
                {
                    // Something different for CPQBox?
                    if (sweeper->get_name() == (*b)->get_name() + "sweeper"
                        && (*b)->getType() == "connectionpoint")
                    {
                        ptr<CPQBox> cpq = (*b).static_cast_to<CPQBox>();
                        cpq->add_sweeper_view(string(),
                                              sweeper->historical_queue());
                        DEBUG << " CP " << (*b)->get_name()
                              << " got sweeper " << sweeper->get_name();
                        continue;
                    }

                    vector<string> anchors = (*b)->get_anchors();
                    for (vector<string>::iterator a = anchors.begin();
                         a != anchors.end(); ++a )
                    {
                        DEBUG << (*b)->get_name() << " -> " << *a << " ? "
                              << s->second->get_name();
                        if (strcasecmp(s->second->get_name().c_str(),
                                       ((*a)+"sweeper").c_str()) == 0)
                        {
                            (*b)->add_sweeper(sweeper);
                            DEBUG << " box " << (*b)->get_name()
                                  << " got sweeper " << sweeper->get_name();
                            sweeper->set_output_queue((*b)->get_id(),
                                        (*b)->get_input_tuple_queue(0));
                            // break;
                        }
                    }
                }
            }
        }

        DEBUG << "~~~~~~~~~~~~~~~~~~FINISHED DOING SWEEPER TQ STUFF";

        INFO << "Done running ModifyNetworkTask; new entities:\n";
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::subscribe_task::run()
{
//..............................................................................


    if ( _stream && _q )
    {
        DEBUG << "Running SubscribeTask - adding queue to " << _stream->get_name();
        _stream->add_queue(_q.get());
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::unsubscribe_task::run()
{
//..............................................................................


    // Check if stream still exists, to avoid race condition between unsubscribing and delete stream.
    ptr<Stream> _stream = _node->get_stream(_stream_name);

    if ( _stream )
    {
        DEBUG << "Running UnsubscribeTask - removing queue from "
              << _stream->get_name();
        _stream->remove_queue( _q );
        DEBUG << "Running UnsubscribeTask - removed queue!";
    }
    else
    {
        DEBUG << "Running UnsubscribeTask - stream " << _stream_name
              << " no longer exists, ignoring unsubscribe!";
    }

    _node->_monitor.unsubscribe_run_statistics((void *)_context.get(),
                                               _node,
                                               _stream_name);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::unsubscribe_all_task::run()
{
//..............................................................................


    DEBUG << "Running UnsubscribeAllTask";

    typedef map<Stream*, AuroraNode::Subscription> Subscriptions;
    Subscriptions& subs = _context->_subscriptions;

    for (Subscriptions::iterator i = subs.begin(); i != subs.end(); ++i)
    {
        DEBUG << " - unsubscribing from " << i->first->get_name();
        i->first->remove_queue( i->second._q.get() );
    }

    // Clear subscriptions, destroying TupleQueues
    subs.clear();

    return;
}


// End scheduler tasks.
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// Engine's network representation helpers.
bool AuroraNode::Topology::contains_entity( string  name ) const
{
//..............................................................................


    return  _streams.count(name) ||
            _boxes.count(name)   ||
            _schemas.count(name) ||
            _tables.count(name);
}



////////////////////////////////////////////////////////////////////////////////
//
// Copy a node's Topology to another.
//
void AuroraNode::Topology::copy_from( const Topology  &other )
{
//..............................................................................


    copy( other._streams.begin(), other._streams.end(),
          inserter(_streams, _streams.begin()));

    copy( other._boxes.begin(), other._boxes.end(),
          inserter(_boxes, _boxes.begin()));

    copy( other._schemas.begin(), other._schemas.end(),
          inserter(_schemas, _schemas.begin()));

    _tables.copy_from( other._tables );
}



////////////////////////////////////////////////////////////////////////////////
//
string AuroraNode::Topology::repr() const
{
    string  out;
//..............................................................................


    out << "TupleDescriptions {\n";

    for ( SchemaMap::const_iterator  i  = _schemas.begin();
                                     i != _schemas.end(); ++i )
    {
        out << "    " << i->second << "\n";
    }

    out << "}\n";
    out << "Streams {\n";

    for ( StreamMap::const_iterator  i  = _streams.begin();
                                     i != _streams.end(); ++i )
    {
        out << "    " << *(i->second) << "\n";
    }

    out << "}\n";

    _tables.repr( out );

    out << "Boxes {\n";

    for ( BoxMap::const_iterator i = _boxes.begin(); i != _boxes.end(); ++i )
    {   out << "    " << *(i->second) << "\n";
    }

    out << "}\n";

    return( out );
}



////////////////////////////////////////////////////////////////////////////////
//
// Stream Processing Engine.
//
AuroraNode::AuroraNode(Diagram  &local_catalog, string id)
                 throw(AuroraException) :
    _id(id),
    _state(STATE_INIT),
    _local_catalog(local_catalog),
    _node_table(_topology._tables),      // Link node references to
    _streams(_topology._streams),        // elements in the node's Topology.
    _tables(_topology._tables),
    _boxes(_topology._boxes),
    _schemas(_topology._schemas)
{
        const char  *num_pages_str;
        const char  *page_size_str;

        int32        num_pages = AURORA_PAGES_DEFAULT;
        int32        page_size = AURORA_PAGE_SIZE_DEFAULT;
//..............................................................................


    INFO << "Force PseudoScheduler";
    _scheduler = ptr<Scheduler>(&scheduler);

    _scheduler->_node = this;
    num_pages_str = getenv( "AURORA_PAGES" );
    page_size_str = getenv( "AURORA_PAGE_SIZE" );

    if ( num_pages_str )
    {
        if ( !to_scalar( num_pages_str, num_pages ))
        {
            WARN << "Invalid number of pages; using " << AURORA_PAGES_DEFAULT;
            num_pages = AURORA_PAGES_DEFAULT;
        }
        else
        {
            if ( num_pages < AURORA_PAGES_MIN )
            {
                WARN << "Number of pages is too small; using "
                     << AURORA_PAGES_MIN;

                num_pages = AURORA_PAGES_MIN;
            }
        }
    }

    if ( page_size_str )
    {
        if ( !to_scalar( page_size_str, page_size ))
        {
            WARN << "Invalid page size; using " << AURORA_PAGE_SIZE_DEFAULT;
            page_size = AURORA_PAGE_SIZE_DEFAULT;
        }
        else
        {
            uint32  ps = page_size;

            while ( !( ps & 1 ))
            {   ps = ps >> 1;
            }

            if ( ps != 1 )
            {
                WARN << "Page size is not a power of two; using "
                     << AURORA_PAGE_SIZE_DEFAULT;

                page_size = AURORA_PAGE_SIZE_DEFAULT;
            }
            else
            {
                if ( page_size < AURORA_PAGE_SIZE_MIN )
                {
                    WARN  << "Page size is too small; using "
                          << AURORA_PAGE_SIZE_MIN;

                    page_size = AURORA_PAGE_SIZE_MIN;
                }
            }
        }
    }

    INFO  << "Creating PagePool with " << num_pages
          << " pages of size " << page_size;

    _page_pool.set( new PagePool( page_size, num_pages, false ));

    _monitor.node_statistics();
}



////////////////////////////////////////////////////////////////////////////////
//
AuroraNode::~AuroraNode()
{
//..............................................................................


    if ( _state == STATE_RUNNING )
    {
        INFO << "Destroyed AuroraNode without stopping it first!";
        shutdown();
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Engine execution control.
//
void AuroraNode::start() throw (AuroraException)
{
//..............................................................................


    if (_state != STATE_INIT)
    {   Throw(AuroraException,
              "Attempted to start a network that has already been started");
    }

    _state = STATE_RUNNING;
    INFO << "Starting Aurora network";

    // Init all the QBoxes
    for (BoxMap::const_iterator i = _boxes.begin(); i != _boxes.end(); ++i)
    {
        QBox *box = i->second.get();
        DEBUG << "INITING " << box->get_name();
        box->init(*_page_pool);
    }

    if (getenv("BOREALIS_LOGGING") != 0)
    {
        char* iteration = getenv("BOREALIS_CASE_ID");
        string file_name = _id;
        if (iteration != 0)
            (file_name += ".") += iteration;
        file_name += ".cpu.log";
        NOTICE << "From now on CPU utilization will be text-dumped to file "
               << file_name;
        _cpu_stats.set(ptr<ofstream> (new ofstream(file_name.c_str())));
    }

    // Start the CPU stats thread
    _cpu_stats.start();

    _scheduler->start();
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::shutdown() throw (AuroraException)
{
//..............................................................................


    if (_state != STATE_RUNNING)
    {
        Throw(AuroraException,
              "Attempted to stop a network that is not running");
    }

    _state = STATE_STOPPED;
    _cpu_stats.terminate();

    NOTICE << "AuroraNode::shutdown";

    _monitor.finish(); // -- tatbul

    {
        LockHolder hold(_lock);

        // Close mailboxes and schedule unsubscription
        // for any outstanding contexts
        //
        for (DeqContexts::iterator i = _deqs.begin(); i != _deqs.end(); ++i)
        {
            (*i)->_mailbox.close();

            // Schedule unsubscription of all streams/queues
            ptr<unsubscribe_all_task> task(new unsubscribe_all_task);
            task->_context = *i;
            schedule(task);
        }
    }

    _scheduler->shutdown();

    _cpu_stats.join();

    _node_table.shutdown();     // Flush any cached table information.
}



////////////////////////////////////////////////////////////////////////////////
//
// Task execution control.
//
void AuroraNode::schedule(ptr<SchedulerTask> task)
{
//..............................................................................


    if (_state == AuroraNode::STATE_INIT)
    {   task->run();
    }
    else
    {   _scheduler->schedule_exclusive_task(task);
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Query Network execution control.
//
void AuroraNode::drain() throw (AuroraException)
{
//..............................................................................


    if (_state != STATE_RUNNING)
    {
        Throw(AuroraException, "Only a running network can be drained");
    }

    if (!_scheduler->has_drain())
    {
        Throw(AuroraException, "Scheduler doesn't support draining");
    }

    _scheduler->drain();

    // Close all mailboxes
    {
        LockHolder hold(_lock);

        for (DeqContexts::iterator i = _deqs.begin(); i != _deqs.end(); ++i)
        {
            (*i)->_mailbox.close();
        }
    }
}


void AuroraNode::choke_inputs(vector<string> input_names)
{
    _scheduler->choke_inputs(input_names);
}


void AuroraNode::resume_inputs(vector<string> input_names)
{
    _scheduler->resume_inputs(input_names);
}


void AuroraNode::drain_boxes(vector<string> names)
{
    _scheduler->drain_sub_network(names);
}


void AuroraNode::choke_boxes( vector<string>  names )
{
    _scheduler->choke_sub_network( names );
}


void AuroraNode::suspend_boxes( vector<string>  names )
{
    _scheduler->suspend_sub_network( names );
}


void AuroraNode::resume_boxes( vector<string>  names )
{
    _scheduler->resume_sub_network( names );
}


void AuroraNode::resume_box_streams( vector<string>  names )
{
    _scheduler->resume_box_streams( names );
}



////////////////////////////////////////////////////////////////////////////////
//
// Enqueue input tuples to a stream.  The input stream must not be tied to
// the output of any box in the network.
//
void AuroraNode::enqueue(// Name of the input stream to which to enqueue.
                         const char  *stream_name,

                         // A Batch of tuple data including headers.
                         const void  *data,

                         // Array data ordered as referenced in the tuples.
                         const void  *array,

                         // Size of each tuple including the header.
                         size_t       tuple_length,

                         // Number of input tuples sent.
                         size_t       num_tuples
                        )
                   throw(AuroraException)
{
//..............................................................................


    if (_state != STATE_RUNNING)
    {   Throw(AuroraException,
              "Attempted to enqueue on a network that is not running");
    }

    DEBUG << "AuroraNode::enqueue: " << num_tuples << " tuples on " << stream_name;
    Stream  *stream = lookup(_streams, stream_name).get();

    if (!stream)
    {
        DEBUG << "Cannot enqueue to undefined stream \""
              << string(stream_name) << "\"";

        Throw(aurora_no_such_entity_exception,
              "Cannot enqueue to undefined stream \"" + string(stream_name) + "\"");
    }

    if (stream->get_source_port())
    {
        DEBUG << "Cannot enqueue to non-input stream \"" << string(stream_name) << "\" (it has an upstream box)";

        Throw(AuroraException,
              "Cannot enqueue to non-input stream \"" + string(stream_name)
                  + "\" (it has an upstream box)");
    }

    if (tuple_length != stream->get_tuple_description().get_size())
    {
        DEBUG << "Wrong tuple size on enqueue to " + string(stream_name) << " ("
              << tuple_length << "; should be "
              << stream->get_tuple_description().get_size() << ")";

        Throw(AuroraException,
              "Wrong tuple size on enqueue to " + string(stream_name)
                + " (" + tuple_length + "; should be "
                + stream->get_tuple_description().get_size() + ")");
    }

    // XXX: potential concurrency issue: shouldn't enqueue on a Stream
    // if an exclusive task is running!  Add runNonexclusively to
    // Scheduler API to facilitate this?

    Stream::EnqIterator enq = stream->enq_iterator();

    DEBUG << "Got EnqIterator to enqueue " << num_tuples << " tuples.";

    for (size_t  i = 0; i < num_tuples; ++i)
    {
        //DEBUG << "copy tuple_length=" << tuple_length;
        memcpy(enq.tuple(), data, tuple_length);

        //DEBUG << " * #" << i << ": " << stream->get_tuple_description().tuple_to_string(enq.tuple());

        // If the tuple has an array, allocate it, set the array flag in the
        // header, and convert the array size to an array reference.
        //
        if (array)
        {   array = array_allocator(stream, enq.tuple(), array);
        }

        // set global timestamp, if zero
        //
        // olga + nesime
        //add global timestamp (the first node)
        timeval now;
        gettimeofday(&now, 0);

        //DEBUG  << "set timestamps ...";

        Tuple  tuple = Tuple::factory(enq.tuple());

        if (tuple.get_timestamp() == Timestamp())
        {
            tuple.set_timestamp((Timestamp)now);
        }

        // set local timestamp at this node
        //
        tuple.set_local_timestamp((Timestamp)now);

        DEBUG << " - #" << i << ": " << stream->get_tuple_description().tuple_to_string(enq.tuple());

        _monitor.enqueue_case_specific((char *)enq.tuple(), stream, now);

        ++enq;
        data = ((const char *)data) + tuple_length;
    }

    /// Calls the notifyEnq() method for each TupleQueue attached to
    /// this stream (including TupleQueues belonging to box's input
    /// ports).  This notifies any listeners on those TupleQueues that
    /// tuples may be ready for consumption.
    ///
    DEBUG << "notify_enq()";
    stream->notify_enq();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Allocate any arrays in a tuple.  Return value is after the array data.
//
const void  *AuroraNode::array_allocator(
                    // Stream for the tuple.
                    //
                    Stream      *stream,

                    // Enqueued tuple data including the header.
                    // Upon exit the array flag is set in the header and
                    // the array size in the tuple is changed to a reference.
                    //
                    void        *tuple,

                    // Array data ordered as referenced in the tuple.
                    //
                    const void  *array)
{
    CatalogStream  *catalog_stream;
    CatalogSchema  *schema;
    const void     *next;
    DynamicArray   *header;

    uint16    offset;
    uint32    size;
    uint32    head;
    uint32   *element;

    vector<SchemaField>::iterator  f;
//..............................................................................


    DEBUG << "Allocate arrays ...";

    // Set the array flag in the tuple header.
    Tuple  t = Tuple::factory(tuple);
    t.set_array_flag(true);

    //DEBUG << "Allocate arrays setup ...";

    catalog_stream = stream->get_local_stream();
    schema = catalog_stream->get_stream_schema();
    next   = array;

    vector<SchemaField>  &field = schema->get_schema_field();

    for (f = field.begin(); f != field.end(); f++)
    {   if (f->get_type() == DataType::ARRAY)
        {
            // Extract the size of the array data from the tuple.
            //
            offset  = (uint32)f->get_offset();
            element = (uint32 *)((uint8 *)tuple + offset);
            size    = *element;
            head    = DynamicArray::header_size();

            DEBUG << "Array field offset=" << offset << " size=" << size
                  << " header size=" << head;

            // Allocate space for the dynamic array header and data.
            //
            header = DynamicArray::allocate_array(size);
            header->set_boundary(size);

            // Fill in the dynamic array header and copy the array data
            // from the input buffer to the allocated array.
            // NYI:  Dynamic array sizes
            //
            memcpy((void *)((uint8 *)header + head), next, size);


            // Establish the reference count and the width of the leftmost
            // dimension.  For static arrays the width will be 1.
            // For dynamic arrays it will be 1 or more.
            // Something is wrong if the size does not divide evenly.
            //
            //DEBUG << "static size = " << f->get_array()->array_size();

            header->set_count_size(catalog_stream->get_sink_port()->size(),
                                   size / f->get_array()->array_size());

            // Reference the dynamic array header from the tuple.
            //
            *element = (uint32)header;

            next = (void *)&(((uint8 *)next)[size]);
            //DEBUG << "next @ " << next << "  array @ " << array;
        }
    }

    return  next;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dequeue tuples from any subscribed output stream.  This method blocks until
//
//  - at least one tuple is available on any subscribed stream, or
//  - close() is called, or
//  - the node is shut down.
//
// Sets stream_name to the name of stream and td to the description
// of the tuples that have been dequeued.  Up to buffer_size bytes
// are written to the buffer (the actual number of bytes written
// can be calculated as <i>return-value</i> * td.getSize()).
//
//  Returns the number of tuples dequeued or
//  0 if close() was called or the node has been shut down.
//
size_t AuroraNode::DequeueHandle::dequeue(// Out - Name of the output stream.
                                          string            &stream_name,

                                          // Out - Set to the stream schema.
                                          TupleDescription  &td,

                                          // Out - At the tuple data.
                                          void              *buffer,

                                          // Maximum bytes in the buffer.
                                          size_t             buffer_size)
                                    throw(AuroraException)
{
    bool            output_stream;
    CatalogStream  *catalog_stream;
    Stream         *stream;
    TupleQueue     *q;
    size_t          count;
    size_t          tuple_size;
    size_t          max_tuples;

    TupleQueue::DeqIterator  deq;
//..............................................................................


    assert(_node);
    assert(_context);

    DEBUG << "AuroraNode::DequeueHandle::dequeue";

    while (1)
    {   stream_name.clear();
        td = TupleDescription();

        if (!_context->_mailbox.get_event(q))
        {
            return  0;
        }

        stream = q->get_stream();

        if (!stream)
        {   FATAL << "Dequeued on a TupleQueue whose stream has not been set!";
        }

        stream_name = stream->get_name();
        td = stream->get_tuple_description();

        //olga check if the stream is an output stream
        output_stream  = false;
        catalog_stream = _node->get_local_stream(stream_name);

        if (catalog_stream->is_output_stream())
        {
            DEBUG << "We dequeue from an output stream : " <<  stream_name;
            output_stream = true;
        }

        DEBUG << " - going to dequeue on " << stream_name << " TD " << td;

        tuple_size = td.get_size();
        max_tuples = buffer_size / tuple_size;

        if (max_tuples == 0)
        {   Throw(AuroraException,
                  "Buffer not big enough to dequeue even a single tuple");
        }

        deq   = q->deq_iterator();
        count = 0;

        while (count < max_tuples  &&  deq.avail())
        {
            if (output_stream) // Hack for HA
            {
                // hard coded offset???
                //in_time.tv_sec  = ((timeval*)deq.tuple())->tv_sec;
                //in_time.tv_usec = ((timeval*)deq.tuple())->tv_usec;
                //handle(Time(in_time));

                Tuple  tuple = Tuple::factory(deq.tuple());
                handle(tuple.get_timestamp());
            }

            _node->_monitor.dequeue_statistics((void  *)this, deq.tuple(), td,
                                               stream_name, output_stream);

            DEBUG << " - got one: \""
                  << to_escaped_string(deq.tuple(), tuple_size) << "\"";

            memcpy(buffer, deq.tuple(), tuple_size);
            buffer = ((char*)buffer) + tuple_size;
            ++deq;
            ++count;
        }

        if (deq.avail())
        {
            // Post another event, so I'll keep dequeueing next time around
            DEBUG << " - not done... will keep dequeueing";
            _context->_mailbox.on_non_empty(q);
        }

        _node->_monitor.dequeue_output_statistics3(_node,
                                                   (void *)_context.get(),
                                                   q, output_stream);

        if (count)
        {   DEBUG << " Returned # tuples: " << count;
            return  count;
        }

        DEBUG << " - aww... nothin'.  Looping";
    }
}



////////////////////////////////////////////////////////////////////////////////
// Dequeue handle control.
//
AuroraNode::DequeueHandle AuroraNode::get_dequeue_handle()
{
//..............................................................................


    LockHolder hold(_lock);

    DequeueHandle handle;
    handle._node = this;
    handle._context.set( new DequeueContext );

    // olga initialize timer for statistics
    handle._context->_last_outlatency_update=Scheduler::ticks();
    _deqs.insert(handle._context);

    return handle;
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::DequeueHandle::close()
{
//..............................................................................


    assert(_node);
    assert(_context);

    LockHolder hold(_node->_lock);

    // No more context!  Won't get erased until this DequeueHandle
    // gets destroyed, though
    if (_node->_deqs.erase(_context) == 0)
    {
        if (_node->_state == STATE_RUNNING)
        {
            // Only warn about this if the network is running (not
            // being shut down)
            WARN << "Closed DequeueHandle twice!";
        }

        return;
    }

    // Close mailbox
    _context->_mailbox.close();

    // Schedule unsubscription of all streams/queues
    ptr<unsubscribe_all_task> task( new unsubscribe_all_task );
    task->_context = _context;
    _node->schedule(task);
}

////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::DequeueHandle::subscribe(string stream_name) throw (AuroraException)
{
//..............................................................................


    assert(_node);
    assert(_context);

    INFO << "Subscribing to " << stream_name;

    LockHolder hold(_node->_lock);

    ptr<Stream> pstream = lookup(_node->_streams, stream_name);
    if (!pstream)
        Throw(aurora_no_such_entity_exception,
              "Cannot subscribe to undefined stream \"" + stream_name + "\"");
    Stream* stream = pstream.get();

    if (_context->_subscriptions.count(stream))
    {
        // Already subscribed
        return;
    }

    // Create the TupleQueue
    Subscription sub;

    sub._q.set( new TupleQueue(*(_node->_page_pool),
                stream->get_tuple_description().get_size()) );

    _context->_subscriptions[stream] = sub;
    sub._q->set_stream(stream);


    _node->_monitor.dequeue_subscribe_statistics(_node,
                                        (void  *)_context.get(), stream_name);

    // Add the dequeue mailbox
    sub._q->set_enq_notify_target(&_context->_mailbox);

    INFO << "Scheduling SubscribeTask";

    ptr<subscribe_task> task(new subscribe_task);
    task->_stream = pstream;
    task->_q = sub._q;

    _node->schedule(task);
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::DequeueHandle::unsubscribe(string stream_name) throw (AuroraException)
{
//..............................................................................


    assert(_node);
    assert(_context);

    INFO << "Unsubscribing from " << stream_name;

    LockHolder hold(_node->_lock);

    //Stream *stream = lookup(_node->_streams, stream_name).get();
    ptr<Stream> pstream = lookup(_node->_streams, stream_name);
    if (!pstream)
        Throw(aurora_no_such_entity_exception,
              "Cannot subscribe to undefined stream \"" + stream_name + "\"");

    map<Stream*, Subscription>::iterator sub = _context->_subscriptions.find(pstream.get());

    if (sub == _context->_subscriptions.end())
    {
        DEBUG << " - not subscribed";
        return;
    }

    INFO << "Scheduling UnsubscribeTask";
    ptr<unsubscribe_task> task(new unsubscribe_task);
    task->_node = _node;
    task->_stream_name = stream_name;
    task->_context = _context;
    task->_q = sub->second._q.get();

    // Remove the subscription.  We can't actually delete the queue
    // until some point in the future, because (a) something may
    // enqueue more tuples on it, and (b) a mailbox may be hanging
    // onto it, in which case dequeue() may still see it; currently we
    // just keep the queue alive until the DequeueContext dies (ick).
    //
    _context->_deleted_queues.push_back(sub->second._q);
    _context->_subscriptions.erase(sub);

    // Schedule removal from the Stream (can't do this right now
    // because something may enqueue more tuples on it; need to
    // run an exclusive task)
    //
    _node->schedule(task);
}




////////////////////////////////////////////////////////////////////////////////
//
// Query Network additions
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
// Main handler to add schemas, streams and boxes to engine's local
// representation through a scheduler task. These elements must
// already be added to the catalog.
//
ptr<AuroraNode::modify_network_task> AuroraNode::create_add_task(
                                            TopologyModifications&  additions,
                                                       BoxPackage&  packed_boxes
    )
    throw( AuroraException )
{
    IDVect ids;
    IDVect instream_ids;
    IDVect anystream_ids;
    IDVect selectivity_ids;

    vector<IDVect> outstream_ids;
    vector<IDVect> new_stream_ids;
//..............................................................................


    ptr<modify_network_task> task( new modify_network_task(*this, sm) );

    vector<CatalogSchema*>& schemas = additions.schemas;
    vector<CatalogStream*>& streams = additions.streams;
    vector<CatalogBox*>& boxes = additions.boxes;
    vector<CatalogTable*>& tables = additions.tables;


    // Set up new schemas.
    vector<CatalogSchema*>::iterator schema_it = schemas.begin();
    vector<CatalogSchema*>::iterator schema_end = schemas.end();

    for ( ; schema_it != schema_end; ++schema_it)
    {
        CatalogSchema* schema = *schema_it;
        string schema_name = schema->get_schema_name();

        TupleDescription desc = lookup(_schemas, schema_name);
        if (!desc) desc = lookup(task->_new_topology._schemas, schema_name);

        if (!desc || !desc.get_schema())
        {
            TupleDescription desc(schema);
            task->_new_topology._schemas[schema_name] = desc;
            DEBUG << "Added schema " << desc;
        }
    }


    // Set up new streams.
    vector<CatalogStream*>::iterator stream_it = streams.begin();
    vector<CatalogStream*>::iterator stream_end = streams.end();

    for ( ; stream_it != stream_end; ++stream_it)
    {
        CatalogStream* c_stream = *stream_it;
        CatalogSchema* c_schema = c_stream->get_stream_schema();

        string stream_name = c_stream->get_stream_name_string();
        string schema_name = c_schema->get_schema_name();

        DEBUG << "Trying to add stream " << stream_name
              << " with schema " << schema_name;

        // Look for the schema in both existing schemas and schemas to be added.

        TupleDescription desc = lookup(_schemas, schema_name);
        if (!desc) desc = lookup(task->_new_topology._schemas, schema_name);

        if (!desc || !desc.get_schema())
        {
            DEBUG << "Failed to find schema " << schema_name;

            Throw(AuroraBadEntityException,
                  "Stream \"" + stream_name +
                  "\" uses undefined schema \"" + schema_name + "\"");
        }

        ptr<Stream> stream( new Stream(stream_name, *this) );
        task->_new_topology._streams[stream_name] = stream;

        // Record stats for new stream.
        IDVect v;
        v.push_back(stream_name);
        new_stream_ids.push_back(v);
        outstream_ids.push_back(v);

        DEBUG << "Added stream " << stream_name;
    }


    // Set up new tables.
    vector<CatalogTable*>::iterator table_it = tables.begin();
    vector<CatalogTable*>::iterator table_end = tables.end();

    for ( ; table_it != table_end; ++table_it)
    {
        CatalogTable* table = *table_it;
        string table_name = to_string(table->get_table_name());
        string schema_name =
            to_string(table->get_table_schema()->get_schema_name());

        // Look for the schema in both engine's schemas and new schemas.
        TupleDescription desc = lookup( _schemas, schema_name );

        if ( !desc )
        {   desc = lookup( task->_new_topology._schemas, schema_name );
        }

        if ( !desc )
        {   Throw( AuroraBadEntityException,
                   "Table \"" + table_name +
                   "\" uses undefined schema \"" + schema_name + "\"" );
        }

        if ( !_node_table.has_table( table_name, _tables )
             && !_node_table.has_table(table_name, task->_new_topology._tables))
        {
            DEBUG << "Adding table " << table_name;
            _node_table.add_table(*this, table, task->_new_topology._tables );
        }
    }


    // Set up new boxes.
    vector<CatalogBox*>::iterator box_it = boxes.begin();
    vector<CatalogBox*>::iterator box_end = boxes.end();

    for ( ; box_it != box_end; ++box_it )
    {
        string box_type = (*box_it)->get_box_type();
        string box_name = (*box_it)->get_box_name().as_string();

        // Initialize QBox
        ptr<QBox> box( QBox::instantiate( box_type ));


        // Look for the table in both engine's tables and new tables.
        if ( (*box_it)->get_box_table() )
        {
            CatalogTable* table = (*box_it)->get_box_table();
            string table_name = to_string(table->get_table_name());

            if ( !_node_table.has_table( table_name, _tables )
                 && !_node_table.has_table( table_name, task->_new_topology._tables ) )
            {   Throw( AuroraBadEntityException,
                       "Box \"" + box_name +
                       "\" uses undefined table \"" + table_name + "\"" );
            }

            if ( _node_table.has_table(table_name, _tables) )
            {
                _node_table.set_table(table_name, box, _tables);
            }
            else
            {
                _node_table.set_table(table_name, box, task->_new_topology._tables);
            }
        }


        // Iterate adding inputs
        CatalogStream::StreamStar::iterator input_it  = (*box_it)->get_box_in()->begin();
        CatalogStream::StreamStar::iterator input_end = (*box_it)->get_box_in()->end();

        for ( uint16 in_port = 0; input_it != input_end ; ++input_it, ++in_port )
        {
            string input_name = (*input_it)->get_stream_name().as_string();
            ptr<Stream> stream = get_stream( input_name );

            if (!stream)
                stream = lookup(task->_new_topology._streams, input_name);

            // Add the stream if necessary.
            if (!stream)
            {
                stream = ptr<Stream>( new Stream(input_name, *this) );
                task->_new_topology._streams[ input_name ] = stream;

                IDVect stream_id;
                stream_id.push_back( input_name );
                new_stream_ids.push_back( stream_id );
            }

            box->set_input( in_port, stream.get() );

            // Record ids for collecting stats.
            //
            _monitor.create_add_statistics(box_name, input_name, in_port,
                             instream_ids, anystream_ids, selectivity_ids);
        }

        // box->setup is in two places... so this line too
        _box_id_by_name.insert(pair<Name, int>(Name(box_name), _box_id_by_name.size()));
        // Setup the box
        box->setup( *this, (*box_it) );


        // Iterate over outputs, creating streas
        CatalogStream::StreamStar::iterator output_it  = (*box_it)->get_box_out()->begin();
        CatalogStream::StreamStar::iterator output_end = (*box_it)->get_box_out()->end();

        for ( uint16 out_port = 0; output_it != output_end; ++output_it, ++out_port)
        {
            ptr<Stream> out_stream;
            string output_name = (*output_it)->get_stream_name().as_string();

            out_stream = get_stream(output_name);

            if (!out_stream)
            {
                out_stream = lookup(task->_new_topology._streams, output_name);
            }

            if (!out_stream)
            {
                out_stream.set( new Stream( output_name, *this ));
                task->_new_topology._streams[ output_name ] = out_stream;
            }

            box->set_output( out_port, out_stream.get() );

            _monitor.create_add_statistics5(box_name, (*output_it), out_port,
                                            outstream_ids, selectivity_ids);
        }


        // Find and set up any state.
        ptr< pair< ptr<AbstractQueueState>, ptr<AbstractBoxState> > > box_state
             = packed_boxes.find_box( box_name );

        if ( box_state )
        {
            DEBUG << "Found state for box " << box_name;
            ptr<QueueState> packed_queues =
                box_state->first.dynamic_cast_to<QueueState>();
            box->set_pending_queue_state(packed_queues);

            ptr<AbstractBoxState> packed_box = box_state->second;
            box->set_pending_box_state(packed_box);
        }

        task->_new_topology._boxes[ box_name ] = box;

        ids.push_back(box_name);
    }

    _monitor.create_add_statistics6(ids, instream_ids, anystream_ids,
                 selectivity_ids, outstream_ids, new_stream_ids, sm);

    return  task;
}



////////////////////////////////////////////////////////////////////////////////
//
// installBoxPorts
// -- moved from QueryProcessor due to asynchronous SchedulerTask.
void AuroraNode::install_box_ports(BoxMap& boxes)
{
//..............................................................................


    BoxMap::iterator box_it = boxes.begin();
    BoxMap::iterator box_end = boxes.end();

    for ( ; box_it != box_end; ++box_it )
    {
        CatalogBox* local_box = get_local_box(Name(box_it->first));

        if ( local_box )
        {
            CatalogStream::StreamStar::iterator in_it =
                local_box->get_box_in()->begin();

            CatalogStream::StreamStar::iterator in_end =
                local_box->get_box_in()->end();

            for ( ; in_it != in_end;++in_it)
            {
                BoxPort port;

                port.set_box_port(local_box,
                                  in_it - local_box->get_box_in()->begin());

                (*in_it)->add_sink_port(port);
            }

            CatalogStream::StreamStar::iterator out_it =
                local_box->get_box_out()->begin();
            CatalogStream::StreamStar::iterator out_end =
                local_box->get_box_out()->end();

            for (; out_it != out_end; ++out_it)
            {
                uint16 port = out_it - local_box->get_box_out()->begin();
                (*out_it)->set_stream_source(local_box, port);
            }
        }
        else
        {
            WARN << "Box " << box_it->first << " not found in the catalog.";
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Uninstall box ports
// -- assumes boxes are still in the catalog.
void AuroraNode::uninstall_box_ports(vector<CatalogBox*> boxes)
{
//..............................................................................


    // First choke the boxes so the enqueue thread doesn't access the
    // box implementation.
    //
    vector<CatalogBox*>::iterator  box_it = boxes.begin();
    vector<CatalogBox*>::iterator  box_end = boxes.end();

    vector<string>  box_str_names;

    for (; box_it != box_end; ++box_it)
        box_str_names.push_back((*box_it)->get_box_name().as_string());

    choke_boxes(box_str_names);


    bool input_is_split = false;
    box_it = boxes.begin();

    for (; box_it != box_end; ++box_it)
    {
        // Remove this box as a sink for its inputs.
        CatalogStream::StreamStar* ins = (*box_it)->get_box_in();
        CatalogStream::StreamStar::iterator in_it = ins->begin();
        CatalogStream::StreamStar::iterator in_end = ins->end();

        for(; in_it != in_end; ++in_it)
        {
            (*box_it)->remove_in_sink_port((*in_it)->get_stream_name());

            if ( !input_is_split && (*in_it)->get_sink_port()->size() > 0 )
            {   input_is_split = true;
            }
        }

        // No need to remove this box as a source for its
        // outputs (stream is deleted).
    }

    // Resume inputs.
    if (input_is_split)
    {   resume_boxes(box_str_names);
    }
    else
    {
        resume_box_streams(box_str_names);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Swaps a box's port from one stream to another (used for connecting boxes).
//
void AuroraNode::swap_box_ports(PortSwaps  &swaps)
{

    uint16          dest_port;

    CatalogBox     *dest_box;

    CatalogStream  *old_stream;
    CatalogStream  *new_stream;

    PortSwaps::iterator swap_it  = swaps.begin();
    PortSwaps::iterator swap_end = swaps.end();
//..............................................................................


    swap_it = swaps.begin();

    for (; swap_it != swap_end; ++swap_it)
    {
        dest_box   = swap_it->_dest_box;
        dest_port  = swap_it->_dest_port;
        old_stream = dest_box->get_in_stream(dest_port);
        new_stream = swap_it->_new_stream;

        DEBUG << "Swapping port " << dest_port << " from "
              << old_stream->get_stream_name() << " to "
              << new_stream->get_stream_name();

        DEBUG << "Before, old stream has "
              << old_stream->get_sink_port()->size()
              << " ports, dest port " << dest_port;

        //Not so???
        //assert(old_stream->get_sink_port()->size() > dest_port);

        dest_box->remove_in_sink_port(old_stream->get_stream_name(), dest_port);

        // NYI:  Set update_queue, key???
        dest_box->reset_in_stream(dest_port, new_stream);

        BoxPort new_stream_sink;
        new_stream_sink.set_box_port(dest_box, dest_port);

        new_stream->add_sink_port(new_stream_sink);

        DEBUG << "After, old stream has "  << old_stream->get_sink_port()->size()
              << " ports, new stream has " << new_stream->get_sink_port()->size()
              << " ports.";
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Adds schemas to the engine's representation
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::add_schema( vector<CatalogSchema*> schemas )
    throw( AuroraException )
{
    TopologyModifications additions;
//..............................................................................


    additions.schemas = schemas;
    BoxPackage empty_package;
    ptr<modify_network_task> task = create_add_task(additions, empty_package);

    PtMutex lock;
    PtCondition cond;
    task->_lock = &lock;
    task->_cond = &cond;

    INFO << "Scheduled modifyNetwork task; waiting...";
    LockHolder hold(lock);
    _scheduler->schedule_exclusive_task(task);
    lock.wait_cond(cond);
    INFO << "Done with modifyNetwork task";
}


////////////////////////////////////////////////////////////////////////////////
//
// Adds streams to the engine's representation
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
// Used to add input streams to the engine
//
void AuroraNode::add_stream( vector<CatalogStream*> streams )
    throw( AuroraException )
{
    TopologyModifications additions;
//..............................................................................


    additions.streams = streams;
    BoxPackage empty_package;
    ptr<modify_network_task> task = create_add_task(additions, empty_package);

    PtMutex lock;
    PtCondition cond;
    task->_lock = &lock;
    task->_cond = &cond;

    INFO << "Scheduled modifyNetwork task; waiting...";
    LockHolder hold(lock);
    _scheduler->schedule_exclusive_task(task);
    lock.wait_cond(cond);
    INFO << "Done with modifyNetwork task";
}


////////////////////////////////////////////////////////////////////////////////
//
// Adds tables to the engine's representation
//
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::add_table( vector<CatalogTable*> tables )
    throw( AuroraException )
{
    TopologyModifications additions;
//..............................................................................


    additions.tables = tables;
    BoxPackage empty_package;
    ptr<modify_network_task> task = create_add_task(additions, empty_package);

    PtMutex lock;
    PtCondition cond;
    task->_lock = &lock;
    task->_cond = &cond;

    INFO << "Scheduled modifyNetwork task; waiting...";
    LockHolder hold(lock);
    _scheduler->schedule_exclusive_task(task);
    lock.wait_cond(cond);
    INFO << "Done with modifyNetwork task";
}



////////////////////////////////////////////////////////////////////////////////
//
// Connect/add assumptions:
// -- box has already been added to the catalog via AdminCatalog
// -- query network has been choked, and will be resumed after calling this
//    function.
//
void AuroraNode::add_box( vector<CatalogBox*> boxes, BoxPackage packed_boxes)
    throw( AuroraException )
{
    TopologyModifications additions;
//..............................................................................


    additions.boxes = boxes;
    ptr<modify_network_task> task = create_add_task(additions, packed_boxes);

    PtMutex lock;
    PtCondition cond;
    task->_lock = &lock;
    task->_cond = &cond;

    INFO << "Scheduled modifyNetwork task; waiting...";
    LockHolder hold(lock);
    _scheduler->schedule_exclusive_task(task);
    lock.wait_cond(cond);
    INFO << "Done with modifyNetwork task";
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::update_box(CatalogBox *box)
    throw( AuroraException )
{
//..............................................................................


    ptr<QBox> b = _boxes[box->get_box_name().as_string()];

    if (!b)
    {
        Throw(AuroraBadEntityException, "Box " + box->get_box_name().as_string() +
                                        " does not exist");
    }

    b->set_local_box_params(box->get_box_parameter_map());

    // We must explicitly update the parameters of the running box.
    // Please note that I am adding this hack so that RandomDrop and
    // WindowDrop works correctly. I am not considering modifications
    // to the other box types.
    // -- tatbul
    //
    b->update_params();
}



////////////////////////////////////////////////////////////////////////////////
//
//  Reconnect an In stream on a box for the last phase of a remote connect.
//
void AuroraNode::update_box_in(// Destination Box.
                               CatalogBox *box,

                               // New stream between the filter and destination.
                               CatalogStream  *new_stream,

                               // Destination Box In port.
                               uint16          port)
                         throw(AuroraException)
{
    string    message;

    BoxPackage             empty_package;
    TopologyModifications  no_addition;

    map< string, vector<TupleQueue*> > downstream_queues;
//..............................................................................


    // Get the existing stream and destination box engine components.
    //
    ptr<Stream>  old_stream = get_stream(
                     box->get_in_stream(port)->get_stream_name().as_string());

    ptr<QBox> dest_qbox = get_box(box->get_box_name().as_string());

    if (!dest_qbox)
    {   message = "Box " + box->get_box_name().as_string()
                         + " implementation not found.";

        WARN << message;
        Throw(AuroraBadEntityException, message);
    }

    // Scan In stream Qs on the old stream.  There will only be 1???
    //
    for (unsigned int i = 0; i < dest_qbox->get_num_inputs(); ++i)
    {
        DEBUG << "Remove Q on old stream.";

        TupleQueue  *q = dest_qbox->get_input_queue(i);
        downstream_queues[box->get_box_name().as_string()].push_back(q);
        old_stream->remove_queue(q);  // delete _queues pointer for stream.
    }

    // Instruct scheduler task to swap ports for the destination box.
    // No box is added; only the port is swapped.
    //
    PortSwap  ps(new_stream, box, port);
    ptr<modify_network_task> task = create_add_task(no_addition, empty_package);

    task->_port_swaps.push_back(ps);

    PtMutex lock;
    PtCondition cond;
    task->_lock = &lock;
    task->_cond = &cond;

    INFO << "Scheduled modifyNetwork task; waiting...";
    LockHolder hold(lock);
    _scheduler->schedule_exclusive_task(task);
    lock.wait_cond(cond);
    INFO << "Done with modifyNetwork task";
}



////////////////////////////////////////////////////////////////////////////////
//
// Connect an inserted box within the engine.
// -- The insertion should already have been added to the Catalog, but we
//    add it to the engine itself here.
// -- This function updates existing streams, ports and queues.
// -- Note the ports must be modified in a critical, so we use a scheduler
//    task for this.
//
void AuroraNode::connect_box(// New stream between the filter and destination.
                             CatalogStream  *add_stream,

                             // Box to receive the filtered Out stream.
                             CatalogBox     *dest_box,

                             // Box being inserted.
                             CatalogBox     *add_box,

                             // In stream to the added box.
                             CatalogStream  *old_stream,

                             // Destination Box In port.
                             uint16          port)
                       throw(AuroraException)
{
    map< string, vector<TupleQueue*> > downstream_queues;
//..............................................................................


    // Get the existing stream and destination box engine components.
    //
    ptr<Stream>  old_source_stream = get_stream(old_stream->get_stream_name().as_string());

    ptr<QBox> dest_qbox = get_box(dest_box->get_box_name().as_string());

    if (!dest_qbox)
    {   WARN << "Box " << dest_box->get_box_name() << " implementation not found.";
        Throw(AuroraBadEntityException,
              "Box " + dest_box->get_box_name().as_string() +
                      " implementation not found.");
    }

    // Scan In stream Qs on the destination box and ???
    //
    for (unsigned int i = 0; i < dest_qbox->get_num_inputs(); ++i)
    {
        TupleQueue  *q = dest_qbox->get_input_queue(i);
        downstream_queues[dest_box->get_box_name().as_string()].push_back(q);
        old_source_stream->remove_queue(q);
    }

    // Create the scheduler task to add the new box.
    //
    BoxPackage             empty_package;
    TopologyModifications  additions;

    additions.boxes.push_back(add_box);
    ptr<modify_network_task> task = create_add_task(additions, empty_package);


    // Connect previous sink queues (including subscriptions) to new output.
    //
    string box_name = add_box->get_box_name().as_string();
    ptr<QBox>   new_box = task->_new_topology._boxes[box_name];

    uint16  source_out_id = 0;          // port of upstream box out???
    Stream  &new_output = new_box->get_output(source_out_id);

    vector<TupleQueue*>::iterator queue_it =
                downstream_queues[box_name].begin();

    vector<TupleQueue*>::iterator queue_end =
               downstream_queues[box_name].end();

    for ( ; queue_it != queue_end; ++queue_it )
    {
         new_output.add_queue(*queue_it);
    }

    // Instruct scheduler task to swap ports for the destination box.
    PortSwap  ps(add_stream, dest_box, port);
    task->_port_swaps.push_back(ps);

    PtMutex lock;
    PtCondition cond;
    task->_lock = &lock;
    task->_cond = &cond;

    INFO << "Scheduled modifyNetwork task; waiting...";
    LockHolder hold(lock);
    _scheduler->schedule_exclusive_task(task);
    lock.wait_cond(cond);
    INFO << "Done with modifyNetwork task";
}



////////////////////////////////////////////////////////////////////////////////
//
// Engine removals
//..............................................................................



////////////////////////////////////////////////////////////////////////////////
//
// Main handler to remove schemas, streams and boxes from the engine.
//
ptr<AuroraNode::modify_network_task>
AuroraNode::create_remove_task(TopologyModifications& removals,
                               set<Name> streams_to_preserve )
    throw( AuroraException )
{
//..............................................................................


    ptr<modify_network_task> task( new modify_network_task(*this, sm) );


    //stats
    IDVect instream_ids;
    IDVect anystream_ids; // -- tatbul
    IDVect outstream_ids;
    IDVect selectivity_ids;
    IDVect box_ids;
    IDVect stream_ids;

    vector<CatalogSchema*>& schemas = removals.schemas;
    vector<CatalogStream*>& streams = removals.streams;
    vector<CatalogBox*>& boxes = removals.boxes;

    vector<string> box_names;
    vector<string> stream_names;


    // TODO: implement schema removal
    // -- yna: this requires tracking down all elements using the schema (hard!)
    vector<CatalogSchema*>::iterator schema_it = schemas.begin();
    vector<CatalogSchema*>::iterator schema_end = schemas.end();

    if ( schema_it != schema_end )
    {
        Throw(AuroraException, "Removing schemas currently not supported!");
    }


    // Remove streams.
    vector<CatalogStream*>::iterator stream_it = streams.begin();
    vector<CatalogStream*>::iterator stream_end = streams.end();

    for ( ; stream_it != stream_end; ++stream_it)
    {
        set<Name>::iterator preserve_found =
            streams_to_preserve.find( (*stream_it)->get_stream_name() );

        if ( preserve_found == streams_to_preserve.end() )
        {
            string stream_name = (*stream_it)->get_stream_name_string();
            ptr<Stream> stream = lookup(_streams, stream_name);

            if ( ! stream )
            {   WARN << "Attempted to remove non-existent stream " << stream_name;
                Throw( AuroraBadEntityException,
                       "Attempted to remove non-existent stream " + stream_name);
            }

            INFO << "Removing stream " << stream_name << " from topology";
            task->_deleted_topology._streams[stream_name] = stream;

            if ( (*stream_it)->is_output_stream() )
            {   outstream_ids.push_back(stream_name);
            }
        }
    }


    // Remove boxes.
    vector<CatalogBox*>::iterator box_it = boxes.begin();
    vector<CatalogBox*>::iterator box_end = boxes.end();

    for ( ; box_it != box_end; ++box_it )
    {
        CatalogBox* box = (*box_it);
        string box_name = box->get_box_name().as_string();

        ptr<QBox> qbox = get_box(box_name);

        if ( !qbox )
        {
            WARN << "Attempted to remove non-existent box " << box_name;
            Throw( AuroraBadEntityException,
                   "Attempted to remove non-existent box " + box_name);
        }

        box_names.push_back(box_name);

        // Delete any of the box's outputs, causing a diagram partition.
        CatalogStream::StreamStar::iterator output_it = box->get_box_out()->begin();
        CatalogStream::StreamStar::iterator output_end = box->get_box_out()->end();

        for ( uint16 out_port = 0; output_it != output_end; ++output_it, ++out_port )
        {
            set<Name>::iterator preserve_found =
            streams_to_preserve.find( (*output_it)->get_stream_name() );

            DEBUG << "Remove cost for:" << box_name;
            selectivity_ids.push_back(string(box_name + " : " + out_port));
            box_ids.push_back(box_name);
            string stream_name = (*output_it)->get_stream_name().as_string();
            stream_ids.push_back(stream_name);
            if ( preserve_found == streams_to_preserve.end() )
            {
                ptr<Stream> stream = get_stream(stream_name);
                stream_names.push_back(stream_name);

                //olga for remove stats
                if ( (*output_it)->is_output_stream() )
                {
                    outstream_ids.push_back(stream_name);
                }

                task->_deleted_topology._streams[ stream_name ] = stream;
            }
        }

        // get Input Streams for deleting statistics
        CatalogStream::StreamStar::iterator input_it  = box->get_box_in()->begin();
        CatalogStream::StreamStar::iterator input_end = box->get_box_in()->end();

        for (uint16 in_port = 0; input_it != input_end ; ++input_it, ++in_port)
        {
            // yna -- delete QLengths regardless of whether stream is preserved.
            instream_ids.push_back(string(box_name+ " # " + in_port));

            // Commenting the below, since stats for input streams of
            // a box must never be deleted, even though the box itself
            // is removed. -- tatbul
            //
            /*
            set<Name>::iterator preserve_found =
            streams_to_preserve.find( (*input_it)->getStreamName() );

            if ( preserve_found == streams_to_preserve.end() )
            {
                string stream_name = (*input_it)->getStreamName().as_string();
                stream_ids.push_back(stream_name);
            }
            */
        }


        // Force removal of scheduled boxes
        _scheduler->remove_from_schedule(qbox.get());

        task->_deleted_topology._boxes[ box_name ] = qbox;
    }

    DEBUG << "Updating listeners...";
    _scheduler->update_listeners( box_names, false );
    DEBUG << "Finished updating listeners...";

    for (unsigned int i = 0; i < box_names.size(); ++i )
    {
        _boxes.erase(box_names[i]);
        INFO << "Removed box " << box_names[i];
    }

    for (unsigned int i = 0; i < stream_names.size(); ++i )
    {
        _streams.erase(stream_names[i]);
        INFO << "Removed stream " << stream_names[i];
    }

    // Stats deletion moved to scheduler task.
    task->_instream_ids = instream_ids;
    task->_anystream_ids = anystream_ids;  // -- tatbul
    task->_outstream_ids = outstream_ids;
    task->_selectivity_ids = selectivity_ids;
    task->_box_ids = box_ids;
    task->_stream_ids = stream_ids;

    return task;
}



////////////////////////////////////////////////////////////////////////////////
//
// Disconnect/remove assumptions
// -- box has not yet been removed from the catalog, and AdminCatalog will do this
//    after the execution of these functions
// -- query network has been choked, and will be resumed after these function.
// -- types on partitioned stream have already been checked.
//
void AuroraNode::remove_box( vector<CatalogBox*> boxes,
                            set<Name> streams_to_preserve )
    throw( AuroraException )
{
//..............................................................................


    uninstall_box_ports(boxes);
    TopologyModifications removals;
    removals.boxes = boxes;
    ptr<modify_network_task> task =
        create_remove_task( removals, streams_to_preserve );
    DEBUG << "Scheduling removal task...";
    schedule(task.dynamic_cast_to<SchedulerTask>());
}



////////////////////////////////////////////////////////////////////////////////
//
// disconnectBox notes:
// -- no need to remove listeners, since we're not actually changing the
//    TupleQueues
//
// -- Catalog updates stream and box ports. This function updates queues
//    (i.e. for subscriptions) since the catalog has no notion of this yet.
//
void AuroraNode::disconnect_box(vector<CatalogBox*> boxes)
     throw(AuroraException)
{
//..............................................................................


    map< string, pair< ptr<Stream>, vector<TupleQueue*> > > downstream_queues;

    vector<CatalogBox*>::iterator box_it = boxes.begin();
    vector<CatalogBox*>::iterator box_end = boxes.end();

    PortSwaps ports_to_swap;

    for (; box_it != box_end; ++box_it)
    {
        // Cheat:  Can't know the inserted box if on an upstream node???
        //
        CatalogBox* box = (*box_it);
        CatalogStream* box_in  = box->get_in_stream(0);
        CatalogStream* box_out = box->get_out_stream(0);

        // Connect up streams we would otherwise partition.
        //
        ptr<Stream> input_stream =
            get_stream(box_in->get_stream_name().as_string());

        ptr<Stream> output_stream =
            get_stream(box_out->get_stream_name().as_string());

        downstream_queues[box->get_box_name().as_string()] =
            make_pair(input_stream, vector<TupleQueue*>());

        copy(output_stream->get_queues().begin(),
             output_stream->get_queues().end(),
             back_inserter(downstream_queues[box->get_box_name().as_string()].
                                                                     second));

        box->remove_in_sink_port(box_in->get_stream_name(), 0);

        //ports_to_swap.push_back(PortSwap(box->get_out_stream(0), box->get_in_stream(0), 0));

        CatalogBox* dest_box =
            box_out->get_sink_box_port(0)->get_port_box();

        // TODO: compute correct ports for non one-to-one boxes.
        uint16 dest_port;
        Status stat = dest_box->get_in_stream_index(box_out, dest_port);

        DEBUG  <<      "box_in=" << box_in->get_stream_name()
               <<  "  dest_box=" << dest_box->get_box_name()
               << "  dest_port=" << dest_port;

        //PortSwap ps(box, dest_box, 0, 0, dest_port);
        PortSwap ps(box_in, dest_box, dest_port);

        ports_to_swap.push_back(ps);
    }

    swap_box_ports(ports_to_swap);

    TopologyModifications removals;
    removals.boxes = boxes;
    ptr<modify_network_task> task = create_remove_task( removals );

    box_it = boxes.begin();

    for (; box_it != box_end; ++box_it)
    {
        string box_name = (*box_it)->get_box_name().as_string();
        ptr<Stream> input_stream = downstream_queues[box_name].first;

        vector<TupleQueue*>::iterator dsq_it =
            downstream_queues[box_name].second.begin();
        vector<TupleQueue*>::iterator dsq_end =
            downstream_queues[box_name].second.end();
        for (; dsq_it != dsq_end ; ++dsq_it)
        {
            input_stream->add_queue(*dsq_it);
        }
    }

    schedule(task.dynamic_cast_to<SchedulerTask>());
}




////////////////////////////////////////////////////////////////////////////////
//
//  CHange an In stream on a box.
//
void AuroraNode::modify_box_in(// Box to be modified.
                               CatalogBox     *box,

                               // Box in port to be modified.
                               uint16          port,

                               // New stream to connect to the box in port.
                               CatalogStream  *stream
                              )
                         throw(AuroraException)
{
    CatalogStream         *catalog_stream;
    ptr<Stream>            old_stream;
    ptr<Stream>            new_stream;
    vector<CatalogBox*>    boxes;
    TopologyModifications  additions;

    vector<TupleQueue*>::iterator       queue_it;
    map< string, vector<TupleQueue*> >  downstream_queues;
    ptr<modify_network_task>            task;

    string       box_name;
    BoxPackage   packed_boxes;
    ptr<QBox>    q_box;
    PtMutex      lock;
    PtCondition  cond;
//
//  Entry:  The local catalog has the old stream (unchanged upon exit).
//   Exit:  The old stream is disconnected and the new stream is connected.
//
// modify box notes:
// -- Catalog updates stream and box ports. This function updates queues
//    (i.e. for subscriptions) since the catalog has no notion of this yet.
//..............................................................................


    // Disconnect the old input stream implementation.
    //
    old_stream = get_stream(box->get_in_stream(port)->get_stream_name().as_string());
    new_stream = get_stream(stream->get_stream_name().as_string());

    copy(old_stream->get_queues().begin(),
         old_stream->get_queues().end(),
         back_inserter(downstream_queues[box->get_box_name().as_string()]));

    // fix:  Need to push the Stream's internal buffer to the downstream queues.
    //
    old_stream->clear_queues();

    // Copy the old stream's queue to the new stream.
    //
    for (queue_it  = downstream_queues[box_name].begin();
         queue_it != downstream_queues[box_name].end(); queue_it++)
    {
        new_stream->add_queue(*queue_it);
    }

    // Create an empty add task referenced by the port swap task.
    //
    task = create_add_task(additions, packed_boxes);

    catalog_stream = box->get_in_stream(port);

    ///task->_port_swaps.push_back(PortSwap(catalog_stream, stream, port));
    task->_port_swaps.push_back(PortSwap(catalog_stream, box, port));

    task->_lock = &lock;
    task->_cond = &cond;

    INFO << "Scheduled modifyNetwork task; waiting...";
    LockHolder hold(lock);
    _scheduler->schedule_exclusive_task(task);
    lock.wait_cond(cond);
    INFO << "Done with modifyNetwork task";
}




////////////////////////////////////////////////////////////////////////////////
//
// Network element serialization
//
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::pack_boxes( vector<string>  names, BoxPackage& packs )
{
//..............................................................................


    vector<string>::iterator name_it = names.begin();
    vector<string>::iterator name_end = names.end();

    // drain boxes.
    //_scheduler->drainSubNetwork(names);

    for (; name_it != name_end; ++name_it)
    {
    string box_name = *name_it;

    ptr<QBox> box = get_box(box_name);
    ptr<QueueState> qs = box->pack_queues();
    ptr<AbstractBoxState> bs = box->pack_state();
    if ( bs )
    {
        packs.add_box(box_name, qs, bs);
    } else
    {
        packs.add_box_with_queue_only(box_name, qs);
    }
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Lock set accessors (for lock/unlock boxes)
//
ptr<LockSet> AuroraNode::get_lock_set(string name)
{
//..............................................................................


    ptr<LockSet>& ret = _lock_sets[name];

    if (!ret)
    {
        DEBUG << "Creating LockSet named " << name;
        ret.set( new LockSet());
    }
    return ret;
}



////////////////////////////////////////////////////////////////////////////////
//
set<ptr<QBox> > AuroraNode::get_boxes() const
{
//..............................................................................


    // TODO: unsafe box access! Conflicts with dynamic modification??
    set<ptr<QBox> > boxes;
    for (BoxMap::const_iterator i = _boxes.begin(); i != _boxes.end(); ++i)
        boxes.insert(i->second);
    return boxes;
}



////////////////////////////////////////////////////////////////////////////////
//
//that's if stream is not a part of view description,
//if stream is a part of viewDescription, view_desc should be the only argument to this function
//
void AuroraNode::create_cpview(CPViewDescription view_desc, StreamDef streamdef)
                         throw(AuroraException)
{
//..............................................................................


    INFO << "Creating CPView in AuroraNode";

    static int sequence_num;

    ptr<CPViewDescription> vd(new CPViewDescription(view_desc));

    string stream_name_string = view_desc.get_stream_id().get_name().as_string();
    string new_view_id;
    string new_view_id2;

    ptr<Stream> stream = lookup(_streams, stream_name_string);

    if (!stream)
    {  ERROR << "Stream " << stream_name_string << " does not exist! WTH!!";
    }

    TupleDescription tuple_desc = stream->get_tuple_description();

    if (_cps.count(streamdef.get_name()))
    {
        INFO << "CP already exists on stream " << streamdef.get_name() << "! Creating CPView only ...";
        //do not need to create a CP on this stream, so just create a view
        new_view_id = string() + stream_name_string + ":cpview:" + to_string(++sequence_num); // = ??? where do I get an id from???
        ptr<CPQBox> cp = _cps[streamdef.get_name()];
        ptr<CPView> cpv(new CPView(vd, cp, new_view_id, tuple_desc, CPView::AUTO));
        _cp_views.push_back(cpv);
        stream->get_cp()->add_view(new_view_id, cpv);
        // TODO:
        //////SETTING THE TUPLE QUEUE??????????????????
    }
    else
    {
        INFO << "No CP exists on " << streamdef.get_name();
        //need to create a CP first, then create CPViews for all branches of that stream
        //This is where the new implementaition for creating a CP will have to go for now

        /*** BEGIN MAKING CPQBOX ***/

        //Create the CatalogBox that holds information about the CP
        CatalogBox box_1 ;
        //Name cp_name =  ("CP"+to_string(++sequence_num)) ;  //Give the CP a unique name
        Name cp_name = (streamdef.get_name().as_string() + "CP");
        box_1.set_box_name_type( cp_name  , "connectionpoint");

        //Get a Catalog stream from the actual stream supplied
        CatalogStream* c_stream = stream.get()->get_local_stream();

        //Add the Catalog box to the local catalog
        _local_catalog.add_box(box_1);

        //make sure we are working with the catalog box that is actually stored in the local catalog.
        //NOTE: this step may be unnessesary, but it is done in create_add_task, so this is the way I did it here
        CatalogBox* connection_box = _local_catalog.get_box(box_1.get_box_name());

        //Add the description of the stream to the Catalog Box
        connection_box->add_in_stream(c_stream, CatalogBox::InQueue() );

        //Create the task that will insert the CP into the networks topology
        ptr<modify_network_task> task (new modify_network_task(*this, sm));

        // Initialize QBox
        string box_type = connection_box->get_box_type();
        string box_name = connection_box->get_box_name().as_string();

        //Actually create the CPQBox, and cast it as such
        ptr<QBox> cpq ( QBox::instantiate("connectionpoint"));
        DEBUG << "The CPQBox has been instantiated";
        ptr<CPQBox> cp = cpq.static_cast_to<CPQBox>();

        //Set the CP's input to the stream
        cp->set_input( 0 , stream.get() );

        //set the CP to be added to the topology
        task->_new_topology._boxes[ box_name ] = cpq;

        _monitor.cpview_statistics(cp_name, sm);

        // boxid for CPQBox
        _box_id_by_name.insert(pair<Name, int>(Name(box_name), _box_id_by_name.size()));
        //Set up the box
        cp->setup(*this, connection_box);
        cp->set_order_by_field(view_desc.get_order_by_field_string());

        /*** END MAKING CPQBOX ***/

        /*** START MAKING SWEEPERQBOX ***/

        //STEPS:
        //      Make two streams for the sweeper
        //      Make the sweeper on those two streams
        //      Add the streams and sweeper to the task
        //      setup the sweeper
        //      Make sweeper swap info with each non-CPQBox QBox
        //Right now there is the assumption that there is only one
        //sweeper (and thus only one CP), since I can't yet tell what boxes
        //are supposed to be using this CP as opposed to others.

        // name of sweeper concatenate "sweeper" to name of CP
        // easy to distinguish sweepers for each CP
        Name sw_name = Name(cp_name.as_string()+"sweeper");

        // maximum size of tuples
        set< ptr<QBox> > qboxes = get_boxes();
        int max_schema_size = 0;
        for(set< ptr<QBox> >::iterator i = qboxes.begin(); i != qboxes.end(); ++i)
        {
            for( unsigned int j = 0; j < (*i)->get_num_outputs(); j++ )
            {
                max_schema_size =
                max_schema_size >= (*i)->get_out_schema(j)->get_size()
                ? max_schema_size :  (*i)->get_out_schema(j)->get_size();
            }
        }

        // Set up the stream for historicals to come to the sweeper
        // stream name is sweepername + "historical"
        CatalogSchema *historical_schema = new CatalogSchema();
        Name stream_name = sw_name.as_string()+"historical";
        historical_schema->set_schema_name(stream_name);
        historical_schema->_field = vector<SchemaField>();
        historical_schema->_field.push_back( SchemaField( "__data",
                            DataType::STRING,
                            max_schema_size,
                            HEADER_SIZE ) );
        _local_catalog.add_schema(stream_name, *historical_schema);
        CatalogStream *historical_stream = _local_catalog.add_stream(stream_name, stream_name );
        (task->_new_topology._schemas)[stream_name.as_string()] =    TupleDescription( historical_schema );

        ptr<Stream> historical_dummy_stream = ptr<Stream>(new Stream(stream_name.as_string(), *this));
        (task->_new_topology._streams)[stream_name.as_string()] = historical_dummy_stream;

        // Ditto for requests
        stream_name = sw_name.as_string()+"request";
        CatalogSchema *request_schema = new CatalogSchema();
        request_schema->set_schema_name(stream_name);
        request_schema->_field = vector<SchemaField>();
        request_schema->_field.push_back( SchemaField( "__data",
                             DataType::STRING,
                             max_schema_size,
                             HEADER_SIZE ) );
        _local_catalog.add_schema(stream_name, *request_schema);
        CatalogStream *request_stream = _local_catalog.add_stream(stream_name, stream_name );
        (task->_new_topology._schemas)[stream_name.as_string()] = TupleDescription( request_schema );
        ptr<Stream> request_dummy_stream = ptr<Stream>(new Stream(stream_name.as_string(), *this));
        (task->_new_topology._streams)[stream_name.as_string()] = request_dummy_stream;

        // Create CatalogBox for sweeper
        CatalogBox sw;
        sw.set_box_name_type(sw_name, "sweeper");

        _local_catalog.add_box(sw);

        CatalogBox *sweeper_box = _local_catalog.get_box( sw.get_box_name() );
        sweeper_box->add_in_stream( request_stream, CatalogBox::InQueue() );
        sweeper_box->add_in_stream( historical_stream, CatalogBox::InQueue() );

        box_type = sweeper_box->get_box_type();
        box_name = sweeper_box->get_box_name().as_string();

        ptr<QBox> swq( QBox::instantiate( "sweeper" ) );
        ptr<SweeperQBox> sweeper = swq.static_cast_to<SweeperQBox>();
        sweeper->set_input( 0, request_dummy_stream.get() );
        sweeper->set_input( 1, historical_dummy_stream.get() );
        task->_new_topology._boxes[sw_name.as_string()] = sweeper;

        _monitor.cpview_statistics9(sw_name, sm);

        // give boxid for SweeperQBox
        _box_id_by_name.insert(pair<Name, int>(Name(box_name), _box_id_by_name.size()));
        // setup sweeper
        sweeper->setup(*this, sweeper_box);

        /*** END MAKING SWEEPERQBOX ***/

        // go through the QBoxes and trade info
        // DONE VIA schedule_exclusive_task
        // Info needed for R{A,F,M}QB::set_downstream_environment:
        //      ptr<TupleQueue>    historical queue
        //      ptr<TupleQueue>    request queue
        //      int                locally-assigned ID from teh sweeper
        //      SweeperQBox*       The sweeper itself.
        //
        // Should ensure that the QBox (from qboxes above) is actually a revision box
        //    (is there a better way to do this? QBox::set_downstream_environment perhaps?)
        // Sweeper gets the box's input TupleQueue
        // XXX: this needs to be done /after/ init, since that's when TQs get made.

        //Attain a lock, and actually insert the CP (TODO: and sweeper) into the network.  Currently does not choke lower boxes in the network, should not be a problem.
        PtMutex lock;
        PtCondition cond;
        task->_lock = &lock;
        task->_cond = &cond;

        INFO << "Scheduled modifyNetwork task; waiting...";
        LockHolder hold(lock);
        _scheduler->schedule_exclusive_task(task);
        lock.wait_cond(cond);
        INFO << "Done with modifyNetwork task";

        /*** FINALLY CREATE VIEW ***/

        // let the stream know that it has a cp.
        stream->set_cp(cp);

        _cps[streamdef.get_name()] = cp;

        // create for the the tuplequeue that this view_desc is for
        bool found = false;
        vector<BoxPort> box_ports = *(stream->get_sink_port());
        for (vector<BoxPort>::iterator iter = box_ports.begin();
             iter != box_ports.end(); ++iter)
        {
            CatalogBox *box = iter->get_port_box();
            if ( view_desc.get_name() == box->get_cpview(stream->get_name()) )   // if this view_desc is for the view on the input of this box
            {
                NOTICE << "Creating CPView in stream " << stream->get_name() << " pointing to TupleQueue of box " << box->get_box_name();

                new_view_id = view_desc.get_name().as_string();
                ptr<CPView> new_cpv(new CPView(vd, cp, new_view_id, tuple_desc, CPView::AUTO));
                _cp_views.push_back(new_cpv);
                stream->get_cp()->add_view(new_view_id, new_cpv);
                TupleQueue *tq = get_box(box->get_box_name().as_string())->get_input_queue(iter->get_port());
                ptr<TupleQueue> tq_ptr(tq);
                new_cpv->set_tuple_queue(tq_ptr);
                found = true;
            }
        }
        if (!found)   // when view is on the output stream, no TQ to point to. thus just make a view with TQ pointing to null
        {
            new_view_id = view_desc.get_name().as_string();
            ptr<CPView> new_cpv(new CPView(vd, cp, new_view_id, tuple_desc, CPView::AUTO));
            _cp_views.push_back(new_cpv);
            stream->get_cp()->add_view(new_view_id, new_cpv);
            new_cpv->set_tuple_queue(ptr<TupleQueue>());  // NOTHING TO POINT TO
        }

        DEBUG << view_desc.as_string();
    }

    INFO << "Done creating CPView on " << streamdef.get_name();
}



////////////////////////////////////////////////////////////////////////////////
//
// Topology helpers
//
string AuroraNode::repr() const
{
//..............................................................................


    string out = "AuroraNode {\n";

    out << indent(_topology.repr(), 4);
    out << "}\n";

    return out;
}



////////////////////////////////////////////////////////////////////////////////
//
string AuroraNode::describe(string entity)
    throw (AuroraException)
{
    string desc;
//..............................................................................


    if (ptr<Stream> stream = lookup(_streams, entity))
    {
        desc = stream->as_xml(Stream::FULL_SCHEMA);
    } else if (_boxes.count(entity))
    {
        Throw(AuroraException, "Description of QBox not implemented");
    } else if (TupleDescription schema = lookup(_schemas, entity))
    {
        desc = schema.as_xml();
    }

    return desc;
}



////////////////////////////////////////////////////////////////////////////////
//
// yna -- why is this here if it only uses the Catalog???
// (it should be in the Catalog itself)
//
void AuroraNode::list_entities( entity_type   type,
                           vector<string>  &names )
                        throw( AuroraException )
{
    Catalog::StreamMap::iterator         stream;
    CatalogSchema::SchemaMap::iterator   schema;
    Catalog::BoxMap::iterator            box;
    Catalog::StreamMap                  *stream_map;
    CatalogSchema::SchemaMap            *schema_map;
    Catalog::BoxMap                     *box_map;
//..............................................................................


    names.clear();

    switch (type)
    {
      case STREAM:
      case INPUT_STREAMS:
      case OUTPUT_STREAMS:
      {
      //stream_map = _local_catalog.getStreams();
      stream_map = _local_catalog.get_stream_map();

      for ( stream  = stream_map->begin();
        stream != stream_map->end();
        ++stream )
      {
          // Skip if there's an upstream box
          if (( type == INPUT_STREAMS )  &&
              ( !stream->second.get_stream_schema() ))
          {
              continue;
          }

          // Skip if there's a downstream box
          if (( type == OUTPUT_STREAMS )  &&
              ( !(stream->second.get_sink_port()->empty()) ))
          {
              continue;
          }

          names.push_back( stream->first.as_string() );
      }
      break;
      }

      case SCHEMA:
      {
          //schema_map = _local_catalog.getSchemas();
          schema_map = _local_catalog.get_schema_map();

          for ( schema  = schema_map->begin();
                schema != schema_map->end(); ++schema )
          {
              names.push_back( schema->first.as_string() );
          }

          break;
      }

      case BOX:
      {
      //box_map = _local_catalog.getBoxes();
      box_map = _local_catalog.get_box_map();

      for ( box  = box_map->begin();
            box != box_map->end(); ++box )
      {
          names.push_back( box->first.as_string() );
      }

      break;
      }

      default:
      Throw( aurora_illegal_argument_exception,
         "Illegal entity type #" + to_string((unsigned int)(type)) );
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// HA helpers
//
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::generic_checkpoint_task::run()
{
//..............................................................................


    if ( _cond )
    {
        LockHolder hold( *_lock );
        perform_task();
    }

    _cond->signal();
}



////////////////////////////////////////////////////////////////////////////////
//
// A checkpoint task makes a snapshot of the state of all queues and all boxes
//
void AuroraNode::checkpoint_task::perform_task()
{
//..............................................................................


    INFO << "Now checkpointing";
    // First check if all boxes are ok to checkpoint

    for(AuroraNode::BoxMap::iterator b = _node._boxes.begin(); b != _node._boxes.end(); b++)
    {
        if ( ! b->second->is_stable_for_checkpoint())
        {
            WARN << "Cannot checkpoint because not stable already";
            return;
        }
    }

    long long before = Time::now().to_usecs();

    for( AuroraNode::BoxMap::iterator i  = _node._boxes.begin();
                                      i != _node._boxes.end(); i++ )
    {

        ptr<QBox> box = i->second;
        string box_name = box->get_name();

        ptr<QueueState> qs = box->pack_queues();
        ptr<AbstractBoxState> bs = box->pack_state();
        if ( bs )
        {
            _node._checkpointed_state.add_box(box_name, qs, bs);
        } else
        {
            _node._checkpointed_state.add_box_with_queue_only(box_name, qs);
        }
    }

    long long after = Time::now().to_usecs();
    INFO << "\n After - before is " << after-before;
    _measurements.push_back( after-before );
}



////////////////////////////////////////////////////////////////////////////////
//
// An update task re-initializes the state of all queues
// and boxes from the checkpointed state
//
void AuroraNode::recovery_task::perform_task()
{
//..............................................................................


    NOTICE << "Now recovering";
    for( AuroraNode::BoxMap::iterator i  = _node._boxes.begin();
                                      i != _node._boxes.end(); i++ )
    {
        ptr<QBox> box = i->second;
        string box_name = box->get_name();

        ptr< pair< ptr<AbstractQueueState>, ptr<AbstractBoxState> > > box_state
            = _node._checkpointed_state.find_box( box_name );

        if ( box_state )
        {
            ptr<QueueState> qs = box_state->first.dynamic_cast_to<QueueState>();
            if ( !i->second->do_unpack_queues() )
            {
                WARN << "When restarting from a local checkpoint, we should not touch the input queues of Sunions placed on input streams";
            }
            else
            {
                box->clear_queue(qs);
                box->unpack_queue(qs);
            }

            ptr<AbstractBoxState> bs = box_state->second;

            if ( bs )
            {
                box->unpack_state( bs );
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::checkpoint( vector<long long>  &measurements )
{
//..............................................................................


    ptr<generic_checkpoint_task> task( new checkpoint_task( *this, measurements ));
    schedule_checkpoint_task( task );
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::recover()
{
//..............................................................................


    ptr<generic_checkpoint_task> task( new recovery_task( *this ));
    schedule_checkpoint_task( task );
}



////////////////////////////////////////////////////////////////////////////////
//
void AuroraNode::schedule_checkpoint_task( ptr<generic_checkpoint_task>  task )
{
//..............................................................................


    PtMutex lock;
    PtCondition cond;
    task->_lock = &lock;
    task->_cond = &cond;

    LockHolder hold(lock);
    _scheduler->schedule_exclusive_task( task );
    lock.wait_cond( cond );

    return;
}


BOREALIS_NAMESPACE_END

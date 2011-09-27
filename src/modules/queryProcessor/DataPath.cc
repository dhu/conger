#include "DataPath.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
DataPath::DataPath(MedusaID id, AuroraNode& node, InetAddress data_add)
    : _enqueuing(false),
      _aurora(node),
      _deq_handle(_aurora.get_dequeue_handle()),
      _id(id),
      _enq_thread(*this,data_add),
      _deq_thread(*this),
      _fwd_suspend(false),
      _fast_dequeue_fwd(*this)
{
//..............................................................................


    // Create pipe for dequeue thread to communicate with forward thread
    pair<IOHandle, IOHandle> iohs = IOHandle::pipe();

    _deq_pipe_read  = iohs.first;
    _deq_pipe_write = iohs.second;

    _deq_pipe_read.set_blocking(false);
    _deq_pipe_write.set_blocking(false);
}



////////////////////////////////////////////////////////////////////////////////
//
void  DataPath::init()
{
//..............................................................................


    _enq_thread.start();
    _enqueuing = true;
    _deq_thread.start();
    _fast_dequeue_fwd.start();
}



////////////////////////////////////////////////////////////////////////////////
//
DataPath::~DataPath()
{
//..............................................................................


    if (_enqueuing)
    {   stop_enqueue();
    }

    _deq_thread.stop();
    _fast_dequeue_fwd.stop();
    _deq_thread.join();
    _fast_dequeue_fwd.join();
}



////////////////////////////////////////////////////////////////////////////////
//
void  DataPath::stop_enqueue()
{
//..............................................................................


    _enqueuing = false;
    _enq_thread.stop();
    _enq_thread.join();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  DataPath::suspend_dequeue()
{
//..............................................................................


    _fwd_suspend = true;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  DataPath::resume_dequeue()
{
//..............................................................................


    _fwd_suspend = false;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  DataPath::clear_send_queues()
{
//..............................................................................


    locking(_output_paths_lock)
    {
        // For each output stream
        for(IOPaths::iterator i  = _output_paths.begin();
                              i != _output_paths.end(); ++i)
        {
            IOQueue* ioqueue = i->second;
            ioqueue->clear_buffer();
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  DataPath::inspect_and_buffer_output(Name    stream_name,
                                          char   *buf,
                                          size_t  buf_size,
                                          size_t  tuple_size)
{
//..............................................................................


    DEBUG << "inspect_and_buffer_output stream=" << stream_name;

    // Computing stats on streams
    //
    locking (_output_paths_lock)
    {
        IOQueue* ioqueue = lookup(_output_paths, stream_name);

        if (ioqueue)
        {
            // If a replica set, then inspect the output.
            //
            if (_aurora.is_replica_set())
            {
                // inspect the tuples
                bool  needs_cleaning = ioqueue->inspect_data(buf, buf_size,
                                                             tuple_size);

                DEBUG << "needs_cleaning=" << needs_cleaning;

                if (needs_cleaning)
                {
                    size_t  clean_buf_size = buf_size;
                    char clean_buf[clean_buf_size];

                    clean_buf_size = ioqueue->clean_data(buf,       buf_size,
                                                         clean_buf, clean_buf_size,
                                                         tuple_size);

                    // put the cleaned tuples in the output queue
                    ioqueue->buffer(clean_buf, clean_buf_size, tuple_size);
                }
                else
                {   // put the tuples in the output queue
                    ioqueue->buffer(buf, buf_size, tuple_size);
                }
            }
            else
            {   // put the tuples in the output queue
                ioqueue->buffer(buf, buf_size, tuple_size);
            }
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
bool  DataPath::stable_output(Name stream_name)
{
//..............................................................................


    // Computing stats on streams
    //
    locking (_output_paths_lock)
    {
        IOQueue* ioqueue = lookup(_output_paths,stream_name);

        if (ioqueue)
        {   return  !ioqueue->tentative();
        }
    }

    // By default everything is fine.
    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
bool  DataPath::stabilizing_output(Name stream_name)
{
//..............................................................................


    // Computing stats on streams
    //
    locking (_output_paths_lock)
    {
        IOQueue* ioqueue = lookup(_output_paths, stream_name);

        if (ioqueue)
        {
            return ioqueue->stabilizing();
        }
    }

    // By default everything is fine.
    return  true;
}

////////////////////////////////////////////////////////////////////////////////
//
// Listens on input streams and pushes events to Aurora.
//
void  DataPath::EnqueueThread::run()
{
//..............................................................................


    DEBUG << "Setting up acceptor at address " << _data_add;

    ptr<DataHandler::DHArgs> args(new DataHandler::DHArgs(MAX_SOCKET_BUFFER,
                        wrap(*(this),
                        &DataPath::EnqueueThread::do_handle_input_event)));

    TCPAcceptor<DataHandler, ptr<DataHandler::DHArgs> >
                        acceptor(_data_loop,_data_add,args);

    if (!acceptor)
    {   WARN << "Failed setting-up fast data path: " << acceptor.stat();
    }
    else
    {   NOTICE << "Accepting data connections on "
               << acceptor.get_socket().getsockname();

        _data_loop.run();
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output tuples from Aurora and hands them over to the
// FastDequeueForwarder thread.
//
void  DataPath::DequeueThread::run()
{
    char              buf[BIG_BUF];
    size_t            total_read;
    size_t            tuple_size;
    TupleDescription  td;
    string            stream_name;
//..............................................................................


    INFO << "Aurora dequeue thread running";
    _done = false;

    while (!_done)
    {
        stream_name = "";

        try
        {   // Wait for output tuples sent from Aurora (blocking call)
            //
            total_read = _data_path._deq_handle.dequeue(stream_name,
                                                        td,
                                                        buf,
                                                        sizeof(buf));

            if (total_read == 0  ||  _done) // Aurora got shutdown or we got stopped
            {   return;
            }

            // Buffer tuples in an output queue.
            //
            tuple_size = td.get_size();

            // TO DO:  Modify to insert tuples with arrays???
            //
            _data_path.inspect_and_buffer_output(Name(stream_name),
                                                 buf,
                                                 total_read * tuple_size,
                                                 tuple_size);

            // Construct a StreamEvent from the set of tuples read.
            //
            ptr<StreamEvent> event(new StreamEvent(stream_name));

            event->_inserted_count = total_read;

            //ptr<StreamEvent> event(
            //    new StreamEvent(stream_name,
            //                    string((const char *)&buf,
            //                           total_read * td.get_size()),
            //                    total_read));


            // Deallocate any arrays on the output tuple.
            // Copy the array data into the event.
            // Modify array fields the buffer to contain the array size in bytes.
            //
            event->_bin_tuples = string((const char *)&buf,
                                        total_read * td.get_size());


            Tuple  t = Tuple::factory(buf);

            if (t.get_array_flag())
            {   DynamicArray::array_deallocator(td, buf, total_read, event->_bin_arrays);
            }

            event->_bin_tuples = string((const char *)&buf,
                                        total_read * td.get_size());

            //DEBUG << "  BYTES " << to_hex_string(buf + HEADER_SIZE,
            //                                     td.get_size() - HEADER_SIZE);


            //////////////////
            // We are the source of this stream (optional parameter,
            //    required when using replication)
            //
            event->_source = InetAddress(to_string(_data_path._id));


            //DEBUG << "  BYTES " << to_hex_string(buf + HEADER_SIZE,
            //                                     td.get_size() - HEADER_SIZE);

            ClientsPerStream::iterator stream_iter = _data_path._output_clients.find(stream_name);

            //DEBUG << "stream=" << stream_name << " _output_clients=" << _data_path._output_clients.size();
            //      << " name=" << stream_iter->first << " size=" << stream_iter->second.size();


            // If this is a control message, then directly invoke a callback.
            //
            DataHandler::dhcallback cb = lookup(_data_path._local_clients,
                                                stream_name);

            if (cb)
            {   cb(event);
            }
            else
            {   // Otherwise send to all downstream clients
                //
                locking(_data_path._events_to_send_lock)
                {
                    _data_path._events_to_send.push_back(event);
                }

                // If not suspended, signal the forward thread
                //
                if (! _data_path._fwd_suspend)
                {
                    _data_path._deq_pipe_write.write(constbuf("A",1));
                }
            }
        }
        catch (AuroraException& e)
        {
            ERROR << "Dequeueing events from Aurora failed: " << e.what();
        }
    }

    INFO << "Aurora has shut down";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void DataPath::FastDequeueForwarder::run()
{
//..............................................................................


    _fwd_handler = ptr<ForwardHandler>(new ForwardHandler(_data_fwd_loop,*this));
    _data_fwd_loop.run();
}



////////////////////////////////////////////////////////////////////////////////
//
void DataPath::FastDequeueForwarder::stop()
{
//..............................................................................


    _data_fwd_loop.terminate();
}



////////////////////////////////////////////////////////////////////////////////
//
DataPath::FastDequeueForwarder::ForwardHandler::ForwardHandler(IOEventLoop& loop,
                                                               DataPath::FastDequeueForwarder& fwd_thread)
    : IOHandler(loop, fwd_thread._data_path._deq_pipe_read),
      _data_path(fwd_thread._data_path),
      _fast_dequeue_fwd(fwd_thread)
{
//..............................................................................


    DEBUG << "DataPath::FastDequeueForwarder::ForwardHandler::ForwardHandler sets _data_path";
    want_read(true);
}



////////////////////////////////////////////////////////////////////////////////
//
void DataPath::FastDequeueForwarder::ForwardHandler::ravail()
{
    bool delay = false;
//..............................................................................


    get_ioh().drain();

    locking (_data_path._events_to_send_lock)
    {
        list< ptr<StreamEvent> >::iterator i;
        Status stat(true);
        // DEBUG << "_events_to_send=" << _data_path._events_to_send.size();

        for ( i = _data_path._events_to_send.begin(); i != _data_path._events_to_send.end(); ++i)
        {
            stat = _fast_dequeue_fwd.send_data((*i)->_stream, (*i));

            if ( (to_string(stat) == DataHandler::NO_SPACE) )
            {
                delay = true;
                break;
            }
        }

        _data_path._events_to_send.erase(_data_path._events_to_send.begin(),i);
    }

    if (delay)
    {
        INFO << "Slowing down fwd... helps absorb bursts but may cause dups with multiple downstream nodes";
        Thread::sleep(Time::msecs(20));
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Returns either success OR no_space if at least one downstream client has no_space
//
Status DataPath::FastDequeueForwarder::send_data(Name              stream_name,
                                                 ptr<StreamEvent>  event)
{
    Status  success(true);
    bool    some_clients_failed = false;
    bool    stable_output;
    bool    stabilizing_output;

    ptr<DataHandler>  dh;

    OneClientMap::iterator      client_iter;
    ClientsPerStream::iterator  stream_iter;
//..............................................................................


    // For fault-tolerance, checking the current stability state of this output streams
    //
    stable_output = _data_path.stable_output(stream_name);
    stabilizing_output = _data_path.stabilizing_output(stream_name);

    locking(_data_path._output_clients_lock)
    {
        stream_iter = _data_path._output_clients.find(stream_name);

        // If no one subscribed, then there's nothing to do
        //
        if (stream_iter == _data_path._output_clients.end())
        {   return  true;
        }

        //DEBUG << "stream=" << stream_name << " _output_clients=" << _data_path._output_clients.size()
        //      << " name=" << stream_iter->first << " size=" << stream_iter->second.size();

        // For each client that subscribed to this stream.
        //
        for (client_iter  = stream_iter->second.begin();
             client_iter != stream_iter->second.end(); ++client_iter)
        {
            dh = client_iter->second._data_handler;

            if ( !dh || !dh->_cached_handle )
            {
                WARN << "Outgoing data handler gone... continuing to next.. and deleting client";
                some_clients_failed = true;
                _data_path._failed.push_back(dh);
                stream_iter->second.erase(client_iter);
                continue;
            }

            // Check if we need to bring the client up-to-date first.
            //
            if ( (! client_iter->second._is_up_to_date) && ( stable_output || stabilizing_output ) )
            {   DEBUG << "update_client at " << client_iter->first << "...";
                update_client(client_iter->second, stream_name);
            }
            else
            {   // Otherwise, send the data (the update above will send the latest data as well so don't resend)
                // Send the StreamEvent
                //
                Status  stat(true);
                // DEBUG << "address=" << client_iter->first << " connected=" << dh->_connected << " is_new=" << dh->_is_new;

                // Check if a new data connection was not made when established.
                // This can happen when a subscription is deployed before the
                // client is launched.
                //
                if ((!dh->is_connected())  &&  (dh->is_new()))
                {
                    // Create a new data handler for the connection.
                    // In the process it will reconnect to the client.
                    // The old data handler is then discarded.
                    //
                    NOTICE << "First data access and disconnected ...";
                    IOEventLoop  *loop = dh->get_loop();

                    IOHandle  ioh = TCPSocket(client_iter->first,
                                              Socket::nonblocking);

                    ptr<DataHandler>  new_dh(new DataHandler(*loop,
                                                             ioh,
                                                             dh->_args));

                    client_iter->second._data_handler = new_dh;
                    dh = new_dh;             // Smart ptr deletes the old copy.

                    // Verify that the new connection was made.
                    //
                    if (!dh->is_connected())
                    {   WARN << "Did not connect to the client at " << client_iter->first << "!!!";
                    }
                }

                stat = dh->send_data(event);

                // Succesfully sent
                if (stat)
                {   DEBUG << "Sent " << event->_inserted_count << " tuples to " << client_iter->first;

                    // Make sure new clients will be correctly updated later on
                    if ( (! client_iter->second._is_up_to_date) && (!stable_output) && (!stabilizing_output) )
                    {
                        bool got_tentative_tuples = true;
                        client_iter->second._sub.set_history(// Same first insertion tuple
                                                             client_iter->second._sub.get_last_insertion_tuple_as_string(),
                                                             // Client got tentative tuples for sure now.
                                                             got_tentative_tuples);

                        // Both the node and client have seen a failure
                        client_iter->second._independent_failure = false;
                    }
                }
                else
                {   // Some kind of failure occured

                    if (to_string(stat) == DataHandler::NO_SPACE)
                    {   success = stat;
                    }

                    // If no space, just drop this event otherwise delete handler
                    // The socket is probably gone by now, so DO NOT try to access it
                    //
                    if ((to_string(stat) != DataHandler::NO_SPACE)  &&
                        (to_string(stat) != DataHandler::NOT_YET_CONNECTED))
                    {
                        WARN << "Outgoing data handler to destination " << client_iter->first
                             << " failed... removing";

                        some_clients_failed = true;
                        _data_path._failed.push_back(dh);
                        stream_iter->second.erase(client_iter);
                    }
                }
            }
        }
    }

    //DEBUG  << "some_clients_failed=" << some_clients_failed;

    // If clients failed, then we'll have to clean-up their data handlers
    //
    if (some_clients_failed)
    {   (new CallbackTimer(_data_fwd_loop,
                           wrap(_data_path,
                                &DataPath::clean_failed_handlers)))
                 ->arm(Time::now() + Time::msecs(CLEANUP_DELAY));
    }

    return  success;
}



////////////////////////////////////////////////////////////////////////////////
//
Status  DataPath::FastDequeueForwarder::update_client(ClientInfo  &client_info,
                                                      Name         stream_name)
{
    Status  status = true;
//..............................................................................


    INFO << "Updating a client";

    // First some error checking, just in case
    IOQueue* output_queue = lookup(_data_path._output_paths,stream_name);

    if (!output_queue)
    {   return  Status("Not an output path");
    }

    if (!client_info._sub.is_valid_last_insertion_tuple())
    {   return  Status("No valid history information");
    }

    // Now we can actually update the client.
    //
    Tuple t = client_info._sub.get_last_insertion_tuple();
    INFO << "Client needs to get history starting at tuple " << t.get_tuple_id();


    // Gather the history that the client needs into a StreamEvent...
    // This may require waiting to produce the expected tuple.
    //
    ptr<StreamEvent> hist_event = output_queue->all_tuples_since(client_info._sub.get_last_insertion_tuple_as_string());
    hist_event->_source = InetAddress(to_string(_data_path._id));
    INFO << "Stream event is " << hist_event;

    if (hist_event->_inserted_count == 0)
    {   return  Status("We are running late... nothing to send yet");
    }

    // If client received any tentative tuples,
    // we must send an UNDO tuple before the corrections.
    //
    if (client_info._sub.tentative_tuple_to_correct())
    {
        // UNDO is same as last insertion just with a different type
        string undo_tuple = client_info._sub.get_last_insertion_tuple_as_string();

        ((Tuple)Tuple::factory(undo_tuple.data())).set_tuple_type(TupleType::UNDO);

        ptr<StreamEvent> undo_event(new StreamEvent(hist_event->_stream,undo_tuple,1));

        undo_event->_source = InetAddress(to_string(_data_path._id));
        status = client_info._data_handler->send_data(undo_event);
    }

    // And now try to send the history...
    //
    if (status)
    {   status = client_info._data_handler->send_data(hist_event);
    }

    if (status)
    {
        // If the client has seen a failure the node has not, then we need to
        // send a REC_DONE tuple because the node will not produce one.
        //
        if (client_info._independent_failure)
        {
            INFO << "Sending a special REC_DONE for the client";
            string rec_done_tuple = client_info._sub.get_last_insertion_tuple_as_string();

            Tuple t = Tuple::factory(rec_done_tuple.data());
            t.clear_tuple(rec_done_tuple.length());
            t.set_default_header();
            t.set_tuple_type(TupleType::RECONCILIATION_DONE);

            // Set the tuple_stime as the one corresponding to
            // the last tuple sent.
            //
            const char* buf = hist_event->_bin_tuples.data();
            size_t buf_size = hist_event->_inserted_count * hist_event->_tuple_size;
            Tuple last_history_tuple = Tuple::factory(buf + buf_size - hist_event->_tuple_size);
            t.set_tuple_stime(last_history_tuple.get_tuple_stime());

            ptr<StreamEvent> rec_done_event(new StreamEvent(hist_event->_stream,rec_done_tuple,1));
            rec_done_event->_source = InetAddress(to_string(_data_path._id));

            status = client_info._data_handler->send_data(rec_done_event);

            while (!status)
            {
                WARN << "Failed to send the REC_DONE... retrying ";
                status = client_info._data_handler->send_data(rec_done_event);
                Thread::sleep(Time::msecs(10));
            }
        }

        INFO << "Client is now up to date";
        client_info._is_up_to_date = true;
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
// Any previously failed handlers? Remove before trying to send
//
void  DataPath::clean_failed_handlers()
{
//..............................................................................


    locking(_output_clients_lock)
    {
        _failed.clear();
    }

}



////////////////////////////////////////////////////////////////////////////////
//
Status  DataPath::EnqueueThread::do_handle_input_event(ptr<StreamEvent> event)
{
    Name   stream_name;
    int32  buffer_size;
    int32  tuple_size;
    int32  nb_tuples;

    char  *bin_data;
    char  *bin_array;             // Assume no arrays in the tuples.
//..............................................................................


    ASSERT(event);

    stream_name = event->_stream;
    buffer_size = event->_bin_tuples.size();
    tuple_size  = buffer_size / event->_inserted_count;
    nb_tuples   = event->_inserted_count;
    bin_data    = (char*)event->_bin_tuples.data();

    DEBUG << "do_handle_input_event: The complete event was " << *event;

    //INFO << "Handle in";

    // Perform appropriate verification on any incoming data,
    // for fault-tolerance purposes.
    //
    Status status = _data_path.fault_tolerance_checks(event);

    if (!status)
    {
        // When fault-tolerance checks say that we should drop the input,
        // it is a fault; not an error.
        //
        ERROR << status;
        return  true;
    }

    // Ensure no one is changing the input streams nor swapping an
    // input stream definition until we have safely buffered the data.
    //
    locking (_data_path._input_paths_lock)
    {
        IOQueue  *input_queue = lookup(_data_path._input_paths, stream_name);

        if (!input_queue)
        {   return  "Not an input path";
        }

        // Closed event means we don't have to do anything
        //
        if (event->_closed)
        {   return  true;
        }

        if (_data_path._aurora.is_replica_set())
        {   // The input queue needs to keep track of the data received.
            //
            input_queue->inspect_data(bin_data,
                                      nb_tuples * tuple_size,
                                      tuple_size);
        }
    }

    // Now push these tuples into Aurora
    try
    {
        locking(_data_path._aurora_lock)
        {
            //DEBUG <<   "tuple size=" << event->_bin_tuples.size()
            //      << "  array size=" << event->_bin_arrays.size();

            DEBUG << "Enqueuing " << nb_tuples << " tuples into Aurora on stream " << stream_name;

            // Moved here to test locking???
            //
            bin_array = NULL;

            if (event->_bin_arrays.size())
            {   bin_array = (char*)event->_bin_arrays.data();
            }

            _data_path._aurora.enqueue(stream_name.as_string().c_str(),
                                       bin_data,
                                       bin_array,
                                       tuple_size,
                                       nb_tuples);
        }
    }
    catch (AuroraException& e)
    {
        ERROR << "Failed enqueuing on stream " << stream_name << " : " << e.what();
        ERROR << "The complete event was " << *event;
        return  e.what();
    }

    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
// For every input stream, we need to keep a little bit of runtime information.
// For example, for fault-tolerance purposes, we need to remember the last
// stable data tuple received and whether we got any tentative data after that.
//
// @param stream_name The name of the new input stream
// @param source Optionally, it is the upstream node sending us the data.
//               When this parameter is set, we only accept data from the given source
//
// The above parameters correspond to those of a CatalogSubscription
//
void DataPath::add_input_path(Name stream_name, InetAddress source)
{
//..............................................................................


    INFO << "Added input path for stream " << stream_name << " coming from " << source;

    // Locking because this data structure is also accessed by the EnqueueThread
    //
    locking (_input_paths_lock)
    {
        IOQueue  *ioqueue = lookup(_input_paths,stream_name);

        if (!ioqueue)
        {
            ioqueue = new IOQueue(stream_name);
            _input_paths[stream_name] = ioqueue;
        }
    }

    if (source)
    {   add_input_path_source(stream_name,source);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  DataPath::update_input_history(CatalogSubscription  &sub_to_update)
{
//..............................................................................


    Name stream_name = sub_to_update.get_subscription_stream_name();

    locking (_input_paths_lock)
    {
        IOQueue  *ioqueue = lookup(_input_paths,stream_name);

        if (ioqueue)
        {
            bool    tentative = ioqueue->tentative();
            string  last_insertion_tuple = ioqueue->get_last_insertion_tuple_as_string();
            sub_to_update.set_history(last_insertion_tuple,tentative);
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Add another endpoint (expected data source) for an input path.
//
void DataPath::add_input_path_source(Name stream_name, InetAddress new_source, InputType type)
{
//..............................................................................


    locking (_input_sources_lock)
    {
        // Yes, we do overwrite so one does not need to know the past to set the proper type
        INFO << "For stream " << stream_name  << " added/updated source " << new_source << " as " << type;
        _input_sources[stream_name][new_source] = type;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Remove an endpoint associated with an input path
// We use this for fault-tolerance when changing from one replica
// of an upstream neighbor to another replica of that neighbor
//
void  DataPath::remove_input_path_source(Name         stream_name,
                                         InetAddress  old_source)
{
//..............................................................................


    locking (_input_sources_lock)
    {
        InputSourcesPerStream::iterator i = _input_sources.find(stream_name);

        if (i != _input_sources.end())
        {
            i->second.erase(old_source);
            INFO << "For stream " << stream_name  << " removed source " << old_source;
        }
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
InetAddress  DataPath::get_input_path_source(Name       stream_name,
                                             InputType  type)
{
//..............................................................................


    locking (_input_sources_lock)
    {
        InputSourcesPerStream::iterator input_iter = _input_sources.find(stream_name);

        if (input_iter != _input_sources.end())
        {
            InputSources::iterator source_iter = input_iter->second.begin();

            for (; source_iter != input_iter->second.end(); ++source_iter)
            {
                if (source_iter->second == type)
                {   return  source_iter->first;
                }
            }
        }
    }

    return  InetAddress();
}



////////////////////////////////////////////////////////////////////////////////
//
void DataPath::remove_input_path(Name stream_name)
{
//..............................................................................


    locking (_input_paths_lock)
    {
        IOQueue* ioqueue = lookup(_input_paths,stream_name);

        if (ioqueue)
        {
            _input_paths.erase(stream_name);
            delete ioqueue;
        }
    }

    // Also remove all input sources associated with this input path
    //
    locking (_input_sources_lock)
    {   _input_sources.erase(stream_name);
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
bool DataPath::has_output_path(Name stream_name)
{
//..............................................................................


    locking(_output_paths_lock)
    {
        IOQueue* ioqueue = lookup(_output_paths,stream_name);

        if (ioqueue)
        {   return  true;
        }
        else
        {   return  false;
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// There is ONE output path per output stream
//
void  DataPath::add_output_path(Name  stream_name)
{
//..............................................................................


    // A stream CAN be both an input and output path
    locking(_output_paths_lock)
    {
        IOQueue* ioqueue = lookup(_output_paths,stream_name);

        if (!ioqueue)
        {
            ioqueue = new IOQueue(stream_name);
            _output_paths[stream_name] = ioqueue;

            try
            {   _deq_handle.subscribe(stream_name.as_string());
            }
            catch (AuroraException& e)
            {   ERROR << "Failed to open path from Aurora " << e.what();
            }
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void DataPath::remove_output_path(Name stream_name)
{
//..............................................................................


    INFO << "Removing output path start " << stream_name;

    locking(_output_paths_lock)
    {
        IOQueue* ioqueue = lookup(_output_paths,stream_name);

        if (ioqueue)
        {
            _output_paths.erase(stream_name);
            delete ioqueue;

            try
            {   _deq_handle.unsubscribe(to_string(stream_name));
            }
            catch (AuroraException& e)
            {   INFO << "Stream was already deleted so no need to unsubscribe";
            }
        }
        else
        {   ERROR << "Could not find: " << stream_name;
        }
    }

    // Remove all subscriptions to this output stream
    remove_all_clients(stream_name);
    INFO << "Removing output path end " << stream_name;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// CatalogSubscriptions should be small immutable data structures, so making a copy here
// If you change that, watch out for threads
//
void  DataPath::add_output_path_client(CatalogSubscription sub)
{
    Name         stream_name;
    InetAddress  endpoint;
    bool         is_stable_output;
//
//..............................................................................


    stream_name = sub.get_subscription_stream_name();
    endpoint    = sub.get_subscription_endpoint();

    // Checking condition here because the check requires a lock???
    //
    is_stable_output = stable_output(stream_name);

    locking(_output_clients_lock)
    {
        // If we already have this client in our catalog, then do nothing
        //
        if (_output_clients[stream_name].find(endpoint) !=
            _output_clients[stream_name].end())
        {
            WARN << "We already know about this client... returning";
            return;
        }

        // Using smart pointers as before because I don't want to change too many things at the same time
        //
        ptr<DataHandler::DHArgs>  args(new DataHandler::DHArgs(DataHandler::MAX_PER_SENDER_OUTPUT_QUEUE));

        ptr<DataHandler>  dh(new DataHandler(_fast_dequeue_fwd._data_fwd_loop,
                                             TCPSocket(endpoint,
                                                       Socket::nonblocking),
                                             args));

        if (!dh)
        {
            ERROR << "Failed connecting to " << to_string(endpoint);
            return;
        }

        dh->set_owned(true); // Do not delete self when connection closes

        // Create a new client with a new DataHandler
        bool no_history = !sub.is_valid_last_insertion_tuple();
        ClientInfo new_client(sub, dh, no_history);

        // Check if both the node and the client have seen a failure
        //
        //if ( ! stable_output(stream_name) ) // Careful, we are locking here
        if ( !is_stable_output )
        {
            INFO << "Added client while output is tentative";
            new_client._independent_failure = false;
        }

        _output_clients[stream_name][endpoint] = new_client;
        INFO << " Added client " << endpoint << " for stream " << stream_name << " with history " << sub.is_valid_last_insertion_tuple();
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
//
void DataPath::remove_output_path_client(CatalogSubscription sub)
{
//..............................................................................


    Name      stream_name = sub.get_subscription_stream_name();
    InetAddress  endpoint = sub.get_subscription_endpoint();

    locking(_output_clients_lock)
    {
        // First find the stream
        //
        ClientsPerStream::iterator stream_iter =  _output_clients.find(stream_name);

        if (stream_iter != _output_clients.end())
        {
            OneClientMap::iterator client_iter = stream_iter->second.find(endpoint);

            if (client_iter != stream_iter->second.end())
            {
                // Cannot delete it, deleting must be done on the same event loop as sending the data
                _failed.push_back(client_iter->second._data_handler);

                // Removing the output client from the list. The DataHandler will not go away if it's still being used
                stream_iter->second.erase(client_iter);
                INFO << " Removed client " << endpoint << " for stream " << stream_name;
            }
        }
    }

    // Schedule DataHandler cleaning task
    (new CallbackTimer(_fast_dequeue_fwd._data_fwd_loop, wrap(this,&DataPath::clean_failed_handlers)))->arm(Time::now() +  Time::msecs(CLEANUP_DELAY));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void DataPath::remove_all_clients(Name stream_name)
{
//..............................................................................


    locking(_output_clients_lock)
    {
        // First find the stream
        //
        ClientsPerStream::iterator  stream_iter = _output_clients.find(stream_name);

        if (stream_iter != _output_clients.end())
        {

            // Iterate through all clients and repare their handlers for deletions
            OneClientMap::iterator client_iter;
            for ( client_iter = stream_iter->second.begin(); client_iter != stream_iter->second.end(); ++client_iter )
            {
                // Deleting must be done on the same event loop as sending the data
                _failed.push_back(client_iter->second._data_handler);
            }

            // Remove the clients themselves
            _output_clients.erase(stream_iter);
        }

    }

    // Schedule DataHandler cleaning task
    (new CallbackTimer(_fast_dequeue_fwd._data_fwd_loop, wrap(this,&DataPath::clean_failed_handlers)))->arm(Time::now() +  Time::msecs(CLEANUP_DELAY));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void DataPath::add_local_output_path_client(Name stream_name, DataHandler::dhcallback cb)
{
//..............................................................................


    _local_clients[stream_name] = cb;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// True if event passes all the tests and can be processed.
//
Status  DataPath::fault_tolerance_checks(ptr<StreamEvent> event)
{
//..............................................................................


    InetAddress source = event->_source;

    // If there is no source information set, then we accept and skip all tests.
    //
    if (!source)
    {   return  true;
    }

    Name   stream_name = event->_stream;

    locking (_input_sources_lock)
    {
        DEBUG << "_input_sources=" << _input_sources;

        // Basic error checking first.
        //
        InputSourcesPerStream::iterator inputs_iter = _input_sources.find(event->_stream);

        if (inputs_iter == _input_sources.end() )
        {   return  string("Unknown input stream ? ") + to_string(stream_name);
        }

        InputSources::iterator source_iter = inputs_iter->second.find(source);

        if (source_iter == inputs_iter->second.end())
        {   return  string("Not an expected input source  ? ") + source;
        }

        // Is there a stream of background corrections? (could be optimized)
        //
        bool exists_corrections_stream = false;
        bool exists_corrected_stream = false;
        InputSources::iterator corr;

        for (corr = inputs_iter->second.begin(); corr != inputs_iter->second.end(); ++corr)
        {
            if (corr->second == InputType::CORRECTIONS)
            {   exists_corrections_stream = true;
            }
            if (corr->second == InputType::CORRECTED)
            {   exists_corrected_stream = true;
            }
        }


        // If source is a regular input and there are no background corrections, then all is great
        //if ((source_iter->second == InputType::REGULAR) && (!exists_corrections_stream) && (!exists_corrected_stream) )
        //{   return true;
        //}

        // Inspect the buffer for tuples of interests
        //
        const char*  buf      = event->_bin_tuples.data();
        size_t       buf_size = event->_inserted_count * event->_tuple_size;

        bool         exists_rec_done      = false;
        bool         exists_non_tentative = false;

        Timestamp current_time = Timestamp::now();

        for (const char* index = buf; index < buf + buf_size; index += event->_tuple_size)
        {
            Tuple t = Tuple::factory(index);

            if (t.get_tuple_type() == TupleType::RECONCILIATION_DONE)
            {
                exists_rec_done = true;
                INFO << "Got a REC_DONE tuple from " << source << " tuple is " << t.get_tuple_stime();
            }

            if (t.get_tuple_type() != TupleType::TENTATIVE)
            {   exists_non_tentative = true;
            }

            // for debugging
            if (t.get_tuple_type() == TupleType::UNDO)
            {   INFO << "Got an UNDO tuple from " << source << " tuple is " << t.get_tuple_stime();
            }
        }

        // XXX Should go above the scanning of the buffer.
        // If source is a regular input and there are no background corrections, then all is great
        //
        if ((source_iter->second == InputType::REGULAR) &&
            (!exists_corrections_stream) && (!exists_corrected_stream))
        {
            return  true;
        }

        // First, if source is a background correction source AND even contains a RECONCILIATION_DONE
        // then label this source as CORRECTED
        //
        if ((source_iter->second == InputType::CORRECTIONS) && (exists_rec_done))
        {
            INFO << "Corrections stream is now new regular input stream. Closing all other input streams";
            source_iter->second = InputType::CORRECTED;
        }

        // If source is regular input but there is a CORRECTED stream, then drop
        //
        if ((source_iter->second == InputType::REGULAR) &&
            (exists_rec_done || exists_corrected_stream))
        {
            INFO << "Background stream is corrected. No more duplicate streams";
            return "Background stream is corrected. No more duplicate streams";
        }

        // If source is a regular input and there is a background corrections stream that is still correcting data, then
        // the stream event should only contain TENTATIVE tuples
        //
        if ((source_iter->second == InputType::REGULAR)  &&
            (exists_non_tentative)                       &&
            (exists_corrections_stream)                  &&
            (!exists_corrected_stream))
        {
            INFO << "Regular stream contains non tentative tuples while we have a corrections stream";
            return "Regular stream contains non tentative tuples while we have a corrections stream";
        }

    }

    // Event passed all the tests
    //
    return  true;
}


BOREALIS_NAMESPACE_END

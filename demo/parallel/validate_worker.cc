#include  <queue>

#include  "WorkerMarshalCommon.h"

using namespace Borealis;

//#define  FRAME_WIDTH   320
//#define  FRAME_HEIGHT    1
//#define  FRAME_DEPTH     1

const uint32  SLEEP_TIME  = 100;           // Delay between injections.
const uint32  BATCH_SIZE  =   1;           // Number of input tuples per batch.
const uint32  BATCH_COUNT =  20;           // Number batches to send.

const Time  time0 = Time::now() - Time::msecs( 100 );

      WorkerMarshal   marshal;        // Client and I/O stream state.

      uint32  result = 0;
      uint32  total  = BATCH_COUNT * BATCH_SIZE;
      uint32  remain = BATCH_COUNT;

      uint8   expect  = 0;
      uint8   counter = 0;
      //uint8   value[FRAME_WIDTH / 2];  //[FRAME_HEIGHT][FRAME_DEPTH];

      
      queue<WorkerMarshal::tile>  work;


////////////////////////////////////////////////////////////////////////////////
//
// Filter function to transform an input tuple to an output tuple.
//
// Print the content of received tuples.
//
WorkerMarshal::tile  *WorkerMarshal::receivedScatter1(tile  *tuple)
{
    uint8   *array;
//
//  Note that in a worker; inputs and outputs are reversed.
//  A worker input is a borealis output and vice versa.
//
//  subscribeScatter1/set_data_handler(outputHandler)
//
//  DataPath::DequeueThread::run
//      // Construct a StreamEvent from the set of tuples read.
//      ptr<StreamEvent> event(new StreamEvent(stream_name));
//
//      // Deallocate any arrays on the output tuple.
//      // Copy the array data into the event.
//      // Modify array fields the buffer to contain the array size in bytes.
//      event->_bin_tuples = string((const char *)&buf, total_read * td.get_size());
//      _data_path._events_to_send.push_back(event);
//      _data_path._deq_pipe_write.write(constbuf("A",1));
//
//  DataPath::FastDequeueForwarder::ForwardHandler::ravail
//     _fast_dequeue_fwd.send_data((*i)->_stream, (*i));
//
//  DataPath::FastDequeueForwarder::send_data
//     dh->send_data(event);
//
//  DataHandler::send_data
//     oss << (*event);
//
//  DataHandler::incoming_data
//     ptr<StreamEvent> event(new StreamEvent());
//     total_read = input.pos();
//     Status stat = _args->cb(event);
//
//  WorkerMarshal::outputHandler(ptr<StreamEvent>  event)
//     WorkerMarshal::Scatter1Handler(ptr<StreamEvent>  event)
//        WorkerMarshal::receivedScatter1(tile  *tuple)
//
//     * Copy the event pointer and clear it.
//       The clear needs to propagate back up to incoming_data.
//       Could also copy it and increment the reference count.
//
//     * Lock and Enqueue batched tuples.
//       Has to return quickly to avoid input overflow.
//
//  Lock and dequeue tuples in a thread and process them.
//     Enqueue output tuples as is in an output event.
//     
//  needs to go fast, too.  It could block the input thread if slow.
//  Use another queue (defer for now).
//  WorkerMarshal::sentGather1
//     batchGather1(&gather);
//     sendGather1(SLEEP_TIME);
//  
//..............................................................................


    expect += 1;
    NOTICE << "expect=" << (uint32)expect << " value=" << (uint32)*(tuple->array);

    if ((uint32)*(tuple->array) != (uint32)expect)
    {   NOTICE << "receivedScatter1:  Failed:  Expected value is " << (uint32)expect;
        result = 2;
    }

    return  tuple;
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  WorkerMarshal::sentGather1()
{
    tile        *tuple;
    int32         index;
    uint32        offset = 0;
    uint32        size   = 0;
    uint8        *array;
    uint32        size_array;

    int32        timestamp;
    Time         current_time;
//..............................................................................


    if (marshal._active_event.empty())
    {
       (new CallbackTimer(_client->get_loop(),
                          wrap(this, &WorkerMarshal::sentGather1) ))
            ->arm(Time::now() + Time::msecs(SLEEP_TIME));
    }
    else
    {   current_time = Time::now();

        timestamp = (int32)( current_time - time0 ).to_secs();
        if ( timestamp < 0 ) timestamp = 0;
        DEBUG  << "timestamp = " << timestamp << "  current_time = " << current_time;

        ptr<StreamEvent>  event = marshal._active_event.front();

        // For each tuple that was received,
        //
        for (index = 0; index < event->_inserted_count; index++)
        {
            offset += HEADER_SIZE;

            // At the tuple data past the header.
            //
            tuple = (tile *)&event->_bin_tuples[offset];
            DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(tile));

            // At the array data in the event.
            //
            array = (uint8 *)&(event->_bin_arrays[size]);

            // Offset past the array data.
            //
            size_array = *(uint32 *)&tuple->array;
            size += size_array;
            DEBUG << "size=" << size << "  size_array=" << size_array;

            // Modify the array field to reference the array data.
            //
            tuple->array = array;

            //
            //receivedScatter1(tuple);

            Gather1  gather;
            //gather._data = *tuple;

            // Does a copy???
            //
            gather._data = *WorkerMarshal::receivedScatter1(tuple);
            batchGather1(&gather);

            offset += sizeof(tile);
        }

        marshal._active_event.pop();    
        counter += 1;

        NOTICE << "counter=" << (uint32)counter << " value=" << (uint32)*(tuple->array);

        if ((uint32)*(tuple->array) != (uint32)counter)
        {   NOTICE << "sentGather1:  Failed:  Expected value is " << (uint32)counter;
            result = 3;
        }

        DEBUG << "call sendGather1...";
        sendGather1(SLEEP_TIME);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
int  main(int  argc, const char  *argv[])
{
    uint16   instance;
    string   endpoint;    // Use port 25100 on the localhost.
//
//  Return:  1 - Could not deploy the network.
//           2 - An invalid tuple value was received.
//..............................................................................


    if (argc != 2)
    {   WARN << "USAGE:  validate_worker  <instance #>";
    }
    else
    {   instance = atoi(argv[1]);
        endpoint = Util::form_endpoint(":25100");
        NOTICE << "instance=" << instance << " at endpoint=" << endpoint;
    
        // Run the front-end, open a client, subscribe to outputs and inputs.
        marshal.openInputOutput(instance, endpoint);
        //result = marshal.open();  // launches XML; not needed.

        DEBUG  << "time0 = " << time0;

        // Use a constant array each tile.
        //for (uint32  i = 0; i < FRAME_WIDTH; i++)
        //{   value[i] = i;
        //}

        //{   for (uint32  j = 0; j < FRAME_HEIGHT; j++)
        //    {   for (uint32  k = 0; k < FRAME_DEPTH; k++)
        //        {   value[i][j][k] = k;
        //}   }   }

        // Establish an initial delay waiting for input.
        //
        marshal.sentGather1();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    NOTICE << "validate_worker:  Done";

    return  result;
}

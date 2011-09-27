#include "SweeperQBox.h"

BOREALIS_NAMESPACE_BEGIN

// Insert stuff here

void SweeperQBox::setup_impl() throw (AuroraException)
{
  // TODO: Set up the inputs, parameters, outputs, TupleDescriptions
  // Should be: 2 inputs (REQUEST, HISTORICAL)
  //            0 (real) outputs (or a dummy one?)
  //             -- Current QBox outputs are Streams, but for now we need to
  //                output to TupleQueues, so we'll have something separate
  //                to Sweeper
  // Where do we get the TupleQueues (i.e. _output) from?
  // Ditto on their TDs
  //   -- Don't actually need TDs, since we know the headers and the TQ
  //      itself knows the size of its own tuples
  //
  // SOOO....  All the more we need is the TQs. How do we get the TQs?
    
  if( get_num_inputs() != 2 )
    {
      Throw( aurora_typing_exception,
         "Sweeper requires exactly 2 inputs (not "
         + to_string(get_num_inputs()) + ")"       );
    }
}

void SweeperQBox::init_impl() throw (AuroraException)
{
  // nothing to initialize
}

void SweeperQBox::run_impl( QBoxInvocation &inv ) throw (AuroraException)
{
  // Do the standard QBox inv-controlled loop:
  //    If the tuple is TupleType::REQUEST:
  //       Push it onto the request deque
  //    If the tuple is TupleType::HISTORICAL:
  //       Push it onto the historical buffer
  //       Find a request that matches the historical (LIFO should do it)
  //       Put a (new) NOTIFICATION tuple on the TupleQueue of the box the 
  //         request came from
    
  DeqIterator di_req = deq(0);
  DeqIterator di_his = deq(1);

  TupleDescription req_desc(get_in_schema(0));
  TupleDescription his_desc(get_in_schema(1));
    

  // Where do we get the output TupleQueues from?
  // They're not streams, at least
  // Let's say that they're magically in a vector<TupleQueue>
        
  // Pull requests first.
  while( inv.continue_dequeue_on( di_req, 0 ) )
    {
      uint8 *data = new uint8[req_desc.get_size()];
      memcpy(data, di_req.tuple(), req_desc.get_size());  // memcpy tuples since need to store them
      Tuple tuple = Tuple::factory(data);

      INFO << "Sweeper " << get_name() << " (REQ) IN: " << TupleDescription(get_in_schema(0)).tuple_to_string(data);
      ////      DEBUG << getchar();

      // Ignore all tuples on this queue that are not REQUESTs
      if( tuple.get_tuple_type() == TupleType::REQUEST )
    {
      // Push the tuple onto the requests buffer
      DEBUG << "Storing tuple "; // << // What's REQ's TD?
      _req_buffer.push_front( data );
    }

      // Next!
      ++di_req;
    }

  // Done pulling requests. Pull historicals.
  while( inv.continue_dequeue_on( di_his, 1 ) )
    {
      uint8 *data = new  uint8[his_desc.get_size()];
      memcpy(data, di_his.tuple(), his_desc.get_size());  // memcpy tuples since need to store them
      Tuple tuple = Tuple::factory(data);

      INFO << "Sweeper " << get_name() << " (HIS) IN: " << TupleDescription(get_in_schema(1)).tuple_to_string(data);
      ////      DEBUG << getchar();

      // Ignore all tuples on this queue that are not HISTORICALs
      if( tuple.get_tuple_type() == TupleType::HISTORICAL )
    {
      DEBUG << "Looking for matching request";
      // Look for a matching request, and ignore if there isn't one
      list<uint8  *>::iterator i = match_request( data );
      if( i != _req_buffer.end() )
        {
          DEBUG << "Found match";

          Tuple request_tuple = Tuple::factory(*i);
          int output_to_box = request_tuple.get_revision_id();
          if( request_tuple.get_revised() != -1 )
        {
          // request.rev_field has the revision_id (box_id) requesting this historical
          // will be used to match in notify() method
          tuple.set_revision_id( request_tuple.get_revised() );
        }
                
          DEBUG << "Storing: " << TupleDescription(get_in_schema(1)).tuple_to_string(data);
          _his_buffer.push_back( data );
                
          // Create a NOTIFICATION and put it on the right queue
          // NOTIFICATION is TT:NOTIFICATION + RevID -- which means that
          // it can be represented by just a header. But
          // TupleQueue::enqueue_multiple needs blocks of the appropriate
          // tuple size. The best way to do that is...
          // Let's prod TupleQueue into responding to such a query.
          INFO << "Sending notification to boxid: " << output_to_box;
          unsigned int notification_tuple_size = _output[output_to_box]->get_tuple_size();
          uint8  new_tuple[notification_tuple_size];
          memset( new_tuple, '\0', notification_tuple_size );
          Tuple notification = Tuple::factory( new_tuple );

          // Set header fields. The only ones /I/ think are relevant are
          // tuple_type, tuple_id, and revision_id, but the others can be
          // assigned later.
          notification.set_tuple_type( TupleType::NOTIFICATION );
          notification.set_tuple_id( tuple.get_tuple_id() );
          notification.set_revision_id( tuple.get_revision_id() );
          notification.set_win_spec( tuple.get_win_spec() );

          // Enqueue onto the TupleQueue.
          vector<uint8 *> notification_wrapper = vector<uint8  *>( 1, new_tuple );
          _output[output_to_box]->enqueue_multiple( notification_wrapper );
          _output[output_to_box]->notify_enq();
                
          // Remove the request; it's been fulfilled
          _req_buffer.erase( i );
        }
    }

      // Next!
      ++di_his;
    }

  // And we're done.
}

list<uint8  *>::iterator SweeperQBox::match_request(uint8  *data)
{
  // Search _req_buffer for a request matching the historical data  
  Tuple historical_tuple = Tuple::factory(data);

  list<uint8 *>::iterator i;
  for( i = _req_buffer.begin(); i != _req_buffer.end(); ++i )
    {
      Tuple request_tuple = Tuple::factory(*i);
      // Match revision_id, tuples_id, win_spec
      if( request_tuple.get_tuple_id() == historical_tuple.get_tuple_id() &&
      request_tuple.get_revision_id() == historical_tuple.get_revision_id() &&
      request_tuple.get_win_spec() == historical_tuple.get_win_spec() )
    {
      return i;
    }
    }
    
  return i;
}

uint8  *SweeperQBox::notify( const void *message )
{
  // message /should/ be the notification tuple that the sweeper sent out
  //  -- if not: return NULL?
  // Look in the historical buffer for the matching tuple
  // Remove it from the buffer and return the data pointer
    
  // Due to the assumption that only one request is doing downstream at once
  // match on RevID will be sufficient.
    
  Tuple message_tuple = Tuple::factory(message);

  if( message_tuple.get_tuple_type() == TupleType::NOTIFICATION )
    {
      vector<uint8  *>::iterator i;
      for( i = _his_buffer.begin(); i != _his_buffer.end(); ++i )
    {
      Tuple historical_tuple = Tuple::factory(*i);
      DEBUG << "Considering " << TupleDescription(get_in_schema(1)).tuple_to_string(*i);

      if( historical_tuple.get_revision_id() == message_tuple.get_revision_id() &&
          historical_tuple.get_tuple_id() == message_tuple.get_tuple_id() &&
          historical_tuple.get_win_spec() == message_tuple.get_win_spec()
          )
        {
          uint8  *data = *i;
          _his_buffer.erase(i);
          INFO << "SweeperQBox::notify() " << get_name() << " returns: " << TupleDescription(get_in_schema(1)).tuple_to_string(data);
          ////              DEBUG << getchar();
          return data;
        }
    }

      // By now, there's known to be no match. Return the default.
    }

  // No other tuple_types supported as messages
  return NULL;
}

void SweeperQBox::set_output_queue( int i, ptr<TupleQueue> q )
{
  pair<int, ptr<TupleQueue> > p(i, q);
  _output.insert(p);
}

ptr<TupleQueue> SweeperQBox::request_queue()
{
  return get_input_tuple_queue(0);
}

ptr<TupleQueue> SweeperQBox::historical_queue()
{
  return get_input_tuple_queue(1);
}

// Make sure that request r is not in _req_buffer
// r is really a NOTIFICATION now
void SweeperQBox::remove_request(uint8  *r)
{
  int tid = Tuple::factory(r).get_tuple_id();
  int rid = Tuple::factory(r).get_revision_id();
  Tuple notification = Tuple::factory(r);
    
  for( list<uint8 *>::iterator i = _req_buffer.begin(); i != _req_buffer.end(); ++i )
    {
      Tuple request = Tuple::factory(*i);
      if( (request.get_win_spec() == tid && request.get_revised() == rid) ||
      (request.get_win_spec() == -1 && request.get_revised() == -1
       && request.get_tuple_id() == tid && request.get_revision_id() == rid) )
    {
      _req_buffer.erase(i);
      return;
    }
    }
}
    

BOREALIS_NAMESPACE_END

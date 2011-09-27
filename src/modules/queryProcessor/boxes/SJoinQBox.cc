#include "SJoinQBox.h"

BOREALIS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
//
void SJoinQBox::setup_impl() throw( AuroraException )
{
    JoinQBox::setup_impl();

    _leftjoin = True; 
    _rightjoin = True;

    string left_join_param;
    if ( typed_param( "leftjoin", left_join_param ) )
    { _leftjoin = ( left_join_param == "false" ) ? False : True;
    }

    string right_join_param;
    if ( typed_param( "rightjoin", right_join_param ) )
    { _rightjoin = ( right_join_param == "false" ) ? False : True;
    }

    WARN << "Join parameters are: ";
    if ( _leftjoin )
    { WARN << "Joining on the left";
    }
    if ( _rightjoin )
    { WARN << "Joining on the right";
    }

    _last_left = Timestamp();
    _last_right = Timestamp();
    _produce_rec_done = false;
    _rec_done_tuple = ptr<dynbuf>(new dynbuf(HEADER_SIZE));

}

///////////////////////////////////////////////////////////////////////////
//
void SJoinQBox::run_impl( QBoxInvocation  &inv )
    throw( AuroraException )
{
    bool outputted = false;

    run_streams_in_parallel(inv,outputted);

    // Done, notify if there was output
    if ( outputted )
    { get_output(0).notify_enq();
    }

}



///////////////////////////////////////////////////////////////////////////
//
void SJoinQBox::run_streams_in_parallel( QBoxInvocation &inv, bool &outputted )
{

    // XXX Needs more testing and tuning. XXX

    DeqIterator left_di = deq(LEFT);
    DeqIterator right_di = deq(RIGHT);
    
    bool more_on_left = inv.continue_dequeue_on(left_di,LEFT);
    bool more_on_right = inv.continue_dequeue_on(right_di,RIGHT);
    //bool more_on_left = left_di.avail();
    //bool more_on_right = left_di.avail();


    // There must be tuples on both inputs if we want to ensure that we process them by increasing tuple_stime values
    while ( more_on_left || more_on_right )
    {


        // Handle control tuples first
        if ( more_on_left && is_control_tuple(Tuple::factory(left_di.tuple())) )
        {
            process_control(inv,left_di,outputted,more_on_left,_last_left,LEFT);
            continue;
        }

        if ( more_on_right && is_control_tuple(Tuple::factory(right_di.tuple())) )
        {
            process_control(inv,right_di,outputted,more_on_right,_last_right,RIGHT);
            continue;
        }



        // DATA tuples: If we have data tuples on both inputs, then process by increasing tuple_stime values
        if ( more_on_left && more_on_right )
        {
            if ( Tuple::factory(left_di.tuple()).get_tuple_stime() < Tuple::factory(right_di.tuple()).get_tuple_stime())
            { process_data(inv,left_di,outputted,more_on_left,_last_left,LEFT,_leftjoin); 
            }

            else
            { process_data(inv,right_di,outputted,more_on_right,_last_right,RIGHT,_rightjoin); 
            }

        }

        // If we have DATA only one one input, then process only if it is safe to do so
        //else if ( more_on_left && 
        //        ( (_last_right == Timestamp()) || 
        //          ( Tuple::factory(left_di.tuple()).get_tuple_stime() < _last_right) ||
        //          ( Tuple::factory(left_di.tuple()).get_tuple_type() == TupleType::TENTATIVE ) ) )
        else if ( more_on_left )
        { process_data(inv,left_di,outputted,more_on_left,_last_left,LEFT,_leftjoin); 
        }

        //else if ( more_on_right && 
        //        ( (_last_left == Timestamp()) || 
        //          ( Tuple::factory(right_di.tuple()).get_tuple_stime() < _last_left) || 
        //          ( Tuple::factory(right_di.tuple()).get_tuple_stime() == _last_left) || 
        //          ( Tuple::factory(right_di.tuple()).get_tuple_type() == TupleType::TENTATIVE ) ) )
        else if ( more_on_right )
        { process_data(inv,right_di,outputted,more_on_right,_last_right,RIGHT,_rightjoin); 
        }

        // If cannot process remaining data, let go of CPU
        else 
        { break;
        }

    }

    // If there are no tuples available for processing, that means the queus are finally empty
    if ( (!left_di.avail() || !right_di.avail()) && _produce_rec_done )
    {
        _produce_rec_done = false;
        Timestamp rec_done_updated_timestamp = _last_left > _last_right ? _last_left : _last_right;
        Tuple::factory(_rec_done_tuple->data()).set_tuple_stime(rec_done_updated_timestamp);
        process_control_tuple(Tuple::factory(_rec_done_tuple->data()));
    }

}


///////////////////////////////////////////////////////////////////////////
// 
bool SJoinQBox::is_control_tuple(const Tuple& tuple)
{

    if ( (tuple.get_tuple_type() == TupleType::BOUNDARY) ||
         (tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE))
    {
        if ( tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE)
        { WARN << "Processing a REC_DONE inside join";
        }
        return true;
    }
    return false;

}

///////////////////////////////////////////////////////////////////////////
void
SJoinQBox::process_control(QBoxInvocation& inv, DeqIterator& di,                            
                           bool& outputted, bool& more_processing,                             
                           Timestamp& last_time, int stream_id)                            
{

    Tuple tuple = Tuple::factory(di.tuple());

    if ( tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE)
    {
        _produce_rec_done = true;
        Tuple::factory(_rec_done_tuple->data()).copy_header(tuple);
    }
    else
    { 
        process_control_tuple(tuple);
        outputted = true;
    }

    ++di;
    more_processing = inv.continue_dequeue_on(di,stream_id);
    //more_processing = di.avail();
    last_time = tuple.get_tuple_stime();

}


///////////////////////////////////////////////////////////////////////////
void
SJoinQBox::process_data(QBoxInvocation& inv, DeqIterator& di,                            
                        bool& outputted, bool& more_processing,                             
                        Timestamp& last_time, int stream_id, bool do_join)                            
{

    Tuple tuple = Tuple::factory(di.tuple());
    process_data_tuple(tuple,stream_id,outputted,do_join);
    ++di;
    more_processing = inv.continue_dequeue_on(di,stream_id);
    //more_processing = di.avail();
    last_time = tuple.get_tuple_stime();
}


///////////////////////////////////////////////////////////////////////////
void
SJoinQBox::process_control_tuple(const Tuple& tuple)
{
    EnqIterator ei = enq(0);
    Tuple output_tuple = Tuple::factory(ei.tuple());

    // Zero the whole output tuple
    output_tuple.clear_tuple(HEADER_SIZE + get_out_schema(0)->get_size());
    // Copy only the header
    output_tuple.copy_header(tuple);
    // Propagate the tuple on the output stream
    ++ei;
}


///////////////////////////////////////////////////////////////////////////
// 
void SJoinQBox::process_data_tuple(const Tuple& tuple, int stream_id, bool& outputted, Boolean do_join)
{

    int comparison_count, match_count; // -- tatbul

    // Now process the tuple
    int other_stream_id = (stream_id == LEFT) ? RIGHT : LEFT;

    if ( do_join )
    {
        comparison_count = match_count = 0; // -- tatbul
        _buffer[other_stream_id]->join(tuple.get_data(), _predicate.get(),
                                       outputted,
                                       &comparison_count, &match_count);
    }
    DEBUG << "Joined " << Tuple::factory(enq(0).tuple()).get_tuple_id() << " outputted " << outputted;
    
    _buffer[stream_id]->insert_tuple(tuple.get_data());
        
    // We may also need to modify other places in the SJoin code. -- tatbul
    //
    _buffer[stream_id]->back()->add_comparison_count(comparison_count);
    _buffer[stream_id]->back()->add_match_count(match_count);

    // We must avoid the selectivity vector from growing indefinitely.
    // This may happen when scheduler is not collecting the accumulated stats.
    // For now, let's simply clear the vector if it grows beyond a limit.
    // -- tatbul
    //
    if (_join_selectivity[LEFT].size() > SELECTIVITY_VECTOR_LIMIT)
    {
        _join_selectivity[LEFT].clear();
        _join_selectivity_sum[LEFT] = 0;
    }
    if (_join_selectivity[RIGHT].size() > SELECTIVITY_VECTOR_LIMIT)
    {
        _join_selectivity[RIGHT].clear();
        _join_selectivity_sum[RIGHT] = 0;
    }

}



///////////////////////////////////////////////////////////////////////////
void 
SJoinQBox::produce_dont_emit_tuple(EvalContext &ctxt)
{

    // Regular processing
    JoinQBox::produce_dont_emit_tuple(ctxt);

    // Check if output should be TENTATIVE
    // Output tentative when either input tuple is TENTATIVE
    EnqIterator ei = enq(0);
    const Tuple left = Tuple::factory(ctxt.getTuple(LEFT));
    const Tuple right = Tuple::factory(ctxt.getTuple(RIGHT));
    Tuple output = Tuple::factory(ei.tuple());

    if ( (left.get_tuple_type() == TupleType::TENTATIVE) || 
         (right.get_tuple_type() == TupleType::TENTATIVE))
    {
        output.set_tuple_type(TupleType::TENTATIVE);
    }
    
}

void SJoinQBox::unpack_state(ptr<AbstractBoxState> box_state)
{
    WARN << "Resetting last timetamp values";
    _last_left = Timestamp();
    _last_right = Timestamp();
    JoinQBox::unpack_state(box_state);
}


BOREALIS_NAMESPACE_END

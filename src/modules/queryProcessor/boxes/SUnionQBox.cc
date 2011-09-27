#include "SUnionQBox.h"

BOREALIS_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////
/// Mostly from Union because we cannot inherit from UnionQBox and SControlQBox
/// at the same time. SUnion can have either one output or multiple outputs
void
SUnionQBox::setup_impl() throw (AuroraException)
{
    _number_of_inputs = get_num_inputs();
    _number_of_data_outputs = get_num_outputs() - 1; // Don't count control output
    DEBUG << "Nb inputs " << _number_of_inputs << ", nb outputs " << _number_of_data_outputs;
   
    if ( (_number_of_data_outputs != 1) && (_number_of_data_outputs != _number_of_inputs))
    {
        FATAL << "Improper use of SUnion. SUnion must either have identical inputs and one output "
              << "or the same number of inputs as outputs (plus the control output)";
    }

    // Set the size of inputs. Will need that for the buckets
    for (unsigned int i = 0; i < get_num_inputs(); ++i)
    {
        _data_input_tuple_sizes.push_back(get_in_schema(i)->get_size() + HEADER_SIZE);
    }

    // set the first output stream's type to be the same as that of first input
    set_out_description(DATA_STREAM, get_in_stream(0));

    // If many outputs, then set their types to the corresponding inputs
    if ( _number_of_data_outputs > 1 )
    {
        for (unsigned int i = 1; i < get_num_inputs(); ++i)
        { set_out_description(DATA_STREAM+i, get_in_stream(i));
        }
    } 
    // Otherwise, check if types of all inputs are the same
    else
    {
        for (unsigned int i = 1; i < get_num_inputs(); ++i)
        {
            if (! get_in_schema(0)->equal_schema(get_in_schema(i)) )
            { 
                FATAL << "Improper use of SUnion. SUnion must either have identical inputs and one output "
                      << "or the same number of inputs as outputs (plus the control outputs";
            }
        }
    }

    // SUnion boxes have 2 outputs: one with data the second with control messages
    SControlQBox::setup_impl();
    setup_parameters();

}

///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::init_impl() throw (AuroraException)
{
    SControlQBox::init_impl();

    _current_interval_nb = INIT;
    _checkpointed_interval_nb = INIT;
    //_last_boundary_time = Timestamp::now();

    // Currently, we never sort tuples in bucket when single input (XXX should add a parameter for this)
    //_sort = false;

    // Restarting from a checkpoint doesn't work well with this feature
    // If we forward without waiting, we need to remember last stable tuple to produce correct UNDO
    //_last_tuple = ptr<dynbuf>(new dynbuf(_output_tuple_size[DATA_STREAM]));
    //memset(_last_tuple->data(),0,_output_tuple_size[DATA_STREAM]);

}

///////////////////////////////////////////////////////////////////////////
TupleDescription
SUnionQBox::create_control_output_description()
{
    vector<SchemaField> fields;
    return TupleDescription(fields.begin(), fields.end());

}

///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::setup_parameters()
{
    // Basic parameters.
    typed_param("interval", _interval, PARAM_NON_EMPTY);

    // Important! SUnions can appear on inputs or in the middle of a node
    // They must know if they are at the inputs
    string location; // Must be either "input" or "middle"
    typed_param("location", location, PARAM_NON_EMPTY);
    _is_input_sunion = true; // Default is input
    if ( location == "middle" )
    { _is_input_sunion = false;
    } 
    WARN << "Input Sunion is " << _is_input_sunion;

    // SUnion supports a max-delay specification PER INPUT STREAM
    for (unsigned int i = 0; i < get_num_inputs(); ++i)    
    {
        string max_delay_param = "max-delay." + to_string(i);
        typed_param(max_delay_param, _max_msec_delays[i], PARAM_NON_EMPTY);
        _actual_msec_delays[i] = _max_msec_delays[i] * MAX_ALPHA;
        WARN << "Setting actual delay to " << _actual_msec_delays[i];
    }

    int stagger_value = 1;
    typed_param("stagger", stagger_value, PARAM_NON_EMPTY);
    WARN << "Stagger value is " << stagger_value;

    // Stagger means yes, do DELAY all new tuples
    _stagger = (stagger_value == 1);
    WARN << "Should stagger is now " << _stagger;

    // For experiments, we force some nodes not to reconcile
    int reconcile_value = 1;
    typed_param("should-reconcile",reconcile_value);
    _should_reconcile =  (reconcile_value == 0) ? false : true;
    WARN << "Should reconcile is " << _should_reconcile;

    // What type of experiment are we running
    string benchmark_type;
    typed_param("benchmark",benchmark_type);
    _benchmark = MicroBenchmarkType(benchmark_type);
    WARN << "Benchmark type identified as " << _benchmark;

}

///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::pure_union_run_impl(QBoxInvocation& inv) throw (AuroraException)
{

    for (unsigned int i = 0; i < _number_of_inputs; ++i)
    {
        DeqIterator my_deq_iterator = deq(i);
        while (inv.continue_dequeue_on(my_deq_iterator, i))
        {

            Tuple tuple = Tuple::factory(my_deq_iterator.tuple());
            ++my_deq_iterator;

            // We can only run the pure union mode as a real union not in front of a join
            emit_tuple(tuple); 
        }
    }
    notify_outputs();

}


///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
  
    static int iteration = 0;
    iteration++;
    
    // At the moment, the time spent reconciling vs the time spent processing new input
    // tuples is fixed and the proportion depends on the 
    // TUPLE_PROCESSING_LIMIT 100  value set in src/modules/common.h and the 
    // values relating to the size of buckets and input rates and the constant below
    static int MAX_ITERATIONS_DURING_RECONCILIATION = 1;

    if ( !_benchmark.do_serialize())
    { pure_union_run_impl(inv);
    }

    else
    {
        // Read and store all input tuples
        for (unsigned int i = 0; i < _number_of_inputs; ++i) // For each stream to dequeue from...
        {
            DeqIterator my_deq_iterator = deq(i);
            while (inv.continue_dequeue_on(my_deq_iterator, i)) // while I can still dequeue tuples from it...
            {
                const Tuple tuple = Tuple::factory(my_deq_iterator.tuple());
                handle_input_tuple(tuple,i);
                ++my_deq_iterator;
            }

        }

        // Under normal operation, we process at most one bucket of data per burst of input
        // tuples.
        // During state reconciliation, we alter the proportion of time spent processing
        // old buckets of data versus accepting and processing new tuples. That's because the
        // node is receiving both corrections and new tentative data, so it may be receiving a significant
        // amount of input data. We want to move the focus onto processing old data.
        int max_iterations = 1;
        if ( _state.is_reconciling_forward() )
        { max_iterations = MAX_ITERATIONS_DURING_RECONCILIATION;
        }

        for ( int i = 0; i < max_iterations; i++ )
        { produce_output();
        }


        if ( _state.is_reconciling_forward() && ( _is_input_sunion ) )
        { check_for_end_of_reconciliation();
        }
        
        // XXX Debug: force scheduling of downstream operators. Pretend we always output tuples
        _output[DATA_STREAM] = true;

        // And now the normal notification
        notify_outputs();

        //if ( _state.is_reconciling_forward() )
        //{ WARN << "Running SUnion: done";
        //}
    }

  
}


///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::handle_input_tuple(const Tuple& tuple, int stream_id)
{

    // Read timestamp of the tuple
    Timestamp tuple_timestamp = tuple.get_tuple_stime();
    long long interval_nb = timestamp_to_interval_nb(tuple_timestamp);

    // First check if tuple is a control tuple:
    TupleType tuple_type = tuple.get_tuple_type();

    if ( tuple_type == TupleType::UNDO_REDO_START ) // Launch reconciliation
    {
        INFO << "Should undo and redo since tuple " << tuple_timestamp << " or interval " << interval_nb;

        // Reset the delays in preperation of the next failure
        for (unsigned int i = 0; i < get_num_inputs(); ++i)    
        { _actual_msec_delays[i] = _max_msec_delays[i] * MAX_ALPHA;
        }

        undo_redo(interval_nb);
    }
    else if ( tuple_type == TupleType::UNDO ) // Start correcting input streams
    {
        WARN << "Got UNDO inside SUnion";
        _state.start_overwrite_tentatives();
        // If we got an UNDO tuple then we expect to get a REC_DONE later on.
        _expect_rec_done[stream_id] = true;
    }
    else if ( tuple_type == TupleType::RECONCILIATION_DONE ) // Done correcting input streams (received from upstream)
    {
        WARN << "Got REC_DONE inside SUnion";

        _state.stop_overwrite_tentatives();

        // All TENTATIVE tuples received before a RECONCILIATION_DONE belong to the old failure
        // So remove any remaining tentative tuples. They have now all been corrected
        clear_all_tentative_tuples();

        process_rec_done(tuple,stream_id);

    }
    else
    { insert_tuple_into_bucket(tuple, stream_id, interval_nb);
    }

}

///////////////////////////////////////////////////////////////////////////
// WARNING! This technique works only for CHECKPOINT/REDO
// We basically wait to receive a REC_DONE tuple on all input streams
void
SUnionQBox::process_rec_done(const Tuple& tuple, int stream_id)
{

    Timestamp rec_done_timestamp = tuple.get_tuple_stime();
    if ( _input_rec_done.size() == 0 )
    { _max_input_rec_done_timestamp = rec_done_timestamp;
    } 
    else if ( rec_done_timestamp > _max_input_rec_done_timestamp )
    { _max_input_rec_done_timestamp = rec_done_timestamp;
    }
    
    _input_rec_done[stream_id] = rec_done_timestamp;

    WARN << "Received REC_DONE " << rec_done_timestamp << " on stream id " << stream_id
         << " expected to receive " << _expect_rec_done.size() << " and now we already have " << _input_rec_done.size();

    // If this SUnion is not on the input, then we will propagate the RECONCILIATION_DONE tuple
    // once we get a REC_DONE on every input stream
    if ( ( !_is_input_sunion ) && ( _input_rec_done.size() == _number_of_inputs) )             
    {
        INFO << "Forwarding the REC_DONE tuple " << stream_id;
        emit_tuple(tuple);
        _input_rec_done.clear();

        // That means we are now reconciled (well, SUnions in the middle should wait to clear their queues after receiving all REC_DONE)
        _state.reconciled();
    }

}



///////////////////////////////////////////////////////////////////////////
// void
// SUnionQBox::apply_undo(const Tuple& tuple, int stream_id, long long interval_nb)
// {

//     // Change state to signal that we are in the process of undoing and redoing buffer content
//     _state.undoing();

//     int32 undo_tuple_id = tuple.get_tuple_id();

//     INFO << "Should apply undo starting from ID " << undo_tuple_id << " and interval " << interval_nb;

//     // Update tuples in the buffer of the correct bucket
//     ptr<Bucket> bucket = lookup(_buckets,interval_nb);
//     if ( bucket )
//     {
//         INFO << "Clearing bucket " << interval_nb << " after tuple " << undo_tuple_id;
//         // Delete all tuples following the given one
//         bucket->clear_stream_after(stream_id,undo_tuple_id);
//     }

//     // All following buckets can be cleared
//     long long temp_interval_nb = interval_nb + 1;
//     bucket = lookup(_buckets,temp_interval_nb);
//     while ( bucket )
//     {
//         INFO << "Undoing bucket " << temp_interval_nb << " and stream " << stream_id;
//         bucket->clear_stream(stream_id);
//         temp_interval_nb++;
//         bucket = lookup(_buckets,temp_interval_nb);
//     }

//     if ( _should_reconcile )
//     { _current_interval_nb = interval_nb;
//     }
//     INFO << "Current interval reset back to " << _current_interval_nb;

// }


///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::undo_redo(long long interval_nb)
{

    // First produce the UNDO tuples.
    // It is the last tuple that was stable before the failure
    interval_nb = interval_nb - 1;
    ptr<Bucket> bucket = lookup(_buckets,interval_nb);

    // Find last data tuple in this one or a preceeding bucket
    bool found = false;
    while ( (bucket) && (!found) )
    {
        if ( !bucket->is_empty())
        {
            INFO << "Undoing tuple " << bucket->get_last_tuple().get_tuple_id() << " from interval " << interval_nb;
            emit_undo_tuple(bucket->get_last_tuple()); // Size doesn't matter. We only care about the header info
            found = true;
        }
        else
        {
            INFO << "Empty interval " << interval_nb << " so going to next";
            interval_nb--;
            bucket = lookup(_buckets,interval_nb);
        }
    }

    if ( !found )
    {
        //if ( forward_without_waiting())
        //{
        //  Tuple t = Tuple::factory(_last_tuple->data());
        //  emit_undo_tuple(t);
        //  interval_nb = timestamp_to_interval_nb(t.get_tuple_stime());
        //  INFO << "Using the last stable tuple that we saved for the undo id "
        //       << t.get_tuple_id() << " interval " << interval_nb;
        //  // Culd have become tentative any time during that same interval so must continue since then
        //  interval_nb--;
        //} else
        // {
        ERROR << "Don't know what to produce as UNDO tuple";
        //}
    }

    // Now produce the redo tuples
    _state.reconciling_rollforward();
    _current_interval_nb = interval_nb+1;

}

// --------------------------------------------------
void
SUnionQBox::clear_all_tentative_tuples()
{

    Buckets::iterator i;
    for ( i = _buckets.begin(); i != _buckets.end(); ++i )
    {
        ptr<Bucket> bucket = i->second;
        if ( bucket )
        { bucket->clear_all_tentative_tuples();
        }
    }       
}

///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::insert_tuple_into_bucket(const Tuple& tuple, int stream_id, long long interval_nb)
{

    // If this is the first tuple ever, then we set the current interval
    if ( _current_interval_nb == INIT )
    { _current_interval_nb = interval_nb;
    }

    // Check if we are not skipping over some buckets
    long long temp_interval = _current_interval_nb;
    //while ( temp_interval < interval_nb )    
    while ( temp_interval <= interval_nb )  // DIRECTLY CREATE A BUCKET EVEN FOR LATEST BOUNDARY RECEIVED
    {
        ptr<Bucket> temp_bucket = lookup(_buckets,temp_interval);
        if ( ! temp_bucket )
        {
            DEBUG << "Instantiating bucket for interval " << temp_interval << " at time " << Timestamp::now();
            temp_bucket = ptr<Bucket>(new Bucket(_number_of_inputs,_data_input_tuple_sizes));
            temp_bucket->set_timer(stream_id); // From expiration purposes, it's as if we got a tuple on this stream
            _buckets[temp_interval] = temp_bucket;
        }
        temp_interval++;
    }

    TupleType tuple_type = tuple.get_tuple_type();

    // If boundary, lookup previous intervals and bound the appropriate stream. Drop from input
    if ( tuple_type == TupleType::BOUNDARY )
    {
        long long interval_to_bound = interval_nb - 1;
        //INFO << "Got boundary for interval " << interval_to_bound;
        ptr<Bucket> bound_bucket = lookup(_buckets,interval_to_bound);
        while ( bound_bucket )
        {
            if ( bound_bucket->is_bounded(stream_id) )
            { break;
            }
            DEBUG << _is_input_sunion << " bounded bucket interval " << interval_to_bound << " on stream " << stream_id;
            bound_bucket->bound(stream_id);
            interval_to_bound--;
            bound_bucket = lookup(_buckets,interval_to_bound);
        }
    }
    else
    {
        // Move above
        // Get the right bucket. Exists because was created above if necessary
        ptr<Bucket> bucket = lookup(_buckets,interval_nb);
        if ( !bucket )
        {
            //FATAL << "Bucket should exist by now interval for this tuple " << interval_nb << " current interval " << _current_interval_nb;
            WARN << "Bucket should already exist for interval " << interval_nb;
            bucket = ptr<Bucket>(new Bucket(_number_of_inputs,_data_input_tuple_sizes));
            bucket->set_timer(stream_id);
            _buckets[interval_nb] = bucket;
        }

        // Forward without waiting but only stable tuples
        //if ( (forward_without_waiting()) && (_state.is_stable() ) && (tuple_type == TupleType::INSERTION))
        //{
        //  DEBUG << "Wait-free processing... Forwarding tuples for bucket: " << interval_nb;
        // Forwarding without waiting works only when there is a single input and a single output so
        // it is safe to forward on the first data stream
        //emit_tuple(tuple);
        //save_last_stable(tuple); // Always save last stable tuple so we can create a proper UNDO tuple
        //}
        //else // Add the tuple to the bucket only if not emitted
        // {
        DEBUG << "Inserting the tuple";
        if ( _state.get_overwrite_tentatives())
        { 
            bucket->insert_tuple_overwriting(stream_id,tuple);
        } else
        {
            bucket->insert_tuple(stream_id,tuple);
        }
        //}
    }
}

///////////////////////////////////////////////////////////////////////////
// Only used when there is one output
void
SUnionQBox::save_last_stable(const Tuple& src_tuple)
{
    char* tuple_start = _last_tuple->data();
    Tuple dst_tuple = Tuple::factory(tuple_start);
    dst_tuple.copy_tuple(src_tuple,_output_tuple_size[DATA_STREAM]);
}

///////////////////////////////////////////////////////////////////////////
//bool
//SUnionQBox::forward_without_waiting()
//{
//  return (( _number_of_inputs == 1 ) && (!_sort) );
//}


///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::produce_output()
{

    // IF we are in tentative state, our first priority is to check if we can make stable progress
    // So we go back and look at the current state of the first tentative bucket
    if ( _state.is_tentative() && _should_reconcile )
    {
        long long first_tentative_interval_nb = _state.first_tentative_interval();
        ptr<Bucket> first_tentative = lookup(_buckets,first_tentative_interval_nb);
        if ( (first_tentative) && (!first_tentative->is_tentative()) &&  (first_tentative->is_bounded()))
        {
            INFO << "First tentative bucket is now stable";
            process_output_stable_bucket(*first_tentative,first_tentative_interval_nb);
        }
    }

    // Now let's see what to do with the most recent buckets
    ptr<Bucket> current_bucket = lookup(_buckets,_current_interval_nb);
    if (current_bucket)
    {
        long long interval_nb = _current_interval_nb;
        //if ( current_bucket->is_empty())
        //{ DEBUG << "Processing empty bucket at interval " << interval_nb;
        //} else
        //{ DEBUG << "Processing bucket at interval " << interval_nb << " where last tuple is "
        //     << current_bucket->get_last_tuple().get_tuple_id();
        //}

        // Independently of the above, check what to do with the most recent bucket
        if ( (!current_bucket->is_tentative()) && (current_bucket->is_bounded()) )
        {
            if ( process_output_stable_bucket(*current_bucket,_current_interval_nb) )
            { _current_interval_nb++;
            }
        }

        // Check if need to proceed with tentative data
        else if ( (_state.can_process_tentative() || (!_should_reconcile)) && (!_benchmark.do_block()))
        {
            if ( process_output_un_stable_bucket(*current_bucket,_current_interval_nb) )
            { _current_interval_nb++;
            }
        }

        // ELSE CHECK IF A FAILURE IS OCCURING DURING RECOVERY
        // IF we are rolling forward and a bucket has expired, then we have failure during recovery
        // XXXXX Current implementation supports failures during recovery only when running a single node
        else if ( _state.is_reconciling_forward() && current_bucket->is_expired(_actual_msec_delays) && (_expect_rec_done.size() == 0))
        {
            WARN << "Failure during recovery at bucket: " << _current_interval_nb;

            // Signal the end of reconciliation but do not change state
            // The state will change when the checkpoint will occur.             
            _state.reconciling_interrupted();
            emit_control_tuple(DATA_STREAM,TupleType::RECONCILIATION_DONE,interval_nb_to_timestamp(_current_interval_nb));
            emit_control_tuple(CONTROL_STREAM,TupleType::RECONCILIATION_DONE,interval_nb_to_timestamp(_current_interval_nb));
        }
        

        // If processed the bucket, check if there's more stuff to do
        if ( interval_nb != _current_interval_nb)
        {
            // If next bucket appears ready to go, then ask to be re-scheduled ASAP
            ptr<Bucket> next_bucket = lookup(_buckets,_current_interval_nb);

            // Only continue processing if next bucket is bounded or is also already expired
            if ( next_bucket &&
                 ( ( (!next_bucket->is_tentative()) && (next_bucket->is_bounded()) ) ||
                   ( _state.can_process_tentative() && next_bucket->is_expired(_actual_msec_delays)) ) )
            {
                DEBUG << "Need more processing for interval " << _current_interval_nb;
                more_processing();
            }
        }

    }
 
    DEBUG << "Completed iteration... exiting produceOutput";
}



///////////////////////////////////////////////////////////////////////////
/// We declare that we have reconciled after we have drained our queues
/// We have drained our queues when the most recent buckets are no longer delayed
void 
SUnionQBox::check_for_end_of_reconciliation() {

    //Timestamp interval_start_time = interval_nb_to_timestamp(_current_interval_nb);
    ptr<Bucket> current_bucket = lookup(_buckets,_current_interval_nb);
    if ( ! current_bucket )
    { return;
    }

    Timestamp bucket_first_tuple_time = current_bucket->get_first_tuple_time();

    // Debug info
    static int debug_counter = 0;
    debug_counter++;
    if ( (debug_counter % 20) == 0 )
    {
        WARN << "Current interval nb is " << _current_interval_nb;
        WARN << "Bucket first tuple time is " << bucket_first_tuple_time;
        WARN << "Running behind by " << (Time::now() - bucket_first_tuple_time).to_msecs() << " msecs";
        WARN << "Empty bucket " << current_bucket->is_empty();
    }

    // If the input is arriving on time (upstream neighbors are no longer delayed themeselves)
    // If the current bucket is not much delayed anymore, then we are done
    // XXX Instead of SMALL_DELAY this should be some kind of MIN_DELAY per input stream
    if ( (_input_rec_done.size() == _expect_rec_done.size()) &&
         ( (Time::now() - bucket_first_tuple_time).to_msecs() < SMALL_DELAY ) &&
         ( ( _input_rec_done.size() == 0 ) ||           
           ( interval_nb_to_timestamp(_current_interval_nb) > _max_input_rec_done_timestamp) ) )
    {

        _input_rec_done.clear();
        _expect_rec_done.clear();

        WARN << "Drained our queues so emiting flow tuple";
        _state.reconciled();
            
        // No, let's do that as soon as we start rolling forward
        // Reset the delays in preperation of the next failure
        //for (unsigned int i = 0; i < get_num_inputs(); ++i)    
        //{ _actual_msec_delays[i] = _max_msec_delays[i] * MAX_ALPHA;
        //}

        // NO!! The whole check_for_end_of_reconciliation will not get invoked unless the SUnion is on the input
        // Only INPUT sunions indicate they are done reconciling
        //if ( _is_input_sunion )
        //{ 
        WARN << "Producing a REC_DONE for interval " << _current_interval_nb 
             << " so stime " << interval_nb_to_timestamp(_current_interval_nb);
        emit_control_tuple(DATA_STREAM,TupleType::RECONCILIATION_DONE,interval_nb_to_timestamp(_current_interval_nb));
        emit_control_tuple(CONTROL_STREAM,TupleType::RECONCILIATION_DONE,interval_nb_to_timestamp(_current_interval_nb));
        //WARN << "Emitted rec_done at time " << Timestamp::now();
        //}
    }
}

///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::emit_tuples(Bucket& bucket, TupleType::Type tuple_type) {

    // Beware. One output is for control. Don't count it
    if ( _number_of_data_outputs == 1 ) 
    { bucket.emit_tuples(enq(DATA_STREAM),tuple_type);
    }

    else 
    {
        vector<Stream::EnqIterator> outs;

        for ( unsigned int i = 0; i < _number_of_data_outputs; i++)
        { outs.push_back(enq(DATA_STREAM+i));
        }
        bucket.emit_tuples_on_separate_outputs(outs,tuple_type);
    }
}


///////////////////////////////////////////////////////////////////////////
bool
SUnionQBox::process_output_stable_bucket(Bucket& bucket, long long interval_nb)
{

    DEBUG << "Checking if should process as stable bucket. State is ";

    bool processed_bucket = false;
    _state.new_stable_bucket(); // Update our state given that we have a stable bucket

    // Normal operation, state is stable
    if ( _state.is_stable() || _state.is_reconciling_forward())
    {
        emit_tuples(bucket, TupleType::INSERTION);
        _output[DATA_STREAM] = true;

        if ( _benchmark.do_boundary() )
        { 
            // Timestamp of boundary must be the higher end of the bucket
            produce_boundary(interval_nb_to_timestamp(interval_nb+1));
            //produce_boundary(interval_nb_to_timestamp(interval_nb));
        }

        // SUnions on input streams must keep tuples that accumulate between checkpoints
        // In all other cases, we can delete the buckets that get processed
        // but make sure to leave at least one bucket with some tuples inside
        if ( !_benchmark.do_checkpoint() || !_is_input_sunion)
        { garbage_collect(interval_nb);
        }

        processed_bucket = true;
    }

    // State just became stable after period of unstability: request reconciliation
    // ALL Sunion signal when they are ready to reconcile. Requestion continuously
    // untill reconciliation gets started
    else if ( _state.something_to_correct() && _should_reconcile )
    {
        DEBUG << "Should reconcile";
        //_state.reconciling(); NO. DONT CHANGE STATE YET
        TupleType correction_request = TupleType::RECONCILIATION_REQUEST;
        Timestamp correction_start = _state.correct_since_when();
        emit_control_tuple(CONTROL_STREAM,correction_request,correction_start);

        // And continue processing as unstable while waiting to start reconciliation
        if ( interval_nb == _current_interval_nb )
        { processed_bucket = process_output_un_stable_bucket(bucket,interval_nb);
        }
    }

    // If we should never reconcile (used in experiments), then we process as tentative forever
    else if ( !_should_reconcile )
    {
        DEBUG << "Remaining in tentative state";
        processed_bucket = process_output_un_stable_bucket(bucket,interval_nb);
        DEBUG << "Processing result is " << processed_bucket;
    }

    return processed_bucket;

}

///////////////////////////////////////////////////////////////////////////
bool
SUnionQBox::process_output_un_stable_bucket(Bucket& bucket, long long interval_nb)
{

    DEBUG << "Checking if should process as tentative bucket";

    bool processed_bucket = false;

    Timestamp interval_start_time = interval_nb_to_timestamp(interval_nb);

    // If tuples have been buffered for too long
    if ( bucket.is_expired(_actual_msec_delays) )
    {
        DEBUG << "Need to proceed with tentative data at interval " << interval_nb;
        // XXX DEbug
        Timestamp bucket_first_tuple_time = bucket.get_first_tuple_time();
        DEBUG << "Because late by: " << (Time::now() - bucket_first_tuple_time).to_msecs() << " msecs";
        // XXX

        emit_tuples(bucket, TupleType::TENTATIVE);
        _output[DATA_STREAM] = true;

        _state.processing_tentative_bucket(interval_start_time,interval_nb);

        // ALL Sunions signal when they go into up_failure mode. Stop signalling after requesting reconciliation
        if ( ! _state.something_to_correct() )
        { emit_control_tuple(CONTROL_STREAM,TupleType::UP_FAILURE,_state.correct_since_when());
        }
        processed_bucket = true;

        // *** Once we have waited for max_delay, we reduce this delay to some small value so we don't delay further
        if ( !_stagger )
        {
            DEBUG << "Stagger is false";
            for (unsigned int i = 0; i < get_num_inputs(); ++i)    
            {
                //_actual_msec_delays[i] = _max_msec_delays[i] * MIN_ALPHA;
                _actual_msec_delays[i] = MIN_ALPHA;
                DEBUG << "Setting actual delay to " << _actual_msec_delays[i];
            }
        }
    }
    return processed_bucket;
}


///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::produce_boundary(Timestamp boundary_time)
{

    // No! We want to generate boundary tuples after every bucket. 
    //Timestamp now = Timestamp::now();
    //if ( (now - _last_boundary_time).to_msecs() > _interval )
    //{
    //  _last_boundary_time = now;    
    emit_control_tuple(DATA_STREAM,TupleType::BOUNDARY,boundary_time);
    if ( _is_input_sunion )
    { emit_control_tuple(CONTROL_STREAM,TupleType::BOUNDARY,boundary_time);
    }
    //}
}


///////////////////////////////////////////////////////////////////////////
long long
SUnionQBox::timestamp_to_interval_nb(Timestamp timestamp)
{
    return (timestamp.to_msecs() / _interval);
}

///////////////////////////////////////////////////////////////////////////
Timestamp
SUnionQBox::interval_nb_to_timestamp(long long interval_nb)
{

    long long current_interval_start = interval_nb * _interval;
    long long current_interval_start_usec = current_interval_start*THOUSAND;
    long sec = current_interval_start_usec/MILLION;
    long usec = current_interval_start_usec - (sec*MILLION);
    return Timestamp(sec,usec);

}

///////////////////////////////////////////////////////////////////////////
// Erase all stable buckets but leave one bucket with
// some tuples inside
void
SUnionQBox::garbage_collect(long long interval_nb)
{
    ptr<Bucket> bucket = lookup(_buckets,interval_nb);

    // Find first non-empty stable bucket, except if we forward tuples as they arrive
    //while ( bucket && (!forward_without_waiting()) && (bucket->is_empty() || bucket->is_tentative()))
    while ( bucket && (bucket->is_empty() || bucket->is_tentative()))   
    {
        interval_nb--;
        bucket = lookup(_buckets,interval_nb);
    }

    // Skip over the one non-empty bucket
    interval_nb--;
    bucket = lookup(_buckets,interval_nb);

    while (bucket)
    {
        _buckets.erase(interval_nb);
        interval_nb--;
        bucket = lookup(_buckets,interval_nb);
    }

}


///////////////////////////////////////////////////////////////////////////
bool SUnionQBox::State::can_process_tentative()
{
    return ( (_state_value == State::STABLE_STATE) ||
             (_state_value == State::TENTATIVE_STATE) ||
             (_state_value == State::CONVERGED_STATE)
           );
}


///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::processing_tentative_bucket(Timestamp tentative_start, long long interval_nb)
{

    if ( _state_value == State::STABLE_STATE )
    {
        WARN << "State was previously stable so going to tentative... setting start interval to " << tentative_start
             << " which is nb " << interval_nb;
        _tentative_start = tentative_start;
        _tentative_interval_nb = interval_nb;
        _state_value = State::TENTATIVE_STATE;
    }
    else if ( _state_value == State::TENTATIVE_STATE)
    {
        DEBUG << "ok. State is already tentative";
    }
    else
    {
        DEBUG << "Illegal state transition from " << _state_value << " to TENTATIVE_STATE";
    }

}

///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::undoing()
{
    if ( (_state_value == State::TENTATIVE_STATE) )
    {
        _state_value = State::UNDONE_STATE;
    } else
    {
        ERROR << "Invalid state transition from " << _state_value << " to UNDONE";
    }
}

///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::new_stable_bucket()
{

    if ( ( _state_value == State::TENTATIVE_STATE) || ( _state_value == State::UNDONE_STATE ) )
    {
        INFO << "State was previously tentative or undo so it is now converged";
        _state_value = State::CONVERGED_STATE;
    } else if ( ( _state_value == State::STABLE_STATE) ||
                ( _state_value == State::CONVERGED_STATE) ||
                //                ( _state_value == State::RECONCILING_STATE_ROLLBACK) ||
                ( _state_value == State::RECONCILING_STATE_ROLLFORWARD) )
    {
        DEBUG << "State is already STABLE or CONVERGED or RECONCILING. Actually it is " << _state_value;
    } else
    {
        DEBUG << "Illegal state transision. Cannot process stable buckets in state " << _state_value;
    }

}


///////////////////////////////////////////////////////////////////////////
bool SUnionQBox::State::something_to_correct()
{
    return (_state_value == State::CONVERGED_STATE);
}

///////////////////////////////////////////////////////////////////////////
Timestamp SUnionQBox::State::correct_since_when()
{
    return _tentative_start;
}


///////////////////////////////////////////////////////////////////////////
long long SUnionQBox::State::first_tentative_interval()
{
    return _tentative_interval_nb;
}

///////////////////////////////////////////////////////////////////////////
//void SUnionQBox::State::reconciling()
//{
//  if ( ( _state_value == State::CONVERGED_STATE) )
//  {
//      _state_value = State::RECONCILING_STATE_ROLLBACK;
//  } else
//  {
//      ERROR << "Invalid state transition from " << _state_value << " to reconciling";
//  }
//}

///////////////////////////////////////////////////////////////////////////
//bool SUnionQBox::State::is_reconciling()
//{
//  return (_state_value == State::RECONCILING_STATE_ROLLBACK);
//}

///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::reconciling_rollforward()
{

    // Either after an UNDO request or forced by a checkpoint/redo
    _state_value = State::RECONCILING_STATE_ROLLFORWARD;

}

///////////////////////////////////////////////////////////////////////////
bool SUnionQBox::State::is_reconciling_forward()
{
    return (_state_value == State::RECONCILING_STATE_ROLLFORWARD);
}

///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::reconciling_interrupted()
{

    // Either after an UNDO request or forced by a checkpoint/redo
    _state_value = State::RECONCILING_STATE_INTERRUPTED;

}

///////////////////////////////////////////////////////////////////////////
bool SUnionQBox::State::is_reconciling_interrupted()
{
    return (_state_value == State::RECONCILING_STATE_INTERRUPTED);
}


///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::reconciled()
{
    if  ( _state_value == State::RECONCILING_STATE_ROLLFORWARD)
    {
        _state_value = State::STABLE_STATE;
    } else
    {
        ERROR << "Invalid state transition from " << _state_value << " back to STABLE_STATE ";
    }
}

///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::no_reconciliation()
{
    if  ( _state_value != State::STABLE_STATE)
    {
        _state_value = State::TENTATIVE_STATE;
    }
}


///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::start_overwrite_tentatives()
{
    _overwrite_tentatives = true;
}

///////////////////////////////////////////////////////////////////////////
void SUnionQBox::State::stop_overwrite_tentatives()
{
    _overwrite_tentatives = false;
}

///////////////////////////////////////////////////////////////////////////
bool SUnionQBox::State::get_overwrite_tentatives()
{
    return _overwrite_tentatives;
}


///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    _pending_box_state = packed_box.dynamic_cast_to<SUnionBoxState>();
}

///////////////////////////////////////////////////////////////////////////
bool SUnionQBox::is_stable_for_checkpoint()
{
    return (_state.is_stable() || _state.is_reconciling_interrupted());
    //return !_state.is_tentative();
}

///////////////////////////////////////////////////////////////////////////
bool SUnionQBox::do_unpack_queues()
{
    // Unpack queues only for SUnions that are NOT on input streams. Do not touch 
    // these queues otherwise
    return  (!_is_input_sunion );
}



///////////////////////////////////////////////////////////////////////////
//ptr<AbstractBoxState> SUnionQBox::pack_state()
ptr<AbstractBoxState> 
SUnionQBox::pack_state(bool delta_only)
{

    // If we were reconciling and another failure occurred at the same time, we
    // reset our state to STABLE after the first checkpoint
    if ( _state.is_reconciling_interrupted() )
    { _state.reset();
    }

    // We can see a checkpoint either in STABLE state or in RECONCILE_ROLLFORWARD state
    // The latter case means we have a failure during recovery, so we just claim that
    // we have recovered and go straihgt back into UP_FAILURE

    DEBUG << "Checkpointed interval " << _current_interval_nb;
    _checkpointed_interval_nb = _current_interval_nb;

    if ( ! is_stable_for_checkpoint() )
    {
        INFO << "State is already tentative so checkpointint should have been canceled";
    }

    // Erase buckets preceding the checkpoint (leave one bucket with some tuples in it)
    long long interval_nb = _checkpointed_interval_nb-1;
    garbage_collect(interval_nb);

    // INPUT SUnions, never pack and unpack state because they are in charge of buffering input tuples as well
    ptr<AbstractBoxState> union_box_state;
    if ( _is_input_sunion )  
    {  union_box_state = ptr<AbstractBoxState>(new SUnionBoxNoBucketState(_checkpointed_interval_nb));
    }
    else 
    { union_box_state = ptr<AbstractBoxState>(new SUnionBoxState(_checkpointed_interval_nb,_buckets));
    }
        
    return union_box_state;

}

///////////////////////////////////////////////////////////////////////////
void
SUnionQBox::unpack_state(ptr<AbstractBoxState> box_state)
{
    INFO << "Starting recovery";

    // Input sunion goes back in time and replays all the tuples
    if ( _is_input_sunion )
    {
        ptr<SUnionBoxNoBucketState> my_state = box_state.dynamic_cast_to<SUnionBoxNoBucketState>();
        _current_interval_nb = my_state->get_interval_nb();
        _state.reconciling_rollforward();
    }

    // SUnions in the middle of the network go back in time and assume everything is just fine
    else 
    {
        ptr<SUnionBoxState> my_state = box_state.dynamic_cast_to<SUnionBoxState>();
        _current_interval_nb = my_state->get_interval_nb();
        my_state->reset_buckets(_buckets);
        _state.reconciling_rollforward();
    }

    // Reset the delays in preperation of the next failure
    for (unsigned int i = 0; i < get_num_inputs(); ++i)    
    { _actual_msec_delays[i] = _max_msec_delays[i] * MAX_ALPHA;
    }

    INFO << "Recovered interval " << _current_interval_nb;

}

///////////////////////////////////////////////////////////////////////////
SUnionBoxState::SUnionBoxState(long long interval_nb, SUnionQBox::Buckets& buckets)
    : _interval_nb(interval_nb)
{
    // Reminder: typedef map<long long, ptr<SUnionQBox::Bucket> > SUnionQBox::Buckets;
    SUnionQBox::Buckets::iterator i;

    // Iterate through all buckets
    for ( i = buckets.begin(); i != buckets.end(); ++i)
    {
        // Add bucket to vector
        _packed_buckets.push_back(pair<long long,Bucket>(i->first,i->second->deep_copy()));
    }

}


///////////////////////////////////////////////////////////////////////////
void
SUnionBoxState::reset_buckets(SUnionQBox::Buckets& buckets)
{
    // Clear old buckets
    buckets.clear();

    // Reminder: typedef  vector< pair<long long, SUnionQBox::Bucket> > PackedBuckets;
    PackedBuckets::iterator i;

    // Iterate through all our saved buckets
    for ( i = _packed_buckets.begin(); i != _packed_buckets.end(); ++i)
    {
        // Add bucket to map
        buckets[i->first] = ptr<Bucket>(new Bucket(i->second.deep_copy()));
    }

}

NMSTL_SERIAL_DEFINE(SUnionBoxNoBucketState, 3001);
NMSTL_SERIAL_DEFINE(SUnionBoxState, 3002);


BOREALIS_NAMESPACE_END

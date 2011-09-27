#include "ExperimentalJoinQBox.h"

BOREALIS_NAMESPACE_BEGIN;


// --------------------------------------------------
ExperimentalJoinQBox::~ExperimentalJoinQBox()
{

    // Manually destroy the TupleQueues so they get deleted before
    // the local PagePool does
    _input_history[LEFT] = ptr<TupleQueue>();
    _input_history[RIGHT] = ptr<TupleQueue>();

    if ( _measurements.size() > 0 )
    {

        _log_file.open("ExperimentalJoinQBox.log",fstream::app);
        if ( _log_file.is_open() )
        {
            // Just write the median mesurement.
            int middle = _measurements.size()/2;
            int quarter = int(0.25*_measurements.size());
            int threequarter = int(0.75*_measurements.size());
            sort(_measurements.begin(),_measurements.end());
            DEBUG << "Measurements are " << _measurements;
            double median_value = _measurements[middle];

            long state_size = _buffer_size[LEFT]+ _buffer_size[RIGHT];

            _log_file << _benchmark << " state_size " << state_size
                      << " state_bound_usec_median " << median_value
                      << " 25percentile " << _measurements[quarter]
                      << " 75percentile " << _measurements[threequarter] << endl;
            _log_file.close();
        } else
        {
            ERROR << "Couldn't open stats file";
        }
    }
}




// --------------------------------------------------
void ExperimentalJoinQBox::setup_impl() throw( AuroraException )
{
    JoinQBox::setup_impl();
}

// --------------------------------------------------
void ExperimentalJoinQBox::init_impl() throw( AuroraException )
{
    JoinQBox::init_impl();

    // We need this to decide how far back we want to undo
    typed_param( "left-buffer-size", _buffer_size[LEFT], PARAM_NON_EMPTY );
    typed_param( "right-buffer-size", _buffer_size[RIGHT], PARAM_NON_EMPTY );

    string benchmark_type;
    typed_param("benchmark",benchmark_type);
    _benchmark = MicroBenchmarkType(benchmark_type);
    WARN << "Benchmark type identified as " << _benchmark;

    typed_param("checkpoint",_state_bound_interval,PARAM_NON_EMPTY);
    _last_state_bound_time = Timestamp::now();
    WARN << "State bound interval " << _state_bound_interval;

    // First initialize the sizes of tuples on input and output streams
    //_output_tuple_size = getOutDescription(0)->getSize();
    _output_tuple_size = get_out_schema(0)->get_size() + HEADER_SIZE;
    int biggest_tuple = 0;
    for ( int stream_id = 0; stream_id < TWO_INPUTS; stream_id++)
    {
        //_input_tuple_size[stream_id] = getInDescription(stream_id)->getSize();
        _input_tuple_size[stream_id] = get_in_schema(stream_id)->get_size()
                                       + HEADER_SIZE;
        _boundary[stream_id] = false;
        if ( _input_tuple_size[stream_id] > biggest_tuple )
        { biggest_tuple = _input_tuple_size[stream_id];
        }
    }

    _max_history = MAX_HISTORY;
    typed_param( "max-history", _max_history);
    WARN << "Max history is " << _max_history;

    // +1 for rounding, and then +1 for each of first and last page that are partially filled
    int nb_pages = TWO_INPUTS * ( ( (biggest_tuple*_max_history)/PAGE_SIZE ) + 1 + 1 + 1 );
    WARN << "Nb pages is " << nb_pages << " and page size should be " << getpagesize();

    _pool = ptr<PagePool>(new PagePool(PAGE_SIZE,nb_pages,false));
    for ( int stream_id = 0; stream_id < TWO_INPUTS; stream_id++)
    { _input_history[stream_id] = ptr<TupleQueue>(new TupleQueue(*_pool,_input_tuple_size[stream_id]));
    }

}

//--------------------------------------------------
void ExperimentalJoinQBox::run_impl( QBoxInvocation  &inv ) throw( AuroraException )
{

    bool outputted = false;
    bool perform_undo = false;

    DeqIterator left_di = deq(LEFT);
    DeqIterator right_di = deq(RIGHT);

    INFO << "Running SJoin";

    // Actually, we should always process the next tuple in order...
    // since we use tuple order, we do one by one
    while ( inv.continue_dequeue_on(left_di,LEFT)
            && (inv.continue_dequeue_on(right_di,RIGHT)))
    {
        Tuple left_tuple = Tuple::factory(left_di.tuple());
        Tuple right_tuple = Tuple::factory(right_di.tuple());
        DEBUG << "Processing input tuples (" << left_tuple.get_tuple_id()
              << "," << right_tuple.get_tuple_id() << ")";

        if ( _benchmark._type == MicroBenchmarkType::NONE )
        {
            process_tuple(left_tuple,LEFT, outputted);
            process_tuple(right_tuple,RIGHT, outputted);
        }
        else
        {
            TupleType left_type = left_tuple.get_tuple_type();
            TupleType right_type = right_tuple.get_tuple_type();

            // Most commonly, we get data tuples so we check for that first
            if ( ( ( left_type == TupleType::INSERTION) || (left_type == TupleType::TENTATIVE))
                 && ( ( right_type == TupleType::INSERTION) || (right_type == TupleType::TENTATIVE)) )
            {

                if ( _benchmark.do_keep_undo_log() )
                {
                    store_tuple_for_undo(left_tuple,LEFT);
                    store_tuple_for_undo(right_tuple,RIGHT);
                }
                process_tuple(left_tuple,LEFT, outputted);
                process_tuple(right_tuple,RIGHT, outputted);
            }
            else if ( (left_type == TupleType::UNDO) || (right_type == TupleType::UNDO))
            {
                INFO << "Got undo tuple: left type is " << left_type << " left id is " << left_tuple.get_tuple_id();
                INFO << "Got undo tuple: right type is " << right_type << " right id is " << right_tuple.get_tuple_id();
                perform_undo = true;
                break;
            }
            else if ( ( left_type == TupleType::BOUNDARY ) || ( right_type == TupleType::BOUNDARY) )
            {
                handle_boundary(left_tuple,right_tuple);
                outputted = true;
            }
            else
            {
                // Other tuples just go through
                // XXX Need to fix to handle RECONCILIATION_DONE and RECONCILIATION_DONE_FWD properly
                pass_through(left_tuple);
                //passThrough(right_tuple); Pass through on one input to avoid duplication
                outputted = true;
            }
        }
        ++left_di;
        ++right_di;
    }

    if ( perform_undo )
    {
        handle_undo();
        outputted = true;
    }

    // Done, notify if there was output
    if ( outputted )
    {
        DEBUG << "Notifying output";
        get_output(0).notify_enq();
    }
}

//------------------------------------------------
// If we run in benchmark NONE, we don't use SUnion so there will be
// no boundary to worry about
void
ExperimentalJoinQBox::handle_boundary(const Tuple& left_tuple, const Tuple& right_tuple)
{

    const Tuple tuple[] = { left_tuple, right_tuple};

    for ( int i = 0; i < TWO_INPUTS; i++)
    {
        if ( tuple[i].get_tuple_type() == TupleType::BOUNDARY)
        {
            _boundary[i] = true;
            //_boundary_time[i] = tuple[i].get_timestamp();
            _boundary_time[i] = tuple[i].get_tuple_stime();
        }
    }

    // Only produce boundary after receiving 2 input boundaries
    if ( _boundary[LEFT] && _boundary[RIGHT] )
    {
        _boundary[LEFT] = _boundary[RIGHT] = false;

        Timestamp min_timestamp[TWO_INPUTS];
        for ( int j = 0; j < TWO_INPUTS; j++)
        {
            Timestamp min_buf = find_min_timestamp(j);
            min_timestamp[j] = min_buf < _boundary_time[j] ? min_buf : _boundary_time[j];
        }

        Timestamp min = min_timestamp[LEFT] <  min_timestamp[RIGHT] ? min_timestamp[LEFT] : min_timestamp[RIGHT];
        emit_boundary_tuple(min);
    }
}


// --------------------------------------------------
void
ExperimentalJoinQBox::pass_through(const Tuple& tuple)
{
    INFO << "Propagating tuple of type " << tuple.get_tuple_type();

    // Double-check that we are not forwarding ordinary tuples
    TupleType type = tuple.get_tuple_type();
    if ( (type != TupleType::INSERTION) && (type != TupleType::TENTATIVE) )
    { emit_tuple_with_different_type(tuple,type);
    }

}

// --------------------------------------------------
/// XXX This should become part of boxes APIs
Timestamp
ExperimentalJoinQBox::find_min_timestamp(int stream_id)
{
    // Get the tuples
    int max_nb_tuples = _buffer[stream_id]->size();
    int tuple_size = _input_tuple_size[stream_id];
    // Dynbufs are always allocated on the heap because constructor calls malloc
    dynbuf buffer(max_nb_tuples*tuple_size);
    int nb_tuples = _buffer[stream_id]->dump(buffer);

    Timestamp min = Timestamp::now();
    int next_tuple_offset = 0;
    for (int i = 0; i < nb_tuples; i++)
    {
        //Timestamp current = Tuple::factory(buffer.data()+next_tuple_offset).get_timestamp();
        Timestamp current = Tuple::factory(buffer.data()+next_tuple_offset).get_tuple_stime();
        next_tuple_offset += tuple_size;
        if ( current < min )
        { min = current;
        }
    }
    return min;
}

// --------------------------------------------------
/// XXX This should become part of boxes API
int32
ExperimentalJoinQBox::find_oldest_tuple_id(int stream_id)
{
    // NO need to make a copy!
    return _buffer[stream_id]->get_oldest();

    // XXXXX NO NEED TO MAKE A COPY
    // Get the tuples
    int max_nb_tuples = _buffer[stream_id]->size();
    int tuple_size = _input_tuple_size[stream_id];
    // Dynbufs are always allocated on the heap because constructor calls malloc
    dynbuf buffer(max_nb_tuples*tuple_size);
    int nb_tuples = _buffer[stream_id]->dump(buffer);

    if ( nb_tuples == 0 )
    {
        return -1;
    }

    // The first tuple in the buffer is the oldest
    Tuple t = Tuple::factory(buffer.data());
    return t.get_tuple_id();

}


// --------------------------------------------------
void ExperimentalJoinQBox::handle_undo()
{

    INFO << "Handling undo";

    bool has_undo[TWO_INPUTS];
    int32 undo_tuple_ids[TWO_INPUTS];

    for ( int i = 0; i < TWO_INPUTS; i++)
    {
        scan_for_undo(i,has_undo[i],undo_tuple_ids[i]);
        INFO << "For stream id " << i << " has undo is " << has_undo[i]
             << " and undo_tuple_id is " << undo_tuple_ids[i];
        if ( has_undo[i] )
        { dequeue_until_undo(i,undo_tuple_ids[i]);
        }
    }

    // - reset states: find starting point on input stream and re-populate state accordingly
    // (also emits the undo tuple that it got from the state)
    undo_state(has_undo,undo_tuple_ids);

    // Join no emit wrote all tuples on first position of output queue
    // We use that to produce the undo/redo tuple
    EnqIterator my_enq_iterator = enq(0);
    string tuple_string((char*)my_enq_iterator.tuple(),_output_tuple_size);
    emit_undo_tuple(Tuple::factory(tuple_string.data()));
}


//--------------------------------------------------
void ExperimentalJoinQBox::scan_for_undo(int stream_id, bool& has_undo, int32& undo_id)
{

    SeekIterator si = seek(stream_id);
    has_undo = false;
    while ( si.avail())
    {
        Tuple t = Tuple::factory(si.tuple());
        TupleType type = t.get_tuple_type();
        if ( type == TupleType::UNDO)
        {
            has_undo = true;
            undo_id = t.get_tuple_id();
            break;
        }
        ++si;
    }
}

//--------------------------------------------------
void ExperimentalJoinQBox::dequeue_until_undo(int stream_id, int32 undo_tuple_id)
{
    // Remove all tuples preceding undo, plus the undo tuple itself
    DeqIterator di = deq(stream_id);
    int32 current_tuple_id;
    do
    {
        if ( di.avail())
        {
            INFO << "Dequeued one on stream " << stream_id;
            const void* current_tuple = di.tuple();
            current_tuple_id = Tuple::factory(current_tuple).get_tuple_id();
            ++di;
        } else
        {
            FATAL << "Nothing to read?";
        }
    } while ( current_tuple_id != undo_tuple_id );
}

// --------------------------------------------------
void ExperimentalJoinQBox::check_iterator(int stream_id)
{
    DeqIterator di = deq(stream_id);
    if ( di.avail())
    {
        const void* current_tuple = di.tuple();
        int32 current_tuple_id = Tuple::factory(current_tuple).get_tuple_id();
        TupleType current_tuple_type = Tuple::factory(current_tuple).get_tuple_type();
        DEBUG << "On stream " << stream_id << " tuple " << current_tuple_id
              << ": " << current_tuple_type;
    }

}


// --------------------------------------------------
void ExperimentalJoinQBox::undo_state(bool has_undo[], int32 undo_tuple_ids[])
{
    INFO << "Going to undo for tuples " << undo_tuple_ids[LEFT] << "," << undo_tuple_ids[RIGHT];

    // Walk *backwards* until we find id tuples and then some more
    TupleQueue::RSeekIterator ri[TWO_INPUTS];
    ri[LEFT] = _input_history[LEFT]->rseek_iterator();
    ri[RIGHT] = _input_history[RIGHT]->rseek_iterator();

    StateBound bound = find_bound(ri,has_undo,undo_tuple_ids);
    int32 bound_ids[TWO_INPUTS];
    bound_ids[LEFT] = bound._left;
    bound_ids[RIGHT] = bound._right;
    INFO << "Bound is " << bound_ids[LEFT] << "," << bound_ids[RIGHT];

    //Iterate until you get both left and right
    int counter = rollback(ri,bound_ids);

    // Clear all internal state
    clear_state();

    // Now go forward rebuilding the state
    rollforward(ri,counter);


}


// --------------------------------------------------
/**
 * Rollsback state until finds the oldest tuple to undo then looks
 * up the following conservative state bound. We use left stream
 * to find bound because we use left stream to store bounds periodically
 */
ExperimentalJoinQBox::StateBound
ExperimentalJoinQBox::find_bound(TupleQueue::RSeekIterator ri[], bool has_undo[], int32 undo_tuple_ids[])
{

    bool found_undos = false;
    bool done[TWO_INPUTS];
    done[LEFT] = (!has_undo[LEFT]);
    done[RIGHT] = (!has_undo[RIGHT]);

    // Find the oldest of the two undo tuples
    while ( ri[LEFT].avail() && ri[RIGHT].avail() && !found_undos)
    {
        for ( int r = 0; r < TWO_INPUTS; r++)
        {
            int32 tuple_id = Tuple::factory(ri[r].tuple()).get_tuple_id();
            INFO << "Undoing tuple with id " << tuple_id;

            if ( !done[r] && (tuple_id == undo_tuple_ids[r]) )
            {
                INFO << "Found undo tuple on stream " << r << " with id " << tuple_id;
                done[r] = true;
            }
        }
        ++ ri[LEFT];
        ++ ri[RIGHT];
        found_undos = done[LEFT] && done[RIGHT];
    }

    if ( ! found_undos )
    {
        int32 first[TWO_INPUTS];
        ri[LEFT] = _input_history[LEFT]->rseek_iterator();
        ri[RIGHT] = _input_history[RIGHT]->rseek_iterator();
        first[LEFT] = Tuple::factory(ri[LEFT].tuple()).get_tuple_id();
        first[RIGHT] = Tuple::factory(ri[RIGHT].tuple()).get_tuple_id();
        ERROR << "Undo tuples are " << undo_tuple_ids[LEFT] << "," << undo_tuple_ids[RIGHT];
        ERROR << "First tuples in log are " <<  first[LEFT] << "," << first[RIGHT];
        FATAL << "Trying to undo too far while looking for undo tuples";
    }

    // Move left iterator ahead to find conservative bound
    int counter = 0;
    StateBound bound;
    while ( !bound && ri[LEFT].avail())
    {
        int32 tuple_id = Tuple::factory(ri[LEFT].tuple()).get_tuple_id();
        INFO << "Looking for bound at tuple id " << tuple_id;
        bound = _state_bounds[tuple_id];
        counter++;
        ++ri[LEFT];
        // XXX Debuggin
        if ( bound )
        {
            INFO << "Bound is " << bound._left << "," << bound._right;
            INFO << "Found this bound for tuple id " << tuple_id;
        }
    }

    // Bring iterator back
    for ( int i = 0; i < counter; i++)
    { --ri[LEFT];
    }

    if ( ! bound )
    { FATAL << "Trying to find bound too far";
    }

    return bound;

}



// --------------------------------------------------
int ExperimentalJoinQBox::rollback(TupleQueue::RSeekIterator ri[], int32 bound_ids[])
{

    int counter = 0;

    // For each input stream, there are up to 2 bounds to find
    bool all_done = false;
    bool done[TWO_INPUTS];
    for ( int i = 0; i < TWO_INPUTS; i++)
    { done[i] = false;
    }

    while ( ri[LEFT].avail() && ri[RIGHT].avail() && !all_done)
    {
        // Check left tuple against left bounds and right tuple against right bounds
        for ( int r = 0; r < TWO_INPUTS; r++)
        {
            int32 tuple_id = Tuple::factory(ri[r].tuple()).get_tuple_id();
            DEBUG << "Undoing tuple with id " << tuple_id;

            if ( !done[r] && (tuple_id == bound_ids[r] ) )
            {
                done[r] = true;
                INFO << "Found one bound on stream " << r << " bound is "
                     << bound_ids[r] << " tuple is " << tuple_id;
            }
        }

        // Condition to stop iterating
        all_done = done[LEFT] && done[RIGHT];
        ++ ri[LEFT];
        ++ ri[RIGHT];
        counter++;
    }
    // Bound is first tuple to input (not the one before the bound)
    -- ri[LEFT];
    -- ri[RIGHT];

    if ( ! all_done )
    {
        int32 first[TWO_INPUTS];
        int32 last[TWO_INPUTS];
        -- ri[LEFT];
        -- ri[RIGHT];
        last[LEFT] = Tuple::factory(ri[LEFT].tuple()).get_tuple_id();
        last[RIGHT] = Tuple::factory(ri[RIGHT].tuple()).get_tuple_id();
        ri[LEFT] = _input_history[LEFT]->rseek_iterator();
        ri[RIGHT] = _input_history[RIGHT]->rseek_iterator();
        first[LEFT] = Tuple::factory(ri[LEFT].tuple()).get_tuple_id();
        first[RIGHT] = Tuple::factory(ri[RIGHT].tuple()).get_tuple_id();
        ERROR << "Bound tuples are " << bound_ids[LEFT] << "," << bound_ids[RIGHT];
        ERROR << "First tuples in log are " <<  first[LEFT] << "," << first[RIGHT];
        ERROR << "Last tuples in log are " <<  last[LEFT] << "," << last[RIGHT];
        FATAL << "Trying to undo too far while looking for bound";
    }

    INFO << "Returning counter " << counter;
    return counter;

}


// --------------------------------------------------
void ExperimentalJoinQBox::rollforward(TupleQueue::RSeekIterator ri[], int counter)
{

    // Repopulate the states from correct location
    bool anything_joined = false;

    // We go forward until we process the *undo* tuples again
    for ( int t = 0; t <= counter; t++)
    {
        for ( int b = 0; b < TWO_INPUTS; b++)
        {
            DEBUG << "Redoing tuple with id " << Tuple::factory(ri[b].tuple()).get_tuple_id();
            int other_stream_id = (b == LEFT) ? RIGHT : LEFT;
            bool outputted = false;
            if ( b == RIGHT ) // Only do a half-way join
            {_buffer[other_stream_id]->join_no_emit(ri[b].tuple(), _predicate.get(), outputted);
            }
            if ( outputted )
            { anything_joined = true;
            }
            _buffer[b]->insert_tuple(ri[b].tuple());
            --ri[b];
        }
    }
    // XXX TODO XXX If the undo tuples were at different offsets, then we still need to read
    // some old tuples for one of the streams. Currently, we are dropping these tuples

    // Output undo tuple
    if ( !anything_joined )
    { FATAL << "Nothing joined when recreating state!!!";
    }

}



// --------------------------------------------------
void ExperimentalJoinQBox::clear_state()
{
    _buffer[ LEFT ]->clear();
    _buffer[ RIGHT ]->clear();
}


// --------------------------------------------------
void ExperimentalJoinQBox::emit_tuple_with_different_type(const Tuple& tuple, TupleType type)
{
    // Write the values
    EnqIterator my_enq_iterator = enq(0);
    char* tuple_start = (char*)my_enq_iterator.tuple();
    Tuple dst_tuple = Tuple::factory(tuple_start);
    dst_tuple.copy_tuple(tuple,_output_tuple_size);
    dst_tuple.set_tuple_type(type);
    ++my_enq_iterator;
}

// --------------------------------------------------
void ExperimentalJoinQBox::emit_undo_tuple(const Tuple& src_tuple)
{
    emit_tuple_with_different_type(src_tuple,TupleType::UNDO);
}

// --------------------------------------------------
void ExperimentalJoinQBox::emit_boundary_tuple(Timestamp min_timestamp)
{
    // Write the values
    EnqIterator my_enq_iterator = enq(0);
    char* tuple_start = (char*)my_enq_iterator.tuple();
    Tuple dst_tuple = Tuple::factory(tuple_start);

    // Clear tuples and set header to default values
    dst_tuple.clear_tuple(_output_tuple_size);
    dst_tuple.set_default_header();

    dst_tuple.set_tuple_type(TupleType::BOUNDARY);
    dst_tuple.set_tuple_stime(min_timestamp);
    //dst_tuple.set_timestamp(min_timestamp);

    // XXX Other fields in header maybe set to some decen value

    ++my_enq_iterator;
}


// --------------------------------------------------
void ExperimentalJoinQBox::store_tuple_for_undo(const Tuple& src_tuple, int stream_id)
{

    // Because saveStateBound has half the cost of checkpoints, we can do this twice as often
    Timestamp current_time = Timestamp::now();
    if ( (stream_id == LEFT) && ((current_time - _last_state_bound_time).to_msecs() > _state_bound_interval))
    {
        _last_state_bound_time = current_time;
        save_state_bound(src_tuple,stream_id);
    }

    // Store the tuple in input history
    ptr<TupleQueue> tq = _input_history[stream_id];

    // While stable tuples are incoming, periodically cut the useless history
    if ( (src_tuple.get_tuple_type() != TupleType::TENTATIVE) && (tq->size() >= _max_history-1))
    {
        cut_history(LEFT);
        cut_history(RIGHT);
    }

    if ( (tq->size() < _max_history) )
    {
        TupleQueue::EnqIterator enq = tq->enq_iterator();
        Tuple dst_tuple = Tuple::factory(enq.tuple());
        dst_tuple.copy_tuple(src_tuple,_input_tuple_size[stream_id]);
        ++enq;
    }

}

// --------------------------------------------------
void ExperimentalJoinQBox::cut_history(int stream_id)
{
    // Course grained approach: cutting 1/2 the history
    ptr<TupleQueue> tq = _input_history[stream_id];
    TupleQueue::DeqIterator deq = tq->deq_iterator();
    while ( tq->size() >= _max_history - 1)
    { ++deq;
    }
}


// --------------------------------------------------
void ExperimentalJoinQBox::save_state_bound(const Tuple& src_tuple, int stream_id)
{
    if ( stream_id != LEFT)
    {
        ERROR << "Saving bounds only on LEFT stream";
        return;
    }

    long long before = Time::now().to_usecs();

    // Periodically, ask buffer what is its oldest tuple on each input stream
    int32 left_tuple_id = find_oldest_tuple_id(LEFT);
    int32 right_tuple_id = find_oldest_tuple_id(RIGHT);

    //if ( right_tuple_id > src_tuple.getTupleId())
    //
    //{
    //  FATAL << "For tuple " << src_tuple.getTupleId() << " right bound appears to be " << right_tuple_id
    //        << " while left bound is " << left_tuple_id;
    //}

    // Associate current tuple to pair of oldest tuples
    int32 src_tuple_id = src_tuple.get_tuple_id();
    _state_bounds[src_tuple_id] = StateBound(left_tuple_id,right_tuple_id);

    long long after = Time::now().to_usecs();

    _measurements.push_back(after-before);

    DEBUG << "For tuple " << src_tuple_id << " saved bound ("
          << _state_bounds[src_tuple_id]._left << ","
          << _state_bounds[src_tuple_id]._right << ")";
    DEBUG << "\n After - before is " << after-before;

}


// --------------------------------------------------
void ExperimentalJoinQBox::process_tuple(const Tuple& tuple, int stream_id, bool& outputted)
{
    int comparison_count, match_count; // -- tatbul

    // Now process the tuple
    int other_stream_id = (stream_id == LEFT) ? RIGHT : LEFT;

    // To make the computation less expensive, we only do a halfway join
    if ( stream_id == RIGHT)
    {
        comparison_count = match_count = 0; // -- tatbul
        _buffer[other_stream_id]->join(tuple.get_data(), _predicate.get(),
                                       outputted,
                                       &comparison_count, &match_count);
        DEBUG << "Joined " << Tuple::factory(enq(0).tuple()).get_tuple_id() << " outputted " << outputted;
    }

    _buffer[stream_id]->insert_tuple(tuple.get_data());

    // We may also need to modify other places in the SJoin code. -- tatbul
    //
    _buffer[stream_id]->back()->add_comparison_count(comparison_count);
    _buffer[stream_id]->back()->add_match_count(match_count);

}

AURORA_DEFINE_QBOX(ExperimentalJoinQBox, "experimentaljoin");

BOREALIS_NAMESPACE_END;


// --------------------------------------------------
//ifstream in1("/proc/stat");
//string label;
// Values are hundredths of a second
//long user, nice, system, idle;
//long user2, nice2, system2, idle2;

//if (!(in1 >> label >> user >> nice >> system >> idle))
//{    FATAL << "Unable to read /proc/stat";
//}
//in1.close();
//ifstream in2("/proc/stat");
//if (!(in2 >> label >> user2 >> nice2 >> system2 >> idle2))
//{    FATAL << "Unable to read /proc/stat";
//}
//in2.close();
//    << "\n  delta_user is " << user2 - user
//<< "\n  delta_nice is " << nice2 - nice
//<< "\n  delta_system is " << system2 - system
//<< "\n  delta_idle is " << idle2 - idle;

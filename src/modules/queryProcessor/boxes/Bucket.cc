#include "Bucket.h"

BOREALIS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
Bucket::Bucket(int nb_buffers, vector<size_t> tuple_sizes)
    : _contains_tentative_tuples(false),
      _contains_tuples(false),
      _tuple_sizes(tuple_sizes)
{

    for (int i = 0; i < nb_buffers; i++)
    {
        ptr<TuplesBuffer> tb(new TuplesBuffer(tuple_sizes[i]));
        _buffers[i] = tb;
        _boundaries[i] = false;
    }

    //_last_emitted_tuple =  ptr<dynbuf>(new dynbuf(_tuple_size));
    //memset(_last_emitted_tuple->data(),0,_tuple_size);
    //_valid_last_emitted_tuple = false;

}

///////////////////////////////////////////////////////////////////////////
Bucket
Bucket::deep_copy()
{
    Bucket b(_buffers.size(),_tuple_sizes);

    // Copy each buffer
    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        int stream_id = i->first;
        ptr<TuplesBuffer> tb = i->second;

        // Copy each tuple in each buffer
        for ( int j = 0; j < tb->nb_tuples(); j++)
        {
            b.insert_tuple(stream_id,tb->tuple_at(j));
        }
    }
    return b;
}

///////////////////////////////////////////////////////////////////////////
void
Bucket::bound(int stream_id)
{
    _boundaries[stream_id] = true;
}

void
Bucket::clear_all_tentative_tuples()
{
    TuplesBufferSet::iterator i;
    for ( i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        i->second->clear_tentative();
    }
    reset_stability();
}


///////////////////////////////////////////////////////////////////////////
void
Bucket::insert_tuple_overwriting(int stream_id, const Tuple& data)
{

    ptr<TuplesBuffer> tb = lookup(_buffers,stream_id);
    if ( tb )
    {
        // Note: an empty buffer is also stable
        bool stable_buffer = tb->is_stable_buffer();
        bool empty_buffer = tb->is_empty();
        bool stable_tuple = data.get_tuple_type() == TupleType::INSERTION;

        // If tuple and buffer are stable, then insert
        if ( (stable_tuple) && (stable_buffer) )
        {
            DEBUG << "Inserted as stable into stable " << data.get_tuple_id();
            tb->insert_tuple(data);
            _contains_tuples = true;
        }

        // If tuple tentative and buffer tentative, then insert
        else if ( (!stable_tuple) && (  (!stable_buffer) || (empty_buffer) ) )
        {
            DEBUG << "Inserted as tentative into tentative " << data.get_tuple_id();
            tb->insert_tuple(data);
            _contains_tentative_tuples = true;
            _contains_tuples = true;
        }

        // If tuple stable but buffer tentative, then clear all and insert
        else if ( stable_tuple && (!stable_buffer) )
        {
            DEBUG << "Inserted as stable into tentative " << data.get_tuple_id();
            // Leave any stable tuples that might be at the beginning of the buffer
            tb->clear_tentative();
            reset_stability();
            tb->insert_tuple(data);
            _contains_tuples = true;
        }

        // If tuple tentative and buffer stable, then drop
        else
        {
            DEBUG << "Dropping tentative " << data.get_tuple_id();
            // do nothing
        }

    }
    else
    {
        ERROR << "Trying to insert into out of bound stream " << stream_id;
    }
}

///////////////////////////////////////////////////////////////////////////
void
Bucket::insert_tuple(int stream_id, const Tuple& data)
{

    ptr<TuplesBuffer> tb = lookup(_buffers,stream_id);
    if ( tb )
    {
        tb->insert_tuple(data);
        _contains_tuples = true;

        // If tuple tentative, indicate that buffer contains tentative tuples
        if ( data.get_tuple_type() == TupleType::TENTATIVE )
        {
            _contains_tentative_tuples = true;
        }

    }
    else
    {
        ERROR << "Trying to insert into out of bound stream " << stream_id;
    }
}


///////////////////////////////////////////////////////////////////////////
void
Bucket::clear_all()
{
    _contains_tentative_tuples = false;
    _contains_tuples = false;
    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        i->second->clear();
    }

}


///////////////////////////////////////////////////////////////////////////
void
Bucket::clear_stream(int stream_id)
{
    ptr<TuplesBuffer> tb = lookup(_buffers,stream_id);
    if ( tb )
    {
        tb->clear();
        reset_stability();
    }

    _contains_tuples = false;
    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        if ( !i->second->is_empty())
        { _contains_tuples = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
void
Bucket::clear_stream_after(int stream_id, int undo_tuple_id)
{

    ptr<TuplesBuffer> tb = lookup(_buffers,stream_id);
    if ( tb )
    {
        tb->clear_after(undo_tuple_id);
        reset_stability();
    }

    _contains_tuples = false;
    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        if ( !i->second->is_empty())
        { _contains_tuples = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
const Tuple
Bucket::get_last_tuple()
{
    // If tuples were already emitted, return the one that was truely
    // the last one. This helps when tuples are not necessarily sorted
    //if ( _valid_last_emitted_tuple)
    //{ return Tuple::factory(_last_emitted_tuple->data());
    // }

    if ( !is_empty() )
    {
        int highest_index = find_highest_index();
        return _buffers[highest_index]->highest_tuple();
    }

    FATAL << "Cannot get_last_tuple on an empty bucket. Check if empty first.";
    return Tuple::factory(string("")); // Bogus will have exited before this line
}


///////////////////////////////////////////////////////////////////////////
void
Bucket::emit_tuples(Stream::EnqIterator out, TupleType::Type tuple_type)
{
    
    int lowest_index = find_lowest_index();
    DEBUG << "Initial lowest index is " << lowest_index;

    while ( lowest_index != -1 )
    {
        ptr<TuplesBuffer> tb = _buffers[lowest_index];

        Timestamp lowest_value =  tb->lowest_value();
        // Second lowest value is "Timestamp()" if there is no actual second lowest values
        Timestamp second_lowest_value = find_second_lowest_value(lowest_index,lowest_value);

        while ( (tb->more_tuples()) && 
                ((lowest_value < second_lowest_value) || (lowest_value == second_lowest_value) || (second_lowest_value == Timestamp()) )
               )
        {

            DEBUG << "Emitting one tuple";
            Tuple dst_tuple = Tuple::factory(out.tuple());
            Tuple src_tuple = tb->lowest_tuple();            
            dst_tuple.copy_tuple(src_tuple,tb->tuple_size());
            dst_tuple.set_tuple_type(tuple_type);
            ++out;
            
            // XXX This will definitely add overhead and we 
            // assume tuples are sorted on each stream
            // update_last_emitted_tuple(src_tuple);

            // Check if there are more tuples and update the lowest value
            tb->advance_lowest();
            if (tb->more_tuples())
            { lowest_value = tb->lowest_value(); 
            }
        }
        lowest_index = find_lowest_index();
        DEBUG << "Updated lowest index to " << lowest_index;
    }

    // Reset all lowest indexes if we need to emit again
    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        i->second->reset_lowest();
    }

}

// --------------------------------------------------
//void
//Bucket::update_last_emitted_tuple(const Tuple& src_tuple)
//{
//_valid_last_emitted_tuple = true;
//char* tuple_start = _last_emitted_tuple->data();
//Tuple dst_tuple = Tuple::factory(tuple_start);
//dst_tuple.copy_tuple(src_tuple,_tuple_size);
//}

///////////////////////////////////////////////////////////////////////////
// Note: assumes that the size of outs equals the size of _buffers
void
Bucket::emit_tuples_on_separate_outputs(vector<Stream::EnqIterator>& outs, TupleType::Type tuple_type)
{

    vector<Stream::EnqIterator>::iterator out = outs.begin();

    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        // xxx should use an iterator instead
        for ( int j = 0; j < i->second->nb_tuples(); j++)
        {

            Tuple dst_tuple = Tuple::factory((*out).tuple());
            Tuple src_tuple = i->second->tuple_at(j);

            dst_tuple.copy_tuple(src_tuple,i->second->tuple_size());
            dst_tuple.set_tuple_type(tuple_type);

            ++(*out); // Add one tuple
        }
        ++out; // Go to next output stream
    }

}


///////////////////////////////////////////////////////////////////////////
int
Bucket::find_highest_index()
{    
    int highest_index = -1;
    Timestamp highest_value = Timestamp();
    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        if ( !i->second->is_empty())
        {
            Timestamp value = i->second->highest_value();
            if ( value > highest_value )
            {
                highest_value = value;
                highest_index = i->first;
            }
        }
    }
    return highest_index;
}

///////////////////////////////////////////////////////////////////////////
int
Bucket::find_lowest_index()
{    
    int lowest_index = -1;
    Timestamp lowest_value = Timestamp();
    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        if ( i->second->more_tuples() )
        {
            Timestamp value = i->second->lowest_value();
            if ( (lowest_value == Timestamp() ) || (value < lowest_value) )
            {
                lowest_value = value;
                lowest_index = i->first;
            }
        }
    }
    return lowest_index;
}

///////////////////////////////////////////////////////////////////////////
Timestamp
Bucket::find_second_lowest_value(int lowest_index, Timestamp lowest_value)
{    
    //Timestamp second_lowest_value = Timestamp::now();
    Timestamp second_lowest_value = Timestamp();

    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        if ( lowest_index != i->first)
        {
            if ( i->second->more_tuples() )
            {
                Timestamp value = i->second->lowest_value();
                if ( ((value > lowest_value) || ( value == lowest_value)) // If greater or equal to lowest value
                     && 
                     ( (value < second_lowest_value) || ( second_lowest_value == Timestamp())) ) // Less that second lowest
                {
                    second_lowest_value = value;
                }
            }
        }
    }
    return second_lowest_value;
}


///////////////////////////////////////////////////////////////////////////
bool
Bucket::is_expired(map<int,double>& max_msec_delays)
{

    TuplesBufferSet::iterator i;
    for ( i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        int stream_id = i->first;
        ptr<TuplesBuffer> tb = i->second;
        if ( tb->is_expired(max_msec_delays[stream_id]))
            return true;
    }
    return false;

}

///////////////////////////////////////////////////////////////////////////
void
Bucket::set_timer(int stream_id)
{

    ptr<TuplesBuffer> tb = lookup(_buffers,stream_id);
    if ( tb )
    {
        tb->set_timer();
    }
}


///////////////////////////////////////////////////////////////////////////
Timestamp
Bucket::get_first_tuple_time()
{
    Timestamp first_tuple_time = Timestamp::now();

    TuplesBufferSet::iterator i;
    for ( i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        Timestamp buffer_time = i->second->get_first_tuple_time();
        if ( buffer_time < first_tuple_time )
            first_tuple_time = buffer_time;
    }
    return first_tuple_time;
}


///////////////////////////////////////////////////////////////////////////
bool
Bucket::is_bounded()
{
    bool bounded = true;
    for ( map<int, bool>::iterator i = _boundaries.begin(); i != _boundaries.end(); ++i)
    {
        if ( !(i->second) )
        { bounded = false;
        }
    }
    return bounded;
}

///////////////////////////////////////////////////////////////////////////
bool
Bucket::is_bounded(int stream_id)
{
    return _boundaries[stream_id];
}


///////////////////////////////////////////////////////////////////////////
bool
Bucket::is_empty()
{
    return ( !_contains_tuples);
}

///////////////////////////////////////////////////////////////////////////
bool
Bucket::is_tentative()
{
    return _contains_tentative_tuples;
}


///////////////////////////////////////////////////////////////////////////
void
Bucket::reset_stability()
{
    _contains_tentative_tuples = false;
    for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
    {
        if ( !(i->second->is_stable_buffer()) )
        {
            DEBUG << "Bucket still contains tentative data";
            _contains_tentative_tuples = true;
            return;
        }
    }
    DEBUG << "Bucket now contains only stable data";

}

///////////////////////////////////////////////////////////////////////////
Bucket::TuplesBuffer::TuplesBuffer(size_t tuple_size) : 
    _timer_set(false),
    _tuple_size(tuple_size),
    _cur_nb_tuples(0),
    _max_nb_tuples(DEFAULT_SIZE),
    _lowest_index(0),
    _tuples(ptr<dynbuf>(new dynbuf(_max_nb_tuples*_tuple_size)))
{

}


///////////////////////////////////////////////////////////////////////////
bool
Bucket::TuplesBuffer::is_empty()
{
    return ( _cur_nb_tuples == 0 );
}

///////////////////////////////////////////////////////////////////////////
bool
Bucket::TuplesBuffer::is_expired(double max_msec_delay)
{
    bool expired = false;

    // Either expired as per info on our oldest tuple
    if (  (!is_empty()) && (Timestamp::now() - _first_tuple_time).to_msecs() > max_msec_delay )
    { 
        expired = true;
    }

    // Or expired as per explicit timer
    if ( is_empty() && 
         _timer_set &&
         (Timestamp::now() - _first_tuple_time).to_msecs() > max_msec_delay )
    {
        WARN << "Using timer-based expiration";
        expired = true;
    }

    return expired;
}

///////////////////////////////////////////////////////////////////////////
Timestamp
Bucket::TuplesBuffer::get_first_tuple_time()
{
    if ( is_empty() && (!_timer_set ) )
    { 
        return Timestamp::now(); 
    }

    return _first_tuple_time;
}

///////////////////////////////////////////////////////////////////////////
void
Bucket::TuplesBuffer::set_timer()
{
    if ( is_empty())
    {
        _first_tuple_time = Timestamp::now();
        _timer_set = true;
    }

}


void
Bucket::TuplesBuffer::insert_tuple(const Tuple& tuple)
{

    // Only update the time if we didn't set a timer on bucket expiration already
    if ( is_empty() && !_timer_set)
    {
        _first_tuple_time = Timestamp::now();
    }

    // Double the buffer if necessary
    if ( _cur_nb_tuples >= _max_nb_tuples )
    {
        INFO << "Doubling the size of the buffer to insert tuple " << tuple.get_tuple_id();
        _max_nb_tuples = 2*_max_nb_tuples;
        ptr<dynbuf> new_buf(new dynbuf(_max_nb_tuples*_tuple_size));
        memcpy(new_buf->data(),_tuples->data(),_cur_nb_tuples*_tuple_size);
        _tuples = new_buf;
    }

    memcpy(_tuples->data()+(_cur_nb_tuples*_tuple_size),tuple.get_data(),_tuple_size);
    _cur_nb_tuples ++;

}

///////////////////////////////////////////////////////////////////////////
const Tuple
Bucket::TuplesBuffer::tuple_at(int index)
{
    return Tuple::factory(_tuples->data() + (index * _tuple_size));
}

///////////////////////////////////////////////////////////////////////////
bool
Bucket::TuplesBuffer::more_tuples()
{
    return ( _lowest_index < _cur_nb_tuples );
    
}

///////////////////////////////////////////////////////////////////////////
void
Bucket::TuplesBuffer::advance_lowest()
{
    if ( more_tuples() )
    {
        _lowest_index++;
    }
    
}

///////////////////////////////////////////////////////////////////////////
const Tuple
Bucket::TuplesBuffer::lowest_tuple()
{
    return Tuple::factory(_tuples->data() + (_lowest_index * _tuple_size));
}

///////////////////////////////////////////////////////////////////////////
const Timestamp
Bucket::TuplesBuffer::lowest_value()
{
    if ( !more_tuples() )
    {  FATAL << "Cannot invoke lowest_value once there are no more tuples to emit. Check more_tuples() first.";
    }
    return Tuple::factory(_tuples->data() + (_lowest_index * _tuple_size)).get_tuple_stime();

}

///////////////////////////////////////////////////////////////////////////
const Tuple
Bucket::TuplesBuffer::highest_tuple()
{
    if (is_empty())
    { FATAL << "Cannot invoke highest_tuple on an empty buffer. Check if empty first.";
    }

    return Tuple::factory(_tuples->data() + ((_cur_nb_tuples-1) * _tuple_size));
}

///////////////////////////////////////////////////////////////////////////
const Timestamp
Bucket::TuplesBuffer::highest_value()
{
    if (is_empty())
    { FATAL << "Cannot invoke highest_value on an empty buffer. Check if empty first.";
    }

    return Tuple::factory(_tuples->data() + ((_cur_nb_tuples-1) * _tuple_size)).get_tuple_stime();
}


///////////////////////////////////////////////////////////////////////////
void
Bucket::TuplesBuffer::clear()
{
    _cur_nb_tuples = 0;
}


///////////////////////////////////////////////////////////////////////////
void
Bucket::TuplesBuffer::clear_after(int undo_tuple_id)
{

    char* index = _tuples->data();
    char* max_index = _tuples->data() + (_cur_nb_tuples*_tuple_size);
    bool found = false;

    int tuples_removed = 0;
    while ( !found && (index < max_index) )
    {
        Tuple t = Tuple::factory(index);
        int32 tuple_id = t.get_tuple_id();
        if ( tuple_id == undo_tuple_id)
        { found = true; INFO << "Found tuple id after " << tuples_removed << " tuples";
        }
        // and skip over found tuple as well
        tuples_removed++;
        index += _tuple_size;
    }

    if ( found )
    { _cur_nb_tuples = _cur_nb_tuples - tuples_removed;
    } else
    {
        INFO << "Did not find the undo tuple in this buffer... clearing all";
        _cur_nb_tuples = 0;
    }

}

///////////////////////////////////////////////////////////////////////////
void
Bucket::TuplesBuffer::clear_tentative()
{

    for ( int j = 0; j < _cur_nb_tuples; j++)
    {
        char* index = _tuples->data() + (j*_tuple_size);
        Tuple t = Tuple::factory(index);
        if ( t.get_tuple_type() == TupleType::TENTATIVE)
        {
            _cur_nb_tuples = j;
            break;
        }
    }

}

///////////////////////////////////////////////////////////////////////////
bool
Bucket::TuplesBuffer::is_stable_buffer()
{

    if ( _cur_nb_tuples == 0)
    { return true;
    }

    char* index = _tuples->data() + (_cur_nb_tuples - 1) * _tuple_size;
    Tuple t = Tuple::factory(index);
    TupleType tuple_type = t.get_tuple_type();
    if ( tuple_type == TupleType::TENTATIVE )
    {    return false;
    } else
    {    return true;
    }

}

BOREALIS_NAMESPACE_END


// const Tuple
// Bucket::get_last_tuple()
// {

//     if ( !is_empty() )
//     {
//         for ( TuplesBufferSet::reverse_iterator i = _buffers.rbegin(); i != _buffers.rend(); ++i)
//         {
//             if ( i->second->nb_tuples() > 0 )
//             { return i->second->tuple_at(i->second->nb_tuples()-1);
//             }
//         }
//     }
//     throw "Empty bucket";
//     return Tuple::factory(string(_tuple_size,0));
// }


// Output tuples quickly
// void
// Bucket::emit_tuples(Stream::EnqIterator out, TupleType::Type tuple_type)
// {

//     for ( TuplesBufferSet::iterator i = _buffers.begin(); i != _buffers.end(); ++i)
//     {
//         // xxx should use an iterator instead
//         for ( int j = 0; j < i->second->nb_tuples(); j++)
//         {

//             Tuple dst_tuple = Tuple::factory(out.tuple());
//             Tuple src_tuple = i->second->tuple_at(j);

//             dst_tuple.copy_tuple(src_tuple,_tuple_size);
//             dst_tuple.set_tuple_type(tuple_type);

//             // Copy tuple
//             ++out;
//         }
//     }

// }




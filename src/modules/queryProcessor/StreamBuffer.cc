#include "StreamBuffer.h"
#include "Tuple.h"

BOREALIS_NAMESPACE_BEGIN

// --------------------------------------------------
StreamBuffer::StreamBuffer(Name stream_name) :
    _name(stream_name),
    _init(false),
    _tuple_size(0),
    _begin(0),
    _end(0),
    _next(0),
    _next_equals_begin(true),
    _empty(true)
    {
}

// --------------------------------------------------
StreamBuffer::~StreamBuffer()
{
    DEBUG << "Destroying " << *this;
}

// --------------------------------------------------
string StreamBuffer::as_string() const
{
    int begin = 0;
    int end = 0;
    int next = 0;
    const char* buffer = 0;
    locking (_buffer_lock) { // Want to view a consistent state
        if ( _init )
        {
            buffer = _bin_tuples->data();
            begin = (_begin - buffer)/_tuple_size;
            end = (_end - buffer)/_tuple_size;
            next = (_next - buffer)/_tuple_size;
        }
    }
    return string("StreamBuffer={") + "("+begin+", "+end+", "+next+")}";
}

// --------------------------------------------------
string StreamBuffer::repr() const
{
    return as_string();
}

// --------------------------------------------------
void StreamBuffer::init_buffer(int tuple_size)
{

    _init = true;
    _tuple_size = tuple_size;

    // Make the buffer an integer number of tuples so tuples always line up
    int buffer_size = (3*HUGE_BUF)/_tuple_size;
    _bin_tuples = ptr<dynbuf>(new dynbuf(buffer_size*_tuple_size));
    memset(_bin_tuples->data(),0,buffer_size*_tuple_size);
    _begin = _bin_tuples->data();
    _next = _begin;
    _next_equals_begin = true;
    _end = _bin_tuples->data();
    INFO << "Allocated buffer of size " << _bin_tuples->max_length();
}

// --------------------------------------------------
void StreamBuffer::reset_next()
{
//    NOTICE << "reset_next";
    locking(_buffer_lock)
    {
        _next_equals_begin = true;
        _next = _begin;
    }
}

// --------------------------------------------------
void StreamBuffer::buffer(const char* new_tuples, int length, int tuple_size)
{
//    NOTICE << "buffer";
    if (length == 0) return;
    locking(_buffer_lock)
    {

        bool init = false;
        if ( !_init )
        {
            init = true;
            init_buffer(tuple_size);
        }

        // If more tuples than we can hold, then only store the last tuples that fit
        int capacity = _bin_tuples->max_length();
        if ( length > capacity )
        {
            INFO << "Cannot buffer everything.";
            memcpy(_bin_tuples->data(),new_tuples+length-capacity,capacity);
            _begin = _bin_tuples->data();
            _next = _begin;
            _next_equals_begin = true;
            _end = _bin_tuples->data()+capacity;
            _empty = false;
        }
        // Otherwise we just need to buffer the tuples possibly looping around
        else
        {
            int capacity_end = _bin_tuples->data() + capacity - _end;
            bool move_begin = false;
            if ( capacity_end >= length )
            {
                DEBUG << "Enough capacity, adding to the end";
                DEBUG << "Capacity_end is " << capacity_end;
                // XXX Modified here. If m_end == m_begin but buffer empty then no need to move
                // begin
                if ( (_end <= _begin) && (!init) && (!_empty) )
                {
                    move_begin = true;
                }
                memcpy(_end,new_tuples,length);
                _end += length;
                _empty = false;
            } else
            {
                NOTICE << "No enough capacity, rolling over";
                DEBUG << "Capacity_end is " << capacity_end;
                memcpy(_end,new_tuples,capacity_end);
                memcpy(_bin_tuples->data(),new_tuples+capacity_end,length-capacity_end);
                // XXX Modified here, if we're rolling over and crossing m_begin, reset the
                // latter to the beginning of the buffer
                if ( (_end <= _begin) && (!init) && (!_empty) )
                {
                    DEBUG << "Rolled over m_begin";
                    _begin = _bin_tuples->data();
                }
                _end = _bin_tuples->data()+length-capacity_end;
                // XXX Check if not empty before deciding to move
                if ( !_empty)
                {
                    move_begin = true;
                }
                _empty = false;
            }
            // Now push begin so that it is in front of end (begin could rollover
            if (move_begin)
            {
                char* buffer_end = _bin_tuples->data() + capacity;
                while ( _begin < _end)
                {
                    _begin += _tuple_size;
                    if (_next_equals_begin)
                        _next = _begin;
                }
                if ( _begin >= buffer_end )
                {
                    _begin = _bin_tuples->data() + (_begin - buffer_end);
                }
                if (_next_equals_begin)
                    _next = _begin;
            }
        }
    }
}

// --------------------------------------------------
/**
 * Return an event that contains all tuples since first_tuple to
 * m_end. If first_tuple is empty, return all tuples since begin to end
 * If first_tuple is not empty but is not found, return an empty set of tuples
 * Does NOT modify m_next at all
 */
ptr<StreamEvent> StreamBuffer::all_tuples_since(string first_tuple)
{
    //NOTICE << "all_tuples_since";
    if ( ! _init )
    {
        INFO << "Not initialized";
        return ptr<StreamEvent>(new StreamEvent(_name,string(),0)); // Return an empty tuple
    }

    ptr<StreamEvent> event;

    locking(_buffer_lock)
    {

        // Find the desired tuple
        char* start;

        // We were not looking for any tuple in particular so we want to get the full buffer
        if ( first_tuple.size() == 0)
        {
            INFO << "Start is begin";
            start = _begin;
        } 
        else 
        { 
            INFO << "Looking ";
            // Find the tuple that we want
            start = find(first_tuple);
            //// XXXX temporary hack by JH: ignore the tuple given: this is because there seems some cases
            //// where the first_tuple argument is badly given. It seemed like a timing problem.
            //// The first_tuple must be the last tuple acked from downstream..
            // Magda updated: ... I need the actual value for my experiments
            //start = _begin;

            if (_empty) 
            { 
                WARN << "Buffer is empty?";
                start = NULL;
            }
        }

        // We did not find the tuple that we were looking or the tuple that we wanted is
        // the last one in the buffer then we will return an empty tuple
        if ( (!start) || (start + _tuple_size == _end))
        {
            INFO << "Did not find the tuple";
            event =  ptr<StreamEvent>(new StreamEvent(_name,string(),0)); // Return an empty tuple
        }

        // Ok, we found the tuple so we need to start at the next one
        else
        {
            INFO << "Did find the tuple";
            start += _tuple_size;
            char* buffer_end = _bin_tuples->data()+_bin_tuples->max_length();
            // We might have to loop around
            if ( start >= buffer_end)
            {
                start = _bin_tuples->data() + (start - buffer_end);
            }
            event = buffer_as_event(start,_end);
        }
    }
    return event;

}

// --------------------------------------------------
/**
 * Produces a StreamEvent from all tuples between m_begin and the
 * tuple given in parameters. If the tuple is not given or is not found
 * then uses all tuples between m_begin and m_end
 */
ptr<StreamEvent> StreamBuffer::all_tuples_before(string last_tuple)
{
//    NOTICE << "all_tuples_before";
    if ( ! _init )
    {
        return ptr<StreamEvent>(new StreamEvent(_name,string(),0)); // Return an empty tuple set
    }

    ptr<StreamEvent> event;

    locking( _buffer_lock )
    {

        // Find the desired tuple
        char* end;
        // We were not looking for any tuple in particular so we want to get the full buffer
        if ( last_tuple.size() == 0)
        {
            end = _end;
        } else { // Find the tuple that we want
            end = find(last_tuple);
            WARN << "All tuples before... found tuple at: " << (end - _bin_tuples->data())/_tuple_size
                 << " while begin is at " << (_begin - _bin_tuples->data())/_tuple_size;
        }

        // We did not find the tuple that we were looking or the tuple that we wanted is
        // the first one in the buffer (end points to the beginning of the desired tuple)
        if ( (!end) || ( end == _begin) )
        {
            event =  ptr<StreamEvent>(new StreamEvent(_name,string(),0)); // Return an empty tuple
        } else
        {
            event = buffer_as_event(_begin,end);
        }
    }
    return event;

}


// --------------------------------------------------
/**
 * Return an event that contains all tuples that where buffered
 * since the last time buffer_next_as_event was called
 */
ptr<StreamEvent> StreamBuffer::next_tuples()
{
//    NOTICE << "next_tuples";
    if ( (!_init) || (_empty) )
    {
        return ptr<StreamEvent>(new StreamEvent(_name,string(),0)); // Return an empty tuple set
    }
    ptr<StreamEvent> event;
    locking ( _buffer_lock)
    {

        char* buffer_begin = _bin_tuples->data();
        char* buffer_end = _bin_tuples->data()+_bin_tuples->max_length();

        if ( (_next == _end) && (!_next_equals_begin) )
        {
            event =  ptr<StreamEvent>(new StreamEvent(_name,string(),0));
        }
        // Special case when m_next rolled over last time but we didn't buffer anything since then
        else if ( (_next == buffer_begin) && (_begin == buffer_begin)
                  && (_end == buffer_end)  && (!_next_equals_begin))
                  {
            event =  ptr<StreamEvent>(new StreamEvent(_name,string(),0));
        }
        else
        {
            _next_equals_begin = false;
            event =  buffer_as_event(_next,_end);
            _next = _end;
            char* buffer_end = _bin_tuples->data()+_bin_tuples->max_length();
            if ( _next >= buffer_end)
            {
                _next = _bin_tuples->data() + (_next - buffer_end);
            }
        }
    }
    return event;
}



// --------------------------------------------------
// Assumes calles is already hodling the locks
ptr<StreamEvent> StreamBuffer::buffer_as_event(char* start, char* end)
{
//    NOTICE << "buffer_as_event";

    ptr<StreamEvent> event;
    char* buffer_begin = _bin_tuples->data();
    char* buffer_end = _bin_tuples->data()+_bin_tuples->max_length();

    if ( start < end )
    {
        event = ptr<StreamEvent>(new StreamEvent(_name,
                                                 string(start,end),
                                                 (end-start)/_tuple_size));
    } else
    {
        int total_length = (buffer_end - start) + (end - buffer_begin);
        int nb_tuples = total_length/_tuple_size;
        event = ptr<StreamEvent>(new StreamEvent(_name,
                                                 string(start,buffer_end) +
                                                 string(buffer_begin,end),
                                                 nb_tuples));
    }
    return event;
}



// --------------------------------------------------
string StreamBuffer::last_tuple()
{
//    NOTICE << "last_tuple";

    string tuple;
    locking ( _buffer_lock)
    {
        if ( _init )
        {
            if ( (_end - _bin_tuples->data()) >= _tuple_size)
            {
                tuple =  string(_end-_tuple_size,_tuple_size);
            } else
            {
// commented out the following things because this case may happen when there's been no input to this buffer: JH        
//                ERROR << "Tuple written partly at end and partly at beginning of the buffer"
//                      << "... this should never occur!";
//                ERROR << "m_end is " << m_end - m_bin_tuples->data();
//                ERROR << "m_begin is " << m_begin - m_bin_tuples->data();
//                int bytes_begin = m_end - m_bin_tuples->data();
//                int bytes_end = m_tuple_size - bytes_begin;
//                ERROR << "bytes_begin is " << bytes_begin;
//                ERROR << "bytes ends is " << bytes_end;
//                tuple = string(m_bin_tuples->data()+m_bin_tuples->max_length()-bytes_end,bytes_end)
//                    + string(m_bin_tuples->data(),bytes_begin);
                tuple = string();   // nothing in this buffer.
            }
        }
    }
    Tuple t = Tuple::factory(tuple);
    DEBUG << "On stream " << *this << " last tuple has id " << t.get_tuple_id();

    if ( _init)
    {
        Tuple t_begin = Tuple::factory(_begin);
        DEBUG << "And id at begin is id " << t_begin.get_tuple_id();
    }
    return tuple;
}

// --------------------------------------------------
void StreamBuffer::trim(string tuple)
{
//    NOTICE << "trim";
    // If the data buffer was non-empty, it never becomes empty after trimming.
    // tuple remains in the buffer after this method is invoked.
    locking(_buffer_lock)
    {
        char* temp_begin = find(tuple);
        if ( temp_begin )
        {
            // Adjusting the position of m_next
            // This part is a bit tricky because when the buffer is full, m_begin == m_end
            // and in many cases, m_next can remain behind m_begin when we trim
            if (_begin < _end)
            {
                if (_next < temp_begin)
                {
                    _next = temp_begin;
                    _next_equals_begin = true;
                }
            }
            else
            {
                if ( (_begin < temp_begin && _begin <= _next && _next < temp_begin)
                  || (temp_begin <= _end && (_begin <= _next || _next < temp_begin)))
                  {
                    _next = temp_begin;
                    _next_equals_begin = true;
                }
            }
            _begin = temp_begin;
            if ( _begin == _end)
            {
                _empty = true;
            }
        }
    }

}

// --------------------------------------------------
void StreamBuffer::clear()
{
//    NOTICE << "clear";
    locking(_buffer_lock)
    {
        if (_init)
        {
            _begin = _bin_tuples->data();
            _next = _begin;
            _next_equals_begin = true;
            _end = _begin;
            _empty = true;
        }
    }
}

// --------------------------------------------------
// Assumes that caller takes care of locking
// Assumes tuple line up with buffer boundaries
char* StreamBuffer::find(string tuple)
{

    int header_size = HEADER_SIZE;

    // If we didn't buffer anything yet, then we can't find it for sure!
    if ( !_init )
    {
        INFO << "Not init";
        return NULL;
    }

    // If tuple is empty, then we can't find it
    if ( (int)tuple.size() <= header_size )
    {
        INFO << "Tuple shorter than header size";
        return NULL;
    }

    char* location = NULL;

    const char* ref = tuple.data();

    char* curr = _begin;
    char* buffer_end = _bin_tuples->data() + _bin_tuples->max_length();

    if ( curr >= buffer_end)
    {
        ERROR << "m_begin should never end up being set to the end of the buffer";
        curr = _bin_tuples->data() + (curr - buffer_end);
    }

    int data_size = _tuple_size-HEADER_SIZE;

    INFO << "Looking for tuple " << Tuple::factory(ref).get_tuple_id();
    INFO << "Going to access tuple " << Tuple::factory(curr).get_tuple_id();

    // we could use the tuple identifier but currently it doesn't work.
    bool at_least_one = false;
    while ( ((memcmp(curr+HEADER_SIZE, ref+HEADER_SIZE,data_size))))
    {

        curr += _tuple_size;

        // check if we went through the whole buffer. Often, that's the
        // way we start so we need to make sure we look at at least one tuple
        if ( curr == _end && at_least_one)
        {
            INFO << "Current is end ";
            break;
        }
        if ( curr >= buffer_end)
        {
            INFO << "Got to the end, looping around ";
            curr = _bin_tuples->data() + (curr - buffer_end);
        }
        INFO << "Going to access tuple " << Tuple::factory(curr).get_tuple_id();
        at_least_one = true;

    }

    if ( curr != _end)
    {
        location = curr;
        DEBUG << "Found tuple " << to_hex_string(location+HEADER_SIZE, data_size);
    } else
    {

        //int index = HEADER_SIZE;
        //int seq = *((int*)(&(tuple[index])));
        //INFO << "The seq num of that tuple is seq num " << seq;
        if ( _init)
        {
            char* buffer = _bin_tuples->data();
            int begin = (_begin - buffer)/_tuple_size;
            int end = (_end - buffer)/_tuple_size;
            int next = (_next - buffer)/_tuple_size;
            DEBUG << "The stream is " << string("StreamBuffer={") + _name + "("+begin+", "+end+", "+next+")}";
        }

        //seq = *((int*)(m_begin+index));
        //INFO << "And seq num at begin is seq num " << seq;
    }
    return location;
}

// --------------------------------------------------
char* StreamBuffer::first_new(char* buffer, int buffer_size)
{
//    NOTICE << "first_new";

    string last = last_tuple();
    if ( last.size() == 0 )
        return buffer;

    const char* tuple_data = last.data();
    //int header_size = HEADER_SIZE;
    Tuple tuple = Tuple::factory(tuple_data);
    int32 tuple_id = tuple.get_tuple_id();

    // If finds last_tuple in the buffer, return a reference to the following tuple
    for ( int i = 0; i < (buffer_size - _tuple_size); i += _tuple_size)
    {
        char* curr = buffer+i;
        //if ( strncmp(curr+header_size,tuple_data+header_size,m_tuple_size-header_size) == 0 )
        //{
        if ( Tuple::factory(curr).get_tuple_id() == tuple_id)
        {
            return (buffer + i + _tuple_size);
        }
    }
    // Otherwise, return the reference to the beginning of the buffer
    return buffer;
}

BOREALIS_NAMESPACE_END;

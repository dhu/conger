#ifndef BOREALIS_STREAMBUFFER_H
#define BOREALIS_STREAMBUFFER_H

#include "common.h"
#include "StreamEvent.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * Circular buffer to hold input and output tuples. Should be replaced
 * by connection points once they become available.
 */
class StreamBuffer
{

 private:

    Name _name;

    // Circular buffer of tuples produced or received on that stream
    bool _init; // Buffer has been initialized... default false
    ptr<dynbuf> _bin_tuples; // Buffer of tuples
    int _tuple_size; // Size of tuples in this buffer
    char* _begin; // Points to what is currently the oldest tuples
    char* _end;  // Points to what is currently the end of the most recent tuple
    char* _next; // Points to the position for the next buffer_as_next_event.
    bool _next_equals_begin;
    bool _empty; // If we buffer but trim fast enough, buffer is empty
    Mutex _buffer_lock; // Thread safety

    /**
     * @return the reference to the beginning of the given tuple in the stream
     * Ignores tuples headers in the comparison process. Returns null if tuple
     * is not in the buffer
     */
    char* find(string tuple);

    /**
     * Produce a StreamEvent from the tuples between begin and end
     */
    ptr<StreamEvent> buffer_as_event(char* begin, char* end);

 public:
    
    /// We give the StreamName to the buffer so it can produce proper StreamEvents
    StreamBuffer(Name name);
    ~StreamBuffer();
    string as_string() const;
    string repr() const;

    /**
     * Initialize the buffer of tuples
     */
    void init_buffer(int tuple_size);

    /**
     * Reset the cursor to the beginning of the buffer
     */
    void reset_next();

    /**
     * Buffer tuples produced on this stream
     */
    void buffer(const char* new_tuples, int length, int tuple_size);

    /**
     * @return true if this tuple is already in the buffer
     */
    bool is_duplicate(string tuple)
    {
        char* location;
        locking(_buffer_lock)
        {
            location = find(tuple);
        }
        return location != NULL;
    }

    /**
     * In a buffer partially filled with duplicates, finds
     * the location of the last tuple in this.m_bin_tuples
     * and returns a pointer to the first tuple following it
     * Assumes the tuples in the buffer given as parameter
     * have the same size as the tuples in our buffer
     * @return pointer to the tuple following the last tuple in
     * this.m_bin_tuples
     */
    char* first_new(char* buffer, int buffer_size);

    /**
     * Produces a StreamEvent from the content of the buffer
     * Only take all tuples succeeding first_tuple. If first_tuple
     * is not specified then return all the tuples in the buffer.
     * If first_tuple is not in the buffer then return an empty
     * set of tuples
     */
    ptr<StreamEvent> all_tuples_since(string first_tuple = string());

    /**
     * Produces a StreamEvent from all tuples between m_begin and the
     * tuple given in parameters. If the tuple is not given or is not found
     * then uses all tuples between m_begin and m_end
     */
    ptr<StreamEvent> all_tuples_before(string last_tuple = string());

    /**
     * Produces a StreamEvent from all tuples that were buffered
     * since the last time we called buffer_next_end_as_event
     * Advances m_next to m_end
     */
    ptr<StreamEvent> next_tuples();

    /**
     * @return the last tuple produced/received on the stream
     */
    string last_tuple();

    /**
     * Trims queue iff finds a tuple identical to the one
     * given as parameter. If not then does not do anything
     */
    void trim(string tuple);

    /**
     * Removes all the tuples.
     */
    void clear();

};


BOREALIS_NAMESPACE_END

#endif

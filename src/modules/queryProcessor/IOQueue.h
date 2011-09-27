#ifndef IOQUEUE_H
#define IOQUEUE_H

#include "Tuple.h"
#include "StreamBuffer.h"

BOREALIS_NAMESPACE_BEGIN


/**
 * Stores runtime information for every input stream or output stream.
 * Monitors the data that passes through the stream. Optionally, it
 * can also buffer the data
 */
class IOQueue 
{

 public:

    IOQueue(Name stream_name) :
        _stream_name(stream_name), 
        _buffer(stream_name),
        _last_insertion_tuple(NULL),
        _tuple_size(0),
        _tentative(false),
        _stabilizing(false)
    {}

    ~IOQueue();

    /// We actually buffer data only on output queues
    void clear_buffer()
    { _buffer.clear();
    }

    /// We actually buffer data only on output queues
    void buffer(char* buf, int length, size_t tuple_size)
    { _buffer.buffer(buf,length,tuple_size);
    }

    /// Return a batch of historical tuples as a StreamEvent
    ptr<StreamEvent> all_tuples_since(string first_tuple = string())
    { return _buffer.all_tuples_since(first_tuple);
    }

    /// @return true if _last_insertion_tuple holds a copy of a valid tuple
    bool is_valid_last_insertion_tuple() 
    { return ( _last_insertion_tuple != NULL ); 
    }

    /// @return the last insertion tuple that passed through the queue
    /// Pre-condition, the last insertion tuple must be valid
    const Tuple get_last_insertion_tuple()
    { return Tuple::factory(_last_insertion_tuple->data());
    }

    /// @return string containing the last INSERTION tuple
    ///         or an empty string if there is no such tuple
    string get_last_insertion_tuple_as_string();
   

    /// Did we get tentative tuples after stable tuples
    bool tentative() { return _tentative; }

    /// Are we in the process of correcting data on this stream
    bool stabilizing() { return _stabilizing; }

    /// Inspect the data that is going through this queue
    /// Update information about the most recent tuples seen
    /// @return true if data contains any TENTATIVE or UNDO
    ///         tuples and needs to be cleaned. False otherwise
    bool inspect_data(char* buf, int length, size_t tuple_size);

    /// Remove all UNDO and TENTATIVE tuples from the buffer
    /// while copying the data to the new buffer. 
    /// @return the final size of the new buffer
    int clean_data(char* src_buf, int original_length, char* dst_buf, int dst_length, size_t tuple_size);

 private:

    /// Update the value of the last insertion tuple with the one given in parameter
    void update_last_insertion_tuple(const Tuple& src_tuple, size_t tuple_size);

    /// The name of this stream
    Name _stream_name;
 
    /// Temporary buffer for tuples produced on this stream
    /// Currently, we only buffer in OutputQueues
    StreamBuffer _buffer; 
    
    /// An exact copy of the most recent INSERTION tuple on this stream
    dynbuf *_last_insertion_tuple;
    size_t _tuple_size;

    /// True between the time we see a TENTATIVE tuple and the time we see a RECONCILIATION_DONE
    bool _tentative;

    /// True between the time we see an UNDO tuple and the time we see a RECONCILIATION_DONE
    bool _stabilizing;
    
};




BOREALIS_NAMESPACE_END

#endif

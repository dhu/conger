#ifndef STREAMEVENT_H
#define STREAMEVENT_H

#include "Name.h"
#include "StreamID.h"
#include <NMSTL/net>

BOREALIS_NAMESPACE_BEGIN

class StreamEvent
{

  public:
    /// Just the name of the stream (for backward compatibility)
    ///
    Name         _stream;


    /// The complete unique stream identifier... supposed to include the version of the stream
    /// but the new catalog does not support versions.
    ///
    StreamID     _stream_id; 


    /// Because there are no versions for streams, we can optionally add
    /// information about the source of tuples.  We will check with
    /// "if (_source)" to see if it's set.
    ///
    InetAddress  _source;


    bool         _closed;


    /// true when tuples are injected by a source; false when forwarded
    ///
    bool         _inject;


    /// Sent tuples in a batch.
    ///
    int32        _inserted_count;


    /// Size of the tuple header + the tuple data.
    ///
    int32        _tuple_size;


    /// A batch of tuples as the header followed by tuple data for each tuple.
    ///
    string       _bin_tuples;


    /// Raw array data ordered by use in the tuples.
    ///
    string       _bin_arrays;



    /// An event with no settings.
    ///
    StreamEvent(Name stream = Name()) :
        _stream(stream),
        _stream_id(StreamID(stream)),
        _closed(false),
        _inject(false),
        _inserted_count(0),
        _tuple_size(0),
        _bin_tuples(string()),
        _bin_arrays(string())
    {}



    /// Stream Event consists of injecting a set of binary tuples (by name).
    /// This is used to inject control tuples; which do not have arrays.
    ///
    StreamEvent(Name           stream,
                const string  &bin_tuples,
                int32          num_tuples,
                bool           injected = false)
        : _stream(stream),
          _stream_id(StreamID(stream)),
          _closed(false),
          _inject(injected),
          _inserted_count(num_tuples),
          _bin_tuples(bin_tuples),
          _bin_arrays(string())
    {
        if (num_tuples == 0)
        {   _tuple_size = 0;
        }
        else
        {   _tuple_size = bin_tuples.size() / num_tuples;
        }
    }



    /// Stream Event consists of injecting a set of binary tuples (by id)
    ///
    StreamEvent(StreamID       stream_id,
                const string  &bin_tuples,
                const string  &bin_arrays,
                int32          num_tuples,
                bool           injected = false)
        : _stream(stream_id.get_name()),
          _stream_id(stream_id),
          _closed(false),
          _inject(injected),
          _inserted_count(num_tuples),
          _bin_tuples(bin_tuples)
    {
        if (num_tuples == 0)
        {   _tuple_size = 0;
        }
        else
        {   _tuple_size = bin_tuples.size() / num_tuples;
        }
    }



    StreamEvent(Name  stream,
                bool  closed)
        : _stream(stream),
          _stream_id(StreamID(stream)),
          _closed(closed),
          _inject(false),
          _inserted_count(0),
          _tuple_size(0),
          _bin_tuples(string()),
          _bin_arrays(string())
    {}



    string as_string() const
    {
        return string("StreamEvent{stream=") + _stream +
            "; id="             + _stream_id +
            "; source="         + _source +
            "; closed="         + _closed +
            "; inject="         + _inject +
            "; inserted_count=" + _inserted_count +
            "; tuple_size="     + _tuple_size +
            "}";
    }



    string repr() const
    {
        return as_string() + "Bin tuples = ["
            + to_escaped_string(_bin_tuples)
            + "]";
    }


    /// This is used only in creating input event tuples. Therefore,
    /// we can safely do the required padding to the nearest byte-size here.
    ///
    void insert_bin(string  bin_tuple,
                    int32   nb_tuples = 1)
    {      
        _bin_tuples += bin_tuple;
        _inserted_count += nb_tuples;
    }


    /// This is used only in creating input event tuples. Therefore,
    /// we can safely do the required padding to the nearest byte-size here.
    ///
    void insert_array(string  bin_array)
    {      
        _bin_arrays += bin_array;
    }



    ////////////////////////////////////////////////////////////////////////////
    ///
    /// Return the last tuple in this stream event
    ///
    string  last_tuple()
    {
        string  last_tuple;
    ///.........................................................................


        if (_inserted_count >= 1)
        {
            int32  buffer_size = _bin_tuples.size();
            int32  tuple_size  = buffer_size / _inserted_count;

            last_tuple = string(_bin_tuples.data() + buffer_size - tuple_size,
                                tuple_size);
        }

        return  last_tuple;
    }



    ////////////////////////////////////////////////////////////////////////////
    ///
    ///  Return the first tuple in this stream event
    ///
    string first_tuple()
    {
        string  first_tuple;
    ///.........................................................................


        if (_inserted_count >= 1)
        {
            int32  buffer_size = _bin_tuples.size();
            int32  tuple_size  = buffer_size / _inserted_count;
            first_tuple        = string(_bin_tuples.data(), tuple_size);
        }

        return  first_tuple;
    }


    NMSTL_SIMPLY_SERIALIZABLE(StreamEvent,
                              << _stream << _stream_id << _source << _closed
                              << _inject << _inserted_count << _tuple_size
                              << _bin_tuples << _bin_arrays);
};


NMSTL_TO_STRING(StreamEvent);

BOREALIS_NAMESPACE_END

#endif                       // STREAMEVENT_H

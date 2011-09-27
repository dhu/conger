#ifndef STREAM_H
#define STREAM_H

#include <vector>
#include <deque>
#include <sstream>
#include <sys/time.h>
#include "common.h"

#include "LockHolder.h"
#include "TupleQueue.h"
#include "TupleDescription.h"
#include "TuplesAvailableMailbox.h"
#include "StreamStatsSample.h"
#include "CpuStats.h"
#include "Expression.h"
#include "common.h"

BOREALIS_NAMESPACE_BEGIN

class AuroraNode;
class QBox;
class CPQBox;

/// A flow of tuples, all with the same type.  A stream is connected
/// to zero or more TupleQueues and zero or more QBoxes.  Whenever a
/// tuple is written to the stream, it is multicasted to each of these
/// TupleQueues and QBoxes.
///
/// This class is not thread-safe.
class Stream
{
  public:

    /// Constructs a stream with a given name.
    ///
    Stream( /// Name of the stream to create.  It must be in the local catalog.
        string   name,
        AuroraNode  &node
          )
     throw( AuroraException );


    /// Destructor.
    ///
    ~Stream();


    /// Returns the name of the stream.
    ///
    string get_name() const
    {
        return( to_string( _local_stream->get_stream_name() ));
    }


    /// Number of tuples that have ever been enqueued into this Stream.
    ///
    size_t get_num_tuples_enqueued() const { return _num_tuples_enqueued; }


    /// Returns the stream's description.
    ///
    const  CatalogSchema  *get_stream_schema()  const
    {   return  _local_stream->get_stream_schema();
    }


    /// Returns the stream definition in the local catalog.
    ///
    CatalogStream  *get_local_stream()  const
    {   return  _local_stream;
    }


    /// deprecated:  Returns the stream's description.
    ///
    const  TupleDescription  &get_tuple_description()  const
    {   return  _desc;
    }


    /// Adds a TupleQueue not associated with a box.  Whenever a tuple
    /// is written to this stream, the tuple will be copied into the
    /// TupleQueue.
    ///
    void  add_queue(TupleQueue* q);


    /// Removes a TupleQueue.
    void  remove_queue(TupleQueue*);


    /// Clears all queues this stream outputs to.
    void  clear_queues()
    {   _queues.clear();
    }


    // Check if all queues are empty. -- tatbul
    //
    bool all_queues_empty()
    {
        for (vector<TupleQueue*>::iterator i = _queues.begin(); 
             i != _queues.end(); ++i)
        {
            if ((*i)->size() > 0)
            {
                return  false;
            }
        }

        return  true;
    }


    /// Returns a vector of all TupleQueues attached to the stream.
    ///
    const vector<TupleQueue*>& get_queues()
    {   return  _queues;
    }


    /// Returns a vector of all box input ports attached to the stream.
    ///
    const vector<BoxPort>  *get_sink_port()
    {
        return  _local_stream->get_sink_port();
    }


    /// Returns the box output port that is feeding input tuples to
    /// this stream.  May be an empty QBoxOutputPort, in which case
    /// <i>no</i> box precedes this stream in the catalog, i.e., this
    /// is an input stream.
    ///
    const BoxPort  *get_source_port()  const
    {
        // fix up???????????????
        // even intermediate streams will be assigned schemas.
        //
        if ( _local_stream->is_input_stream() )   // IF an input stream,
        {   return  NULL;
        }
        else                            // ELSE an intemediate (box out) stream,
        {   return  _local_stream->get_stream_source();
        }
    }


    /// An iterator used to write data into a Stream.
    ///
    class EnqIterator
    {
      public:
        /// Dummy contsructor.
        ///
        EnqIterator() : _stream(0) {}


        /// Returns a pointer to a tuple-sized chunk of memory
        /// to which data may be written.
        ///
        void *tuple() const
        {   ASSERT(_stream);

            return  _stream->_buf;
        }


        /// Indicates that the tuple to which this iterator points has
        /// been populated and may be input to the stream.
        ///
        inline EnqIterator& operator ++ ()
        {
            ASSERT(_stream);
            _stream->enqueue_tuple();

            return  *this;
        }


      private:
        EnqIterator(Stream& stream) : _stream(&stream) {}
        Stream  *_stream;

        friend class Stream;
    };


    friend class EnqIterator;


    /// Returns an EnqIterator for the stream.
    ///
    EnqIterator enq_iterator()
    {   return  EnqIterator(*this);
    }


    /// Returns a string representation of the stream.
    ///
    string as_string() const;


    /// A description of how to provide the schema in an XML representation
    /// of a stream.
    ///
    enum schema_representation
    {
        /// Provide the schema name only.
        SCHEMA_NAME,

        /// Provide the full schema if the stream is anonymous, else just
        /// the schema name.
        FULL_SCHEMA_IF_ANONYMOUS,

        /// Always provide the full schema of the stream.
        FULL_SCHEMA
    };


    /// Returns an XML representation of the stream.
    ///
    string as_xml(schema_representation rep = FULL_SCHEMA_IF_ANONYMOUS) const;


    /// Calls the notifyEnq() method for each TupleQueue attached to
    /// this stream (including TupleQueues belonging to box's input
    /// ports).  This notifies any listeners on those TupleQueues that
    /// tuples may be ready for consumption.
    ///
    void notify_enq();

    void begin_buffering();

    void stop_buffering();


    /// methods to access CP on this stream
    ///
    void set_cp(ptr<CPQBox> cp);
    ptr<CPQBox> get_cp();


  private:
    void enqueue_tuple();

    /// The AuroraNode whose query network uses this Stream.
    ///
    AuroraNode  &_node;
    const CpuStats  &_cpu_stats;

    /// Buffer returned by the iterator
    ///
    void *_buf;


    /// this is just a expression elimination. The function would
    /// send the _buf to all outputs.
    ///
    void push_tuple( const void *_buf );


    size_t _num_tuples_enqueued;


    /// New declarations in the catalog.
    ///
    CatalogStream      *_local_stream;

    TupleDescription    _desc;              //remove this at some point???

    vector<TupleQueue*> _queues;


    /// this flag tells the stream to start buffering tuples (instead of
    /// forwarding them to queues) until further notice.
    ///
    bool   _buffering;


    /// tuple queue to keep buffered tuples. Will use the same page pool
    /// so that storage manager has no surprises
    ///
    TupleQueue   *_tuple_buffer;


    /// pointer to the CP on this stream
    ///
    ptr<CPQBox> _cp;
};

BOREALIS_NAMESPACE_END

#endif                              // STREAM_H

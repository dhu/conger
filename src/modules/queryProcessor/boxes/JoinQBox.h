#ifndef JOIN_QBOX_H
#define JOIN_QBOX_H

#include "QBox.h"
#include "Tuple.h"
#include <list>

#define SELECTIVITY_VECTOR_LIMIT 100

BOREALIS_NAMESPACE_BEGIN

/**
 * TODO:
 * - Window by values.
 * - Order by and slack.
 */
class JoinQBox : public QBox
{
    friend class JoinBoxState;

    // SJoinQBox is a kind of JoinBox. It manipulates the _buffer directly
    friend class SJoinQBox;

  public:

    // Get the join selectivity of the left stream. -- tatbul
    // (vector of all values collected since last call)
    //
    void get_left_join_selectivity(vector<double> &left_sel)
    {
        left_sel = _join_selectivity[LEFT];
        _join_selectivity[LEFT].clear();
    }

    // Get the join selectivity of the left stream. -- tatbul
    // (average and count of all values collected since last call)
    //
    void get_left_join_selectivity(double *average, int *count)
    {
        *count = _join_selectivity[LEFT].size();
        if (*count > 0)
        {
            *average = _join_selectivity_sum[LEFT]/double(*count);
            _join_selectivity[LEFT].clear();
            _join_selectivity_sum[LEFT] = 0;
        }
        else
        {
            *average = 0;
        }
    }

    // Get the join selectivity of the right stream. -- tatbul
    // (vector of all values collected since last call)
    //
    void get_right_join_selectivity(vector<double> &right_sel)
    {
        right_sel = _join_selectivity[RIGHT];
        _join_selectivity[RIGHT].clear();
    }

    // Get the join selectivity of the right stream. -- tatbul
    // (average and count of all values collected since last call)
    //
    void get_right_join_selectivity(double *average, int *count)
    {
        *count = _join_selectivity[RIGHT].size();
        if (*count > 0)
        {
            *average = _join_selectivity_sum[RIGHT]/double(*count);
            _join_selectivity[RIGHT].clear();
            _join_selectivity_sum[RIGHT] = 0;
        }
        else
        {
            *average = 0;
        }
    }

  protected:
    virtual void setup_impl() throw (AuroraException);
    virtual void init_impl() throw (AuroraException);
    virtual void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state);

    /// Constants for keeping track of the IDs for left and right.
    // TODO: Consider typesafe enumeration.
    static const int LEFT;
    static const int RIGHT;

  private:

    /// Print a tuple by stream id.
    string tuple_to_string(int stream_id, const void *data)
    {
        //TupleDescription  stream_desc( getInDescription( stream_id ));
        TupleDescription  stream_desc( get_in_schema( stream_id ));
        return( stream_desc.tuple_to_string( data ));
    }

    /// Run a stream.
    void run_stream( QBoxInvocation  &inv, int  stream_id, bool  &outputted );

    /// Emit a tuple based on the tuples loaded in _ctxt to enq(0).
    void emit_tuple( EvalContext  &ctxt );

    /// Just produce the tuple but don't emit it
    virtual void produce_dont_emit_tuple( EvalContext  &ctxt );

    /// Timeout information.
    bool       _timing_out;
    Timestamp  _timeout;

    // Structure for storing a tuple in the buffer.
    class BufferElement
    {
      public:
        const void *get_data() const
        {
            //return _tuple.getData();
            return _data.data();
        }

        const Tuple &getTuple() const
        {
            if (_tuple.get_data() != (uint8 *)_data.data())
            { FATAL << "Tuple points to wrong data";
            }
            return _tuple;
        }

        Timestamp get_timestamp() const
        {   return _tuple.get_timestamp();
        }

        int get_field_value() const
        {   return _field_value;
        }

        // Get, Add, and Increment methods for selectivity counts. -- tatbul
        //
        int get_comparison_count()
        {
            return(_comparison_count);
        }

        int get_match_count()
        {
            return(_match_count);
        }

        void add_comparison_count(int count)
        {
            _comparison_count += count;
        }

        void add_match_count(int count)
        {
            _match_count += count;
        }

        void incr_comparison_count()
        {
            ++_comparison_count;
        }

        void incr_match_count()
        {
            ++_match_count;
        }
        //
        //

        BufferElement( const void  *data,
                           size_t   length,
                              int   field_value = 0 ) :

            _data((const char *)data, length),
            //_tuple(Tuple::factory( data )),  --> not data will go away!!!
            _tuple(Tuple::factory( _data)),
            _field_value(field_value),
            _comparison_count(0),
            _match_count(0) {}

      private:
        const string  _data;
        const Tuple   _tuple;
        const int     _field_value;
        int              _comparison_count;    // for selectivity -- tatbul
        int              _match_count;            // for selectivity -- tatbul
    };


    class JoinBuffer
    {
      public:
        JoinBuffer( int stream_id, JoinQBox *qbox ) :
            _stream_id( stream_id ),
            _qbox( qbox ),
            _tuple_size(_qbox->get_in_schema(_stream_id)->get_size()
                        + HEADER_SIZE),
            //_tuple_size( _qbox->getInDescription( _stream_id )->getSize()),
            _ctxt( 2 )
        {}

        virtual ~JoinBuffer() {}

        virtual void insert_tuple( const void  *tuple_data );

        void join(const void *other_tuple,
                  Expression *predicate,
                  bool &outputted,
                  int *comparison_count,
                  int *match_count);

        /// Writes joined tuples at first position of output queue. Doesn't signal their presence
        void join_no_emit( const void  *other_tuple,
                         Expression  *predicate,
                         bool  &outputted );

        void clear();

        int size() { return _buffer.size(); }

        int dump(dynbuf& dst); // Copy tuples to buffer and return nb tuples copied

        int get_oldest();

        void init(const dynbuf& src, int nb_tuples); // Insert the tuples from this buffer into self

        // -- tatbul
        //
        BufferElement *back()
        {
            return(&(_buffer.back()));
        }

      protected:
        typedef list<BufferElement> BufferList;
        BufferList  _buffer;
        int         _stream_id;
        JoinQBox   *_qbox;

      private:
        size_t       _tuple_size;
        EvalContext  _ctxt;
    };

    friend class JoinBuffer;

    // Subclass for ORDER_BY_VALUES
    class TupleCountJoinBuffer : public JoinBuffer
    {
      public:
        TupleCountJoinBuffer(int stream_id, JoinQBox *qbox,
                             size_t buffer_size) :
            JoinBuffer(stream_id, qbox),
            _buffer_size(buffer_size)
        { }
        virtual void insert_tuple(const void *tuple_data);
      private:
        size_t _buffer_size;
    };

    // Subclass for ORDER_BY_FIELD
    template<typename T>
    class FieldJoinBuffer : public JoinBuffer
    {
      public:
        FieldJoinBuffer(int stream_id, JoinQBox *qbox,
                        ptr<Expression> field,
//                        size_t buffer_size) :
                        T buffer_size) :
            JoinBuffer(stream_id, qbox), _field(field),
            _buffer_size(buffer_size)
        {
//            if (!_field->is<int32>())
//                Throw (aurora_typing_exception,
//                       "Order by field value must be of type int.");
            if ( !_field->is_numeric() )
                Throw (aurora_typing_exception,
                       "Order by field value must be a numeric type.");

            if ( !_field->template is<T>() )
                Throw(aurora_typing_exception,
                    "Order by field cannot be evaluated as a " +
                    string(typeid(T).name()));
        }

        virtual void insert_tuple(const void *tuple_data);
      private:
        T field_value(const void *tuple_data);

        ptr<Expression> _field;
//        size_t _buffer_size;
        T _buffer_size;
        EvalContext _ctxt;
    };

 protected: // -- magda the stuff below should be protected so subclasses can manipulate the buffers directly

    // Create "left" and "right" buffers.
    ptr<JoinBuffer> create_buffer_by_parameter(int stream_id);

    // Left and right join buffers.
    ptr<JoinBuffer> _buffer[2];

    ptr<Expression> _predicate;

    // What kind of output.
    bool _just_concat;
    vector<ptr<Expression> > _output_expressions;

    // Cached information about tuples
    unsigned int _left_body_size, _right_body_size;

    // Join selectivity stats. -- tatbul
    //
    vector<double> _join_selectivity[2];// 0: LEFT, 1: RIGHT (as defined above)
    double _join_selectivity_sum[2];     // 0: LEFT, 1: RIGHT (as defined above)
};



/**
 * Captures the state of a join operator
 */
class JoinBoxState : public AbstractBoxState
{
 public:
  JoinBoxState() : _left_nb_tuples(0), _right_nb_tuples(0) {}
  JoinBoxState(const dynbuf& left, int nb_left, const dynbuf& right, int nb_right) :
      _left_buffer(left), _left_nb_tuples(nb_left),
      _right_buffer(right), _right_nb_tuples(nb_right)
      {
          // The copy constructor of a dynbuff makes another reference to
          // the other one's underlying storage. The basic dynbuf constructor
          // uses malloc so the dynbufs are always on the heap
          //memcpy(_left_buffer.data(),left->data(),left->max_length());
          //memcpy(_right_buffer.data(),right->data(),right->max_length());
      }
  ~JoinBoxState() {}

  NMSTL_SERIAL_SUBCLASS(JoinBoxState, AbstractBoxState, << _left_buffer << _left_nb_tuples
                      << _right_buffer << _right_nb_tuples);

  const dynbuf& get_left() const { return _left_buffer; }
  int get_nb_left_tuples() const { return _left_nb_tuples; }

  const dynbuf& get_right() const { return _right_buffer; }
  int get_nb_right_tuples() const { return _right_nb_tuples; }


 private:
  dynbuf _left_buffer;
  int _left_nb_tuples;

  dynbuf _right_buffer;
  int _right_nb_tuples;

};


BOREALIS_NAMESPACE_END

#endif // JOIN_QBOX_H

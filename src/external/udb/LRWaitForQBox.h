#ifndef  LRWAITFORQBOX_H
#define  LRWAITFORQBOX_H

#include "Tuple.h"
#include <ext/hash_map>
#include <vector>
#include <list>
#include "QBox.h"


/**
 * This box is a waitfor modification for linear road. One stream buffers tuples.
 * The other stream is like a "control stream" , providing a new value for
 * some key "n" (int). Buffered tuples are let through if one of their fields
 * is <= n.
 *
 * Specifically - the control stream updates the "key field value" (n)
 * This box always lets through tuples that have key field <= n
 *
 * At startup, without any control tuples, all data is buffered
 *
 * Data comes from input stream 0
 * "Control" comes from input stream 1
 */

BOREALIS_NAMESPACE_BEGIN

using __gnu_cxx::hash_map;


class LRWaitForQBox : public QBox
{

  private:

    struct HashString
    {
        size_t  operator()( const string  s ) const
        {
            // TODO: Find a better hash function or something.
            uint32  h = 0;

            for ( string::const_iterator i = s.begin() ; i != s.end(); ++i )
            {   h = (5 * h) + *i;
            }

            return( size_t( h ));
        }
    };


    class StoredTuple
    {
      public:
        StoredTuple( const char  *tuple, size_t  size, int  key ):
               _key( key )
        {
            _tuple = new char[ size ];
            memcpy( _tuple, tuple, size );
        }

        char  *getTuple() { return( _tuple ); }

        int    get_key()   { return( _key ); }

    private:
        char  *_tuple;
        int    _key;
    };


    // Maps group-by to n
    typedef hash_map<string, int32, HashString>  MinuteHash;
    MinuteHash  _max_keys;

    // Maps group-by to hash of keys to tuple buffer
    typedef list<ptr<StoredTuple> > TuplesList;
    typedef hash_map<int, TuplesList> KeyedTuples;
    typedef hash_map<string, TuplesList, HashString > tuples_hash;
    typedef hash_map<string, KeyedTuples, HashString > tuples_hash_2;
    tuples_hash _buffers;
    tuples_hash_2 _buffers2;

    // Size of the group_by values from the data stream
    size_t _group_by_value_size_data;
    // Group by a set of attributes from the data stream
    vector<ptr<Expression> > _group_by_fields_data;

    // Size of the group_by values from the data stream
    size_t _group_by_value_size_control;
    // Group by a set of attributes from the data stream
    vector<ptr<Expression> > _group_by_fields_control;

    // A starting keyval, if desired
    // Note that a control key value OVERRIDES the default value (i.e. default only is used while you have not heard a control key)
    bool _use_start_keyval;
    int _start_keyval;
    bool _use_data_as_first_keyval; // see comment in setupImpl section

    // Tuple sizes
    unsigned int  _output_tuple_size;
    unsigned int  _data_tuple_size;

    // Access to the minute field of both streams
    ptr<Expression>  _data_key_field;
    ptr<Expression>  _control_key_field;

    string  group_by_for_tuple_data(const EvalContext& ctxt) const;
    string  group_by_for_tuple_control(const EvalContext& ctxt) const;

    void  parse_group_by_data(const string atts, const ExprContext& ctxt);
    void  parse_group_by_control(const string atts, const ExprContext& ctxt);
    void  emit_tuple(const char* tuple);


  protected:

    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};


BOREALIS_NAMESPACE_END

#endif                     // LRWAITFORQBOX_H

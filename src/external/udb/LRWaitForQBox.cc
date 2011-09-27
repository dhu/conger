#include "LRWaitForQBox.h"


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


////////////////////////////////////////////////////////////////////////////////
//
string LRWaitForQBox::group_by_for_tuple_data(const EvalContext& ctxt) const
{

    char buffer[_group_by_value_size_data];
    char *pos = buffer;

    vector<ptr<Expression> >::const_iterator gbi;
//..............................................................................


    for (gbi = _group_by_fields_data.begin(); gbi != _group_by_fields_data.end(); ++gbi)
    {
        (*gbi)->eval_into(pos, ctxt);
        pos += (*gbi)->getLength();
    }

    return string(buffer, _group_by_value_size_data);

}



////////////////////////////////////////////////////////////////////////////////
//
string LRWaitForQBox::group_by_for_tuple_control(const EvalContext& ctxt) const
{
    char buffer[_group_by_value_size_control];
    char *pos = buffer;
    vector<ptr<Expression> >::const_iterator gbi;
//..............................................................................


    for (gbi = _group_by_fields_control.begin(); gbi != _group_by_fields_control.end(); ++gbi)
    {
        (*gbi)->eval_into(pos, ctxt);
        pos += (*gbi)->getLength();
    }
    return string(buffer, _group_by_value_size_control);

}



////////////////////////////////////////////////////////////////////////////////
//
void LRWaitForQBox::parse_group_by_data(const string atts, const ExprContext& ctxt)
{
//..............................................................................


    DEBUG << "Got DATA group by of " << atts;
    vector<string> parts;
    split(atts, ',', parts);

    _group_by_value_size_data = 0;
    vector<string>::const_iterator p;
    for (p = parts.begin(); p != parts.end(); p++)
    {
        DEBUG << "Parsing field " << *p;
        ptr<Expression> f = Expression::parse(*p, ctxt);
        _group_by_fields_data.push_back(f);

        if (f->getLength() < 0)
            Throw(AuroraException, "group-by fields must be fixed-length");
        _group_by_value_size_data += f->getLength();
    }
}



////////////////////////////////////////////////////////////////////////////////
//
void LRWaitForQBox::parse_group_by_control(const string atts, const ExprContext& ctxt)
{
//..............................................................................


    DEBUG << "Got CONTROL group by of " << atts;
    vector<string> parts;
    split(atts, ',', parts);

    _group_by_value_size_control = 0;
    vector<string>::const_iterator p;
    for (p = parts.begin(); p != parts.end(); p++)
    {
        DEBUG << "Parsing field " << *p;
        ptr<Expression> f = Expression::parse(*p, ctxt);
        _group_by_fields_control.push_back(f);

        if (f->getLength() < 0)
            Throw(AuroraException, "group-by fields must be fixed-length");
        _group_by_value_size_control += f->getLength();
    }
}



////////////////////////////////////////////////////////////////////////////////
//
void LRWaitForQBox::emit_tuple(const char* tuple)
{
//..............................................................................


    EnqIterator ei = enq(0);
    TupleDescription in_td(get_in_schema(0));
    memcpy(ei.tuple(),tuple, _data_tuple_size);
    DEBUG << "LRWaitForQBox OUT: " << in_td.tuple_to_string(tuple);
    ++ei;
}



////////////////////////////////////////////////////////////////////////////////
//
void LRWaitForQBox::setup_impl() throw (AuroraException)
{
    ExprContext ctxt;
//..............................................................................


    if (get_num_inputs() != 2)
    {   Throw(aurora_typing_exception,
              "LRWaitFor requires exactly two input streams (not "
                  + to_string(get_num_inputs()) + ")");
    }

    string keyfield = param("key-field",PARAM_REQUIRED);
    string groupby  = param("group-by");

    // Group by and the minute field from the data stream
    ctxt.set_context_schema(*get_in_schema(0));
    parse_group_by_data(groupby, ctxt);
    _data_key_field = Expression::parse(keyfield, ctxt);

    // Group by and the minute field from the data stream
    ctxt.set_context_schema(*get_in_schema(1));
    parse_group_by_control(groupby, ctxt);
    _control_key_field = Expression::parse(keyfield, ctxt);


    // Allow a parameter to define the initial value for the key
    // TODO - Consider that here, this applies for ALL GROUP BYs. That may not be what is desired
    //         but then, what can you do?
    _use_start_keyval = typed_param("start-key-value", _start_keyval, PARAM_NOT_REQUIRED);

    // Allow a special mode that lets the FIRST data tuple for some group by through
    //  and takes the keyfield from that data tuple to be its first control value
    _use_data_as_first_keyval = false;
    typed_param("use-data-as-first-key-value", _use_data_as_first_keyval, PARAM_NOT_REQUIRED);

    if (_use_data_as_first_keyval)
    {
        DEBUG << "Will use the first data key value as first control value";
    }

    // The output is simply the tuples from the data stream
    set_out_description(0, get_in_stream(0));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void LRWaitForQBox::init_impl() throw (AuroraException)
{
//..............................................................................


    TupleDescription in_td(get_in_schema(0));
    TupleDescription out_td(get_out_schema(0));

    _output_tuple_size = out_td.get_size();
    _data_tuple_size = in_td.get_size();
}



////////////////////////////////////////////////////////////////////////////////
//
void LRWaitForQBox::run_impl( QBoxInvocation  &inv )
                      throw( AuroraException )
{
    DeqIterator di;
    EvalContext ctxt;

    bool outputted = false;
//..............................................................................


    /**
     * Tuples from stream 1 (CONTROL STREAM) are looked at first to update the max_keys
     * You then go through the buffer to look for people to emit
     * You finally read every tuple on stream 0 (data) and either emit it, or buffer it
     */
    TupleDescription in1_td(get_in_schema(1));


    /** GO THROUGH CONTROL STREAM UPDATING MAX_KEYS */
    di = deq( 1 );
    unsigned int max_tuples = inv.get_max_tuples_to_dequeue(1);

    while ( max_tuples-- && di.avail() )
    {
        char* control_tuple = (char*) di.tuple();

        DEBUG << "LRWaitForQBox CONTROL IN: " << in1_td.tuple_to_string(control_tuple);

        ctxt.reset();
        ctxt.set_tuple( control_tuple );
        string group_by = group_by_for_tuple_control(ctxt);
        int32  control_key = _control_key_field->eval<int32>(ctxt);


        // Accept control ONLY IF ITS LARGER THAN BEFORE, unless its the first time
        //
        if ( _max_keys.find( group_by ) != _max_keys.end() )
        {   if ( control_key > _max_keys[ group_by ])
            {    _max_keys[ group_by ] = control_key;
            }
        }
        else
        {   _max_keys[ group_by ] = control_key;
        }

        ++di;
    }

    /** GO THROUGH BUFFERS LOOKING FOR PEOPLE TO EMIT **/
    for ( MinuteHash::iterator  i = _max_keys.begin();
          i != _max_keys.end();
          ++i )
    {
        string  groupby_key = (*i).first;       // key is a groupby string
        int  max_key = (*i).second;  // this is the max minute for that group by

        KeyedTuples  &keyed_tuples = _buffers2[ groupby_key ];  // the keyed data tuples held corresponding to the current groupby

        KeyedTuples::iterator  j = keyed_tuples.begin();

        while ( j != keyed_tuples.end() )
        {   int data_key = (*j).first;

            if ( data_key <= max_key )
            {
                // Data key <= max_key, so emit the entire list
                TuplesList& tuples_out = (*j).second;
                TuplesList::iterator t = tuples_out.begin(); // Go through the list (if any) - all these are matches, output them!

                while ( t != tuples_out.end() )
                {   outputted = true;
                    emit_tuple((*t)->getTuple());
                    t = tuples_out.erase( t );
                }

                ++j;
            }
            else
            {   ++j;
            }
        }

        /**
        // Now must try every valid key (from 0 to max_key inclusive) to look for every list
        // TODO ---- THIS ASSUMES KEYS ARE NEVER NEGATIVE (FIX: Get all keys from the hash)
        for ( int ii = 0; ii <= max_key; ++ii )
        {
            TuplesList  &tuples = keyed_tuples[ ii ];  // The list of tuples for this key

            TuplesList::iterator j = tuples.begin(); // Go through the list (if any) - all these are matches, output them!

            while ( j != tuples.end() )
            {   outputted = true;
                emitTuple((*j)->getTuple());
                j = tuples.erase( j );
            }
        }
        */
    }

    /**
       for ( MinuteHash::iterator i = _max_keys.begin();
             i != _max_keys.end();
             ++i )
       {
           string   key = (*i).first;   // key is a groupby string
           int  max_key = (*i).second;  // this is the max minute for that group by

           TuplesList& tuples = _buffers[ key ]; // the data tuples held corresponding to the current groupby
           TuplesList::iterator j = tuples.begin();

           while ( j != tuples.end() )
           {
               ctxt.reset();
               ctxt.setTuple((*j)->getTuple());

               int  data_key = (*j)->getKey(); //  _data_key_field->eval<int32>(ctxt);

               // Data
               if ( data_key  <= max_key )
               {   outputted = true;
                   emitTuple((*j)->getTuple());
                   j = tuples.erase( j );
               }
               else
               {   ++j;
               }
           }
       }
        */


    TupleDescription in0_td(get_in_schema(0));

    /** GO THROUGH ALL NEW DATA TUPLES AND EMIT OR BUFFER */
    di = deq( 0 );
    max_tuples = inv.get_max_tuples_to_dequeue( 0 );

    while (max_tuples-- && di.avail())
    {
        char* data_tuple = (char*) di.tuple();

        DEBUG << "LRWaitForQBox DATA IN: " << in0_td.tuple_to_string(data_tuple);

        ctxt.reset();
        ctxt.set_tuple( data_tuple );

        string  group_by = group_by_for_tuple_data( ctxt );
        int32   data_key = _data_key_field->eval<int32>( ctxt );

        // If you know a control value for this group by
        if ( _max_keys.find( group_by ) != _max_keys.end() )
        {
            if ( data_key <= _max_keys[ group_by ]) // and the key val <= control val, pass
            {
                DEBUG << "DATA tuple has key value " << data_key << " <= known control value " << _max_keys[group_by] << " for group by " << group_by;
                outputted = true;
                emit_tuple( data_tuple );
            }
            else                             // else buffer
            {   DEBUG << "DATA tuple key value " << data_key << " > known control value " << _max_keys[group_by] << " for group by " << group_by;
                DEBUG << "Buffering DATA tuple";
                // Buffer the tuple
                ptr<StoredTuple> p(new StoredTuple(data_tuple, _data_tuple_size, data_key));
                KeyedTuples& k = _buffers2[group_by];
                k[data_key].push_back(p);
            }

            ++di;
            continue;
        }

        // If you allow the data to provide the first control value, implicit pass and set of control value
        if ( _use_data_as_first_keyval )
        {
            DEBUG << "Taking first control value of " << data_key
                  << " from DATA tuple, and passing, for group by " << group_by;

            _max_keys[ group_by ] = data_key;
            outputted = true;
            emit_tuple( data_tuple );
            ++di;
            continue;
        }

        // Try looking for a default, and key <= n, emit
        if ( _use_start_keyval && data_key <= _start_keyval )
        {
            DEBUG << "Passing DATA tuple due to key value " << data_key << " <= default start key-value (for all groups) of " << _start_keyval;
            outputted = true;
            emit_tuple( data_tuple );
            ++di;
            continue;
        }


        // Oh well! Buffer
        DEBUG << "Buffering DATA tuple";
        // Buffer the tuple
        ptr<StoredTuple>  p( new StoredTuple( data_tuple,
                                              _data_tuple_size,
                                              data_key ));

        KeyedTuples& k = _buffers2[ group_by ];
        k[ data_key ].push_back( p );
        ++di;
    }

    /**
    di = deq( 0 );
    max_tuples = inv.getMaxTuplesToDequeue( 0 );

    while ( max_tuples-- && di.avail() )
    {
        char* data_tuple = (char*) di.tuple();

        DEBUG << "LRWaitForQBox DATA IN: "
              << getInputDescription(0).tupleToString( data_tuple );

        ctxt.reset();
        ctxt.setTuple( data_tuple );
        string  group_by = groupByForTupleData( ctxt );
        int32   data_key = _data_key_field->eval<int32>( ctxt );

        // If you know a control value for this group by
        if ( _max_keys.find( group_by ) != _max_keys.end() )
        {
            if ( data_key <= _max_keys[ group_by ]) // and the key val <= control val, pass
            {
                DEBUG << "DATA tuple has key value " << data_key << " <= known control value " << _max_keys[group_by] << " for group by " << group_by;
                outputted = true;
                emitTuple( data_tuple );
            }
            else                                    // else buffer
            {   DEBUG << "DATA tuple key value " << data_key << " > known control value " << _max_keys[group_by] << " for group by " << group_by;
                DEBUG << "Buffering DATA tuple";

                // Buffer the tuple
                ptr<StoredTuple>  p( new StoredTuple( data_tuple,
                                                      _data_tuple_size,
                                                      data_key ));

                _buffers[ group_by ].push_back( p );
            }

            ++di;
            continue;
        }

        // If you allow the data to provide the first control value, implicit pass and set of control value
        if ( _use_data_as_first_keyval )
        {
            DEBUG << "Taking first control value of " << data_key
                  << " from DATA tuple, and passing, for group by " << group_by;

            _max_keys[ group_by ] = data_key;
            outputted = true;
            emitTuple( data_tuple );
            ++di;
            continue;
        }

        // Try looking for a default, and key <= n, emit
        if ( _use_start_keyval && data_key <= _start_keyval )
        {
            DEBUG << "Passing DATA tuple due to key value " << data_key
                  << " <= default start key-value (for all groups) of "
                  << _start_keyval;

            outputted = true;
            emitTuple( data_tuple );
            ++di;
            continue;
        }


        // Oh well! Buffer
        DEBUG << "Buffering DATA tuple";

        // Buffer the tuple
        ptr<StoredTuple>  p( new StoredTuple( data_tuple,
                                              _data_tuple_size,
                                              data_key ));

        _buffers[ group_by ].push_back( p );
        ++di;
    }

    */

    if ( outputted )
    {   get_output(0).notify_enq();
    }
}


BOREALIS_NAMESPACE_END

/*
 * Caveat Emptor
 * This box has not been run by many networks and users, as such this box
 * may not be perfect, may crash, etc. Please remove this comment when
 * this box has been thoroughly tested. - eddie
 *

TODO: expressionize this box. Needs a rewrite like SimpleAgg
 */

#include "ResampleQBox.h"
#include "parseutil.h"

BOREALIS_NAMESPACE_BEGIN;

void ResampleQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 2)
        Throw(aurora_typing_exception,
              "Resample requires exactly two input streams (not " +
              to_string(get_num_inputs()) + ")");

    // Mandatory stuff
    string s;
    int i;

    typed_param("window-size", i, PARAM_NON_EMPTY); // WINDOW SIZE
    _window_size = i;

    s = param("left-order-by", PARAM_NON_EMPTY);// ORDER BY ("TUPLENUM" or "FIELD")
    if (s == "TUPLENUM")
    {
        _LEFT_ORDER_BY_TUPLENUM = true;
        // we'll fake a field extract object, to be used on the tuple_counter "tuple"
        _left_field_att = new FieldExt(":0:i:-12:4:"); // -12 cuz fieldext adds 12 (and yes, atoi can parse negative numbers)
    }
    else if (s == "FIELD")
    {
        _LEFT_ORDER_BY_TUPLENUM = false;
        s = param("left-order-on-field", PARAM_NON_EMPTY);
        _left_field_att = new FieldExt(s.c_str());
    }
    else Throw(aurora_typing_exception,"Join parameter left-order-by must be either TUPLENUM or FIELD (not "+s+")");
    _left_order_att_type = _left_field_att->getType();
    if (_left_order_att_type != 'i' && _left_order_att_type != 't')
        Throw(aurora_typing_exception,
              "Resample requires only allows integer and timestamp types for order-on attributes");
    _left_order_att_size = _left_field_att->get_returned_size();
    typed_param("left-slack", i, PARAM_NON_EMPTY);  // LEFT SLACK
    _left_slack = i;

    s = param("right-order-by", PARAM_NON_EMPTY);// ORDER BY ("TUPLENUM" or "FIELD")
    if (s == "TUPLENUM")
    {
        _RIGHT_ORDER_BY_TUPLENUM = true;
        // we'll fake a field extract object, to be used on the tuple_counter "tuple"
        _right_field_att = new FieldExt(":0:i:-12:4:"); // -12 cuz fieldext adds 12 (and yes, atoi can parse negative numbers)
    }
    else if (s == "FIELD")
    {
        _RIGHT_ORDER_BY_TUPLENUM = false;
        s = param("right-order-on-field", PARAM_NON_EMPTY);
        _right_field_att = new FieldExt(s.c_str());
    }
    else Throw(aurora_typing_exception,"Join parameter right-order-by must be either TUPLENUM or FIELD (not "+s+")");
    _right_order_att_type = _right_field_att->getType();
    if (_left_order_att_type != 'i' && _left_order_att_type != 't')
        Throw(aurora_typing_exception,
              "Resample requires only allows integer and timestamp types for order-on attributes");
    _right_order_att_size = _right_field_att->get_returned_size();
    typed_param("right-slack", i, PARAM_NON_EMPTY);  // RIGHT SLACK
    _right_slack = i;



    /**

        // Need at least one agg function
    ExprContext ctxt;
    ctxt.setTupleDescription(getInputDescription(0));
    s = param("aggregate-function",PARAM_NON_EMPTY);
    if (s == "")
        Throw(AuroraTypingException,"Aggregate box must have an 'aggregate-function' defined");
    _resample_function = Aggregate::parse(s,ctxt);


    // Set the output stream description, based on the function (see MapQBox.C for some discussion on how to do this)
    vector<TupleDescription::Field> fields;

    // OUTPUT TYPE IS : order-on agg-function-return-types

    // ORDER-ON field
    fields.push_back(TupleDescription::Field("orderedon", (_left_field_att->getReturnTypes())[0], _left_order_att_size));


    // AGGREGATE RETURN TYPES
    for (int i = 0; i < _resample_function->numOutputFields(); ++i)
    {

        string name("agg[0]." + to_string(i));
        TupleDescription::Field f_field = _resample_function->getOutputField(i);
        TupleDescription::Field my_field(name, f_field.getType(),
                                         f_field.getSize(), f_field.getOffset());
        fields.push_back(my_field);
    }


    */



    // the new parsing now
    parse* parser = new parse();
    // Need at least one agg function
    s = param("aggregate-function",PARAM_NON_EMPTY);
    if (s == "")
        Throw(aurora_typing_exception,"Aggregate box must have an aggregate function defined");
    _resample_function = (aggregate_function*) parser->parse_af(s.c_str());


    // Set the output stream description, based on the function (see MapQBox.C for some discussion on how to do this)
    vector<TupleDescription::Field> fields;
    vector<DataType> types_from_agg = _resample_function->get_return_types();
    vector<int> sizes_from_agg = _resample_function->get_return_sizes();

    // OUTPUT TYPE IS : order-on agg-function-return-types

    // ORDER-ON field
    fields.push_back(TupleDescription::Field("orderedon", (_left_field_att->get_return_types())[0], _left_order_att_size));


    // AGGREGATE RETURN TYPES
    for (unsigned int n = 0; n < _resample_function->get_return_types().size(); ++n)
    {
        fields.push_back(TupleDescription::Field("out"+to_string(n),
                                                 types_from_agg[n],
                                                 sizes_from_agg[n]));
    }

    // *phew*, now set output stream description
    TupleDescription t(fields.begin(), fields.end());
    set_output_description(0, t);

}

void ResampleQBox::init_impl() throw (AuroraException)
{

    _left_buffer = new BufferList(_left_order_att_type);
    _right_buffer = new BufferList(_right_order_att_type);
    _state_hash = new hash_for_new_state(_left_order_att_type);

    // SET THESE NULLS (else bad for realloc)
    _left_order_att_str = NULL;
    _left_order_att_value = NULL;
    _right_order_att_value = NULL;

    // for order-by TUPLENUM
    _left_tuple_counter_state = new unsigned int;
    *_left_tuple_counter_state = 0;
    _right_tuple_counter_state = new unsigned int;
    *_right_tuple_counter_state = 0;

    _left_tuple_counter = new char[sizeof(int)];
    memcpy(_left_tuple_counter,_left_tuple_counter_state,sizeof(unsigned int));
    _right_tuple_counter = new char[sizeof(int)];
    memcpy(_right_tuple_counter,_right_tuple_counter_state,sizeof(unsigned int));

    // An output tuple buffer
    _output_tuple_size = get_output_description(0).get_size(); // for the output
    _output_tuple = new char[_output_tuple_size];

    _left_tuple_size = get_input_description(0).get_size(); // from the left input
    _right_tuple_size = get_input_description(1).get_size(); // from the right input
}


void ResampleQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    unsigned int left_max_tuples = inv.get_max_tuples_to_dequeue(0);
    unsigned int right_max_tuples = inv.get_max_tuples_to_dequeue(1);

    DeqIterator left_my_deq_iterator = deq(0);
    DeqIterator right_my_deq_iterator = deq(1);

    _my_enq = enq(0);

    bool outputted = false;

    while (left_max_tuples-- && left_my_deq_iterator.avail())
    {
        const void *left_data = left_my_deq_iterator.tuple();
        ++left_my_deq_iterator;

        _left_curr_tuple = (char*) malloc(_left_tuple_size);
        memcpy(_left_curr_tuple, left_data, _left_tuple_size);

        INFO << "ResampleQBox IN LEFT: " << get_input_description(0).tuple_to_string(left_data);

        // da true first thing, if ordering by tuple number, increment the tuple number...
        if (_LEFT_ORDER_BY_TUPLENUM)
        {
            ++(*_left_tuple_counter_state);
            memcpy(_left_tuple_counter,_left_tuple_counter_state,sizeof(int));
        }

        gettimeofday(&_curr_ts, NULL);
        _curr_seconds = _curr_ts.tv_sec;

        // if ordering by tuple number, make the FieldExt object go to the "fake" tuple that's just the counter!
        _left_order_att_value = (_LEFT_ORDER_BY_TUPLENUM) ? _left_field_att->evaluate(_left_tuple_counter) : _left_field_att->evaluate(_left_curr_tuple);
        _left_buffer->add_to_vector(_left_order_att_value);

        if (_left_buffer->size() == 0)
        {
            _left_buffer->set_threshold(_left_order_att_value, _left_order_att_size);
        }

        int left_value = *(int*) _left_order_att_value;
        int left_threshold = *(int*) _left_buffer->get_threshold();
        int left_curr_slack = _left_buffer->get_slack();

        if (left_value >= left_threshold)
        {
            int32 chars_needed =  0;
            _left_order_att_str = _left_field_att->evaluate_as_char(_left_curr_tuple, chars_needed, true);
            new_state *new_state = new new_state(_left_order_att_value,
                                               _left_order_att_type,
                                               _left_order_att_str,
                                               vector<aggregate_function*>(1, _resample_function->make_new()),
                                               //_resample_function->makeNew(),
                                               _curr_ts);
            free(_left_order_att_str);
            _state_hash->add_state(new_state);
            new_state->call_init();   //getAF()->init();
            new_state->set_to(get_ts(_left_curr_tuple));

            if (left_value > left_threshold)
            {
                _left_buffer->increase_slack();
                left_curr_slack = _left_buffer->get_slack();

                while (left_curr_slack > _left_slack)
                {
                    _left_buffer->increase_threshold();
                    left_threshold = *(int*) _left_buffer->get_threshold();
                    left_curr_slack = _left_buffer->count_and_clear_vector();
                    _left_buffer->set_slack(left_curr_slack);
                }
            } // close if left_value > threshold

            int right_value = 0;
            list<BufferList::node>::iterator right_iter = _right_buffer->begin();

            BufferList::node temp_node;
            while (right_iter != _right_buffer->end())
            {
                temp_node = *right_iter;
                right_value = *(int*) temp_node.att;

                if (right_value < (left_threshold - _window_size))
                {
                    right_iter = (_right_buffer->_buffer).erase(right_iter);
                }
                else if (((left_value - _window_size) <= right_value) &&
                         (right_value <= (left_value + _window_size)))
                {
                    //NewState *temp_state = _state_hash->getState(_left_order_att_str);
                    Timestamp tuple_ts = get_ts(_left_curr_tuple);
                    Timestamp state_to = new_state->get_to();

                    new_state->call_increment(_left_curr_tuple); //getAF()->incr(_left_curr_tuple);

                    if (tuple_ts < state_to)
                    {
                        new_state->set_to(tuple_ts);
                    }
                    ++right_iter;
                }
                else
                {
                    ++right_iter;
                }
            } // close while

            // This node will store the pointer to the current tuple
            BufferList::node new_node = {_left_order_att_value, _left_curr_tuple};
            _left_buffer->push_back(new_node);

        }

    } // close left-for-loop


    /*************
                    ^^^^^^^^^^^^^^^ LEFT PROCESSING ----------- RIGHT PROCESSING vvvvvvvvvvvvvvvvvvvv
    **************/


    while (right_max_tuples-- && right_my_deq_iterator.avail())
    {
        const void *right_data = right_my_deq_iterator.tuple();
        ++right_my_deq_iterator;

        _right_curr_tuple = (char*) malloc(_right_tuple_size);
        memcpy(_right_curr_tuple, right_data, _right_tuple_size);

        INFO << "ResampleQBox IN RIGHT: " << get_input_description(1).tuple_to_string(right_data);

        // da true first thing, if ordering by tuple number, increment the tuple number...
        if (_RIGHT_ORDER_BY_TUPLENUM)
        {
            ++(*_right_tuple_counter_state);
            memcpy(_right_tuple_counter,_right_tuple_counter_state,sizeof(int));
        }

        gettimeofday(&_curr_ts, NULL);
        _curr_seconds = _curr_ts.tv_sec;

        // if ordering by tuple number, make the FieldExt object go to the "fake" tuple that's just the counter!
        _right_order_att_value = (_RIGHT_ORDER_BY_TUPLENUM) ? _right_field_att->evaluate(_right_tuple_counter) : _right_field_att->evaluate(_right_curr_tuple);
        _right_buffer->add_to_vector(_right_order_att_value);

        if (_right_buffer->size() == 0)
        {
            _right_buffer->set_threshold(_right_order_att_value, _right_order_att_size);
        }

        int right_value = *(int*) _right_order_att_value;
        int right_threshold = *(int*) _right_buffer->get_threshold();
        int right_curr_slack = _right_buffer->get_slack();

        if (right_value >= right_threshold)
        {
            if (right_value > right_threshold)
            {
                _right_buffer->increase_slack();
                right_curr_slack = _right_buffer->get_slack();

                while (right_curr_slack > _right_slack)
                {
                    _right_buffer->increase_threshold();
                    right_threshold = *(int*) _right_buffer->get_threshold();
                    right_curr_slack = _right_buffer->count_and_clear_vector();
                    _right_buffer->set_slack(right_curr_slack);
                }
            } // close if right_value > threshold

            int left_value = 0;
            list<BufferList::node>::iterator left_iter = _left_buffer->begin();

            BufferList::node temp_node;
            char left_value_str[64]; // left_order_att_value as string buffer. Why 64? see FieldExt.C for details

            while (left_iter != _left_buffer->end())
            {
                temp_node = *left_iter;
                left_value = *(int*) temp_node.att;

                // Convert the node's attribute value (tuple bytes) to c-string
                sprintf(left_value_str, "%d", left_value); // better not overrun ! (could use snprintf, but it really shouldn't happen)

                if (left_value < (right_threshold - _window_size))
                {
                    new_state *temp_state = _state_hash->get_state(left_value_str);
                    if (temp_state != NULL)
                    {
                        // EMIT!
                        emit_tuple(temp_state);
                        outputted = true;
                        _state_hash->remove_state(temp_state);
                    }
                    left_iter = (_left_buffer->_buffer).erase(left_iter);
                }
                else if (((right_value - _window_size) <= left_value) &&
                         (left_value <= (right_value + _window_size)))
                {
                    new_state *temp_state = _state_hash->get_state(left_value_str);
                    Timestamp tuple_ts = get_ts(_right_curr_tuple);
                    Timestamp state_to;
                    if (temp_state != NULL)
                    {
                        temp_state->call_increment(_right_curr_tuple); //getAF()->incr(_right_curr_tuple);
                        state_to = temp_state->get_to();
                        if (tuple_ts < state_to)
                        {
                            temp_state->set_to(tuple_ts);
                        }
                    }
                    ++left_iter;
                }
                else
                {
                    ++left_iter;
                }

            } // close while

            BufferList::node new_node = {_right_order_att_value, _right_curr_tuple};
            _right_buffer->push_back(new_node);


        }

    } // close right-for-loop

    // Done, notify everyone (note this later should be done at some other moment than just at the end
    if (outputted) get_output(0).notify_enq();

}

void ResampleQBox::emit_tuple(new_state *s)
{
    char *temp = _output_tuple;

    //copy the timestamp
    Timestamp t = s->get_to();
    memcpy(_output_tuple, &t, get_ts_size());
    _output_tuple += get_ts_size();

    // copy sid
    int sid = 10;
    memcpy(_output_tuple, &sid, get_sid_size());
    _output_tuple += get_sid_size();

    // order-by val from state
    if (!_LEFT_ORDER_BY_TUPLENUM)
    {
        memcpy(_output_tuple, s->getValue(), s->get_value_size());
        _output_tuple += s->get_value_size();
    }

    // copy final from aggregate
    char* temp_af =  s->get_af(0)->final();
    memcpy(_output_tuple, temp_af, s->get_af(0)->get_returned_size());
    delete[] temp_af;
    _output_tuple += s->get_af(0)->get_returned_size();

    // ACTUALLY emit (enqueue it to the output)
    memcpy(_my_enq.tuple(),temp,_output_tuple_size);
    ++_my_enq;
    //   REPOSITION _output_tuple to the start of the buffer (was held by temp)
    _output_tuple = temp;

    DEBUG << "ResampleQBox OUT: " << get_output_description(0).tuple_to_string(temp);

}

AURORA_DEFINE_QBOX(ResampleQBox, "resample");

BOREALIS_NAMESPACE_END;

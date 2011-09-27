
#include "WindowDropQBox.h"
#include <fstream>

BOREALIS_NAMESPACE_BEGIN

using NMSTL::to_string;

void WindowDropQBox::setup_impl() throw (AuroraException)
{
    if (get_num_inputs() != 1)
    {
        Throw(aurora_typing_exception,
              "WindowDrop requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    setup_parameters();

    // set the output stream's type, same as input
    set_out_description(0, get_in_stream(0));

    srand(time(NULL));
}

void WindowDropQBox::setup_parameters()
{
    // Basic parameters.
    typed_param("drop_rate", _window_drop_rate, PARAM_NON_EMPTY);
    typed_param("max_batch_size", _max_batch_size, PARAM_NON_EMPTY);
    typed_param("window-size", _window_size, PARAM_NON_EMPTY);
    typed_param("window-slide", _window_slide, PARAM_NON_EMPTY);
    NOTICE << get_name() << ": drop_rate = " << _window_drop_rate;

    if (_window_slide > _window_size)
    {
        Throw(aurora_typing_exception,
            "WindowDrop doesn't support window slide > window size");
    }

    _window_extent = _window_size - _window_slide;
    _across_union = false; // for now, assume there is no union downstream

    string window_size_by = param("window-size-by", PARAM_NON_EMPTY);
    if (window_size_by == "VALUES")
    {
        _window_method = WINDOW_BY_VALUES;
    }
    else if (window_size_by == "TUPLES")
    {
        _window_method = WINDOW_BY_TUPLES;
    }
    else
    {
        Throw(aurora_typing_exception,
              "WindowDrop only supports window-size-by VALUES or TUPLES");
    }

    string order_by = param("order-by", PARAM_NON_EMPTY);
    if (order_by == "FIELD")
    {
        _order_by_method = ORDER_BY_FIELD;
    }
    else if (order_by == "TUPLENUM")
    {
        _order_by_method = ORDER_BY_TUPLENUM;
    }
    else
    {
        Throw(aurora_typing_exception, 
              "Unsupported order-by type: " + order_by);
    }

    ExprContext ctxt;
    //ctxt.setTupleDescription(TupleDescription(getInDescription(0)));
    ctxt.set_context_schema( *get_in_schema( 0 ));

    // Also, we don't actually reorder tuples,
    // for now we call this _window_field.
    if (_window_method == WINDOW_BY_VALUES ||
        _order_by_method == ORDER_BY_FIELD)
    {
        _window_field = Expression::parse(param("order-on-field",
                                                PARAM_NON_EMPTY), ctxt);
        _window_field_name = param("order-on-field", PARAM_NON_EMPTY);
        DEBUG << "Window field: " << _window_field;
        DEBUG << "Window field name: " << _window_field_name;
        // For now we only support windowing on an int.
        if (!_window_field->is<int32>())
        {
            Throw(aurora_typing_exception, "Order on field must be an Int.");
        }
    }

    // group-by is optional.
    parse_group_by(param("group-by"), ctxt);
}

void WindowDropQBox::update_params()
{
    typed_param("drop_rate", _window_drop_rate, PARAM_NON_EMPTY);
    typed_param("max_batch_size", _max_batch_size, PARAM_NON_EMPTY);
    typed_param("window-size", _window_size, PARAM_NON_EMPTY);
    typed_param("window-slide", _window_slide, PARAM_NON_EMPTY);
    NOTICE << get_name() << ": drop_rate = " << _window_drop_rate;

    srand(time(NULL));
}

void WindowDropQBox::parse_group_by(const string atts, const ExprContext& ctxt)
{
    DEBUG << "Got group by of " << atts;
    vector<string> parts;
    split(atts, ',', parts);

    _group_by_value_size = 0;
    vector<string>::const_iterator p;
    for (p = parts.begin(); p != parts.end(); p++)
    {
        DEBUG << "Parsing field " << *p;
        ptr<Expression> f = Expression::parse(*p, ctxt);
        _group_by_fields.push_back(f);

        if (f->getLength() < 0)
        {
            Throw(AuroraException, "group-by fields must be fixed-length");
        }
        _group_by_value_size += f->getLength();
    }
}

void WindowDropQBox::init_impl() throw (AuroraException)
{
    //_output_tuple_size = getOutDescription(0)->getSize();
    _output_tuple_size = get_out_schema(0)->get_size() + HEADER_SIZE;
}

void WindowDropQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    DeqIterator di = deq(0);
    EnqIterator ei = enq(0);
    bool win_drop, tuple_drop;
    bool outputted = false;

    //TupleDescription in_desc(getInDescription(0));
    //TupleDescription out_desc(getOutDescription(0));
    TupleDescription in_desc(get_in_schema(0));
    TupleDescription out_desc(get_out_schema(0));

    //int counter = 0;
    //while (inv.continue_dequeue_on(di, 0))
    while (di.avail())
    {
        /*
        ++counter;

        if (counter > 100)
        {
            WARN << "It is unfair to other boxes, WinDrop will give a break";
            break;
        }
        */

        tuple_drop = true;

        const void *data = di.tuple();
        char *tuple = (char *)data;
        Tuple input_tuple = Tuple::factory(data);

        DEBUG << "WindowDropQBox IN: "  << in_desc.tuple_to_string(data);

        _ctxt.reset();
        _ctxt.set_tuple(tuple);

        string group_by_val = group_by_for_tuple(_ctxt);
        DEBUG << "Group by value: " << to_escaped_string(group_by_val);

        WindowInfo &ws = _window_info[group_by_val];

        // not handling the count-based windowing case yet..
        //
        if (_window_method == WINDOW_BY_TUPLES)
        {
            Throw(AuroraException,
                   "WindowDrop DOES NOT HANDLE COUNT-BASED WINDOWS YET!!");
            //if (w_list.empty() ||
            //    w_list.back().dynamic_cast_to<TupleWindow>()->size()
            //                              >= _advance_size)
            //{
            //    int32  start_value = 0;

            //    if (_order_by_method == ORDER_BY_FIELD)
            //{
            //        start_value = _window_field->eval<int32>(_ctxt);
            //    }

            //    ptr<Window> w(new TupleWindow(_afs, group_by_val,
            //                                  _window_size, start_value));

            //    insert_new_window(w_list, w);
            //}
        }
        else if (_window_method == WINDOW_BY_VALUES)
        {
            int32 value = _window_field->eval<int32>(_ctxt);

            if (!ws.initialized)
            {

                ws.initialized = true;
                ws.next_win_start = value;
            }
            // skipping the multi-level checks...

            if (value == ws.next_win_start)
            {
                DEBUG << " -> Window start case";
                if (ws.extra_needed)
                {
                    ws.extra_needed = false;
                    assert(ws.batch_drop == true);
                }
                else if (ws.drop_extra)
                {
                    if ((ws.extra_counter == 0) ||
                        (ws.drop_counter == _max_batch_size))
                    {
                        ws.drop_extra = false;
                        ws.drop_counter = 0;
                        assert(ws.batch_drop == false);
                    }
                    else
                    {
                        // stay in drop_extra mode
                        assert(ws.batch_drop == false);
                    }
                }
                else if ((ws.batch_counter == 0) ||
                           (ws.batch_counter >= _max_batch_size))
                {
                    // decide what to do with this new batch
                    //
                    //srand(time(NULL));
                    static unsigned int seed = 1;
                    double rand_val;
                    rand_val = (double)rand_r(&seed)/(double)(RAND_MAX);
                    if (rand_val <= _window_drop_rate)
                    {
                        // drop it
                        DEBUG << " -> Drop Batch";
                        if (ws.batch_drop) // previous batch was dropped, too
                        {
                            ws.extra_needed = true;
                            ++ws.extra_counter;
                        }
                        else
                        {
                           ws.batch_drop = true;
                        }
                    }
                    else
                    {
                        // keep it
                        DEBUG << " -> Keep Batch";
                        if (!ws.batch_drop) // previous batch was kept, too
                        {
                            ws.drop_extra = true;
                        }
                        else
                        {
                            ws.batch_drop = false;
                        }
                    }
                    ws.batch_counter = 0; // reset the counter
                }

                // decide what to do with this new window
                //
                if (!ws.extra_needed && ws.batch_drop)
                {
                    if (ws.force_open ||
                        ((ws.batch_counter == (_max_batch_size-1)) &&
                         ws.positive_batch))
                    {
                        win_drop = false;
                        ws.force_open = false;
                    }
                    else
                    {
                        win_drop = true;
                    }
                }
                else
                {
                    win_drop = false;
                    if (ws.drop_extra && !ws.extra_needed &&
                        (ws.extra_counter > 0))
                    {
                        // compensate
                        win_drop = true;
                        --ws.extra_counter;
                        ++ws.drop_counter;
                    }
                }
                if (!ws.extra_needed && !ws.drop_extra)
                {
                    ++ws.batch_counter;
                }
                if (!win_drop) // we will allow starting a window here
                {
                    DEBUG << " -> A window can start at " << ws.next_win_start;

                    // mark when it should close
                    //
                    input_tuple.set_win_spec(value + _window_size);

                    // mark how much we should keep for completeness of the win
                    //
                    ws.keep_until = value + _window_size;

                    // update next_win_start
                    //
                    ws.next_win_start += _window_slide;

                    // now you can release this tuple
                    //
                    memcpy(ei.tuple(), data, _output_tuple_size);
                    outputted = true;
                    DEBUG << "WindowDropQBox OUT: "
                          << out_desc.tuple_to_string(data);
                    //Time before_enq = Time::now();
                    ++ei;
                    //Time after_enq = Time::now();
                    /*
                    long long enq_diff = (after_enq - before_enq).to_usecs();
                    if (_stats_file)
                    {
                        (*_stats_file) << enq_diff << endl;
                    }
                    DEBUG << "enq for " << get_name() << " took (usecs) : "
                          << enq_diff;
                    */
                    ++di;
                    continue;
                }
                else // we will not allow starting a window here
                {
                    DEBUG << " -> A window can NOT start at "
                          << ws.next_win_start;

                    // mark as no start
                    //
                    input_tuple.set_win_spec(0);

                    tuple_drop = false;

                    // update next_win_start
                    //
                    ws.next_win_start = ws.next_win_start + _window_slide;

                    // now, what do we do with this tuple? release or drop it?
                    //
                    if (tuple_drop && !_across_union)
                    {
                        DEBUG << " -> drop tuple";
                    }
                    else
                    {
                        memcpy(ei.tuple(), data, _output_tuple_size);
                        outputted = true;
                        DEBUG << "WindowDropQBox OUT: "
                              << out_desc.tuple_to_string(data);
                        //Time before_enq = Time::now();
                        ++ei;
                        //Time after_enq = Time::now();
                        /*
                        long long enq_diff = 
                                    (after_enq - before_enq).to_usecs();
                        if (_stats_file)
                        {
                            (*_stats_file) << enq_diff << endl;
                        }
                        DEBUG << "enq for " << get_name() << " took (usecs) : "
                                << enq_diff;
                        */
                    }
                    ++di;
                    continue;
                }
            }
            else if (value < ws.next_win_start)
            {
                DEBUG << " -> No attraction case";

                // keep_until stays the same, but check if it allows tuple drop
                //
                if (value < ws.keep_until)
                {
                    tuple_drop = false;
                }

                // now, what do we do with this tuple? release or drop it?
                //
                if (tuple_drop && !_across_union)
                {
                    DEBUG << " -> drop tuple";
                }
                else
                {
                    memcpy(ei.tuple(), data, _output_tuple_size);
                    outputted = true;
                    DEBUG << "WindowDropQBox OUT: "
                          << out_desc.tuple_to_string(data);
                    //Time before_enq = Time::now();
                       ++ei;
                    //Time after_enq = Time::now();
                    /*
                    long long enq_diff = (after_enq - before_enq).to_usecs();
                    if (_stats_file)
                    {
                        (*_stats_file) << enq_diff << endl;
                    }
                    DEBUG << "enq for " << get_name() << " took (usecs) : "
                            << enq_diff;
                    */
                }
                ++di;
                continue;
            }
            else if (value > ws.next_win_start)
            {
                DEBUG << " -> Fake window case";

                // skip gaps
                //
                while (value >= (ws.next_win_start + _window_slide))
                {
                    ws.next_win_start += _window_slide;
                    ++ws.batch_counter;
                }

                assert(!ws.zero_batch); // a zero_batch will never get here

                // when batch_counter reaches the max, make a decision but
                // first check if an extra was kept. if so, next batch is a drop
                //
                if (ws.extra_needed)
                {
                    ws.extra_needed = false;
                    assert(ws.batch_drop);
                }
                else if (ws.drop_extra)
                {
                    if ((ws.extra_counter == 0) ||
                        (ws.drop_counter == _max_batch_size))
                    {
                        ws.drop_extra = false;
                        ws.drop_counter = 0;
                        assert(ws.batch_drop);
                    }
                    else
                    {
                        // stay in drop_extra mode
                        assert(!ws.batch_drop);
                    }
                }
                else if ((ws.batch_counter == 0) ||
                           (ws.batch_counter >= _max_batch_size))
                {
                    assert(!ws.positive_batch); // this never occurs in pos_bat
                    // decide what to do with this new batch
                    //
                    //srand(time(NULL));
                    static unsigned int seed = 1;
                    double rand_val;
                    rand_val = (double)rand_r(&seed)/(double)(RAND_MAX);
                    if (rand_val <= _window_drop_rate)
                    {
                        // drop it
                        DEBUG << " -> Drop Batch";
                        if (ws.batch_drop) // previous batch was dropped, too
                        {
                            ws.extra_needed = true;
                            ++ws.extra_counter;
                        }
                        else
                        {
                            ws.batch_drop = true;
                        }
                    }
                    else
                    {
                        // keep it
                        DEBUG << " -> Keep Batch";
                        if (!ws.batch_drop) // previous batch was kept, too
                        {
                            ws.drop_extra = true;
                        }
                        else
                        {
                            ws.batch_drop = false;
                        }
                    }
                    ws.batch_counter = 0; // reset the counter
                }
                // decide what to do with this new window
                //
                if (!ws.extra_needed && ws.batch_drop)
                {
                    if (ws.force_open ||
                        ((ws.batch_counter == (_max_batch_size-1)) &&
                        ws.positive_batch))
                    {
                        win_drop = false;
                        ws.force_open = false;
                    }
                    else
                    {
                        win_drop = true;
                    }
                }
                else
                {
                    win_drop = false;
                    if (ws.drop_extra && !ws.extra_needed &&
                        (ws.extra_counter > 0))
                    {
                        // compensate
                        win_drop = true;
                        --ws.extra_counter;
                        ++ws.drop_counter;
                    }
                }
                if (!ws.extra_needed && !ws.drop_extra)
                {
                    ++ws.batch_counter;
                }
                // if next_win_start is in fact the current tuple
                //
                if (value == ws.next_win_start)
                {
                    if (!win_drop) // we will allow starting a window here
                    {
                        DEBUG << " -> A window can start at "
                              << ws.next_win_start;

                        // mark when it should close
                        //
                        input_tuple.set_win_spec(value + _window_size);

                        // mark how much we should keep for a complete window
                        //
                        ws.keep_until = value + _window_size;

                        // update next_win_start
                        //
                        ws.next_win_start += _window_slide;

                        // now you can release this tuple
                        //
                        memcpy(ei.tuple(), data, _output_tuple_size);
                        outputted = true;
                        DEBUG << "WindowDropQBox OUT: "
                              << out_desc.tuple_to_string(data);
                        //Time before_enq = Time::now();
                           ++ei;
                        //Time after_enq = Time::now();
                        /*
                        long long enq_diff = 
                                (after_enq - before_enq).to_usecs();
                        if (_stats_file)
                        {
                            (*_stats_file) << enq_diff << endl;
                        }
                        DEBUG << "enq for " << get_name() << " took (usecs) : "
                                  << enq_diff;
                        */
                        ++di;
                        //get_output(0).notify_enq();
                        continue;
                    }
                    else // we will not allow starting a window here
                    {
                        DEBUG << " -> A window can NOT start at "
                              << ws.next_win_start;

                        // mark as no start
                        //
                        input_tuple.set_win_spec(0);

                        tuple_drop = false;

                        // update next_win_start
                        //
                        ws.next_win_start = ws.next_win_start + _window_slide;

                        // what do we do with this tuple? release or drop it?
                        //
                        if (tuple_drop && !_across_union)
                        {
                            DEBUG << " -> drop tuple";
                        }
                        else
                        {
                            memcpy(ei.tuple(), data, _output_tuple_size);
                            outputted = true;
                            DEBUG << "WindowDropQBox OUT: "
                                  << out_desc.tuple_to_string(data);
                            //Time before_enq = Time::now();
                               ++ei;
                            //Time after_enq = Time::now();
                            /*
                            long long enq_diff = 
                                        (after_enq - before_enq).to_usecs();
                            if (_stats_file)
                            {
                                (*_stats_file) << enq_diff << endl;
                            }
                            DEBUG << "enq for " << get_name() 
                                  << " took (usecs) : " << enq_diff;
                            */
                        }
                        ++di;
                        continue;
                    }
                }
                else // next_win_start should be a fake tuple
                {
                    assert(value > ws.next_win_start);

                    if (!win_drop)
                    {
                        DEBUG << " -> A window can start at "
                              << ws.next_win_start;

                        // mark when it should close
                        //
                        input_tuple.set_win_spec(ws.next_win_start
                                               + _window_size);
                        // mark it as a fake tuple
                        //
                        input_tuple.set_fake_flag(true);
                        // set its window_field to ws.next_win_start
                        //

                        int offset = out_desc.get_offset(
                                out_desc.index_of_field(_window_field_name));
                        DEBUG << " -> offset of " << _window_field_name
                              << " = " << offset;
                         memcpy(tuple + offset, &(ws.next_win_start),
                                sizeof(ws.next_win_start));

                        // mark how much we should keep for a complete window
                        //
                        ws.keep_until = ws.next_win_start + _window_size;

                        // update next_win_start
                        //
                        ws.next_win_start += _window_slide;

                        // now you can release this tuple
                        //
                        memcpy(ei.tuple(), data, _output_tuple_size);
                        outputted = true;
                        DEBUG << "WindowDropQBox OUT: "
                              << out_desc.tuple_to_string(data);
                        //Time before_enq = Time::now();
                           ++ei;
                        //Time after_enq = Time::now();
                        /*
                        long long enq_diff = 
                                        (after_enq - before_enq).to_usecs();
                        if (_stats_file)
                        {
                            (*_stats_file) << enq_diff << endl;
                        }
                        DEBUG << "enq for " << get_name() << " took (usecs) : "
                                  << enq_diff;
                        */
                        ++di;
                        //get_output(0).notify_enq();
                        continue;
                    }
                    else // we will not allow starting a window here
                    {
                        DEBUG << " -> A window can NOT start at "
                              << ws.next_win_start;

                        // mark as no start
                        //
                        input_tuple.set_win_spec(0);

                        tuple_drop = false;

                        // what do we do with this tuple? release or drop it?
                        //
                        if (tuple_drop && !_across_union)
                        {
                            DEBUG << " -> drop it";
                        }
                        else
                        {
                            // mark it as a fake tuple
                            //
                            input_tuple.set_fake_flag(true);
                            // set its window_field to ws.next_win_start
                            //
                            int offset =  out_desc.get_offset(
                                   out_desc.index_of_field(_window_field_name));
                            DEBUG << " -> offset of " << _window_field_name
                                  << " = " << offset;
                            memcpy(tuple + offset, &(ws.next_win_start),
                                   sizeof(ws.next_win_start));
                            // now you can release this tuple
                            //
                            memcpy(ei.tuple(), data, _output_tuple_size);
                            outputted = true;
                            DEBUG << "WindowDropQBox OUT: "
                                  << out_desc.tuple_to_string(data);
                            //Time before_enq = Time::now();
                               ++ei;
                            //Time after_enq = Time::now();
                            /*
                            long long enq_diff = 
                                        (after_enq - before_enq).to_usecs();
                            if (_stats_file)
                            {
                                (*_stats_file) << enq_diff << endl;
                            }
                            DEBUG << "enq for " << get_name() 
                                  << " took (usecs) : " << enq_diff; 
                            */
                        }

                        // update next_win_start
                        //
                        ws.next_win_start += _window_slide;
                        ++di;
                        continue;
                    }
                }
            }
        }
        else
        {
            FATAL << "Invalid window method.";
        }
        ++di;
    }

    // Notify when we emit something
    //
    if (outputted)
    {
        get_output(0).notify_enq();
    }
}

string WindowDropQBox::group_by_for_tuple(const EvalContext& ctxt) const
{
    char buffer[_group_by_value_size];
    char *pos = buffer;
    vector<ptr<Expression> >::const_iterator gbi;
    for (gbi = _group_by_fields.begin(); gbi != _group_by_fields.end(); ++gbi)
    {
        (*gbi)->eval_into(pos, ctxt);
        pos += (*gbi)->getLength();
    }
    return string(buffer, _group_by_value_size);
}


BOREALIS_NAMESPACE_END

#include <AggregateQBox.h>
#include <cmath>

BOREALIS_NAMESPACE_BEGIN

#define COST_OF_CAPTURING_NEW_WINDOW 18.64*0.001*0.001
#define COST_OF_CAPTURING_UPDATED_WINDOW 7.30*0.001*0.001
#define COST_OF_CREATING_NEW_WINDOW 30.23*0.001*0.001
#define COST_OF_UPDATING_WINDOW 7.25*0.001*0.001

NMSTL_SERIAL_DEFINE(AggregateQBox::Window, 1000);
NMSTL_SERIAL_DEFINE(AggregateQBox::ValueWindow, 1001);
NMSTL_SERIAL_DEFINE(AggregateQBox::TupleWindow, 1002);
NMSTL_SERIAL_DEFINE(AggregateBoxState, 1003);

using NMSTL::to_string;

void AggregateQBox::setup_impl() throw (AuroraException)
{
    _is_updated = false;
    _count_new_windows = 0;
    _count_updated_windows = 0;
    
    if (get_num_inputs() != 1)
    {
        Throw(aurora_typing_exception,
              "Aggregate requires exactly one input stream (not " +
              to_string(get_num_inputs()) + ")");
    }

    //_in_tuple_size = getInDescription(0)->getSize();
    _in_tuple_size = get_in_schema(0)->get_size() + HEADER_SIZE;

    setup_parameters();

    TupleDescription out_td = create_output_description();
    DEBUG << get_name() << " output description: " << out_td;
    set_out_description(0, out_td);
}


void AggregateQBox::setup_parameters()
{
    // Basic parameters.
    typed_param("window-size", _window_size, PARAM_NON_EMPTY);
    typed_param("advance", _advance_size, PARAM_NON_EMPTY);

    if (_advance_size > _window_size)
    {
        Throw(aurora_typing_exception,
              "Aggregate doesn't support advance size > window size");
    }

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
              "Aggregate only supports window-size-by VALUES or TUPLES");
    }

    string order_by = param("order-by", PARAM_NON_EMPTY);
    _order_by_string = order_by;
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
        Throw(aurora_typing_exception, "Unsupported order-by type: " 
                                       + order_by);
    }

    ExprContext ctxt;
    //ctxt.setTupleDescription(TupleDescription(getInDescription(0)));
    ctxt.set_context_schema( *get_in_schema( 0 ));

    // drop_mode is optional
    _drop_mode = false;
    typed_param("drop-mode", _drop_mode, PARAM_NOT_REQUIRED);

    // last_agg is optional
    _last_agg = true;
    typed_param("last-agg", _last_agg, PARAM_NOT_REQUIRED);

    // Magda added: Initially output is not tentative
    _tentative = false;

    // Magda added: Adding option to drop outputs that result 
    // from processing an empty window
    _drop_empty_outputs = false;
    typed_param("drop-empty-outputs", _drop_empty_outputs, PARAM_NOT_REQUIRED);

    // Magda added: Adding option to make window alignment 
    // independent of the first tuple received
    // i.e., making the aggregate convergent capable
    _independent_alignment = false;
    typed_param("independent-window-alignment", _independent_alignment, 
                PARAM_NOT_REQUIRED);

    _slack = 0;
    typed_param("slack", _slack, PARAM_NOT_REQUIRED);

    // Also, we don't actually reorder tuples,
    // for now we call this _window_field.
    if (_window_method == WINDOW_BY_VALUES ||
        _order_by_method == ORDER_BY_FIELD)
    {
        _order_by_string = param("order-on-field",PARAM_NON_EMPTY);
        _window_field = Expression::parse(_order_by_string, ctxt);
        DEBUG << "Window field: " << _window_field;

        // We only support windowing on an int for tuple-based, and
        // any numeric type for value-based -- yna.
        if (_window_method == WINDOW_BY_TUPLES && !_window_field->is<int32>())
        {
            Throw(aurora_typing_exception,
                "Order on field for tuple-based windows must be an integer.");
        }
        else if ( !_window_field->is_numeric() )
        {
            Throw(aurora_typing_exception,
                "Order on field must be a numeric type.");
        }
    }

    // Timeout
    double timeout;
    _timing_out = typed_param("timeout", timeout, PARAM_NOT_REQUIRED);

    if (_timing_out && (!_passive))
    {
        if (timeout < 0)
        {
            // Negative timeout means not timing out.
            // TODO: What does a timeout of 0 mean? Maybe its like
            // window_size by tuples = 1.
            _timing_out = false;
        }
        else
        {
            // Convert to timeval.
            int32 sec = static_cast<int32>(floor(timeout));
            int32 usec = static_cast<int32>(
                         floor((timeout - static_cast<double>(sec)) * 1000000));

            _timeout = Timestamp(sec, usec);
            DEBUG << "Got timeout of " << _timeout
                  << " from parameter \"" << param("timeout", "") << "\"";
        }
    }

    // Try parsing aggregates.
    for (unsigned int i = 0; ; ++i)
    {
        string expr = param("aggregate-function." + to_string(i));
        if (expr.empty())
        {
            break;
        }
        _afs.push_back(Aggregate::parse(expr, ctxt));
    }
    DEBUG << "Aggregate functions: " << _afs;
    if (_afs.empty())
    {
        Throw(aurora_typing_exception,
              "Aggregate box must have at least 1 aggregate function defined");
    }

    // group-by is optional.
    parse_group_by(param("group-by"), ctxt);
}


#define CAST_PTR_WITH_DATATYPE_TEMPLATE(templatized_ptr, arg_var, result_var, body) \
    if (arg_var->getType() == DataType::INT) {              \
    ptr< templatized_ptr<int32> > result_var = arg_var.dynamic_cast_to< templatized_ptr<int32> >(); \
    body;                               \
    }                                   \
    else if (arg_var->getType() == DataType::LONG) {            \
    ptr< templatized_ptr<int64> > result_var = arg_var.dynamic_cast_to< templatized_ptr<int64> >(); \
    body;                               \
    }                                   \
    else if (arg_var->getType() == DataType::SHORT) {            \
    ptr< templatized_ptr<int16> > result_var = arg_var.dynamic_cast_to< templatized_ptr<int16> >(); \
    body;                               \
    }                                   \
    else if (arg_var->getType() == DataType::BYTE) {            \
    ptr< templatized_ptr<int8> > result_var = arg_var.dynamic_cast_to< templatized_ptr<int8> >(); \
    body;                               \
    }                                   \
    else if (arg_var->getType() == DataType::SINGLE) {          \
    ptr< templatized_ptr<single> > result_var = arg_var.dynamic_cast_to< templatized_ptr<single> >(); \
    body;                               \
    }                                   \
    else if (arg_var->getType() == DataType::DOUBLE) {          \
    ptr< templatized_ptr<double> > result_var = arg_var.dynamic_cast_to< templatized_ptr<double> >(); \
    body;                               \
    }                                   \
    else if (arg_var->getType() == DataType::STRING) {          \
    ptr< TypedExpression<EString> > result_var = arg_var.dynamic_cast_to< TypedExpression<EString> >(); \
    body;                               \
    }                                   \
    else if (arg_var->getType() == DataType::TIMESTAMP) {       \
    ptr< templatized_ptr<Timestamp> > result_var = arg_var.dynamic_cast_to< templatized_ptr<Timestamp> >(); \
    body;                               \
    }                                   \
    else {                              \
    Throw(AuroraException, "Unknown field expression type" + arg_var->as_string()); \
    }                                   \


TupleDescription AggregateQBox::create_output_description()
{
    // Output type: (group-by-attributes)? order-on agg-function-return-types
    vector<SchemaField> fields;

    //int temp_offset = HEADER_SIZE;

    // Group by fields.
    // TODO: It would be nice to use their names here.
    vector<ptr<Expression> >::const_iterator g;
    int n;
    for (g = _group_by_fields.begin(), n = 0; g != _group_by_fields.end();
         g++, n++)
    {
        CAST_PTR_WITH_DATATYPE_TEMPLATE
        (FieldExpression, (*g), g_expr,
          fields.push_back(g_expr->get_schema_field( _group_by_strings[n] ));
         )
    }

    if (_order_by_method == ORDER_BY_FIELD)
    {
      fields.push_back(_window_field->get_schema_field( _order_by_string ));
    }

    // Aggregate function return types.
    AFVector::const_iterator f;
    int fn;
    for (f = _afs.begin(), fn = 0; f != _afs.end(); ++f, ++fn)
    {
        // If unspecified, basename is out[<function-number>]
        string basename = param("aggregate-function-output-name."
                                + to_string(fn), "agg_" + to_string(fn) + "");

        if ((*f)->numOutputFields() == 1)
        {
            SchemaField f_field = (*f)->get_output_field(0);
            SchemaField my_field(basename, f_field.get_type(),
                                 f_field.get_size());
            fields.push_back(my_field);
        }
        else
        {
            for (int i = 0; i < (*f)->numOutputFields(); ++i)
            {
                // Name is <basename>.<outputnumber>
                string name(basename + "_" + to_string(i));
                SchemaField f_field = (*f)->get_output_field(i);
                SchemaField my_field(name, f_field.get_type(),
                                     f_field.get_size());
                fields.push_back(my_field);
            }
        }
    }

    DEBUG << "Output tuple description is " 
          << TupleDescription(fields.begin(), fields.end());

    return TupleDescription(fields.begin(), fields.end());
}


void AggregateQBox::parse_group_by(const string atts, const ExprContext& ctxt)
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
        _group_by_strings.push_back(*p);

        if (f->getLength() < 0)
        {
            Throw(AuroraException, "group-by fields must be fixed-length");
        }
        _group_by_value_size += f->getLength();
    }
}


void AggregateQBox::init_impl() throw (AuroraException)
{
    DEBUG << "Initializing nothing.";
}


// Insert a window to the given window list, which should be a
// group-by window list. If we are timing out, also add the window to
// the end of the timeout list, and record the bookkeeping
// information.
void AggregateQBox::insert_new_window(WindowList &wl, ptr<Window> w)
{
    // TODO: This assumes that tuples come in timestamp order. Should
    // actually be doing a sorted insert here.
    if (_timing_out)
    {
        WindowList::iterator i = wl.insert(wl.end(), w);
        w->set_open_windows_i(i);
        i = _timeout_list.insert(_timeout_list.end(), w);
        w->set_timeout_i(i);
    }
    else
    {
        wl.push_back(w);
    }
}

void AggregateQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{
    _is_updated = true;
    
    DeqIterator di = deq(0);
    bool outputted = false;
    int tuple_count = 0;

    //TupleDescription in_desc(getInDescription(0));
    TupleDescription in_desc( get_in_schema( 0 ));

    while (inv.continue_dequeue_on(di, 0))
    {
        ++tuple_count;

        char *tuple = (char *)di.tuple();

        Tuple input_tuple = Tuple::factory(tuple);
        int win_spec = input_tuple.get_win_spec();

        // Because tuple_stime on output tuples is the maximum value of all tuples in a window
        // and because we disallow timeouts with fault-tolerance, we can simply propagate BOUNDARY
        // tuples as they arrive:
        // If tuple is a BOUNDARY, re-copy the header, set all data fields to zero, and forward
        // Same for RECONCILIATION_DONE. These tuples should simply be forwarded
        if ( (input_tuple.get_tuple_type() == TupleType::BOUNDARY) ||
             (input_tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE))
        { 
            EnqIterator ei = enq(0);
            Tuple output_tuple = Tuple::factory(ei.tuple());

            // Zero the whole output tuple
            output_tuple.clear_tuple(HEADER_SIZE + 
                                     get_out_schema(0)->get_size());
            // Copy only the header
            output_tuple.copy_header(input_tuple);
            // Propagate the tuple on the output stream
            outputted = true;
            ++ei;
            // And move on to the next input tuple
            ++di;
            continue;
        }
 
        // shall we switch into the drop_mode?
        //
        if ((!_drop_mode) && (win_spec != -1))
        {
            DEBUG << "AggregateQBox is switching into drop_mode";
            _drop_mode = true;
        }

        INFO << "AggregateQBox IN: " << in_desc.tuple_to_string(tuple);

        _ctxt.reset();
        _ctxt.set_tuple(tuple);

        if (_timing_out)
        {
            Timestamp *t = (Timestamp*)tuple;
            Timestamp threshold(*t);
            threshold -= _timeout;
            while (!_timeout_list.empty() &&
                   threshold > _timeout_list.front()->t0())
            {
                ptr<Window> w = _timeout_list.front();
                DEBUG << "Timing out " << w;
                // Added by Magda: do not emit anything when window is empty 
                // (I did not test for timeouts only for normal processing)
                if ( (!w->is_empty())  || !_drop_empty_outputs )
                {
                    emit_for_window(w);
                    outputted = true;
                }
                _open_windows[w->group_by_value()].erase(
                                                    w->get_open_windows_i());
                _dirty_windows[w->group_by_value()] = 1;
                _timeout_list.erase(w->get_timeout_i());
            }
        }

        string group_by_val = group_by_for_tuple(_ctxt);
        //DEBUG << "Group by value: " << to_escaped_string(group_by_val);

        /// Jeong-Hyon: for HA
        WindowList &w_list = _open_windows[group_by_val];
        bool was_dirty = (_dirty_windows.find(group_by_val) != _dirty_windows.end());
        int before_new = 0;
        int before_updated = 0;
        if (was_dirty)
        {
            for (WindowList::iterator in_iter = w_list.begin(); in_iter != w_list.end(); ++in_iter)
            {
                bool is_checkpointed = (*in_iter)->_is_checkpointed;
                if (is_checkpointed)
                    before_updated++;
                else  // brand_new window
                    before_new++;
            }
        }
        _dirty_windows[group_by_val] = 1;
        /// Jeong-Hyon: for HA

        //DEBUG << "Open windows for group by: " << w_list;
        // Use references so we don't have to keep hashing the group by value.
        WindowStart &ws = _window_starts[group_by_val];
        //_dirty_windows[group_by_val] = 1;

        // Window by tuples opens the window before adding tuples.
        if (_window_method == WINDOW_BY_TUPLES)
        {
            if (w_list.empty() ||
                w_list.back().dynamic_cast_to<TupleWindow>()->size()
                                                    >= _advance_size)
            {
                int32  start_value = 0;

                if (_order_by_method == ORDER_BY_FIELD)
                {
                    start_value = _window_field->eval<int32>(_ctxt);
                }

                int32 tuple_window_size =
                    static_cast<int32>(floor(_window_size));

                ptr<Window> w(
                    new TupleWindow(_afs, group_by_val, tuple_window_size,
                        start_value, -1, false));

                insert_new_window(w_list, w);

            }
        } // TODO: _drop_mode to be implemented for WINDOW_BY_TUPLES
        else if (_window_method == WINDOW_BY_VALUES)
        {

            double value = _window_field->eval<double>(_ctxt);
            //DEBUG << "New tuple for window by values with value " << value;
            
            if (!ws.initialized)
            {
                // Added by Magda to get, if desired, 
                // a window alignment independent of the first tuple
                if ( _independent_alignment ) 
                {
                    DEBUG << "Value is " << value;
                    double adjustment = fmod(value, _advance_size);
                    DEBUG << "Adjustement is " << adjustment;
                    value = value - adjustment;
                    DEBUG << "Updated value is " << value;
                }
                // End added by Magda

                DEBUG << "Initializing window setting next to " << value;
                ws.initialized = true;
                ws.next = value;
                ws.keep_until = 0;
            }

            /*
            // Magda commented out: breaks everything!
            // new - skip gaps..
            //
            while (value > ws.next)
            {
                DEBUG << "Skipping gap... value is " << value 
                      << " and next is " << ws.next;
                ws.next += _advance_size;
            }
            */

            if (_drop_mode)
            {
                // But, I need to skip gaps.. Let me try having it only in
                // drop_mode. -- tatbul
                //
                while (value > ws.next)
                {
                    DEBUG << "Skipping gap... value is " << value
                          << " and next is " << ws.next;
                    ws.next += _advance_size;
                }

                DEBUG << "in drop-mode";
                if (value == ws.next)
                {
                    DEBUG << "Window Start Position";
                    if (win_spec > 0)
                    {
                        DEBUG << "+ win_spec";
                        DEBUG << "Open window";
                        double window_end = ws.next + _window_size;
                        int new_win_spec =
                            win_spec - static_cast<int>(_window_size - 1);
                        ptr<Window> w(new ValueWindow(_afs, group_by_val,
                                      _window_field,
                                      ws.next, window_end,
                                      _slack,
                                      new_win_spec,
                                      false));
                        ws.next += _advance_size;
                        ws.keep_until =
                            static_cast<double>(win_spec) - (_window_size - 1);
                        DEBUG << "Creating " << w;
                        insert_new_window(w_list, w);
                    }
                    else if (win_spec <= 0)
                    {
                        DEBUG << "not + win_spec";
                        if (value >= ws.keep_until)
                        {
                            if ((win_spec == 0) && (!_last_agg))
                            {
                                DEBUG << "Have to keep this tuple as fake";
                                ptr<Window> w(new ValueWindow(_afs,
                                              group_by_val,
                                              _window_field,
                                              value, value+_window_size,
                                              _slack,
                                              0, true));
                                ws.next += _advance_size;
                                DEBUG << "Creating " << w;
                                insert_new_window(w_list, w);
                            }
                            else
                            {
                                DEBUG << "Skip window (beyond)";
                                ws.next += _advance_size;

                                if ((win_spec == -1) && (_last_agg))
                                {
                                    DEBUG << "AggregateQBox is coming out of "
                                          << "the drop_mode";
                                    _drop_mode = false;
                                }
                                else if ((win_spec == -1) && (!_last_agg))
                                {
                                    WARN << "now what????";
                                    // now what??
                                }
                            }
                        }
                        else
                        {
                            DEBUG << "Open window (within)";
                            double window_end = ws.next + _window_size;
                            ptr<Window> w(new ValueWindow(_afs, group_by_val,
                                                          _window_field,
                                                          ws.next, window_end,
                                                          _slack,
                                                          win_spec, false));
                            ws.next += _advance_size;
                            if (ws.keep_until < (value + _window_size))
                            {
                                ws.keep_until = value + _window_size;
                            }
                            DEBUG << "Creating " << w;
                            insert_new_window(w_list, w);
                        }
                    }
                }
                else
                {
                    DEBUG << "Not Window Start Position";
                    if (win_spec > 0)
                    {
                        DEBUG << "+ win_spec";
                        DEBUG << "Open fake window";
                        int new_win_spec =
                            win_spec - static_cast<int>(_window_size-1);
                        ptr<Window> w(new ValueWindow(_afs, group_by_val,
                                                      _window_field,
                                                      value, value+_window_size,
                                                      _slack,
                                                      new_win_spec,
                                                      true));
                        ws.keep_until = win_spec - (_window_size - 1);
                        DEBUG << "Creating " << w;
                        insert_new_window(w_list, w);
                    }
                    else if (win_spec == 0)
                    {
                        DEBUG << "0 win_spec";
                        DEBUG << "Open fake window";
                        ptr<Window> w(new ValueWindow(_afs, group_by_val,
                                                      _window_field,
                                                      value, value+_window_size,
                                                      _slack,
                                                      0, true));
                        DEBUG << "Creating " << w;
                        insert_new_window(w_list, w);
                    }
                    else
                    {
                        DEBUG << "Ignore...";
                    }
                }
            }
            else
            {
                // This is normal operation (no dropping)
                while (value >= ws.next)
                {
                    //DEBUG << "Regular Window Start value is " << value 
                    //      << " next is " << ws.next;
                    double window_end = ws.next + _window_size;
                    ptr<Window> w(new ValueWindow(_afs, group_by_val,
                                                  _window_field,
                                                  ws.next, window_end,
                                                  _slack,
                                                  win_spec,
                                                  false));
                    ws.next += _advance_size;
                    //DEBUG << "Creating " << w;
                    insert_new_window(w_list, w);
                }
            }
        }
        else
        {
            FATAL << "Invalid window method.";
        }

        // --magda added: when receiving the first TENTATIVE tuple, 
        // change state of box to produce TENTATIVE output
        if (Tuple::factory(_ctxt.getTuple()).get_tuple_type() 
                                                    == TupleType::TENTATIVE)
        { 
            _tentative = true;
        }

        // Try to put the tuple in each window.
        WindowList::iterator wi = w_list.begin();

        while (wi != w_list.end())
        {
            //DEBUG << *wi;
            //DEBUG << "*wi";
            
            bool emitp = (*wi)->insert_tuple(_ctxt);
            
            //DEBUG << emitp;
          
            if (emitp)
            {
                // Window is closing.
                DEBUG << *wi << " is closing.";

                // Magda added: if a window is empty, DO NOT output anything!  
                if ( ( !(*wi)->is_empty() )  || !_drop_empty_outputs)
                {
                    emit_for_window(*wi);
                    outputted = true;
                }

                if (_timing_out)
                {
                    _timeout_list.erase((*wi)->get_timeout_i());
                }
                wi = w_list.erase(wi);
            }
            else
            {
                ++wi;
            }
        }
        /// Jeong-Hyon: for HA
        for (WindowList::iterator in_iter = w_list.begin(); in_iter != w_list.end(); ++in_iter)
        {
            bool is_checkpointed = (*in_iter)->_is_checkpointed;
            if (is_checkpointed)
                _count_updated_windows++;
            else  // brand_new window
                _count_new_windows++;
        }
        _count_updated_windows -= before_updated;
        _count_new_windows -= before_new;
        /// Jeong-Hyon: for HA
        ++di;
    }

    //NOTICE << "aggregate processed " << tuple_count << " inputs";

    // TODO: Make sure we actually emitted something.
    if (outputted)
    {
        get_output(0).notify_enq();
    }
}

/// Emit based on the tuples from the window.
void AggregateQBox::emit_for_window(const ptr<Window> w)
{

    DEBUG << "emitForWindow for " << w;

    EnqIterator ei = enq(0);
    Tuple output = Tuple::factory(ei.tuple());

    char * const data = (char *)ei.tuple();
    size_t offset = 0;

    DEBUG << " - into " << (const void *)data;

    // *** Revision Processor may want to change the assignments below!
    //
    // First create the Tuple Header.
    // Copy timestamp from window.
    //*(Timestamp *)(data + offset) = w->t0();
    //*(Timestamp *)(data + offset) = w->t_last(); // -- tatbul
    //offset += sizeof(Timestamp);
    // Copy the local timestamp from window.
    //*(Timestamp *)(data + offset) = w->lt0();
    //*(Timestamp *)(data + offset) = w->lt_last(); // -- tatbul
    //offset += sizeof(Timestamp);
    // Assign the unique tuple-id.
    //*(int *)(data + offset) = w->tid(); // id of the starter tuple for now.
    //offset += sizeof(int);
    // Assign the tuple-type.
    //*(TupleType *)(data + offset) = TupleType::INSERTION;
    //offset += sizeof(TupleType);
    // Assign the revision-id.
    //*(int *)(data + offset) = -1; // -1 for now
    //offset += sizeof(int);
    // Assign the quality.
    //*(double *)(data + offset) = double(1.0); // 1.0 for now
    //offset += sizeof(double);
    // Assign the win-spec.
    //*(int *)(data + offset) = w->get_win_spec();
    //offset += sizeof(int);
    // Assign the fake-flag.
    //*(bool *)(data + offset) = w->get_fake_flag();
    //offset += sizeof(bool);

    // Magda modified: Use the set* functions rather than computing offsets as above
    output.set_timestamp(w->t_last());
    output.set_local_timestamp(w->lt_last());
    output.set_tuple_id(w->tid());
    output.set_tuple_type(TupleType::INSERTION);
    output.set_revision_id(-1);
    output.set_quality(double(1.0));
    output.set_win_spec(w->get_win_spec());
    output.set_fake_flag(w->get_fake_flag());
    // Magda added: Assign the tuple_stime as the max tuple_stime from
    // all tuples in window by doing so, the output boundary is the
    // minimum of the two input boundaries This assumes that timeouts
    // are not allowed.
    output.set_tuple_stime(w->max_stime());

    // Magda added: check if output should be labeled as tentative
    if ( _tentative )
    { output.set_tuple_type(TupleType::TENTATIVE);
    } 

    offset = HEADER_SIZE;

    // Copy group_by.
    string gbv = w->group_by_value();
    memcpy(data + offset, gbv.c_str(), gbv.length());

    offset += gbv.length();

    if (_order_by_method == ORDER_BY_FIELD)
    {
        // Copy state value.
        if ( _window_field->is<int32>() )
        {
            int32 start = static_cast<int32>(w->get_start_value());
            memcpy(data + offset, &start, _window_field->getLength());
            offset += _window_field->getLength();
        }
        else
        {
            double start = w->get_start_value();
            memcpy(data + offset, &start, _window_field->getLength());
            offset += _window_field->getLength();
        }
    }

    // Copy final from aggregate.
    if (!w->get_fake_flag())
    {
        offset += w->final_value(data + offset);
    }

    //TupleDescription out_desc(getOutDescription(0));
    TupleDescription  out_desc( get_out_schema( 0 ));
    DEBUG << "AggregateQBox OUT: " << out_desc.tuple_to_string(ei.tuple());
    ++ei;
}

string AggregateQBox::group_by_for_tuple(const EvalContext& ctxt) const
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

string AggregateQBox::Window::as_string() const
{

    typedef vector<ptr<AggregateWindow> > Windows;

    string s = "Window[_windows = {";

    bool first = true;

    for (Windows::const_iterator i = _windows.begin();
         i != _windows.end(); i++)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s += ", ";
        }
        s += (**i);
    }
    s += "}, _group_by = ";
    s += _group_by;
    s += ", _initialized = ";
    s += _initialized;
    s += ", _t0 = ";
    s += to_string(_t0);
    s += ", _lt0 = ";
    s += _lt0;
    s += ", _t_last = ";
    s += to_string(_t_last);
    s += ", _lt_last = ";
    s += _lt_last;
    s += ", _tid = ";
    s += _tid;
    s += ", _max_stime = ";
    s += _max_stime;
    s += "]";
    return s;
}

void AggregateQBox::Window::set_aggregates(const AFVector &af)
{
    size_t n = _windows.size();
    if (n < af.size())
        n = af.size();
    for (size_t i = 0; i < n; i++)
        _windows[i]->set_aggregate(af[i]);
}

size_t AggregateQBox::Window::final_value(char *pos) const
{
    size_t s = 0;
    vector<ptr<AggregateWindow> >::const_iterator w;
    for (w = _windows.begin(); w != _windows.end(); ++w)
    {
        const ptr<Aggregate> agg = (*w)->get_aggregate();
        DEBUG << "Calling final() on window " << typeid(**w) << " into "
              << (const void*)pos;
        (*w)->final(pos + s);

        DEBUG << "final() said: \""
              << to_escaped_string(pos + s, agg->get_output_size()) << "\"";

        s += agg->get_output_size();
    }
    return s;
}

// Return true if this window should emit now.
bool AggregateQBox::Window::insert_tuple(const EvalContext& ctxt)
{
    if (!_initialized)
    {
        // Set up the header fields from the first tuple.
        _t0 = *((Timestamp *)ctxt.getTuple());
        _lt0 = *((Timestamp *)ctxt.getTuple() + 1);
        // -- tatbul
        _t_last = *((Timestamp *)ctxt.getTuple());
        _lt_last = *((Timestamp *)ctxt.getTuple() + 1);
        _tid = *((int *)ctxt.getTuple() + 4);
        _initialized = true;
    }
    
    // Magda added: once we insert at leaast one tuple, the window is no longer empty
    _is_empty = false;

    bool fake_tuple = Tuple::factory(ctxt.getTuple()).get_fake_flag();
    if (fake_tuple || _fake_flag)
    {
        return false;
    }

    // -- magda added: update the max_stime value if necessary
    Timestamp tuple_stime = Tuple::factory(ctxt.getTuple()).get_tuple_stime();
    _max_stime = (tuple_stime > _max_stime) ? tuple_stime : _max_stime;

    // -- tatbul
    Timestamp current_t = *((Timestamp *)ctxt.getTuple());
    Timestamp current_lt = *((Timestamp *)ctxt.getTuple() + 1);
    if (current_t > _t_last)
    {
        _t_last = current_t;
    }
    if (current_lt > _lt_last)
    {
        _lt_last = current_lt;
    }

    vector<ptr<AggregateWindow> >::const_iterator w;

    for (w = _windows.begin(); w != _windows.end(); ++w)
    {
        (*w)->incr(ctxt);
    }
    // Window doesn't know when to emit. Subclasses will handle that.
    return false;
}

void AggregateQBox::Window::update(OSerial& o)
{
    o << _t_last << _lt_last << _max_stime;
//    o << _t0 _lt0 << _tid;
    for (register size_t i = 0; i < _windows.size(); i++)
        _windows[i]->update(o);
}

void AggregateQBox::Window::update(ISerial& i)
{
    i >> _t_last >> _lt_last >> _max_stime;
//    i >> _t0 >> _lt0 >> _tid;
    for (register size_t j = 0; j < _windows.size(); j++)
        _windows[j]->update(i);
}

string AggregateQBox::ValueWindow::as_string() const
{
    string s = "ValueWindow[";
    s += AggregateQBox::Window::as_string();
    s += "], ";
    s += "first value = ";
    s += to_string(_first_val);
    s += ", last value = ";
    s += to_string(_last_val);
    s += "]";
    return s;
}

bool AggregateQBox::ValueWindow::insert_tuple(const EvalContext& ctxt)
{
    double window_val = _window_field->eval<double>(ctxt);

    if ( window_val < _first_val )
    {
//        DEBUG << "Window val " << window_val
//              << " is before first_val " << _first_val
//              << ". Not adding to window but returning false.";
        return false;
    }
    if (window_val >= _last_val)
    {
        if (_slack_remaining > 0)
        {
//            DEBUG << "Window val " << window_val
//                  << " is after last_val " << _last_val
//                  << " but we still have slack(" << _slack_remaining
//                  << "). Not adding to window but returning false.";
            _slack_remaining--;
            return false;
        }
        else
        {
//            DEBUG << "Window val " << window_val
//                  << " is after last_val " << _last_val
//                  << " and we are out of slack, returning true.";

            // start: added by ying
            // We set the timestamp of a window to be the timestamp of the tuple
            // that emit the window
            
            Timestamp tuple_stime = 
                Tuple::factory(ctxt.getTuple()).get_tuple_stime();
            _max_stime = (tuple_stime > _max_stime) ? tuple_stime : _max_stime;

            Timestamp current_t = *((Timestamp *)ctxt.getTuple());
            Timestamp current_lt = *((Timestamp *)ctxt.getTuple() + 1);
            if (current_t > _t_last)
            {
                _t_last = current_t;
            }
            if (current_lt > _lt_last)
            {
                _lt_last = current_lt;
            }
            
            // end: added by ying

            return true;
        }
    }
    //DEBUG << "Adding tuple with window_val " << window_val
    //      << " to " << *this;
    AggregateQBox::Window::insert_tuple(ctxt);
    // Don't emit.
    return false;
}

void AggregateQBox::ValueWindow::update(OSerial& o)
{
    AggregateQBox::Window::update(o);
//    o << _first_val << _last_val << 
    o << _slack_remaining;
}

void AggregateQBox::ValueWindow::update(ISerial& i)
{
    AggregateQBox::Window::update(i);
//    i >> _first_val >> _last_val;
    i >>_slack_remaining;
}

string AggregateQBox::TupleWindow::as_string() const
{
    string s = "TupleWindow[";
    s += AggregateQBox::Window::as_string();
    s += "], ";
    s += "count = ";
    s += to_string(_count);
    s += ", window size = ";
    s += to_string(_window_size);
    s += "]";
    return s;
}

bool AggregateQBox::TupleWindow::insert_tuple(const EvalContext& ctxt)
{
    _count++;
    AggregateQBox::Window::insert_tuple(ctxt);
    if (_count >= _window_size)
    {
        // Emit.
        return true;
    }
    else
    {
        DEBUG << "Not closing " << *this
              << ", count(" << _count
              << ") is less than window_size(" << _window_size
              << ")";
        return false;
    }
}

void AggregateQBox::set_pending_box_state(ptr<AbstractBoxState> packed_box)
{
    //AggregateBoxState* actual_packed_box = static_cast<AggregateBoxState*>(packed_box.get());
    //_pending_box_state.set(new AggregateBoxState(*actual_packed_box));
    _pending_box_state = packed_box.dynamic_cast_to<AggregateBoxState>();
}

ptr<AbstractBoxState> AggregateQBox::pack_state(bool delta_only)
{
    DEBUG << "Packing state for AggregateQBox named [" << get_name() << "]";

    ptr<AggregateBoxState> aggregate_box_state(new AggregateBoxState(*this, delta_only));

    DEBUG << "Copied WindowStarts map size = " << _window_starts.size();

    DEBUG << "Copied TimeoutList size = " << _timeout_list.size();

    DEBUG << "Copied WindowHash map size = " << _open_windows.size();
    return aggregate_box_state;
}

/*
void AggregateQBox::set_window_counts()
{
    if (_is_updated)
    {
        _count_windows_new = 0;
        _count_windows_updated = 0;
        
        for (DirtyWindowHash::iterator iter = _dirty_windows.begin();iter != _dirty_windows.end();++iter)
        {
            WindowList& temp_list = _open_windows[iter->first];
            for (WindowList::iterator in_iter = temp_list.begin(); in_iter != temp_list.end(); ++in_iter)
            {
                bool is_checkpointed = (*in_iter)->_is_checkpointed;
                if (is_checkpointed)
                    _count_windows_updated++;
                else  // brand_new window
                    _count_windows_new++;
            }
        }
        
        NOTICE << "::" << _count_windows_updated << ", " << _count_windows_new;
        NOTICE << "::" << _count_updated_windows << ", " << _count_new_windows;
    }
}
*/

Time AggregateQBox::get_expected_capture_cost()
{
//    set_window_counts();
    double value = _count_new_windows*COST_OF_CAPTURING_NEW_WINDOW + _count_updated_windows*COST_OF_CAPTURING_UPDATED_WINDOW;
    return Time::secs(value);
}

Time AggregateQBox::get_expected_paste_cost()
{
//    set_window_counts();
    double value = _count_new_windows*COST_OF_CREATING_NEW_WINDOW + _count_updated_windows*COST_OF_UPDATING_WINDOW; 
    return Time::secs(value);
}

AggregateBoxState::AggregateBoxState(AggregateQBox& aggregate, 
                                     bool delta_only)
{

//    NOTICE << "capture: " << aggregate._count_new_windows << " " << aggregate._count_updated_windows;

    /*
    NMSTL OSerial can easily serialize vector, map, set
    serializing and deserializing hash_maps and lists are difficult.
        - need to write to_string methods to serialize
        - DON'T KNOW WHAT METHODS TO WRITE TO DESERIALIZE
    So the solution I (Anurag) came up with
        - convert hash_map to map to serialize
    - convert list to vector
    - vice-versa to deserialize
    IF SOMEONE KNOWS A BETTER WAY - LEMME KNOW
    */
    
    Time start_time = Time::now();

    // convert hash_map window_starts to map
    OSerialString serial_box(OSerial::binary);
//    OSerialString serial_box;
    // serial_box.reserve(1000000);
    serial_box << delta_only;
//    serial_box << window_starts_map;
    
//    map<string, AggregateQBox::WindowStart> window_starts_map;
    int n;
    if (delta_only)
    {
        n = aggregate._dirty_windows.size();
        serial_box << n;
        for (DirtyWindowHash::iterator iter = aggregate._dirty_windows.begin();iter != aggregate._dirty_windows.end();++iter)
        {
            serial_box << iter->first;
            AggregateQBox::WindowStart* window_start = &(aggregate._window_starts[iter->first]);
            bool is_checkpointed = window_start->is_checkpointed;
            serial_box << is_checkpointed;
            if (is_checkpointed)
                window_start->update(serial_box);
            else
                serial_box << *window_start;
            window_start->is_checkpointed = true;
        }
    }
    else
    {
        n = aggregate._window_starts.size();
        serial_box << n;
        for (WindowStarts::iterator iter = aggregate._window_starts.begin();iter != aggregate._window_starts.end();++iter)
            serial_box << iter->first << iter->second;
    }
    
//    NOTICE << "Original WindowStarts hash_map size = "
//          << window_starts.size() << " || "
          //<< "Copied WindowStarts map size = " << window_starts_map.size();

    // TODO: fix
    // yna -- this does not correctly serialize the _timeout_list since
    // NMSTL's serialization doesn't support pointer sharing.

    // convert list timeout_list to vector
//     vector<ptr<AggregateQBox::Window> > timeout_list_vector;
//     for (WindowList::iterator iter = timeout_list.begin();
//          iter != timeout_list.end(); ++iter)
//{
//         timeout_list_vector.push_back(*iter);
//         DEBUG << *iter;
//     }

    vector< pair<string, unsigned int> > timeout_list_vector;
    WindowList::iterator tl_iter = aggregate._timeout_list.begin();
    WindowList::iterator tl_end = aggregate._timeout_list.end();
    for ( ; tl_iter != tl_end; ++tl_iter)
    {
        const string& group_by = (*tl_iter)->group_by_value();
        WindowList::iterator ow_iter = aggregate._open_windows[group_by].begin();
        unsigned int ow_size = aggregate._open_windows[group_by].size();
        unsigned int pos = 0;
        for (; pos < ow_size; ++pos)
        {
            if (*tl_iter == *ow_iter)
                break;
            ow_iter++;
        }
        if (pos != ow_size)
        {
            pair<string, unsigned int> tl_pair = make_pair(group_by, pos);
            timeout_list_vector.push_back(tl_pair);
            DEBUG << tl_pair;
        }
    }

    DEBUG << "Original timout list size = " << aggregate._timeout_list.size()
          << " || " << "Copied timout vector size = "
          << timeout_list_vector.size();
    
    serial_box << timeout_list_vector;

    // end yna
    
    if (delta_only)
    {
        n = aggregate._dirty_windows.size();
        serial_box << n;

        for (DirtyWindowHash::iterator iter = aggregate._dirty_windows.begin();iter != aggregate._dirty_windows.end();++iter)
        {
            serial_box << iter->first;
            WindowList& temp_list = aggregate._open_windows[iter->first];
            n = temp_list.size();
            serial_box << n;
            for (WindowList::iterator in_iter = temp_list.begin(); in_iter != temp_list.end(); ++in_iter)
            {
                bool is_checkpointed = (*in_iter)->_is_checkpointed;
                serial_box << is_checkpointed;
                if (is_checkpointed)
                {
                    (*in_iter)->update(serial_box);
                }
                else  // brand_new window
                {
                    serial_box << *in_iter;
                }
                (*in_iter)->_is_checkpointed = true;
            }
        }
    }
    else
    {
        // convert hash_map open_windows to map
        // map<string, vector<ptr<AggregateQBox::Window> > > open_windows_map;
        n = aggregate._open_windows.size();
        serial_box << n;
        for (WindowHash::iterator iter = aggregate._open_windows.begin(); iter != aggregate._open_windows.end(); ++iter)
        {
            serial_box << iter->first;
            WindowList& temp_list = iter->second;
            n = temp_list.size();
            serial_box << n;
            // vector<ptr<AggregateQBox::Window> > temp_vector;
            // change the list to vector
            for (WindowList::iterator in_iter = temp_list.begin(); in_iter != temp_list.end(); ++in_iter)
            {
                serial_box << *in_iter;
            // temp_vector.push_back(*in_iter);
            }
            // open_windows_map[iter->first] = temp_vector;
        }
    }
    
//    NOTICE << dirty_windows.size();
    
    // Jeong-Hyon: for HA
    aggregate._dirty_windows.clear();
    aggregate._count_updated_windows = 0;
    aggregate._count_new_windows = 0;
    
    // Jeong-Hyon: for HA
    
//    serial_box << open_windows_map;

//    NOTICE << "Original WindowHash hash_map size = " << open_windows.size()
//          << " || " << "Copied WindowHash map size = "
//          << open_windows_map.size();
          
//    OSerialStream serial_box(ostringstream(_serialized_state));
    
    serial_box << aggregate._tentative;
    _serialized_state = serial_box.str();
    
    aggregate._is_updated = false;
    
//    NOTICE << "capture: " << aggregate._count_windows_new << " " << aggregate._count_windows_updated << " " << (Time::now() - start_time).to_msecs();
    
}

void AggregateQBox::unpack_state(ptr<AbstractBoxState> box_state)
{

    Time start_time = Time::now();
    _count_new_windows = 0;
    _count_updated_windows = 0;

    // SEE AggregateQBox.h::AggregateBoxState on why this unpacking
    // looks like this
    DEBUG << "Unpacking state for AggregateQBox named [" << get_name() << "]";

    AggregateBoxState *aggregate_box_state;
    aggregate_box_state = static_cast<AggregateBoxState*>(box_state.get());

    ISerialString iss(aggregate_box_state->_serialized_state);
    if (!iss)
    {
        DEBUG << "Failed: " << iss.stat();
    }

    // these are what's in the serialized_state
//    map<string, AggregateQBox::WindowStart> window_starts_map;
    //vector<ptr<AggregateQBox::Window> > timeout_list_vector;
    vector< pair<string, unsigned int> > timeout_list_vector;
//    map<string, vector<ptr<AggregateQBox::Window> > > open_windows_map;
    bool delta_only;
    iss >> delta_only;
//    NOTICE << delta_only;
    
//    iss >> window_starts_map;

    // now copy over each of the above to AggregateQBox variables
    // convert window_starts_map to hash_map
    if (!delta_only)
        _window_starts.clear();
    int n;
    iss >> n;
//    NOTICE << n;
    string s;
    AggregateQBox::WindowStart window_start;
    for (register int i = 0; i < n; i++)
    {
        iss >> s;
        bool is_checkpointed = false;
        if (delta_only)
        {
            iss >> is_checkpointed;
//            NOTICE << is_checkpointed;
        }
        if (is_checkpointed)
        { // reuse the old window
            _window_starts[s].update(iss);    
        }
        else // brand new window
        {
            iss >> window_start;
            _window_starts[s] = window_start;
        }
        _window_starts[s].is_checkpointed = true;
    }
    
//    for (map<string, AggregateQBox::WindowStart>::iterator
//         iter = window_starts_map.begin(); iter != window_starts_map.end();
//         ++iter)
//         {
//        _window_starts.insert(make_pair(iter->first, iter->second));
//    }
//    DEBUG << "Original WindowStarts map size = " << window_starts_map.size()
//           << "||" << "Copied WindowStarts map size = "
//           << _window_starts.size();

    iss >> timeout_list_vector;
    
//    iss >> open_windows_map;
    // convert hash_map open_windows_map to hash_map
    if (!delta_only)
        _open_windows.clear();
    iss >> n; // number of dirty windows
//    NOTICE << n;
    for (register int i = 0; i < n; i++)
    {
        iss >> s; // group by value
        WindowList& old_window_list = _open_windows[s];
        int m;
        iss >> m;
        if (m == 0) 
        {
            _open_windows.erase(s);
        }
        else
        {
            WindowList new_window_list;
    //        NOTICE << m;
            WindowList::iterator iter = old_window_list.begin();
            for (register int j = 0; j < m; j++)
            {
                bool is_checkpointed = false;
                if (delta_only)
                {
                    iss >> is_checkpointed;
    //                NOTICE << is_checkpointed;
                }
                if (is_checkpointed)
                { // reuse the old window
                    ptr<AggregateQBox::Window> window = *iter;
                    window->update(iss);
                    iter++;
                    window->_is_checkpointed = true;
                    new_window_list.push_back(window);
                    _count_updated_windows++;
                    
                }
                else // brand new window
                {
                    ptr<AggregateQBox::Window> window;
                    iss >> window;
                    window->set_aggregates(_afs);
                    window->set_window_field(_window_field);
                    window->_is_checkpointed = true;
                    new_window_list.push_back(window);
                    _count_new_windows++;
                    
                }
            }
            _open_windows[s] = new_window_list;
        }
    }
//    for (map<string, vector<ptr<AggregateQBox::Window> > >::iterator
//         iter = open_windows_map.begin(); iter != open_windows_map.end();
//         ++iter)
//         {
//        vector<ptr<AggregateQBox::Window> > temp_vector = iter->second;
//        WindowList temp_list;
// change vector to list
//        for (vector<ptr<AggregateQBox::Window> >::iterator
//             in_iter = temp_vector.begin(); in_iter != temp_vector.end();
//             ++in_iter)
//             {
//                (*in_iter)->set_window_field(_window_field);
//                temp_list.push_back(*in_iter);
//        }
//        _open_windows.insert(make_pair(iter->first, temp_list));
//    }
//    DEBUG << "Original WindowHash map size = " << open_windows_map.size()
//          << "||" << "Copied WindowHash map size = "
//          << _open_windows.size();

    // yna -- deserialize timeout list after open windows, so timeout list can point to
    // their correct contents.

    // convert list timeout_list_vector to list
    _timeout_list.clear();
//     for (vector<ptr<AggregateQBox::Window> >::iterator
//          iter = timeout_list_vector.begin(); iter != timeout_list_vector.end();
//          ++iter)
//{
//         (*iter)->set_window_field(_window_field);
//         _timeout_list.push_back(*iter);
//     }

    // yna -- use group by index to accomplish _timeout_list pointer sharing.
    
/*    JH: temporarily commented out

    vector< pair<string, unsigned int> >::iterator tlv_iter = timeout_list_vector.begin();
    vector< pair<string, unsigned int> >::iterator tlv_end = timeout_list_vector.end();

    for ( ; tlv_iter != tlv_end; ++tlv_iter)
    {
        // Get the windows for our group by.
        WindowList::iterator wl_it = _open_windows[tlv_iter->first].begin();
        for (unsigned int i=0; i < tlv_iter->second; ++i) ++wl_it;
        _timeout_list.push_back(*wl_it);

        WindowList::iterator tl_it = _timeout_list.begin();
        for (unsigned int i=0; i < _timeout_list.size(); ++i) ++tl_it;
        (*wl_it)->set_timeout_i(tl_it);
    }

//    DEBUG << "Original TimeoutList size = " << timeout_list_vector.size()
//          << "||" << "Copied TimeoutList size = " << _timeout_list.size();
*/

    iss >> _tentative;
    
    _is_updated = false;
    
//    cout << "paste: " << _count_windows_new << " " << _count_windows_updated << " " << (Time::now() - start_time).to_msecs() << endl;
    
    
}

BOREALIS_NAMESPACE_END

#include <RevisionAggregateQBox.h>
#include <cmath>

BOREALIS_NAMESPACE_BEGIN

NMSTL_SERIAL_DEFINE(RevisionAggregateQBox::Window,                1100);
NMSTL_SERIAL_DEFINE(RevisionAggregateQBox::ValueWindow,           1101);
NMSTL_SERIAL_DEFINE(RevisionAggregateQBox::TupleWindow,           1102);

const int NULL_REV_ID = -1;

using NMSTL::to_string;

void RevisionAggregateQBox::setup_impl() throw (AuroraException)
{
  if (get_num_inputs() != 1)
    Throw(aurora_typing_exception,
      "Aggregate requires exactly one input stream (not " +
      to_string(get_num_inputs()) + ")");
  
  _in_tuple_size = get_in_schema(0)->get_size() + HEADER_SIZE;
  
  setup_parameters();
  
  TupleDescription out_td = create_output_description();
  DEBUG << get_name() << " output description: " << out_td;
  set_out_description(0, out_td);
  
  _out_tuple_size = get_out_schema(0)->get_size() + HEADER_SIZE;
  _last_emitted_tuple = new uint8[_out_tuple_size];
  
  set_emit_hist( true );
  
  /** MODE OF REVISION PROCESSING **/
  string mode = param("processing-mode", PARAM_NON_EMPTY);
  if (mode == "UPSTREAM") 
    {
      DEBUG << get_name() << " map box in UPSTREAM mode";
      _revision_processing_mode = UPSTREAM;
    } 
  else if (mode == "DOWNSTREAM") 
    {
      DEBUG << get_name() << " map box in DOWNSTREAM mode";
      _revision_processing_mode = DOWNSTREAM;
    }
  else 
    {
      Throw(aurora_typing_exception, "Unsupported RevisionProcessingMode: " + mode);
    }
}

void RevisionAggregateQBox::setup_parameters()
{
  // Basic parameters.
  typed_param("window-size", _window_size, PARAM_NON_EMPTY);
  typed_param("advance", _advance_size, PARAM_NON_EMPTY);
  
  if (_advance_size > _window_size)
    {  
      Throw(aurora_typing_exception,
        "Aggregate doesn't support advance size greater than window size");
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
      Throw(aurora_typing_exception, "Aggregate only supports window-size-by VALUES or TUPLES");
    }
  
  string order_by = param("order-by", PARAM_NON_EMPTY);
  _order_by_string = order_by;
  _last_order_by = static_cast<int32>(-1);
  
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
      Throw(aurora_typing_exception, "Unsupported order-by type: " + order_by);
    }
  
  ExprContext ctxt;
  ctxt.set_context_schema( *get_in_schema( 0 ));
  
  // Magda added: Initially output is not tentative
  _tentative = false;
  
  _slack = 0;
  typed_param("slack", _slack, PARAM_NOT_REQUIRED);
  
  // Also, we don't actually reorder tuples, for for now we call this _window_field.
  if (_window_method == WINDOW_BY_VALUES ||
      _order_by_method == ORDER_BY_FIELD)
    {
      _order_by_string = param("order-on-field",PARAM_NON_EMPTY);
      _window_field = Expression::parse(_order_by_string, ctxt);
      DEBUG << "Window field: " << _window_field;
      // For now we only support windowing on an int.
      if (!_window_field->is<int32>())
    Throw(aurora_typing_exception, "Order on field must be an Int.");
    }
  
  // Timeout
  double timeout;
  _timing_out = typed_param("timeout", timeout, PARAM_NOT_REQUIRED);
  
  if (_timing_out)
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
      int32 usec = static_cast<int32>(floor((timeout - static_cast<double>(sec)) * 1000000));
      
      _timeout = Timestamp(sec, usec);
      DEBUG << "Got timeout of " << _timeout
        << " from parameter \"" << param("timeout", "") << "\"";
    }
    }
  
  // Try parsing aggregates.
  for (unsigned int i = 0; ; ++i)
    {
      string expr = param("aggregate-function." + to_string(i));
      if (expr.empty()) break;
      
      _afs.push_back(Aggregate::parse(expr, ctxt));
      _afs2.push_back(Aggregate::parse(expr,ctxt));
    }
  DEBUG << "Aggregate functions: " << _afs;
  if (_afs.empty())
    Throw(aurora_typing_exception,
      "Aggregate box must have at least one aggregate function defined");
  
  // group-by is optional.
  parse_group_by(param("group-by"), ctxt);
  
  // anchors for revision proessing, optional, comma-separated
  string anchor_name = param("anchor");
  split(anchor_name, ',', _anchor);
  
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


TupleDescription RevisionAggregateQBox::create_output_description()
{
  // Ouput type: (group-by-attributes)? order-on agg-function-return-types
  vector<SchemaField> fields;
  
  // Group by fields.
  // TODO: It would be nice to use their names here.
  vector<ptr<Expression> >::const_iterator g;
  int n;
  for (g = _group_by_fields.begin(), n = 0;
       g != _group_by_fields.end();
       g++, n++)
    {
      CAST_PTR_WITH_DATATYPE_TEMPLATE
    (FieldExpression, (*g), g_expr,
     fields.push_back(g_expr->get_schema_field( _group_by_strings[n] ));
     )  
    }
  
  if (_order_by_method == ORDER_BY_FIELD)
    {
      fields.push_back(_window_field->get_schema_field( _order_by_string));
    }
  
  // Aggregate function return types.
  AFVector::const_iterator f;
  int fn;
  for (f = _afs.begin(), fn = 0;
       f != _afs.end();
       ++f, ++fn)
    {
      // If unspecified, basename is out[<function-number>]
      string basename = param("aggregate-function-output-name." + to_string(fn),
                  "agg_" + to_string(fn) + "");
      
      if ((*f)->numOutputFields() == 1)
    {
      SchemaField f_field = (*f)->get_output_field(0);
      SchemaField my_field(basename, f_field.get_type(), f_field.get_size());
      fields.push_back(my_field);
    } else
      {
        for (int i = 0; i < (*f)->numOutputFields(); ++i)
          {
        // Name is <basename>.<outputnumber>
        string name(basename + "_" + to_string(i));
        SchemaField f_field = (*f)->get_output_field(i);
        SchemaField my_field(name, f_field.get_type(), f_field.get_size());
        fields.push_back(my_field);
          }
      }
    }
  
  int max_size = 0;
  int r_offset = HEADER_SIZE;
  
  for( unsigned int i = 0; i < fields.size(); i++ )
    {
      SchemaField this_field = fields.at(i);
      max_size = (max_size > this_field.get_size()) ? max_size : this_field.get_size();
      r_offset += this_field.get_size();
    }
  
  DEBUG << "Creating revised_value: size=" << max_size << "; offset=" << r_offset;
  
  // revised_value for the revised values of the rev_field (in header)
  SchemaField revised_field;
  revised_field.set_name( "revised_value" );
  revised_field.set_type_size( DataType::STRING, max_size );
  revised_field.set_offset( r_offset );
  
  fields.push_back( revised_field );
  
  return TupleDescription(fields.begin(), fields.end());
}

void RevisionAggregateQBox::parse_group_by(const string atts, const ExprContext& ctxt)
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
    Throw(AuroraException, "group-by fields must be fixed-length");
      _group_by_value_size += f->getLength();
    }
}


void RevisionAggregateQBox::init_impl() throw (AuroraException)
{
  
  DEBUG << "RevisionAggregate::init_impl()";
  // the very first window value
  _first_window_start = -9999;
  
  // Clear _last_emitted_tuple of garbage
  memset(_last_emitted_tuple, '\0', _out_tuple_size);
  
  // Hard-coded tid-allocation stuff for now
  _block_size = 400; ////////////////////////////////////////
  _next_unallocated_tid_block = 0;
  
  _downstream_revision_number = 0;
}


// Insert a window to the given window list, which should be a
// group-by window list. If we are timing out, also add the window to
// the end of the timeout list, and record the bookkeeping
// information.
void RevisionAggregateQBox::insert_new_window(WindowList &wl, ptr<Window> w)
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

vector<int> RevisionAggregateQBox::tids_affected( const uint8 *data )
{
  vector<int> ret;
  _ctxt.reset();
  _ctxt.set_tuple(data);
  int32 order_by = _window_field->eval<int32>(_ctxt);
  string group_by = group_by_for_tuple(_ctxt);
  if( _tid_block_allocation.find(group_by) == _tid_block_allocation.end() || 
      _tid_block_allocation[group_by].empty() )
    {
      DEBUG << "Too early to have emitted anything.";
      return( ret );
    }
  
  int first_order_by = _first_order_by[group_by];
  int first_order_by_affected = order_by - _window_size + 1;
  while (first_order_by < first_order_by_affected)
    {
      first_order_by += _advance_size;
    }
  first_order_by_affected =first_order_by;
  
  int last_order_by_emitted = _last_order_by_emitted[group_by];
  int advance = 0;
  while( advance < _window_size && 
     (advance + first_order_by_affected) <= order_by && 
     (advance + first_order_by_affected) <= last_order_by_emitted )
    {
      int tuple_num = (first_order_by_affected + advance) / _advance_size;
      DEBUG << "tuple_num = " << tuple_num << ",_block_size = " << _block_size;
      int block_num = (_tid_block_allocation[group_by])[tuple_num / _block_size];
      int offset_within_block = tuple_num % _block_size;
      ret.push_back( ( block_num * _block_size) + offset_within_block + 1 );
      DEBUG << "first_order_by=" << first_order_by << ",first_order_by_affected=" << first_order_by_affected << ",advance=" << advance
        << ",block_num=" << block_num << ",offset_within_block=" << offset_within_block << ",tuple_num=" << tuple_num
        << ",tid=" << ret.back();
      advance += _advance_size;
    }   
  return(ret);
}

void RevisionAggregateQBox::run_impl(QBoxInvocation& inv) throw (AuroraException)
{  
  unsigned int max_tuples = inv.get_max_tuples_to_dequeue(0);
  DeqIterator di = deq(0);
  //    bool outputted = false;
  
  TupleDescription in_desc( get_in_schema( 0 ));
  TupleDescription out_desc( get_out_schema( 0 ));
  
  // Do stuff
  for ( ; max_tuples-- && di.avail(); ++di)
    {
      uint8 *tuple = new uint8[in_desc.get_size()];
      memcpy(tuple, (uint8 *)di.tuple(), in_desc.get_size());  // memcpy coz the tuple may need to be stored
      Tuple tuple_object = Tuple::factory(tuple);
      
      // Because tuple_stime on output tuples is the maximum value of all tuples in a window
      // and because we disallow timeouts with fault-tolerance, we can simply propagate BOUNDARY
      // tuples as they arrive:
      // If tuple is a BOUNDARY, re-copy the header, set all data fields to zero, and forward
      if (tuple_object.get_tuple_type() == TupleType::BOUNDARY)
        { 
      EnqIterator ei = enq(0);
      Tuple output_tuple = Tuple::factory(ei.tuple());
      
      // Zero the whole output tuple
      output_tuple.clear_tuple(HEADER_SIZE + get_out_schema(0)->get_size());
      // Copy only the header
      output_tuple.copy_header(tuple_object);
      // Propagate the tuple on the output stream
      get_output(0).notify_enq();      
      ++ei;
      // And move on to the next input tuple
      ++di;
      continue;
        }
      
      _ctxt.reset();
      _ctxt.set_tuple(tuple);
      TupleType tuple_type = tuple_object.get_tuple_type();
      int32 tuple_order_by = _window_field->eval<int32>(_ctxt);
      string group_by_val = group_by_for_tuple(_ctxt);
      
      if (!group_by_val.empty())
        {
      map<string, int32 >::iterator group_by = _group_by_store.find(group_by_val);
      if (group_by != _group_by_store.end())
            {
          _last_order_by = group_by->second;
            }
        }
      
      // revision if deletion, replacement, or out-of-order insertion
      bool revision =  ( ( tuple_type == TupleType::INSERTION && tuple_order_by < _last_order_by  )
             || tuple_type == TupleType::DELETION
             || tuple_type == TupleType::REPLACEMENT );
      
      bool stepping = false;
      
      INFO << "RevisionAggregateQBox " << get_name() << " IN: " << in_desc.tuple_to_string(tuple);
      
      // nothing to do with request tuples
      if (tuple_type == TupleType::REQUEST ) {
    DEBUG << "Dropping " << tuple_type << " tuple";
    continue;
      }
      
      //        if(stepping)
      //        {
      //            ////      DEBUG << (char)getchar();
      //        }
      
      // bloot -- Timeout needs something done to it; I haven't a clue now
      /*      if (_timing_out)
                {
                Timestamp *t = (Timestamp*)tuple;
                Timestamp threshold(*t);
                threshold -= _timeout;
                while (!_timeout_list.empty() &&
                threshold > _timeout_list.front()->t0())
                {
                ptr<Window> w = _timeout_list.front();
                DEBUG << "Timing out " << w;
                //emit_for_window(w);   // Make this right by the new reckoning
                outputted = true;
                _open_windows[w->group_by_value()].erase(w->get_open_windows_i());
                _timeout_list.erase(w->get_timeout_i());
                }
                }
      */
      
      // Else it already knows the _last_order_by, since there's only one
      
      DEBUG << "Group_by_val: " << to_escaped_string(group_by_val) << ", Last Order By: " << _last_order_by;
      
      // revision_or_historical
      bool revision_or_h = ( revision || 
                 tuple_type == TupleType::HISTORICAL || 
                 tuple_type == TupleType::DELIMITER || 
                 tuple_type == TupleType::NOTIFICATION );
      
      WindowList &w_list = _open_windows[group_by_val];
      DEBUG << "Open windows for group by [" << group_by_val << "]:"  << w_list;
      
      /*********************** REGULAR TUPLE PROCESSING ************************/                    

      // IF REGULAR TUPLE, SPECIAL CONSIDERATION
      if (!revision_or_h) { 
    _ctxt.reset();
    _ctxt.set_tuple(tuple);
    // Open a new window, if need be.
    WindowStart &ws = _window_starts[group_by_val];
    ptr<Window> last_win = _last_ins_window[group_by_val];
    
    // Window by tuples opens the window before adding tuples.
    if (_window_method == WINDOW_BY_TUPLES)
      {
        if (last_win == NULL ||
        last_win.dynamic_cast_to<TupleWindow>()->size() >= _advance_size)
          {
        int32  start_value = 0;
        
        if (_order_by_method == ORDER_BY_FIELD)
          start_value = _window_field->eval<int32>(_ctxt);
        if (_first_window_start == -9999)
          _first_window_start = 0; // start at index 0
        ptr<Window> w(new TupleWindow(_afs, _afs2, group_by_val,
                          _window_size, start_value, this,
                          in_desc, out_desc));
        w->set_state( WindowState::INSERTION );
        insert_new_window(w_list, w);
        _last_ins_window[group_by_val] = w;                     
          }
      }
    else if (_window_method == WINDOW_BY_VALUES)
      {
        int32 value = _window_field->eval<int32>(_ctxt);
        
        if (!ws.initialized)
          {
        ws.initialized = true;                      
        ws.next = value;
          }
        if (_first_window_start == -9999)
          _first_window_start = value;
        
        while (value >= ws.next)
          {
        // Time to start a new window.
        int window_end = ws.next + _window_size;
        ptr<Window> w(new ValueWindow(_afs, _afs2, group_by_val,
                          _window_field, ws.next,
                          window_end, _slack, this,
                          in_desc, out_desc));
        ws.next += _advance_size;
        DEBUG << "Creating " << w;
        w->set_state( WindowState::INSERTION );
        insert_new_window(w_list, w);
        _last_ins_window[group_by_val] = w;
          }
      }
    else
      {
        FATAL << "Invalid window method.";
      }
    WindowList windows_to_delete = WindowList();
    
    WindowList::const_iterator w;
    for( w = w_list.begin(); w != w_list.end(); ++w )
      {
        if( (*w)->insert_tuple(_ctxt) )
          {
        windows_to_delete.push_back( *w );
          }
      }
    // Delete the marked windows.
    for( w = windows_to_delete.begin(); w != windows_to_delete.end(); ++w )
      {
        _open_windows[(*w)->group_by_value()].remove(*w);
      }
    
    // On a normal Insertion tuple, increment the appropriate _last_order_by
    if( !group_by_val.empty() )
      _group_by_store[group_by_val] = tuple_order_by;
    else
      _last_order_by = tuple_order_by;
    
      }   // DONE REGULAR TUPLE
  
      /*********************** DONE REGULAR TUPLE PROCESSING ************************/                   


      
      // IF REVISION OR HISTORICAL GO THROUGH VECTOR
      if (revision_or_h) {  
    
    vector<Tuple> tuples_to_insert = vector<Tuple>();
    // Determine which tuples go in to the windows this run, and which
    // get delayed into _revision_tuple_store
    // Only need to deal with fixing tuple order in upstream


    /*********************** UPSTREAM PROCESSING ************************/              

    if( _revision_processing_mode == UPSTREAM )
      {
        // Delay tuples with null rev_id
        if( revision )
          {
        if( tuple_object.get_revision_id() == NULL_REV_ID )
          {
            if( tuple_type == TupleType::INSERTION )
              {
            // Give it a rev_id
            tuple_object.set_revision_id( tuple_object.get_tuple_id() );
              }
            _revision_tuple_store.push_back( tuple );
            DEBUG << "Storing tuple: " << in_desc.tuple_to_string(tuple);
            continue;   // Go on to get next tuple
          }
          }
        // Recall tuples when their order_by value is exceeded
        if( revision_or_h )
          {
        list<const uint8 *>::iterator w;
        list<const uint8 *> marked_for_deletion = list<const uint8 *>();
        
        for( w = _revision_tuple_store.begin();
             w != _revision_tuple_store.end();
             ++w )
          {
            _ctxt.reset();
            _ctxt.set_tuple(*w);
            int32 rev_order_by = _window_field->eval<int32>(_ctxt);
            string rev_group_by = group_by_for_tuple(_ctxt);
            if ( (tuple_order_by > rev_order_by) && 
             (rev_group_by == group_by_val) && 
             (tuple_object.get_tuple_type() == TupleType::HISTORICAL) &&
             (tuple_object.get_revision_id() == Tuple::factory(*w).get_tuple_id())
             )
              {
            tuples_to_insert.push_back(Tuple::factory(*w));
            marked_for_deletion.push_back(*w);
            DEBUG << "Pulling revision from store: " << in_desc.tuple_to_string(*w);
            break;
              }
          }
        for( w = marked_for_deletion.begin();
             w != marked_for_deletion.end();
             ++w                              )
          {
            _revision_tuple_store.remove(*w);
          }
        _ctxt.reset();
        _ctxt.set_tuple(tuple);
        // then the tuple that just came in
        tuples_to_insert.push_back(tuple_object);
          }
      }
    else
      {
        if (tuple_type == TupleType::DELIMITER)
          {
        DEBUG << "Skipping " << tuple_type << " in DOWNSTREAM mode";
        continue;
          }
        tuples_to_insert.push_back(tuple_object);
      }
    
    
    // insert tuples
    for( vector<Tuple>::iterator t = tuples_to_insert.begin();
         t != tuples_to_insert.end();
         ++t                           )
      {
        _ctxt.reset();
        _ctxt.set_tuple((*t).get_data());
        // Open a new window, if need be.
        if( _revision_processing_mode == UPSTREAM )
          {
        WindowStart &ws = _historical_starts[group_by_val];
        ptr<Window> last_win =  _last_rev_window[group_by_val];
        
        // Window by tuples opens the window before adding tuples.
        if (_window_method == WINDOW_BY_TUPLES)
          {
            if (last_win == NULL ||
            last_win.dynamic_cast_to<TupleWindow>()->size() >= _advance_size)
              {
            int32  start_value = 0;
            
            if (_order_by_method == ORDER_BY_FIELD)
              {
                start_value = _window_field->eval<int32>(_ctxt);
              }
            if (_first_window_start == -9999)
              {
                _first_window_start = 0; // start at index 0
              }
            ptr<Window> w(new TupleWindow(_afs, _afs2, group_by_val,
                              _window_size, start_value, this,
                              in_desc, out_desc));
            if( revision && (*t).get_tuple_type() == TupleType::INSERTION )
              {
                w->set_state( WindowState::HISTORICAL );
              }
            insert_new_window(w_list, w);
            _last_rev_window[group_by_val] = w;
              }
          }
        else if (_window_method == WINDOW_BY_VALUES)
          {
            int32 value = _window_field->eval<int32>(_ctxt);
            
            if (!ws.initialized)
              {
            ws.initialized = true;
            
            int order_by = _first_order_by[group_by_val];
            if (order_by == 0) order_by++;   // if not in the map yet, it will return 0, increase it
            while (order_by < value)
              order_by += _advance_size;
            ws.next = order_by;
              }
            if (_first_window_start == -9999)
              {
            _first_window_start = value;
              }
            
            while (value >= ws.next)
              {
            // Time to start a new window.
            int window_end = ws.next + _window_size;
            ptr<Window> w(new ValueWindow(_afs, _afs2, group_by_val,
                              _window_field, ws.next,
                              window_end, _slack, this,
                              in_desc, out_desc));
            ws.next += _advance_size;
            DEBUG << "Creating " << w;
            w->set_state( WindowState::HISTORICAL );
            insert_new_window(w_list, w);
            _last_rev_window[group_by_val] = w;
              }
          }
        else
          {
            FATAL << "Invalid window method.";
          }
        // Thank God that's all the window bookkeeping I have to do.
        
        // Send the tuple to all the windows.
        // Keep note of the ones that need
        // to be deleted.
        WindowList windows_to_delete = WindowList();
        
        WindowList::const_iterator w;
        for( w = w_list.begin(); w != w_list.end(); ++w )
          {
            _ctxt.reset();
            _ctxt.set_tuple((*t).get_data());
            if ( (*t).get_tuple_type() != TupleType::DELIMITER) 
              {
            if( (*w)->insert_tuple(_ctxt) )
              {
                windows_to_delete.push_back( *w );
              }
              }
          }
        // delete marked windows
        for(w = windows_to_delete.begin(); w != windows_to_delete.end(); ++w )
          {
            _open_windows[(*w)->group_by_value()].remove(*w);
          }
        windows_to_delete.clear();
        
        // Delimiter tuples need to be emitted by the box, tacking on a
        // delimiter header to the data of the last outputted tuple.
        // Fortunately, emit_tuple deals with the details.
        if( (*t).get_tuple_type() == TupleType::DELIMITER )
          {
            // if delimiter, go through all windows and close windows with last_value-1 <= window_val of delimiter
            for (WindowHash::iterator i = _open_windows.begin(); i != _open_windows.end(); ++i)
              {
            WindowList wl = i->second;
            for (WindowList::iterator j = wl.begin(); j != wl.end(); ++j)
              {
                ptr<Window> w = (*j);
                if ( w->get_state() != WindowState::INSERTION )  // historical windows only
                  {
                if ( (w->get_last_value()-1) <= tuple_order_by )  // delimiter says this window must close
                  {
                    INFO << "DELIMITER closing window " << i->first << " " << w;
                    vector<uint8 *> tup = w->as_tuples();
                    windows_to_delete.push_back(w);
                    for( vector<uint8*>::const_iterator tt = tup.begin(); tt != tup.end(); ++tt)
                      {
                    emit_tuple(*tt);
                      }
                  }
                else  // other windows must delete only
                  {
                    INFO << "DELIMITER deleting window " << i->first << " " << w;
                    windows_to_delete.push_back(w);
                  }
                  }
              }
              }
            _historical_starts.clear();
            // now actually delete the windows
            for(w = windows_to_delete.begin(); w != windows_to_delete.end(); ++w )
              {
            _open_windows[(*w)->group_by_value()].remove(*w);
              }
            
            emit_tuple( (*t).get_data() );
          } // done closing/deleting windows by delimiter
        
          } // DONE REVISION UPSTREAM PROCESSING

        /*********************** DONE UPSTREAM PROCESSING ************************/             



        /*********************** DOWNSTREAM PROCESSING ************************/

        // special cases for downstream-processing tuples:
        if( _revision_processing_mode == DOWNSTREAM )
          {
        DEBUG << "DOWNSTREAM processing!";
        // revision => store + requests
        if(revision)
          {
            DEBUG << "Putting revision tupel on stack";
            
            _downstream_revision_number++;
            _downstream_revision_tuple_store[_downstream_revision_number] = tuple;
            
            // Fire off the set of requests
            DEBUG << "PROCESSING REVISION";
            
            uint8 request[HEADER_SIZE];
            uint8 padded_request[_out_tuple_size];
            memset(request, '\0', HEADER_SIZE);
            memset(padded_request, '\0', HEADER_SIZE);
            Tuple::factory(request).set_revision_id( _local_boxid );
            Tuple::factory(request).set_tuple_type( TupleType::REQUEST );
            
            /// win_spec will be used as the downstream_revision_number of the revision that the request is for
            Tuple::factory(request).set_win_spec(_downstream_revision_number);            
            
            // which tids to request from downstream cp
            _outstanding_requests[_downstream_revision_number] = 0;
            vector<int> affected_tids = tids_affected(tuple);
            DEBUG << "affected_tids: " << affected_tids;
            
            for( vector<int>::iterator i = affected_tids.begin(); i != affected_tids.end(); ++i )
              {
            // Send out a request with this tid
            Tuple::factory(request).set_tuple_id( *i );
            memcpy(padded_request, request, HEADER_SIZE);
            // increment number of requests sent
            _outstanding_requests[_downstream_revision_number] += 1;
            // and send
            emit_tuple(padded_request);
            send_to_sweeper(padded_request);
            
            DEBUG << "Tuples enqueued onto request sweeper";
            INFO << "RevisionAggregateQBox " << get_name() << " OUT to SWEEPER: " << out_desc.tuple_to_string(padded_request);
            
            if (stepping) 
              {
                ////              DEBUG << getchar();
              }
              }
            if( affected_tids.size() == 0 )
              {
            DEBUG << "No outputs affected";
            _downstream_revision_tuple_store.erase(_downstream_revision_number);
              }                 
          }
        // notification => grab + recreate window + send out new revision
        else if( tuple_type == TupleType::NOTIFICATION )
          {
            INFO << "Received NOTIFICATION tuple";
            // TODO: notify all of them, remove request from all of them once found
            
            // get historical tuples from sweeper
            uint8 *historical_tuple = retrieve_historical( tuple );

            if( historical_tuple != NULL )
              {
            int request_number_historical_is_for = Tuple::factory(historical_tuple).get_win_spec();
            INFO << "HISTORICAL: " << out_desc.tuple_to_string(historical_tuple);
            INFO << "REVISION:   " << in_desc.tuple_to_string(_downstream_revision_tuple_store[request_number_historical_is_for]);
            
            // TODO: Make this deal directly with Window,Aggregate,AggregateWindow
            // Hack OR Feature - done by iadams
            // the way this is implemented makes it work for both SUM and COUNT
            Window revision_window = ValueWindow(_afs, _afs2, group_by_val, _window_field, 0, _window_size, _slack, this, in_desc, out_desc);
            revision_window.set_state( WindowState::HISTORICAL );
            _ctxt.reset();
            _ctxt.set_tuple(_downstream_revision_tuple_store[request_number_historical_is_for]);
            revision_window.insert_tuple(_ctxt);
            vector<uint8 *> revised_tuples = revision_window.as_tuples();
            for( vector<uint8 *>::iterator t = revised_tuples.begin(); t != revised_tuples.end(); ++t )
              {
                DEBUG << "Revision comes through window as: " << out_desc.tuple_to_string(*t);
                if (Tuple::factory(*t).get_revised() < 0)  
                  {
                DEBUG << "Nothing to revise from this window";
                continue;
                  }
                const SchemaField *revised_field = out_desc.get_schema_field( Tuple::factory(*t).get_revised() );
                const SchemaField *revised_value = out_desc.get_schema_field( out_desc.index_of_field( "revised_value" ));
                Tuple::factory(historical_tuple).set_tuple_type(TupleType::REPLACEMENT);
                Tuple::factory(historical_tuple).set_revised( Tuple::factory(*t).get_revised());
                // this works for SUM and COUNT
                *((int *) (historical_tuple + revised_value->get_offset())) = *((int *) (historical_tuple + revised_field->get_offset()));  // set the value
                *((int *) (historical_tuple + revised_value->get_offset())) -= *((int *) (*t + revised_field->get_offset()));  // subtract old
                *((int *) (historical_tuple + revised_value->get_offset())) += *((int *) (*t + revised_value->get_offset()));  // add new

                Tuple::factory(historical_tuple).set_revision_id(
                    Tuple::factory((uint8 *)_ctxt.getTuple()).get_tuple_id()); // revision_id of output is tuple_id of revision

                emit_tuple(historical_tuple);
                *((int *) (historical_tuple + revised_field->get_offset())) = *((int *) (historical_tuple + revised_value->get_offset()));  // incase another revision is sent, update the value revised
              }
            _outstanding_requests[request_number_historical_is_for] -= 1;  // decrement remaining
            if( _outstanding_requests[request_number_historical_is_for] == 0 )
              _downstream_revision_tuple_store.erase(request_number_historical_is_for);
            // no need to send to sweeper since ds proceseeing can't go above aggreagte
              }
            else
              {
            DEBUG << "Notification didn't find anything; ignored";
              }
          }
          }  // DONE REVISION DOWNSTREAM PROCESSING
        
        /*********************** DONE DOWNSTREAM PROCESSING ************************/       

      }  // DONE tuples_to_insert
    
      }  // done IF REVISION OR HISTORICAL
      
    } // done INPUT TUPLES
  
  // bloot -- some cleanup stuff?
  
}


string RevisionAggregateQBox::group_by_for_tuple(const EvalContext& ctxt) const
{
  char  buffer[_group_by_value_size];
  char *pos = buffer;
  vector<ptr<Expression> >::const_iterator gbi;

  for (gbi = _group_by_fields.begin(); gbi != _group_by_fields.end(); ++gbi)
    {
      (*gbi)->eval_into(pos, ctxt);
      pos += (*gbi)->getLength();
    }
  return string(buffer, _group_by_value_size);
}

// Do the actual insertion of tuple into window. Return true if Window knows
// it should delete.
bool RevisionAggregateQBox::Window::insert_tuple(/*const*/ EvalContext& ctxt)
{
  Tuple tuple = Tuple::factory(ctxt.getTuple());
  
  if( !_initialized )
    {
      // Set up header, window state information
      _t0 = tuple.get_timestamp();
      _lt0 = tuple.get_local_timestamp();
      _tid = tuple.get_tuple_id();
      _rev_id = tuple.get_revision_id();
      
      _initialized = true;
      
      // If the window never gets initialized, then all the timestamps are garbage.
      // Tuple ID and Revision ID are dealt with at the as_tuples/emit level.
      // Future fix to do that to timestamps too?
    }
  
  // -- magda added: update the max_stime value if necessary
  Timestamp tuple_stime = Tuple::factory(ctxt.getTuple()).get_tuple_stime();
  _max_stime = (tuple_stime > _max_stime) ? tuple_stime : _max_stime;
  
  
  vector< ptr<AggregateWindow> >::const_iterator w;
  
  if( _state.type() == WindowState::INSERTION )
    {
      if( tuple.get_tuple_type() == TupleType::INSERTION )
        {
      for( w = _old_windows.begin(); w != _old_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::HISTORICAL )
        {
      // Ignore.
        }
      else if( tuple.get_tuple_type() == TupleType::REPLACEMENT )
        {
      // Found a match. Do a replacement.
      // i.e. delete and insert
      for( w = _old_windows.begin();
           w != _old_windows.end();  ++w )
        {
          (*w)->remove(ctxt);
        }
      // Change the tuple in the EvalContext to
      // insert the new value
      char new_data[_in_td.get_size()];
      memcpy( new_data, tuple.get_data(), _in_td.get_size() );
      Tuple new_tuple = Tuple::factory( new_data );

      string revised_val =
        _parent->get_revised_val( new_tuple,
                      *(_in_td.get_schema()) );
      const SchemaField *revised_field = _in_td.get_schema_field(new_tuple.get_revised());

      revised_val.copy( new_data + revised_field->get_offset(),
                        revised_field->get_size() );

      ctxt.reset();
      ctxt.set_tuple(new_tuple.get_data());
      // Now insert the new tuple:
      for( w = _old_windows.begin();
           w != _old_windows.end();  ++w )
        {
          (*w)->incr(ctxt);
        }
        }
      else if( tuple.get_tuple_type() == TupleType::DELETION )
        {
      int rev_tid = tuple.get_tuple_id();
      // Remove tuple with tid = rev_tid.
      // Do this by remove(ctxt) on each w.
      for( w = _old_windows.begin();
           w != _old_windows.end();  ++w )
        {
          (*w)->remove(ctxt);
        }
      _tid_store.erase( rev_tid );
        }
      else if ( tuple.get_tuple_type() == TupleType::DELIMITER )
        {
      // Do nothing
        }
      else
        {
      WARN << "Unrecognized TupleType: " << tuple.get_tuple_type();
        }
    }
  else if( _state.type() == WindowState::HISTORICAL )
    {
      // HISTORICAL keeps both old and new because it can become replacement
      
      if( tuple.get_tuple_type() == TupleType::INSERTION )
        {
      // Out of order. WS::HISTORICAL + TT::INSERTION -> WS::REPLACEMENT
      // Add new tuple to new
      for( w = _new_windows.begin(); w != _new_windows.end(); ++w)
        {
          (*w)->incr(ctxt);
        }
      _state.set_type( WindowState::REPLACEMENT );
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::HISTORICAL )
        {
      // Insert normally.
      for( w = _old_windows.begin(); w != _old_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      for( w = _new_windows.begin(); w != _new_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::REPLACEMENT )
        {
      // Become a REPLACEMENT window
      _state.set_type( WindowState::REPLACEMENT );
      
      int rev_tid = tuple.get_tuple_id();
      if( _tid_store.find( rev_tid ) != _tid_store.end() )
            {
          // Found a match. Remove the old value.
          for( w = _new_windows.begin();
           w != _new_windows.end(); ++w )
                {
          (*w)->remove(ctxt);
                }
            }
      else
            {
          // No match; add old value to old
          for( w = _old_windows.begin();
           w != _old_windows.end(); ++w )
                {
          (*w)->incr(ctxt);
                }
            }
      // Change the tuple in the EvalContext to
      // insert the new value
      char new_data[_in_td.get_size()];
      memcpy( new_data, tuple.get_data(), _in_td.get_size() );
      Tuple new_tuple = Tuple::factory( new_data );

      string revised_val = _parent->get_revised_val( new_tuple,
                             *(_in_td.get_schema()) );

      const SchemaField *revised_field = _in_td.get_schema_field(
                                             new_tuple.get_revised());

      revised_val.copy( new_data + revised_field->get_offset(),
                revised_field->get_size() );

      ctxt.reset();
      ctxt.set_tuple(new_tuple.get_data());

      // Now insert the new tuple:
      for( w = _new_windows.begin();
           w != _new_windows.end();  ++w )
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( rev_tid );
        }
      else if( tuple.get_tuple_type() == TupleType::DELETION )
        {
      if( !_tid_store.count( tuple.get_tuple_id() ) == 1 )
        {
          _state.set_type( WindowState::REPLACEMENT );
          for( w = _old_windows.begin(); w != _old_windows.end(); ++w )
        {
          (*w)->incr(ctxt);
        }
        }
      else
            {
          _state.set_type( WindowState::REPLACEMENT );
          for( w = _new_windows.begin(); w != _new_windows.end(); ++w)
                {
          (*w)->remove(ctxt);
                }
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::DELIMITER )
        {
      return( true );
        }
      else
        {
      WARN << "Unrecognized TupleType: " << tuple.get_tuple_type();
        }
    }
  else if( _state.type() == WindowState::REPLACEMENT )
    {
      if( tuple.get_tuple_type() == TupleType::INSERTION )
        {
      // Out of order, add to new
      for( w = _new_windows.begin(); w != _new_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::HISTORICAL )
        {
      // Add to old and new
      for( w = _new_windows.begin(); w != _new_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      for( w = _old_windows.begin(); w != _old_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::REPLACEMENT )
        {
      // Add old to old; new to new
      int rev_tid = tuple.get_tuple_id();
      if( _tid_store.find( rev_tid ) != _tid_store.end() )
            {
          // Found a match. Remove the old value from new.
          for( w = _new_windows.begin();
           w != _new_windows.end(); ++w )
                {
          (*w)->remove(ctxt);
                }
            }
      else
            {
          // No match; add old value to old
          for( w = _old_windows.begin();
           w != _old_windows.end(); ++w )
                {
          (*w)->incr(ctxt);
                }
            }
      // Change the tuple in the EvalContext to
      // insert the new value
      char  new_data[_in_td.get_size()];
      memcpy( new_data, tuple.get_data(), _in_td.get_size() );
      Tuple new_tuple = Tuple::factory( new_data );

      string revised_val = _parent->get_revised_val( new_tuple,
                             *(_in_td.get_schema()) );

      const SchemaField *revised_field = _in_td.get_schema_field(new_tuple.get_revised());

      revised_val.copy( new_data + revised_field->get_offset(),
                revised_field->get_size() );

      ctxt.reset();
      ctxt.set_tuple(new_tuple.get_data());

      // Now insert the new tuple:
      for( w = _new_windows.begin();
           w != _new_windows.end();  ++w )
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( rev_tid );
        }
      else if( tuple.get_tuple_type() == TupleType::DELETION )
        {
      // Add to old
      for( w = _old_windows.begin(); w != _old_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::DELIMITER )
        {
      return(true);
        }
      else
        {
      WARN << "Unrecognized TupleType: " << tuple.get_tuple_type();
        }
    }
  else if( _state.type() == WindowState::DELETION )
    {
      if( tuple.get_tuple_type() == TupleType::INSERTION )
        {
      // Out of order
      DEBUG << "shouldn't happen..";
        }
      else if( tuple.get_tuple_type() == TupleType::HISTORICAL )
        {
      // Add to old
      for( w = _old_windows.begin(); w != _old_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::REPLACEMENT )
        {
      // Add old part of tuple to old
      // Pretty easy, actually, since you don't have
      // to muck around in revised_field for this one
      for( w = _old_windows.begin(); w != _old_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::DELETION )
        {
      DEBUG << "shouldn't happen..";
        }
      else if( tuple.get_tuple_type() == TupleType::DELIMITER )
        {
      return(true);
        }
      else
        {
      WARN << "Unrecognized TupleType: " << tuple.get_tuple_type();
        }
    }
  else if( _state.type() == WindowState::OOINSERTION )
    {
      if( tuple.get_tuple_type() == TupleType::INSERTION )
        {
      DEBUG << "shouldn't happen..";
        }
      else if( tuple.get_tuple_type() == TupleType::HISTORICAL )
        {
      // Add to old
      for( w = _old_windows.begin(); w != _old_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::REPLACEMENT )
        {
      // Add new part of tuple to old
      
      // Start by changing the ctxt to have the new tuple
      char new_data[_in_td.get_size()];
      memcpy( new_data, tuple.get_data(), _in_td.get_size() );
      Tuple new_tuple = Tuple::factory( new_data );

      string revised_val = _parent->get_revised_val( new_tuple,
                             *(_in_td.get_schema()) );

      const SchemaField *revised_field = _in_td.get_schema_field(
                                             new_tuple.get_revised());

      revised_val.copy( new_data + revised_field->get_offset(),
                revised_field->get_size() );
      ctxt.reset();
      ctxt.set_tuple(new_tuple.get_data());
      
      // Then add to old
      for( w = _old_windows.begin(); w != _old_windows.end(); ++w)
            {
          (*w)->incr(ctxt);
            }
      _tid_store.insert( tuple.get_tuple_id() );
        }
      else if( tuple.get_tuple_type() == TupleType::DELETION )
        {
      DEBUG << "shouldn't happen..";
        }
      else if( tuple.get_tuple_type() == TupleType::DELIMITER )
        {
      // Delete self
      return(true);
        }
      else
        {
      WARN << "Unrecognized TupleType: " << tuple.get_tuple_type();
        }
    }
  else // if other states
    {
      WARN << "shouldn't happen...";
    }
  
  // Delete if the window is empty.
  return _tid_store.empty();
}

bool RevisionAggregateQBox::ValueWindow::insert_tuple(/*const*/ EvalContext& ctxt)
{
  int32 window_val = _window_field->eval<int32>(ctxt);
  
  // bloot -- this only covers insertion now.
  //          Need to make a more intelligent decision.
  
  // ASSUMPTIONS:
  //    - CP sends stuff ordered right.
  //    -
  
  if ( window_val < _first_val )
    {
      DEBUG << "Window val " << window_val
        << " is before first_val " << _first_val
        << ". Not adding to window but returning false.";
      return false;
    }
  if (window_val >= _last_val)
    {
      if (_slack_remaining > 0)
    {
      DEBUG << "Window val " << window_val
        << " is after last_val " << _last_val
        << " but we still have slack(" << _slack_remaining
        << "). Not adding to window but returning false.";
      _slack_remaining--;
      return false;
    } 
      else
    {
      DEBUG << "Window val " << window_val
        << " is after last_val " << _last_val
        << " and we are out of slack, emit and return true.";
      // Emit.
      // SPECIAL CASE FOR INSERTION WINDOW
      if (get_state() == WindowState::INSERTION) 
        {
          uint8 *tuple = as_insertion_tuple();
          parent()->emit_tuple(tuple);
        } 
      else 
        {
          vector<uint8 *>                  tuples = as_tuples();
          vector<uint8 *>::const_iterator  t;
          for( t = tuples.begin(); t != tuples.end(); ++t )
        {
          parent()->emit_tuple( *t );
        }
        }
      // And delete.
      return true;
      
    }
    }
  DEBUG << "Adding tuple with window_val " << window_val
    << " to " << *this;
  return( RevisionAggregateQBox::Window::insert_tuple(ctxt) );
  
  // Don't emit.
  // return false;
}

bool RevisionAggregateQBox::TupleWindow::insert_tuple(/*const*/ EvalContext& ctxt)
{
  // bloot -- need this to be right -- theoretical problems with ooi,del
  // bloot -- need more intelligent decision of add/emit/delete
  _count++;
  // store what ids when into this window
  _ids_in_window.push_back(Tuple::factory(ctxt.getTuple()).get_tuple_id());
  
  RevisionAggregateQBox::Window::insert_tuple(ctxt);
  if (_count >= _window_size)
    {
      // Emit.
      vector<uint8 *>                  tuples = as_tuples();
      vector<uint8 *>::const_iterator  t;
      for( t = tuples.begin(); t != tuples.end(); ++t )
        {
      parent()->emit_tuple( *t );
        }
      // And delete.
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

// Takes a tuple and emits if appropriate 
void RevisionAggregateQBox::emit_tuple( const uint8 *data )
{
  Tuple tuple = Tuple::factory( data );
  TupleType tt = tuple.get_tuple_type();
  EnqIterator ei = enq(0);
  
  if( (tt == TupleType::REPLACEMENT || tt == TupleType::HISTORICAL) && tuple.get_revision_id() == -1 )
    {
      Tuple last_tuple = Tuple::factory( _last_emitted_tuple );
      tuple.set_revision_id( last_tuple.get_revision_id() );
    }
  
  bool stepping = // STEPPER: Change this to change paused-on OUTPUT tuples in DEBUG
    tt == TupleType::REQUEST ||
    tt == TupleType::REPLACEMENT;
  
  if(    ( tt == TupleType::HISTORICAL || tt == TupleType::DELIMITER )
     && !get_emit_hist() )
    {
      // End of upstream processing -- ignore these
      //      return false;
    }
  else if( tt == TupleType::DELIMITER )
    {
      uint8 *out = (uint8 *)ei.tuple();
      memcpy( out, data, HEADER_SIZE );
      memcpy( out + HEADER_SIZE,
          _last_emitted_tuple + HEADER_SIZE,
          _out_tuple_size - HEADER_SIZE      );
      
      TupleDescription td = TupleDescription( get_out_schema(0) );
      INFO << "RevisionAggregateQBox " << get_name() << " OUT: "
       << td.tuple_to_string( out );
      if(stepping)
    {
      ////      DEBUG << (char)getchar();
    }
      ++ei;
      get_output(0).notify_enq();
    }
  else
    {
      memcpy( _last_emitted_tuple, data, _out_tuple_size );
      uint8 *out = (uint8 *)ei.tuple();
      memcpy( out, data, _out_tuple_size );
      ++ei;
      
      TupleDescription td = TupleDescription( get_out_schema(0) );
      INFO << "RevisionAggregateQBox " << get_name() << " OUT: "
       << td.tuple_to_string( out );
      if(stepping)
    {
      ////      DEBUG << (char)getchar();
    }
      get_output(0).notify_enq();
    }
}

// Turns a INSERTIONS window into a uint8* tuple for emitting
uint8* RevisionAggregateQBox::Window::as_insertion_tuple() const
{
  // TID THINGY
  // Fix up tid of the window, based on orderby/groupby
  if( _parent->_first_order_by.find( _group_by ) == _parent->_first_order_by.end() )
    {
      // Initialize this group-by value with new entries
      _parent->_first_order_by[_group_by] = get_start_value();
    }
  unsigned int blocknum = ((get_start_value() - _parent->_first_order_by[_group_by]) / _parent->_advance_size) / _parent->_block_size;
  if( _parent->_tid_block_allocation[_group_by].size() <= blocknum )
    {
      _parent->_tid_block_allocation[_group_by].push_back( _parent->_next_unallocated_tid_block );
      _parent->_next_unallocated_tid_block++;
    }
  int block_offset = ((get_start_value() - _parent->_first_order_by[_group_by]) / _parent->_advance_size) % _parent->_block_size;
  int tid = ((_parent->_tid_block_allocation[_group_by][blocknum]) * _parent->_block_size) + block_offset + 1;
  
  char *data = new char[_out_td.get_size()];
  Tuple output = Tuple::factory(data);
  int offset = 0;
  
  // Header
  output.set_timestamp( t0() );
  output.set_local_timestamp( lt0() );
  output.set_tuple_id( tid );
  output.set_tuple_type(TupleType::INSERTION );
  output.set_revision_id( rev_id() );
  output.set_quality( 0.0 );           // for now
  output.set_win_spec( 0 );            // for now
  output.set_fake_flag( false );       // for now
  output.set_revised( -1 );
  output.set_tuple_stime( _max_stime );
  
  offset = HEADER_SIZE;
  
  // Copy group_by.
  string gbv = group_by_value();
  memcpy(data + offset, gbv.c_str(), gbv.length());
  offset += gbv.length();
  
  // Copy state value.
  int start = get_start_value();
  memcpy(data + offset, &start, sizeof(start));
  offset += sizeof(start);
  
  // Copy final from aggregate.
  for( unsigned int i = 0; i < _old_windows.size(); i++ )
    {
      _old_windows.at(i)->final(data + offset);
      offset += _old_windows.at(i)->get_aggregate()->get_output_size();
    }
  //    offset += final_value(data + offset);
  
  // revised_value
  const SchemaField *current_field = _out_td.get_schema_field(_out_td.index_of_field( "revised_value" ) );
  memset( data + current_field->get_offset(),
      ~0,
      current_field->get_size() );
  
  // update the last order by emitted
  parent()->_last_order_by_emitted[_group_by] = get_start_value();
  
  return (uint8 *)data;
}

// Turns a window into a vector of tuples for emitting
vector<uint8 *> RevisionAggregateQBox::Window::as_tuples() const
{
  vector<uint8 *> tuple_vector;
  
  // Fix up tid of the window, based on orderby/groupby
  if( _parent->_first_order_by.find( _group_by ) == _parent->_first_order_by.end() )
    {
      // Initialize this group-by value with new entries
      _parent->_first_order_by[_group_by] = get_start_value();
    }
  unsigned int blocknum = ((get_start_value() - _parent->_first_order_by[_group_by]) / _parent->_advance_size) / _parent->_block_size;
  if( _parent->_tid_block_allocation[_group_by].size() <= blocknum )
    {
      _parent->_tid_block_allocation[_group_by].push_back( _parent->_next_unallocated_tid_block );
      _parent->_next_unallocated_tid_block++;
    }
  int block_offset = ((get_start_value() - _parent->_first_order_by[_group_by]) / _parent->_advance_size) % _parent->_block_size;
  int tid = ((_parent->_tid_block_allocation[_group_by][blocknum]) * _parent->_block_size) + block_offset + 1;
  
  if( _state != WindowState::REPLACEMENT )
    {
      // Just drop out one tuple based on _old_windows
      tuple_vector.push_back( new uint8[_out_td.get_size()] );
      uint8 *t = tuple_vector.back();
      memset( t, 0, _out_td.get_size() );
      Tuple tuple = Tuple::factory( t );
      
      // Header:
      tuple.set_timestamp( t0() );
      tuple.set_local_timestamp( lt0() );
      tuple.set_tuple_id( tid );

      tuple.set_tuple_type( _state == WindowState::DELETION ? TupleType::DELETION : 
                _state == WindowState::HISTORICAL ? TupleType::HISTORICAL : 
                TupleType::INSERTION );

      tuple.set_revision_id( rev_id() );
      tuple.set_quality( 0.0 );           // for now
      tuple.set_win_spec( 0 );            // for now
      tuple.set_fake_flag( false );       // for now
      tuple.set_array_flag( false );      // for now
      tuple.set_revised( -1 );
      tuple.set_tuple_stime( _max_stime );
      
      // Group-by, order-by, aggregates, revised value = -1
      bool grouping = _group_by.size() != 0;
      uint16 current_size;
      uint16 current_offset;

      if(grouping)
        {
      current_size = _out_td.get_schema_field(0)->get_size();
      current_offset = _out_td.get_schema_field(0)->get_offset();

      _group_by.copy( (char *)t + current_offset,
              _group_by.size() < current_size ? _group_by.size() : current_size );
        }
      
      current_offset = _out_td.get_schema_field( grouping?1:0 )->get_offset();
      *( (int *)( t + current_offset ) ) = get_start_value();
      
      for( unsigned int i = 0; i < _old_windows.size(); i++ )
        {
      current_offset = _out_td.get_schema_field( i + 1 + (grouping?1:0) )->get_offset();
      _old_windows.at(i)->final( (char *)t + current_offset );
        }
      
      const SchemaField *current_field = _out_td.get_schema_field(_out_td.index_of_field( "revised_value" ) );
      memset( t + current_field->get_offset(),
          ~0,
          current_field->get_size()          );
    }
  else
    {
      // Replacement tuple(s) coming out.
      
      // Do the running copy trick, and if tuple_vector is empty at the end,
      // then just emit a historical.
      char running_data[_out_td.get_size()];
      memset( running_data, 0, _out_td.get_size() );
      Tuple tuple = Tuple::factory( running_data );
      
      // Header:
      tuple.set_timestamp( t0() );
      tuple.set_local_timestamp( lt0() );
      tuple.set_tuple_id( tid );
      tuple.set_tuple_type( TupleType::REPLACEMENT );
      tuple.set_revision_id( rev_id() );
      tuple.set_quality( 0.0 );           // for now
      tuple.set_win_spec( 0 );            // for now
      tuple.set_fake_flag( false );       // for now
      tuple.set_array_flag( false );      // for now
      tuple.set_revised( -1 );
      tuple.set_tuple_stime( _max_stime );
      
      // Group-by, order-by, aggregates, revised value = -1 (to start)
      bool grouping = _group_by.size() != 0;
      uint16 current_size;
      uint16 current_offset;

      if(grouping)
        {
      current_size = _out_td.get_schema_field(0)->get_size();
      current_offset = _out_td.get_schema_field(0)->get_offset();

      _group_by.copy( running_data + current_offset,
                      _group_by.size() < current_size ? _group_by.size() : current_size );
        }
      
      current_offset = _out_td.get_schema_field( grouping?1:0 )->get_offset();
      *( (int *)( running_data + current_offset ) ) = get_start_value();
      
      for( unsigned int i = 0; i < _old_windows.size(); i++ )
        {
      current_offset = _out_td.get_schema_field( i + 1 + (grouping?1:0) )->get_offset();
      _old_windows.at(i)->final( running_data + current_offset );
        }
      
      const SchemaField *revised_field = _out_td.get_schema_field(_out_td.index_of_field( "revised_value" ) );
      memset( running_data + revised_field->get_offset(),
          ~0,
          revised_field->get_size()          );
      
      // Now copy the header over into new_data, and put the fields from
      // _new_windows in instead
      char  new_data[_out_td.get_size()];
      memset( new_data, 0, _out_td.get_size() );
      Tuple new_tuple = Tuple::factory( new_data );
      memcpy( new_data, running_data,
          _out_td.get_schema_field(1+(grouping?1:0))->get_offset() );
      for( unsigned int i = 0; i < _new_windows.size(); i++ )
        {
      current_offset = _out_td.get_schema_field( i + 1 + (grouping?1:0) )->get_offset();
      _new_windows.at(i)->final( new_data + current_offset );
        }
      
      // For each field in _out_td, compare the fields, and push a tuple
      // if they're different. Don't bother with revised_field
      for( unsigned int i = 0; i < _out_td.get_num_fields() - 1; i++ )
        {
      current_offset = _out_td.get_schema_field( i )->get_offset();
      current_size = _out_td.get_schema_field( i )->get_size();
      if( memcmp( running_data + current_offset,
              new_data + current_offset,
              current_size                   ))
            {
          memcpy( running_data + revised_field->get_offset(),
              new_data + current_offset,
              current_size                               );
          tuple.set_revised( i );
          tuple_vector.push_back( new uint8[_out_td.get_size()] );
          uint8 *t = tuple_vector.back();
          memcpy( t, running_data, _out_td.get_size() );
          memcpy( running_data + current_offset,
              new_data + current_offset,
              current_size                    );
            }
        }
      
      // If it's empty, historical
      if( tuple_vector.empty() )
        {
      tuple_vector.push_back( new uint8[_out_td.get_size()] );
      uint8 *t = tuple_vector.back();
      memset( t, 0, _out_td.get_size() );
      Tuple tuple = Tuple::factory( t );
      
      // Header:
      tuple.set_timestamp( t0() );
      tuple.set_local_timestamp( lt0() );
      tuple.set_tuple_id( tid );
      tuple.set_tuple_type( TupleType::HISTORICAL );
      tuple.set_revision_id( rev_id() );
      tuple.set_quality( 0.0 );           // for now
      tuple.set_win_spec( 0 );            // for now
      tuple.set_fake_flag( false );
      tuple.set_revised( -1 );
      
      // Group-by, order-by, aggregates, revised value = -1
      bool grouping = _group_by.size() != 0;
      uint16 current_size;
      uint16 current_offset;
      if(grouping)
            {
          current_size = _out_td.get_schema_field(0)->get_size();
          current_offset = _out_td.get_schema_field(0)->get_offset();

          _group_by.copy( (char *)t + current_offset,
                  _group_by.size() < current_size ? _group_by.size() : current_size );
            }
      
      current_offset = _out_td.get_schema_field( grouping?1:0 )->get_offset();
      *( (int *)( (char *)t + current_offset ) ) = get_start_value();
      
      for( unsigned int i = 0; i < _old_windows.size(); i++ )
            {
          current_offset = _out_td.get_schema_field( i + 1 + (grouping?1:0) )->get_offset();
          _old_windows.at(i)->final( (char *)t + current_offset );
            }
      
      const SchemaField *current_field = _out_td.get_schema_field(_out_td.index_of_field( "revised_value" ) );
      memset( t + current_field->get_offset(),
          ~0,
          current_field->get_size()          );
        }
    }
  if (_state == WindowState::INSERTION) 
    parent()->_last_order_by_emitted[_group_by] = get_start_value();
  return( tuple_vector );
}


BOREALIS_NAMESPACE_END

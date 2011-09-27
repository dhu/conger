#include <CPView.h>
#include "CPQBox.h"

BOREALIS_NAMESPACE_BEGIN;

CPView::CPView(ptr<CPView> base_view, ptr<CPQBox> base_cp, string new_view_id)
{
  DEBUG << "CPView constructor : given CPView, new_view_id is " << new_view_id;
  FATAL << "THIS CONSTRUCTOR HASN'T BEEN TESTED SINCE IT ISN'T USED";  ////////////////////////////////

  _view_id = new_view_id;
  _view_desc = *(base_view->get_description().get());   /// WILL NEED TO BE CHANGED ... SEE OTHER CONSTRUCTOR BELOW
  _cp_view_type = _view_desc.get_cpview_type();
  _order_by_method = _view_desc.get_order_by_method();

  _cp = base_cp;
  _view_type = AUTO;


  //Temporary init of the radius to a fixed number and call the CP's setRadius function
  //    _radius = 4;
  _radius = _view_desc.get_radius();
  //  int radius = param("radius" , PARAM_NON_EMPTY);
  //  _cp->set_radius();

  //initialize pointers
  _auto_start_ptr = DEFAULT_INT;
  _auto_end_ptr = DEFAULT_INT;

  //  _cp->set_pointers(_view_id, _auto_start_ptr, _auto_end_ptr);
  //set pointers (start/end) on CP corresponding to this view
  //_tq is not initialized here, it's set by setTupleQueue() in CP

  // CPViewDescription stores only order_by_field in string
  string order_by_field_string = base_view->get_description()->get_order_by_field_string();
  ExprContext ctxt;
  _order_by_field = Expression::parse(order_by_field_string, ctxt);

}

CPView::CPView( ptr<CPViewDescription> desc,
                ptr<CPQBox> base_cp,
                string new_view_id,
                TupleDescription tuple_desc,
                view_type v_type )
  : _view_desc(*desc)
{
  DEBUG << "CPView constructor WITH view_type : given CPViewDescription, new_view_id is " << new_view_id;

  //  _view_desc = *(new CPViewDescription(*(desc.get())));
  _view_id = new_view_id;
  _cp_view_type = _view_desc.get_cpview_type();
  _order_by_method = _view_desc.get_order_by_method();
  _view_type =v_type;
  _tuple_desc = tuple_desc;
  _cp = base_cp;

  _radius = _view_desc.get_radius();

  //initialize pointers
  if (v_type == AUTO)
    {
      _auto_start_ptr = DEFAULT_INT;
      _auto_end_ptr = DEFAULT_INT;
    }
  else if (v_type == MANUAL)
    {
      FATAL << "NOT TESTED"; /////////////////////////////
      _man_start_ptr = NULL;
      _man_end_ptr = NULL;
      //    _cp->set_pointers(_view_id, _man_start_ptr, _man_end_ptr);
    }
  else
    {
      DEBUG << "Wrong type provided for CPView constructor";
    }

  //_tq is not initialized here, it's set by setTupleQueue()

  // CPViewDescription stores only order_by_field in string
  string order_by_field_string = _view_desc.get_order_by_field_string();

  ExprContext ctxt;
  //ctxt.setTupleDescription(tuple_desc);
  ctxt.set_context_schema( *tuple_desc.get_schema() );
  _order_by_field = Expression::parse(order_by_field_string, ctxt);

}



CPView::~CPView()
{
  //removes itself from its CP's list of views
  _cp->remove_cpview(_view_id);
}

string CPView::get_id()
{
  return _view_id;
}

ptr<CPViewDescription> CPView::get_description()
{
  return ptr<CPViewDescription>(&_view_desc);
}

int CPView::get_radius()
{
  return _radius;
}


////////////////////////////////////////////////////////////////////////////////
//
void CPView::crop_tuples(vector<uint8*>  all_tuples,
                         uint8          *tuple,
                         int             revision_id)
{
    int32 start_order_by, end_order_by, tuple_order_by, temp_order_by;
    vector<uint8*> final_tuples;
    uint8* last_tuple = NULL;
    EvalContext ctxt;
//..............................................................................


  DEBUG <<"Cropping " << all_tuples.size() << " tuples from history in CPVIEW with id of "<< _view_id;
  ctxt.reset();
  ctxt.set_tuple(tuple);
  tuple_order_by = _order_by_field->eval<int32>(ctxt);
  start_order_by = tuple_order_by - _radius;
  end_order_by = tuple_order_by + _radius;

  int tuple_id = Tuple::factory(tuple).get_tuple_id();

  vector<uint8*>::iterator crop_iter;

  for(crop_iter = all_tuples.begin(); crop_iter != all_tuples.end(); crop_iter++)
    {
      ctxt.reset();
      ctxt.set_tuple(*crop_iter);
      temp_order_by = _order_by_field->eval<int32>(ctxt);
      if( (temp_order_by <=end_order_by) && 
      (temp_order_by >=start_order_by) && 
      (tuple_id != Tuple::factory(*crop_iter).get_tuple_id()) )  // don't send the tuple being revised
    {
      DEBUG << "Enquing Tuple of Type: " << Tuple::factory(*crop_iter).get_tuple_type() 
        << ", Tuple ID: " <<Tuple::factory(*crop_iter).get_tuple_id() 
        << " and Revision ID: " << Tuple::factory(*crop_iter).get_revision_id();

      final_tuples.push_back(*crop_iter);
      DEBUG << "Tuple's final value before enqueuing into TQ was "<< _tuple_desc.tuple_to_string(*crop_iter);
      last_tuple = (*crop_iter);
    }
  }
  temp_order_by = _order_by_field->eval<int32>(ctxt);

  if (last_tuple != NULL)
  {  final_tuples.push_back(make_delimiter_tuple(last_tuple, revision_id));
  }
  else  // no tuple was picked
    {
      DEBUG << "No historical tuples, thus sending made-up delimiter";
      final_tuples.push_back(make_empty_delimiter(revision_id, end_order_by));
    }

  if (_tq.get() != NULL) {
    DEBUG <<" Enqueing into the TupleQueue is hidden for now";
    _tq->enqueue_multiple(final_tuples);
  }
}


////////////////////////////////////////////////////////////////////////////////
//
uint8  *CPView::make_delimiter_tuple(uint8* last_tuple, int revision_id)
{
  DEBUG <<"Making a delimiter tuple";

  Tuple marker_tuple = Tuple::factory(new uint8[_tuple_desc.get_size()]);
  memcpy(marker_tuple.get_data() , last_tuple, _tuple_desc.get_size());
  // only care about tuple_type, revision_id and orderBy
  marker_tuple.set_tuple_type(TupleType::DELIMITER);
  marker_tuple.set_tuple_id(revision_id);
  //SET REVISED VAL CHANGED, implementation is wrong anyway

  return marker_tuple.get_data();
}


////////////////////////////////////////////////////////////////////////////////
//
uint8  *CPView::make_empty_delimiter(int revision_id, int end_order_by)
{  
  // what are other types and what's their default values??
  int default_int = -1;
  double default_double = -1.0;
  string default_string = "-1";
  Timestamp default_timestamp = Timestamp();
  bool default_bool = false;

  char *tuple = new char[_tuple_desc.get_size()];
  CatalogSchema *schema = _tuple_desc.get_schema();
  unsigned int order_by_index = schema->index_of_field(_view_desc.get_order_by_field_string());
  
  vector<SchemaField> fields = schema->get_schema_field();
  for (unsigned int i = 0; i < fields.size(); ++i)
    {
      if (order_by_index == i)  // if order-by, put the end_order_by
    memcpy(tuple+fields[i].get_offset(), &end_order_by, sizeof(int));
      else if (fields[i].get_type() == DataType::INT)
    memcpy(tuple+fields[i].get_offset(), &default_int, sizeof(int));
      else if (fields[i].get_type() == DataType::DOUBLE)
    memcpy(tuple+fields[i].get_offset(), &default_double, sizeof(double));
      else if (fields[i].get_type() == DataType::STRING)
    memcpy(tuple+fields[i].get_offset(), default_string.c_str(), fields[i].get_size());
      else if (fields[i].get_type() == DataType::TIMESTAMP)
    memcpy(tuple+fields[i].get_offset(), &default_timestamp, sizeof(Timestamp));
      else if (fields[i].get_type() == DataType::BOOL)
    memcpy(tuple+fields[i].get_offset(), &default_bool, sizeof(bool));
      else
    memcpy(tuple+fields[i].get_offset(), &default_int, fields[i].get_size());
    }

  Tuple marker_tuple = Tuple::factory(tuple);
  marker_tuple.set_tuple_type(TupleType::DELIMITER);
  marker_tuple.set_revision_id(revision_id);
  marker_tuple.set_tuple_id(revision_id);
  marker_tuple.set_fake_flag(false);
  marker_tuple.set_quality(0.0);
  marker_tuple.set_win_spec(-1);
  marker_tuple.set_revised(-1);

  // change revised_value to -1 (as bytes)
  char negative_one[sizeof(int)];
  (*(int *) negative_one) = -1;
  _cp->set_revised_val(marker_tuple, *schema, to_string(negative_one));
  
  return marker_tuple.get_data();
}


////////////////////////////////////////////////////////////////////////////////
//
vector<uint8 *>  CPView::modify_multiple_tuples(vector<uint8 *> tuples,
                                                int        tuple_size,
                                                TupleType  type,
                                                int        revision_id,
                                                bool       revisions)
{
  vector<uint8*> new_tuples;

  for (unsigned int ii = 0; ii < tuples.size(); ++ii)
  {
      if (!revisions && Tuple::factory(tuples[ii]).get_tuple_type() != TupleType::INSERTION)
      {   continue;  // do not include revisions in replay
      }
      else
      {   new_tuples.push_back(CPStorage::modify_tuple(tuples[ii],
                                                       tuple_size,
                                                       type,
                                                       revision_id));
      }
  }

  return new_tuples;
}







////////////////////////////////////////////////////////////////////////////////
//
// is called by CP on all its views every time CP recieves new tuples
void CPView::update_ptrs()
{  
  DEBUG << "CPView::" << get_id() << "::update_ptrs()"; 

  switch(_view_type)
    {
    case BOTH:
      FATAL << "NOT TESTED"; /////////////////////////////
      //advance both start_ pointers and _man_end_ptr
      /*************
      update_auto_start_ptr();
      update_auto_end_ptr();
      _man_end_ptr = last_tuple;
      //make start_ptr equal to the earlier one out of _man_start_ptr and _auto_start_ptr
      char* start_ptr;
      if (is_later(_man_start_ptr, _auto_start_ptr))
    {
      start_ptr = _man_start_ptr;
    }
      else
    {
      start_ptr = _auto_start_ptr;
    }
      //end_ptr is equal to last_tuple since in any case it's later out of _man_end_ptr and
      //_auto_end_ptr
      _cp->set_pointers(_view_id, start_ptr, last_tuple);
      ******************/
      break;
    case MANUAL:
      FATAL << "NOT TESTED"; ///////////////////////////
      /*******************
      //advance _man_end_ptr
      if (_man_start_ptr == NULL)
    {
      _man_start_ptr = last_tuple;
    }
      _man_end_ptr = last_tuple;
      //make CP update its pointers corresponding to this view
      _cp->set_pointers(_view_id, _man_start_ptr, _man_end_ptr);
      DEBUG << "_man_start_ptr set to " << _tuple_desc.tuple_to_string(_man_start_ptr);
      DEBUG << "_man_end_ptr set to " << _tuple_desc.tuple_to_string(_man_end_ptr);
      *********************/
      break;
    case AUTO:
      //advance both _auto_start_ptr and _auto_end_ptr
      update_auto_start_ptr();
      update_auto_end_ptr();
      break;

    }
  
}

////////////////////////////////////////////////////////////////////////////////
//
void CPView::update_auto_start_ptr()
{
  DEBUG << "Updating AUTO start_pointer for " << _cp_view_type << " window and order-by " << _order_by_method;
  
  // if not moving -- no need to update
  if (_cp_view_type == CPViewDescription::CPViewType::MOVING)
    {
      if (_order_by_method == CPViewDescription::OrderByMethod::TUPLENUM)
    {
      //change next line when _storage will be changed from vector(char*) to smth else
      //subtract size of view from size of CPStorage of its CP to find what's the first tuple view
      //cares about
      //(i.e. kind of counting from the end of storage)
      _auto_start_ptr = Tuple::factory(_cp->get_storage()->get_nth_tuple_from_end(_view_desc.get_size())).get_tuple_id();
    }
      else
    {
      // figure out what to do if it is by field
      EvalContext ctxt;
      ctxt.set_tuple(_cp->get_storage()->get_last_tuple());
      int32 last_tuple_val = _order_by_field->eval<int32>(ctxt);
      int first_tuple_val = last_tuple_val - _view_desc.get_size() + 1;//here assume that size os in
      if (first_tuple_val < 0)
        {
          INFO << "first_tuple_val=" << first_tuple_val << " < 0 .... resetting to 0";
          first_tuple_val = 0;
        }
      _auto_start_ptr = first_tuple_val;
    }
    }
  DEBUG << "_auto_start_ptr set to " << _auto_start_ptr;
  
}

////////////////////////////////////////////////////////////////////////////////
//
void CPView::update_auto_end_ptr()
{
  DEBUG << "Updating AUTO end_pointer";

  //if fixed => no need to update => updates only if moving
  if (_cp_view_type == CPViewDescription::CPViewType::MOVING)
    {
      if (_order_by_method == CPViewDescription::OrderByMethod::TUPLENUM)
    {
      _auto_end_ptr = Tuple::factory(_cp->get_storage()->get_last_tuple()).get_tuple_id();
    }
      else 
    {
      EvalContext ctxt;
      ctxt.set_tuple(_cp->get_storage()->get_last_tuple());
      _auto_end_ptr = _order_by_field->eval<int32>(ctxt);
      // here assuming that a moving view will always have the last N hrs,
      //i.e. end pointer is always to the very last tuple
    }
    }
  DEBUG << "_auto_end_ptr set to " << _auto_end_ptr;

}



////////////////////////////////////////////////////////////////////////////////
//
void CPView::set_tuple_queue(ptr<TupleQueue> tq)
{
  _tq = tq;
}


////////////////////////////////////////////////////////////////////////////////
//
//Sets the CPView's radius
void CPView::set_radius(int new_radius)
{
  _radius = new_radius;
  _cp->set_radius();
}

BOREALIS_NAMESPACE_END;

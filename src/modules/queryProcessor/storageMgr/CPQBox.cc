#include "CPQBox.h"

BOREALIS_NAMESPACE_BEGIN


//Setup Impl acts much like a constructor
void CPQBox::setup_impl() throw (AuroraException)
{
  set_emit_hist( true) ;
 
  TupleDescription tuple_desc (get_in_schema(0));
  _tuple_desc = tuple_desc;
}

CPQBox::CPQBox()
{
  _cp_storage = ptr<CPStorage>(new CPStorage());
  _enqueued_count = 0;
  _last_order_by = 0;

}


CPQBox::CPQBox(ptr<CPViewDescription> cp_desc, TupleDescription tuple_desc)
{
  _cp_storage = ptr<CPStorage>(new CPStorage());
  _tuple_desc = tuple_desc;
  _enqueued_count = 0;
  _last_order_by = 0;
}

void CPQBox::init_impl() throw(AuroraException)
{
  //Does not need to do anything
}

void CPQBox::run_impl(QBoxInvocation& inv) throw(AuroraException)
{
  //This is a hack, should be in setup_impl
  CatalogSchema cs_schema = (*get_in_schema( 0 ));
  TupleDescription tuple_desc (get_in_schema(0));
  _tuple_desc = tuple_desc;
  //End Hack



  DeqIterator di = deq(0);

  while (inv.continue_dequeue_on(di,0))
    {
      const void* tuple = di.tuple();
      char *input_tuple = new char[tuple_desc.get_size()];
      memcpy(input_tuple, tuple, tuple_desc.get_size());

      INFO << "CPQBox " << get_name() << " IN: " << tuple_desc.tuple_to_string(input_tuple);
      if( Tuple::factory(input_tuple).get_tuple_type() != TupleType::HISTORICAL &&
      Tuple::factory(input_tuple).get_tuple_type() != TupleType::DELIMITER     )
    {
      ////    DEBUG << getchar();
    }
      enqueue(input_tuple);
      ++di;
    }
  return;
}

void CPQBox::set_order_by_field_string(string order_by) 
{
  _order_by_field_string = order_by;
}

string CPQBox::get_order_by_field_string()
{
  return _order_by_field_string;
}

void CPQBox::set_order_by_field(string order_by)
{
  _order_by_field_string = order_by;
  ExprContext ctxt;
  ctxt.set_context_schema(*get_in_schema(0));
  _order_by_field = Expression::parse(order_by, ctxt);
}
void CPQBox::set_order_by_field(ptr<Expression> order_by)
{
  _order_by_field = order_by;
}

ptr<Expression> CPQBox::get_order_by_field()
{
  return _order_by_field;
}


void CPQBox::add_view(string view_id, ptr<CPView> cp_view)
{
  DEBUG << "Adding CPView with id=" << view_id << " to CP " << this;
  _my_cp_views.insert(make_pair(view_id, cp_view));

  DEBUG << "Setting RADIUS as a debug method";
  set_radius();
}



void CPQBox::add_sweeper_view(string view_id, ptr<TupleQueue> cp_view)
{
  DEBUG << "Adding CPView with id=" << view_id << " to CP " << this;
  _sweeper_view = cp_view;
}


void CPQBox::enqueue(const void *tuple)
{
  DEBUG << "CPQBox::enqueue(tuple): Tuple is of Type: "<< Tuple::factory(tuple).get_tuple_type();

  //Note, since Request Tuples are not implemented yet, cannot include them in if statement
  if ( (Tuple::factory(tuple).get_tuple_type() == TupleType::DELIMITER) || (Tuple::factory(tuple).get_tuple_type() == TupleType::HISTORICAL)) 
    {
      DEBUG<< "TupleType is HISTORICAL or DELIMITER";
      return;
    }
  int t_order_by;
  EvalContext ctxt;
  ctxt.reset();
  ctxt.set_tuple(tuple);
  t_order_by = _order_by_field->eval<int32>(ctxt);

  //If it is a replacement tuple, check it's revision_id.  If the revision id is null, send out historical tuples and then change the historical value
  if ( (Tuple::factory(tuple).get_tuple_type() == TupleType::REPLACEMENT) || 
       (Tuple::factory(tuple).get_tuple_type() == TupleType::DELETION) || 
       ( (Tuple::factory(tuple).get_tuple_type() == TupleType::INSERTION) && 
     (t_order_by < _last_order_by) ) )
    {      
      DEBUG <<"Tuple was a revision";
      int tuple_id, revision_id;
      tuple_id = Tuple::factory(tuple).get_tuple_id();
      revision_id = tuple_id;
      if (Tuple::factory(tuple).get_revision_id() == -1)  // only send historical tuples if revision_id = -1
    {
      DEBUG << "revision_id=1, thus sending historical tuples";
      //Get the historical tuples based on the enqueing tuple
      vector<uint8*> hist_tuples = get_historical_tuples(tuple_id, revision_id, (uint8*) tuple);
      CPViewMap::iterator crop_iter;
      for(crop_iter =_my_cp_views.begin(); crop_iter != _my_cp_views.end(); ++crop_iter)
        {
          (crop_iter->second)->crop_tuples(hist_tuples, (uint8*)tuple, revision_id);
        }
    }
      if (Tuple::factory(tuple).get_tuple_type() == TupleType::REPLACEMENT)
    {
      DEBUG<< "Tuple was a replacement, updating storage";
      //Change the old value in storage.  Perhaps just delete the old value, and change the tuple's type into insertion.
      
      int position = _cp_storage->find_exact_tuple(tuple_id);
      uint8  *new_tuple = extract(tuple_id);

      if (new_tuple != NULL)  // new_tuple may be NULL if tuple_id is not found
        {
          const SchemaField *field = _tuple_desc.get_schema_field( _tuple_desc.index_of_field( "revised_value" ) );
          const char *data = (char *)Tuple::factory(tuple).get_data() + field->get_offset();
          string str( data, field->get_size());
          const SchemaField *rfield = _tuple_desc.get_schema_field(Tuple::factory(tuple).get_revised()) ;
          str.copy( (char *)new_tuple + rfield->get_offset(), rfield->get_size());
          
          DEBUG << "The revised value of the tuple is " << _tuple_desc.tuple_to_string(Tuple::factory(new_tuple));
          _cp_storage->revise_tuple(new_tuple, position, tuple_id);
        }
    }
      else if(Tuple::factory(tuple).get_tuple_type() == TupleType::DELETION)
    {
      //Delete the old value in storage
      int position;
      position = _cp_storage->find_exact_tuple(tuple_id);
      _cp_storage->delete_from_storage(position, tuple_id);
    }
    }
  
  DEBUG<< "Past Checking for Revisions";
  
  if (Tuple::factory(tuple).get_tuple_type() == TupleType::REQUEST) 
    {
      DEBUG << "Tuple was a request tuple";
      int tuple_id, revision_id;
      tuple_id = Tuple::factory(tuple).get_tuple_id();
      revision_id = Tuple::factory(tuple).get_revision_id();
      //Note, extract(tuple_id) uses the storage hash, should probably be updated 
      uint8* raw_tuple = extract(tuple_id);
    
      if (raw_tuple != NULL)  // raw_tuple may be NULL if tuple_id is not found
    { 
      uint8  *tuple_to_send = CPStorage::modify_tuple(raw_tuple, _tuple_desc.get_size(), TupleType::HISTORICAL, revision_id);
      Tuple::factory(tuple_to_send).set_win_spec(Tuple::factory(tuple).get_win_spec()); // revised_field of historical is revised_field of request
      INFO << "Enqueuing to sweeper: " << _tuple_desc.tuple_to_string(tuple_to_send);
      ////    DEBUG << getchar();

      _sweeper_view->enqueue_multiple(vector<uint8 *>(1, tuple_to_send));
      _sweeper_view->notify_enq();
    }
      return;
    }

  if (Tuple::factory(tuple).get_tuple_type() != TupleType::INSERTION)
    {
      DEBUG << "Tuple was a revision, so not actually enqueuing to storage";
      return;
    }
  
  if (t_order_by < _last_order_by )
    {
      DEBUG << "Tuple was an out-of-order insertion";
      //special case for out of order insertions
      int position = _cp_storage->find_last_pos(t_order_by, _order_by_field);
      _cp_storage->out_of_order_enqueue((uint8 *)tuple, position);
    }
  else
    {
      DEBUG << "Tuple was an in-order insertion";
      //The insertion is in order
      DEBUG << "In cp.enqueue(tuple) for tuple" << tuple;
      _cp_storage->enqueue((uint8 *)tuple, _tuple_desc.get_size());
      //Now make sure _last_order_by is current
      if(t_order_by > _last_order_by)
    {
      _last_order_by = t_order_by;
    }
    }

  //keeps track of how many has been enqued since last discard
  _enqueued_count++;

  INFO << "Added to CPStorage " << tuple << ": Now " << _cp_storage->get_num_tuples() << " tuples in CPStorage";

  DEBUG << "Updating pointers also";
  update_view_ptrs();

  if (_enqueued_count == MAX_ENQUEUED)
    {
      discard_tuples();
      DEBUG << "Now " << _cp_storage->get_num_tuples() << " tuples in CPStorage";
      _enqueued_count = 0;
    }
}


//Iterates through the CPViews, and takes the maximum of thier radii
void CPQBox::set_radius()
{
  DEBUG << "Setting The CP's radius";
  int temp_rad = 0;
  int max_rad  = 0;
  CPViewMap::iterator rad_iter;
  for(rad_iter =_my_cp_views.begin(); rad_iter != _my_cp_views.end(); ++rad_iter)
    {
      temp_rad=(rad_iter->second)->get_radius();
      if(temp_rad > max_rad)
    {
      max_rad = temp_rad;
    }
    }
  _radius = max_rad;
  DEBUG<< "CP's radius has been set to " << max_rad;
}


void CPQBox::update_view_ptrs()
{
  if (_my_cp_views.size() == 0)
    FATAL << "WHAT!!!! No CPViews. There must be atleast one CPView. Check AuroraNode::create_cpview()";

  CPViewMap::iterator iter;
  for (iter = _my_cp_views.begin(); iter != _my_cp_views.end(); ++iter)
    (iter->second)->update_ptrs();
}


bool CPQBox::is_later(char* p1, char* p2)
{
  EvalContext ctxt;
  ctxt.set_tuple(p1);
  int32 p1_val = _order_by_field->eval<int32>(ctxt);
  ctxt.reset();
  ctxt.set_tuple(p2);
  int32 p2_val = _order_by_field->eval<int32>(ctxt);

  return (p1_val>p2_val);
}

void CPQBox::discard_tuples()
{
  DEBUG << "CPQBox::" << get_name() << "::discard_tuples()";

  // for safety make sure pointers in view are updated
  update_view_ptrs();

  int smallest_order_by = 9999999, largest_order_by = -99999;

  ptr<CPView> v;
  for (CPViewMap::iterator iter = _my_cp_views.begin(); iter != _my_cp_views.end(); ++iter)
    {
      v = iter->second;
      // the type MOVING or FIXED does not matter
      if (v->get_order_by_method() == CPViewDescription::OrderByMethod::TUPLENUM)
    {
      EvalContext ctxt;
      int order_by;
      uint8  *tuple = _cp_storage->extract(v->get_auto_start_ptr());

      if (tuple != NULL)
        {
          ctxt.reset();
          ctxt.set_tuple(tuple);
          order_by = _order_by_field->eval<int32>(ctxt);
          if (order_by < smallest_order_by)
        smallest_order_by = order_by;
        }
      tuple = _cp_storage->extract(v->get_auto_end_ptr());
      if (tuple != NULL)
        {
          ctxt.reset();
          ctxt.set_tuple(tuple);
          order_by = _order_by_field->eval<int32>(ctxt);
          if (order_by > largest_order_by)
        largest_order_by = order_by;
        }
    } 
      else // FIELD
    {
      if (v->get_auto_start_ptr() < smallest_order_by)
        smallest_order_by = v->get_auto_start_ptr();
      if (v->get_auto_end_ptr() > largest_order_by)
        largest_order_by = v->get_auto_end_ptr();
    }
    }

  _cp_storage->discard_tuples(smallest_order_by, largest_order_by, _order_by_field);

  return;
}


vector<uint8 *>  CPQBox::get_historical_tuples(int     tuple_id,
                                               int     rev_id,
                                               uint8  *tuple)
{
  DEBUG << "Inside get_historical+tuples with rev_id " << rev_id <<" and tuple_id "<< tuple_id;
  //  ptr<Expression> _order_by_field = (_my_cp_views.begin())->second->get_order_by_field();
  int start_order_by, end_order_by, tuple_order_by;
  EvalContext ctxt;

  ctxt.reset();
  ctxt.set_tuple(tuple);
  tuple_order_by = _order_by_field->eval<int32>(ctxt);
  start_order_by = tuple_order_by - _radius;
  end_order_by = tuple_order_by + _radius;
  vector<uint8 *> tuples;
  DEBUG << "Getting historical tuples from order-by: " << start_order_by << " to order-by:" << end_order_by;

  tuples = _cp_storage->extract_updated(start_order_by, end_order_by, _order_by_field);
  vector<uint8 *> hist_tuples(tuples.size());
  hist_tuples = modify_multiple_tuples(tuples, _tuple_desc.get_size(), TupleType::HISTORICAL, rev_id, false);
  return hist_tuples;
}



vector<uint8*> CPQBox::modify_multiple_tuples(vector<uint8*> tuples, int tuple_size, TupleType type, int revision_id, bool revisions)
{
  DEBUG << " Modifying multiples tuples in CP";
  vector<uint8  *> new_tuples;
  for (unsigned int ii = 0; ii < tuples.size(); ++ii)
    {
      if (!revisions && Tuple::factory(tuples[ii]).get_tuple_type() != TupleType::INSERTION)
    continue;  // do not include revisions in replay
      else
    new_tuples.push_back(CPStorage::modify_tuple(tuples[ii], tuple_size, type, revision_id));
    }
  return new_tuples;
}

uint8  *CPQBox::extract(int tuple_id)
{
  return _cp_storage->extract(tuple_id);
}

ptr<CPStorage> CPQBox::get_storage()
{
  return _cp_storage;
}

void CPQBox::remove_cpview(string view_id)
{
  int ret;
  // remove from CPView
  ret = _my_cp_views.erase(view_id);

  if (ret == 0)
    {
      ERROR << "No CPView with key [" << view_id << "]";
    }
}


BOREALIS_NAMESPACE_END

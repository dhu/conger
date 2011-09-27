#include "CP.h"

BOREALIS_NAMESPACE_BEGIN;

CP::CP(ptr<CPViewDescription> cp_desc, TupleDescription tuple_desc)
{
  DEBUG << "CP constructor : nothing to do";
  _cp_storage = ptr<CPStorage>(new CPStorage());
  _tuple_desc = tuple_desc;
  //MAX_ENQUEUED = 10;
  _enqueued_count = 0;
  _last_order_by = 0;


}

void CP::set_start_pointer(string view_id, char* start)
{
  DEBUG << "Setting start pointer for view " << view_id << " to " << start;
  /*
  StartPointers::iterator iter = _start_pointers.find(view_id);
  if (iter == _start_pointers.end())  // view_id not in map
    {
      _start_pointers[view_id] = start;
      // _start_pointers.insert(make_pair(view_id, start));
    }
  else // view_id in map
    {
      iter->second = start;
      //_start_pointers[view_id] = start;
    }
  */
}

void CP::set_end_pointer(string view_id, char* end)
{
  DEBUG << "Setting end pointer for view " << view_id << " to " << end;
/*
  EndPointers::iterator iter = _end_pointers.find(view_id);
  if (iter == _end_pointers.end())  // view_id not in map
    {
      _end_pointers[view_id] = end;
      // _end_pointers.insert(make_pair(view_id, end));
    }
  else // view_id in map
    {
      iter->second = end;
      //_end_pointers[view_id] = end;
    }
*/
}

void CP::set_pointers(string view_id, char* start, char* end)
{
  /*
  set_start_pointer(view_id, start);
  set_end_pointer(view_id, end);
  */
}

void CP::set_view(string view_id)
{
  DEBUG << "DON'T KNOW WHY THIS IS BEING CALLED!!!!!!!!!";
  //////////// THINK ABOUT WHAT THIS METHOD SHOULD DO?
  //////// WHERE DOES THE CPVIEW COME FROM GIVEN THE VIEW_ID
}

ptr<CPView> CP::create_implict_view(string base_view_id)
  //I DON'T THINK THIS IS USED ANYWHERE
{
  // find CPView from map
  CPViewMap::iterator iter= _my_cp_views.find(base_view_id);
  ptr<CPView> cp_view = iter->second;
  // get CP description
  ptr<CPViewDescription> cp_desc = cp_view->get_description();
  // create new CPView
  string new_view_id;    // WHERE DOES THIS COME FROM????????????????

  ptr<CP> myself(this);  // pointer to myself

  TupleDescription tuple_desc; ///// GET THIS FROM SOMEWHERE
  ptr<CPView> new_cp_view(new CPView(cp_desc, myself, new_view_id, tuple_desc, CPView::MANUAL));

  // add to _my_cp_views,
  _my_cp_views.insert(make_pair(new_view_id, new_cp_view));  // constructor of CPView sets start and end pointers
  //      new_cp_view->setTupleQueue(tuple_queue_ptr);

  return new_cp_view;
}

ptr<CPView> CP::create_explicit_view(ptr<CPViewDescription> desc)
  //I DON'T THINK IT'S EVER USED
{
  string new_view_id;    // WHERE DOES THIS COME FROM???????????????? PASS IT ON ... JUST LIKE TUPLE_DESC ... SEE BELOW

  ptr<CP> myself(this); // pointer to myself

  TupleDescription tuple_desc; ///// GET THIS FROM SOMEWHERE
  ptr<CPView> new_cp_view(new CPView(desc, myself, new_view_id, tuple_desc, CPView::AUTO));
  // constructor of CPView sets start and end pointers
  _my_cp_views.insert(make_pair(new_view_id, new_cp_view));
  //      new_cp_view->setTupleQueue(tuple_queue_ptr);

  return new_cp_view;
}

void CP::add_view(string view_id, ptr<CPView> cp_view)
{
  DEBUG << "Adding CPView with id=" << view_id << " to CP " << this;
  _my_cp_views.insert(make_pair(view_id, cp_view));

  DEBUG << "Setting RADIUS as a debug method";
  set_radius();
}
void CP::enqueue(const void *tuple)
{
  DEBUG<< "Begininning To Enqueue Tuple";


  DEBUG << "Tuple is of Type: "<< Tuple::factory(tuple).get_tuple_type();


  //Note, since Request Tuples are not implemented yet, cannot include them in if statement
  if ( (Tuple::factory(tuple).get_tuple_type() == TupleType::DELIMITER) || (Tuple::factory(tuple).get_tuple_type() == TupleType::HISTORICAL)) //(Tuple::factory(tuple).getTupleType() == TupleType::REQUEST) ||
    {
      DEBUG<< "tuple type was HISTORICAL or DELIMITER";
      return;
    }
  int t_order_by;
  EvalContext ctxt;
  ctxt.reset();
  ctxt.set_tuple(tuple);
  _order_by_field = ((_my_cp_views.begin())->second)->get_order_by_field();
  DEBUG<<"Got the orderbyField";
  t_order_by = _order_by_field->eval<int32>(ctxt);
  DEBUG<< "Got Eval Context";
  //If it is a replacement tuple, check it's revision_id.  If the revision id is null, send out historical tuples and then change the historical value
  if (((Tuple::factory(tuple).get_tuple_type() == TupleType::REPLACEMENT) &&  (Tuple::factory(tuple).get_revision_id() == -1)) || (Tuple::factory(tuple).get_tuple_type() == TupleType::DELETION) || ((Tuple::factory(tuple).get_tuple_type() == TupleType::INSERTION ) && (t_order_by < _last_order_by)))
    {

      DEBUG <<"Tuple was a revision";
      int tuple_id, revision_id;
      tuple_id = Tuple::factory(tuple).get_tuple_id();
      revision_id = tuple_id;
      vector<char*> hist_tuples = get_historical_tuples(tuple_id, revision_id, (char*) tuple);
      CPViewMap::iterator crop_iter;
      for(crop_iter =_my_cp_views.begin(); crop_iter != _my_cp_views.end(); ++crop_iter)
    {
      (crop_iter->second)->crop_tuples(hist_tuples, (char*) tuple, revision_id);
    }
      if (Tuple::factory(tuple).get_tuple_type() == TupleType::REPLACEMENT)
    {
      DEBUG<< "TUPLE WAS A REPLACEMENT, BUT REPLACEMENT IN STORAGE IS NOT DONE YET";
      //Change the old value in storage.  Perhaps just delete the old value, and change the tuple's type into insertion.

      int position = _cp_storage->find_exact_tuple(tuple_id);
      char* new_tuple = _cp_storage->extract(tuple_id);


      const SchemaField *field = _tuple_desc.get_schema_field( _tuple_desc.index_of_field( "revised_value" ) );
      const char *data = Tuple::factory(tuple).get_data() + field->get_offset();
      string str( data, field->get_size());
      const SchemaField *rfield = _tuple_desc.get_schema_field(Tuple::factory(tuple).get_revised()) ;
      str.copy( new_tuple + rfield->get_offset(), rfield->get_size());

      DEBUG << "The revised value of the tuple is " << _tuple_desc.tuple_to_string(Tuple::factory(new_tuple));
      _cp_storage->revise_tuple(new_tuple, position, tuple_id);

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
  // if revision tuple, then don't clean up CPStorage

  if (Tuple::factory(tuple).get_tuple_type() != TupleType::INSERTION)
    {
      DEBUG <<"TUPLE WAS A REVISION TUPLE, SO NOT ACTUALLY ENQUEING A NEW TUPLE IN STORAGE";
    return;
    }

  if (t_order_by < _last_order_by )
  {
    DEBUG <<"TUPLE WAS AN OUT OF ORDER INSERTION";
    //special case for out of order insertions
    int position = _cp_storage->find_last_pos(t_order_by, _order_by_field);
    _cp_storage->out_of_order_enqueue((char*) tuple, position);
  }
  else
    {
      DEBUG <<"TUPLE WAS AN IN ORDER INSERTION";
      //The insertion is in order
      DEBUG << "In cp.enqueue(tuple) for tuple" << tuple;
      _cp_storage->enqueue((char*)tuple, _tuple_desc.get_size());
      //Now make sure _last_order_by is current
      if(t_order_by > _last_order_by)
    {
      _last_order_by = t_order_by;
    }
    }


  DEBUG <<"PAST THE CP ENQUEUE CODE BEN WROTE";
  //keeps track of how many has been enqued since last discard
  _enqueued_count++;

  DEBUG << "Added to CPStorage " << tuple << ": Now " << _cp_storage->get_num_tuples() << " tuples in CPStorage";


  DEBUG << "Updating pointers also";

  /*
  char* last_tuple = _cp_storage->get_last_tuple();
  CPViewMap::iterator iter;
   for (iter = _my_cp_views.begin(); iter != _my_cp_views.end(); ++iter)
   {
     (iter->second)->update_ptrs(last_tuple);
   }
  */
  /*********************STUFF FOR TESTING****************************/
  //to test replaySince()
  //if (_cp_storage->getNumTuples() == 5)
  //{
  //  replaySinceTuple = (char*)tuple;
  //  }

  //if (_cp_storage->getNumTuples() == 8)
  //{
      //    DEBUG << "Calling replayAll() ";
    //DEBUG << "Calling trimStorage()";
  //DEBUG << "Calling replaySince() from 4th tuple" ;
  //CPViewMap::iterator iter = _my_cp_views.begin();
  //DEBUG << "TupleQueue size before is " << (iter->second->getTupleQueue())->size();
  //vector<char*> v =  iter->second->replayAll();
  //iter->second->trimStorage((char*)tuple);
  //iter->second->trimStorage(replaySinceTuple);
    //DEBUG << "Done with trimStorage()";
  //DEBUG << "Is about to call replaySince ";
  //vector<char*> v =  iter->second->replaySince(replaySinceTuple);
  //DEBUG << "TupleQueue size before read is " <<  (iter->second->getTupleQueue())->size();
  //vector<char*> v =  iter->second->read();
  //DEBUG << "Returned " << v.size() << " tuples as a vector";
  //DEBUG << "TupleQueue size after is " << (iter->second->getTupleQueue())->size();
  //DEBUG << "Now " << _cp_storage->getNumTuples() << " tuples in CPStorage";
  //}


    /************************END OF TESTING*****************************/

  if (_enqueued_count == MAX_ENQUEUED)
  {
    discard_tuples();
    DEBUG << "Now " << _cp_storage->get_num_tuples() << " tuples in CPStorage";
    _enqueued_count = 0;
  }
}


//Iterates through the CPViews, and takes the maximum of thier radii
void CP::set_radius()
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
  DEBUG<< "THE CP'S RADIUS HAS BEEN SET TO "<<max_rad;
}


bool CP::is_later(char* p1, char* p2)
{
  //  ptr<Expression> order_by = _view_desc.getOrderByField();

  //get CP's order_by_field from one opf its CPViews (or make later CP to have its own _order_by_field)
  ptr<Expression> _order_by_field = (_my_cp_views.begin())->second->get_order_by_field();

  EvalContext ctxt;
  ctxt.set_tuple(p1);
  int32 p1_val = _order_by_field->eval<int32>(ctxt);
  ctxt.reset();
  ctxt.set_tuple(p2);
  int32 p2_val = _order_by_field->eval<int32>(ctxt);

  return (p1_val>p2_val);
}

void CP::discard_tuples()
{
  DEBUG << "In CP's discardTuples()";

  //find the earliest tuple to keep
  StartPointers::iterator iter;
  char* ptr_to_min = (_start_pointers.begin())->second;

  for (iter = _start_pointers.begin(); iter != _start_pointers.end(); ++iter)
  {
    if (is_later(ptr_to_min, iter->second))
    {
      ptr_to_min = iter->second;
    }
  }

  //find the latest tuple to keep
  EndPointers::iterator iter1;
  char* ptr_to_max = (_end_pointers.begin())->second;

  for (iter1 = _end_pointers.begin(); iter1 != _end_pointers.end(); ++iter1)
  {
    if (! is_later(ptr_to_max, iter1->second))
    {
      ptr_to_max = iter1->second;
    }
  }

  DEBUG << "Found pointers to tuples to keep";

  //finds min and max values to keep
  ptr<Expression> _order_by_field = (_my_cp_views.begin())->second->get_order_by_field();

  EvalContext ctxt;
  ctxt.set_tuple(ptr_to_min);
  int32 min_val = _order_by_field->eval<int32>(ctxt);
  DEBUG << "Min value to keep is " << min_val;
  ctxt.reset();
  ctxt.set_tuple(ptr_to_max);
  int32 max_val = _order_by_field->eval<int32>(ctxt);
  DEBUG << "Max value to keep is " << max_val;

  DEBUG << "Discarding tuples with order_by less than " << min_val << " or greater than " << max_val;
  //calls a method in CPStorage to trim tuples with order_by values
  //less than min or greater than max
  _cp_storage->discard_tuples(min_val, max_val, _order_by_field);

  DEBUG << "Discarding tuples with order_by less than " << min_val << " or greater than " << max_val;
}




vector<char*> CP::get_historical_tuples(int tuple_id, int rev_id, char* tuple)
{
  DEBUG <<"INSIDE GET_HISTORICAL_TUPLES WITH REV ID " << rev_id <<" and tuple id "<< tuple_id;
  ptr<Expression> _order_by_field = (_my_cp_views.begin())->second->get_order_by_field();
  int start_order_by, end_order_by, tuple_order_by;
  EvalContext ctxt;
  ctxt.reset();
  ctxt.set_tuple(tuple);
  tuple_order_by = _order_by_field->eval<int32>(ctxt);
  start_order_by = tuple_order_by - _radius;
  end_order_by = tuple_order_by + _radius;
  vector<char*> tuples;
  DEBUG << "GETTING HISTORICAL TUPLES FROM ORDER BY: "<< start_order_by <<" TO ORDER BY: " <<end_order_by;
  DEBUG << "RADIUS OF CP IS SET TO " << _radius;

  tuples = extract_updated(start_order_by, end_order_by, _order_by_field);
  vector<char*> hist_tuples(tuples.size());
  hist_tuples = modify_multiple_tuples(tuples, _tuple_desc.get_size(), TupleType::HISTORICAL, rev_id, false);
  return hist_tuples;
}



vector<char*> CP::modify_multiple_tuples(vector<char*> tuples, int tuple_size, TupleType type, int revision_id, bool revisions)
{
  DEBUG <<"MODIFYING MULTIPLE TUPLES IN CP";
  vector<char*> new_tuples;
  for (unsigned int ii = 0; ii < tuples.size(); ++ii)
  {
    if (!revisions && Tuple::factory(tuples[ii]).get_tuple_type() != TupleType::INSERTION)
      continue;  // do not include revisions in replay
    else
      new_tuples.push_back(CPStorage::modify_tuple(tuples[ii], tuple_size, type, revision_id));
  }
  return new_tuples;
}


vector<char*> CP::extract(char* first, char* last)
{
  DEBUG << "In cp.extract(first, last) with args " << first << " and " << last ;
  return _cp_storage->extract(first, last);

  /*      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
      char* tuple;
      EvalContext ctxt;
      ctxt.reset();
      ctxt.setTuple(tuple);
      int a = order_by->eval<int32>(ctxt);
  */

}

char* CP::extract(int tuple_id)
{
  return _cp_storage->extract(tuple_id);
}
vector<char*> CP::extract_updated(int start_tuple_id, int end_tuple_id)
{
  return _cp_storage->extract_updated(start_tuple_id, end_tuple_id);
}

vector<char*> CP::extract(int start_order_by, int end_order_by, ptr<Expression> order_by_field, bool revisions)
{
  return _cp_storage->extract(start_order_by, end_order_by, order_by_field, revisions);
}

vector<char*> CP::extract_updated(int start_order_by, int end_order_by, ptr<Expression> order_by_field)
{
  return _cp_storage->extract_updated(start_order_by, end_order_by, order_by_field);
}


ptr<CPStorage> CP::get_storage()
{
  return _cp_storage;
}

void CP::remove_cpview(string view_id)
{
  int ret;
  // remove from CPView
  ret = _my_cp_views.erase(view_id);
  if (ret == 0)
    {
      ERROR << "No CPView with key [" << view_id << "]";
    }

  // remove from _start_pointers
  ret = _start_pointers.erase(view_id);
  if (ret == 0)
    {
      ERROR << "No _start_pointers with key [" << view_id << "]";
    }

  // remove from _end_pointers
  ret = _end_pointers.erase(view_id);
  if (ret == 0)
    {
      ERROR << "No _end_pointers with key [" << view_id << "]";
    }
}

char* CP::find_first_pointer_to_val(int val, ptr<Expression> order_by)
{
  return _cp_storage->find_first_value(val, order_by);
}

char* CP::find_last_pointer_to_val(int val, ptr<Expression> order_by)
{
  return _cp_storage->find_last_value(val, order_by);
}

BOREALIS_NAMESPACE_END;

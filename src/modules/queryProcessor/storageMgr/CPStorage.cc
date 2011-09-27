#include "CPStorage.h"

BOREALIS_NAMESPACE_BEGIN

CPStorage::CPStorage()
{
  // _num_to_wait = 1;
}

CPStorage::~CPStorage()
{
}


void CPStorage::print_all_tuples(TupleDescription *desc)
{
    for (vector<uint8*>::iterator iter  = _storage.begin();
                                  iter != _storage.end(); ++iter)
    {
        if (*iter != NULL)
        {  DEBUG << desc->tuple_to_string(*iter);
        }
    }
}


void CPStorage::enqueue(uint8 *tuple, int tuple_size)
{
  DEBUG << "In-order enqueue";
  assert(Tuple::factory(tuple).get_tuple_type() == TupleType::INSERTION);
  _latest_values[Tuple::factory(tuple).get_tuple_id()] = tuple;
  _storage.push_back(tuple);
}


void CPStorage::out_of_order_enqueue(uint8 *tuple, int position)
{
  DEBUG << "Out-of-order enqueue for INSERTION tuple";

  assert(Tuple::factory(tuple).get_tuple_type() == TupleType::INSERTION);
  vector<uint8  *>::iterator iter;

  for(iter = _storage.begin(); iter != _storage.end(); ++iter)
    {
      if ( (*iter) != NULL)
    {
      
      DEBUG <<"At the position in storage where *iter id is  = "<< Tuple::factory(*iter).get_tuple_id();
      if((*iter) == _storage[position])
        {
          DEBUG<<"Found the Correct Thing";
          _storage.insert(iter, tuple);
          break;
        }
    }
    }
  _latest_values[Tuple::factory(tuple).get_tuple_id()] = tuple;

}

uint8  *CPStorage::extract(int tuple_id)
{
  uint8  *tuple = _latest_values[tuple_id];

  if (tuple != NULL)
  {   return tuple;
  }

  WARN << "Tuple with CPStorage::extract() tuple-id [" << tuple_id << "] not found !!!!!";
  return NULL;
}


vector<uint8 *> CPStorage::extract_updated(int start_order_by, int end_order_by, ptr<Expression> order_by_field)
{
  DEBUG << "Extracting from order-by " << start_order_by << " to " << end_order_by;

  vector<uint8 *> tuples;  //the vector to return
  EvalContext ctxt;

  for (unsigned int i = 0; i < _storage.size(); ++i)  // DO NOT iter coz tuples are in order-by order
  {
    uint8  *tuple = _storage[i];
    if (tuple != NULL)
      {
    if (Tuple::factory(tuple).get_tuple_type() != TupleType::INSERTION)
      continue;
    ctxt.reset();
    ctxt.set_tuple(tuple);
    int order_by = order_by_field->eval<int32>(ctxt);
    if (order_by >= start_order_by && order_by <= end_order_by)
      {
        tuples.push_back(tuple);
      } 
      }
  }
  return tuples;
}



//Finds the position of the latest tuple which had a orderby value equal to or less than than the given value
int CPStorage::find_last_pos(int val, ptr<Expression> order_by)
{

  DEBUG <<"IN FIND_LAST_POS IN CPSTORAGE";
  uint8  *tuple;
  //  uint8* previous_tuple = NULL;
  int previous_order_by_val = -99999;
  EvalContext ctxt;
  int stored_ii=0;
  for (uint32 ii = 0; ii < _storage.size(); ++ii)
  {
    tuple = _storage[ii];
    if(tuple == NULL)
      {
    stored_ii = ii;
    while(tuple == NULL)
      {
        ++ii;
        tuple = _storage[ii];
        if(tuple != NULL)
          {
        ctxt.reset();
        ctxt.set_tuple(tuple);
        int32 order_by_val = order_by->eval<int32>(ctxt);
        if (order_by_val > val)
          {
            if (previous_order_by_val == val)
            {
              DEBUG<<"The Correct Order-By was" <<ii;
              return ii;
            }
            else
              {   ERROR << "Tuple with order_by = [" << val << "] not found !!";
              return -1;
              }
          }
        else
          {
            break;
          }
          }
      }
      }
    ctxt.reset();
    ctxt.set_tuple(tuple);
    int32 order_by_val = order_by->eval<int32>(ctxt);
    if (order_by_val > val)
      {
    if (previous_order_by_val <= val)
      {
        DEBUG <<"Not directly after a null:  Correct orderBy was "<<ii;
        return (ii);
      }
    else
      {
        DEBUG << "The tuple is the greatest value in the vector";
        return (_storage.size()-1);
        //      ERROR << "Tuple with order_by = [" << val << "] not found !!";
        //    return -1;
      }
      }

    //    previous_tuple = tuple;
    previous_order_by_val = order_by_val;
  }
  DEBUG << "The tuple is the greatest value in the vector";
  //  ERROR << "Tuple with order_by = [" << val << "] not found !!";
  return (_storage.size()-1);
}



//Finds the location in storage of the tuple with tuple ID
int CPStorage::find_exact_tuple(int tuple_id)
{
  DEBUG <<"FINDING THE EXACT TUPLE IN CPSTORAGE";

  uint8  *tuple;
  int temp_tuple_id;


  for (uint32 ii = 0; ii < _storage.size(); ++ii)
    {
      tuple = _storage[ii];
      if(tuple != NULL)
    {
      temp_tuple_id = Tuple::factory(tuple).get_tuple_id();
      if (temp_tuple_id == tuple_id)
        {
          return ii;
        }
    }
    }
  ERROR << "Tuple with CPStorage::find_exact_tuple() tuple_id = [" << tuple_id << "] not found !!";
  return -1;
}

//Changes the value of a stored tuple as the result of a REPLACEMENT tuple
void CPStorage::revise_tuple(uint8* tuple, int pos, int tuple_id)
{
  DEBUG <<"REVISING A TUPLES VALUE.  Place to insert the tuple is " << pos ;
  _storage[pos] = tuple;
  _latest_values[tuple_id] = tuple;
}

//Deletes a value from storage as the result of a DELETION tuple
void CPStorage::delete_from_storage(int pos, int tuple_id)
{
  DEBUG <<"DELETEING FROM STORAGE";
  //vector<uint8  *>::iterator iter;
  //  iter = iter + pos-1 ;
  _storage[pos] = NULL;
  //  _storage.erase(iter);

  _latest_values[tuple_id] = NULL;
  //_latest_values.erase(tuple_id);
}

void CPStorage::discard_tuples(int32 min_val, int32 max_val, ptr<Expression> order_by)
{
  DEBUG << "CPStorage::discard_tuples() : < " << min_val << " and > " << max_val;
  EvalContext ctxt;
  int val, cnt = 0, old = _storage.size();

  vector<uint8  *>::iterator iter = _storage.begin();

  while (iter != _storage.end()) 
    {
      ctxt.reset();
      if (*iter != NULL)
    {
      ctxt.set_tuple(*iter);
      val = order_by->eval<int32>(ctxt);
      if (val < min_val || val > max_val)
        {
          _latest_values.erase(Tuple::factory(*iter).get_tuple_id());
          iter = _storage.erase(iter);
          ++cnt;
        }
      else
        ++iter; // next
    }
      else
    iter = _storage.erase(iter); // when NULL, cleanup
    }
  DEBUG << "Discarded " << cnt << " tuples from storage ... size changed from " << old << " to " << _storage.size();

  return;
}

uint8  *CPStorage::get_last_tuple()
{
  uint8  *l_tuple;
  l_tuple = _storage[_storage.size()- 1];
  int i = 2;

  while (l_tuple == NULL)
    {
      l_tuple = _storage[_storage.size() - i];
      i++;
    }
  return l_tuple;
}

uint8  *CPStorage::get_nth_tuple_from_end(int n)
{
  uint8  *l_tuple;
  do 
    {
      l_tuple = _storage[(_storage.size() - n)];
      n++;
    }
  while (l_tuple == NULL);
  return l_tuple;
}

int CPStorage::get_num_tuples()
{
  return _storage.size();
}

uint8  *CPStorage::modify_tuple(uint8     *tuple,
                                int        tuple_size,
                                TupleType  type,
                                int        revision_id)
{
    DEBUG << "Modifying tuple with new type=" << type << " and new revision_id=" << revision_id;

    Tuple new_tuple = Tuple::factory(new uint8[tuple_size]);
    new_tuple.copy_tuple(Tuple::factory(tuple), tuple_size);
    new_tuple.set_tuple_type(type);
    new_tuple.set_revision_id(revision_id);

  return new_tuple.get_data();
}


uint8* CPStorage::modify_tuple_type(uint8     *tuple,
                                    int        tuple_size,
                                    TupleType  type)
{
    DEBUG << "Modifying tuple with new type=" << type;

    Tuple new_tuple = Tuple::factory(new uint8[tuple_size]);
    new_tuple.copy_tuple(Tuple::factory(tuple), tuple_size);
    new_tuple.set_tuple_type(type);

    return new_tuple.get_data();
}
  


BOREALIS_NAMESPACE_END

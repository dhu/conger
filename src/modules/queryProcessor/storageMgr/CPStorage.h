#ifndef CPSTORAGE_H
#define CPSTORAGE_H

#include <vector>
#include <Expression.h>
#include "common.h"
#include "Tuple.h"
#include "FieldExt.h"

BOREALIS_NAMESPACE_BEGIN;


class CPStorage
{

 public:

  // constructor
  CPStorage();

  // destructor
  ~CPStorage();

  // Debugging method
  void print_all_tuples(TupleDescription *desc);

  // enqueue tuple
  void enqueue(uint8  *tuple, int tuple_size);

  //enqueue an out of order tuple
  void out_of_order_enqueue(uint8  *tuple, int position);

  // extracts tuple with given tuple_id
  uint8* extract(int tuple_id);

  vector<uint8*> extract_updated(int start_order_by, int end_order_by, ptr<Expression> order_by_field);

  //Find the position in the storage of a specific tuple
  int find_exact_tuple(int tuple_id);

  //Change the value of a tuple in storage to it's revised value
  void revise_tuple(uint8* tuple, int pos, int tuple_id);

  //Deletes a value from storage as the result of a DELETION tuple
  void delete_from_storage(int pos, int tuple_id);

  //returns a pointer to last tuple
  uint8* get_last_tuple();

  //returns a pointer to the tuple that is on the n'th position from the end of storage
  uint8* get_nth_tuple_from_end(int n);

  // returns the number of tuples in storage (excluding revision tuples)
  int get_num_tuples();

  // returns the position in the storage of the last tuple with val in the order_by field
  int find_last_pos(int val, ptr<Expression> order_by);

  //eliminates all tuples with order_by value less than min_val or greater than max_val
  void discard_tuples(int32 min_val, int32 max_val, ptr<Expression> order_by);



  /****** STATIC METHODS TO MODIFY TUPLES *****/
  static uint8  *modify_tuple(uint8  *tuple, int tuple_size, TupleType type, int revision_id);
  static uint8  *modify_tuple_type(uint8  *tuple, int tuple_size, TupleType type);
  /****** STATIC METHODS TO MODIFY TUPLES *****/

  
  // maps all tuples' keys to tuples
  typedef map<int32, uint8 *> key_to_tuple;

 private:

  // using a vector to store tuples for now.
  // WHAT IS A BETTER STORAGE???? BDB?
  vector<uint8 *>   _storage;

  // maps tupleKeys of all tuples to their most recent values
  key_to_tuple    _latest_values;
};

BOREALIS_NAMESPACE_END;

#endif                           // CPSTORAGE_H

#ifndef CP_H
#define CP_H

#include "CPStorage.h"
#include "CPViewDescription.h"
#include "TupleQueue.h"
#include "CPView.h"
#include <vector>
#include <map>
#include "common.h"

BOREALIS_NAMESPACE_BEGIN;

#define MAX_ENQUEUED 1000

class CPView;

class CP
{

  //  friend class CPView;

 public:

  struct ltstr
  {
    bool operator()(string s1, string s2) const
    {
      return strcmp(s1.c_str(), s2.c_str()) < 0;
    }
  };


  /// constructor
  CP(ptr<CPViewDescription> cp_desc, TupleDescription tuple_desc);

  /// destructor
  ~CP() {};

  /// change the start_pointer for the given view_id
  void set_start_pointer(string view_id, char* start);

  /// change the end_pointer for the given view_id
  void set_end_pointer(string view_id, char* end);

  /// set the start and end pointers for view_id
  void set_pointers(string view_id, char* start, char* end);

  /// create a view identical to the one in base_view_id
  ptr<CPView> create_implict_view(string base_view_id);

  /// create a view with the specifications is desc
  ptr<CPView> create_explicit_view(ptr<CPViewDescription> desc);

  /// enqueue tuple
  void enqueue(const void *tuple);

  //Get historical tuples for a revision
  vector<char*> get_historical_tuples(int tuple_id, int rev_id, char* tuple);

  //Modify a vector of tuples
  vector<char*>  modify_multiple_tuples(vector<char*> tuples, int tuple_size, TupleType type, int revision_id, bool revisions);

  //Set the CP's _radius
  void set_radius();

  /// return tuples between first and last pointers
  vector<char*> extract(char* first, char* last);

  // NEW -- SINCE TUPLE_IDs ARE IN HEADER
  // extract tuple with the given tuple_id
  char* extract(int tuple_id);   ///// use this

  vector<char*> extract_updated(int start_tuple_id, int end_tuple_id);

  // extracts tuples from start_order_by to end_order_by given by order_by_field
  // bool revisions says whether to include revisions or not
  vector<char*> extract(int start_order_by, int end_order_by, ptr<Expression> order_by_field, bool revisions);

  //extracts tuples from start_order_by to end_order_by given by order_by_field, all revisions applied except for pending ones
  vector<char*> CP::extract_updated(int start_order_by, int end_order_by, ptr<Expression> order_by_field);


  ptr<CPStorage> get_storage();


  /// retrieve the view from my_cp_views given view_id
  ptr<CPView> get_cpview(string view_id);

  map<string, ptr<CPView>, ltstr> get_all_cpviews() { return _my_cp_views; }

  /// removes CPView, _start_pointers and _end_pointers for view_id
  void remove_cpview(string view_id);

  /// returns the first tuple with val in the order_by field
  char* find_first_pointer_to_val(int val, ptr<Expression> order_by);

  /// returns the last tuple with val in the order_by field
  char* find_last_pointer_to_val(int val, ptr<Expression> order_by);

  // TODO stuff
  // 1. how is bulk copying of tuples to storage done? The listener is supposed to
  // copy the TupleQueue to CP's storage. How's that done???

  //adds a view to its list of views
  void set_view(string view_id);

  void add_view(string view_id, ptr<CPView> cp_view);

  //discards tuples that are not used by any views
  void discard_tuples();

  string as_string() const
  {
    return string() + "CP{num_views=" + _my_cp_views.size() + ",num_tuples=" + _cp_storage->get_num_tuples() + "}";
  }

  bool is_later(char* p1, char* p2);

// informs the storage that a downstream box processed a revision tuple for this tuple_id
  void revision_consumed(int _tuple_id)
  {
    _cp_storage->revision_consumed(_tuple_id);
  }

  //int getNumConnectedBoxes()
  //{
  //  return _num_boxes;
  //}

  //sets number of connected downstream boxes on the CPStorage
  void set_num_boxes(int num)
  {
    DEBUG << "Sets number of boxes for CP in CP!!!!!!!!!";
    _cp_storage->set_num_boxes(num);
  }

 private:
  // maps CPView's view_ids to start pointers
  typedef map<string, char*, ltstr> StartPointers;
  StartPointers _start_pointers;

  // maps CPView's view_ids to end pointers
  typedef map<string, char*, ltstr> EndPointers;
  EndPointers _end_pointers;

  // all views current on "this" CP
  typedef map<string, ptr<CPView>, ltstr> CPViewMap;
  CPViewMap _my_cp_views;

  // where do I store tuples? => here
  ptr<CPStorage> _cp_storage;

  //Stores the order by field
  ptr<Expression> _order_by_field;

  TupleDescription _tuple_desc;

  // the current radius of the CP
  int _radius;

  //  int32 MAX_QUEUED;
  int32 _enqueued_count;

  //just for testing replaySince(), should be removed afterwards
  char* replay_since_tuple;

  //Store the last order by value enqueued into the CP
  int _last_order_by;
  // number of immediate downstream boxes
  //_num_boxes;
};

BOREALIS_NAMESPACE_END;

#endif                         // CP_H

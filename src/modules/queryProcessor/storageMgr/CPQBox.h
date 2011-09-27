#ifndef CPQbox_H
#define CPQbox_H

#include "CPStorage.h"
#include "CPViewDescription.h"
#include "TupleQueue.h"
#include "CPView.h"
#include <vector>
#include <map>
#include "common.h"
#include "QBox.h"
#include "Expression.h"

BOREALIS_NAMESPACE_BEGIN

#define MAX_ENQUEUED 10000

class CPView;

class CPQBox :public QBox
{

 public:

  struct ltstr
  {
    bool operator()(string s1, string s2) const
    {
      return strcmp(s1.c_str(), s2.c_str()) < 0;
    }
  };


  /// Box Style constructor
  CPQBox();

  /// constructor
  CPQBox(ptr<CPViewDescription> cp_desc, TupleDescription tuple_desc);

  /// destructor
  ~CPQBox() {};

 protected: 
  /// Replaces much of the constructors function
  void setup_impl() throw (AuroraException);

  /// Qbox does not require any special states to be initialized, so this method is blank
  void init_impl() throw (AuroraException);

  /// Basically dequeues a tuple from the tupleQueue and calls enqueue on it.
  void run_impl( QBoxInvocation&) throw (AuroraException);

 public:
  /// stores order-by as a string
  void set_order_by_field_string(string order_by);
  string get_order_by_field_string();

  /// stores order-by as an Expression
  void set_order_by_field(string order_by);
  void set_order_by_field(ptr<Expression> order_by);

  ptr<Expression> get_order_by_field();

  /// enqueue tuple
  void enqueue(const void *tuple);

  /// Get historical tuples for a revision
  vector<uint8 *> get_historical_tuples(int tuple_id, int rev_id, uint8 *tuple);

  /// Modify a vector of tuples
  vector<uint8 *>  modify_multiple_tuples(vector<uint8 *> tuples, int tuple_size, TupleType type, int revision_id, bool revisions);

  /// Set the CP's _radius by taking the max(radius) of all views
  void set_radius();

  // NEW -- SINCE TUPLE_IDs ARE IN HEADER
  // extract tuple with the given tuple_id
  uint8  *extract(int tuple_id);   ///// use this

  /// returns _cp_storage
  ptr<CPStorage> get_storage();


  /// retrieve the view from my_cp_views given view_id
  ptr<CPView> get_cpview(string view_id);

  /// call update_ptrs() on all views
  void update_view_ptrs();

  /// removes CPView, 
  void remove_cpview(string view_id);

  // TODO stuff
  // 1. how is bulk copying of tuples to storage done? The listener is supposed to
  // copy the TupleQueue to CP's storage. How's that done???

  //adds a view to its list of views
  void add_view(string view_id, ptr<CPView> cp_view);
  
  //Add a view that points to a sweeper
  void add_sweeper_view(string view_id, ptr<TupleQueue> cp_view);

  //discards tuples that are not used by any views
  void discard_tuples();

  string as_string() const
    {
      return string() + "CP{num_views=" + _my_cp_views.size() + ",num_tuples=" + _cp_storage->get_num_tuples() + "}";
    }

  bool is_later(char* p1, char* p2);

 private:

  // all views current on "this" CP
  typedef map<string, ptr<CPView>, ltstr> CPViewMap;
  CPViewMap _my_cp_views;

  //Pointer to a view pointing to a sweeper
  ptr<TupleQueue> _sweeper_view;

  // where do I store tuples? => here
  ptr<CPStorage> _cp_storage;

  //Stores the order by field
  ptr<Expression> _order_by_field;
  string _order_by_field_string;

  TupleDescription _tuple_desc;

  // the current radius of the CP
  int _radius;

  // number of tuples enqueued so far
  // used to determine when to discard_tuples()
  int32 _enqueued_count;

  //just for testing replaySince(), should be removed afterwards
  char* replay_since_tuple;

  //Store the last order by value enqueued into the CP
  int _last_order_by;
};

BOREALIS_NAMESPACE_END

#endif                         // CP_H

#ifndef CPVIEW_H
#define CPVIEW_H

#include "TupleQueue.h"
#include <Stream.h>
#include <Expression.h>
#include "CPViewDescription.h"

BOREALIS_NAMESPACE_BEGIN

#define NOW -99
#define DEFAULT_INT -1

class CPView
{

  friend class CPQBox;

 public:

  enum view_type {AUTO, MANUAL, BOTH};

  //Constructors
  CPView(ptr<CPView> base_view, ptr<CPQBox> base_cp, string new_view_id);
  CPView(ptr<CPViewDescription> desc, ptr<CPQBox> base_cp, string new_view_id, TupleDescription tuple_desc, CPView::view_type v_type);

  //Destructor
  ~CPView();

  //Getter for unique id
  string get_id();

  //Getter for description
  ptr<CPViewDescription> get_description();



  //make a DELIMITER tuple with revision_id
  uint8  *make_delimiter_tuple(uint8  *last_tuple, int revision_id);
  uint8  *make_empty_delimiter(int revision_id, int end_order_by);

  //Enqueue historical tuples from the cp into the tuple queue
  void crop_tuples(vector<uint8*> all_tuples, uint8  *tuple, int revision_id);

  // is called by CP on all its views every time CP recieves new tuples to update
  //start/end pointers according to spec and current state of stream/CP
  void update_ptrs();

  // updates the start and end ptrs
  void update_auto_start_ptr();
  void update_auto_end_ptr();


  void set_tuple_queue(ptr<TupleQueue> tq);
  ptr<TupleQueue> get_tuple_queue() 
    { 
      return _tq; 
    }

  ptr<Expression> get_order_by_field()
  {
    return _order_by_field;
  }

  //Sets the radius
  void set_radius(int new_radius);

  //Returns the radius
  int get_radius();

  int get_auto_start_ptr() { return _auto_start_ptr; }
  int get_auto_end_ptr() { return _auto_end_ptr; }

  CPViewDescription::OrderByMethod get_order_by_method() { return _order_by_method; }

 private:

  // alter the tuple : change type to historical and change revision_id to given
  vector<uint8*> modify_multiple_tuples(vector<uint8*>  tuples,
                                        int             tuple_size,
                                        TupleType       type,
                                        int             revision_id,
                                        bool            revisions);

  char* _man_start_ptr;
  char* _man_end_ptr;

  // if ordered-by FIELD, then these are order-by values
  // if ordered-by TUPLENUM, then these are tuple-ids
  int _auto_start_ptr;  
  int _auto_end_ptr;    

  ptr<CPQBox>        _cp;
  ptr<TupleQueue>    _tq;
  string             _view_id;

  //Radius, which tells the CPView how many historical tuples to request from the CP
  int                _radius;

  view_type          _view_type;
  CPViewDescription  _view_desc;
  ptr<Expression>    _order_by_field;

  TupleDescription   _tuple_desc;   // tuple description of the tuple on cpview's stream

  CPViewDescription::CPViewType     _cp_view_type;
  CPViewDescription::OrderByMethod  _order_by_method;
};

BOREALIS_NAMESPACE_END

#endif                              // CPVIEW_H

#ifndef REVISION_AGG_QBOX_H
#define REVISION_AGG_QBOX_H

#include <ext/hash_map>
#include <vector>
#include <list>

#include "QBox.h"
#include "ExprUtil.h"
#include "Aggregate.h"
#include "Timestamp.h"
#include "SweeperQBox.h"
#include "TupleQueue.h"

#include <NMSTL/serial>
#include <NMSTL/iserial>
#include <NMSTL/oserial>

BOREALIS_NAMESPACE_BEGIN

/**
 * - GroupBy values shouldn't be slung around as a string maybe.
 */

using __gnu_cxx::hash_map;
using NMSTL::to_string;

class SweeperQBox;

class RevisionAggregateQBox : public QBox
{
  friend class AggregateBoxState;
  friend class Window;

 private:

  struct HashString
  {
    size_t operator()(const string s) const
    {
      // TODO: Find a better hash function or something.
      uint32  h = 0;
      for (string::const_iterator i = s.begin() ; i != s.end(); ++i)
    h = (5 * h) + *i;
      return size_t(h);
    }

    NMSTL_TO_STRING(HashString);
    NMSTL_SIMPLY_SERIALIZABLE(HashString, );
  };


  typedef vector<ptr<Aggregate> > AFVector;
  AFVector _afs;
  AFVector _afs2;

  // Size of the tuples
  size_t _in_tuple_size;
  size_t _out_tuple_size;

  // Size of the group_by values.
  size_t _group_by_value_size;

  // Magda added: initially false, becomes true if we receive a TENTATIVE tuple. 
  // This state variable is saved as part of checkpoints so it should get reset when we restart from a checkpoint
  bool _tentative; 

  // Window will contain the last window_size values of
  // window_field, advancing by advance_size values
  // when the window fills.
  enum window_method
    {
      WINDOW_BY_TUPLES,
      WINDOW_BY_VALUES
    };
  window_method _window_method;
  int _window_size;
  int _advance_size;
  // If WINDOW_BY_VALUES
  ptr<Expression> _window_field;
  // the name of the order-by field
  string _order_by_string;

  // Keep track of when the next window starts for each group by
  // value.
  // TODO: Consider tossing this in with the WindowList.
  struct WindowStart
  {
    bool initialized;
    int next;
    WindowStart() : initialized(false) { }

    NMSTL_TO_STRING(WindowStart);
    NMSTL_SIMPLY_SERIALIZABLE(WindowStart, << initialized << next);

  };
  hash_map<string, WindowStart, HashString> _window_starts;
  hash_map<string, WindowStart, HashString> _historical_starts;

  enum OrderByMethod
    {
      ORDER_BY_FIELD,
      ORDER_BY_TUPLENUM
    };
  OrderByMethod _order_by_method;

  EvalContext _ctxt;

  // Group by a set of attributes.
  vector<ptr<Expression> > _group_by_fields;
  // the names of the group-by fields
  vector<string> _group_by_strings;

  class Window;
  typedef list<ptr<Window> > WindowList;

  struct WindowState
  {
    public:
typedef enum { INSERTION = 0,
           HISTORICAL,
           REPLACEMENT,
           DELETION,
           OOINSERTION  } Type;

    private:
           Type _type;

    public:
           WindowState( Type t = INSERTION ) : _type(t) {}

    bool operator==(const WindowState &t) const
    {
      return( t._type == _type );
    }

    bool operator!=(const WindowState &t) const
    {
      return( t._type != _type );
    }

    string as_string() const
    {
      string strings[] = { "INSERTION",
               "HISTORICAL",
               "REPLACEMENT",
               "DELETION",
               "OOINSERTION"  };
      return( strings[_type] );
    }

    int as_int() const
    {
      return( _type == INSERTION   ? 0 :
          _type == HISTORICAL  ? 1 :
          _type == REPLACEMENT ? 2 :
          _type == DELETION    ? 3 :
          _type == OOINSERTION ? 4 : -1 );
    }

    Type type() {return _type;}
    void set_type( Type type ) {_type = type;}
  };

  // Abstract class for structure that stores a Window
  // Manages the state of the Aggregate Functions.
  class Window
    {
    public:
      Window(const AFVector &af,const AFVector &af2, const string &group_by,
         RevisionAggregateQBox *parent,
         TupleDescription &in_td, TupleDescription &out_td) :
    _old_windows(), _new_windows(), _tid_store(),
    _group_by(group_by), _initialized(false), _tid(0), _rev_id(-1),
    _parent(parent), _in_td(in_td), _out_td(out_td)
        {
      // Open up windows.
      AFVector::const_iterator i;
      for (i = af.begin(); i != af.end(); ++i)
            {
          ptr<AggregateWindow>w((*i)->open_window());
          ptr<Aggregate>agg_ptr(*i);
          //w->setAggregate(**i);
          w->set_aggregate(agg_ptr);
          w->init();
          _old_windows.push_back(w);
            }
      for (i = af2.begin(); i != af2.end(); ++i)
            {
          ptr<AggregateWindow>w((*i)->open_window());
          ptr<Aggregate>agg_ptr(*i);
          //w->setAggregate(**i);
          w->set_aggregate(agg_ptr);
          w->init();
          _new_windows.push_back(w);
            }
        };
      NMSTL_TO_STRING(WindowState);

      Window() {}
      virtual ~Window() {};

      // Return true if this window should delete now.
      virtual bool insert_tuple(/*const*/ EvalContext& ctxt);

      virtual const int get_start_value() const { return -999; };
      virtual const int get_last_value() const { return -999; };

      const Timestamp t0() const { return _t0; }
      const Timestamp lt0() const { return _lt0; }
      const int tid() const { return _tid; }
      const int rev_id() const { return _rev_id; }
      void set_rev_id( int r ) { _rev_id = r; }

      // Magda added: returns that max tuple_stime of all tuples in window
      const Timestamp max_stime() const { return _max_stime; }

      RevisionAggregateQBox *parent() const { return _parent; }

      void set_state( WindowState s ) { _state = s; }
      WindowState get_state() { return( _state ); }

      const string &group_by_value() const { return _group_by; }

      virtual string as_string() const { return NULL; };

      // Turns a INSERTIONS window into a uint8* tuple for emitting
      uint8* as_insertion_tuple() const;
      // Turns a window into a vector of tuples for emitting
      vector<uint8 *> as_tuples() const;

      void set_timeout_i(WindowList::iterator i) {_timeout_i = i; }
      void set_open_windows_i(WindowList::iterator i) {_ow_i = i; }
      WindowList::iterator get_timeout_i() { return _timeout_i; }
      WindowList::iterator get_open_windows_i() { return _ow_i; }

      // set _window_field for ValueWindow
      // here because of serialization problems with Expression
      virtual void set_window_field(ptr<Expression> window_field) {}

    private:
      vector<ptr<AggregateWindow> > _old_windows, _new_windows;
      set<int> _tid_store;
      string _group_by;
      bool _initialized;
      Timestamp _t0;
      Timestamp _lt0;
      int _tid;   // tid of the output from this window
      int _rev_id;   // the revision_id output from this window will have
      WindowList::iterator _timeout_i, _ow_i;
      RevisionAggregateQBox *_parent;
      WindowState _state;  // what will this window output - replacement, deletion, etc
      TupleDescription  _in_td;
      TupleDescription  _out_td;
      Timestamp _max_stime; // Magda added: holds the max_stime value of all tuples in the window

    public:
      NMSTL_TO_STRING(Window);
      NMSTL_SERIAL_BASE(Window, int32,
            << _old_windows  << _new_windows << _group_by
            << _initialized << _t0
            << _lt0 << _tid << _max_stime);
    };

  // Structure that stores a Window by values
  class ValueWindow : public Window
    {
    public:
      ValueWindow(const AFVector &af,const AFVector &af2, const string &group_by,
          ptr<Expression> window_field,
          int window_start, int window_end,
          int slack, RevisionAggregateQBox *parent,
          TupleDescription &in_td, TupleDescription &out_td) :
    Window(af, af2, group_by, parent, in_td, out_td),
    _window_field(window_field),
    _first_val(window_start), _last_val(window_end),
    _slack_remaining(slack)
        { }

      ValueWindow() {};

      virtual bool insert_tuple(/*const*/ EvalContext& ctxt);

      virtual const int get_start_value() const
        {
      return _first_val;
        }

      virtual const int get_last_value() const
        {
      return _last_val;
        }

      void set_window_field(ptr<Expression> window_field)
        {
      _window_field = ptr<Expression>(window_field);
        }

      virtual string as_string() const
        {
      return ("ValueWindow{[" + to_string(_first_val) + ", " +
          to_string(_last_val) + ") }");
        }

    private:
      ptr<Expression> _window_field;
      // Window runs from _first_val (inclusive) to _last_val (exclusive).
      int _first_val, _last_val;
      int _slack_remaining;

    public:
      NMSTL_TO_STRING(ValueWindow);
      NMSTL_SERIAL_SUBCLASS(ValueWindow, Window,
                << _first_val << _last_val << _slack_remaining);
    };

  // Structure that stores a Window by tuples
  class TupleWindow : public Window
    {
    public:
      TupleWindow(const AFVector &af,const AFVector &af2, const string group_by,
          int window_size, int start_value,
          RevisionAggregateQBox *parent,
          TupleDescription &in_td, TupleDescription &out_td) :
    Window(af,af2, group_by, parent, in_td, out_td),
    _window_size(window_size),
    _count(0), _start_value(start_value)
        { };

      TupleWindow() {};

      virtual bool insert_tuple(/*const*/ EvalContext& ctxt);

      virtual const int get_start_value() const { return _start_value; }

      int size() const { return _count; };

      virtual string as_string() const
        {
      return ("TupleWindow{[" + to_string(_count) + " out of "
          + to_string(_window_size) + " tuples] }");
        }

    private:
      int _window_size;
      int _count;
      int _start_value;
      vector<int> _ids_in_window;

    public:
      NMSTL_TO_STRING(TupleWindow);
      NMSTL_SERIAL_SUBCLASS(TupleWindow, Window,
                << _window_size << _count << _start_value);

    };

  // Map of group-by values to lists of open windows.
  typedef hash_map<string, WindowList, HashString> WindowHash;
  WindowHash _open_windows;
  WindowHash _open_historical_windows;

  // Slack stuff
  int _slack;

  // Timeout stuff.
  bool _timing_out;           // True if we are doing timeout.
  Timestamp _timeout;         // Timeout in seconds and useconds.
  WindowList _timeout_list;   // List of windows, in the order they
  // will be timed-out.

  // Record-keeping for emit_tuple()
  // so that delimiter will be the same as the last tuple emitted
  uint8 * _last_emitted_tuple; 

  // Private functions.
  // setup the parameters of this box from the xml
  void setup_parameters();
  
  // create the TupleDescription of the output stream of this box
  TupleDescription create_output_description();

  // parse group-by string from xml separated by ','
  void parse_group_by(const string atts, const ExprContext& ctxt);

  // returns the group-by values as concatenated as one string
  string group_by_for_tuple(const EvalContext& ctxt) const;

  // Insert a window to the given window list, which should be a
  // group-by window list. 
  void insert_new_window(WindowList &wl, ptr<Window> w);

  // takes a tuple and emits appropriate tuple
  void emit_tuple( const uint8 *data );

  // which output tids are affected by a revision in data
  vector<int> tids_affected( const uint8 *data );

  /****** Added on this box for revision processing*******/

  // revisions stored by box in upstream mode waiting for historical tuples
  list<const uint8 *> _revision_tuple_store;

  // revision stored by box in downstream mode after sending out requests
  // stored separately so that lookup can be done by _downstream_revision_number
  map<int, const uint8*> _downstream_revision_tuple_store;  
  
  // current revision number so that fulfilled requests can be separated by revisions 
  // that made the requests
  int _downstream_revision_number;

  // maps _downstream_revision_number to number of outstanding requests
  map<int, int> _outstanding_requests; 

  // the last order-by values for each group-by
  map<string, int32> _group_by_store;

  // last order-by emitted when not grouping
  // SIMPLICATION: combine with _group_by_store
  int32 _last_order_by;

  // the last window emitted???? 
  // CHECK TO MAKE SURE
  hash_map<string, ptr<Window>, HashString> _last_ins_window;
  hash_map<string, ptr<Window>, HashString> _last_rev_window;

  // the very first window value, since this value determines values for all windows
  int _first_window_start;

  // Data for mapping output groupby/orderby to output tid
  hash_map<string, int, HashString> _first_order_by;  // first order-by for each group
  map<string, int> _last_order_by_emitted;   // last order-by emitted for each group
  int _block_size;  // the number of tids assigned for each group 
  hash_map<string, vector<int>, HashString> _tid_block_allocation;  // the starting tids of different blocks for each group
  int _next_unallocated_tid_block;

    
  /********
       struct RevisionStates
       {
       bool initialized;
       int revision_id;
       bool revision_processed;
       WindowHash before_states;
       WindowHash after_states;
       hash_map<string, WindowStart, HashString> before_wstarts;
       hash_map<string, WindowStart, HashString> after_wstarts;
       hash_map<string, WindowStart, HashString> wstarts;

       RevisionStates() : initialized(false), revision_processed(false) {}

       string as_string() const
       {
       return string() + "RevisionStates{"
       + "revision-id=" + revision_id
       + "," + before_states.size() + " before_states" +
       + "," + after_states.size() + " after_states" +
       + "," + before_wstarts.size() + " before_wstarts" +
       + "," + after_wstarts.size() + " after_wstarts" +
       "}";
       }
       };

       map<int, RevisionStates> _revision_states;


       // process revision tuple
       // returns true when revision tuple is outputted
       bool process_revision(Tuple revision_tuple, Tuple historical_tuple, int revision_id);
       // returns a pair tid->concatenated group-by, order-by, agg values
       pair<int, string> produce_core_tuple(const ptr<Window> w);

       bool check_and_emit_revision(bool before_emitp, bool after_emitp, ptr<Window> before_w, ptr<Window> after_w);
       void emit_revision(const ptr<Window> w, TupleType revision_type);
       void emit_replacement_revision(const ptr<Window> before_w, const ptr<Window> w, TupleType revision_type);

  *************/

 protected:
  void setup_impl() throw (AuroraException);
  void init_impl() throw (AuroraException);
  void run_impl(QBoxInvocation&) throw (AuroraException);

  friend string to_string(const WindowHash& m);
};

inline string to_string(const RevisionAggregateQBox::WindowHash& m)
{
  string out = "WindowHash{";

  for (RevisionAggregateQBox::WindowHash::const_iterator i = m.begin();
       i != m.end(); ++i)
    {
      if (i != m.begin())
        {
      out.append("; ");
        }
      out.append(to_escaped_string(i->first)).append("->").append(
                                  to_string(i->second));
    }

  out.append("}");
  return out;
}


BOREALIS_NAMESPACE_END

#endif

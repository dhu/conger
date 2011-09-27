#ifndef SIMPLEAGG_QBOX_H
#define SIMPLEAGG_QBOX_H

#include <ext/hash_map>
#include <vector>
#include <list>

#include "QBox.h"
#include "ExprUtil.h"
#include "Aggregate.h"
#include "Timestamp.h"

#include <NMSTL/serial>
#include <NMSTL/iserial>
#include <NMSTL/oserial>

BOREALIS_NAMESPACE_BEGIN

/**
 * - GroupBy values shouldn't be slung around as a string maybe.
 */

using __gnu_cxx::hash_map;
using NMSTL::to_string;

class AggregateQBox : public QBox
{
  friend class AggregateBoxState;

  private:

    struct HashString
    {
        size_t operator()(const string s) const
        {
            // TODO: Find a better hash function or something.
            uint32  h = 0;
            for (string::const_iterator i = s.begin() ; i != s.end(); ++i)
            {
                h = (5 * h) + *i;
            }
            return size_t(h);
        }

        NMSTL_TO_STRING(HashString);
        NMSTL_SIMPLY_SERIALIZABLE(HashString, );
    };


    typedef vector<ptr<Aggregate> > AFVector;
    AFVector _afs;

    // Size of the input tuples
    size_t _in_tuple_size;

    // Size of the group_by values.
    size_t _group_by_value_size;

    bool _drop_mode;
    bool _last_agg;

    // Magda added: initially false, becomes true if we receive a TENTATIVE tuple. 
    // This state variable is saved as part of checkpoints so it should get reset when we restart from a checkpoint
    bool _tentative; 

    bool _drop_empty_outputs;
    bool _independent_alignment;

    // Window will contain the last window_size values of
    // window_field, advancing by advance_size values
    // when the window fills.
    enum window_method
    {
        WINDOW_BY_TUPLES,
        WINDOW_BY_VALUES
    };
    window_method _window_method;

    double _window_size;
    double _advance_size;

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
        double  next;
        double  keep_until;
        bool is_checkpointed;
        WindowStart() : initialized(false), is_checkpointed(false) { }
        
        void update(OSerial& o) 
        {
            o << next << keep_until;
        };
        
        void update(ISerial& i)
        {
            i >> next >> keep_until;
        };

        string as_string() const
        {
            return ("WindowStart{[" + to_string(initialized) + ", "
                    + to_string(next) + ", " +
                    to_string(keep_until) + ") }");
        }

        NMSTL_SIMPLY_SERIALIZABLE(WindowStart,
                                << initialized << next << keep_until);
    };
    NMSTL_TO_STRING(WindowStart);

    hash_map<string, WindowStart, HashString> _window_starts;

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

    // Abstract class for structure that stores a Window
    // Manages the state of the Aggregate Functions.
    class Window
    {
      public:
        Window(const AFVector &af, const string &group_by, int win_spec,
               bool fake_flag) :
            _windows(),
            _group_by(group_by), 
            _initialized(false),
            _is_empty(true),
            _win_spec(win_spec), 
            _fake_flag(fake_flag),
            _is_checkpointed(false)
        {
            // Open up windows.
            AFVector::const_iterator i;
            for (i = af.begin(); i != af.end(); ++i)
            {
                ptr<AggregateWindow>w((*i)->open_window());
                ptr<Aggregate> agg_ptr(*i);
                //w->setAggregate(**i);
                w->set_aggregate(agg_ptr);
                w->init();
                _windows.push_back(w);
            }
        };

        Window() :_is_empty(true) {}
        virtual ~Window() {};
        
        void set_aggregates(const AFVector &af);
        
        /**
         * Returns a string representation of this AggregateQBox::Window.
         */
        virtual string as_string() const;

        // Return true if this window should emit now.
        virtual bool insert_tuple(const EvalContext& ctxt);

        virtual const double get_start_value() const
        {
            return numeric_limits<double>::min();
        }

        int get_win_spec() { return _win_spec; }
        bool get_fake_flag() { return _fake_flag; }

        const Timestamp t0() const { return _t0; }
        const Timestamp lt0() const { return _lt0; }
        const Timestamp t_last() const { return _t_last; }
        const Timestamp lt_last() const { return _lt_last; }
        bool is_empty() const { return _is_empty; } // Magda added: keep track if window is empty

        const int tid() const { return _tid; }

        // Magda added: returns that max tuple_stime of all tuples in window
        const Timestamp max_stime() const { return _max_stime; }
        
        const string &group_by_value() const { return _group_by; }

        size_t final_value(char *pos) const;

        void set_timeout_i(WindowList::iterator i) {_timeout_i = i; }
        void set_open_windows_i(WindowList::iterator i) {_ow_i = i; }
        WindowList::iterator get_timeout_i() { return _timeout_i; }
        WindowList::iterator get_open_windows_i() { return _ow_i; }

        // set _window_field for ValueWindow
        // here because of serialization problems with Expression
        virtual void set_window_field(ptr<Expression> window_field) {}
        virtual void update(OSerial& o);
        virtual void update(ISerial& i);

      protected:
        vector<ptr<AggregateWindow> > _windows;
        string _group_by;
        bool _initialized;
        Timestamp _t0;
        Timestamp _lt0;
        Timestamp _t_last; // the most recent timestamp seen in the window
        Timestamp _lt_last;
        int _tid;
        Timestamp _max_stime; // Magda added: holds the max_stime value of all tuples in the window
        WindowList::iterator _timeout_i, _ow_i;
        bool _is_empty;

    protected:
        int      _win_spec;
        bool     _fake_flag;

    public:
        NMSTL_TO_STRING(Window);
        NMSTL_SERIAL_BASE(Window, int32,
              << _windows  << _group_by
              << _initialized << _t0
              << _lt0 << _t_last << _lt_last <<_tid << _max_stime
              << _win_spec << _fake_flag);
        bool _is_checkpointed;
    };

    // Structure that stores a Window by values
    class ValueWindow : public Window
    {
      public:
        ValueWindow(const AFVector &af, const string &group_by,
                    ptr<Expression> window_field,
                    double window_start, double window_end,
                    int slack, int win_spec, bool fake_flag) :
            Window(af, group_by, win_spec, fake_flag),
            _window_field(window_field),
            _first_val(window_start), _last_val(window_end),
            _slack_remaining(slack)
        {}

        ValueWindow() {};

        virtual bool insert_tuple(const EvalContext& ctxt);

        virtual const double get_start_value() const
        {
            return _first_val;
        }

        void set_window_field(ptr<Expression> window_field)
        {
            _window_field = ptr<Expression>(window_field);
        }

        virtual string as_string() const;

        virtual void update(OSerial& o);
        virtual void update(ISerial& i);

      private:
        ptr<Expression> _window_field;
        // Window runs from _first_val (inclusive) to _last_val (exclusive).
        double  _first_val, _last_val;
        int     _slack_remaining;

      public:
        NMSTL_TO_STRING(ValueWindow);
        NMSTL_SERIAL_SUBCLASS(ValueWindow, Window,
            << _first_val << _last_val << _slack_remaining);
    };

    // Structure that stores a Window by tuples
    class TupleWindow : public Window
    {
      public:
        TupleWindow(const AFVector &af, const string group_by,
            int window_size, int start_value, int win_spec, bool fake_flag) :
            Window(af, group_by, win_spec, fake_flag),
            _window_size(window_size),
            _count(0), _start_value(start_value) {};

        TupleWindow() {};

        virtual bool insert_tuple(const EvalContext& ctxt);
        virtual const double get_start_value() const
        {
            return static_cast<double>(_start_value);
        }

        int size() const { return _count; };

        virtual string as_string() const;

      private:
        int     _window_size;
        int     _count;
        int     _start_value;

    public:
        NMSTL_TO_STRING(TupleWindow);
        NMSTL_SERIAL_SUBCLASS(TupleWindow, Window,
            << _window_size << _count << _start_value);

    };

    // Slack stuff
    int _slack;

    // Timeout stuff.
    bool _timing_out;           // True if we are doing timeout.
    Timestamp _timeout;         // Timeout in seconds and useconds.
    WindowList _timeout_list;   // List of windows, in the order they
                                // will be timed-out.
                           
    ///// beginning of HA: Jeong-Hyon                                     
    // Map of group-by values to lists of open windows.
    typedef hash_map<string, WindowList, HashString> WindowHash;
    WindowHash _open_windows;
    
    // Map of group-by values to booleans: it keeps track of open windows that have changed after the last checkpoint.
    typedef hash_map<string, bool, HashString> DirtyWindowHash;
    DirtyWindowHash _dirty_windows;
    
    bool _is_updated;  // is updated recently?
    
    int _count_new_windows; // number of windows recently created.                
    
    int _count_updated_windows; // number of windows recently updated.                    
    
    ///// end of HA: Jeong-Hyon                                     

    // Private functions.
    void setup_parameters();
    TupleDescription create_output_description();
    void parse_group_by(const string atts, const ExprContext& ctxt);
    void emit_for_window(const ptr<Window> w);
    string group_by_for_tuple(const EvalContext& ctxt) const;
    void insert_new_window(WindowList &wl, ptr<Window> w);
    virtual Time get_expected_capture_cost();
    virtual Time get_expected_paste_cost();

  protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state);
//    void set_window_counts();

    friend string to_string(const WindowHash& m);

  public:

    int count_new_windows() { return _count_new_windows; };

    int count_updated_windows() { return _count_updated_windows; };

};

inline string to_string(const AggregateQBox::WindowHash& m)
{

    string out = "WindowHash{";

    for (AggregateQBox::WindowHash::const_iterator i = m.begin();
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


class AggregateBoxState : public AbstractBoxState
{
 public:
    // Keep track of when the next window starts for each group by value.
    typedef hash_map<string, AggregateQBox::WindowStart, AggregateQBox::HashString> WindowStarts;
    // List of windows, in the order they will be timed-out.
    typedef AggregateQBox::WindowList WindowList;
    // hash_map of group-by values to lists of open windows
    typedef AggregateQBox::WindowHash WindowHash;
    // hash_map of group-by values to booleans
    typedef AggregateQBox::DirtyWindowHash DirtyWindowHash;

    AggregateBoxState(AggregateQBox& aggregate, bool delta_only = false);

    AggregateBoxState() {}
    ~AggregateBoxState() {}

    string as_string() const
    {
        string out("AggregateQBoxState{");
        out += _serialized_state;
        out += "}";
        return out;
    }
    
    NMSTL_SERIAL_SUBCLASS(AggregateBoxState, AbstractBoxState, );
};
NMSTL_TO_STRING(AggregateBoxState);

BOREALIS_NAMESPACE_END

#endif

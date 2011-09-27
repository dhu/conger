
#ifndef WINDOW_DROP_QBOX_H
#define WINDOW_DROP_QBOX_H

#include <ext/hash_map>
#include <vector>
#include <list>

#include "QBox.h"
#include "Timestamp.h"

BOREALIS_NAMESPACE_BEGIN

using __gnu_cxx::hash_map;

// Assumption: no _slack and _timeout
//
class WindowDropQBox : public QBox
{
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
    };

    size_t _output_tuple_size;

    // Size of the group_by values.
    size_t _group_by_value_size;

    // Window will contain the last window_size values of
    // window_field, advancing by advance_size values
    // when the window fills.
    enum window_method
    {
        WINDOW_BY_TUPLES,
        WINDOW_BY_VALUES
    };
    window_method     _window_method;
    ptr<Expression>  _window_field;    // needed if WINDOW_BY_VALUES
    string           _window_field_name;

    double  _window_drop_rate; // for now, common drop_rate for all groups
    int     _max_batch_size;   // for now, common batch_size for all groups
    int     _window_size;
    int     _window_slide;     // same as _advance_size
    int     _window_extent;
    bool    _across_union;

    // per group parameters
    struct WindowInfo
    {
        bool    initialized;
        int     next_win_start;
        int     keep_until;
        int     batch_counter; // num of windows in the current batch
        bool    batch_drop;    // decision about the current batch
        bool    extra_needed;  // we need to keep one window
        bool    drop_extra;    // we can drop one to compensate
        int     extra_counter; // how many extra windows kept
        int     drop_counter;  // how many compensating drops in a row
        bool    zero_batch;    // ??
        bool    positive_batch;// ??
        bool    force_open;    // ??
        //double    window_drop_rate;
        //int       max_batch_size;
        WindowInfo()
        {
            initialized = false;
            next_win_start = -1;
            keep_until = 0;
            batch_counter = 0;
            batch_drop = true;
            extra_needed = false;
            drop_extra = false;
            extra_counter = 0;
            drop_counter = 0;
            zero_batch = false;     // ??
            positive_batch = false; // ??
            force_open = false;
            //window_drop_rate = 0;
            //max_batch_size = 0;
        }
    };
    hash_map<string, WindowInfo, HashString> _window_info;

    enum OrderByMethod
    {
        ORDER_BY_FIELD,
        ORDER_BY_TUPLENUM
    };
    OrderByMethod _order_by_method;

    EvalContext _ctxt;

    // Group by a set of attributes.
    vector<ptr<Expression> > _group_by_fields;

    // Private functions.
    void setup_parameters();
    void parse_group_by(const string atts, const ExprContext& ctxt);
    string group_by_for_tuple(const EvalContext& ctxt) const;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
    void update_params(); // -- tatbul
};

BOREALIS_NAMESPACE_END

#endif

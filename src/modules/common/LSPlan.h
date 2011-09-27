
#ifndef LS_PLAN_H
#define LS_PLAN_H

#include <map>
#include <vector>
#include "common.h"
#include "Catalog.h"

BOREALIS_NAMESPACE_BEGIN

class LSPlan
{
  public:

    LSPlan();

    ~LSPlan();

    // Drop box and its location.
    //
    typedef struct drop_box
    {
        Name            box_name;          // Drop box name.
        string          box_type;          // "window_drop" or "random_drop".
        string          node;              // Location of the drop box.
        BoxParameter    box_params;        // Parameters of the drop box.
        Name            in_stream_name;    // Existing stream into drop box.
        Name            dest_box_name;     // Box receiving drop box output.
        uint16          dest_box_port;     // Receiving port.
        string          dest_node;         // Location of the receiving box.

        drop_box() {}

        drop_box(const drop_box &d) 
        {
            box_name       = d.box_name;
            box_type       = d.box_type;
            node           = d.node;
            box_params     = d.box_params;
            in_stream_name = d.in_stream_name;
            dest_box_name  = d.dest_box_name;
            dest_box_port  = d.dest_box_port;
            dest_node      = d.dest_node;
        }

        string as_string() const
        {
            return(string("DropBox{") + box_name.as_string() + ", " +
                                        box_type + ", " +
                                        node + ", " +
                                        box_params.as_string() + ", " +
                                        in_stream_name.as_string() + ", " +
                                        dest_box_name.as_string() + ", " +
                                        dest_box_port + ", " +
                                        dest_node + "}");
        }

        NMSTL_SIMPLY_SERIALIZABLE(drop_box,
                              << box_name << box_type << node
                              << box_params << in_stream_name << dest_box_name
                              << dest_box_port << dest_node);

    } drop_box;

    LSPlan(vector<drop_box> drops) { _drop_insertion_plan = drops; }

    vector<drop_box>    _drop_insertion_plan;

    bool find(Name box_name);

    vector<drop_box>::iterator locate(Name box_name);

    void set_plan(vector<drop_box> drops);

    string as_string() const;

    string string_rep() const;

    NMSTL_SIMPLY_SERIALIZABLE(LSPlan, << _drop_insertion_plan);
};
NMSTL_TO_STRING(LSPlan);

typedef map<int, ptr<LSPlan> >  LSPlans;
typedef LSPlans::const_iterator lsplans_iter;

class LSPlanMessage
{
  public:

    LSPlanMessage() {}

    LSPlanMessage(const LSPlanMessage &lspm)
    {
        for (lsplans_iter iter = lspm._plans.begin();
             iter != lspm._plans.end(); iter++)
        {
            _plans[iter->first].set(
                            new LSPlan(iter->second->_drop_insertion_plan));
        }
    }

    ~LSPlanMessage() {}

    LSPlans _plans;

    string as_string() const;

    string string_rep() const;

    NMSTL_SIMPLY_SERIALIZABLE(LSPlanMessage, << _plans);

};
NMSTL_TO_STRING(LSPlanMessage);

BOREALIS_NAMESPACE_END

#endif

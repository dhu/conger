
#include "LSPlan.h"

BOREALIS_NAMESPACE_BEGIN

LSPlan::LSPlan()
{
    _drop_insertion_plan.clear();
}

LSPlan::~LSPlan()
{
    _drop_insertion_plan.clear();
}

bool LSPlan::find(Name box_name)
{
    for (unsigned int i = 0; i < _drop_insertion_plan.size(); i++)
    {
        if (_drop_insertion_plan[i].box_name == box_name)
        {
            return(true);
        }
    }

    return(false);
}

vector<LSPlan::drop_box>::iterator LSPlan::locate(Name box_name)
{
    vector<LSPlan::drop_box>::iterator iter;

    for (iter = _drop_insertion_plan.begin(); 
         iter != _drop_insertion_plan.end(); iter++)
    {
        if (iter->box_name == box_name)
        {
            return iter;
        }
    }

    return _drop_insertion_plan.end();
}

void LSPlan::set_plan(vector<drop_box> drops)
{
    for (unsigned int i = 0; i < drops.size(); i++)
    {
        drop_box d(drops[i]);
        _drop_insertion_plan.push_back(d);
    }
}

string LSPlan::as_string() const
{
    string s("\nLSPlan{\n");

    for (unsigned int i = 0; i < _drop_insertion_plan.size(); i++)
    {
        s += (_drop_insertion_plan[i].as_string());
        s += ("\n");
    }

    s += "}";

    return s;
}

string LSPlan::string_rep() const
{
    string s("    LSPlan{\n");

    for (unsigned int i = 0; i < _drop_insertion_plan.size(); i++)
    {
        s += "        ";
        s += (_drop_insertion_plan[i].as_string());
        s += ("\n");
    }

    s += "    }";

    return s;
}

string LSPlanMessage::as_string() const
{
    string s("\nLSPlanMessage{\n");

    for (lsplans_iter iter = _plans.begin(); iter != _plans.end(); iter++)
    {
        s += iter->first;
        s += " : ";
        s += ((iter->second)->as_string());
        s += "\n";
    }

    s += "}";

    return s;
}

string LSPlanMessage::string_rep() const
{
    string s("\nLSPlanMessage{\n");

    for (lsplans_iter iter = _plans.begin(); iter != _plans.end(); iter++)
    {
        s += ((iter->second)->string_rep());
        s += ("\n");
    }

    s += "}\n";

    return s;
}

BOREALIS_NAMESPACE_END

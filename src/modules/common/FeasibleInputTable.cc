
#include "FeasibleInputTable.h"

BOREALIS_NAMESPACE_BEGIN

FeasibleInputTable::FeasibleInputTable()
{
    _table.clear();
}

FeasibleInputTable::~FeasibleInputTable()
{
}

bool FeasibleInputTable::empty()
{
    return(_table.empty());
}

void FeasibleInputTable::clear()
{
    _table.clear();
}

string FeasibleInputTable::as_string() const
{
    string s("FeasibleInputTable{");

    for (unsigned int i = 0; i < _table.size(); i++)
    {
        s += (_table[i].as_string());
    }

    s += "}";

    return s;
}

string FeasibleInputTable::FIT_Entry::as_string() const
{
    string s("FIT_Entry{\n");

    s += "Rates{ ";
    for (RateMapIter iter = _rates.begin(); iter != _rates.end(); iter++)
    {
        s += iter->second;
        s += " ";
    }
    s += "}\n";

    s += "Score{ ";
    s += _score;
    s += " }\n";

    s += "Plan{";
    if (_additional_plan)
    {
        s += _additional_plan->as_string();
    }
    s += "\n}\n";

    s += "RemotePlans{\n";
    for (map<string, ptr<LSPlan> >::const_iterator iter = _remote_plans.begin();
         iter != _remote_plans.end(); iter++)
    {
        s += "Plan{ ";
        s += iter->first;
        s += ", ";
        s += iter->second->as_string();
        s += "\n}\n";
    }
    s += "}\n";
    s += "}\n";

    return s;
}

BOREALIS_NAMESPACE_END

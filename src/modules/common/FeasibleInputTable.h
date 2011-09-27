
#ifndef FEASIBLE_INPUT_TABLE_H
#define FEASIBLE_INPUT_TABLE_H

#include <map>
#include <vector>
#include "common.h"
#include "LSPlan.h"

BOREALIS_NAMESPACE_BEGIN

class FeasibleInputTable
{
  public:

    typedef map<string, double >    RateMap;
    typedef RateMap::const_iterator RateMapIter;

    typedef struct FIT_Entry
    {
        RateMap                       _rates;
        double                        _score;
        ptr<LSPlan>                   _additional_plan;
        map<string, ptr<LSPlan> >     _remote_plans; // for the global version
        
        FIT_Entry() {}

        FIT_Entry(RateMap rates, double score, ptr<LSPlan> plan)
        {
            _rates = rates;
            _score = score;
            _additional_plan = plan;
        }

        void add_remote_plan(string node, ptr<LSPlan> plan)
        {
            _remote_plans[node] = plan;
        }

        string as_string() const;

        NMSTL_SIMPLY_SERIALIZABLE(FIT_Entry,
                                  << _rates << _score << _additional_plan
                                  << _remote_plans);

    } FIT_Entry;

    FeasibleInputTable();

    FeasibleInputTable(vector<FIT_Entry> entries) { _table = entries; }

    FeasibleInputTable(int size) { _table = vector<FIT_Entry>(size); }

    ~FeasibleInputTable();

    bool empty();

    void clear();

    vector<FIT_Entry> _table;

    string as_string() const;

    NMSTL_SIMPLY_SERIALIZABLE(FeasibleInputTable, << _table);
};
NMSTL_TO_STRING(FeasibleInputTable);

BOREALIS_NAMESPACE_END

#endif

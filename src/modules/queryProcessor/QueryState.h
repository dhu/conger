#ifndef BOREALIS_QUERYSTATE_H
#define BOREALIS_QUERYSTATE_H

#include "common.h"
#include "Objects.h"
#include "StreamState.h"
#include "Stats.h"

BOREALIS_NAMESPACE_BEGIN

class QueryState
{
  public:
    Query _query;
    QueryStatus _status;
    vector<Name> _new_inputs; // Streams to which we need to subscribe (temporarily neede while query is setup)

    QueryState(Query query, QueryStatus status) : _query(query), _status(status) {}

    void clear()
    {
    }

    string repr() const { return as_string(); }
    string as_string() const
    {
        string out = string("QueryState{") + _query;
        // Perhaps other parameters here
        out += "}";
        return out;
    }

};

NMSTL_TO_STRING(QueryState);

BOREALIS_NAMESPACE_END

#endif

#ifndef MEDUSA_PENDINGQUERY_H
#define MEDUSA_PENDINGQUERY_H

#include "common.h"

MEDUSA_NAMESPACE_BEGIN

// --------------------------------------------------
class PendingQuery
{
  public:
    PendingQuery(ptr<Query> query) : _query(query) {}
    ~PendingQuery() {}

    ptr<Query> _query;

    // Completion for initial request
    AsyncRPC<void> completion;

};

// --------------------------------------------------
class PendingBatch
{

 public:
    PendingBatch(int nb_queries)
        : _nb_queries(nb_queries) {}
    ~PendingBatch() {}

    int _nb_queries;
    AsyncRPC<void> completion;

};

MEDUSA_NAMESPACE_END


#endif

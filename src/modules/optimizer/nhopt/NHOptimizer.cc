#include "NHOptimizer.h"

MEDUSA_NAMESPACE_BEGIN

AsyncRPC< ptr<LoadOffer> > NHOptimizer::handle_load_offer(ptr<LoadOffer> offer)
{
    WARN << "Received load offer " << *offer;
    AsyncRPC< ptr<LoadOffer> > completion;
    handle_load_offer(completion,offer);
    return completion;
}

// --------------------------------------------------
RPC< vector<Time> > NHOptimizer::get_load_moves()
{
    return _load_moves;
}


MEDUSA_NAMESPACE_END


//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_NHOptimizer.h"
#undef NMSTL_RPC_DEFINE
//---

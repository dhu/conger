
#ifndef MEDUSA_NHOPTIMIZER_H
#define MEDUSA_NHOPTIMIZER_H

#include "common.h"
#include "BasicComponent.h"
#include "LoadOffer.h"

MEDUSA_NAMESPACE_BEGIN


/**
 * Specification for the neighborhood optimizer
 * Only general API
 */
class NHOptimizer : public BasicComponent
{

 public:

    NHOptimizer(string id)
        : BasicComponent(id, "NHOptimizer")
    { }

    ~NHOptimizer() {}

    // RPC: #include "LoadOffer.h"

    // Load management API
    AsyncRPC< ptr<LoadOffer> > handle_load_offer(ptr<LoadOffer> offer);

    RPC< vector<Time> > get_load_moves();


    NMSTL_RPC_OBJECT(NHOptimizer);

 protected:
    string _config_file_name;
    virtual void handle_load_offer(AsyncRPC< ptr<LoadOffer> > completion, ptr<LoadOffer> offer)
    {
        completion.post(RPCFault(string("Unsupported operation")));
    }

    /// Times when load movements occured
    vector<Time> _load_moves;
};


MEDUSA_NAMESPACE_END

#endif

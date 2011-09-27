
#ifndef LOCAL_LOAD_SHEDDER_H
#define LOCAL_LOAD_SHEDDER_H

#include "common.h"
#include "BasicComponent.h"
#include "LSPlan.h"

// plan_id = 0 represents a plan with no drops
//
#define DEFAULT_PLAN_ID    0

BOREALIS_NAMESPACE_BEGIN

class LocalLoadShedder : public BasicComponent
{
  public:

    LocalLoadShedder(string id);

    ~LocalLoadShedder() {}

    void do_something();

    // RPC: #include "LSPlan.h"

    AsyncRPC<void> send_lsplans(LSPlanMessage plans); // static

    RPC<void> send_plan_id(int plan_id);         // dynamic
    RPC<void> send_additional_plan(LSPlan plan); // dynamic

    NMSTL_RPC_OBJECT(LocalLoadShedder);

  private:

    int         _previous_plan_id;
    int         _current_plan_id;

    LSPlans     _ls_roadmap;

    map<string, double > _additional_drops;

    void in_thread_init();

    void local_apply_lsplan(int plan_id);
    void local_apply_additional_lsplan(LSPlan plan);

    void apply_lsplan();

};

BOREALIS_NAMESPACE_END

#endif

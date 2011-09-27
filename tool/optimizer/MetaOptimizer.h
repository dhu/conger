
#ifndef META_OPTIMIZER_H
#define META_OPTIMIZER_H

#include "common.h"
#include "OptimizerCommon.h"
#include "RegionServer.h"
#include "WriteRegion.h"
#include "rpc_QueryProcessor.h" // for getting RPC<Statistics>
#include "LoadShedder.h"
#include "BorealisLoadManager.h"
#include "StatisticsServer.h"   // for sending stats to visualizer //olga
#include "time.h"

BOREALIS_NAMESPACE_BEGIN

using namespace std;

typedef enum
{
    NO_ACTION,
    LOAD_SHEDDING,
    LOAD_BALANCING,
    HA
} OptimizationAction;

class MetaOptimizer
{
  public:

    MetaOptimizer(int period, 
                  int region_server_port,
                  double load_shedding_threshold,
                  double warmup_delay,
                  OptimizationAction debugging_action,
                  int load_shedding_quality_metric,
                  int runtime);

    ~MetaOptimizer();

    void terminate();

    void optimize();

    int  get_update_period();
    void set_update_period(int period);

    ptr<RegionServer> get_region_server() const;

    bool start_region_server();
    bool start_optimizer();

    void add_ignore_node_in_load_distribution(const string& node_name);
    void add_ignore_node_in_load_shedder(const string& node_name);

    string as_string() const;

    set<string>                 _ignore_nodes;

    IOEventLoop                 *_loop;

  private:

    /*
     * If this value is not NO_ACTION, then only the specified method
     * will be called for debugging that method only
     */
    OptimizationAction          _debugging_action;
    OptimizationAction          _current_action;

    int                         _update_period;  // unit: milliseconds

    pthread_t                   _thread;
    bool                        _running;
    unsigned int                _time_stamp;
    double                      _load_shedding_threshold;
    int                         _load_shedding_quality_metric; // 0: thruput
                                                               // 1: tuple_del
                                                               // 2: value
    ptr<RegionServer>           _region_server;
    map<string, Statistics>     _region_statistics;
    RegionalCatalog             _catalog;

    ptr<LoadShedder>            _ls;
    ptr<BorealisLoadManager>    _lm;

    int                         _ls_level; // optional load level from the gui
    bool                        _ls_done;
    bool                        _load_shedder_running;
    double                      _warmup_delay;

    int                         _run_time; // when to terminate, in msecs

    map<string, map<string, double> >  _tuple_delivery;

    OptimizationAction evaluate_load();

    void periodical_run();

    void copy_catalog_optimize(unsigned int time_stamp);

    static void run(IOEventLoop* loop)
    {
        loop->run();
    }

    /*
     * Collect Statistics
     * TODO: needs to be changed if statistics collection rpc call is
     * changed to asynchronous.
     */
    void update_statistics();

};

NMSTL_TO_STRING(MetaOptimizer);

BOREALIS_NAMESPACE_END

#endif


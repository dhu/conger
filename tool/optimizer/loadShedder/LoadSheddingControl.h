
#ifndef LOAD_SHEDDING_CONTROL_H
#define LOAD_SHEDDING_CONTROL_H

#include "common.h"
#include "RegionServer.h"
#include "rpc_QueryProcessor.h" // for getting RPC<Statistics>
#include "LoadShedder.h"
#include "time.h"

BOREALIS_NAMESPACE_BEGIN

class LoadSheddingControl
{
  public:

    LoadSheddingControl(int region_server_port,
                        int ls_period,
                        int ls_quality_metric,
                        int ls_mode,
                        int warmup_time,
                        int offline_duration,
                        int percent_fit_error,
                        int run_time);

    ~LoadSheddingControl();

    bool start_region_server();
    bool start_load_shedder();

    string as_string() const;

  private:

    IOEventLoop             *_loop;

    int                     _period;
    int                     _quality_metric;
    int                     _mode;
    int                     _warmup_time;
    int                     _offline_duration;
    int                     _percent_fit_error;
    int                     _run_time;

    bool                    _dropping;

    pthread_t               _thread;

    ptr<RegionServer>       _region_server;
    map<string, Statistics> _statistics;
    RegionalCatalog         _catalog;
    ptr<LoadShedder>        _ls;

    map<string, map<string, double> > _tuple_delivery;

    void read_regional_catalog();
    void read_regional_statistics();

    static void run(IOEventLoop* loop)
    {
        loop->run();
    }

    void setup_PLAN();
    void setup_FIT();

    void terminate();

    //void shed_load_FIT();     // from FIT
    void shed_load_static();  // from precomputed plans
    void shed_load_dynamic(); // from on the fly plan
};

NMSTL_TO_STRING(LoadSheddingControl);

BOREALIS_NAMESPACE_END

#endif

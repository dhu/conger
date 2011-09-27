#ifndef BorealisController_H
#define BorealisController_H

#include <common.h>
#include <RegionServer.h>
#include <rpc_QueryProcessor.h> // for RPC<Statistics>
#include "BorealisLoadManager.h"


BOREALIS_NAMESPACE_BEGIN

class BorealisController
{
public:

    BorealisController(
        double period,
        const string& region_server_address);

    ~BorealisController();

    void start_input_reading() { _lm->start_input_reading(); }
    bool start_region_server();
    bool start_load_manager();

    IOEventLoop*    get_loop() const;
    ptr<RegionServer>   get_region_server() const;

private:

    double                 _update_period;  // unit: millisecond

    IOEventLoop*    _loop;

    ptr<RegionServer>           _region_server;

    map<string, Statistics>     _region_statistics;

    ptr<BorealisLoadManager>    _lm;

    void periodical_run();

    void copy_catalog(unsigned int time_stamp);

    void optimize();

    RegionalCatalog _catalog;

    pthread_t         _thread;

    bool _running;

    static void run(IOEventLoop* loop) {loop->run();}

    unsigned int _time_stamp;
    double _optimization_time;

    /*
     * Collect Statistics
     * TODO need to be changed if statistics collection rpc call is
     * changed to asynchronous
     */
    void update_statistics();
};

NMSTL_TO_STRING(BorealisController);

BOREALIS_NAMESPACE_END

#endif

#include "BorealisController.h"
#include "MedusaClient.h"
#include "LMParams.h"
#include "Timeval.h"

BOREALIS_NAMESPACE_BEGIN

using namespace time_tools;

BorealisController::BorealisController(
        double period,
        const string& region_server_address): _running(false)
{
    _update_period = period;

    InetAddress address(region_server_address);
    long port = address.get_port();
    _region_server.set(new RegionServer(port));

    _lm.set(new BorealisLoadManager(
                _update_period,
                _region_server->get_regional_catalog(),
                _region_statistics));

    _loop = new IOEventLoop();
}

BorealisController::~BorealisController()
{
    delete _loop;
}

bool BorealisController::start_region_server()
{
    NOTICE << "Starting region server ...";
    _region_server->start();
    return true;
}

bool BorealisController::start_load_manager()
{
    if (_running)
    {
        NOTICE << "LoadManager has already started.";
        return true;
    }


    NOTICE << "Starting the Load Manager ...";

    if (pthread_create(&_thread,
                                      0,
                                      (void*(*)(void*))run,
                                      (void*)_loop) < 0)
    {
        WARN << "Can not start load manager thread.";
        return false;
    }

    _optimization_time =
        static_cast<int>(Timeval::now().double_value()) + 0.5 +
        LMParams::time_param.load_distribution_starting_delay;

    (new CallbackTimer(*_loop,
                                          wrap(this, &BorealisController::periodical_run)))
                        ->arm(Time::secs(_optimization_time));

    NOTICE << "Load Manager stated." ;

    _running = true;

    return _running;
}


IOEventLoop* BorealisController::get_loop() const
{
    return _loop;
}

ptr<RegionServer> BorealisController::get_region_server() const
{
    return _region_server;
}

void BorealisController::periodical_run()
{
    ++_time_stamp;

    _optimization_time += _update_period;

    DEBUG << "Starting optimzation process ... ";
    (new CallbackTimer(_region_server->get_ioevent_loop(),
                                          wrap(this, &BorealisController::copy_catalog, _time_stamp)))
                                                                                ->arm(Time::now());

    (new CallbackTimer(*_loop, wrap(this, &BorealisController::periodical_run)))
                                                                                ->arm(Time::secs(_optimization_time));
}

void BorealisController::copy_catalog(unsigned int time_stamp)
{
    if (time_stamp != _time_stamp)
    {
        DEBUG << "Catalog reading timed out.";
        return;
    }

    DEBUG << "Copying Catalog ... ";
    _catalog = _region_server->get_regional_catalog();
    DEBUG << "Catalog copied: \n" << _catalog.as_string();
    (new CallbackTimer(*_loop, wrap(this, &BorealisController::optimize)))
                                                                                ->arm(Time::now());
}


void BorealisController::optimize()
{

    update_statistics();

    _lm->update();
    _lm->redistribute_load();
}


void BorealisController::update_statistics()
{
    DEBUG << "Updating statistics.";
    // clear old statistics
    _region_statistics.clear();

    // collect new statistics
    set<string>& node_addresses =
        *(_region_server->get_regional_catalog().get_region_node());

    for (set<string>::const_iterator it = node_addresses.begin();
            it != node_addresses.end();
            ++it)
    {

            const string& node_address = *it;

            ptr<MedusaClient> client =
                                                    (ptr<MedusaClient>) new MedusaClient(node_address);

            Remote<QueryProcessor> query_processor =
                                                    client->qp_for(MedusaID(node_address));

            DEBUG << "Request statistics from node " << node_address;
            DEBUG << "Statistics collection from node " << node_address;

    _region_statistics[node_address] =  _region_server->_region_statistics[node_address]=*query_processor.get_statistics();

    _region_server->_region_cpu_statistics[node_address]=
                                                            *query_processor.get_cpu_statistics(); //olga

    }
    DEBUG << "Statistics collection finished.";
}



BOREALIS_NAMESPACE_END



#include "LoadSheddingControl.h"
#include "MedusaClient.h"
#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>

BOREALIS_NAMESPACE_BEGIN

LoadSheddingControl::LoadSheddingControl(int region_server_port, 
                                         int ls_period, 
                                         int ls_quality_metric, 
                                         int ls_mode,
                                         int warmup_time, 
                                         int offline_duration, 
                                         int percent_fit_error, 
                                         int run_time)
{
    _region_server.set(new RegionServer(region_server_port));

    _period = ls_period;

    _quality_metric = ls_quality_metric;

    _mode = ls_mode;

    _warmup_time = warmup_time;

    _offline_duration = offline_duration;

    _percent_fit_error = percent_fit_error;

    _run_time = run_time;

    _loop = new IOEventLoop();

    _ls.set(new LoadShedder(_catalog, 
                            _statistics, 
                            _loop, 
                            _quality_metric,
                            _offline_duration,
                            _percent_fit_error));
    _tuple_delivery.clear();

    _dropping = false;
}

LoadSheddingControl::~LoadSheddingControl()
{
    delete _loop;
}

void LoadSheddingControl::terminate()
{
    _region_server->terminate();
    _loop->terminate();
}

bool LoadSheddingControl::start_region_server()
{
    NOTICE << "Starting the RegionServer...";

    _region_server->start();

    return true;
}

bool LoadSheddingControl::start_load_shedder()
{
    NOTICE << "Starting the LoadSheddingControl...";

    if (pthread_create(&_thread,
                       0,
                       (void*(*)(void*))run,
                       (void*)_loop) < 0)
    {
        WARN << "Can not start the LoadSheddingControl thread.";
        return false;
    }

    NOTICE << "LoadSheddingControl thread has started.";

    // Setup the timers.
    //
    if (_mode == 0)
    {
        (new CallbackTimer(*_loop, 
                           wrap(this, &LoadSheddingControl::setup_PLAN)))
             ->arm(Time::now() + Time::msecs(_warmup_time));
    }
    else if (_mode == 1)
    {
        (new CallbackTimer(*_loop, 
                           wrap(this, &LoadSheddingControl::setup_FIT)))
             ->arm(Time::now() + Time::msecs(_warmup_time));
    }

    if (_run_time != INIT)
    {
        (new CallbackTimer(*_loop, wrap(this, &LoadSheddingControl::terminate)))
             ->arm(Time::now() + Time::msecs(_run_time));
    }

    return true;
}

void LoadSheddingControl::setup_FIT()
{
    NOTICE << "LoadSheddingControl setting up FIT...";

    read_regional_catalog();
    read_regional_statistics();

    Time start_time, stop_time;
    double time_passed1, time_passed2, time_passed3;

    // generate
    //
    start_time = Time::now();

    _ls->prepare_node_metadata();

    stop_time = Time::now();

    time_passed1 = static_cast<double>((stop_time - start_time).to_msecs());
    NOTICE << "*** Metadata precomputation took = " 
           << time_passed1 << " milliseconds.";

    start_time = Time::now();

    _ls->precompute_FIT_plans();

    stop_time = Time::now();

    time_passed2 = static_cast<double>((stop_time - start_time).to_msecs());
    NOTICE << "*** FIT-based plan precomputation took = " 
           << time_passed2 << " milliseconds.";

    fstream f;
    f.open("/tmp/offline", ios::out);
    f << time_passed2;
    f.close();

    _ls->print_plans();

    // upload
    //
    start_time = Time::now();

    _ls->upload_plans();

    stop_time = Time::now();

    time_passed3 = static_cast<double>((stop_time - start_time).to_msecs());
    NOTICE << "*** Plan upload took = " << time_passed3 << " milliseconds.";

    shed_load_static();
}

void LoadSheddingControl::setup_PLAN()
{
    NOTICE << "LoadSheddingControl setting up PLAN...";

    read_regional_catalog();
    read_regional_statistics();

    Time start_time, stop_time;
    double time_passed1, time_passed2, time_passed3;

    // generate
    //
    start_time = Time::now();

    _ls->prepare_stable_metadata();

    stop_time = Time::now();

    time_passed1 = static_cast<double>((stop_time - start_time).to_msecs());
    NOTICE << "*** Metadata precomputation took = "
           << time_passed1 << " milliseconds.";

    start_time = Time::now();

    _ls->precompute_plans();
    return;

    stop_time = Time::now();

    time_passed2 = static_cast<double>((stop_time - start_time).to_msecs());
    NOTICE << "*** Solver-based plan precomputation took = " 
           << time_passed2 << " milliseconds.";

    fstream f;
    f.open("/tmp/offline", ios::out);
    f << time_passed2;
    f.close();

    _ls->print_plans();

    // upload
    //
    start_time = Time::now();

    _ls->upload_plans(); 

    stop_time = Time::now();

    time_passed3 = static_cast<double>((stop_time - start_time).to_msecs());
    NOTICE << "*** Plan upload took = " << time_passed3 << " milliseconds.";

    shed_load_static(); 
}

/*
void LoadSheddingControl::shed_load_FIT()
{
    DEBUG << "LoadSheddingControl in shed_load_FIT...";

    read_regional_statistics();
    _ls->refresh_rates();

    if (!(_ls->compute_node_loads_FIT()))
    {
        NOTICE << "No server is overloaded.";
        _ls->shed_load(0);
    }
    else
    {
        NOTICE << "Some servers are overloaded.";
        int plan_id = _ls->select_plan();
        _ls->shed_load(plan_id);
    }

    // Periodically check.
    //
    (new CallbackTimer(*_loop, 
                       wrap(this, &LoadSheddingControl::shed_load_FIT)))
         ->arm(Time::now() + Time::msecs(_period));
}
*/

void LoadSheddingControl::shed_load_static()
{
    DEBUG << "LoadSheddingControl in shed_load_static...";

    read_regional_statistics();
    _ls->refresh_rates();

    if (!(_ls->compute_node_loads_original()))
    {
        NOTICE << "No server is overloaded.";
        _ls->shed_load(0);
    }
    else
    {
        NOTICE << "Some servers are overloaded.";
        int plan_id = _ls->select_plan();
        _ls->shed_load(plan_id);
    }

    // Periodically check.
    //
    (new CallbackTimer(*_loop, 
                       wrap(this, &LoadSheddingControl::shed_load_static)))
         ->arm(Time::now() + Time::msecs(_period));
}

void LoadSheddingControl::shed_load_dynamic()
{
    DEBUG << "LoadSheddingControl in shed_load_dynamic...";

    read_regional_catalog();
    read_regional_statistics();
    _ls->refresh_rates();

    if (!(_ls->compute_node_loads_original()))
    {
        NOTICE << "No server is overloaded.";
        if (_dropping)
        {
            _ls->shed_load(0);    
            _dropping = false;
        }
    }
    else
    {
        NOTICE << "Some servers are overloaded.";
        _dropping = _ls->generate_new_plan();
    }

    // Periodically check.
    //
    (new CallbackTimer(*_loop, 
                       wrap(this, &LoadSheddingControl::shed_load_dynamic)))
         ->arm(Time::now() + Time::msecs(_period));
}

void LoadSheddingControl::read_regional_catalog()
{
    _catalog = _region_server->get_regional_catalog();

    NOTICE << "Regional Catalog: \n" << _catalog.as_string();
}

void LoadSheddingControl::read_regional_statistics()
{
    _statistics.clear();

    set<string>& node_addresses =
            *(_region_server->get_regional_catalog().get_region_node());

    for (set<string>::const_iterator it = node_addresses.begin();
         it != node_addresses.end(); ++it)
    {
        const string& node_address = *it;

        ptr<MedusaClient> client = (ptr<MedusaClient>)
                                   new MedusaClient(node_address);
        Remote<QueryProcessor> query_processor = client->qp_for(
                                                 MedusaID(node_address));
        if (!query_processor)
        {
            WARN << "No QueryProcessor for " << node_address;
        }

        _statistics[node_address]
                            = _region_server->_region_statistics[node_address]
                            = *query_processor.get_statistics();

        NOTICE << "Stats for node " << node_address << " : "
               << _statistics[node_address].as_string();
    }
}

string LoadSheddingControl::as_string() const
{
    return string("LoadSheddingControl");
}

BOREALIS_NAMESPACE_END


#include "MetaOptimizer.h"
#include "args.h"
#include "rpc_QueryProcessor.h" // for RPC<Statistics>
#include "MedusaClient.h"
#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>
#include "StatisticsServer.h"   // for sending statistics to visualizer

BOREALIS_NAMESPACE_BEGIN

MetaOptimizer::MetaOptimizer(
    int period,
    int port,
    double load_shedding_threshold,
    double warmup_delay,
    OptimizationAction debugging_action,
    int load_shedding_quality_metric, 
    int runtime):
    _time_stamp(0),
    _load_shedding_threshold(load_shedding_threshold),
    _ls_done(true),
    _load_shedder_running(false),
    _warmup_delay(warmup_delay)
{
    _update_period    = period;
    _current_action   = NO_ACTION;
    _debugging_action = debugging_action;

    _region_server.set(new RegionServer(port));

    _loop = new IOEventLoop();

    _load_shedding_quality_metric = load_shedding_quality_metric;

    _run_time = runtime;

    _ls.set(new LoadShedder(
                    _catalog,
                    _region_statistics,
                    _loop,
                    _load_shedding_quality_metric));
    _tuple_delivery.clear();

    _lm.set(new BorealisLoadManager(
                    (double)_update_period/1000,
                    _catalog,
                    _region_statistics));

    fstream fs;
    fs.open("/tmp/MetaOptimizerInstruction", ios::out);
    if (!fs)
    {
        fs << "None\n";
        fs.close();
    }

    _ls_level = 0; // default

    _ignore_nodes.clear();
}

MetaOptimizer::~MetaOptimizer()
{
    delete _loop;
}

void MetaOptimizer::terminate()
{
    _region_server->terminate();

    _loop->terminate();
}

int MetaOptimizer::get_update_period()
{
    return _update_period;
}

void MetaOptimizer::set_update_period(int period)
{
    _update_period = period;
}

ptr<RegionServer> MetaOptimizer::get_region_server() const
{
    return _region_server;
}

bool MetaOptimizer::start_region_server()
{
    NOTICE << "Starting RegionServer...";

    _region_server->start();

    return true;
}

bool MetaOptimizer::start_optimizer()
{
    if (_running)
    {
        NOTICE << "MetaOptimizer has already started.";
        return true;
    }

    NOTICE << "Starting the MetaOptimizer...";

    if (pthread_create(&_thread,
                       0,
                       (void*(*)(void*))run,
                       (void*)_loop) < 0)
    {
        WARN << "Can not start the optimizer thread.";
        return false;
    }

    Time starting_time = Time::secs(Time::now().to_secs())  +
                         Time::msecs(500) + Time::secs(_warmup_delay);

    (new CallbackTimer(*_loop, wrap(this, &MetaOptimizer::periodical_run)))
         ->arm(starting_time);

    if (_run_time != INIT)
    {
        (new CallbackTimer(*_loop, wrap(this, &MetaOptimizer::terminate)))
             ->arm(Time::now() + Time::msecs(_run_time));
    }

    NOTICE << "Optimizer started.";

    _running = true;

    return _running;
}

void MetaOptimizer::periodical_run()
{
    ++_time_stamp;
    Time now = Time::now();
    Time next_time_optimizer_runs = now + Time::msecs(_update_period);

    NOTICE << "Starting the optimization process... ";

    (new CallbackTimer(_region_server->get_ioevent_loop(),
                       wrap(this, &MetaOptimizer::copy_catalog_optimize, 
                            _time_stamp)))->arm(Time::now());

    (new CallbackTimer(*_loop, wrap(this, &MetaOptimizer::periodical_run)))
                                          ->arm(next_time_optimizer_runs);
}

void MetaOptimizer::copy_catalog_optimize(unsigned int time_stamp)
{
    if (time_stamp != _time_stamp)
    {
        WARN << "Catalog reading timed out.";
        return;
    }

    NOTICE << "Copying the RegionalCatalog... ";

    _catalog = _region_server->get_regional_catalog();

    WriteRegion writer;
    string file_name = string("/tmp/catalog") + _time_stamp + ".xml";
    writer.write_region(_catalog, file_name);

    NOTICE << "Catalog copied: \n" << _catalog.as_string();

    (new CallbackTimer(*_loop, wrap(this, &MetaOptimizer::optimize)))
                                                  ->arm(Time::now());
}

void MetaOptimizer::optimize()
{
    NOTICE << "Running the MetaOptimizer...";

    update_statistics();

    if (!_ls_done)
    {
        NOTICE << "MetaOptimizer will wait LoadShedder to finish...";

        _tuple_delivery = _ls->get_current_tuple_delivery();
        if (!_tuple_delivery.empty())
        {
            _region_server->set_tuple_delivery(_tuple_delivery);
        }

        return;
    }

    if (_debugging_action == NO_ACTION)
    {
        // Evaluate load and take action accordingly.
        //
        _current_action = evaluate_load();

        switch (_current_action)
        {
            case NO_ACTION:
                NOTICE << "No Action...";
                /*
                _ls->run(false);
                _tuple_delivery = _ls->getCurrentTupleDelivery();
                if (!_tuple_delivery.empty())
                {
                    _region_server->setTupleDelivery(_tuple_delivery);
                }
                */
                break;
            case LOAD_SHEDDING:
                NOTICE << "Load Shedder should run...";
                _ls->run(true, &_ls_done, _ls_level);
                /*
                _tuple_delivery = _ls->getCurrentTupleDelivery();
                if (!_tuple_delivery.empty())
                {
                    _region_server->setTupleDelivery(_tuple_delivery);
                }
                */
                break;
            case LOAD_BALANCING:
                NOTICE << "Load Manager should run...";
                _lm->redistribute_load();
                break;
            case HA:
                NOTICE << "HA should run...";
                break;
            default:
                WARN << (int)_current_action
                     << ": Invalid optimization action!";
                break;
        }
    }
    else
    {
        // We are in debugging mode; no need to evaluate load.
        //
        _current_action = _debugging_action;

        _ls_level = 0;

        if (!_ls_done)
        {
            NOTICE << "MetaOptimizer will wait LoadShedder to finish...";
            _tuple_delivery = _ls->get_current_tuple_delivery();
            if (!_tuple_delivery.empty())
            {
                _region_server->set_tuple_delivery(_tuple_delivery);
            }
            return;
        }

        switch (_debugging_action)
        {
            case LOAD_SHEDDING:
                /*
                if (_load_shedder_running)
                {
                    DEBUG << "LoadShedder is already running, stop that first";
                    _load_shedder_running = false;
                    _ls->run(false);
                }
                else
                {
                    DEBUG << "LoadShedder is gonna run";
                    _load_shedder_running = true;
                    _ls->run(true);
                }
                */
                _ls->run(true, &_ls_done, _ls_level);
                /*
                _tuple_delivery = _ls->getCurrentTupleDelivery();
                if (!_tuple_delivery.empty())
                {
                    _region_server->setTupleDelivery(_tuple_delivery);
                }
                */
                break;
            case LOAD_BALANCING:
                _lm->update();
                _lm->redistribute_load();
                break;
            case HA:
                NOTICE << "HA should run...";
                break;
            default:
                WARN << (int)_debugging_action << ": Invalid debugging action!";
                break;
        }
    }

    // Set tuple delivery for output streams
    //
    // First try refreshing it from the load shedder.
    // If not available, then refresh it from the catalog.
    //
    _tuple_delivery.clear();

    _tuple_delivery = _ls->get_current_tuple_delivery();
    if (!_tuple_delivery.empty())
    {
        _region_server->set_tuple_delivery(_tuple_delivery);
    }
    else
    {
        Catalog::StreamMap *streams = _catalog.get_stream_map();
        for (Catalog::StreamMap::iterator iter = streams->begin();
             iter != streams->end(); ++iter)
        {
            CatalogStream& catalog_stream = iter->second;
            if (catalog_stream.is_output_stream())
            {
                CatalogBox *box = catalog_stream.get_stream_source_box();

                string out_node;

                if (box)
                {
                    out_node = box->get_box_node();
                    if (_ignore_nodes.find(out_node) != _ignore_nodes.end())
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }

                _tuple_delivery[out_node][(iter->first).as_string()] = 100.0;
            }
        }
        _region_server->set_tuple_delivery(_tuple_delivery);
    }
}

OptimizationAction MetaOptimizer::evaluate_load()
{
    fstream fs;
    fs.open("/tmp/MetaOptimizerInstruction", ios::in);
    if (fs)
    {
        string method;
        fs >> method;
        fs.close();
        if (method == "HA")
        {
            _ls_level = 0;
            return HA;
        }
        else
        {
            _lm->update();
            if (method == "None")
            {
                _ls_level = 0;
                return NO_ACTION;
            }
            if (method == "LoadShedder")
            {
                fstream fs2;
                fs2.open("/tmp/LoadLevel", ios::in);
                if (fs2)
                {
                    string ls_level;
                    fs2 >> ls_level;
                    fs2.close();
                    _ls_level = atoi(ls_level.c_str());
                    DEBUG << "ls_level = " << _ls_level;
                }
                else
                {
                    _ls_level = 0;
                }

                /*
                if (_load_shedder_running)
                {
                    _load_shedder_running = false;
                    return NO_ACTION;
                }
                else
                {
                    _load_shedder_running = true;
                    return LOAD_SHEDDING;
                }
                */
                return LOAD_SHEDDING;
            }
            if (method == "LoadManager")
            {
                /*
                if (_load_shedder_running)
                {
                    _load_shedder_running = false;
                    return NO_ACTION;
                }
                else
                {
                    return LOAD_BALANCING;
                }
                */
                _ls_level = 0;
                return LOAD_BALANCING;
            }
            if (method == "MetaOptimizer")
            {
                _ls_level = 0;
                double system_average_node_load =
                                        _lm->get_system_average_node_load();
                time_t t;
                time(&t);
                NOTICE << "Time: " << ctime(&t)
                       << ": System Average Node Load = "
                       << system_average_node_load;
                if (system_average_node_load > _load_shedding_threshold)
                {
                    return LOAD_SHEDDING;
                }
                else
                {
                    return LOAD_BALANCING;
                }
            }
        }
    }

    // If no file, pick one based on average system load.
    //

    _lm->update();

    _ls_level = 0;

    double system_average_node_load = _lm->get_system_average_node_load();

    time_t t;
    time(&t);
    NOTICE << "Time: " << ctime(&t)
           << ": System Average Node Load = " << system_average_node_load;

    if (system_average_node_load > _load_shedding_threshold)
    {
        /*
        if (_load_shedder_running)
        {
            _load_shedder_running = false;
            return NO_ACTION;
        }
        else
        {
            _load_shedder_running = true;
            return LOAD_SHEDDING;
        }
        */
        return LOAD_SHEDDING;
    }
    else
    {
        /*
        if (_load_shedder_running)
        {
            _load_shedder_running = false;
            return NO_ACTION;
        }
        else
        {
            return LOAD_BALANCING;
        }
        */
        return LOAD_BALANCING;
    }
}

void MetaOptimizer::update_statistics()
{
    // clear old statistics
    //
    _region_statistics.clear();

    // collect new statistics
    //
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

        _region_statistics[node_address]
                            = _region_server->_region_statistics[node_address]
                            = *query_processor.get_statistics();

        NOTICE << "Stats for node " << node_address << " : "
               << _region_statistics[node_address].as_string();

        _region_server->_region_cpu_statistics[node_address]=
                                *query_processor.get_cpu_statistics(); //olga

        DEBUG << "CPU Stats for node " << node_address << " : "
              << _region_server->_region_cpu_statistics[node_address];
    }
}

void MetaOptimizer::add_ignore_node_in_load_distribution(
                                    const string& node_name)
{
    _lm->add_ignore_node(node_name);
}

void MetaOptimizer::add_ignore_node_in_load_shedder(const string& node_name)
{
    _ls->add_ignore_node(node_name);
}

string MetaOptimizer::as_string() const
{
    return string("MetaOptimizer");
}

BOREALIS_NAMESPACE_END

using namespace Borealis;
using namespace NMSTL;

int main(int argc, const char* argv[])
{
    int period                               = DEFAULT_UPDATE_PERIOD;
    int region_server_port                   = DEFAULT_REGION_SERVER_PORT;
    int debugging_action                     = DEFAULT_DEBUGGING_ACTION;
    double load_shedding_threshold           = DEFAULT_LOAD_SHEDDING_THRESHOLD;
    double warmup_delay                      = DEFAULT_WARM_UP_DELAY;
    string load_distributon_ignore_node_name = "";
    int load_shedding_quality_metric         = DEFAULT_LS_QUALITY_METRIC;
    int runtime                              = INIT;

    stdArgs(argc, argv, 
            period, 
            region_server_port, 
            debugging_action,
            load_shedding_threshold,
            warmup_delay,
            load_distributon_ignore_node_name,
            load_shedding_quality_metric,
            runtime);

    ptr<MetaOptimizer> meta_optimizer;

    NOTICE << "Initializing the MetaOptimizer...";
    meta_optimizer.set(new MetaOptimizer(period, 
                                         region_server_port,
                                         load_shedding_threshold,
                                         warmup_delay,
                                         (OptimizationAction)debugging_action,
                                         load_shedding_quality_metric,
                                         runtime));

    // Some nodes may have to be ignored in the optimization decisions.
    // (e.g., the node that runs the game engine).
    //
    if (load_distributon_ignore_node_name != "")
    {
        meta_optimizer->add_ignore_node_in_load_distribution(
                                         load_distributon_ignore_node_name);
        meta_optimizer->add_ignore_node_in_load_shedder(
                                         load_distributon_ignore_node_name);
        meta_optimizer->_ignore_nodes.insert(load_distributon_ignore_node_name);
    }

    meta_optimizer->start_optimizer();

    meta_optimizer->start_region_server();

    NOTICE << "Terminating the MetaOptimizer...";

    return 0;
}


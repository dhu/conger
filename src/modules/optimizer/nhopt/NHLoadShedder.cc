
#include "NHLoadShedder.h"
#include "rpc_NHLoadShedder.h"
#include <fstream>

BOREALIS_NAMESPACE_BEGIN

// NHLoadShedderMode is NONE unless otherwise specified.
//
NHLoadShedder::NHLoadShedder(string id, NHLoadShedderMode mode)
             : BasicComponent(id, "NHLoadShedder"), _mode(mode)
{
    _local_catalog = NULL;
    _done = false;
    _measuring = false;

    assert((_mode == NONE) || 
           (_mode == LOCAL) || 
           (_mode == NH) ||
           (_mode == LOCAL_TOPDOWN)); 

    _sink_servers.clear();
    _source_servers.clear();

    _stats_file.set(new ofstream("/tmp/borealis_stats_tput.txt"));

    _current_fit_index = -1; // none
    _current_drop_boxes.clear();
}

NHLoadShedder::~NHLoadShedder()
{
}

// Initially, wait a while to make sure catalog and stats are populated
// properly.
//
void NHLoadShedder::in_thread_init()
{
    NOTICE << "NHLoadShedder at " << _id << " has started...";

    if (_mode == LOCAL)
    {
        (new CallbackTimer(_my_loop, wrap(this,
                                          &NHLoadShedder::setup_local)))
             ->arm(Time::now() + Time::msecs(INITIAL_WARM_UP_WAIT));
    }
    else if (_mode == NH)
    {
        (new CallbackTimer(_my_loop, wrap(this,
                                          &NHLoadShedder::setup_nh)))
             ->arm(Time::now() + Time::msecs(INITIAL_WARM_UP_WAIT));
    }
    else if (_mode == LOCAL_TOPDOWN) // *** don't use this one ***
    {
        (new CallbackTimer(_my_loop, wrap(this,
                                          &NHLoadShedder::setup_local_td)))
             ->arm(Time::now() + Time::msecs(INITIAL_WARM_UP_WAIT));
    }
    else
    {
        (new CallbackTimer(_my_loop, wrap(this,
                                          &NHLoadShedder::setup_none)))
             ->arm(Time::now() + Time::msecs(INITIAL_WARM_UP_WAIT));
    }
}

// Record the most recent window of throughput stats before shutting down. 
//
void NHLoadShedder::finish_up()
{
    read_local_statistics();
    NOTICE << "Final stats before NHLoadShedder shuts down:";
    NOTICE << _statistics.as_string();

    // Let's also read the latest catalog.
    //
    read_local_catalog();

    double total_tput = 0;
    Catalog::StreamMap *boxes = _local_catalog->get_stream_map();
    for (Catalog::StreamMap::iterator iter = boxes->begin(); 
         iter != boxes->end(); ++iter)
    {
        CatalogStream& stream = iter->second;
        if (!(stream.is_output_stream()))
        {
            continue;
        }

        string stream_name = (iter->first).as_string();

        double avg_tput; 

        try
        {
            const FixLenTimeSeries &f = _statistics.get_statistics(
                                                "TupleNum", stream_name);
            try
            {
                avg_tput = f.get_average();
            }
            catch(...)
            {
                WARN << "Could not get \"Average TupleNum\" for stream "
                     << stream_name;
                return;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"TupleNum\" for stream " 
                 << stream_name;
            return;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"TupleNum\" for stream " 
                 << stream_name;
            return;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"TupleNum\" for stream " 
                 << stream_name;
            return;
        }

        total_tput += avg_tput;
    }

    NOTICE << "Total recent tput = " << total_tput;
    *_stats_file << total_tput << endl;
}

// Let local QP know that it can start measuring throughput.
//
void NHLoadShedder::measure_local()
{
    RPC<void> result = _qp.start_measure();

    DEBUG << "result : " << result.stat();

    if (!result.stat())
    {
        WARN << "Couldn't start the measuring process";
        return;
    }

    _measuring = true;
}

// Let remote QP know that it can start measuring throughput.
//
AsyncRPC<void> NHLoadShedder::measure_remote()
{
    AsyncRPC<void> completion;

    if (_measuring)
    {
        NOTICE << "Node " << _id.get_id() << " has already started measuring.";
        completion.post(true);
        return completion;
    }

    RPC<void> result = _qp.start_measure();

    DEBUG << "result : " << result.stat();

    if (!result.stat())
    {
        WARN << "Couldn't start the measuring process";
        completion.post(false);
        return completion;
    }
    else
    {
        _measuring = true;
        completion.post(true);
        return completion;
    }
}

void NHLoadShedder::setup_none()
{
     NOTICE << "NHLoadShedder setting up none...";

     read_local_catalog();

     read_local_statistics();
     //NOTICE << _statistics.as_string();

     //generate_local_FIT();
     initial_prep();

     _local_fit = FeasibleInputTable();

     //NOTICE << "There is no FIT";

     //shed_load_continuous();
}

void NHLoadShedder::setup_local()
{
    NOTICE << "NHLoadShedder setting up local...";

    read_local_catalog();

    read_local_statistics();
    NOTICE << _statistics.as_string();

    initial_prep();
    generate_local_FIT();

    _fit = _local_fit; // no propagation from children, use the local FIT.

    NOTICE << "FIT ok";

    //NOTICE << "can shed load";

    //shed_load();
    shed_load_continuous();

    //_done = true;
}

// Generate local metadata and shed load locally, topdown from root to leaves.
//
void NHLoadShedder::setup_local_td()
{
    NOTICE << "NHLoadShedder setting up topdown-local...";

    read_local_catalog();

    read_local_statistics();
    NOTICE << _statistics.as_string();

    initial_prep();
    generate_local_FIT();

    _fit = _local_fit; // no propagation from children, use the local FIT.

    NOTICE << "FIT ok";

    if (is_root_node())
    {
        DEBUG << "root node..";

        NOTICE << "can shed load";

        shed_load();

        //_done = true;
    }
    else
    {
        DEBUG << "I am a non-root node, "
              << "I will wait for all my parents to be done.";

        while (1)
        {
            bool all_done = true;

            for (unsigned int i = 0; i < _source_servers.size(); i++)
            {
                DEBUG << "Checking source server " 
                      << to_string(_source_servers[i]);

                Remote<NHLoadShedder> remote_nh_ls = 
                            nh_ls_for(MedusaID(to_string(_source_servers[i])));

                if (remote_nh_ls)
                {
                    RPC<bool> nh_done = remote_nh_ls.is_done();
                    DEBUG << "done : " << *nh_done;
                    if (*nh_done == false)
                    {
                        all_done = false;
                        break;
                    }
                }
                else
                {
                    WARN << "No NHLoadShedder for " << _source_servers[i];
                }
            }

            if (all_done)
            {
                DEBUG << "All done.";
                break;
            }
            else
            {
                if (is_overloaded())
                {
                    NOTICE << "I am overloaded, but I have to wait!";
                }

                DEBUG << "Not all done yet, will have to wait...";
                Thread::sleep(Time::msecs(NEIGHBOR_WAIT));
            }
        }

        NOTICE << "can shed load";

        shed_load();

        //_done = true;
    }
}

// Generate local metadata and shed load in the neighborhood by FIT propagation.
//
void NHLoadShedder::setup_nh()
{
    NOTICE << "NHLoadShedder setting up NH...";

    read_local_catalog();

    read_local_statistics();
    NOTICE << _statistics.as_string();

    initial_prep();
    generate_local_FIT();

    if (is_leaf_node())
    {
        DEBUG << "leaf node..";
        
        _fit = _local_fit; 
        
        NOTICE << "FIT ok";

        propagate_FIT_to_parent();
    
        shed_load_continuous();
    }
    else
    {
        DEBUG << "not a leaf, nothing to do until it can merge children fits";
    }

    /*
    if (is_leaf_node())
    {
        DEBUG << "leaf node..";

        _fit = _local_fit; // no propagation from children, use the local FIT.

        NOTICE << "FIT ok";

        propagate_FIT_to_parent();

        DEBUG << "I am non-root node, "
              << "I will wait for all my parents to be done.";

        while (1)
        {
            bool all_done = true;

            for (unsigned int i = 0; i < _source_servers.size(); i++)
            {
                DEBUG << "Checking source server " 
                      << to_string(_source_servers[i]);

                Remote<NHLoadShedder> remote_nh_ls = 
                            nh_ls_for(MedusaID(to_string(_source_servers[i])));

                if (remote_nh_ls)
                {
                    RPC<bool> nh_done = remote_nh_ls.is_done();
                    DEBUG << "done : " << *nh_done;
                    if (*nh_done == false)
                    {
                        all_done = false;
                        break;
                    }
                }
            }

            if (all_done)
            {
                DEBUG << "All done.";
                break;
            }
            else
            {
                if (is_overloaded())
                {
                    NOTICE << "I am overloaded, but I have to wait!";
                }

                DEBUG << "Not all done yet, will have to wait...";
                Thread::sleep(Time::msecs(NEIGHBOR_WAIT));
            }
        }

        NOTICE << "can shed load";

        shed_load();

        //_done = true;
    }
    else
    {
        DEBUG << "Nothing else to do "
              << "until children nodes propagate their FITs.";
    }
    */
}

bool NHLoadShedder::is_leaf_node()
{
    return (_sink_servers.empty());
}

bool NHLoadShedder::is_root_node()
{
    return (_source_servers.empty());
}

RPC<bool> NHLoadShedder::is_done()
{
    if (_done)
    {
        return true;
    }
    else
    {
        return false;
    }
}

AsyncRPC<void> NHLoadShedder::send_FIT(InetAddress addr, FeasibleInputTable fit)
{
    AsyncRPC<void> completion;

    NOTICE << "Received FIT from child";

    for (unsigned int i = 0; i < fit._table.size(); i++)
    {
        DEBUG << fit._table[i].as_string();
    }

    _children_fit[addr] = fit;

    while (1)
    {
        bool all_done = true;

        for (unsigned int i = 0; i < _sink_servers.size(); i++)
        {
            DEBUG << "Checking sink server " 
                  << to_string(_sink_servers[i]);

            if ((_children_fit[_sink_servers[i]]).empty())
            {
                DEBUG << "FIT from " << to_string(_sink_servers[i]) 
                      << " not received";
                all_done = false;
                break;
            }
        }

        if (all_done)
        {
            NOTICE << "All children's FITs are received.";
            break;
        }
        else
        {
            DEBUG << "Not all children's FITs are received yet, will wait...";
            Thread::sleep(Time::msecs(NEIGHBOR_WAIT));
        }
    }

    merge_FIT_from_children();

    NOTICE << "FIT ok";

    if (!(is_root_node()))
    {
        DEBUG << "NOT root node..";

        propagate_FIT_to_parent();
    }

    shed_load_continuous();

    /*
    if (!(is_root_node()))
    {
        DEBUG << "NOT root node..";

        propagate_FIT_to_parent();
        
        DEBUG << "I am a non-root node, "
              << "I will wait for all my parents to be done.";

        while (1)
        {
            bool all_done = true;

            for (unsigned int i = 0; i < _source_servers.size(); i++)
            {
                DEBUG << "Checking source server " 
                      << to_string(_source_servers[i]);

                Remote<NHLoadShedder> remote_nh_ls = 
                            nh_ls_for(MedusaID(to_string(_source_servers[i])));

                if (remote_nh_ls)
                {
                    RPC<bool> nh_done = remote_nh_ls.is_done();
                    DEBUG << "done : " << *nh_done;
                    if (*nh_done == false)
                    {
                        all_done = false;
                        break;
                    }
                }
            }

            if (all_done)
            {
                DEBUG << "All done.";
                break;
            }
            else
            {
                if (is_overloaded())
                {
                    NOTICE << "I am overloaded, but I have to wait!";
                }

                DEBUG << "Not all done yet, will have to wait...";
                Thread::sleep(Time::msecs(NEIGHBOR_WAIT));
            }
        }

        NOTICE << "can shed load";

        shed_load();

        //_done = true;
    }
    else
    {
        NOTICE << "can shed load";

        shed_load();

        //_done = true;
    }
    */

    completion.post(true);
    return completion;
}

void NHLoadShedder::insert_source_server(InetAddress addr)
{
    for (unsigned int i = 0; i < _source_servers.size(); i++)
    {
        if (_source_servers[i] == addr)
        {
            return;
        }
    }

    DEBUG << "Adding source server : " << to_string(addr);

    _source_servers.push_back(addr);
}

// Find out which Borealis nodes (if any) I am subscribed to.    
//
void NHLoadShedder::find_source_servers() throw(AuroraBadEntityException)
{
    for (InputInfoMap::iterator iter= _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        DEBUG << "Input " << iter->first;

        CatalogStream *stream = _local_catalog->get_stream(Name(iter->first));
        if (!stream)
        {
            WARN << "Stream \"" << iter->first 
                 << "\" does not exist in the catalog.";
            Throw(AuroraBadEntityException,
                  "Stream \"" + iter->first
                  + "\" does not exist in the catalog.");
        }

        if (stream->is_injected_stream())
        {
            DEBUG << "Input stream " << iter->first 
                  << " is an injected stream, so NO source servers..";
            continue;
        }

        if (stream->is_subscribed_stream()) // there is a parent node.
        {
            DEBUG << "Input stream " << iter->first 
                  << " is a subscribed stream, so will add its source server..";

            CatalogSubscription *sub = stream->get_source_subscription();
            if (!sub)
            {
                WARN << "Source subscription for stream \"" << iter->first 
                     << "\" does not exist in the catalog.";
                Throw(AuroraBadEntityException,
                      "Source subscription for stream \"" + iter->first
                      + "\" does not exist in the catalog.");
            }

            InetAddress addr = sub->get_subscription_endpoint();
            (iter->second).source_server = new InetAddress(addr);
            insert_source_server(addr);
        }
    }
}

void NHLoadShedder::insert_sink_server(InetAddress addr)
{
    // First convert the data port address to server port address: 
    //     server_addr.port = data_addr.port - DATA_PORT_OFFSET
    //
    unsigned int data_port = (unsigned int)(addr.get_port());
    assert(data_port > 0);
    unsigned int server_port = data_port - DATA_PORT_OFFSET;
    string host = addr.get_host();
    InetAddress server_addr = InetAddress(host, server_port);    
    DEBUG << "Actual server addr = " << server_addr;

    for (unsigned int i = 0; i < _sink_servers.size(); i++)
    {
        if (_sink_servers[i] == server_addr)
        {
            return;
        }
    }

    DEBUG << "Adding sink server : " << to_string(server_addr);
    
    _sink_servers.push_back(server_addr);
}

// Find out which Borealis nodes (if any) are subscribed to me.    
//
void NHLoadShedder::find_sink_servers()
{
    Catalog::StreamMap *streams = _local_catalog->get_stream_map();

    for (Catalog::StreamMap::iterator iter = streams->begin();
         iter != streams->end(); ++iter)
    {
        CatalogStream& catalog_stream = iter->second;

        if ((catalog_stream.get_subscribers())->size() > 0)
        {
            DEBUG << "Checking the subscribers for stream " << iter->first;

            vector<CatalogSubscription*> *subs = 
                                          catalog_stream.get_subscribers();

            for (unsigned int i = 0; i < subs->size(); i++)
            {
                InetAddress addr = ((*subs)[i])->get_subscription_endpoint();

                DEBUG << to_string(addr);

                if (((*subs)[i])->get_sink_flag()) // leaving the diagram
                {
                    DEBUG << to_string(addr) 
                          << " is a sink subscriber, will ignore it..";
                }
                else
                {
                    DEBUG << to_string(addr)
                          << " is NOT a sink subscriber, will add it.";
                    insert_sink_server(addr);
                }

            }
        }
    }
}

bool NHLoadShedder::is_feasible(FeasibleInputTable::RateMap rates, 
                                double *excess_load) throw(AuroraException)
{
    double total_load = 0;

    for (FeasibleInputTable::RateMapIter iter = rates.begin(); 
         iter != rates.end(); iter++)
    {
        if (_input_info.find(iter->first) == _input_info.end())
        {
            WARN << "Input information for stream \"" << iter->first
                 << "\" does not exist.";
            Throw(AuroraException,
                  "Input information for stream \"" + iter->first
                  + "\" does not exist.");
        }
        else 
        {
            total_load += iter->second *
                          (_input_info[iter->first].total_load_coef);
        }
    }

    if (total_load > 1.0)
    {
        (*excess_load) = total_load - 1.0;
        return false;
    }
    else 
    {
        (*excess_load) = total_load - 1.0; // can be negative..
        return true;
    }
}

bool NHLoadShedder::is_feasible_with_plan(FeasibleInputTable::RateMap rates,
                                 double *excess_load) throw(AuroraException)
{
    double total_load = 0;

    for (FeasibleInputTable::RateMapIter iter = rates.begin();
         iter != rates.end(); iter++)
    {
        if (_input_info.find(iter->first) == _input_info.end())
        {
            WARN << "Input information for stream \"" << iter->first
                 << "\" does not exist.";
            Throw(AuroraException,
                  "Input information for stream \"" + iter->first
                  + "\" does not exist.");
        }
        else
        {
            total_load += iter->second *
                          (_input_info[iter->first].reduced_load_coef);
        }
    }

    if (total_load > 1.0)
    {
        (*excess_load) = total_load - 1.0;
        return false;
    }
    else
    {
        (*excess_load) = 0;
        return true;
    }
}

int NHLoadShedder::get_drop_location_id(Name stream, Name box_name,
                                        uint16 sink_port)
{
    for (DropLocationMap::const_iterator iter = _drop_locations.begin();
         iter != _drop_locations.end(); iter++)
    {
        if ((iter->second).input_flag)
        {
            continue;
        }

        Name dl_stream = (iter->second).in_stream_name;
        Name dl_box = (iter->second).dest_box_name;
        uint16 dl_port = (iter->second).dest_box_port;

        if ((stream == dl_stream) && (box_name == dl_box) && 
            (sink_port == dl_port))
        {
            return(iter->first);
        }
    }

    return 0; // it means: no such drop location.
}

void NHLoadShedder::find_dlproperties(Name box_name, string drop_type,
                                      BoxParameter::ParameterMap box_params)
{
    int loc_id;

    CatalogBox *box = _local_catalog->get_box(box_name);
    BoxParameter::ParameterMap *pm;

    if (box->get_box_type() == "aggregate")
    {
        pm = box->get_box_parameter_map();

        if (drop_type == "random_drop")
        {
            drop_type = "window_drop";
            box_params["window-size-by"] = (*pm)["window-size-by"];
            box_params["window-size"] = (*pm)["window-size"] ;
            box_params["window-slide"] = (*pm)["advance"];
            box_params["order-by"] = (*pm)["order-by"];
            box_params["order-on-field"] = (*pm)["order-on-field"];
            if ((*pm).find("group-by") != (*pm).end())
            {
                box_params["group-by"] = (*pm)["group-by"];
            }
        }
        else if (drop_type == "window_drop")
        {
            assert(box_params["window-size-by"] == (*pm)["window-size-by"]);
            assert(box_params["order-by"] == (*pm)["order-by"]);
            assert(box_params["order-on-field"] == (*pm)["order-on-field"]);
            if (box_params.find("group-by") != box_params.end())
            {
                 assert(box_params["group-by"] == (*pm)["group-by"]);
            }

            box_params["window-size"] = string() +
                                (atoi(box_params["window-size"].c_str()) +
                                 atoi((*pm)["window-size"].c_str()) - 1);
            // TO DO: splits => changes in window-slide
            // TO DO: splits => changes in max_batch_size
        }
    }
    else if (box->get_box_type() == "window_drop")
    {
        // TO DO
    }
    else if (box->get_box_type() == "union")
    {
        // TO DO
    }
    else if (box->get_box_type() == "join")
    {
        // TO DO
    }

    CatalogStream::StreamStar *in_streams = box->get_box_in();

    for (unsigned int i = 0; i < in_streams->size(); i++)
    {
        CatalogStream *in_stream = (*in_streams)[i];
        vector<BoxPort> *sinks = in_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            
            if ((loc_id = get_drop_location_id(in_stream->get_stream_name(),
                                               box_name,
                                               (*sinks)[j].get_port())))
            {
                _drop_locations[loc_id].box_type = drop_type;
                _drop_locations[loc_id].box_params.set_parameter_map(
                                                            &box_params);
            }
        }

        if ((in_stream->is_input_stream()) || 
            (in_stream->is_subscribed_stream()))
        {
            if (_input_info[in_stream->get_stream_name_string()].dl_id.size() 
                                                                        > 1)
            {
                int input_dl = 
                    _input_info[in_stream->get_stream_name_string()].dl_id[0];
                _drop_locations[input_dl].box_type = drop_type;
                _drop_locations[input_dl].box_params.set_parameter_map(
                                                                &box_params);
            }
        }
        else 
        {
            find_dlproperties(
                        in_stream->get_stream_source_box()->get_box_name(),
                        drop_type, box_params);
        }
    }
}

void NHLoadShedder::compute_load_down(Name box_name, double *load, 
                                      double *quality) throw(AuroraException)
{
    DEBUG << "NHLoadShedder::compute_load_down";

    CatalogBox *box = _local_catalog->get_box(box_name);
    assert(box->get_box_node() == _id.get_id());

    try
    {
        const FixLenTimeSeries &f = _statistics.get_statistics(
                                            "Cost", box_name.as_string());
        try
        {
            double avg = f.get_average();
            (*load) += avg;
        }
        catch(...)
        {
            WARN << "Could not get \"Average Cost\" for box " 
                 << box_name.as_string();
            return;
        }
    }
    catch(Statistics::error e)
    {
        WARN << "Statistics::Error " << (int)e;
        WARN << "Could not read \"Cost\" for box " << box_name;
        return;
    }
    catch(TSStats::error e)
    {
        WARN << "TSStats::Error " << (int)e;
        WARN << "Could not read \"Cost\" for box " << box_name;
        return;
    }
    catch(...)
    {
        WARN << "Unknown stats exception";
        WARN << "Could not read \"Cost\" for box " << box_name;
        return;
    }

    CatalogStream::StreamStar *out_streams = box->get_box_out();
    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];
        string selectivity_id = box_name.as_string() + " : " +
                                out_stream->get_stream_source()->get_port();
        double selectivity;
        try
        {
            const FixLenTimeSeries &f = _statistics.get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
                DEBUG << "selectivity for " << box_name.as_string()
                      << " = " << selectivity;
            }
            catch(...)
            {
                WARN << "Could not get \"Average Selectivity\" for "
                     << selectivity_id;
                return;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return;
        }


        if (out_stream->is_output_stream())
        {
            int num_sub = (out_stream->get_subscribers())->size();
            (*quality) += num_sub * (selectivity * 1.0);
            continue;
        }

        vector<BoxPort> *sinks = out_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            double quality_down = 0;
            CatalogBox *next_box = (*sinks)[j].get_port_box();

            if (!next_box) // output sink
            {
                DEBUG << "output sink (no next box)";
                quality_down = 1.0;
            }
            else
            {
                if (next_box->get_box_node() != _id.get_id()) // output sink
                { 
                    DEBUG << "output sink (node boundary)";
                    /*
                    StreamSinkPair p;
                    p.first = out_stream->get_stream_name_string();
                    p.second = j;
                    if (_priorities.find(p) == _priorities.end()) 
                    {
                        WARN << "Priority information for stream sink pair \""
                             << p.first << ", " << p.second
                             << "\" does not exist.";
                        Throw(AuroraException,
                              "Priority information for stream sink pair \"" 
                              + p.first + ", " + p.second
                              + "\" does not exist.");
                    }
                    else 
                    {
                        quality_down = _priorities[p];
                    }
                    */
                    quality_down = 1.0;
                }
                else 
                {
                    double load_down = 0;
                    compute_load_down(((*sinks)[j].get_port_box())
                                      ->get_box_name(), 
                                      &load_down, &quality_down);
                    (*load) += selectivity * load_down;
                }
            }
            (*quality) += selectivity * quality_down; // selectivity?
        }
    }

    DEBUG << "load = " << *load;
    DEBUG << "quality = " << *quality;
}

void NHLoadShedder::compute_load_coefs() throw(AuroraException)
{
    DEBUG << "NHLoadShedder::compute_load_coefs()";

    for (DropLocationMap::iterator iter = _drop_locations.begin();
         iter != _drop_locations.end(); iter++)
    {
        if ((iter->second).input_flag)
        {
            continue;
        }

        string box_name = (iter->second).dest_box_name.as_string();
        double load = 0;
        double quality = 0;

        try
        {
            const FixLenTimeSeries &f = _statistics.get_statistics(
                                                    "Cost", box_name);
            try
            {
                double avg = f.get_average();
                load += avg;
            }
            catch(...)
            {
                WARN << "Could not get \"Average Cost\" for box " << box_name;
                return;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"Cost\" for box " << box_name;
            return;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"Cost\" for box " << box_name;
            return;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"Cost\" for box " << box_name;
            return;
        }

        CatalogBox *box = _local_catalog->get_box(Name(box_name));
        CatalogStream::StreamStar *out_streams = box->get_box_out();
        for (unsigned int i = 0; i < out_streams->size(); i++)
        {
            CatalogStream *out_stream = (*out_streams)[i];
            string selectivity_id = box_name + " : " +
                                    out_stream->get_stream_source()->get_port();
            double selectivity;
            try
            {
                const FixLenTimeSeries &f = _statistics.get_statistics(
                                                "Selectivity", selectivity_id);
                try
                {
                    selectivity = f.get_average();
                    DEBUG << "selectivity for " << box_name
                          << " = " << selectivity;
                }
                catch(...)
                {
                    WARN << "Could not get \"Average Selectivity\" for "
                         << selectivity_id;
                    return;
                }
            }
            catch(Statistics::error e)
            {
                WARN << "Statistics::Error " << (int)e;
                WARN << "Could not read \"Selectivity\" for " << selectivity_id;
                return;
            }
            catch(TSStats::error e)
            {
                WARN << "TSStats::Error " << (int)e;
                WARN << "Could not read \"Selectivity\" for " << selectivity_id;
                return;
            }
            catch(...)
            {
                WARN << "Unknown stats exception";
                WARN << "Could not read \"Selectivity\" for " << selectivity_id;
                return;
            }

            if (out_stream->is_output_stream())
            {
                int num_sub = (out_stream->get_subscribers())->size();
                quality += num_sub * (selectivity * 1.0);
                continue;
            }

            vector<BoxPort> *sinks = out_stream->get_sink_port();
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                double quality_down = 0;
                CatalogBox *next_box = (*sinks)[j].get_port_box();

                if (!next_box) // output sink
                {
                    quality_down = 1.0;
                }
                else
                {
                    if (next_box->get_box_node() != _id.get_id()) // output sink
                    { 
                        /*
                        StreamSinkPair p;
                        p.first = out_stream->get_stream_name_string();
                        p.second = j;
                        if (_priorities.find(p) == _priorities.end()) 
                        {
                            WARN << "Priority info for stream sink pair \""
                                 << p.first << ", " << p.second
                                 << "\" does not exist.";
                            Throw(AuroraException,
                                  "Priority info for stream sink pair \"" 
                                  + p.first + ", " + p.second
                                  + "\" does not exist.");
                        }
                        else 
                        {
                            quality_down = _priorities[p];
                        }
                        */
                        quality_down = 1.0;
                    }
                    else 
                    {
                        double load_down = 0;
                        compute_load_down(next_box->get_box_name(),
                                          &load_down, &quality_down);
                        load += selectivity * load_down;
                    }
                }

                quality += selectivity * quality_down; // selectivity?
            }
        }
    
        DEBUG << "load = " << load;
        DEBUG << "quality = " << quality;

        (iter->second).load_coef = load;
        (iter->second).reduced_load_coef = load; // initialize
        (iter->second).quality_score = quality;
        (iter->second).reduced_quality_score = quality; // initialize
    }
}

void NHLoadShedder::find_drop_locations() throw(AuroraBadEntityException)
{
    int dl_count = 0;

    // All input arcs are potential drop locations.
    //
    Catalog::StreamMap *streams = _local_catalog->get_stream_map();
    for (Catalog::StreamMap::iterator iter = streams->begin();
         iter != streams->end(); ++iter)
    {
        CatalogStream& catalog_stream = iter->second;

        if ((catalog_stream.is_input_stream()) ||
            (catalog_stream.is_subscribed_stream()))
        {
            vector<BoxPort> *sinks = catalog_stream.get_sink_port();

            if (!sinks)
            {
                WARN << "Stream \"" << catalog_stream.get_stream_name_string()
                     << "\" has no sink ports.";
                Throw(AuroraBadEntityException,
                      "Stream \"" + catalog_stream.get_stream_name_string()
                      + "\" has no sink ports.");
            }

            int good_sink_count = 0;
            for (unsigned int i = 0; i < sinks->size(); i++)
            {
                if (((*sinks)[i].get_port_box()) == NULL)
                {
                    continue;
                }

                ++good_sink_count;
                DropLocation loc;
                loc.id = ++dl_count;
                loc.in_stream_name = catalog_stream.get_stream_name();
                loc.dest_box_name = 
                            ((*sinks)[i].get_port_box())->get_box_name();
                loc.dest_box_port = (*sinks)[i].get_port();
                _drop_locations[loc.id] = loc;
                _input_info[(iter->first).as_string()].dl_id.push_back(loc.id);
            }

            if (good_sink_count > 1) // we need a separate one for the input
            {
                DropLocation loc;
                loc.id = ++dl_count;
                loc.in_stream_name = catalog_stream.get_stream_name();
                loc.input_flag = true;
                loc.input_stream = catalog_stream.get_stream_name_string();
                _drop_locations[loc.id] = loc;
                _input_info[(iter->first).as_string()].dl_id.insert(
                    _input_info[(iter->first).as_string()].dl_id.begin(), 
                    loc.id);
            }
        }
    }

    // All fan-out branches of a split box are potential drop locations.
    //
    Catalog::BoxMap *boxes = _local_catalog->get_box_map();
    for (Catalog::BoxMap::iterator iter = boxes->begin();
         iter != boxes->end(); ++iter)
    {
        CatalogBox& catalog_box = iter->second;

        // Assuming all branches carry replicas (i.e, no case-like filter).
        //
        CatalogStream::StreamStar *out_streams = catalog_box.get_box_out();

        if (!out_streams)
        {
            continue;
        }

        if (out_streams->empty())
        {
            continue;
        }

        if ((catalog_box.get_out_stream(0))->is_output_stream())
        {
            continue;
        }

        if (((catalog_box.get_box_out())->size() > 1) ||
            ((catalog_box.get_out_stream(0))->get_sink_port()->size() > 1))
        {
            for (unsigned int i = 0; i < out_streams->size(); i++)
            {
                CatalogStream *stream = (*out_streams)[i];

                vector<BoxPort> *sinks = stream->get_sink_port();

                if (!sinks)
                {
                    WARN << "Stream \"" << stream->get_stream_name_string()
                         << "\" has no sink ports.";
                    Throw(AuroraBadEntityException,
                          "Stream \"" + stream->get_stream_name_string()
                          + "\" has no sink ports.");
                }

                for (unsigned int j = 0; j < sinks->size(); j++)
                {
                    DropLocation loc;
                    loc.id = ++dl_count;
                    loc.in_stream_name = stream->get_stream_name();
                    loc.dest_box_name = 
                                ((*sinks)[j].get_port_box())->get_box_name();
                    loc.dest_box_port = (*sinks)[j].get_port();
                    _drop_locations[loc.id] = loc;
                }
            }
        }
        else
        {
            continue;
        }
    }

    // Find box_type and box_params for each drop location.
    //
    for (Catalog::StreamMap::iterator iter = streams->begin();
         iter != streams->end(); ++iter)
    {
        CatalogStream& catalog_stream = iter->second;

        if (catalog_stream.is_output_stream())
        {
            BoxParameter::ParameterMap pm;
            uint16 min_gap = MAX_GAP;
            /*
            vector<CatalogSubscription*> *subs = 
                                        catalog_stream.get_subscribers();
            if (subs)
            {
                for (unsigned int i = 0; i < subs->size(); i++)
                {
                    StreamSinkPair p;
                    p.first = catalog_stream.get_stream_name_string();
                    p.second = i;
                    if (_gaps.find(p) == _gaps.end())
                    {
                        WARN << "Gap information for stream sink pair \""
                             << p.first << ", " << p.second
                             << "\" does not exist.";
                        Throw(AuroraBadEntityException,
                              "Gap information for stream sink pair \"" 
                              + p.first + ", " + p.second 
                              + "\" does not exist.");
                    }
                    else 
                    {
                        if (_gaps[p] < min_gap)
                        {
                            min_gap = _gaps[p];
                        }
                    }
                }
            }
            */

            pm["max_batch_size"] = string() + min_gap;

            if (catalog_stream.get_stream_source_box() == NULL)
            {
                continue;
            }

            find_dlproperties(
                        catalog_stream.get_stream_source_box()->get_box_name(), 
                        "random_drop", pm);
        }
    }
}

void NHLoadShedder::compute_input_info() throw(AuroraException)
{ 
    DEBUG << "NHLoadShedder::compute_input_info()";

    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        string stream_name = iter->first;

        double load_coef = 0;
        double quality = 0;
        unsigned int k = 0;
        if ((iter->second).dl_id.size() > 1)
        {
            ++k;
        }

        for (unsigned int i = k; i < (iter->second).dl_id.size(); i++)
        {
            if (_drop_locations.find(((iter->second).dl_id)[i]) ==
                _drop_locations.end())
            {
                WARN << "Drop location \"" << ((iter->second).dl_id)[i]
                     << "\" does not exist.";
                Throw(AuroraException,
                      "Drop location does not exist.");
            }
            else
            {
                load_coef += 
                    _drop_locations[((iter->second).dl_id)[i]].load_coef;
                quality +=
                    _drop_locations[((iter->second).dl_id)[i]].quality_score;
            }
        }

        (iter->second).total_load_coef = load_coef;
        (iter->second).reduced_load_coef = load_coef; // initialize
        (iter->second).total_quality = quality;

        if (load_coef > 0)
        {
            (iter->second).max_rate = 1.0/load_coef; 
        }
        else
        {
            WARN << "Zero load coefficient.";
            Throw(AuroraException,
                  "Zero load coefficient.");
        }

        if (_drop_locations[((iter->second).dl_id)[0]].input_flag)
        {
            _drop_locations[((iter->second).dl_id)[0]].load_coef = load_coef;
            _drop_locations[((iter->second).dl_id)[0]].quality_score = quality;
        }
    }
}

void NHLoadShedder::compute_spreads()
{
    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        (iter->second).spread = 1; // this will change
    }
}

void NHLoadShedder::collect_dl_from_subtree(Name stream_name, Name box_name, 
                                            double sel)
{
    CatalogBox *box = _local_catalog->get_box(box_name);

    if (!box)
    {
        return;
    }

    if (box->get_box_node() != _id.get_id())
    {
        return;
    }

    CatalogStream::StreamStar *out_streams = box->get_box_out();

    if (!out_streams)
    {
        return;
    }

    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];
        string selectivity_id = box_name.as_string() + " : " +
                                out_stream->get_stream_source()->get_port();
        double selectivity;
        try
        {
            const FixLenTimeSeries &f = _statistics.get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
                DEBUG << "selectivity for " << box_name.as_string()
                      << " = " << selectivity;
            }
            catch(...)
            {
                WARN << "Could not get \"Average Selectivity\" for "
                     << selectivity_id;
                return;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return;
        }

        vector<BoxPort> *sinks = out_stream->get_sink_port();

        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            CatalogBox *next_box = (*sinks)[j].get_port_box();
            assert(next_box);

            int dl_id = get_drop_location_id(out_stream->get_stream_name(),
                                             next_box->get_box_name(),
                                             (*sinks)[j].get_port());
            if (dl_id > 0)
            {
                _drop_locations[dl_id].input_stream = stream_name.as_string();
                _drop_locations[dl_id].rate_coef = sel*selectivity;

                _input_info[stream_name.as_string()].dl_subtree.
                                                     push_back(dl_id);
            }

            collect_dl_from_subtree(stream_name,
                                ((*sinks)[j].get_port_box())->get_box_name(),
                                sel*selectivity);
        }
    }
}

// Pick the dl with the smallest ratio.
//
int NHLoadShedder::pick_dl(Name stream_name) throw(AuroraException)
{
    double min_ratio = 1000;
    int min_dl = 0;

    for (unsigned int i = 0; 
         i < _input_info[stream_name.as_string()].dl_subtree.size(); 
         i++)
    {
        int dl_id = (_input_info[stream_name.as_string()].dl_subtree)[i];
        assert(_drop_locations.find(dl_id) != _drop_locations.end());
        DEBUG << "checking " << dl_id;

        if (_drop_locations[dl_id].reduced_load_coef <= 0)
        {
            WARN << "Negative reduced load coefficient.";
            Throw(AuroraException,
                  "Negative reduced load coefficient.");
        }

        double ratio = _drop_locations[dl_id].reduced_quality_score/
                       _drop_locations[dl_id].reduced_load_coef;

        DEBUG << "ratio = " << ratio;

        if (ratio < min_ratio)
        {
            min_ratio = ratio;
            min_dl = dl_id;
        }
    }

    DEBUG << "min dl = " << min_dl;

    return min_dl;
}

vector<int>::iterator NHLoadShedder::get_dl_iter(Name stream_name, int dl_id)
{
    vector<int>::iterator r, iter;
    r = _input_info[stream_name.as_string()].dl_subtree.end();
    
    for (iter = _input_info[stream_name.as_string()].dl_subtree.begin(); 
         iter != _input_info[stream_name.as_string()].dl_subtree.end(); 
         iter++)
    {
        if ((*iter) == dl_id)
        {
            return iter;
        }
    }

    WARN << "Couldn't find the dl iter";
    return r;
}

void NHLoadShedder::eliminate_dl_subtree(Name stream_name, Name next_box_name)
{
    DEBUG << "NHLoadShedder::eliminate_dl_subtree()";

    CatalogBox *box = _local_catalog->get_box(next_box_name);

    if (!box)
    {
        return;
    }

    if (box->get_box_node() != _id.get_id())
    {
        return;
    }

    CatalogStream::StreamStar *out_streams = box->get_box_out();

    if (!out_streams)
    {
        return;
    }

    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];

        vector<BoxPort> *sinks = out_stream->get_sink_port();

        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            CatalogBox *next_box = (*sinks)[j].get_port_box();
            assert(next_box);

            int dl_id = get_drop_location_id(out_stream->get_stream_name(),
                                             next_box->get_box_name(),
                                             (*sinks)[j].get_port());
            if (dl_id > 0)
            {
                vector<int>::iterator iter = get_dl_iter(stream_name, dl_id);
                if (iter != 
                    _input_info[stream_name.as_string()].dl_subtree.end())
                {
                    _input_info[stream_name.as_string()].dl_subtree.erase(iter);
                }
            }

            eliminate_dl_subtree(stream_name,
                                 ((*sinks)[j].get_port_box())->get_box_name());
        }
    }
}

// Walk back from box, reducing load and quality as necessary.
//
void NHLoadShedder::update_dl_supertree(Name stream_name, int min_dl, 
                                        CatalogBox *box)
{
    DEBUG << "NHLoadShedder::update_dl_supertree()";

    CatalogStream::StreamStar *in_streams = box->get_box_in();

    for (unsigned int i = 0; i < in_streams->size(); i++)
    {
        CatalogStream *in_stream = (*in_streams)[i];
        vector<BoxPort> *sinks = in_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            int loc_id;

            if ((loc_id = get_drop_location_id(in_stream->get_stream_name(),
                                               box->get_box_name(),
                                               (*sinks)[j].get_port())))
            {
                DEBUG << "reducing from location " << loc_id;

                _drop_locations[loc_id].reduced_load_coef -=
                    _drop_locations[min_dl].reduced_load_coef *
                    (_drop_locations[min_dl].rate_coef/
                     _drop_locations[loc_id].rate_coef);
                _drop_locations[loc_id].reduced_quality_score -=
                    _drop_locations[min_dl].reduced_quality_score *
                    (_drop_locations[min_dl].rate_coef/
                     _drop_locations[loc_id].rate_coef);
            }
        }

        if (in_stream->get_stream_name() == stream_name)
        {
            DEBUG << "reducing from input stream " << stream_name.as_string();

            if (_input_info[stream_name.as_string()].dl_id.size() > 1)
            {
                DEBUG << "it is a split one..";

                int input_dl = _input_info[stream_name.as_string()].dl_id[0];

                _drop_locations[input_dl].reduced_load_coef -=
                    _drop_locations[min_dl].reduced_load_coef *
                    (_drop_locations[min_dl].rate_coef/
                     _drop_locations[input_dl].rate_coef);
                _drop_locations[input_dl].reduced_quality_score -=
                    _drop_locations[min_dl].reduced_quality_score *
                    (_drop_locations[min_dl].rate_coef/
                     _drop_locations[input_dl].rate_coef);
                _input_info[stream_name.as_string()].reduced_load_coef =
                        _drop_locations[input_dl].reduced_load_coef;
                _input_info[stream_name.as_string()].reduced_quality = 
                        _drop_locations[input_dl].reduced_quality_score;
            }
            else
            {
                DEBUG << "it is NOT a split one..";

                int input_dl = _input_info[stream_name.as_string()].dl_id[0];

                _input_info[stream_name.as_string()].reduced_load_coef =
                        _drop_locations[input_dl].reduced_load_coef;
                _input_info[stream_name.as_string()].reduced_quality =
                        _drop_locations[input_dl].reduced_quality_score;
                DEBUG << "reduced_load_coef = "
                      << _input_info[stream_name.as_string()].reduced_load_coef;
                DEBUG << "reduced_quality = "
                      << _input_info[stream_name.as_string()].reduced_quality;
            }
        }
        else 
        {
            update_dl_supertree(stream_name, min_dl,
                                in_stream->get_stream_source_box());
        }
    }
}

void NHLoadShedder::sort_dl_subtree(Name stream_name)
{
    DEBUG << "NHLoadShedder::sort_dl_subtree()";

    while (_input_info[stream_name.as_string()].dl_subtree.size() > 0)
    {
        DEBUG << "Drop locations before the sort iteration";
        for (DropLocationMap::const_iterator iter = _drop_locations.begin();
              iter != _drop_locations.end(); iter++)
        {
            DEBUG << iter->second;
        }

        int min_dl = pick_dl(stream_name);

        if (min_dl == 0) // can not be picked for some bad reason, so stop.
        {
            WARN << "min_dl can not be picked";
            break;
        }

        // Transfer min_dl to the sorted list.
        //
        (_input_info[stream_name.as_string()].dl_subtree_sorted).
                                                push_back(min_dl);
    
        // Eliminate all drop locations (from dl_subtree)
        // in min_dl's subtree, including min_dl.
        //
        vector<int>::iterator iter = get_dl_iter(stream_name, min_dl);
        assert(iter != _input_info[stream_name.as_string()].dl_subtree.end());
        _input_info[stream_name.as_string()].dl_subtree.erase(iter);
        eliminate_dl_subtree(stream_name,
                             _drop_locations[min_dl].dest_box_name);

        // Update load and quality for drop locations (in dl_subtree)
        // in min_dl's supertree.
        //
        Name in_stream_name = _drop_locations[min_dl].in_stream_name;
        if (in_stream_name != stream_name) // not at the root yet.
        {
            CatalogStream *in_stream =
                            _local_catalog->get_stream(in_stream_name);
            CatalogBox *in_box = in_stream->get_stream_source_box();
            update_dl_supertree(stream_name, min_dl, in_box);
        }
        else
        {
            DEBUG << "reducing from input stream " << stream_name.as_string();

            if (_input_info[stream_name.as_string()].dl_id.size() > 1)
            {
                DEBUG << "it is a split one..";

                int input_dl = _input_info[stream_name.as_string()].dl_id[0];

                _drop_locations[input_dl].reduced_load_coef -=
                    _drop_locations[min_dl].reduced_load_coef *
                    (_drop_locations[min_dl].rate_coef/
                     _drop_locations[input_dl].rate_coef);
                _drop_locations[input_dl].reduced_quality_score -=
                    _drop_locations[min_dl].reduced_quality_score *
                    (_drop_locations[min_dl].rate_coef/
                     _drop_locations[input_dl].rate_coef);
                _input_info[stream_name.as_string()].reduced_load_coef =
                        _drop_locations[input_dl].reduced_load_coef;
                _input_info[stream_name.as_string()].reduced_quality = 
                        _drop_locations[input_dl].reduced_quality_score;
            }
            else
            {
                DEBUG << "it is NOT a split one..";

                int input_dl = _input_info[stream_name.as_string()].dl_id[0];

                _input_info[stream_name.as_string()].reduced_load_coef =
                        _drop_locations[input_dl].reduced_load_coef;
                _input_info[stream_name.as_string()].reduced_quality =
                        _drop_locations[input_dl].reduced_quality_score;
                DEBUG << "reduced_load_coef = "
                      << _input_info[stream_name.as_string()].reduced_load_coef;
                DEBUG << "reduced_quality = "
                      << _input_info[stream_name.as_string()].reduced_quality;
            }
        }
    }
}

void NHLoadShedder::collect_dl_for_inputs() throw(AuroraException)
{
    DEBUG << "NHLoadShedder::collect_dl_for_inputs()";

    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        DEBUG << "Input stream: " << iter->first;

        if ((iter->second).dl_id.size() < 1)
        {
            WARN << "Invalid number of drop locations for stream "
                 << iter->first;
            Throw(AuroraException,
                  "Invalid number of drop locations for stream \""
                  + iter->first + "\".");
        }

        string stream_name = iter->first;
        CatalogStream *stream = _local_catalog->get_stream(Name(stream_name));

        _drop_locations[((iter->second).dl_id)[0]].input_stream = stream_name;
        _drop_locations[((iter->second).dl_id)[0]].rate_coef = 1.0;
        (iter->second).dl_subtree.push_back(((iter->second).dl_id)[0]);

        if ((iter->second).dl_id.size() == 1)
        {
            collect_dl_from_subtree(Name(stream_name),
                    _drop_locations[((iter->second).dl_id)[0]].dest_box_name,
                    1.0);
        }
        else // input is a split one
        {
            DEBUG << "Split input";

            vector<BoxPort> *sinks = stream->get_sink_port();
            assert(sinks);
            assert(sinks->size() > 1);

            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                DEBUG << "Sink " << j;
                int dl_id = get_drop_location_id(Name(stream_name),
                                ((*sinks)[j].get_port_box())->get_box_name(),
                                (*sinks)[j].get_port());
                assert(dl_id > 0);

                _drop_locations[dl_id].input_stream = stream_name;
                _drop_locations[dl_id].rate_coef = 1.0;
                (iter->second).dl_subtree.push_back(dl_id);

                collect_dl_from_subtree(Name(stream_name),
                                ((*sinks)[j].get_port_box())->get_box_name(),
                                1.0);
            }
        }

        sort_dl_subtree(Name(stream_name));

        if ((iter->second).reduced_load_coef > 0)
        {
            (iter->second).rate_bound = 1.0/((iter->second).reduced_load_coef);
            DEBUG << "Input " << iter->first << " rate_bound  " 
                  << (iter->second).rate_bound;
        }
        else
        {
            WARN << "Invalid reduced load coefficient.";
            Throw(AuroraException,
                  "Invalid reduced load coefficient.");
        }
    }
}

// When there is no additional local plan, and rates are feasible.
//
double NHLoadShedder::compute_FIT_score(FeasibleInputTable::RateMap rates)
                                                    throw(AuroraException)
{
    double total_score = 0;

    for (FeasibleInputTable::RateMapIter iter = rates.begin(); 
         iter != rates.end(); iter++)
    {
        if (_input_info.find(iter->first) == _input_info.end())
        {
            WARN << "Input information for stream \"" << iter->first
                 << "\" does not exist.";
            Throw(AuroraException,
                  "Input information for stream \"" + iter->first
                  + "\" does not exist.");
        }
        else 
        {
            total_score += iter->second *
                           (_input_info[iter->first].total_quality);
        }
    }

    return total_score;
}

vector<LSPlan::drop_box>::iterator NHLoadShedder::locate_drop(
                                        Name box_name,
                                        vector<LSPlan::drop_box> drops)
{
    vector<LSPlan::drop_box>::iterator iter;

    for (iter = drops.begin(); iter != drops.end(); iter++)
    {
        if (iter->box_name == box_name)
        {
            return iter;
        }
    }

    return iter;
}

// Merge-sort dl_subtree_sorted from all inputs into one sorted list.
//
void NHLoadShedder::determine_dl_sort_order()
{
    DEBUG << "NHLoadShedder::determine_dl_sort_order()";

    _dl_sort_order.clear();

    // where we are on each input's dl_subtree_sorted
    map<string, unsigned int > pointers; 

    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        pointers[iter->first] = 0;
    }

    while (1)
    {
        string selected_input = "";
        double selected_ratio;

        for (InputInfoMap::iterator iter = _input_info.begin();
             iter != _input_info.end(); iter++)
        {
            if (pointers[iter->first] < 
                (iter->second).dl_subtree_sorted.size())
            {
                if (selected_input == "")
                {
                    selected_input = iter->first;
                    DropLocation d;
                    d = _drop_locations[(iter->second).dl_subtree_sorted[
                                                    pointers[iter->first]]];
                    selected_ratio = d.reduced_quality_score/
                                     d.reduced_load_coef;
                }
                else
                {
                    DropLocation d;
                    d = _drop_locations[(iter->second).dl_subtree_sorted[
                                                    pointers[iter->first]]];
                    double ratio = d.reduced_quality_score/
                                   d.reduced_load_coef;
                    if (ratio < selected_ratio)
                    {
                        selected_input = iter->first;
                        selected_ratio = ratio;
                    }
                }
            }
        }

        if (selected_input == "") // nothing left to select
        {
            break;
        }

        _dl_sort_order.push_back(_input_info[selected_input].
                                 dl_subtree_sorted[pointers[selected_input]]);
        DEBUG << "DL: " << _input_info[selected_input].
                                 dl_subtree_sorted[pointers[selected_input]];
        pointers[selected_input] += 1;
    }
}

void NHLoadShedder::create_local_FIT()
{
    DEBUG << "NHLoadShedder::create_local_FIT()";

    // First, create a table with all possible input rate combinations.
    //
    unsigned int fit_size = 1;

    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        DEBUG << "Input stream: " << iter->first;

        (iter->second).num_points = (int(ceil((iter->second).rate_bound/
                                              (iter->second).spread)) + 1);

        DEBUG << "Number of points: " << (iter->second).num_points;

        fit_size *= (iter->second).num_points;
    }     

    DEBUG << "Local fit size = " << fit_size;

    _local_fit = FeasibleInputTable(fit_size);

    unsigned int repeat_count = fit_size;
    unsigned int times;

    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        repeat_count = repeat_count/((iter->second).num_points);    

        times = fit_size/repeat_count;

        unsigned int k = 0;

        double rate = (iter->second).rate_bound;

        //bool cycle = false;

        for (unsigned int i = 0; i < times; i++) 
        {
            for (unsigned int j = 0; j < repeat_count; j++)
            {
                DEBUG << k << ": " << iter->first << " = " << rate;
                (_local_fit._table[k]._rates)[iter->first] = rate; 
                ++k;
            }

            if (rate >= (iter->second).spread)
            {
                rate -= (iter->second).spread;
            }
            else if (abs(rate - 0) <= 0.00001) // practically zero
            {
                //cycle = true; // if more to repeat, cycle from the bound.
                rate = (iter->second).rate_bound;
            }
            else if ((rate > 0) && (rate < (iter->second).spread))
            {
                rate = 0;
            }
            else 
            {
                assert(false); // complain (never should get here)
            }
        }

        assert(k == fit_size);
    }

    // Next, eliminate the infeasible ones & enter scores for the feasible ones.
    // Also, compute additional local plan for an entry if feasible with
    // a plan, but not feasible without a plan.
    //
    vector<FeasibleInputTable::FIT_Entry>::iterator fit_iter = 
                                                _local_fit._table.begin();

    while (fit_iter != _local_fit._table.end())
    {
        DEBUG << fit_iter->as_string();

        double excess_load_w, excess_load;
    
        if (is_feasible_with_plan(fit_iter->_rates, &excess_load_w))
        {
            DEBUG << "is feasible with plan";
            if (is_feasible(fit_iter->_rates, &excess_load))
            {
                DEBUG << "is feasible without plan";

                fit_iter->_score = compute_FIT_score(fit_iter->_rates);
                fit_iter->_additional_plan.set(new LSPlan());

                DEBUG << fit_iter->as_string();
            }
            else // additional local plan is needed.
            {
                DEBUG << "Additional local plan is needed";
                DEBUG << "actual excess load = " << excess_load;

                fit_iter->_score = compute_FIT_score(fit_iter->_rates);

                vector<LSPlan::drop_box>::iterator drop_iter;
                vector<LSPlan::drop_box> drops;
                drops.clear();

                for (unsigned int j = 0; j < _dl_sort_order.size(); j++)
                {
                    int k = _dl_sort_order[j];
                    double rate = fit_iter->_rates[
                                            _drop_locations[k].input_stream];
                    double load_saved = rate * 
                                        _drop_locations[k].rate_coef * 
                                        _drop_locations[k].reduced_load_coef; 

                    DEBUG << "load that can be saved by " << k << " = "
                          << load_saved;

                    if (load_saved > excess_load)
                    {
                        // add fractional plan entry here and that's it.
                        //
                        double drop_rate = excess_load/load_saved;
                        if (drop_rate <= 0.01) // it's not worth it
                        {
                            excess_load = 0;
                            break;
                        }
                        LSPlan::drop_box d;
                        d.box_name = (string("drop_box_") + k);
                        d.box_type = _drop_locations[k].box_type;
                        d.node = _id.get_id();
                        BoxParameter::ParameterMap pm;
                        pm = *(_drop_locations[k].box_params.
                                        get_parameter_map_const());
                        ostringstream s;
                        s << drop_rate;
                        pm["drop_rate"] = s.str();
                        d.box_params.set_parameter_map(&pm);
                        d.in_stream_name = _drop_locations[k].in_stream_name;
                        d.dest_box_name = _drop_locations[k].dest_box_name;
                        d.dest_box_port = _drop_locations[k].dest_box_port;
                        d.dest_node = _id.get_id();
                        drops.push_back(d);
                        DEBUG << "Drop:" << d.as_string();

                        fit_iter->_score -=
                                    drop_rate * rate * 
                                    _drop_locations[k].rate_coef *
                                    _drop_locations[k].reduced_quality_score;

                        excess_load = 0;

                        break;
                    }
                    else 
                    {
                        // add full plan entry and continue adding.
                        //
                        double drop_rate = 1.0;
                        LSPlan::drop_box d;
                        d.box_name = (string("drop_box_") + k);
                        d.box_type = _drop_locations[k].box_type;
                        d.node = _id.get_id();
                        BoxParameter::ParameterMap pm;
                        pm = *(_drop_locations[k].box_params.
                                        get_parameter_map_const());
                        ostringstream s;
                        s << drop_rate;
                        pm["drop_rate"] = s.str();
                        d.box_params.set_parameter_map(&pm);
                        d.in_stream_name = _drop_locations[k].in_stream_name;
                        d.dest_box_name = _drop_locations[k].dest_box_name;
                        d.dest_box_port = _drop_locations[k].dest_box_port;
                        d.dest_node = _id.get_id();
                        drops.push_back(d);
                        DEBUG << "Drop:" << d.as_string();

                        fit_iter->_score -=
                                    drop_rate * rate * 
                                    _drop_locations[k].rate_coef *
                                    _drop_locations[k].reduced_quality_score;

                        excess_load -= load_saved;
                    }
                }

                fit_iter->_additional_plan.set(new LSPlan(drops));
            }

            ++fit_iter;
        }
        else
        {
            DEBUG << "is NOT feasible even with plan, erase..";
            fit_iter = _local_fit._table.erase(fit_iter);
        }

        if (fit_iter == _local_fit._table.end())
        {
            DEBUG << "No more entries to process..";
            break;
        }
    }
}

void NHLoadShedder::initial_prep()
{
    find_drop_locations();
    find_source_servers();
    find_sink_servers();
}

void NHLoadShedder::generate_local_FIT()
{
    DEBUG << "NHLoadShedder::generate_local_FIT()";

    //find_drop_locations();
    //find_source_servers();
    //find_sink_servers();

    compute_load_coefs();

    compute_input_info();

    compute_spreads();

    collect_dl_for_inputs();

    for (DropLocationMap::const_iterator iter = _drop_locations.begin();
         iter != _drop_locations.end(); iter++)
    {
        NOTICE << iter->first << " : " << iter->second;
    }

    determine_dl_sort_order();

    for (InputInfoMap::const_iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        DEBUG << iter->first << " : " << iter->second;
    }

    create_local_FIT();

    DEBUG << "Local FIT:";
    for (unsigned int i = 0; i < _local_fit._table.size(); i++)
    {
        DEBUG << _local_fit._table[i].as_string();
    }
}

// can we make the search more intelligent?
//
int NHLoadShedder::find_best_FIT(FeasibleInputTable::RateMap rates)
{
    int best_fit = -1;
    double max_score = 0;

    for (unsigned int i = 0; i < _fit._table.size(); i++)
    {
        FeasibleInputTable::RateMap fit_rates = _fit._table[i]._rates;

        bool found = true;

        for (FeasibleInputTable::RateMapIter iter = fit_rates.begin(); 
              iter != fit_rates.end(); iter++)
        {
            assert(rates.find(iter->first) != rates.end());

            if (iter->second > rates[iter->first]) // I can never adopt this.
            {
                found = false;
                break;
            }
        }

        if (found) // found a point within the plan-feasible region
        {
            if (_fit._table[i]._score > max_score) // see if it is better
            {
                best_fit = i;
                max_score = _fit._table[i]._score;
            }
        }
    }

    return best_fit;
}

// Apply the additional local plan at the given FIT entry, and also 
// cut the current rates down to the rates at the given FIT netry
//
void NHLoadShedder::apply_FIT(FeasibleInputTable::RateMap current_rates, 
                              int fit_index)
{
    // If fit_index == -1, then simply remove all the drops.
    //
    if (fit_index == -1)
    {
        assert(_current_fit_index != -1);
        NOTICE << "We will remove all the drops.";

        for (map<string, double>::iterator iter = _current_drop_boxes.begin();
             iter != _current_drop_boxes.end(); iter++)
        {
            DEBUG << "Disconnecting drop box: " << iter->first;

            string xml;
            xml = "<disconnect stream=\"";
            xml += iter->first;
            xml += "_out_stream";
            xml += "\" />";

            NOTICE << "xml=\n" << xml;

            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                WARN << "RPCFault";
            }
        }

        _current_drop_boxes.clear();

        return;
    }

    vector<LSPlan::drop_box> cur_dip, new_dip;

    if (_current_fit_index == -1)
    {
        cur_dip.clear();
    }
    else
    {
        cur_dip = (_fit._table[_current_fit_index]._additional_plan)
                                                ->_drop_insertion_plan;
    }

    new_dip = (_fit._table[fit_index]._additional_plan)->_drop_insertion_plan;

    // 1. Handle drops for the additional local plan.
    //
    if (fit_index != _current_fit_index)
    {
        NOTICE << "We will apply a different FIT entry.";

        // 1.1. Make sure all new drops are there with correct drop rates.
        //
        for (unsigned int i = 0; i < new_dip.size(); i++)
        {
            DEBUG << "Add drop from FIT entry.";

            if (_current_drop_boxes.find(new_dip[i].box_name.as_string()) 
                != _current_drop_boxes.end())
            {
                DEBUG << "Drop box " << new_dip[i].box_name.as_string() 
                      << " is already there, just adjust its drop rate.";

                double new_drop_rate = atof(
                    (new_dip[i].box_params.get_parameter("drop_rate")).c_str());

                double current_drop_rate = 
                    _current_drop_boxes[new_dip[i].box_name.as_string()];

                if (abs(new_drop_rate - current_drop_rate) >= DROP_RATE_LIMIT)
                                                            // not worth it
                {
                    DEBUG << "Modify drop rate from " << current_drop_rate 
                          << " to " << new_drop_rate;

                    string xml;
                    xml = "<modify box=\"";
                    xml += new_dip[i].box_name.as_string();
                    xml += "\" >\n";
                    xml += " ";
                    BoxParameter bp = new_dip[i].box_params;
                    ostringstream s;
                    s << new_drop_rate;
                    bp.set_parameter("drop_rate", s.str());
                    xml += bp.get_parameter_xml();
                    xml += "\n";
                    xml += "</modify>";

                    NOTICE << "xml=\n" << xml;

                    RPC<void> result = _qp.add_xml_string(xml);

                    DEBUG << "result : " << result.stat();
                    if (!result.stat())
                    {
                        WARN << "RPCFault";
                    }

                    _current_drop_boxes[new_dip[i].box_name.as_string()] = 
                                                            new_drop_rate;
                }
                else // it's not worth it
                {
                    DEBUG << "Change in drop rate is too small, ignore it..";
                    continue;
                }
            }
            else
            {
                DEBUG << "Drop box " << new_dip[i].box_name.as_string() 
                      << " is new.";

                double new_drop_rate = atof(
                   (new_dip[i].box_params.get_parameter("drop_rate")).c_str());

                if (new_drop_rate <= DROP_RATE_LIMIT) // it's not worth it
                {
                    DEBUG << "The required drop rate is too small, ignore it..";
                    continue;
                }

                string xml;
                xml = "<connect box=\"";
                xml += new_dip[i].dest_box_name.as_string();
                xml += "\" port=\"";
                xml += new_dip[i].dest_box_port;
                xml += "\" node=\"";
                xml += new_dip[i].dest_node;
                xml += "\">\n";
                xml += "  <box name=\"";
                xml += new_dip[i].box_name.as_string();
                xml += "\" type=\"";
                xml += new_dip[i].box_type;
                xml += "\">\n";
                xml += "    <in  stream=\"";
                xml += new_dip[i].in_stream_name;
                xml += "\" />\n";
                xml += "    <out stream=\"";
                xml += new_dip[i].box_name.as_string();
                xml += "_out_stream";
                xml += "\" />\n";
                xml += "   ";
                xml += new_dip[i].box_params.get_parameter_xml();
                xml += "\n";
                xml += "  </box>\n";
                xml += "</connect>";

                NOTICE << "xml=\n" << xml;

                RPC<void> result = _qp.add_xml_string(xml);

                DEBUG << "result : " << result.stat();
                if (!result.stat())
                {
                    WARN << "RPCFault";
                }

                _current_drop_boxes[new_dip[i].box_name.as_string()] = 
                                                        new_drop_rate;
            }
        }

        // 1.2. Make sure there are no redundant drops from the old dip.
        //
        for (unsigned int i = 0; i < cur_dip.size(); i++)
        {
            vector<LSPlan::drop_box>::iterator iter = (_fit._table[fit_index].
                                _additional_plan)->locate(cur_dip[i].box_name);
            if (iter == ((_fit._table[fit_index]._additional_plan)
                                    ->_drop_insertion_plan).end())
            {
                DEBUG << "Drop box " << cur_dip[i].box_name.as_string()
                      << " must be disconnected.";
            
                string xml;
                xml = "<disconnect stream=\"";
                xml += cur_dip[i].box_name.as_string();
                xml += "_out_stream";
                xml += "\" />";

                NOTICE << "xml=\n" << xml;

                RPC<void> result = _qp.add_xml_string(xml);

                DEBUG << "result : " << result;
                if (!result.stat())
                {
                    WARN << "RPCFault";
                }
            }
        }
    }
    else
    {
        DEBUG << "We will apply the same FIT entry,"
              << " i.e. additional local plan doesn't change.";
    }

    // 2. Handle drops that should be placed at the inputs to adjust rates.
    //
    FeasibleInputTable::RateMap fit_rates = _fit._table[fit_index]._rates;

    for (FeasibleInputTable::RateMapIter iter = fit_rates.begin(); 
         iter != fit_rates.end(); iter++)
    {
        NOTICE << "CURRENT RATE FOR " << iter->first << " = " 
               << current_rates[iter->first];

        int dl_id = _input_info[iter->first].dl_id[0];
        Name box_name = (string("drop_box_") + dl_id);

        if (current_rates[iter->first] > iter->second)
        {
            NOTICE << "We will have to cut down from " << iter->first;

            double new_drop_rate = 1 - iter->second/current_rates[iter->first];

            if (new_drop_rate <= DROP_RATE_LIMIT) // it's not worth it
            {
                DEBUG << "The required drop rate is too small, ignore it..";
                continue;
            }

            vector<LSPlan::drop_box>::iterator iter2 = 
                (_fit._table[fit_index]._additional_plan)->locate(box_name);

            // I think an additional plan never contains drops at inputs.
            // But I am not completely sure if this is the case ???
            // If I am wrong, the following if block must be added back in.
            //
            assert(iter2 == ((_fit._table[fit_index]._additional_plan)
                                    ->_drop_insertion_plan).end());

            /*
            if (iter2 != ((_fit._table[fit_index]._additional_plan)
                                    ->_drop_insertion_plan).end()) // ???
            {
                DEBUG << "Drop box " << box_name.as_string() 
                      << " is already there, just adjust its drop rate.";

                BoxParameter bp = iter2->box_params;
                string dr = bp.get_parameter("drop_rate");
                double current_drop_rate = atof(dr.c_str());

                if (abs(new_drop_rate - current_drop_rate) < DROP_RATE_LIMIT) 
                                                                // not worth
                {
                    DEBUG << "Change in drop rate is too small, ignore it..";
                    continue;
                }

                DEBUG << "Modify drop rate from " << current_drop_rate
                      << " to " << new_drop_rate;

                ostringstream s;
                s << new_drop_rate;
                bp.set_parameter("drop_rate", s.str());
                string xml;
                xml = "<modify box=\"";
                xml += box_name.as_string();
                xml += "\" >\n";
                xml += " ";
                xml += bp.get_parameter_xml();
                xml += "\n";
                xml += "</modify>";

                NOTICE << "xml=\n" << xml;

                RPC<void> result = _qp.add_xml_string(xml);

                DEBUG << "result : " << result.stat();
                if (!result.stat())
                {
                    WARN << "RPCFault";
                }

                _current_drop_boxes[box_name.as_string()] = new_drop_rate;
            }
            */
            if (_current_drop_boxes.find(box_name.as_string()) !=
                _current_drop_boxes.end())
            {
                DEBUG << "Drop box " << box_name.as_string() 
                      << " is already there, just adjust its drop rate.";

                BoxParameter::ParameterMap pm;
                pm = *(_drop_locations[dl_id].box_params.
                                    get_parameter_map_const());
                double current_drop_rate = 
                            _current_drop_boxes[box_name.as_string()];

                if (abs(new_drop_rate - current_drop_rate) < DROP_RATE_LIMIT) 
                                                                // not worth
                {
                    DEBUG << "Change in drop rate is too small, ignore it..";
                    continue;
                }

                DEBUG << "Modify drop rate from " << current_drop_rate
                      << " to " << new_drop_rate;

                ostringstream s;
                s << new_drop_rate;
                pm["drop_rate"] = s.str();
                BoxParameter bp;
                bp.set_parameter_map(&pm);
                string xml;
                xml = "<modify box=\"";
                xml += box_name.as_string();
                xml += "\" >\n";
                xml += " ";
                xml += bp.get_parameter_xml();
                xml += "\n";
                xml += "</modify>";

                NOTICE << "xml=\n" << xml;

                RPC<void> result = _qp.add_xml_string(xml);

                DEBUG << "result : " << result.stat();
                if (!result.stat())
                {
                    WARN << "RPCFault";
                }

                _current_drop_boxes[box_name.as_string()] = new_drop_rate;
            }
            else
            {
                DEBUG << "Drop box " << box_name.as_string() 
                      << " is new.";

                string xml;
                xml = "<connect box=\"";
                xml += _drop_locations[dl_id].dest_box_name.as_string();
                xml += "\" port=\"";
                xml += _drop_locations[dl_id].dest_box_port;
                xml += "\" node=\"";
                xml += _id.get_id();;
                xml += "\">\n";
                xml += "  <box name=\"";
                xml += box_name.as_string();
                xml += "\" type=\"";
                xml += _drop_locations[dl_id].box_type;
                xml += "\">\n";
                xml += "    <in  stream=\"";
                xml += _drop_locations[dl_id].in_stream_name;
                xml += "\" />\n";
                xml += "    <out stream=\"";
                xml += box_name.as_string();
                xml += "_out_stream";
                xml += "\" />\n";
                xml += "   ";
                BoxParameter::ParameterMap pm;
                pm = *(_drop_locations[dl_id].box_params.
                                    get_parameter_map_const());
                ostringstream s;
                s << new_drop_rate;
                pm["drop_rate"] = s.str();
                BoxParameter bp;
                bp.set_parameter_map(&pm);
                xml += bp.get_parameter_xml();
                xml += "\n";
                xml += "  </box>\n";
                xml += "</connect>";

                NOTICE << "xml=\n" << xml;

                RPC<void> result = _qp.add_xml_string(xml);

                DEBUG << "result : " << result.stat();
                if (!result.stat())
                {
                    WARN << "RPCFault";
                }

                _current_drop_boxes[box_name.as_string()] = new_drop_rate;
            }
        }
        else
        {
            NOTICE << "No need to cut down from " << iter->first;

            // Remove any drops that may be there from the previous dip.
            //
            if (_current_drop_boxes.find(box_name.as_string()) !=
                _current_drop_boxes.end())
            {
                DEBUG << "Drop box " << box_name.as_string()
                      << " must be disconnected.";
            
                string xml;
                xml = "<disconnect stream=\"";
                xml += box_name.as_string();
                xml += "_out_stream";
                xml += "\" />";

                NOTICE << "xml=\n" << xml;

                RPC<void> result = _qp.add_xml_string(xml);

                DEBUG << "result : " << result;
                if (!result.stat())
                {
                    WARN << "RPCFault";
                }
            }
        }
    }
}

bool NHLoadShedder::is_overloaded()
{
    FeasibleInputTable::RateMap current_rates;

    // Read the latest stream rate stats.
    //
    read_local_statistics();
    DEBUG << _statistics.as_string();

    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        string stream_name = iter->first;

        double avg_rate; 
        //vector<double> stream_rates;

        try
        {
            const FixLenTimeSeries &f = _statistics.get_statistics(
                                                    "StreamRate", stream_name);
            try
            {
                avg_rate = f.get_average();
                /*
                //stream_rates = f.get_samples();
                avg_rate = f.get_sample();
                if (avg_rate <= 0)
                {
                    avg_rate = f.get_average();
                }
                */
            }
            catch(...)
            {
                WARN << "Could not get \"Average StreamRate\" for stream "
                     << stream_name;
                return false;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return false;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return false;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return false;
        }

        current_rates[stream_name] = avg_rate;
    }

    double excess_load;

    if (!(is_feasible(current_rates, &excess_load)))
    {
        NOTICE << "Overload detected at node " << _id.get_id();
        return true;
    }

    return false;
}

void NHLoadShedder::shed_load() throw(AuroraException)
{
    DEBUG << "NHLoadShedder in shed_load...";

    FeasibleInputTable::RateMap current_rates;

    // Read the latest stream rate stats.
    //
    read_local_statistics();
    NOTICE << _statistics.as_string();

    bool steady = true;

    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        string stream_name = iter->first;

        double avg_rate;
        double stddev_rate;
        //vector<double> stream_rates;

        try
        {
            const FixLenTimeSeries &f = _statistics.get_statistics(
                                                    "StreamRate", stream_name);
            try
            {
                avg_rate = f.get_average();
                stddev_rate = f.get_stddev();
                /*
                //stream_rates = f.get_samples();
                avg_rate = f.get_sample();
                if (avg_rate <= 0)
                {
                    avg_rate = f.get_average();
                }
                */
            }
            catch(...)
            {
                WARN << "Could not get \"Average StreamRate\" for stream "
                     << stream_name;
                return;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return;
        }

        current_rates[stream_name] = avg_rate;

        if (stddev_rate > STDDEV_LIMIT)
        {
            NOTICE << "Standard deviation for rate on " << stream_name 
                   << " = " << stddev_rate;
            steady = false;
            break;
        }
    }

    double excess_load;

    if (!steady)
    {
        NOTICE << "We will wait until we observe steady rates";
        steady = true;
    }
    else if (is_feasible(current_rates, &excess_load))
    {
        DEBUG << "Good news! There is no overload at node " 
               << _id.get_id();

        if (excess_load < 0)
        {
            DEBUG << "Need to check if we can remove any drops..";
        }
    }
    else
    {
        NOTICE << "Bad news.. Node " << _id.get_id() 
               << " has an excess load of " << excess_load;

        int fit_index = find_best_FIT(current_rates);

        if (fit_index >= 0)
        {
            NOTICE << "Apply FIT " << fit_index;
            NOTICE << _fit._table[fit_index].as_string();
            apply_FIT(current_rates, fit_index);
        }
        else // should always return a valid fit index
        {
            WARN << "Negative fit_index.";
            Throw(AuroraException,
                  "Negative fit_index.");
        }

        NOTICE << "Load shedding has been completed at node " << _id.get_id();
        NOTICE << "Will wait an extra " << LS_STABLE 
               << "msecs for things to stabilize.."; 
        Thread::sleep(Time::msecs(LS_STABLE));

        _done = true;
        NOTICE << "Node " << _id.get_id() << " is now done.";

        read_local_statistics();
        NOTICE << _statistics.as_string();

        // Record throughput.
        //
        double total_tput = 0;
        Catalog::StreamMap *boxes = _local_catalog->get_stream_map();
        for (Catalog::StreamMap::iterator iter = boxes->begin(); 
             iter != boxes->end(); ++iter)
        {
            CatalogStream& stream = iter->second;
            if (!(stream.is_output_stream()))
            {
                continue;
            }

            string stream_name = (iter->first).as_string();

            double avg_tput; 

            try
            {
                const FixLenTimeSeries &f = _statistics.get_statistics(
                                                    "TupleNum", stream_name);
                try
                {
                    avg_tput = f.get_average();
                }
                catch(...)
                {
                    WARN << "Could not get \"Average TupleNum\" for stream "
                         << stream_name;
                    return;
                }
            }
            catch(Statistics::error e)
            {
                WARN << "Statistics::Error " << (int)e;
                WARN << "Could not read \"TupleNum\" for stream " 
                     << stream_name;
                return;
            }
            catch(TSStats::error e)
            {
                WARN << "TSStats::Error " << (int)e;
                WARN << "Could not read \"TupleNum\" for stream " 
                     << stream_name;
                return;
            }
            catch(...)
            {
                WARN << "Unknown stats exception";
                WARN << "Could not read \"TupleNum\" for stream " 
                     << stream_name;
                return;
            }

            total_tput += avg_tput;
        }

        *_stats_file << total_tput << endl;

        return;
    }

    // periodically check.
    //
    (new CallbackTimer(_my_loop, wrap(this,&NHLoadShedder::shed_load)))
         ->arm(Time::now() + Time::msecs(LS_PERIOD));
}

void NHLoadShedder::shed_load_continuous() throw(AuroraException)
{
    DEBUG << "NHLoadShedder in shed_load_continuous...";

    FeasibleInputTable::RateMap current_rates;

    // Read the latest stream rate stats.
    //
    read_local_statistics();
    NOTICE << _statistics.as_string();

    // Let's also read the latest catalog.
    //
    read_local_catalog();

    bool steady = true;

    for (InputInfoMap::iterator iter = _input_info.begin();
         iter != _input_info.end(); iter++)
    {
        string stream_name = iter->first;

        double avg_rate;
        double stddev_rate;

        try
        {
            const FixLenTimeSeries &f = _statistics.get_statistics(
                                                    "StreamRate", stream_name);
            try
            {
                avg_rate = f.get_average();
                stddev_rate = f.get_stddev();
            }
            catch(...)
            {
                WARN << "Could not get \"Average StreamRate\" for stream "
                     << stream_name;
                return;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"StreamRate\" for stream " << stream_name;
            return;
        }

        current_rates[stream_name] = avg_rate;

        if (stddev_rate > STDDEV_LIMIT)
        {
            NOTICE << "Stddev for rate on " << stream_name 
                   << " = " << stddev_rate;
            steady = false;
            //break;
        }
    }

    double excess_load;

    /*
    if (is_root_node() && (!steady)) // we require steady rates only at root.
    {
        NOTICE << "We will wait until we observe steady rates.";
        steady = true;
    }
    else if (is_feasible(current_rates, &excess_load))
    */
    if (is_feasible(current_rates, &excess_load))
    {
        NOTICE << "FEASIBLE";
        if (_current_fit_index != -1)
        {
            apply_FIT(current_rates, -1); // remove the drops
            _current_fit_index = -1;
        }
    }
    else
    {
        NOTICE << "NOT FEASIBLE";
        /*
        if (!_measuring)
        {
            measure_local();

            // Also, tell all downstream nodes to start measuring if they
            // are not already doing so.
            //
            for (unsigned int i = 0; i < _sink_servers.size(); i++)
            {
                Remote<NHLoadShedder> remote_nh_ls = 
                            nh_ls_for(MedusaID(to_string(_sink_servers[i])));
                if (remote_nh_ls)
                {
                    AsyncRPC<void> result = remote_nh_ls.measure_remote();
                    NOTICE << "result : " << result.stat();
                    if (!(result.stat()))
                    {
                        WARN << "measure_remote() at node " << _sink_servers[i] 
                             << " failed!";
                        continue;
                    }
                }
                else
                {
                    WARN << "No NHLoadShedder for " << _sink_servers[i];
                }
            }
        }
        */

        NOTICE << "We will apply the best FIT."; 

        int fit_index = find_best_FIT(current_rates);

        if (fit_index >= 0)
        {
            NOTICE << "Apply FIT " << fit_index;
            NOTICE << _fit._table[fit_index].as_string();
            apply_FIT(current_rates, fit_index);
            _current_fit_index = fit_index;
        }
        else // should return a positive fit index
        {
            NOTICE << "No best FIT.";
            //Throw(AuroraException, "Negative fit_index.");
        }
    }

    // periodically check.
    //
    (new CallbackTimer(_my_loop, 
                       wrap(this,&NHLoadShedder::shed_load_continuous)))
         ->arm(Time::now() + Time::msecs(LS_PERIOD));
}

// For now, can assume no node splits (i.e., just flat chains) and
// each server has at most 1 parent server (i.e, _source_servers.size() == 1).
//
void NHLoadShedder::propagate_FIT_to_parent()
{
    for (unsigned int i = 0; i < _source_servers.size(); i++)
    {
        Remote<NHLoadShedder> remote_nh_ls = 
                            nh_ls_for(MedusaID(to_string(_source_servers[i])));
        if (remote_nh_ls)
        {
            NOTICE << "Sending FIT to the parent..";
            AsyncRPC<void> result = remote_nh_ls.send_FIT(_addr, _fit);
            NOTICE << "result : " << result.stat();
            if (!(result.stat()))
            {
                WARN << "Sending FIT to node " << _source_servers[i] 
                     << " failed!";
                continue;
            }
        }
        else
        {
            WARN << "No NHLoadShedder for " << _source_servers[i];
        }
    }
}

// Assumptions:
// 1. Each box has one output stream (which can split), 
// i.e., no case-like filter in the query network.
// 2. An output stream never splits into two different downstream servers.
//
double NHLoadShedder::compute_merged_rate_down(Name box_name,
                                            FeasibleInputTable::RateMap rates,
                                            ptr<LSPlan> plan)
{
    DEBUG << "NHLoadShedder::compute_merged_rate_down " << box_name.as_string();

    CatalogBox *box = _local_catalog->get_box(box_name);
    assert(box->get_box_node() == _id.get_id());

    CatalogStream::StreamStar *out_streams = box->get_box_out();
    assert(out_streams->size() == 1); // we never use case-like filters.

    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];
        string selectivity_id = box_name.as_string() + " : " +
                                out_stream->get_stream_source()->get_port();
        double selectivity;
        try
        {
            const FixLenTimeSeries &f = _statistics.get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
                DEBUG << "selectivity for " << box_name.as_string()
                      << " = " << selectivity;
            }
            catch(...)
            {
                WARN << "Could not get \"Average Selectivity\" for "
                     << selectivity_id;
                return -1;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return -1;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return -1;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return -1;
        }

        // If reached to an output stream, return what you found.
        //
        if (rates.find(out_stream->get_stream_name_string()) != rates.end())
        { 
            DEBUG << "returning "
                  << (rates[out_stream->get_stream_name_string()]/selectivity);
            return (rates[out_stream->get_stream_name_string()]/selectivity);
        }
        else if (out_stream->is_output_stream())
        {
            DEBUG << "returning -1";
            return -1;
        }

        double r, max = -1;
        vector<double> temp_rates;
        unsigned int index;

        vector<BoxPort> *sinks = out_stream->get_sink_port();

        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            CatalogBox *next_box = (*sinks)[j].get_port_box();

            if (next_box->get_box_node() != _id.get_id()) // node sink
            {
                DEBUG << "returning -1";
                return -1;
            }
            else 
            {
                r = compute_merged_rate_down(next_box->get_box_name(), 
                                             rates, plan);
                temp_rates.push_back(r);
                if (r > max)
                {
                    max = r;
                    index = j;
                }
            }
        }

        if (max > -1) // update the plan
        {
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                if (j == index)
                {
                    continue;
                }

                int k = get_drop_location_id(
                                     out_stream->get_stream_name(), 
                                     (*sinks)[j].get_port_box()->get_box_name(),
                                     (*sinks)[j].get_port());

                double drop_rate = 1.0 - temp_rates[j]/max;
                if (drop_rate <= 0.01) // it's not worth it
                {
                    continue;
                }

                LSPlan::drop_box d;
                d.box_name = (string("drop_box_") + k);
                d.box_type = _drop_locations[k].box_type;
                d.node = _id.get_id();
                BoxParameter::ParameterMap pm;
                pm = *(_drop_locations[k].box_params.get_parameter_map_const());
                ostringstream s;
                s << drop_rate;
                pm["drop_rate"] = s.str();
                d.box_params.set_parameter_map(&pm);
                d.in_stream_name = _drop_locations[k].in_stream_name;
                d.dest_box_name = _drop_locations[k].dest_box_name;
                d.dest_box_port = _drop_locations[k].dest_box_port;
                d.dest_node = _id.get_id();
                (plan->_drop_insertion_plan).push_back(d);
            }

            DEBUG << "returning " << (max/selectivity);
            return (max/selectivity);
        }
    }

    DEBUG << "returning -1";
    return -1;
}

double NHLoadShedder::compute_merged_rate(string stream_name, 
                                          FeasibleInputTable::RateMap rates,
                                          ptr<LSPlan> plan)
{
    // if there is no link, then we will return -1.
    double r, max = -1;
    vector<double> temp_rates;
    unsigned int index;

    CatalogStream *stream = _local_catalog->get_stream(Name(stream_name));
    
    vector<BoxPort> *sinks = stream->get_sink_port();
    for (unsigned int i = 0; i < sinks->size(); i++)
    {
        r = compute_merged_rate_down(
                                ((*sinks)[i].get_port_box())->get_box_name(), 
                                rates, plan);
        temp_rates.push_back(r);
        if (r > max)
        {
            max = r;
            index = i;
        }
    }

    if (max > -1) // update the plan
    {
        for (unsigned int i = 0; i < sinks->size(); i++)
        {
            if (i == index)
            {
                continue;
            }

            int k = get_drop_location_id(
                                     Name(stream_name), 
                                     (*sinks)[i].get_port_box()->get_box_name(),
                                     (*sinks)[i].get_port());

            double drop_rate = 1.0 - temp_rates[i]/max;
            if (drop_rate <= 0.01) // it's not worth it
            {
                continue;
            }

            LSPlan::drop_box d;
            d.box_name = (string("drop_box_") + k);
            d.box_type = _drop_locations[k].box_type;
            d.node = _id.get_id();
            BoxParameter::ParameterMap pm;
            pm = *(_drop_locations[k].box_params.get_parameter_map_const());
            ostringstream s;
            s << drop_rate;
            pm["drop_rate"] = s.str();
            d.box_params.set_parameter_map(&pm);
            d.in_stream_name = _drop_locations[k].in_stream_name;
            d.dest_box_name = _drop_locations[k].dest_box_name;
            d.dest_box_port = _drop_locations[k].dest_box_port;
            d.dest_node = _id.get_id();
            (plan->_drop_insertion_plan).push_back(d);
        }
    }

    return max;
}

FeasibleInputTable::FIT_Entry NHLoadShedder::merge_entry(
                                            FeasibleInputTable::FIT_Entry e1,
                                            FeasibleInputTable::FIT_Entry e2)
{
    DEBUG << "will merge the following two:";
    DEBUG << e1.as_string();
    DEBUG << e2.as_string();

    FeasibleInputTable::FIT_Entry result;

    result._rates.insert(e1._rates.begin(), e1._rates.end());
    result._rates.insert(e2._rates.begin(), e2._rates.end());

    result._score = e1._score + e2._score;

    result._additional_plan.set(
                    new LSPlan((e1._additional_plan)->_drop_insertion_plan));
    ((result._additional_plan)->_drop_insertion_plan).insert(
                    ((result._additional_plan)->_drop_insertion_plan).end(), 
                    ((e2._additional_plan)->_drop_insertion_plan).begin(),
                    ((e2._additional_plan)->_drop_insertion_plan).end());

    DEBUG << "resulting FIT entry:";
    DEBUG << result.as_string();

    return result;
}

// merge the given fit into _fit.
//
void NHLoadShedder::merge_child(FeasibleInputTable fit)
{
    vector<FeasibleInputTable::FIT_Entry> child_fit = fit._table;        

    vector<FeasibleInputTable::FIT_Entry> current_fit = _fit._table;

    if (_fit._table.empty())
    {
        DEBUG << "empty, just copy from child";
        _fit._table = child_fit;
        return;
    }

    vector<FeasibleInputTable::FIT_Entry> merged_fit;

    for (unsigned int i = 0; i < current_fit.size(); i++)
    {
        FeasibleInputTable::FIT_Entry current_entry = current_fit[i];

        for (unsigned int j = 0; j < child_fit.size(); j++)
        {
            merged_fit.push_back(merge_entry(current_fit[i], child_fit[j]));
        }
    }

    _fit._table = merged_fit;
}

// For now, let's allow multiple children, but they have no shared paths
// so that we can merge them separately, and then simply take a 
// cross product at the inputs.
//
void NHLoadShedder::merge_FIT_from_children()
{
    DEBUG << "NHLoadShedder::merge_FIT_from_children()";

    map<InetAddress, FeasibleInputTable> children;

    map<InetAddress, FeasibleInputTable>::iterator iter;
    for (iter = _children_fit.begin(); iter != _children_fit.end(); iter++)
    {
        DEBUG << "merging child " << iter->first;

        vector<FeasibleInputTable::FIT_Entry> fit = (iter->second)._table;

        for (unsigned int i = 0; i < fit.size(); i++)
        {
            double score = fit[i]._score;
            ptr<LSPlan> plan;
            plan.set(new LSPlan()); // initialization

            FeasibleInputTable::RateMap rates;

            for (InputInfoMap::iterator iter2 = _input_info.begin();
                 iter2 != _input_info.end(); iter2++)
            {
                double rate = compute_merged_rate(iter2->first,
                                                  fit[i]._rates,
                                                  plan);
                DEBUG << "merged rate = " << rate;
                if (rate >= 0)
                {
                    rates[iter2->first] = rate;
                }
            }

            FeasibleInputTable::FIT_Entry entry(rates, score, plan);

            DEBUG << "New entry: " << entry.as_string();

            children[iter->first]._table.push_back(entry);
        }
    }

    DEBUG << "cross product children";

    // cross product children.
    //
    _fit.clear();
    for (iter = children.begin(); iter != children.end(); iter++)
    {
        merge_child(iter->second);
    }

    DEBUG << "eliminate infeasible ones";

    // eliminate infeasible ones.
    //
    double excess;
    vector<FeasibleInputTable::FIT_Entry>::iterator fit_iter = 
                                                _fit._table.begin();
    while (fit_iter != _fit._table.end())
    {
        if (is_feasible(fit_iter->_rates, &excess))
        {
            ++fit_iter;
        }
        else
        {
            fit_iter = _fit._table.erase(fit_iter);
        }
    }

    DEBUG << "Merged FIT:";
    for (unsigned int i = 0; i < _fit._table.size(); i++)
    {
        DEBUG << _fit._table[i].as_string();
    }
}

BOREALIS_NAMESPACE_END

//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_NHLoadShedder.h"
#undef NMSTL_RPC_DEFINE
//---

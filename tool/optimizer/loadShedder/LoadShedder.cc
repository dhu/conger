#include "LoadShedder.h"
#include "MedusaClient.h"
#include "LocalLoadShedder.h"
#include "rpc_LocalLoadShedder.h"
#include <NMSTL/rpc>

BOREALIS_NAMESPACE_BEGIN

LoadShedder::LoadShedder(RegionalCatalog &catalog,
                         map<string, Statistics > &statistics,
                         IOEventLoop *loop,
                         int load_shedding_quality_metric,
                         bool active) :
                         _catalog(catalog),
                         _statistics(statistics)
{
    _loop = loop;

    _quality_metric = load_shedding_quality_metric;

    _no_drops = true;
    _active = active;
    _ls_level = 0; // default
    
    _drop_locations.clear();
    _dl_prefixes.clear();
    _original_solution_variables.clear();
    
    _load_shedder_nodes.clear();
    _ignore_nodes.clear();
    _overloaded_nodes.clear();
    
    _input_streams.clear();
    _stream_rates.clear();
    _input_rates.clear();

    _load_factor_matrix.clear();
    _load_factor_matrix2.clear();
    _rate_factor_matrix.clear();

    _output_cardinality.clear();
    _output_cardinality2.clear();
    _output_total_selectivity.clear();
    _output_input_mapping.clear();
    _tuple_delivery.clear();

    _node_input_load_coefs.clear();
    _node_input_rate_coefs.clear();
    //_node_input_rates.clear();

    _node_plans.clear();
    _additional_plans.clear();
    _current_plan_id = DEFAULT_PLAN_ID;
    _precomputed_plan_ids.clear();
    _plan_scores.clear();

    // FIT-based stuff
    //
    _input_info_pernode.clear();
    _drop_locations_pernode.clear();
    _source_servers_pernode.clear();
    _sink_servers_pernode.clear();
    _node_fit.clear();
    _dl_sort_order.clear();
    _current_fit_index.clear();
    _current_drop_boxes.clear();
    _max_rates.clear();
}

LoadShedder::LoadShedder(RegionalCatalog &catalog,
                         map<string, Statistics > &statistics,
                         IOEventLoop *loop,
                         int load_shedding_quality_metric,
                         int offline_duration,
                         int percent_fit_error,
                         bool active) :
                         _catalog(catalog), 
                         _statistics(statistics)
{
    _loop = loop;

    _quality_metric = load_shedding_quality_metric;
    _offline_duration = offline_duration;
    _percent_fit_error = percent_fit_error;

    _no_drops = true;
    _active = active;
    _ls_level = 0; // default
    
    _drop_locations.clear();
    _dl_prefixes.clear();
    _original_solution_variables.clear();
    
    _load_shedder_nodes.clear();
    _ignore_nodes.clear();
    _overloaded_nodes.clear();
    
    _input_streams.clear();
    _stream_rates.clear();
    _input_rates.clear();

    _load_factor_matrix.clear();
    _load_factor_matrix2.clear();
    _rate_factor_matrix.clear();

    _output_cardinality.clear();
    _output_cardinality2.clear();
    _output_total_selectivity.clear();
    _output_input_mapping.clear();
    _tuple_delivery.clear();

    _node_input_load_coefs.clear();
    _node_input_rate_coefs.clear();
    //_node_input_rates.clear();

    _node_plans.clear();
    _additional_plans.clear();
    _current_plan_id = DEFAULT_PLAN_ID;
    _precomputed_plan_ids.clear();
    _plan_scores.clear();

    // FIT-based stuff
    //
    _input_info_pernode.clear();
    _drop_locations_pernode.clear();
    _source_servers_pernode.clear();
    _sink_servers_pernode.clear();
    _node_fit.clear();
    _dl_sort_order.clear();
    _current_fit_index.clear();
    _current_drop_boxes.clear();
    _max_rates.clear();
}

LoadShedder::~LoadShedder()
{
    _drop_locations.clear();
    _dl_prefixes.clear();
    _original_solution_variables.clear();

    _load_shedder_nodes.clear();
    _ignore_nodes.clear();
    _overloaded_nodes.clear();

    _input_streams.clear();
    _stream_rates.clear();
    _input_rates.clear();

    _load_factor_matrix.clear();
    _load_factor_matrix2.clear();
    _rate_factor_matrix.clear();

    _output_cardinality.clear();
    _output_cardinality2.clear();
    _output_total_selectivity.clear();
    _output_input_mapping.clear();
    _tuple_delivery.clear();

    _node_input_load_coefs.clear();
    _node_input_rate_coefs.clear();
    //_node_input_rates.clear();

    _node_plans.clear();
    _additional_plans.clear();
    _precomputed_plan_ids.clear();
    _plan_scores.clear();

    if (_lp)
    {
        lpx_delete_prob(_lp);
    }
}

void LoadShedder::activate()
{
    _active = true;
}

void LoadShedder::de_activate()
{
    _active = false;
}

bool LoadShedder::get_status()
{
    return _active;
}

string LoadShedder::as_string() const
{
    return string("LoadShedder");
}

void LoadShedder::run(bool active, bool *ls_done, int ls_level)
{
    (*ls_done) = false;

    time_t t;
    time(&t);
    NOTICE << "Time: " << ctime(&t) << ": Running the Global LoadShedder...";

    bool result;

    _active = active;

    _ls_level = ls_level;
    DEBUG << "ls_level = " << _ls_level;

    if (active)
    {
        result = generate_lsplans(); // this resets current plan to default.

        if (!result)
        {
            WARN << "New LS plan could not be constructed";
            _no_drops = true;
            (*ls_done) = true;
            return; // no change in status
        }

        shed_load(1); // always plan # 1 for now.
        _no_drops = false;

        /*
        DEBUG << "Time: " << Time::now().to_msecs() << ": LS is sleeping...";
        Thread::sleep(Time::msecs(LS_PERIOD));
        DEBUG << "Time: " << Time::now().to_msecs() << ": LS woke up...";
        */

        Time finish_time = Time::now() + Time::msecs(LS_PERIOD);

        (new CallbackTimer(*_loop, wrap(this, &LoadShedder::finish, ls_done)))
                                              ->arm(finish_time);

        /*
        shedLoad(DEFAULT_PLAN_ID);
        _no_drops = true;
        (*ls_done) = true;
        */
    }
    else
    {
        // Make sure there are no drops in passive mode.
        //
        if (!_no_drops)
        {
            shed_load(DEFAULT_PLAN_ID);
            _no_drops = true;
        }
    }
}

void LoadShedder::finish(bool *ls_done)
{
    shed_load(DEFAULT_PLAN_ID);
    _no_drops = true;
    (*ls_done) = true;
}

void LoadShedder::find_drop_locations()
{
    DEBUG << "Finding the drop locations...";

    int dl_count = 0;

    // All input arcs are potential drop locations.
    //
    Catalog::StreamMap *streams = _catalog.get_stream_map();

    for (Catalog::StreamMap::iterator iter = streams->begin();
         iter != streams->end(); ++iter)
    {
        CatalogStream& catalog_stream = iter->second;

        if (catalog_stream.is_input_stream())
        {
            _input_streams.insert((iter->first).as_string());

            vector<BoxPort> *sinks = catalog_stream.get_sink_port();

            if (!sinks)
            {
                continue;
            }

            for (unsigned int i = 0; i < sinks->size(); i++)
            {
                if (((*sinks)[i].get_port_box()) == NULL)
                {
                    DEBUG << "Stream " << iter->first
                          << " is pass-thru on sink # " << i;
                    continue;
                }

                if (_ignore_nodes.find(((*sinks)[i].get_port_box())
                    ->get_box_node()) != _ignore_nodes.end())
                {
                    DEBUG << "it is in ignore node";
                    continue;
                }

                DropLocation loc;
                loc.in_stream_name = catalog_stream.get_stream_name();
                loc.dest_box_name = 
                                ((*sinks)[i].get_port_box())->get_box_name();
                loc.dest_box_port = (*sinks)[i].get_port();
                loc.dest_node = ((*sinks)[i].get_port_box())->get_box_node();

                /*
                loc.query_name = ((*sinks)[i].get_port_box())->get_box_query()
                                                        ->get_query_name();
                */
                loc.node = ((*sinks)[i].get_port_box())->get_box_node();

                loc.id = ++dl_count;
                _drop_locations[loc.id] = loc;

                DEBUG << _drop_locations[loc.id].as_string();

                if (_load_shedder_nodes.find(loc.node) ==
                    _load_shedder_nodes.end())
                {
                    _load_shedder_nodes.insert(loc.node);
                }
            }
        }
    }

    // All fan-out branches of a split box are potential drop locations.
    //
    Catalog::BoxMap *boxes = _catalog.get_box_map();

    for (Catalog::BoxMap::iterator iter = boxes->begin();
         iter != boxes->end(); ++iter)
    {
        CatalogBox& catalog_box = iter->second;

        // TO DO : multi-output boxes like case-like filter may require
        // a special treatment. Handle that later !!!
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

                /*
                if (_input_streams.find(stream->getStreamNameString()) !=
                                                    _input_streams.end())
                {
                    DEBUG << stream->getStreamNameString()
                          << " is already handled as an input stream";
                    continue;
                }
                */

                vector<BoxPort> *sinks = stream->get_sink_port();

                if (!sinks)
                {
                    continue;
                }

                for (unsigned int j = 0; j < sinks->size(); j++)
                {
                    DropLocation loc;

                    // if just one sink, then drop can go to the source node.
                    // this way, we also save bandwidth.
                    // otherwise, have to go the sink node due to sharing.
                    //
                    if (sinks->size() == 1)
                    {
                        // first, try the source
                        //
                        loc.node = 
                            stream->get_stream_source_box()->get_box_node();
                        if (_ignore_nodes.find(loc.node) != _ignore_nodes.end())
                        {
                            // then, let's try the sink
                            //
                            loc.node = 
                                ((*sinks)[j].get_port_box())->get_box_node();
                            if (_ignore_nodes.find(loc.node)
                                        != _ignore_nodes.end())
                            {
                                DEBUG << "it is in ignore node";
                                continue;
                            }
                            else
                            {
                                // sink is fine
                                //
                                /*
                                loc.query_name = ((*sinks)[j].get_port_box())
                                            ->get_box_query()->get_query_name();
                                */
                            }
                        }
                        else
                        {
                            // source is fine
                            //
                            /*
                            loc.query_name = stream->get_stream_source_box()
                                            ->get_box_query()->get_query_name();
                            */
                        }
                    }
                    else
                    {
                        // let's try the sink
                        //
                        loc.node = ((*sinks)[j].get_port_box())->get_box_node();
                        if (_ignore_nodes.find(loc.node) != _ignore_nodes.end())
                        {
                            DEBUG << "it is in ignore node";
                            continue;
                        }
                        else
                        {
                            // sink is fine
                            //
                            /*
                            loc.query_name = ((*sinks)[j].get_port_box())
                                            ->get_box_query()->get_query_name();
                            */
                        }
                    }

                    loc.in_stream_name = stream->get_stream_name();
                    loc.dest_box_name = 
                                ((*sinks)[j].get_port_box())->get_box_name();
                    loc.dest_box_port = (*sinks)[j].get_port();
                    loc.dest_node = 
                                ((*sinks)[j].get_port_box())->get_box_node();

                    loc.id = ++dl_count;
                    _drop_locations[loc.id] = loc;

                    DEBUG << _drop_locations[loc.id].as_string();

                    if (_load_shedder_nodes.find(loc.node) ==
                        _load_shedder_nodes.end())
                    {
                        _load_shedder_nodes.insert(loc.node);
                    }
                }
            }
        }
        else
        {
            if (_ignore_nodes.empty())
            {
                continue;
            }
            // can still be a drop location if it is an ignore node boundary
            //
            assert((catalog_box.get_box_out())->size() == 1);
            assert((catalog_box.get_out_stream(0))->get_sink_port()->size() 
                   <= 1);
            CatalogStream *stream = catalog_box.get_out_stream(0);
            if (stream->is_output_stream())
            {
                continue;
            }
            else
            {
                DEBUG << "Extra Drop Location on the boundary";
                string src_node = 
                            stream->get_stream_source_box()->get_box_node();
                string dest_node = ((*(stream->get_sink_port()))[0].
                                    get_port_box())->get_box_node();
                if ((_ignore_nodes.find(src_node) != _ignore_nodes.end()) &&
                    (_ignore_nodes.find(dest_node) == _ignore_nodes.end()))
                {
                    // I am on the boundary, but are there any locs upstream???
                    // *** FOR NOW:
                    // let's assume that all upstream nodes are ignore_nodes;
                    // and therefore, there can't be any locs upstream....
                    //
                    DropLocation loc;
                    loc.node = dest_node;
                    /*
                    loc.query_name = ((*(stream->get_sink_port()))[0].
                            get_port_box())->get_box_query()->get_query_name();
                    */
                    loc.in_stream_name = stream->get_stream_name();
                    loc.dest_box_name = ((*(stream->get_sink_port()))[0].
                                        get_port_box())->get_box_name();
                    loc.dest_box_port = 
                            (*(stream->get_sink_port()))[0].get_port();
                    loc.dest_node = dest_node;
                    loc.id = ++dl_count;
                    _drop_locations[loc.id] = loc;

                    DEBUG << _drop_locations[loc.id].as_string();

                    if (_load_shedder_nodes.find(loc.node) ==
                        _load_shedder_nodes.end())
                    {
                        _load_shedder_nodes.insert(loc.node);
                    }
                }
            }
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
            //DEBUG << "Output stream " << iter->first;

            BoxParameter::ParameterMap pm;

            // NOTE: subscription info is not correctly reflected !!!
            //
            uint16 min_gap = MAX_GAP;
            vector<CatalogSubscription*> *subs =
                                    catalog_stream.get_subscribers();
            if (subs)
            {
                //DEBUG << "number of subs = " << subs->size();
                for (unsigned int i = 0; i < subs->size(); i++)
                {
                    //DEBUG << (*subs)[i]->getGap();
                    if ((*subs)[i]->get_gap() < min_gap)
                    {
                        min_gap = (*subs)[i]->get_gap();
                    }
                }
            }
            /*
            DEBUG << "minimum gap for output stream \"" << iter->first
                  << "\" = " << min_gap;
            */
            pm["max_batch_size"] = string() + min_gap;

            if (catalog_stream.get_stream_source_box() == NULL)
            {
                DEBUG << "Stream " << iter->first << " is pass-thru";
                continue;
            }

            find_dlproperties(catalog_stream.get_stream_source_box()
                              ->get_box_name(), "random_drop", pm);
        }
    }
}

void LoadShedder::find_prefixes(Name box_name, vector<int> prefix,
                                double fraction)
{
    CatalogBox *box = _catalog.get_box(box_name);
    assert(box);

    CatalogStream::StreamStar *out_streams = box->get_box_out();
    assert(out_streams);

    int loc_id;

    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];

        if (out_stream->is_output_stream())
        {
            string out_stream_name = out_stream->get_stream_name_string();

            _output_cardinality2[_dl_prefixes.size()][out_stream_name]
                                                            = fraction;
            /*
            DEBUG << "adding " << out_stream_name
                  << " with fraction " << fraction
                  << " to the output map";
            */
            continue;
        }

        vector<BoxPort> *sinks = out_stream->get_sink_port();

        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            Name dest_box_name = ((*sinks)[j].get_port_box())->get_box_name();

            CatalogBox *dest_box = _catalog.get_box(dest_box_name);
            assert(dest_box);
            // Demo Hack:
            // Join works like Union in terms of output cardinality.
            //
            if ((dest_box->get_box_type() == "union") ||
                (dest_box->get_box_type() == "join"))
            {
                CatalogStream::StreamStar *in_streams = dest_box->get_box_in();
                double total = 0;
                for (unsigned int ii = 0; ii < in_streams->size(); ii++)
                {
                    CatalogStream *in_stream = (*in_streams)[ii];
                    total += get_stream_rate(
                                in_stream->get_stream_name_string(),
                                dest_box->get_box_node());
                }
                if (total > 0)
                {
                    double my_rate = get_stream_rate(
                                        out_stream->get_stream_name_string(),
                                        dest_box->get_box_node());
                    fraction = fraction*my_rate/total;
                }
                else
                {
                    fraction = fraction*1.0/double(in_streams->size());
                }
            }

            if ((loc_id = get_drop_location_id(out_stream->get_stream_name(),
                                               dest_box_name, 
                                               (*sinks)[j].get_port())))
            {
                vector<int> v = prefix;
                v.push_back(loc_id);
                _dl_prefixes[_dl_prefixes.size()+1] = v;
               
                /*
                DEBUG << "prefix " << _dl_prefixes.size() << ":";
                for (unsigned int k = 0; k < v.size(); k++)
                {
                    DEBUG << v[k];
                }
                */

                find_prefixes(dest_box_name, v, fraction);
            }
            else
            {
                find_prefixes(dest_box_name, prefix, fraction);
            }
        }
    }
}

void LoadShedder::find_dlprefixes()
{
    for (DropLocationMap::const_iterator iter = _drop_locations.begin();
         iter != _drop_locations.end(); iter++)
    {
        //Name dl_stream = (iter->second).in_stream_name;
        //assert(_catalog.getStream(dl_stream));

        if (is_start_location(iter->first))
        {
            vector<int> v;
            v.push_back(iter->first);
            _dl_prefixes[_dl_prefixes.size()+1] = v;

            /*
            DEBUG << "prefix " << _dl_prefixes.size() << ":";
            for (unsigned int i = 0; i < v.size(); i++)
            {
                DEBUG << v[i];
            }
            */

            double fraction = 1.0;
            double total = 0;
            string box_name = (iter->second).dest_box_name.as_string();
            CatalogBox *box = _catalog.get_box(box_name);
            assert(box);
            // Demo Hack:
            // Join works like Union in terms of output cardinality.
            //
            if ((box->get_box_type() == "union") ||
                (box->get_box_type() == "join"))
            {
                CatalogStream::StreamStar *in_streams = box->get_box_in();
                for (unsigned int i = 0; i < in_streams->size(); i++)
                {
                    CatalogStream *in_stream = (*in_streams)[i];
                    total += get_stream_rate(
                                        in_stream->get_stream_name_string(),
                                        (iter->second).dest_node);
                }
                if (total > 0)
                {
                    double my_rate = get_stream_rate(
                                     (iter->second).in_stream_name.as_string(),
                                     (iter->second).dest_node);
                    fraction = my_rate/total;
                }
                else { // it's even
                    fraction = 1.0/double(in_streams->size());
                }
            }

            find_prefixes((iter->second).dest_box_name, v, fraction);
        }
    }

    /*
    DEBUG << "Output cardinalities:";
    for (map<int, map<string, double> >::iterator
         iter = _output_cardinality2.begin();
         iter != _output_cardinality2.end(); iter++)
    {
        DEBUG << "prefix " << iter->first << ":";
        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            DEBUG << iter2->first << " => " << iter2->second;
        }
    }
    */
}

void LoadShedder::go_to_outputs(Name box_name, string stream_name, 
                                int dl_prefix_id, double sel)
{
    CatalogBox *box = _catalog.get_box(box_name);

    string node = box->get_box_node(); 

    CatalogStream::StreamStar *out_streams = box->get_box_out();
    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];
        string selectivity_id = box_name.as_string() + " : " +
                                out_stream->get_stream_source()->get_port();
        double selectivity;
        try
        {
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                        "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
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
            string out_stream_name = out_stream->get_stream_name_string();
            _output_total_selectivity[dl_prefix_id][out_stream_name] 
                                                        = selectivity*sel;
            _output_input_mapping[dl_prefix_id][out_stream_name] 
                                                        = stream_name;
            continue;
        }

        vector<BoxPort> *sinks = out_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            go_to_outputs(((*sinks)[j].get_port_box())->get_box_name(),
                          stream_name, dl_prefix_id, selectivity*sel);
        }
    }
}

void LoadShedder::go_to_inputs(Name box_name, int dl_prefix_id)
{
    CatalogBox *box = _catalog.get_box(box_name);

    CatalogStream::StreamStar *in_streams = box->get_box_in();

    for (unsigned int i = 0; i < in_streams->size(); i++)
    {
        CatalogStream *in_stream = (*in_streams)[i];

        if (in_stream->is_input_stream())
        {
            // traverse from here
            //
            vector<BoxPort> *sinks = in_stream->get_sink_port();
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                Name box_name2 = ((*sinks)[j].get_port_box())->get_box_name();
                CatalogBox *box2 = _catalog.get_box(box_name2);
                string node = box2->get_box_node(); 

                CatalogStream::StreamStar *out_streams = box2->get_box_out();
                for (unsigned int k = 0; k < out_streams->size(); k++)
                {
                    CatalogStream *out_stream = (*out_streams)[k];
                    string selectivity_id = box_name2.as_string() + " : " +
                                    out_stream->get_stream_source()->get_port();
                    double selectivity;
                    try
                    {
                        const FixLenTimeSeries &f = 
                            _statistics[node].get_statistics("Selectivity", 
                                                             selectivity_id);
                        try
                        {
                            selectivity = f.get_average();
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
                        WARN << "Could not read \"Selectivity\" for " 
                             << selectivity_id;
                        return;
                    }
                    catch(TSStats::error e)
                    {
                        WARN << "TSStats::Error " << (int)e;
                        WARN << "Could not read \"Selectivity\" for " 
                             << selectivity_id;
                        return;
                    }
                    catch(...)
                    {
                        WARN << "Unknown stats exception";
                        WARN << "Could not read \"Selectivity\" for " 
                             << selectivity_id;
                        return;
                    }

                    if (out_stream->is_output_stream())
                    {
                        string out_stream_name = 
                                        out_stream->get_stream_name_string();
                        _output_total_selectivity[dl_prefix_id][out_stream_name]
                                         = selectivity;
                        _output_input_mapping[dl_prefix_id][out_stream_name] 
                                         = in_stream->get_stream_name_string();
                        continue;
                    }

                    vector<BoxPort> *sinks2 = out_stream->get_sink_port();
                    for (unsigned int l = 0; l < sinks2->size(); l++)
                    {
                        go_to_outputs(
                                ((*sinks2)[l].get_port_box())->get_box_name(),
                                in_stream->get_stream_name_string(), 
                                dl_prefix_id, selectivity);
                    }
                }
            }    
        }
        else
        {
            // go back further
            //
            go_to_inputs(in_stream->get_stream_source_box()->get_box_name(),
                         dl_prefix_id);
        }
    }
}

void LoadShedder::compute_output_info()
{
    for (map<int, map<string, double> >::iterator
         iter = _output_cardinality2.begin();
         iter != _output_cardinality2.end(); iter++)
    {
        int dl_id = (_dl_prefixes[iter->first]).back();
        int dl_prefix_id = iter->first;

        Name stream_name = _drop_locations[dl_id].in_stream_name;
        CatalogStream *stream = _catalog.get_stream(stream_name);

        if (stream->is_input_stream())
        {
            // traverse from here
            //
            vector<BoxPort> *sinks = stream->get_sink_port();
            for (unsigned int i = 0; i < sinks->size(); i++)
            {
                Name box_name = ((*sinks)[i].get_port_box())->get_box_name();
                CatalogBox *box = _catalog.get_box(box_name);
                string node = box->get_box_node(); 

                CatalogStream::StreamStar *out_streams = box->get_box_out();
                for (unsigned int j = 0; j < out_streams->size(); j++)
                {
                    CatalogStream *out_stream = (*out_streams)[j];
                    string selectivity_id = box_name.as_string() + " : " +
                                    out_stream->get_stream_source()->get_port();
                    double selectivity;
                    try
                    {
                        const FixLenTimeSeries &f = 
                            _statistics[node].get_statistics("Selectivity", 
                                                             selectivity_id);

                        try
                        {
                            selectivity = f.get_average();
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
                        WARN << "Could not read \"Selectivity\" for " 
                             << selectivity_id;
                        return;
                    }
                    catch(TSStats::error e)
                    {
                        WARN << "TSStats::Error " << (int)e;
                        WARN << "Could not read \"Selectivity\" for " 
                             << selectivity_id;
                        return;
                    }
                    catch(...)
                    {
                        WARN << "Unknown stats exception";
                        WARN << "Could not read \"Selectivity\" for " 
                             << selectivity_id;
                        return;
                    }

                    if (out_stream->is_output_stream())
                    {
                        string out_stream_name = 
                                        out_stream->get_stream_name_string();
                        _output_total_selectivity[dl_prefix_id][out_stream_name]
                                        = selectivity;
                        _output_input_mapping[dl_prefix_id][out_stream_name] 
                                        = stream_name.as_string();
                        continue;
                    }

                    vector<BoxPort> *sinks = out_stream->get_sink_port();
                    for (unsigned int k = 0; k < sinks->size(); k++)
                    {
                        go_to_outputs(
                                ((*sinks)[k].get_port_box())->get_box_name(),
                                stream_name.as_string(), 
                                dl_prefix_id, selectivity);
                    }
                }
            }    
        }
        else
        {
            // go back further
            //
            go_to_inputs(stream->get_stream_source_box()->get_box_name(),
                         dl_prefix_id);
        }

        /*
        DEBUG << "prefix " << iter->first << ":";
        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            DEBUG << iter2->first << " => " 
                  << _output_input_mapping[iter->first][iter2->first] << " "
                  << _output_total_selectivity[iter->first][iter2->first];
        }
        */
    }
}

int LoadShedder::get_drop_location_id(Name stream, Name box_name, 
                                      uint16 sink_port)
{
    for (DropLocationMap::const_iterator iter = _drop_locations.begin();
         iter != _drop_locations.end(); iter++)
    {
        Name dl_stream = (iter->second).in_stream_name;
        Name dl_box = (iter->second).dest_box_name;
        uint16 dl_port = (iter->second).dest_box_port;

        if ((stream == dl_stream) && (box_name == dl_box) &&
            (sink_port == dl_port))
        {
            /*
            DEBUG << stream.as_string() << " " << box_name.as_string() << " "
                  << sink_port << " is a drop location " << iter->first;
            */
            return(iter->first);
        }
    }

    /*
    DEBUG << stream.as_string() << " " << box_name.as_string() << " "
          << sink_port << " is NOT a drop location";
    */
    return 0;
}

void LoadShedder::find_dlproperties(Name box_name, string drop_type,
                                    BoxParameter::ParameterMap box_params)
{
    int loc_id;

    CatalogBox *box = _catalog.get_box(box_name);
    BoxParameter::ParameterMap *pm;

    //DEBUG << "Hopping a box of type " << box->getBoxType();

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
    // else, nothing to do...

    CatalogStream::StreamStar *in_streams = box->get_box_in();

    for (unsigned int i = 0; i < in_streams->size(); i++)
    {
        CatalogStream *in_stream = (*in_streams)[i];
        vector<BoxPort> *sinks = in_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            if (((*sinks)[j].get_port_box())->get_box_name() == box_name)
            {
                if ((loc_id = get_drop_location_id(in_stream->get_stream_name(),
                                                   box_name, 
                                                   (*sinks)[j].get_port())))
                {
                    _drop_locations[loc_id].box_type = drop_type;
                    _drop_locations[loc_id].box_params.set_parameter_map(
                                                             &box_params);
                    /*
                    DEBUG << "Drop type and params are set as:";
                    DEBUG << _drop_locations[loc_id].as_string();
                    */
                }
            }
        }

        if (in_stream->is_input_stream())
        {
            // there is some redundant checking here, but should work correctly
            continue;
        }
        else
        {
            find_dlproperties(
                        in_stream->get_stream_source_box()->get_box_name(),
                        drop_type, box_params);
        }
    }
}

void LoadShedder::compute_load(Name box_name, string node, double *load)
{
    /*
    DEBUG << "LoadShedder::computeLoad(" << box_name << ", " << node << ", "
          << *load << ")";
    */

    CatalogBox *box = _catalog.get_box(box_name);

    if (box->get_box_node() != node)
    {
        //DEBUG << "Node boundary: computeLoad returns";
        return;
    }

    try
    {
        const FixLenTimeSeries &f = _statistics[node].get_statistics(
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
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
        return;
    }
    catch(TSStats::error e)
    {
        WARN << "TSStats::Error " << (int)e;
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
        return;
    }
    catch(...)
    {
        WARN << "Unknown stats exception";
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
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
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
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
            double load_down = 0;
            compute_load(((*sinks)[j].get_port_box())->get_box_name(),
                         node, &load_down);
            (*load) += selectivity * load_down;
        }
    }
}

void LoadShedder::get_there(Name box_name, string node, double *load)
{
    /*
    DEBUG << "LoadShedder::getThere(" << box_name << ", " << node << ", "
          << *load << ")";
    */

    CatalogBox *box = _catalog.get_box(box_name);

    if (box->get_box_node() != node)
    {
        //DEBUG << "still not there :(";
        CatalogStream::StreamStar *out_streams = box->get_box_out();
        for (unsigned int i = 0; i < out_streams->size(); i++)
        {
            CatalogStream *out_stream = (*out_streams)[i];
            vector<BoxPort> *sinks = out_stream->get_sink_port();
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                get_there(((*sinks)[j].get_port_box())->get_box_name(),
                          node, load);
            }
        }
    }
    else
    {
        //DEBUG << "got there !";
        try
        {
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
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
            WARN << "Could not read \"Cost\" for box " << box_name.as_string();
            return;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"Cost\" for box " << box_name.as_string();
            return;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"Cost\" for box " << box_name.as_string();
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
                const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                                "Selectivity", selectivity_id);
                try
                {
                    selectivity = f.get_average();
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
                double load_down = 0;
                compute_load(((*sinks)[j].get_port_box())->get_box_name(),
                             node, &load_down);
                (*load) += selectivity * load_down;
            }
        }
    }
}

double LoadShedder::compute_load_factor(int loc_id, string node)
{
    double load = 0;

    Name current_box = _drop_locations[loc_id].dest_box_name;

    if (_statistics[node].empty())
    {
        WARN << "No stats available for node " << node;
        //DEBUG << "returning load factor = " << load;
        return load;
    }

    get_there(current_box, node, &load);

    //DEBUG << "returning load factor = " << load;
    return load;
}

void LoadShedder::generate_load_factor_matrix()
{
    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter1 = node_addresses.begin();
         iter1 != node_addresses.end(); ++iter1)
    {
        string node_address = *iter1;

        if (_ignore_nodes.find(node_address) != _ignore_nodes.end())
        {
            /*
            DEBUG << "Not computing LoadFactorMatrix for node "
                  << node_address;
            */
            continue;
        }

        //DEBUG << "Load factors for node: " << node_address;

        DLLoadFactorMap load_factors;

        for (DropLocationMap::const_iterator iter2 = _drop_locations.begin();
             iter2 != _drop_locations.end(); iter2++)
        {
            load_factors[iter2->first] = compute_load_factor(iter2->first,
                                                             node_address);
            /*
            DEBUG << "loc_id = " << iter2->first << " : "
                  << load_factors[iter2->first];
            */
        }

        _load_factor_matrix[node_address] = load_factors;
    }
}

void LoadShedder::compute_load_2(Name box_name, string node, double *load)
{
    /*
    DEBUG << "LoadShedder::computeLoad2(" << box_name << ", " << node << ", "
          << *load << ")";
    */

    CatalogBox *box = _catalog.get_box(box_name);

    if (box->get_box_node() != node)
    {
        //DEBUG << "Node boundary: computeLoad2 returns";
        return;
    }

    try
    {
        const FixLenTimeSeries &f = _statistics[node].get_statistics(
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
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
        return;
    }
    catch(TSStats::error e)
    {
        WARN << "TSStats::Error " << (int)e;
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
        return;
    }
    catch(...)
    {
        WARN << "Unknown stats exception";
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
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
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
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

        int loc_id;
        vector<BoxPort> *sinks = out_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            double load_down = 0;
            Name dest_box_name = ((*sinks)[j].get_port_box())->get_box_name();
            if ((loc_id = get_drop_location_id(out_stream->get_stream_name(),
                                               dest_box_name, 
                                               (*sinks)[j].get_port())))
            {
                //DEBUG << "Location boundary: " << loc_id;
                continue;
            }
            else
            {
                compute_load_2(dest_box_name, node, &load_down);
                (*load) += selectivity * load_down;
            }
        }
    }
}

void LoadShedder::get_there_2(Name box_name, string node, double *load)
{
    /*
    DEBUG << "LoadShedder::getThere2(" << box_name << ", " << node << ", "
          << *load << ")";
    */

    CatalogBox *box = _catalog.get_box(box_name);

    if (box->get_box_node() != node)
    {
        //DEBUG << "still not there :(";
        CatalogStream::StreamStar *out_streams = box->get_box_out();
        for (unsigned int i = 0; i < out_streams->size(); i++)
        {
            CatalogStream *out_stream = (*out_streams)[i];
            vector<BoxPort> *sinks = out_stream->get_sink_port();
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                Name dest_box_name = 
                        ((*sinks)[j].get_port_box())->get_box_name();
                int loc_id;
                if ((loc_id = get_drop_location_id(
                                    out_stream->get_stream_name(),
                                    dest_box_name, (*sinks)[j].get_port())))
                {
                    if (((*sinks)[j].get_port_box())->get_box_node() != node)
                    {
                        return;
                    }
                }
                get_there_2(dest_box_name, node, load);
            }
        }
    }
    else
    {
        //DEBUG << "got there !";
        try
        {
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
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
            WARN << "Could not read \"Cost\" for box " << box_name.as_string();
            return;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"Cost\" for box " << box_name.as_string();
            return;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"Cost\" for box " << box_name.as_string();
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
                const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                                "Selectivity", selectivity_id);
                try
                {
                    selectivity = f.get_average();
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

            int loc_id;
            vector<BoxPort> *sinks = out_stream->get_sink_port();
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                double load_down = 0;
                Name dest_box_name = 
                            ((*sinks)[j].get_port_box())->get_box_name();
                if ((loc_id = get_drop_location_id(
                                    out_stream->get_stream_name(),
                                    dest_box_name, (*sinks)[j].get_port())))
                {
                    //DEBUG << "Location boundary: " << loc_id;
                    continue;
                }
                else
                {
                    compute_load_2(dest_box_name, node, &load_down);
                    (*load) += selectivity * load_down;
                }
            }
        }
    }
}

double LoadShedder::compute_load_factor_2(int prefix_id, string node)
{
    double load = 0;

    assert(!_dl_prefixes[prefix_id].empty());

    int last_id = _dl_prefixes[prefix_id].back();

    assert(_drop_locations.find(last_id) != _drop_locations.end());

    Name current_box = _drop_locations[last_id].dest_box_name;

    if (_statistics[node].empty())
    {
        WARN << "No stats available for node " << node;
        //DEBUG << "returning load factor = " << load;
        return load;
    }

    get_there_2(current_box, node, &load);

    //DEBUG << "returning load factor = " << load;
    return load;
}

void LoadShedder::generate_load_factor_matrix_2()
{
    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter1 = node_addresses.begin();
         iter1 != node_addresses.end(); ++iter1)
    {
        string node_address = *iter1;

        if (_ignore_nodes.find(node_address) != _ignore_nodes.end())
        {
            /*
            DEBUG << "Not computing LoadFactorMatrix for node "
                  << node_address;
            */
            continue;
        }

        //DEBUG << "Load factors for node: " << node_address;

        DLLoadFactorMap load_factors;

        // Note:
        // We only need them starting from the longest prefix closest
        // to the given node_address. For others, 0 will be returned.
        //
        for (DLPrefixMap::const_iterator iter2 = _dl_prefixes.begin();
             iter2 != _dl_prefixes.end(); iter2++)
        {
            load_factors[iter2->first] = compute_load_factor_2(iter2->first,
                                                               node_address);
            
            /*
            DEBUG << "prefix_id = " << iter2->first << " : "
                  << load_factors[iter2->first];
            */
            
        }

        _load_factor_matrix2[node_address] = load_factors;
    }
}

bool LoadShedder::is_in_prefix(int loc_id, int prefix_id)
{
    vector<int> ids = _dl_prefixes[prefix_id];

    for (unsigned int i = 0; i < ids.size(); i++)
    {
        if (ids[i] == loc_id)
        {
            return true;
        }
    }

    return false;
}

// prefix is already matched, just find the right node.
//
double LoadShedder::compute_rate_factor_node(string node, int prefix_id, 
                                             string box_name, double sel)
{
    CatalogBox *box = _catalog.get_box(Name(box_name));
    string box_node = box->get_box_node();

    if (box_node == node)
    {
        return sel;
    }

    CatalogStream::StreamStar *out_streams = box->get_box_out();
    assert(out_streams->size() == 1); // assuming no case-like filter
    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];
        string selectivity_id = box_name + " : " +
                                out_stream->get_stream_source()->get_port();
        double selectivity;
        try
        {
            const FixLenTimeSeries &f = _statistics[box_node].get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
            }
            catch(...)
            {
                WARN << "Could not get \"Average Selectivity\" for "
                     << selectivity_id;
                return 0;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return 0;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return 0;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return 0;
        }

        vector<BoxPort> *sinks = out_stream->get_sink_port();
        assert(sinks->size() == 1); // no more splits expected
        if ((*sinks)[0].get_port_box())
        {
            Name dest_box_name = ((*sinks)[0].get_port_box())->get_box_name();
            return (compute_rate_factor_node(node, prefix_id,
                                             dest_box_name.as_string(),
                                             selectivity*sel));
        }
        else // couldn't find
        {
            return 0;
        }
    }

    return 0;
}

// both prefix and node must be matched.
//
double LoadShedder::compute_rate_factor(string node, int prefix_id, 
                                        string box_name, double sel)
{
    CatalogBox *box = _catalog.get_box(Name(box_name));
    string box_node = box->get_box_node();

    CatalogStream::StreamStar *out_streams = box->get_box_out();
    assert(out_streams->size() == 1); // assuming no case-like filter
    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];
        string selectivity_id = box_name + " : " +
                                out_stream->get_stream_source()->get_port();
        double selectivity;
        try
        {
            const FixLenTimeSeries &f = _statistics[box_node].get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
            }
            catch(...)
            {
                WARN << "Could not get \"Average Selectivity\" for "
                     << selectivity_id;
                return 0;
            }
        }
        catch(Statistics::error e)
        {
            WARN << "Statistics::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return 0;
        }
        catch(TSStats::error e)
        {
            WARN << "TSStats::Error " << (int)e;
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return 0;
        }
        catch(...)
        {
            WARN << "Unknown stats exception";
            WARN << "Could not read \"Selectivity\" for " << selectivity_id;
            return 0;
        }

        int loc_id;
        vector<BoxPort> *sinks = out_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            Name dest_box_name = ((*sinks)[j].get_port_box())->get_box_name();
            if ((loc_id = get_drop_location_id(out_stream->get_stream_name(),
                                               dest_box_name, 
                                               (*sinks)[j].get_port())))
            {
                if ((loc_id == (_dl_prefixes[prefix_id]).back()) &&
                    (_drop_locations[loc_id].node == node))
                {
                    // we are done.
                    //
                    return selectivity*sel;    
                }
                else if (loc_id == (_dl_prefixes[prefix_id]).back())
                {
                    // next, we need to reach the node.
                    //
                    return (compute_rate_factor_node(node, prefix_id, 
                                                     dest_box_name.as_string(), 
                                                     selectivity*sel));
                }
                else if (is_in_prefix(loc_id, prefix_id))
                {
                    // keep going on this path until we reach loc_id and node.
                    //
                    return (compute_rate_factor(node, prefix_id,
                                                dest_box_name.as_string(),
                                                selectivity*sel));
                }
                else 
                {
                    // it can not be on this path.
                    //
                    continue;
                }
            }
            else
            {
                assert(sinks->size() == 1); //??

                // not a location, keep going.
                //
                return (compute_rate_factor(node, prefix_id, 
                                            dest_box_name.as_string(),
                                            selectivity*sel));
            }
        }
    }

    return 0;
}

void LoadShedder::generate_rate_factor_matrix()
{
    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter1 = node_addresses.begin();
         iter1 != node_addresses.end(); ++iter1)
    {
        string node_address = *iter1;

        if (_ignore_nodes.find(node_address) != _ignore_nodes.end())
        {
            continue;
        }

        for (DLPrefixMap::const_iterator iter2 = _dl_prefixes.begin();
             iter2 != _dl_prefixes.end(); iter2++)
        {
            assert(_drop_locations.find((iter2->second).front()) !=
                   _drop_locations.end());

            string input_name = (_drop_locations[(iter2->second).front()].
                                 in_stream_name).as_string();

            _rate_factor_matrix[node_address][iter2->first].first = input_name;

            string box_name = (_drop_locations[(iter2->second).front()].
                               dest_box_name).as_string();

            if (_load_factor_matrix2[node_address][iter2->first] == 0)
            {
                _rate_factor_matrix[node_address][iter2->first].second = 0;
            }
            else if (((iter2->second).size() == 1) &&
                (node_address == _drop_locations[(iter2->second).front()].node))
            {
                _rate_factor_matrix[node_address][iter2->first].second = 1.0;
            }
            else if ((iter2->second).size() == 1) // only node has to match
            {
                _rate_factor_matrix[node_address][iter2->first].second =
                            compute_rate_factor_node(node_address, iter2->first,
                                                     box_name, 1.0);    
            }
            else // both loc_id and node has to match.
            {
                _rate_factor_matrix[node_address][iter2->first].second =
                            compute_rate_factor(node_address, iter2->first,
                                                box_name, 1.0);    
            }
        }
    }
}

void LoadShedder::output_cardinality(Name box_name, set<string> *output_set)
{
    CatalogBox *box = _catalog.get_box(box_name);

    CatalogStream::StreamStar *out_streams = box->get_box_out();
    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];
        if (out_stream->is_output_stream())
        {
            if (output_set->find(out_stream->get_stream_name_string()) ==
                output_set->end())
            {
                output_set->insert(out_stream->get_stream_name_string());
                /*
                DEBUG << "adding " << out_stream->getStreamNameString()
                      << " to the output set";
                */
            }
            continue;
        }
        else
        {
            vector<BoxPort> *sinks = out_stream->get_sink_port();
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                output_cardinality(((*sinks)[j].get_port_box())->get_box_name(),
                                   output_set);
            }
        }
    }
}

void LoadShedder::compute_output_cardinalities()
{
    //DEBUG << "LoadShedder::computeOutputCardinalities()";

    for (DropLocationMap::const_iterator iter = _drop_locations.begin();
         iter != _drop_locations.end(); iter++)
    {
        set<string> output_set;
        output_set.clear();
        output_cardinality((iter->second).dest_box_name, &output_set);
        
        /*
        DEBUG << "output cardinality for drop location " << iter->first
              << " : " << output_set.size();
        for (set<string>::iterator i = output_set.begin();
             i != output_set.end(); i++)
        {
            DEBUG << *i;
        }
        */
        
        _output_cardinality[iter->first] = output_set;
    }
}

double LoadShedder::get_stream_rate(string stream_name, string node)
{
    double result;

    if (_statistics[node].empty())
    {
        WARN << "No stats available for node " << node;
        return 0;
    }

    try
    {
        const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                        "StreamRate", stream_name);
        try
        {
            result = f.get_average();
            /*
            DEBUG << "stream_rate for " << stream_name << " on node "
                  << node << ": " << result;
            */
            return result;
        }
        catch(...)
        {
            WARN << "Could not get \"Average StreamRate\" for stream "
                 << stream_name;
            return 0;
        }
    }
    catch(Statistics::error e)
    {
        WARN << "Statistics::Error " << (int)e;
        WARN << "Could not read \"StreamRate\" for stream " << stream_name;
        return 0;
    }
    catch(TSStats::error e)
    {
        WARN << "TSStats::Error " << (int)e;
        WARN << "Could not read \"StreamRate\" for stream " << stream_name;
        return 0;
    }
    catch(...)
    {
        WARN << "Unknown stats exception";
        WARN << "Could not read \"StreamRate\" for stream " << stream_name;
        return 0;
    }
}

void LoadShedder::get_stream_rates_pernode()
{
    for (map<string, DropLocationPlusMap>::iterator 
         iter1 = _drop_locations_pernode.begin();
         iter1 != _drop_locations_pernode.end();
         iter1++)
    {
        string node = iter1->first;

        for (DropLocationPlusMap::iterator iter2 = (iter1->second).begin();
             iter2 != (iter1->second).end(); iter2++)
        {
            string stream_name = ((iter2->second).in_stream_name).as_string();

            try
            {
                const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                            "StreamRate", stream_name);
                try
                {
                    _stream_rates[iter2->first] = f.get_average();

                    assert(_catalog.get_stream(Name(stream_name)));
                    if ((_catalog.get_stream(Name(stream_name)))
                        ->is_input_stream())
                    {
                        // HACK: for our experiments, we never decrease
                        // rates. so refresh them only if there is an
                        // increase.
                        //
                        if (_input_rates.find(stream_name) 
                                                == _input_rates.end())
                        {
                            _input_rates[stream_name] = 
                                        _stream_rates[iter2->first];
                        }
                        else if (_input_rates[stream_name] < 
                            _stream_rates[iter2->first])
                        {
                            _input_rates[stream_name] = 
                                                _stream_rates[iter2->first];
                        }
                        // _input_rates[stream_name] = 
                        // _stream_rates[iter2->first];
                        NOTICE << stream_name << " : " 
                               << _input_rates[stream_name];
                    }

                    /*
                    DEBUG << "stream_rate on loc_id = " << iter2->first << ": "
                          << _stream_rates[iter2->first];
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
                WARN << "Could not read \"StreamRate\" for stream " 
                     << stream_name;
                return;
            }
            catch(TSStats::error e)
            {
                WARN << "TSStats::Error " << (int)e;
                WARN << "Could not read \"StreamRate\" for stream " 
                     << stream_name;
                return;
            }
            catch(...)
            {
                WARN << "Unknown stats exception";
                WARN << "Could not read \"StreamRate\" for stream " 
                     << stream_name;
                return;
            }
        }
    }
}

void LoadShedder::get_stream_rates()
{
    for (DropLocationMap::const_iterator iter = _drop_locations.begin();
         iter != _drop_locations.end(); iter++)
    {
        string stream_name = ((iter->second).in_stream_name).as_string();
        string node = (iter->second).node;

        try
        {
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                            "StreamRate", stream_name);
            try
            {
                _stream_rates[iter->first] = f.get_average();

                assert(_catalog.get_stream(Name(stream_name)));
                if ((_catalog.get_stream(Name(stream_name)))->is_input_stream())
                {
                    // HACK: for our experiments, we never decrease
                    // rates. so refresh them only if there is an
                    // increase.
                    //
                    if (_input_rates.find(stream_name) == _input_rates.end())
                    {
                        _input_rates[stream_name] = _stream_rates[iter->first];
                    }
                    else if (_input_rates[stream_name] < 
                             _stream_rates[iter->first])
                    {
                        _input_rates[stream_name] = _stream_rates[iter->first];
                    }
                    // _input_rates[stream_name] = _stream_rates[iter->first];
                    NOTICE << stream_name << " : " << _input_rates[stream_name];
                }

                /*
                DEBUG << "stream_rate on loc_id = " << iter->first << ": "
                      << _stream_rates[iter->first];
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
    }
}

// We assume that there are no split and merge cases for now.
// Therefore, drop locations directly translate into the matrix columns.
//
bool LoadShedder::construct_lp()
{

    //DEBUG << "LoadShedder::constructLP()";

    bool result = true;
    int col_count = 0;
    int row_count = 0;

    _lp = lpx_create_prob();
    lpx_set_prob_name(_lp, "LoadSheddingProblem");
    lpx_create_index(_lp);

    // objective function
    //
    lpx_set_obj_dir(_lp, LPX_MAX);
    lpx_set_obj_name(_lp, "TupleDelivery");
    for (DropLocationMap::const_iterator iter = _drop_locations.begin();
         iter != _drop_locations.end(); iter++)
    {
        ostringstream col_index;
        col_index << iter->first;
        string col_name = "x_" + col_index.str();
        //DEBUG << "column name = " << col_name;
        int index = lpx_find_col(_lp, (char *)col_name.c_str());
        if (index == 0)
        {
            index = lpx_add_cols(_lp, 1);
            lpx_set_col_name(_lp, index, (char *)col_name.c_str());
            /*
            DEBUG << "added column = " << lpx_get_col_name(_lp, index)
                  << " at index = " << index;
            */
            ++col_count;
        }
        else
        {
            WARN << "Column " << col_name << " already exists!";
            continue;
        }
        lpx_set_obj_coef(_lp, index, _output_cardinality[iter->first].size());
    }

    // load constraints
    //
    int *ind = (int *)ucalloc(1+_drop_locations.size(), sizeof(int));
    double *val = (double *)ucalloc(1+_drop_locations.size(), sizeof(double));
    set<string>& node_addresses = *(_catalog.get_region_node());
    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); iter++)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            //DEBUG << "Not computing load constraint for node " << *iter;
            continue;
        }

        ostringstream s;
        s << "Node" << (row_count+1);
        string row_name = s.str();
        int index = lpx_find_row(_lp, (char *)row_name.c_str());
        if (index == 0)
        {
            index = lpx_add_rows(_lp, 1);
            lpx_set_row_name(_lp, index, (char *)row_name.c_str());
            /*
            DEBUG << "added row = " << lpx_get_row_name(_lp, index)
                  << " at index = " << index;
            */
            ++row_count;
        }
        else
        {
            WARN << "Row " << row_name << " already exists!";
            continue;
        }

        int count = 0;
        for (DropLocationMap::const_iterator iter2 = _drop_locations.begin();
             iter2 != _drop_locations.end(); iter2++)
        {
            double coef = _load_factor_matrix[*iter][iter2->first]
                          * _stream_rates[iter2->first];
            //DEBUG << "coef for loc " << iter2->first << " : " << coef;
            if (coef > 0)
            {
                ++count;
                ostringstream col_index;
                col_index << iter2->first;
                string col_name = "x_" + col_index.str();
                //DEBUG << "column name = " << col_name;
                ind[count] = lpx_find_col(_lp, (char *)col_name.c_str());
                //DEBUG << "index = " << ind[count];
                assert(ind[count] > 0);
                val[count] = coef;
            }
        }
        if (count > 0)
        {
            lpx_set_mat_row(_lp, index, count, ind, val);
            lpx_set_row_bnds(_lp, index, LPX_UP, 0.0, 1.0);
        }
        else
        {
            int num[2];
            num[1] = index;
            lpx_del_rows(_lp, 1, num);
        }
    }
    ufree(ind);
    ufree(val);

    // variable bounds
    //
    for (int i = 1; i <= col_count; i++)
    {
        lpx_set_col_bnds(_lp, i, LPX_DB, 0.0, 1.0);
    }

    //DEBUG << "num rows = " << lpx_get_num_rows(_lp);
    //DEBUG << "num cols = " << lpx_get_num_cols(_lp);
    //DEBUG << "num nz = " << lpx_get_num_nz(_lp);

    lpx_delete_index(_lp);

    lpx_order_matrix(_lp);

    if ((lpx_get_num_rows(_lp) > 0) && (lpx_get_num_cols(_lp) > 0))
    {
        lpx_write_cpxlp(_lp, "LoadSheddingProblem.lp");
        system("cat LoadSheddingProblem.lp");
    }
    else
    {
        WARN << "LoadSheddingProblem has 0 rows or 0 columns";
        result = false;
    }

    //DEBUG << "Done constructLP()... " << result;

    return result;
}

int LoadShedder::solve_lp()
{
    int result;

    if (_lp)
    {
        result = lpx_simplex(_lp);
    }

    return result;
}

void LoadShedder::get_children(int prefix_id, set<int> *children)
{

    vector<int> v = _dl_prefixes[prefix_id];

    for (DLPrefixMap::const_iterator iter = _dl_prefixes.begin();
         iter != _dl_prefixes.end(); iter++)
    {
        vector<int> u = iter->second;
        if (u.size() == (v.size()+1))
        {
            u.pop_back();
            if (u == v)
            {
                children->insert(iter->first);
            }
        }
    }
}

bool LoadShedder::construct_lp2(int id, map<string, double> input_rates)
{
    //DEBUG << "LoadShedder::constructLP2()";

    bool result = true;
    int col_count = 0;
    int row_count = 0;

    _lp = lpx_create_prob();
    string prob_name("LoadSheddingProblem");
    prob_name += id;
    char *prob_name2 = (char *)
                       malloc(sizeof(char)*(strlen(prob_name.c_str())+1));
    strcpy(prob_name2, prob_name.c_str());
    lpx_set_prob_name(_lp, prob_name2);
    lpx_create_index(_lp);

    // objective function
    //
    if (_quality_metric == 0) // maximize throughput
    {
        lpx_set_obj_dir(_lp, LPX_MAX);
        lpx_set_obj_name(_lp, "Throughput");
        for (map<int, map<string, double> >::iterator
             iter = _output_cardinality2.begin();
             iter != _output_cardinality2.end(); iter++)
        {
            ostringstream col_index;
            col_index << iter->first;
            string col_name = "x_" + col_index.str();
            //DEBUG << "column name = " << col_name;
            int index = lpx_find_col(_lp, (char *)col_name.c_str());
            if (index == 0)
            {
                index = lpx_add_cols(_lp, 1);
                lpx_set_col_name(_lp, index, (char *)col_name.c_str());
                /*
                DEBUG << "added column = " << lpx_get_col_name(_lp, index)
                      << " at index = " << index;
                */
                ++col_count;
            }
            else
            {
                WARN << "Column " << col_name << " already exists!";
                continue;
            }
            double total = 0;
            for (map<string, double>::iterator iter2 = (iter->second).begin();
                 iter2 != (iter->second).end(); iter2++)
            {
                total += _output_total_selectivity[iter->first][iter2->first] *
                         input_rates[_output_input_mapping[iter->first]
                                                          [iter2->first]];
            }
            lpx_set_obj_coef(_lp, index, total);
        }
    }
    else if (_quality_metric == 1) // maximize tuple_delivery
    {
        lpx_set_obj_dir(_lp, LPX_MAX);
        lpx_set_obj_name(_lp, "TupleDelivery");
        for (map<int, map<string, double> >::iterator
             iter = _output_cardinality2.begin();
             iter != _output_cardinality2.end(); iter++)
        {
            ostringstream col_index;
            col_index << iter->first;
            string col_name = "x_" + col_index.str();
            //DEBUG << "column name = " << col_name;
            int index = lpx_find_col(_lp, (char *)col_name.c_str());
            if (index == 0)
            {
                index = lpx_add_cols(_lp, 1);
                lpx_set_col_name(_lp, index, (char *)col_name.c_str());
                /*
                DEBUG << "added column = " << lpx_get_col_name(_lp, index)
                      << " at index = " << index;
                */
                ++col_count;
            }
            else
            {
                WARN << "Column " << col_name << " already exists!";
                continue;
            }
            double total = 0;
            for (map<string, double>::iterator iter2 = (iter->second).begin();
                 iter2 != (iter->second).end(); iter2++)
            {
                total += iter2->second;
            }
            lpx_set_obj_coef(_lp, index, total);
        }
    }
    else if (_quality_metric == 2) // maximize semantic value
    {
        WARN << "Quality metric : " << _quality_metric 
             << " currently not supported";
    }
    else
    {
        WARN << "Invalid quality metric : " << _quality_metric;
    }

    // load constraints
    //
    int *ind = (int *)ucalloc(1+_dl_prefixes.size(), sizeof(int));
    double *val = (double *)ucalloc(1+_dl_prefixes.size(), sizeof(double));
    set<string>& node_addresses = *(_catalog.get_region_node());
    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); iter++)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            //DEBUG << "Not computing load constraint for node " << *iter;
            continue;
        }

        double node_load = 0;

        ostringstream s;
        s << "Node" << (row_count+1);
        string row_name = s.str();
        int row_index = lpx_find_row(_lp, (char *)row_name.c_str());
        if (row_index == 0)
        {
            row_index = lpx_add_rows(_lp, 1);
            lpx_set_row_name(_lp, row_index, (char *)row_name.c_str());
            /*
            DEBUG << "added row = " << lpx_get_row_name(_lp, row_index)
                  << " at index = " << row_index;
            */
            ++row_count;
        }
        else
        {
            WARN << "Row " << row_name << " already exists!";
            continue;
        }

        int count = 0;
        for (DLPrefixMap::const_iterator iter2 = _dl_prefixes.begin();
             iter2 != _dl_prefixes.end(); iter2++)
        {
            /*
            double coef = _load_factor_matrix2[*iter][iter2->first]
                          * _stream_rates[(iter2->second).back()];
            */
            double coef = _load_factor_matrix2[*iter][iter2->first] 
                          * (_rate_factor_matrix[*iter][iter2->first]).second
                          * input_rates[(_rate_factor_matrix[*iter]
                                                      [iter2->first]).first];

            DEBUG << "coef for prefix " << iter2->first << " : " << coef;

            node_load += coef;

            if (coef > 0)
            {
                ostringstream col_index_str;
                col_index_str << iter2->first;
                string col_name = "x_" + col_index_str.str();
                //DEBUG << "column name = " << col_name;
                int col_index = lpx_find_col(_lp, (char *)col_name.c_str());
                //DEBUG << "index = " << col_index;
                if (col_index == 0)
                {
                    col_index = lpx_add_cols(_lp, 1);
                    lpx_set_col_name(_lp, col_index, (char *)col_name.c_str());
                    /*
                    DEBUG << "added column = "
                          << lpx_get_col_name(_lp, col_index)
                          << " at index = " << col_index;
                    */
                    ++col_count;
                }
                ++count;
                ind[count] = col_index;
                assert(ind[count] > 0);
                val[count] = coef;
            }
        }

        if (count > 0)
        {
            assert(node_load > 0);
            lpx_set_mat_row(_lp, row_index, count, ind, val);
            //lpx_set_row_bnds(_lp, row_index, LPX_UP, 0.0, 1.0);
            //
            // RHS can be set low for the demo
            //
            if (!_ls_level) 
            { // real system load
                lpx_set_row_bnds(_lp, row_index, LPX_UP, 0.0, CAPACITY);
            }
            else 
            { // manual selection
                lpx_set_row_bnds(_lp, row_index, LPX_UP, 0.0,
                                 node_load/(1.0 + _ls_level*0.01));
            }
        }
        else
        {
            int num[2];
            num[1] = row_index;
            lpx_del_rows(_lp, 1, num);
        }

        DEBUG << "Load at node " << *iter << ": " << node_load;
    }

    // variable bounds and additional constraints
    //
    int count = 0;
    for (DLPrefixMap::const_iterator iter2 = _dl_prefixes.begin();
         iter2 != _dl_prefixes.end(); iter2++)
    {
        // set variable bounds for each prefix
        //
        ostringstream col_index_str;
        col_index_str << iter2->first;
        string col_name = "x_" + col_index_str.str();
        //DEBUG << "column name = " << col_name;
        int col_index = lpx_find_col(_lp, (char *)col_name.c_str());
        //DEBUG << "index = " << col_index;
        if (col_index == 0)
        {
            col_index = lpx_add_cols(_lp, 1);
            lpx_set_col_name(_lp, col_index, (char *)col_name.c_str());
            /*
            DEBUG << "added column = " << lpx_get_col_name(_lp, col_index)
                  << " at index = " << col_index;
            */
            ++col_count;
        }
        lpx_set_col_bnds(_lp, col_index, LPX_DB, 0.0, 1.0);

        // additional constraints
        //
        set<int> children;
        get_children(iter2->first, &children);
        for (set<int>::iterator i = children.begin(); i != children.end(); i++)
        {
            //DEBUG << "prefix " << iter2->first << " child " << *i;
            ostringstream s;
            s << "Additional" << ++count;
            int row_index = lpx_find_row(_lp, (char *)(s.str()).c_str());
            if (row_index == 0)
            {
                row_index = lpx_add_rows(_lp, 1);
                lpx_set_row_name(_lp, row_index, (char *)(s.str()).c_str());
                /*
                DEBUG << "added row = " << lpx_get_row_name(_lp, row_index)
                      << " at index = " << row_index;
                */
                row_count++;
            }
            else
            {
                WARN << "Row " << s.str() << " already exists!";
                continue;
            }

            ostringstream child_str;
            child_str << *i;
            string child_name = "x_" + child_str.str();
            int col_index_child = lpx_find_col(_lp, (char *)child_name.c_str());
            if (col_index_child == 0)
            {
                col_index_child  = lpx_add_cols(_lp, 1);
                lpx_set_col_name(_lp, col_index_child, 
                                 (char *)child_name.c_str());
                ++col_count;
            }
            ind[1] = col_index; // parent
            val[1] = 1.0;
            ind[2] = col_index_child;
            val[2] = -1.0;
            lpx_set_mat_row(_lp, row_index, 2, ind, val);
            lpx_set_row_bnds(_lp, row_index, LPX_DB, 0.0, 1.0);
        }
    }
    ufree(ind);
    ufree(val);

    //lpx_delete_index(_lp);

    lpx_order_matrix(_lp);

    if ((lpx_get_num_rows(_lp) > 0) && (lpx_get_num_cols(_lp) > 0))
    {
        string file_name = prob_name + ".lp"; 
        char *file_name2 = (char *)
                           malloc(sizeof(char)*(strlen(file_name.c_str())+1));
        strcpy(file_name2, file_name.c_str());
        //lpx_write_cpxlp(_lp, file_name2); // to minimize I/O
        /*
        string sys_com = "cat " + file_name;
        system(sys_com.c_str());
        */
    }
    else
    {
        WARN << "LoadSheddingProblem has 0 rows or 0 columns";
        result = false;
    }

    //DEBUG << "Done constructLP2()... " << result;

    return result;
}

// Things that we will have to change less often compared to the LP problem.
// 
void LoadShedder::prepare_stable_metadata()
{
    _drop_locations.clear();           // topology and distribution-dependent
    _load_shedder_nodes.clear();       // topology and distribution-dependent
    find_drop_locations();

    // We need the rates to compute the "fraction" for binary boxes (needed
    // for output cardinality estimation), in the case that quality metric 
    // is the tuple delivery. 
    // TODO: Can we get rid of this dependency on rates?
    //
    _stream_rates.clear();             // rate-dependent
    _input_rates.clear();              // rate-dependent
    get_stream_rates();

    _dl_prefixes.clear();              // topology-dependent
    _output_cardinality2.clear();      // rate-dependent (as explained above)
    find_dlprefixes();

    _output_total_selectivity.clear(); // selectivity-dependent 
    _output_input_mapping.clear();     // topology-dependent 
    compute_output_info();

    _load_factor_matrix2.clear();      // cost and selectivity-dependent
    generate_load_factor_matrix_2();
    
    /*
    NOTICE << "Load Factor Matrix:";
    for (NodeDLLoadFactorMap::iterator i = _load_factor_matrix2.begin();
         i != _load_factor_matrix2.end(); i++)
    {
        NOTICE << "Node " << i->first;
        for (DLLoadFactorMap::iterator j = (i->second).begin();
             j != (i->second).end(); j++)
        {
            NOTICE << j->first << " -> " << j->second;
        }
    }
    */

    _rate_factor_matrix.clear();      // topology and selectivity-dependent
    generate_rate_factor_matrix();

    /*
    NOTICE << "Rate Factor Matrix:";
    for (NodeDLInputRateFactorMap::iterator i = _rate_factor_matrix.begin();
         i != _rate_factor_matrix.end(); i++)
    {
        NOTICE << "Node " << i->first;
        for (map<int, pair<string, double> >::iterator j = (i->second).begin();
             j != (i->second).end(); j++)
        {
            NOTICE << j->first << " -> (" << (j->second).first << ", " 
                                         << (j->second).second << ")";
        }
    }
    */
    
    //_node_input_rates.clear();      // topology and rate-dependent (measured)
    _node_input_load_coefs.clear(); // topology, cost, and sel-dependent
    _node_input_rate_coefs.clear(); // topology and sel-dependent (estimated)
    generate_node_input_info();

    /*
    NOTICE << "Node input information:";
    NOTICE << "Measured rates:";
    for (map<string, map<string, double> >::iterator
         iter = _node_input_rates.begin();
         iter != _node_input_rates.end(); iter++)
    {
        NOTICE << "Node " << iter->first;
        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            NOTICE << iter2->first << " -> " << iter2->second;
        }
    }
    NOTICE << "Load coefs:";
    for (map<string, map<string, double> >::iterator
         iter = _node_input_load_coefs.begin();
         iter != _node_input_load_coefs.end(); iter++)
    {
        NOTICE << "Node " << iter->first;
        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            NOTICE << iter2->first << " -> " << iter2->second;
        }
    }
    NOTICE << "Estimated rate coefs:";
    for (map<string, map<string, map<string, double> > >::iterator
         iter = _node_input_rate_coefs.begin();
         iter != _node_input_rate_coefs.end(); iter++) 
    {
        NOTICE << "Node " << iter->first;
        for (map<string, map<string, double> >::iterator 
             iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            NOTICE << "Node input " << iter2->first;
            for (map<string, double>::iterator
                 iter3 = (iter2->second).begin();
                 iter3 != (iter2->second).end(); iter3++)
            {
                NOTICE << iter3->first << " -> " << iter3->second;
            }
        }
    }
    */
}

int LoadShedder::select_plan()
{
    DEBUG << "LoadShedder::select_plan()";

    int selected = 0; // default plan with no drops
    double max_score = 0;

    map<string, double> additional_reduction, additional_reduction_selected;
    additional_reduction.clear();
    additional_reduction_selected.clear();

    for (map<int, map<string, double> >::iterator
         iter = _precomputed_plan_ids.begin();
         iter != _precomputed_plan_ids.end(); iter++)
    {
        bool found = true;
        additional_reduction.clear();

        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            assert(_input_rates.find(iter2->first) != _input_rates.end());

            if (iter2->second > _input_rates[iter2->first])
            {
                found = false;
                break;
            }
            else
            {
                if (_input_rates[iter2->first] > 0)
                {
                    additional_reduction[iter2->first] = 
                                1.0 - iter2->second/_input_rates[iter2->first];
                }
                else
                {
                    additional_reduction[iter2->first] = 0;
                }
            }
        }

        if (found)
        {
            assert(_plan_scores.find(iter->first) != _plan_scores.end());

            double score = _plan_scores[iter->first];

            if (score > max_score)
            {
                selected = iter->first;
                max_score = score;
                additional_reduction_selected.clear();
                additional_reduction_selected.insert(
                    additional_reduction.begin(), additional_reduction.end());
            }
        }
    }

    // create the additional plans from the additional reduction.
    //
    map<string, vector<LSPlan::drop_box> > drops; // per node
    drops.clear();

    for (map<string, double>::iterator 
         iter = additional_reduction_selected.begin();
         iter != additional_reduction_selected.end(); iter++)
    {
        if (iter->second > 0.01) // if worth it
        {
            CatalogStream *stream = _catalog.get_stream(Name(iter->first));
            vector<BoxPort> *sinks = stream->get_sink_port();
            if (!sinks)
            {
                continue;
            }

            for (unsigned int i = 0; i < sinks->size(); i++)
            {
                if (((*sinks)[i].get_port_box()) == NULL)
                {
                    continue;
                }

                Name dest_box = ((*sinks)[i].get_port_box())->get_box_name();

                int id; string n; 

                if (_drop_locations_pernode.size() > 0) // FIT-based
                {
                    id = get_drop_location_id_pernode(iter->first,
                                                      dest_box, 
                                                      (*sinks)[i].get_port(),
                                                      n);
                    assert(id);

                    DropLocationPlus dl = _drop_locations_pernode[n][id];

                    LSPlan::drop_box d;
                    d.box_name = (string("drop_box_") + id);
                    d.box_type = dl.box_type;
                    d.node = dl.node;
                    BoxParameter::ParameterMap pm;
                    pm = *(dl.box_params.get_parameter_map_const());
                    ostringstream s;
                    s << iter->second;
                    pm["drop_rate"] = s.str();
                    d.box_params.set_parameter_map(&pm);
                    d.in_stream_name = dl.in_stream_name;
                    d.dest_box_name = dl.dest_box_name;
                    d.dest_box_port = dl.dest_box_port;
                    d.dest_node = dl.node; // this is acceptable here.

                    DEBUG << d.as_string();

                    drops[dl.node].push_back(d);
                }
                else // Solver-based
                {
                    id = get_drop_location_id(iter->first,
                                              dest_box, 
                                              (*sinks)[i].get_port());
                    assert(id);

                    DropLocation dl = _drop_locations[id];

                    LSPlan::drop_box d;
                    d.box_name = (string("drop_box_") + id);
                    d.box_type = dl.box_type;
                    d.node = dl.node;
                    BoxParameter::ParameterMap pm;
                    pm = *(dl.box_params.get_parameter_map_const());
                    ostringstream s;
                    s << iter->second;
                    pm["drop_rate"] = s.str();
                    d.box_params.set_parameter_map(&pm);
                    d.in_stream_name = dl.in_stream_name;
                    d.dest_box_name = dl.dest_box_name;
                    d.dest_box_port = dl.dest_box_port;
                    d.dest_node = dl.dest_node; 

                    DEBUG << d.as_string();

                    drops[dl.node].push_back(d);
                }
            }
        }
        else
        {
            NOTICE << "not worth it: " << iter->second;
        }
    }

    NOTICE << "Selected Plan";
    print_plan(selected);

    _additional_plans.clear();
    for (map<string, vector<LSPlan::drop_box> >::iterator iter = drops.begin();
         iter != drops.end(); iter++)
    {
        _additional_plans[iter->first] = LSPlan(iter->second);
        NOTICE << "Additional plan for node " << iter->first;
        NOTICE << (_additional_plans[iter->first]).as_string();
    }

    return selected;
}

void LoadShedder::change_plan_id(int old_id, int new_id)
{
    DEBUG << "change_plan_id " << old_id << " -> " << new_id;

    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            DEBUG << "Not preparing LS Plan for node " << *iter;
            continue;
        }

        //_node_plans[*iter]._plans[new_id] =
        //                            _node_plans[*iter]._plans[old_id];
        _node_plans[*iter]._plans[new_id].set(new LSPlan());
        (_node_plans[*iter]._plans[new_id])->set_plan(
                    (_node_plans[*iter]._plans[old_id])->_drop_insertion_plan);
        _node_plans[*iter]._plans.erase(old_id);

        DEBUG << (_node_plans[*iter]._plans[new_id])->as_string();
    }

    /*
    _tuple_delivery[new_id] = _tuple_delivery[old_id];
    _tuple_delivery.erase(old_id);
    */

    _plan_scores[new_id] = _plan_scores[old_id];
    _plan_scores.erase(old_id);

    /*
    string old_file = "LoadSheddingProblem";
    old_file += old_id;
    old_file += ".lp";
    string new_file = "LoadSheddingProblem";
    new_file += new_id;
    new_file += ".lp";
    string sys_com = "mv -f " + old_file + " " + new_file;
    system(sys_com.c_str());
    */
}

bool LoadShedder::add_default_plan()
{
    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            DEBUG << "Not preparing LS Plan for node " << *iter;
            continue;
        }

        DEBUG << "For node = " << *iter;

        _node_plans[*iter]._plans[0].set(new LSPlan());
    }

    /*
    // Update _tuple_delivery stats.
    //
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
                // if in ignore_nodes set, don't keep track of tuple delivery.
                //
                if (_ignore_nodes.find(out_node) != _ignore_nodes.end())
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            (_tuple_delivery[0])[out_node][(iter->first).as_string()] = 100;
        }
    }
    */

    return true;
}

// to be used with feasible rates only.
//
double LoadShedder::compute_score(map<string, double> rates)
{
    double total_score = 0;

    for (map<string, double>::iterator iter = rates.begin();
         iter != rates.end(); iter++)
    {
        string stream_name = iter->first;
        CatalogStream *stream = _catalog.get_stream(Name(stream_name));
        assert(stream->is_input_stream());

        vector<BoxPort> *sinks = stream->get_sink_port();
        for (unsigned int i = 0; i < sinks->size(); i++)
        {
            Name next_box_name = ((*sinks)[i].get_port_box())->get_box_name();
            int id = get_drop_location_id(Name(stream_name), next_box_name, 
                                                   (*sinks)[i].get_port());
            assert(id > 0);

            double total_sel = 0;

            map<string, double> outputs = _output_total_selectivity[id];
            for (map<string, double>::iterator iter2 = outputs.begin();
                 iter2 != outputs.end(); iter2++)
            {
                total_sel += iter2->second;
            }

            total_score += (iter->second) * total_sel;
        }
    }

    return total_score;
}

bool LoadShedder::add_new_plan(int id, map<string, double> rates)
{
    // If rates is already feasible, no need to generate a drop plan.
    //
    if (!(is_overloaded(rates)))
    {
        set<string>& node_addresses = *(_catalog.get_region_node());
        for (set<string>::const_iterator iter = node_addresses.begin();
             iter != node_addresses.end(); ++iter)
        {
            _node_plans[*iter]._plans[id].set(new LSPlan());
            _plan_scores[id] = compute_score(rates);
        }

        /*
        // Create a dummy problem file so that a later "mv" doesn't complain.
        //
        string file_name("LoadSheddingProblem");
        file_name += id;
        file_name += ".lp";
        string sys_com = "touch " + file_name;
        system(sys_com.c_str());
        */

        return true;
    }

    /*
    DEBUG << "LoadShedder::add_new_plan " << id << " with rates: ";
    for (map<string, double>::iterator iter = rates.begin();
         iter != rates.end(); iter++)
    {
        DEBUG << iter->first << " = " << iter->second;
    }
    */

    if (_lp)
    {
        lpx_delete_prob(_lp);
    }

    if (!construct_lp2(id, rates))
    {
        WARN << "LP not constructed properly";
        lpx_delete_index(_lp);
        return false;
    }

    if (solve_lp() == LPX_E_OK)
    {
        DEBUG << "Problem solved!";
        switch(lpx_get_status(_lp))
        {
            case LPX_OPT:
                DEBUG << "Solution is optimal";
                break;
            case LPX_FEAS:
                DEBUG << "Solution is feasible";
                break;
            case LPX_INFEAS:
                DEBUG << "Solution is infeasible";
                break;
            case LPX_NOFEAS:
                DEBUG << "There is no feasible solution";
                break;
            case LPX_UNBND:
                DEBUG << "Solution is unbounded";
                break;
            case LPX_UNDEF:
                DEBUG << "Solution is undefined";
                break;
            default:
                DEBUG << "Unknown solution status";
                break;
        }

        _plan_scores[id] = lpx_get_obj_val(_lp);

        /*
        DEBUG << "objective value = " << lpx_get_obj_val(_lp);
        for (int i = 1; i <= lpx_get_num_cols(_lp); i++)
        {
            DEBUG << lpx_get_col_name(_lp, i)  << " = "
                  << lpx_get_col_prim(_lp, i);
        }
        */
    }
    else
    {
        WARN << "Problem could not be solved!";
        lpx_delete_index(_lp);
        return false;
    }

    // Make assignments to the original drop variables.
    //
    _original_solution_variables.clear();
    for (DLPrefixMap::const_iterator iter = _dl_prefixes.begin();
         iter != _dl_prefixes.end(); iter++)
    {
        ostringstream col_index_str;
        col_index_str << iter->first;
        string col_name = "x_" + col_index_str.str();
        int index = lpx_find_col(_lp, (char *)col_name.c_str());
        if ((iter->second).size() == 1)
        {
            _original_solution_variables[(iter->second).back()] =
                                        lpx_get_col_prim(_lp, index);
            DEBUG << "original variable: " << (iter->second).back() << " = "
                  << _original_solution_variables[(iter->second).back()];
        }
        set<int> children;
        get_children(iter->first, &children);
        for (set<int>::iterator i = children.begin(); i != children.end(); i++)
        {
            ostringstream child_str;
            child_str << *i;
            string child_name = "x_" + child_str.str();
            int col_index_child = lpx_find_col(_lp, (char *)child_name.c_str());
            if (lpx_get_col_prim(_lp, index) > 0)
            {
                _original_solution_variables[_dl_prefixes[*i].back()] =
                                    lpx_get_col_prim(_lp, col_index_child)/
                                    lpx_get_col_prim(_lp, index);
            }
            else
            {
                _original_solution_variables[_dl_prefixes[*i].back()] = 1.0;
            }
            DEBUG << "original variable: " << _dl_prefixes[*i].back() << " = "
                  << _original_solution_variables[_dl_prefixes[*i].back()];
        }
    }

    DEBUG << "Preparing local plans...";
    
    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            DEBUG << "Not preparing LS Plan for node " << *iter;
            continue;
        }

        DEBUG << "For node = " << *iter;

        vector<LSPlan::drop_box> drops;
        drops.clear();

        for (DropLocationMap::const_iterator iter2 = _drop_locations.begin();
             iter2 != _drop_locations.end(); iter2++)
        {
            DEBUG << "For drop location = " << iter2->first;

            if ((iter2->second).node == *iter)
            {
                //int i = iter2->first;
                //double drop_rate = 1 - lpx_get_col_prim(_lp, i);
                double drop_rate = 1 -
                                   _original_solution_variables[iter2->first];
                //DEBUG << "drop_rate = " << drop_rate;
                if (drop_rate <= 0.001)
                {
                    // not worth it
                    continue;
                }
                LSPlan::drop_box d;
                d.box_name = (string("drop_box_") + iter2->first);
                d.box_type = (iter2->second).box_type;
                d.node = (iter2->second).node;
                //d.query_name = (iter2->second).query_name;
                BoxParameter::ParameterMap pm;
                pm = *((iter2->second).box_params.get_parameter_map_const());
                ostringstream s;
                s << drop_rate;
                pm["drop_rate"] = s.str();
                d.box_params.set_parameter_map(&pm);
                d.in_stream_name = (iter2->second).in_stream_name;
                d.dest_box_name = (iter2->second).dest_box_name;
                d.dest_box_port = (iter2->second).dest_box_port;
                d.dest_node = (iter2->second).dest_node;
                //DEBUG << d.as_string();
                drops.push_back(d);
            }
        }

        // new plan
        //
        if (drops.size() > 0)
        {
            _node_plans[*iter]._plans[id].set(new LSPlan(drops));
            DEBUG << (_node_plans[*iter]._plans[id])->as_string();
        }
        else
        {
            // empty plan: keep it so that plan id's are common at all nodes
            //
            _node_plans[*iter]._plans[id].set(new LSPlan());
            DEBUG << "empty plan";
        }
    }

    /*
    // Update _tuple_delivery stats.
    //
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
                // if in ignore_nodes set, don't keep track of tuple delivery.
                //
                if (_ignore_nodes.find(out_node) != _ignore_nodes.end())
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            (_tuple_delivery[id])[out_node][(iter->first).as_string()] = 0;
        }
    }

    for (DLPrefixMap::const_iterator iter = _dl_prefixes.begin();
         iter != _dl_prefixes.end(); iter++)
    {
        map<string, double> output_set = _output_cardinality2[iter->first];
        for (map<string, double>::iterator iter2 = output_set.begin();
             iter2 != output_set.end(); iter2++)
        {
            CatalogStream *stream = _catalog.get_stream(Name(iter2->first));
            string out_node;
            if (stream)
            {
                CatalogBox *box = stream->get_stream_source_box();
                if (box)
                {
                    out_node = box->get_box_node();
                }
                else
                {
                    WARN << "Stream " << iter2->first << " has no source box !";
                    out_node = "";
                }
            }
            else
            {
                WARN << "Stream " << iter2->first << " is not in the catalog !";
                out_node = "";
            }
            (_tuple_delivery[id])[out_node][iter2->first] +=
                    100 * _output_cardinality2[iter->first][iter2->first]
                        * lpx_get_col_prim(_lp, iter->first);
        }
    }
    */

    lpx_delete_index(_lp);

    return true;
}

int LoadShedder::plan_exists(map<string, double> rates, 
                             map<int, map<string, double> > plans)
{
    bool found;

    for (map<int, map<string, double> >::iterator iter = plans.begin();
         iter != plans.end(); iter++)
    {
        found = true;

        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            if (abs(rates[iter2->first] - iter2->second) > 0.0001)
            {
                found = false;
                break;
            }
        }
    
        if (found)
        {
            return iter->first;
        }
    }

    return 0;
}

void LoadShedder::upload_plans()
{
    DEBUG << "LoadShedder::upload_plans()";

    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            DEBUG << "Not upload LS Plan for node " << *iter;
            continue;
        }

        const string& node_address = *iter;

        DEBUG << "Uploading at " << node_address;
        //NOTICE << (_node_plans[node_address]).as_string();

        ptr<MedusaClient> client =
                            (ptr<MedusaClient>) new MedusaClient(node_address);

        Remote<LocalLoadShedder> remote_local_ls =
                            client->local_ls_for((MedusaID)node_address);

        if (remote_local_ls)
        {
            RPC<void> result = remote_local_ls.send_lsplans(
                                                   _node_plans[node_address]);
            DEBUG << "result : " << result;
            if (!result.stat())
            {
                WARN << "Plan upload to node " << node_address << " failed!";
            }
        }
        else
        {
            WARN << "No local load shedder for " << node_address;
        }
    }
}

// Subtract area2 from area1.
//
void LoadShedder::subtract(InfeasibleArea &area1, InfeasibleArea area2)
{
    for (InfeasibleArea::iterator it = area1.begin(); it != area1.end(); it++)
    {
        double low1 = (it->second).first;
        //double high1 = (it->second).second;
        double low2 = (area2[it->first]).first;
        //double high2 = (area2[it->first]).second;
    
        if (low1 < low2)
        {
            (area1[it->first]).second = low2 - 0.001;
        }
    }
}

pair<double, int> LoadShedder::generate(FeasibleInputTable::RateMap rates,
                                        map<string, double>::iterator iter,
                                        double pivot_value, int map_index)
{
    assert(iter != _input_rates.end());
    assert(pivot_value >= 0);

    map<string, double>::iterator current = iter;
    string current_input = current->first;

    /*
    NOTICE << current_input;
    NOTICE << "pivot = " << pivot_value;
    NOTICE << "index = " << map_index;
    */

    map<string, double>::iterator next = ++iter;

    if (next == _input_rates.end())
    {
        // current_input is the pivot.
        // Descend from pivot_value until the first feasible point is found.
        //
        bool found = false;
        do
        {
            rates[current_input] = pivot_value;

            if ((is_overloaded(rates)))
            {
                pivot_value -= FIXED_FIT_SPREAD;
            }
            else
            {
                found = true;
                break;
            }
        } while (pivot_value >= 0);

        if (found)
        {
            // Record rates as a feasible boundary point with a certain score.
            // Also, map the appropriate infeasibility area to this point.
            //

            double score = compute_score(rates);

            InfeasibleArea new_if_area; 
            for (FeasibleInputTable::RateMapIter it = rates.begin();
                 it != rates.end(); it++)
            {
                (new_if_area[it->first]).first  = it->second;
                (new_if_area[it->first]).second = MAX_RATE;
            }

            if (map_index >= 0)
            {
                if (_mappings[map_index].score > score) // old area still holds.
                {
                    subtract(new_if_area, _mappings[map_index].if_area);
                }
                else // old area must reduce.
                {
                    subtract(_mappings[map_index].if_area, new_if_area);
                }
            }

            Mapping m(new_if_area, rates, score);
            //NOTICE << m.as_string();

            _mappings.push_back(m);

            pair<double, int> result;
            result.first = pivot_value;
            result.second = (_mappings.size())-1;
            return result;
        }

        pair<double, int> result;
        result.first = -1;
        result.second = -1;
        return result;
    }
    else
    {
        unsigned int num_points = int(ceil(double(MAX_RATE)/
                                           double(FIXED_FIT_SPREAD))) + 1;
        double rate;
        double initial_pivot = pivot_value;
        int initial_map_index = map_index;

        for (unsigned int i = 0; i < num_points; i++)
        {
            rate = i * FIXED_FIT_SPREAD;
            
            if (rate > MAX_RATE)
            {
                break;
            }

            rates[current_input] = rate;

            pair<double, int> result;
            result = generate(rates, next, pivot_value, map_index);
            pivot_value = result.first;
            map_index = result.second;

            if (i == 0)
            {
                initial_pivot = pivot_value;
                initial_map_index = map_index;
            }

            if (pivot_value < 0)
            {
                // Rest of the values for current_input are infeasible.
                //
                break;
            }
        }

        pair<double, int> result;
        result.first = initial_pivot;
        result.second = initial_map_index;
        return result;
    }
}

// For the solver, this function generates the points on the boundary of
// the feasibility region.
//
void LoadShedder::generate_feasible_boundary()
{
    if (_input_rates.size() == 0)
    {
        WARN << "No input streams!";
        return;
    }

    FeasibleInputTable::RateMap rates; rates.clear();

    generate(rates, _input_rates.begin(), MAX_RATE, -1);

    NOTICE << "Total number of mappings = " << _mappings.size();
    for (unsigned int i = 0; i < _mappings.size(); i++)
    {
        NOTICE << _mappings[i].as_string();
    }
}

void LoadShedder::precompute_plans()
{
    generate_feasible_boundary();
    return;

    DEBUG << "LoadShedder::precompute_plans()";

    Time now;
    Time start_time = Time::now();    
    double time_passed = 0;

    double max_rate = MAX_RATE; // for now, we take this fixed for all inputs.
    double spread   = max_rate; // and also this.
    /*
    map<string, double> max_rate, spread;
    max_rate["input1"] = 58;
    max_rate["input2"] = 48;
    spread["input1"] = max_rate["input1"];
    spread["input2"] = max_rate["input2"];
    */

    // default plan first
    //
    add_default_plan();
    
    while (time_passed <= _offline_duration)
    {
        NOTICE << "Time left = " << _offline_duration - time_passed;
        NOTICE << "Spread = " << spread;

        unsigned int table_size = 1;
        map<string, int> num_points;
        num_points.clear();
        
        for (map<string, double>::iterator iter = _input_rates.begin();
             iter != _input_rates.end(); iter++)
        {
            /*
            num_points[iter->first] = (int(ceil(max_rate[iter->first]/
                                                spread[iter->first])) + 1);
            */
            num_points[iter->first] = (int(ceil(max_rate/spread)) + 1);
            table_size *= num_points[iter->first];
        }

        unsigned int repeat_count = table_size;
        unsigned int times;

        map<int, map<string, double> > new_plan_ids;
        new_plan_ids.clear();

        for (map<string, double>::iterator iter = _input_rates.begin();
             iter != _input_rates.end(); iter++)
        {
            repeat_count = repeat_count/num_points[iter->first];
            times = table_size/repeat_count;

            unsigned int k = 1;

            double rate = max_rate;

            for (unsigned int i = 0; i < times; i++)
            {
                for (unsigned int j = 0; j < repeat_count; j++)
                {
                    new_plan_ids[k][iter->first] = rate;
                    ++k;
                }
                
                if (rate >= spread)
                {
                    rate -= spread;
                }
                else if (abs(rate - 0) <= 0.00001) // practically zero
                {
                    rate = max_rate;
                }
                else if ((rate > 0) && (rate < spread))
                {
                    rate = 0;
                }
                else
                {
                    assert(false); // no such case
                }
            }

            assert(k == (table_size+1));
        }

        now = Time::now();
        time_passed = static_cast<double>((now - start_time).
                                          to_msecs());
        if (time_passed > _offline_duration)
        {
            NOTICE << "Time is up!";
            return;
        }

        int index;

        // 1. change the id's of the existing actual plans.
        //
        map<int, map<string, double> > temp;
        temp = _precomputed_plan_ids;
        /*
        for (map<int, map<string, double> >::reverse_iterator
             iter = _precomputed_plan_ids.rbegin();
             iter != _precomputed_plan_ids.rend(); iter++)
        */
        for (map<int, map<string, double> >::reverse_iterator
             iter = temp.rbegin();
             iter != temp.rend(); iter++)
        {
            index = plan_exists(iter->second, new_plan_ids);
            if (index)
            {
                if (iter->first != index)
                {
                    change_plan_id(iter->first, index);
                    _precomputed_plan_ids[index] = iter->second;
                    _precomputed_plan_ids.erase(iter->first);
                }
            }
            now = Time::now();
            time_passed = static_cast<double>((now - start_time).
                                              to_msecs());
            if (time_passed > _offline_duration)
            {
                NOTICE << "Time is up!";
                return;
            }
        }
        temp.clear();

        // 2. create the plans that don't exist already.
        //
        for (map<int, map<string, double> >::iterator
             iter = new_plan_ids.begin();
             iter != new_plan_ids.end(); iter++)
        {
            index = plan_exists(iter->second, _precomputed_plan_ids);
            if (!index)
            {
                // solve with iter->second
                // 
                add_new_plan(iter->first, iter->second);
                _precomputed_plan_ids[iter->first] = iter->second;

                now = Time::now();
                time_passed = static_cast<double>((now - start_time).
                                                  to_msecs());
                if (time_passed > _offline_duration)
                {
                    NOTICE << "Time is up!";
                    return;
                }
            }
            else
            {
                assert(index == iter->first);
            }
        }
        /*
        // 3. update the _precomputed_plan_ids.
        //
        _precomputed_plan_ids.clear();
        for (map<int, map<string, double> >::iterator
             iter = new_plan_ids.begin();
             iter != new_plan_ids.end(); iter++)
        {
            _precomputed_plan_ids[iter->first] = iter->second;
        }
        */

        // if there is still time left, we will reduce the spread 
        // and do one more pass.
        //
        /*
        spread["input1"] = spread["input1"]/2;
        spread["input2"] = spread["input2"]/2;
        */
        spread = spread/2;

        now = Time::now();
        time_passed = static_cast<double>((now - start_time).to_msecs());
    }
}

// To be called after statistics have been refreshed.
//
void LoadShedder::refresh_rates()
{
    // At drop location arcs and global diagram inputs.
    //
    //_stream_rates.clear();
    //_input_rates.clear();
    if (_drop_locations_pernode.size() > 0)
    {
        get_stream_rates_pernode();
    }
    else
    {
        get_stream_rates();
    }

    /*
    // At node inputs. // now, this is kind of redundant...
    //
    for (map<string, map<string, double> >::iterator
         iter = _node_input_rates.begin();
         iter != _node_input_rates.end(); iter++)
    {
        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            string node = iter->first;
            string stream_name = iter2->first;

            _node_input_rates[node][stream_name] = get_stream_rate(stream_name,
                                                                   node);
        }
    }
    */
}

void LoadShedder::compute_load_node_input(Name box_name, string node, 
                                          double *load)
{
    CatalogBox *box = _catalog.get_box(box_name);

    if (box->get_box_node() != node)
    {
        return;
    }

    try
    {
        const FixLenTimeSeries &f = _statistics[node].get_statistics(
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
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
        return;
    }
    catch(TSStats::error e)
    {
        WARN << "TSStats::Error " << (int)e;
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
        return;
    }
    catch(...)
    {
        WARN << "Unknown stats exception";
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
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
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
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
            double load_down = 0;
            Name dest_box_name = ((*sinks)[j].get_port_box())->get_box_name();
            compute_load_node_input(dest_box_name, node, &load_down);
            (*load) += selectivity * load_down;
        }
    }
}

void LoadShedder::assign_rate_coefs(Name box_name, 
                                    string input_name, 
                                    double sel)
{
    CatalogBox *box = _catalog.get_box(box_name);

    string node = box->get_box_node(); 

    CatalogStream::StreamStar *out_streams = box->get_box_out();
    for (unsigned int i = 0; i < out_streams->size(); i++)
    {
        CatalogStream *out_stream = (*out_streams)[i];
        string selectivity_id = box_name.as_string() + " : " +
                                out_stream->get_stream_source()->get_port();
        double selectivity;
        try
        {
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                        "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
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
            if ((*sinks)[j].get_port_box() == NULL)
            {
                continue;
            }

            string next_node = ((*sinks)[j].get_port_box())->get_box_node();

            if (next_node != node)
            {
                _node_input_rate_coefs[next_node]
                                      [out_stream->get_stream_name_string()]
                                      [input_name] += selectivity*sel;
            }

            assign_rate_coefs(((*sinks)[j].get_port_box())->get_box_name(),
                              input_name, 
                              selectivity*sel);
        }
    }
}

bool LoadShedder::is_cross_stream(Name stream_name)
{
    CatalogStream *stream = _catalog.get_stream(stream_name);

    if (stream->is_input_stream())
    {
        return false;
    }

    CatalogBox *box = stream->get_stream_source_box();
    assert(box);

    string src_node = box->get_box_node();

    vector<BoxPort> *sinks = stream->get_sink_port();

    if (!sinks)
    {
        return false;
    }

    for (unsigned int i = 0; i < sinks->size(); i++)
    {
        if (((*sinks)[i].get_port_box()) == NULL)
        {
            continue;
        }

        string dest_node = ((*sinks)[i].get_port_box())->get_box_node();

        if (src_node != dest_node)
        {
            return true;
        }
    }

    return false;
}

void LoadShedder::generate_node_input_info()
{
    // Initialize.
    //
    Catalog::StreamMap *streams = _catalog.get_stream_map();

    for (Catalog::StreamMap::iterator iter = streams->begin();
         iter != streams->end(); ++iter)
    {
        CatalogStream& catalog_stream = iter->second;

        if ((catalog_stream.is_input_stream()) ||
            (is_cross_stream(catalog_stream.get_stream_name())))
        {
            vector<BoxPort> *sinks = catalog_stream.get_sink_port();

            if (!sinks)
            {
                continue;
            }

            for (unsigned int i = 0; i < sinks->size(); i++)
            {
                if (((*sinks)[i].get_port_box()) == NULL)
                {
                    continue;
                }

                string node = ((*sinks)[i].get_port_box())->get_box_node();

                if (_ignore_nodes.find(node) != _ignore_nodes.end())
                {
                    continue;
                }

                //_node_input_rates[node][(iter->first).as_string()] = 0;
                _node_input_load_coefs[node][(iter->first).as_string()] = 0;

                for (set<string>::const_iterator iter = _input_streams.begin();
                     iter != _input_streams.end(); ++iter)
                {
                    _node_input_rate_coefs
                                      [node]
                                      [catalog_stream.get_stream_name_string()]
                                      [*iter] = 0;
                }
            }
        }
    }

    // Fill in.
    //
    for (map<string, map<string, double> >::iterator
         iter = _node_input_load_coefs.begin();
         iter != _node_input_load_coefs.end(); iter++)
    {
        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            string node = iter->first;
            string stream_name = iter2->first;

            CatalogStream *stream = _catalog.get_stream(Name(iter2->first));

            vector<BoxPort> *sinks = stream->get_sink_port();
            if (!sinks)
            {
                continue;
            }

            for (unsigned int i = 0; i < sinks->size(); i++)
            {
                if (((*sinks)[i].get_port_box()) == NULL)
                {
                    continue;
                }

                if (((*sinks)[i].get_port_box())->get_box_node() != node)
                {
                    continue;
                }
            
                double load_down = 0;
                Name box_name = ((*sinks)[i].get_port_box())->get_box_name();
                compute_load_node_input(box_name, node, &load_down);
                _node_input_load_coefs[node][stream_name] += load_down;
            }

            //_node_input_rates[node][stream_name] = 
            //    get_stream_rate(stream_name, node);
        }
    }
    for (set<string>::const_iterator iter = _input_streams.begin();
         iter != _input_streams.end(); ++iter)
    {
        CatalogStream *stream = _catalog.get_stream(Name(*iter));
        vector<BoxPort> *sinks = stream->get_sink_port();
        if (!sinks)
        {
            continue;
        }

        for (unsigned int i = 0; i < sinks->size(); i++)
        {
            if (((*sinks)[i].get_port_box()) == NULL)
            {
                continue;
            }

            string node = ((*sinks)[i].get_port_box())->get_box_node();

            _node_input_rate_coefs[node][*iter][*iter] = 1.0;

            assign_rate_coefs(((*sinks)[i].get_port_box())->get_box_name(), 
                              *iter, 
                              1.0);
        }
    }
}

// We generate new problem based on current stream rates.
// We solve it and create a new load shedding plan.
// Then we actually apply that plan on the local nodes.
//
bool LoadShedder::generate_new_plan()
{
    if (_lp)
    {
        lpx_delete_prob(_lp);
    }

    if (!construct_lp2(1, _input_rates))
    {
        WARN << "LP not constructed properly";
        lpx_delete_index(_lp);
        return false;
    }

    if (solve_lp() == LPX_E_OK)
    {
        DEBUG << "Problem solved!";
        switch(lpx_get_status(_lp))
        {
            case LPX_OPT:
                DEBUG << "Solution is optimal";
                break;
            case LPX_FEAS:
                DEBUG << "Solution is feasible";
                break;
            case LPX_INFEAS:
                DEBUG << "Solution is infeasible";
                break;
            case LPX_NOFEAS:
                DEBUG << "There is no feasible solution";
                break;
            case LPX_UNBND:
                DEBUG << "Solution is unbounded";
                break;
            case LPX_UNDEF:
                DEBUG << "Solution is undefined";
                break;
            default:
                DEBUG << "Unknown solution status";
                break;
        }
        DEBUG << "objective value = " << lpx_get_obj_val(_lp);
        for (int i = 1; i <= lpx_get_num_cols(_lp); i++)
        {
            DEBUG << lpx_get_col_name(_lp, i)  << " = "
                  << lpx_get_col_prim(_lp, i);
        }
    }
    else
    {
        WARN << "Problem could not be solved!";
        lpx_delete_index(_lp);
        return false;
    }

    // Make assignments to the original drop variables.
    //
    _original_solution_variables.clear();
    for (DLPrefixMap::const_iterator iter = _dl_prefixes.begin();
         iter != _dl_prefixes.end(); iter++)
    {
        ostringstream col_index_str;
        col_index_str << iter->first;
        string col_name = "x_" + col_index_str.str();
        int index = lpx_find_col(_lp, (char *)col_name.c_str());
        if ((iter->second).size() == 1)
        {
            _original_solution_variables[(iter->second).back()] =
                                        lpx_get_col_prim(_lp, index);
            DEBUG << "original variable: " << (iter->second).back() << " = "
                  << _original_solution_variables[(iter->second).back()];
        }
        set<int> children;
        get_children(iter->first, &children);
        for (set<int>::iterator i = children.begin(); i != children.end(); i++)
        {
            ostringstream child_str;
            child_str << *i;
            string child_name = "x_" + child_str.str();
            int col_index_child = lpx_find_col(_lp, (char *)child_name.c_str());
            if (lpx_get_col_prim(_lp, index) > 0)
            {
                _original_solution_variables[_dl_prefixes[*i].back()] =
                                    lpx_get_col_prim(_lp, col_index_child)/
                                    lpx_get_col_prim(_lp, index);
            }
            else
            {
                _original_solution_variables[_dl_prefixes[*i].back()] = 1.0;
            }
            DEBUG << "original variable: " << _dl_prefixes[*i].back() << " = "
                  << _original_solution_variables[_dl_prefixes[*i].back()];
        }
    }

    NOTICE << "Preparing local plans...";

    map<string, LSPlanMessage> plans;

    _node_plans.clear(); // will refill

    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            DEBUG << "Not preparing LS Plan for node " << *iter;
            continue;
        }

        DEBUG << "For node = " << *iter;

        vector<LSPlan::drop_box> drops;
        drops.clear();

        for (DropLocationMap::const_iterator iter2 = _drop_locations.begin();
             iter2 != _drop_locations.end(); iter2++)
        {
            DEBUG << "For drop location = " << iter2->first;

            if ((iter2->second).node == *iter)
            {
                //int i = iter2->first;
                //double drop_rate = 1 - lpx_get_col_prim(_lp, i);
                double drop_rate = 1 -
                                   _original_solution_variables[iter2->first];
                //DEBUG << "drop_rate = " << drop_rate;
                if (drop_rate <= 0.001)
                {
                    // not worth it
                    continue; 
                }
                LSPlan::drop_box d;
                d.box_name = (string("drop_box_") + iter2->first);
                d.box_type = (iter2->second).box_type;
                d.node = (iter2->second).node;
                //d.query_name = (iter2->second).query_name;
                BoxParameter::ParameterMap pm;
                pm = *((iter2->second).box_params.get_parameter_map_const());
                ostringstream s;
                s << drop_rate;
                pm["drop_rate"] = s.str();
                d.box_params.set_parameter_map(&pm);
                d.in_stream_name = (iter2->second).in_stream_name;
                d.dest_box_name = (iter2->second).dest_box_name;
                d.dest_box_port = (iter2->second).dest_box_port;
                d.dest_node = (iter2->second).dest_node;
                //DEBUG << d.as_string();
                drops.push_back(d);
            }
        }

        // default plan
        //
        plans[*iter]._plans[0].set(new LSPlan());

        // new plan
        //
        if (drops.size() > 0)
        {
            plans[*iter]._plans[1].set(new LSPlan(drops));
        }
        else
        {
            // empty plan: keep it so that plan id's are common at all nodes
            //
            plans[*iter]._plans[1].set(new LSPlan());
        }

        // refresh regional copy
        //
        _node_plans[*iter] = plans[*iter];

        DEBUG << "Node Plan for " << *iter << " at the Regional LoadShedder:";
        DEBUG << _node_plans[*iter].string_rep();
    }

    // Update _tuple_delivery stats.
    //
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
                // if in ignore_nodes set, don't keep track of tuple delivery.
                //
                if (_ignore_nodes.find(out_node) != _ignore_nodes.end())
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            (_tuple_delivery[0])[out_node][(iter->first).as_string()] = 100.0;
            (_tuple_delivery[1])[out_node][(iter->first).as_string()] = 0;
        }
    }

    for (DLPrefixMap::const_iterator iter = _dl_prefixes.begin();
         iter != _dl_prefixes.end(); iter++)
    {
        map<string, double> output_set = _output_cardinality2[iter->first];
        for (map<string, double>::iterator iter2 = output_set.begin();
             iter2 != output_set.end(); iter2++)
        {
            CatalogStream *stream = _catalog.get_stream(Name(iter2->first));
            string out_node;
            if (stream)
            {
                CatalogBox *box = stream->get_stream_source_box();
                if (box)
                {
                    out_node = box->get_box_node();
                }
                else
                {
                    WARN << "Stream " << iter2->first << " has no source box !";
                    out_node = "";
                }
            }
            else
            {
                WARN << "Stream " << iter2->first << " is not in the catalog !";
                out_node = "";
            }
            (_tuple_delivery[1])[out_node][iter2->first] +=
                    100 * _output_cardinality2[iter->first][iter2->first]
                        * lpx_get_col_prim(_lp, iter->first);
        }
    }

    NOTICE << "Uploading local plans...";

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            DEBUG << "Not upload LS Plan for node " << *iter;
            continue;
        }

        const string& node_address = *iter;

        NOTICE << "Uploading at " << node_address;

        ptr<MedusaClient> client =
                            (ptr<MedusaClient>) new MedusaClient(node_address);

        Remote<LocalLoadShedder> remote_local_ls =
                            client->local_ls_for((MedusaID)node_address);

        if (remote_local_ls)
        {
            RPC<void> result = remote_local_ls.send_lsplans(
                                                        plans[node_address]);
            DEBUG << "result : " << result;
            if (!result.stat())
            {
                WARN << "Plan upload to node " << node_address << " failed!";
            }
        }
        else
        {
            DEBUG << "No local load shedder for " << node_address;
        }
    }

    lpx_delete_index(_lp);

    shed_load(1);

    return true; // ??? make sure we are actually dropping stuff
}

bool LoadShedder::generate_lsplans()
{
    NOTICE << "Generating plans...";

    _drop_locations.clear();
    _load_shedder_nodes.clear();
    _input_streams.clear();
    find_drop_locations();

    _stream_rates.clear();
    _input_rates.clear();
    get_stream_rates();

    /*
    // NON-SPLIT STUFF
    //
    _load_factor_matrix.clear();
    generateLoadFactorMatrix();

    _output_cardinality.clear();
    computeOutputCardinalities();

    _stream_rates.clear();
    getStreamRates();

    if (_lp)
    {
        lpx_delete_prob(_lp);
    }

    if (!constructLP())
    {
        WARN << "LP not constructed properly";
        return false;
    }
    */

    _dl_prefixes.clear();
    _output_cardinality2.clear();
    find_dlprefixes();

    _output_total_selectivity.clear();
    _output_input_mapping.clear();
    compute_output_info();

    _load_factor_matrix2.clear();
    generate_load_factor_matrix_2();
    
    DEBUG << "Load Factor Matrix:";
    for (NodeDLLoadFactorMap::iterator i = _load_factor_matrix2.begin();
         i != _load_factor_matrix2.end(); i++)
    {
        DEBUG << "Node " << i->first;
        for (DLLoadFactorMap::iterator j = (i->second).begin();
             j != (i->second).end(); j++)
        {
            DEBUG << j->first << " -> " << j->second;
        }
    }
     
    _rate_factor_matrix.clear();
    generate_rate_factor_matrix();
    
    if (_lp)
    {
        lpx_delete_prob(_lp);
    }

    if (!construct_lp2(1, _input_rates))
    {
        WARN << "LP not constructed properly";
        lpx_delete_index(_lp);
        return false;
    }

    if (solve_lp() == LPX_E_OK)
    {
        DEBUG << "Problem solved!";
        switch(lpx_get_status(_lp))
        {
            case LPX_OPT:
                DEBUG << "Solution is optimal";
                break;
            case LPX_FEAS:
                DEBUG << "Solution is feasible";
                break;
            case LPX_INFEAS:
                DEBUG << "Solution is infeasible";
                break;
            case LPX_NOFEAS:
                DEBUG << "There is no feasible solution";
                break;
            case LPX_UNBND:
                DEBUG << "Solution is unbounded";
                break;
            case LPX_UNDEF:
                DEBUG << "Solution is undefined";
                break;
            default:
                DEBUG << "Unknown solution status";
                break;
        }
        DEBUG << "objective value = " << lpx_get_obj_val(_lp);
        for (int i = 1; i <= lpx_get_num_cols(_lp); i++)
        {
            DEBUG << lpx_get_col_name(_lp, i)  << " = "
                  << lpx_get_col_prim(_lp, i);
        }
    }
    else
    {
        WARN << "Problem could not be solved!";
        lpx_delete_index(_lp);
        return false;
    }

    // Make assignments to the original drop variables.
    //
    _original_solution_variables.clear();
    for (DLPrefixMap::const_iterator iter = _dl_prefixes.begin();
         iter != _dl_prefixes.end(); iter++)
    {
        ostringstream col_index_str;
        col_index_str << iter->first;
        string col_name = "x_" + col_index_str.str();
        int index = lpx_find_col(_lp, (char *)col_name.c_str());
        if ((iter->second).size() == 1)
        {
            _original_solution_variables[(iter->second).back()] =
                                        lpx_get_col_prim(_lp, index);
            DEBUG << "original variable: " << (iter->second).back() << " = "
                  << _original_solution_variables[(iter->second).back()];
        }
        set<int> children;
        get_children(iter->first, &children);
        for (set<int>::iterator i = children.begin(); i != children.end(); i++)
        {
            ostringstream child_str;
            child_str << *i;
            string child_name = "x_" + child_str.str();
            int col_index_child = lpx_find_col(_lp, (char *)child_name.c_str());
            if (lpx_get_col_prim(_lp, index) > 0)
            {
                _original_solution_variables[_dl_prefixes[*i].back()] =
                                    lpx_get_col_prim(_lp, col_index_child)/
                                    lpx_get_col_prim(_lp, index);
            }
            else
            {
                _original_solution_variables[_dl_prefixes[*i].back()] = 1.0;
            }
            DEBUG << "original variable: " << _dl_prefixes[*i].back() << " = "
                  << _original_solution_variables[_dl_prefixes[*i].back()];
        }
    }

    NOTICE << "Preparing local plans...";

    map<string, LSPlanMessage> plans;

    _node_plans.clear(); // will refill

    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            //DEBUG << "Not preparing LS Plan for node " << *iter;
            continue;
        }

        //DEBUG << "For node = " << *iter;

        vector<LSPlan::drop_box> drops;
        drops.clear();

        for (DropLocationMap::const_iterator iter2 = _drop_locations.begin();
             iter2 != _drop_locations.end(); iter2++)
        {
            //DEBUG << "For drop location = " << iter2->first;

            if ((iter2->second).node == *iter)
            {
                //int i = iter2->first;
                //double drop_rate = 1 - lpx_get_col_prim(_lp, i);
                double drop_rate = 1 -
                                   _original_solution_variables[iter2->first];
                //DEBUG << "drop_rate = " << drop_rate;
                if (drop_rate <= 0.001)
                {
                    // not worth it
                    continue; 
                }
                LSPlan::drop_box d;
                d.box_name = (string("drop_box_") + iter2->first);
                d.box_type = (iter2->second).box_type;
                d.node = (iter2->second).node;
                //d.query_name = (iter2->second).query_name;
                BoxParameter::ParameterMap pm;
                pm = *((iter2->second).box_params.get_parameter_map_const());
                ostringstream s;
                s << drop_rate;
                pm["drop_rate"] = s.str();
                d.box_params.set_parameter_map(&pm);
                d.in_stream_name = (iter2->second).in_stream_name;
                d.dest_box_name = (iter2->second).dest_box_name;
                d.dest_box_port = (iter2->second).dest_box_port;
                d.dest_node = (iter2->second).dest_node;
                //DEBUG << d.as_string();
                drops.push_back(d);
            }
        }

        // default plan
        //
        plans[*iter]._plans[0].set(new LSPlan());

        // new plan
        //
        if (drops.size() > 0)
        {
            plans[*iter]._plans[1].set(new LSPlan(drops));
        }
        else
        {
            // empty plan: keep it so that plan id's are common at all nodes
            //
            plans[*iter]._plans[1].set(new LSPlan());
        }

        // refresh regional copy
        //
        _node_plans[*iter] = plans[*iter];

        DEBUG << "Node Plan for " << *iter << " at the Regional LoadShedder:";
        DEBUG << _node_plans[*iter].string_rep();
    }

    // Update _tuple_delivery stats.
    //
    Catalog::StreamMap *streams = _catalog.get_stream_map();
    for (Catalog::StreamMap::iterator iter = streams->begin();
         iter != streams->end(); ++iter)
    {
        CatalogStream& catalog_stream = iter->second;

        if (catalog_stream.is_output_stream())
        {
            CatalogBox *box = catalog_stream.get_stream_source_box();
            string out_node;
            //bool pass_thru = false;

            if (box)
            {
                out_node = box->get_box_node();
                // if in ignore_nodes set, don't keep track of tuple delivery.
                //
                if (_ignore_nodes.find(out_node) != _ignore_nodes.end())
                {
                    continue;
                }
            }
            else
            {
                continue;
                /*
                // Demo Hack: pass-thru hack for the demo
                //
                if (!_ignore_nodes.empty())
                {
                    out_node = *(_ignore_nodes.begin());
                    pass_thru = true;
                }
                else
                {
                    out_node = "";
                }
                */
            }

            (_tuple_delivery[0])[out_node][(iter->first).as_string()] = 100.0;
            (_tuple_delivery[1])[out_node][(iter->first).as_string()] = 0;
            // for all

            /*
            if (pass_thru)
            {
                (_tuple_delivery[1])[out_node][(iter->first).as_string()] =
                                                                        100.0;
                pass_thru = false;
            }
            else
            {
                (_tuple_delivery[1])[out_node][(iter->first).as_string()] = 0;
            }
            */
        }
    }

    for (DLPrefixMap::const_iterator iter = _dl_prefixes.begin();
         iter != _dl_prefixes.end(); iter++)
    {
        map<string, double> output_set = _output_cardinality2[iter->first];
        for (map<string, double>::iterator iter2 = output_set.begin();
             iter2 != output_set.end(); iter2++)
        {
            CatalogStream *stream = _catalog.get_stream(Name(iter2->first));
            string out_node;
            if (stream)
            {
                CatalogBox *box = stream->get_stream_source_box();
                if (box)
                {
                    out_node = box->get_box_node();
                }
                else
                {
                    WARN << "Stream " << iter2->first << " has no source box !";
                    out_node = "";
                }
            }
            else
            {
                WARN << "Stream " << iter2->first << " is not in the catalog !";
                out_node = "";
            }
            (_tuple_delivery[1])[out_node][iter2->first] +=
                    100 * _output_cardinality2[iter->first][iter2->first]
                        * lpx_get_col_prim(_lp, iter->first);
        }
    }

    NOTICE << "Uploading local plans...";

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            //DEBUG << "Not upload LS Plan for node " << *iter;
            continue;
        }

        const string& node_address = *iter;

        NOTICE << "Uploading at " << node_address;

        ptr<MedusaClient> client =
                            (ptr<MedusaClient>) new MedusaClient(node_address);

        Remote<LocalLoadShedder> remote_local_ls =
                            client->local_ls_for((MedusaID)node_address);

        if (remote_local_ls)
        {
            RPC<void> result = remote_local_ls.send_lsplans(
                                                        plans[node_address]);
            DEBUG << "result : " << result;
            if (!result.stat())
            {
                WARN << "Plan upload to node " << node_address << " failed!";
            }
        }
        else
        {
            DEBUG << "No local load shedder for " << node_address;
        }
    }

    lpx_delete_index(_lp);
    return true;
}

void LoadShedder::shed_load(int plan_id)
{
    NOTICE << "Shedding load (plan_id = " << plan_id << ")...";

    bool no_planid = false;
    if (plan_id == _current_plan_id)
    {
        NOTICE << "Same plan, no need to send plan id's...";
        no_planid = true;
    }

    if (plan_id == 0)
    {
        _additional_plans.clear();
    }

    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            DEBUG << "Not sending LS Plan for node " << *iter;
            continue;
        }

        const string& node_address = *iter;

        ptr<MedusaClient> client =
                            (ptr<MedusaClient>) new MedusaClient(node_address);

        Remote<LocalLoadShedder> remote_local_ls =
                            client->local_ls_for((MedusaID)node_address);

        if (!remote_local_ls)
        {
            WARN << "No local load shedder for " << node_address;
            continue;
        }

        // send plan id only when it is different.
        //
        if (!no_planid)
        {
               NOTICE << "Sending plan id to the remote local optimizer..";
               RPC<void> result = remote_local_ls.send_plan_id(plan_id);
               NOTICE << "result : " << result;
               if (!result.stat())
               {
                   WARN << "Load shedding at node " << node_address 
                     << " failed!";
                   continue;
               }
        }
    
        // send the additional plan in all cases (if there is any).
        //
        if (_additional_plans.find(node_address) != _additional_plans.end())
        {
            NOTICE << "Sending additional plan to the remote local optimizer..";
            RPC<void> result = remote_local_ls.send_additional_plan(
                                           _additional_plans[node_address]);
            NOTICE << "result : " << result;
            if (!result.stat())
            {
                WARN << "Additional plan at node " << node_address 
                     << " failed!";
                continue;
            }
        }
    }

    _current_plan_id = plan_id;

    NOTICE << "Plan is sent...";
}

LoadShedder::TupleDeliveryMap LoadShedder::get_current_tuple_delivery()
{
    if (_tuple_delivery.find(_current_plan_id) != _tuple_delivery.end())
    {
        return(_tuple_delivery[_current_plan_id]);
    }

    DEBUG << "Returning empty tuple delivery map...";
    TupleDeliveryMap empty_map;
    empty_map.clear();
    return(empty_map);
}

void LoadShedder::add_ignore_node(string node_name)
{
    if (_ignore_nodes.find(node_name) == _ignore_nodes.end())
    {
        //DEBUG << "Will ignore node " << node_name;
        _ignore_nodes.insert(node_name);
    }
    else
    {
        WARN << "Already ignoring node " << node_name;
    }
}

/*
// Game-specific implementation.
// Streams that straightly come from the game node to another node
// are "input streams".
//
bool LoadShedder::isInputStream(string stream_name)
{
    DEBUG << stream_name;

    CatalogStream *stream = _catalog.getStream(Name(stream_name));

    if (!stream)
    {

        WARN << "Stream " << stream_name << " does not exist in the catalog!";
        return false;
    }
    else
    {

        if (_ignore_nodes.empty())
        {
            DEBUG << "Ignore nodes is empty";
            return(stream->isInputStream());
        }

        if (stream->isInputStream()) // all real inputs go to the game node
        { 
            DEBUG << "no";
            return false;
        }
        else
        {
            // check if it goes from game node to another node
            //
            CatalogBox *src = stream->getStreamSourceBox();
            string src_node = src->getBoxNode();
            if (_ignore_nodes.find(src_node) == _ignore_nodes.end())
            {
                DEBUG << "no";
                return false;
            }
            else
            {
                DEBUG << "going remote?";
                vector<BoxPort> *sinks = stream->getSinkPort();
                for (unsigned int i = 0; i < sinks->size(); i++)
                {
                    string dest_node = ((*sinks)[i].getPortBox())->getBoxNode();
                    if (_ignore_nodes.find(dest_node) == _ignore_nodes.end())
                    {
                        DEBUG << "yes";
                        return true;
                    }
                }
            }
        }

        // Use below instead when source subscriptions are properly implemented.
        //
        //CatalogSubscription *sub = stream->getSourceSubscription();
        //if (sub)
        //{
        //    InetAddress address = sub->getSubscriptionEndpoint();
        //    DEBUG << "Stream " << stream_name << " has a remote source: "
        //          << address.as_string();
        //    if (_ignore_nodes.find(to_string(address))
        //                                    != _ignore_nodes.end())
        //    {
        //        DEBUG << "So, it is an input stream!";
        //        return true;
        //    }
        //}
        //else
        //{
        //    DEBUG << "sub is NULL";
        //}
    }

    DEBUG << "no";
    return false;
}
*/

bool LoadShedder::is_start_location(int loc_id)
{
    if (_drop_locations.find(loc_id) == _drop_locations.end())
    {
        WARN << "No such drop location: " << loc_id;
        return false;
    }
    CatalogStream *stream = _catalog.get_stream(
                            _drop_locations[loc_id].in_stream_name);
    assert(stream);
    if (stream->is_input_stream())
    {
        //DEBUG << stream->getStreamNameString() << " is an input stream";
        return true;
    }
    else
    {
        CatalogBox *box = stream->get_stream_source_box();
        string box_name = box->get_box_name().as_string();
        assert(box);
        CatalogStream::StreamStar *in_streams = box->get_box_in();
        for (unsigned int i = 0; i < in_streams->size(); i++)
        {
            CatalogStream *in_stream = (*in_streams)[i];
            vector<BoxPort> *sinks = in_stream->get_sink_port();
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                if (((*sinks)[j].get_port_box())->get_box_name() == box_name)
                {
                    int id;
                    if ((id = get_drop_location_id(in_stream->get_stream_name(),
                                                   box_name, 
                                                   (*sinks)[j].get_port())))
                    {
                        //DEBUG << id;
                        return false;
                    }
                }
            }
            if (in_stream->is_input_stream())
            {
                /*
                DEBUG << in_stream->getStreamNameString()
                      << " is an input stream";
                */
                continue;
            }
            else
            {
                bool result;
                result = exists_upstream_location(in_stream->get_stream_name());
                if (result)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool LoadShedder::exists_upstream_location(Name stream_name)
{
    CatalogStream *stream = _catalog.get_stream(stream_name);
    assert(stream);
    if (stream->is_input_stream())
    {
        //DEBUG << stream->getStreamNameString() << " is an input stream";
        return false;
    }
    else
    {
        CatalogBox *box = stream->get_stream_source_box();
        string box_name = box->get_box_name().as_string();
        assert(box);
        CatalogStream::StreamStar *in_streams = box->get_box_in();
        for (unsigned int i = 0; i < in_streams->size(); i++)
        {
            CatalogStream *in_stream = (*in_streams)[i];
            vector<BoxPort> *sinks = in_stream->get_sink_port();
            for (unsigned int j = 0; j < sinks->size(); j++)
            {
                if (((*sinks)[j].get_port_box())->get_box_name() == box_name)
                {
                    int id;
                    if ((id = get_drop_location_id(in_stream->get_stream_name(),
                                                   box_name, 
                                                   (*sinks)[j].get_port())))
                    {
                        //DEBUG << id;
                        return true;
                    }
                }
            }
            if (in_stream->is_input_stream())
            {
                /*
                DEBUG << in_stream->getStreamNameString()
                      << " is an input stream";
                */
                continue;
            }
            else
            {
                bool result;
                result = exists_upstream_location(in_stream->get_stream_name());
                if (result)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool LoadShedder::compute_node_loads()
{
    bool overloaded = false;

    set<string>& node_addresses = *(_catalog.get_region_node());

    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); iter++)
    {
        if (_ignore_nodes.find(*iter) != _ignore_nodes.end())
        {
            //DEBUG << "Not computing node load node " << *iter;
            continue;
        }

        double load = 0;

        for (DLPrefixMap::const_iterator iter2 = _dl_prefixes.begin();
             iter2 != _dl_prefixes.end(); iter2++)
        {
            double coef = _load_factor_matrix2[*iter][iter2->first]
                          * _stream_rates[(iter2->second).back()];
            //DEBUG << "coef for prefix " << iter2->first << " : " << coef;

            load += coef;
        }

        _node_loads[*iter] = load;

        NOTICE << "Load at node " << *iter << ": " << load;

        if (load > CAPACITY)
        {
            _overloaded_nodes[*iter] = true;
            overloaded = true;
        }
        else
        {
            _overloaded_nodes[*iter] = false;
        }
    }

    return overloaded;
}

// Based on original load coefs and diagram input rates.
//
bool LoadShedder::compute_node_loads_original()
{
    bool overloaded = false;

    for (map<string, map<string, map<string, double> > >::iterator
         iter = _node_input_rate_coefs.begin();
         iter != _node_input_rate_coefs.end(); iter++) // nodes
    {
        double node_load = 0;

        for (map<string, map<string, double> >::iterator 
             iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++) // node inputs
        {
            double load_coef = _node_input_load_coefs[iter->first]
                                                     [iter2->first];
            double total_rate_coef = 0;

            for (map<string, double>::iterator
                 iter3 = (iter2->second).begin();
                 iter3 != (iter2->second).end(); iter3++) // diagram inputs
            {
                total_rate_coef += _node_input_rate_coefs[iter->first]
                                                         [iter2->first]
                                                         [iter3->first]
                                   * _input_rates[iter3->first];
            }

            node_load += load_coef * total_rate_coef;
        }

        _node_loads[iter->first] = node_load;

        NOTICE << "Load at node " << iter->first << ": " << node_load;

        if (node_load > CAPACITY)
        {
            _overloaded_nodes[iter->first] = true;
            overloaded = true;
        }
        else
        {
            _overloaded_nodes[iter->first] = false;
        }
    }

    return overloaded;         
}

// Would there be any overload situation with the given input rates?
//
bool LoadShedder::is_overloaded(map<string, double> rates)
{
    for (map<string, map<string, map<string, double> > >::iterator
         iter = _node_input_rate_coefs.begin();
         iter != _node_input_rate_coefs.end(); iter++) // nodes
    {
        double node_load = 0;

        for (map<string, map<string, double> >::iterator 
             iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++) // node inputs
        {
            double load_coef = _node_input_load_coefs[iter->first]
                                                     [iter2->first];
            double total_rate_coef = 0;

            for (map<string, double>::iterator
                 iter3 = (iter2->second).begin();
                 iter3 != (iter2->second).end(); iter3++) // diagram inputs
            {
                total_rate_coef += _node_input_rate_coefs[iter->first]
                                                         [iter2->first]
                                                         [iter3->first]
                                   * rates[iter3->first];
            }

            node_load += load_coef * total_rate_coef;
        }

        if (node_load > CAPACITY)
        {
            return true;
        }
    }

    return false;         
}

/************************ FIT-based version *****************************/

/*
bool LoadShedder::compute_node_loads_FIT()
{
    bool overloaded = false;

    for (map<string, InputInfoMap>::iterator 
         iter1 = _input_info_pernode.begin();
         iter1 != _input_info_pernode.end(); iter1++)
    {
        double node_load = 0;

        for (InputInfoMap::iterator iter2 = (iter1->second).begin();
             iter2 != (iter1->second).end(); iter2++)
        {
            double total_rate_coef = 0;

            node_load += _input_rates[iter2->first] *
                         (_input_info_pernode[iter1->first][iter2->first].
                                                          total_load_coef);
        }

        _node_loads[iter1->first] = node_load;

        NOTICE << "Load at node " << iter1->first << ": " << node_load;

        if (node_load > 1.0)
        {
            _overloaded_nodes[iter1->first] = true;
            overloaded = true;
        }
        else
        {
            _overloaded_nodes[iter1->first] = false;
        }
    }

    return overloaded;         
}
*/

int LoadShedder::get_drop_location_id_pernode(Name stream, Name box_name, 
                                              uint16 sink_port, string &node)
{
    for (map<string, DropLocationPlusMap>::iterator 
         iter1 = _drop_locations_pernode.begin();
         iter1 != _drop_locations_pernode.end();
         iter1++)
    {
        for (DropLocationPlusMap::iterator iter2 = (iter1->second).begin();
             iter2 != (iter1->second).end(); iter2++)
        {
            Name dl_stream = (iter2->second).in_stream_name;
            Name dl_box = (iter2->second).dest_box_name;
            uint16 dl_port = (iter2->second).dest_box_port;

            if ((stream == dl_stream) && (box_name == dl_box) &&
                (sink_port == dl_port))
            {
                node = (iter2->second).node;
                return(iter2->first);
            }
        }
    }

    return 0;
}

void LoadShedder::find_dlproperties_pernode(Name box_name, string drop_type,
                                       BoxParameter::ParameterMap box_params)
{
    int loc_id;

    CatalogBox *box = _catalog.get_box(box_name);
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
    // else, nothing to do...

    CatalogStream::StreamStar *in_streams = box->get_box_in();

    for (unsigned int i = 0; i < in_streams->size(); i++)
    {
        CatalogStream *in_stream = (*in_streams)[i];
        vector<BoxPort> *sinks = in_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            if (((*sinks)[j].get_port_box())->get_box_name() == box_name)
            {
                string node;
                if ((loc_id = get_drop_location_id_pernode(
                                                in_stream->get_stream_name(),
                                                box_name, 
                                                (*sinks)[j].get_port(),
                                                node)))
                {
                    _drop_locations_pernode[node][loc_id].box_type = drop_type;
                    _drop_locations_pernode[node][loc_id].box_params.
                                        set_parameter_map(&box_params);
                }
            }
        }

        if (in_stream->is_input_stream())
        {
            // there is some redundant checking here, but should work correctly
            continue;
        }
        else
        {
            find_dlproperties_pernode(
                        in_stream->get_stream_source_box()->get_box_name(),
                        drop_type, box_params);
        }
    }
}

void LoadShedder::find_drop_locations_pernode()
{
    int dl_count = 0;

    // All input arcs are potential drop locations.
    //
    Catalog::StreamMap *streams = _catalog.get_stream_map();

    for (Catalog::StreamMap::iterator iter = streams->begin();
         iter != streams->end(); ++iter)
    {
        CatalogStream& catalog_stream = iter->second;

        if (catalog_stream.is_input_stream())
        {
            _input_streams.insert((iter->first).as_string());
        }

        if ((catalog_stream.is_input_stream()) ||
            (is_cross_stream(catalog_stream.get_stream_name())))
        {
            vector<BoxPort> *sinks = catalog_stream.get_sink_port();

            if (!sinks)
            {
                continue;
            }

            string node;
            int good_sink_count = 0;
            for (unsigned int i = 0; i < sinks->size(); i++)
            {
                if (((*sinks)[i].get_port_box()) == NULL)
                {
                    continue;
                }

                ++good_sink_count;
                DropLocationPlus loc;
                loc.id = ++dl_count;
                loc.in_stream_name = catalog_stream.get_stream_name();
                loc.dest_box_name = 
                                ((*sinks)[i].get_port_box())->get_box_name();
                loc.dest_box_port = (*sinks)[i].get_port();
                loc.node = ((*sinks)[i].get_port_box())->get_box_node();
                node = loc.node; // simply assuming all sinks at the same node
                _drop_locations_pernode[loc.node][loc.id] = loc;
                _input_info_pernode[loc.node][(iter->first).as_string()].
                                                dl_id.push_back(loc.id);
            }

            if (good_sink_count > 1) // we need a separate one for the input
            {
                DropLocationPlus loc;
                loc.id = ++dl_count;
                loc.in_stream_name = catalog_stream.get_stream_name();
                loc.input_flag = true;
                loc.input_stream = catalog_stream.get_stream_name_string();
                _drop_locations_pernode[node][loc.id] = loc;
                _input_info_pernode[node][(iter->first).as_string()].dl_id.
                    insert(_input_info_pernode[node][(iter->first).as_string()].
                           dl_id.begin(), loc.id);
            }
        }
    }

    // All fan-out branches of a split box are potential drop locations.
    //
    Catalog::BoxMap *boxes = _catalog.get_box_map();

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
                    continue;
                }

                for (unsigned int j = 0; j < sinks->size(); j++)
                {
                    DropLocationPlus loc;
                    loc.id = ++dl_count;
                    loc.in_stream_name = stream->get_stream_name();
                    loc.dest_box_name = 
                                ((*sinks)[j].get_port_box())->get_box_name();
                    loc.dest_box_port = (*sinks)[j].get_port();

                    // if just one sink, then drop can go to the source node.
                    // this way, we also save bandwidth.
                    // otherwise, have to go the sink node due to sharing.
                    //
                    if (sinks->size() == 1)
                    {
                        // first, try the source
                        //
                        loc.node = 
                            stream->get_stream_source_box()->get_box_node();
                    }
                    else
                    {
                        // let's try the sink
                        //
                        loc.node = ((*sinks)[j].get_port_box())->get_box_node();
                    }

                    _drop_locations_pernode[loc.node][loc.id] = loc;
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
            vector<CatalogSubscription*> *subs =
                                    catalog_stream.get_subscribers();
            if (subs)
            {
                DEBUG << "number of subs = " << subs->size();
                for (unsigned int i = 0; i < subs->size(); i++)
                {
                    //DEBUG << (*subs)[i]->getGap();
                    if ((*subs)[i]->get_gap() < min_gap)
                    {
                        min_gap = (*subs)[i]->get_gap();
                    }
                }
            }
            /*
            DEBUG << "minimum gap for output stream \"" << iter->first
                  << "\" = " << min_gap;
            */
            pm["max_batch_size"] = string() + min_gap;

            if (catalog_stream.get_stream_source_box() == NULL)
            {
                //DEBUG << "Stream " << iter->first << " is pass-thru";
                continue;
            }

            find_dlproperties_pernode(catalog_stream.get_stream_source_box()
                                      ->get_box_name(), "random_drop", pm);
        }
    }
}

bool LoadShedder::is_leaf_node(string node)
{
    return (_sink_servers_pernode[node].empty());
}

bool LoadShedder::is_root_node(string node)
{
    return (_source_servers_pernode[node].empty());
}

void LoadShedder::find_source_sink_servers()
{
    Catalog::StreamMap *streams = _catalog.get_stream_map();

    for (Catalog::StreamMap::iterator iter = streams->begin();
         iter != streams->end(); ++iter)
    {
        CatalogStream& stream = iter->second;

        if (stream.is_input_stream())
        {
            continue;
        }

        CatalogBox *box = stream.get_stream_source_box();
        assert(box);

        string src_node = box->get_box_node();

        vector<BoxPort> *sinks = stream.get_sink_port();

        if (!sinks)
        {
            continue;
        }

        for (unsigned int i = 0; i < sinks->size(); i++)
        {
            if (((*sinks)[i].get_port_box()) == NULL)
            {
                continue;
            }

            string dest_node = ((*sinks)[i].get_port_box())->get_box_node();

            if (src_node != dest_node)
            {
                _source_servers_pernode[dest_node].insert(src_node);
                _sink_servers_pernode[src_node].insert(dest_node);
                if (!(_input_info_pernode[dest_node]
                          [stream.get_stream_name_string()].source_server))
                {
                    _input_info_pernode[dest_node]
                           [stream.get_stream_name_string()].source_server 
                                            = new InetAddress(src_node);
                }
            }
        }
    }
}

void LoadShedder::compute_load_down(Name box_name, double *load,
                                    double *quality)
{
    CatalogBox *box = _catalog.get_box(box_name);
    string node = box->get_box_node();

    try
    {
        const FixLenTimeSeries &f = _statistics[node].get_statistics(
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
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
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
            DEBUG << "number of subscriptions = " << num_sub;
            // HACK until subscriptions can be reflected properly !!!
            if (num_sub == 0)
            {
                num_sub = 1;
            }
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
                if (next_box->get_box_node() != node) // output sink
                { 
                    DEBUG << "output sink (node boundary)";
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
            (*quality) += selectivity * quality_down; 
        }
    }

    DEBUG << "load = " << *load;
    DEBUG << "quality = " << *quality;
}

void LoadShedder::compute_load_coefs_pernode()
{
    for (map<string, DropLocationPlusMap>::iterator 
         iter1 = _drop_locations_pernode.begin();
         iter1 != _drop_locations_pernode.end();
         iter1++)
    {
        string node = iter1->first;

        for (DropLocationPlusMap::iterator iter2 = (iter1->second).begin();
             iter2 != (iter1->second).end(); iter2++)
        {
    /*
        for (DropLocationPlusMap::iterator 
             iter2 = (_drop_locations_pernode[node]).begin(); 
             iter2 != (_drop_locations_pernode[node]).end(); iter2++)
        {
    */
            string box_name = (iter2->second).dest_box_name.as_string();
            double load = 0;
            double quality = 0;

            try
            {
                const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                                "Cost", box_name);
                try
                {
                    double avg = f.get_average();
                    load += avg;
                }
                catch(...)
                {
                    WARN << "Could not get \"Average Cost\" for box "
                         << box_name;
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

            CatalogBox *box = _catalog.get_box(Name(box_name));
            CatalogStream::StreamStar *out_streams = box->get_box_out();
            for (unsigned int i = 0; i < out_streams->size(); i++)
            {
                CatalogStream *out_stream = (*out_streams)[i];
                string selectivity_id = box_name + " : " +
                        out_stream->get_stream_source()->get_port();
                double selectivity;
                try
                {
                    const FixLenTimeSeries &f = 
                                        _statistics[node].get_statistics(
                                            "Selectivity", selectivity_id);
                    try
                    {
                        selectivity = f.get_average();
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
                    WARN << "Could not read \"Selectivity\" for " 
                         << selectivity_id;
                    return;
                }
                catch(TSStats::error e)
                {
                    WARN << "TSStats::Error " << (int)e;
                    WARN << "Could not read \"Selectivity\" for " 
                         << selectivity_id;
                    return;
                }
                catch(...)
                {
                    WARN << "Unknown stats exception";
                    WARN << "Could not read \"Selectivity\" for " 
                         << selectivity_id;
                    return;
                }

                if (out_stream->is_output_stream())
                {
                    int num_sub = (out_stream->get_subscribers())->size();
                    DEBUG << "number of subscriptions = " << num_sub;
                    // HACK until subscriptions can be reflected properly !!!
                    if (num_sub == 0)
                    {
                        num_sub = 1;
                    }
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
                        DEBUG << "output sink (no next box)";
                        quality_down = 1.0;
                    }
                    else
                    {
                        if (next_box->get_box_node() != node)
                        {
                            DEBUG << "output sink (node boundary)";
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
                    quality += selectivity * quality_down;
                }
            }

            _drop_locations_pernode[node][iter2->first].load_coef = load;
            _drop_locations_pernode[node][iter2->first].reduced_load_coef = 
                                                        load; // initialize
            _drop_locations_pernode[node][iter2->first].quality_score = quality;
            _drop_locations_pernode[node][iter2->first].reduced_quality_score =
                                                        quality; // initialize

        }
    }
}

void LoadShedder::compute_input_info_pernode()
{
    for (map<string, InputInfoMap>::iterator 
         iter1 = _input_info_pernode.begin();
         iter1 != _input_info_pernode.end(); iter1++)
    {
        string node = iter1->first;
    
        for (InputInfoMap::iterator iter2 = (iter1->second).begin();
             iter2 != (iter1->second).end(); iter2++)
        {
    /*
        for (InputInfoMap::iterator iter2 = (_input_info_pernode[node]).begin();
             iter2 != (_input_info_pernode[node]).end(); iter2++)
        {
    */
            string stream_name = iter2->first;

            double load_coef = 0;
            double quality = 0;
            unsigned int k = 0;
            if ((iter2->second).dl_id.size() > 1)
            {
                ++k;
            }

            for (unsigned int i = k; i < (iter2->second).dl_id.size(); i++)
            {
                if ((_drop_locations_pernode[node]).
                                             find(((iter2->second).dl_id)[i]) ==
                    _drop_locations_pernode[node].end())
                {
                    WARN << "Drop location \"" << ((iter2->second).dl_id)[i]
                         << "\" does not exist at node " << node;
                }
                else
                {
                    load_coef += _drop_locations_pernode[node]
                                    [((iter2->second).dl_id)[i]].load_coef;
                    quality += _drop_locations_pernode[node]
                                    [((iter2->second).dl_id)[i]].quality_score;
                }
            }

            (iter2->second).total_load_coef = load_coef;
            (iter2->second).reduced_load_coef = load_coef; // initialize
            (iter2->second).total_quality = quality;

            if (load_coef > 0)
            {
                (iter2->second).max_rate = 1.0/load_coef; 
            }
            else
            {
                WARN << "Zero load coefficient.";
            }

            if (_drop_locations_pernode[node]
                        [((iter2->second).dl_id)[0]].input_flag)
            {
                _drop_locations_pernode[node]
                        [((iter2->second).dl_id)[0]].load_coef = load_coef;
                _drop_locations_pernode[node]
                        [((iter2->second).dl_id)[0]].quality_score = quality;
            }
        }
    }
}

void LoadShedder::compute_input_spread_pernode(string node)
{
    for (InputInfoMap::iterator iter = (_input_info_pernode[node]).begin(); 
         iter != (_input_info_pernode[node]).end(); iter++)
    {
        /*
        double temp = (_input_info_pernode[node]).size() 
                       * (iter->second).reduced_quality;
        double e_max = 
        if (temp > 0)
        {
            (iter->second).spread = TOTAL_FIT_ERROR/temp;
        }
        else
        {
            WARN << "Problem in spread for " << iter->first;
            (iter->second).spread = FIXED_FIT_SPREAD;
        }
        */
        (iter->second).spread = (iter->second).rate_bound 
                                * double(_percent_fit_error) / double(100);
        if ((iter->second).spread <= 0)
        {
            (iter->second).spread = FIXED_FIT_SPREAD;
        }
        DEBUG << "Spread along " << iter->first << " = " 
              << (iter->second).spread;
    }
}

void LoadShedder::collect_dl_from_subtree_pernode(string node, Name stream_name,
                                                  Name box_name, double sel)
{
    DEBUG << "collect_dl_from_subtree_pernode";
    DEBUG << node << " " << stream_name.as_string() << " " 
          << box_name.as_string() << " " << sel;

    CatalogBox *box = _catalog.get_box(box_name);

    if (!box)
    {
        return;
    }

    if (box->get_box_node() != node)
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
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
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

            string n;
            int dl_id = get_drop_location_id_pernode(
                                            out_stream->get_stream_name(),
                                            next_box->get_box_name(),
                                            (*sinks)[j].get_port(), n);
            DEBUG << dl_id;
            if (n != node)
            {
                continue;
            }

            if (dl_id > 0)
            {
                _drop_locations_pernode[n][dl_id].input_stream = 
                                            stream_name.as_string();
                _drop_locations_pernode[n][dl_id].rate_coef = sel*selectivity;

                _input_info_pernode[n][stream_name.as_string()].dl_subtree.
                                                     push_back(dl_id);
            }

            collect_dl_from_subtree_pernode(n, stream_name,
                                ((*sinks)[j].get_port_box())->get_box_name(),
                                sel*selectivity);
        }
    }
}

// Pick the dl with the smallest ratio.
//
int LoadShedder::pick_dl_pernode(string node, Name stream_name)
{
    DEBUG << "pick_dl_pernode " << node << " " << stream_name.as_string();

    double min_ratio = 1000;
    int min_dl = 0;

    for (unsigned int i = 0; 
         i < _input_info_pernode[node][stream_name.as_string()].
             dl_subtree.size(); 
         i++)
    {
        int dl_id = (_input_info_pernode[node][stream_name.as_string()].
                    dl_subtree)[i];
        assert(_drop_locations_pernode[node].find(dl_id) 
               != _drop_locations_pernode[node].end());
        DEBUG << "checking " << dl_id;

        if (_drop_locations_pernode[node][dl_id].reduced_load_coef <= 0)
        {
            WARN << "Negative reduced load coefficient.";
        }

        double ratio = 
                _drop_locations_pernode[node][dl_id].reduced_quality_score/
                _drop_locations_pernode[node][dl_id].reduced_load_coef;

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

vector<int>::iterator LoadShedder::get_dl_iter_pernode(string node,
                                                       Name stream_name, 
                                                       int dl_id)
{
    vector<int>::iterator r, iter;
    r = _input_info_pernode[node][stream_name.as_string()].dl_subtree.end();
    
    for (iter = _input_info_pernode[node][stream_name.as_string()].
                dl_subtree.begin(); 
         iter != _input_info_pernode[node][stream_name.as_string()].
                  dl_subtree.end(); 
         iter++)
    {
        if ((*iter) == dl_id)
        {
            return iter;
        }
    }

    DEBUG << "Couldn't find the dl iter for: " 
          << node << " " << stream_name.as_string() << " " << dl_id;
    return r;
}

void LoadShedder::eliminate_dl_subtree_pernode(string node, Name stream_name, 
                                               Name next_box_name)
{
    DEBUG << "eliminate_dl_subtree_pernode " << node << " " 
          << stream_name.as_string() << " " << next_box_name.as_string();

    CatalogBox *box = _catalog.get_box(next_box_name);

    if (!box)
    {
        return;
    }

    if (box->get_box_node() != node)
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

            string n;
            int dl_id = get_drop_location_id_pernode(
                                            out_stream->get_stream_name(),
                                            next_box->get_box_name(),
                                            (*sinks)[j].get_port(), n);
            if (n != node)
            {
                continue;
            }

            DEBUG << dl_id;
            if (dl_id > 0)
            {
                vector<int>::iterator iter = get_dl_iter_pernode(n, stream_name,
                                                                 dl_id);
                if (iter != _input_info_pernode[n][stream_name.as_string()].
                            dl_subtree.end())
                {
                    _input_info_pernode[n][stream_name.as_string()].
                                            dl_subtree.erase(iter);
                }
            }

            eliminate_dl_subtree_pernode(node, stream_name,
                                 ((*sinks)[j].get_port_box())->get_box_name());
        }
    }
}

// Walk back from box, reducing load and quality as necessary.
//
void LoadShedder::update_dl_supertree_pernode(string node, Name stream_name, 
                                              int min_dl, CatalogBox *box)
{
    CatalogStream::StreamStar *in_streams = box->get_box_in();

    for (unsigned int i = 0; i < in_streams->size(); i++)
    {
        CatalogStream *in_stream = (*in_streams)[i];
        vector<BoxPort> *sinks = in_stream->get_sink_port();
        for (unsigned int j = 0; j < sinks->size(); j++)
        {
            int loc_id;
            string n;
            if ((loc_id = get_drop_location_id_pernode(
                                            in_stream->get_stream_name(),
                                            box->get_box_name(),
                                            (*sinks)[j].get_port(), n)))
            {
                if (n != node)
                {
                    continue;
                }

                DEBUG << "reducing from location " << loc_id;

                _drop_locations_pernode[n][loc_id].reduced_load_coef -=
                    _drop_locations_pernode[n][min_dl].reduced_load_coef *
                    (_drop_locations_pernode[n][min_dl].rate_coef/
                     _drop_locations_pernode[n][loc_id].rate_coef);
                _drop_locations_pernode[n][loc_id].reduced_quality_score -=
                    _drop_locations_pernode[n][min_dl].reduced_quality_score *
                    (_drop_locations_pernode[n][min_dl].rate_coef/
                     _drop_locations_pernode[n][loc_id].rate_coef);
            }
        }

        if (in_stream->get_stream_name() == stream_name)
        {
            DEBUG << "reducing from input stream " << stream_name.as_string();

            if (_input_info_pernode[node][stream_name.as_string()].
                                                    dl_id.size() > 1)
            {
                DEBUG << "it is a split one..";

                int input_dl = 
                    _input_info_pernode[node][stream_name.as_string()].dl_id[0];

                _drop_locations_pernode[node][input_dl].reduced_load_coef -=
                    _drop_locations_pernode[node][min_dl].reduced_load_coef *
                    (_drop_locations_pernode[node][min_dl].rate_coef/
                     _drop_locations_pernode[node][input_dl].rate_coef);
                _drop_locations_pernode[node][input_dl].reduced_quality_score -=
                    _drop_locations_pernode[node][min_dl].reduced_quality_score*
                    (_drop_locations_pernode[node][min_dl].rate_coef/
                     _drop_locations_pernode[node][input_dl].rate_coef);
                _input_info_pernode[node][stream_name.as_string()].
                    reduced_load_coef = _drop_locations_pernode[node][input_dl].
                    reduced_load_coef;
                _input_info_pernode[node][stream_name.as_string()].
                    reduced_quality = _drop_locations_pernode[node][input_dl].
                    reduced_quality_score;
            }
            else
            {
                DEBUG << "it is NOT a split one..";

                int input_dl = 
                    _input_info_pernode[node][stream_name.as_string()].dl_id[0];

                _input_info_pernode[node][stream_name.as_string()].
                    reduced_load_coef = _drop_locations_pernode[node][input_dl].
                    reduced_load_coef;
                _input_info_pernode[node][stream_name.as_string()].
                    reduced_quality = _drop_locations_pernode[node][input_dl].
                    reduced_quality_score;
                DEBUG << "reduced_load_coef = "
                      << _input_info_pernode[node][stream_name.as_string()].
                         reduced_load_coef;
                DEBUG << "reduced_quality = "
                      << _input_info_pernode[node][stream_name.as_string()].
                         reduced_quality;
            }
        }
        else 
        {
            update_dl_supertree_pernode(node, stream_name, min_dl,
                                in_stream->get_stream_source_box());
        }
    }
}

void LoadShedder::sort_dl_subtree_pernode(string node, Name stream_name)
{
    DEBUG << "sort_dl_subtree_pernode " << node << " " 
          << stream_name.as_string();

    while ((_input_info_pernode[node][stream_name.as_string()]).
            dl_subtree.size() > 0)
    {
        DEBUG << "Drop locations before the sort iteration";
        for (DropLocationPlusMap::iterator 
             iter = _drop_locations_pernode[node].begin();
             iter != _drop_locations_pernode[node].end(); iter++)
        {
            DEBUG << (iter->second).as_string();
        }

        int min_dl = pick_dl_pernode(node, stream_name);

        if (min_dl == 0) // can not be picked for some bad reason, so stop.
        {
            // complain
            WARN << "min_dl can not be picked";
            break;
        }

        // Transfer min_dl to the sorted list.
        //
        (_input_info_pernode[node][stream_name.as_string()].dl_subtree_sorted).
                                                            push_back(min_dl);
    
        // Eliminate all drop locations (from dl_subtree)
        // in min_dl's subtree, including min_dl.
        //
        vector<int>::iterator iter = get_dl_iter_pernode(node, stream_name, 
                                                         min_dl);
        assert(iter != _input_info_pernode[node][stream_name.as_string()].
                        dl_subtree.end());
        _input_info_pernode[node][stream_name.as_string()].
                        dl_subtree.erase(iter);
        eliminate_dl_subtree_pernode(node, stream_name,
                          _drop_locations_pernode[node][min_dl].dest_box_name);

        // Update load and quality for drop locations (in dl_subtree)
        // in min_dl's supertree.
        //
        Name in_stream_name = _drop_locations_pernode[node][min_dl].
                                                    in_stream_name;
        if (in_stream_name != stream_name) // not at the root yet.
        {
            CatalogStream *in_stream = _catalog.get_stream(in_stream_name);
            CatalogBox *in_box = in_stream->get_stream_source_box();
            update_dl_supertree_pernode(node, stream_name, min_dl, in_box);
        }
        else
        {
            DEBUG << "reducing from input stream " << stream_name.as_string();

            if (_input_info_pernode[node][stream_name.as_string()].
                                                    dl_id.size() > 1)
            {
                DEBUG << "it is a split one..";

                int input_dl = 
                    _input_info_pernode[node][stream_name.as_string()].dl_id[0];

                _drop_locations_pernode[node][input_dl].reduced_load_coef -=
                    _drop_locations_pernode[node][min_dl].reduced_load_coef *
                    (_drop_locations_pernode[node][min_dl].rate_coef/
                     _drop_locations_pernode[node][input_dl].rate_coef);
                _drop_locations_pernode[node][input_dl].reduced_quality_score -=
                    _drop_locations_pernode[node][min_dl].reduced_quality_score*
                    (_drop_locations_pernode[node][min_dl].rate_coef/
                     _drop_locations_pernode[node][input_dl].rate_coef);
                _input_info_pernode[node][stream_name.as_string()].
                    reduced_load_coef = _drop_locations_pernode[node][input_dl].
                    reduced_load_coef;
                _input_info_pernode[node][stream_name.as_string()].
                    reduced_quality = _drop_locations_pernode[node][input_dl].
                    reduced_quality_score;
            }
            else
            {
                DEBUG << "it is NOT a split one..";

                int input_dl = 
                    _input_info_pernode[node][stream_name.as_string()].dl_id[0];

                _input_info_pernode[node][stream_name.as_string()].
                    reduced_load_coef = _drop_locations_pernode[node][input_dl].
                    reduced_load_coef;
                _input_info_pernode[node][stream_name.as_string()].
                    reduced_quality = _drop_locations_pernode[node][input_dl].
                    reduced_quality_score;
                DEBUG << "reduced_load_coef = "
                      << _input_info_pernode[node][stream_name.as_string()].
                           reduced_load_coef;
                DEBUG << "reduced_quality = "
                      << _input_info_pernode[node][stream_name.as_string()].
                           reduced_quality;
            }
        }
    }
}

void LoadShedder::collect_dl_for_inputs_pernode(string node)
{
    //for (map<string, InputInfoMap>::iterator 
    //     iter1 = _input_info_pernode.begin();
    //     iter1 != _input_info_pernode.end(); iter1++)
    //{
    //    string node = iter1->first;
    //    DEBUG << node;
    //
    //    for (InputInfoMap::iterator iter2 = (iter1->second).begin();
    //         iter2 != (iter1->second).end(); iter2++)
    //    {
        for (InputInfoMap::iterator iter2 = (_input_info_pernode[node]).begin();
             iter2 != (_input_info_pernode[node]).end(); iter2++)
        {
            string stream_name = iter2->first;
            DEBUG << stream_name;

            if ((iter2->second).dl_id.size() < 1)
            {
                WARN << "Invalid number of drop locations for stream "
                      << iter2->first;
            }

            CatalogStream *stream = _catalog.get_stream(Name(stream_name));

            _drop_locations_pernode[node][((iter2->second).dl_id)[0]].
                                                input_stream = stream_name;
            _drop_locations_pernode[node][((iter2->second).dl_id)[0]].
                                                rate_coef = 1.0;
            (iter2->second).dl_subtree.push_back(((iter2->second).dl_id)[0]);

            if ((iter2->second).dl_id.size() == 1)
            {
                collect_dl_from_subtree_pernode(node, Name(stream_name),
                    _drop_locations_pernode[node][((iter2->second).dl_id)[0]].
                                                        dest_box_name, 1.0);
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
                    string n;
                    int dl_id = get_drop_location_id_pernode(Name(stream_name),
                                ((*sinks)[j].get_port_box())->get_box_name(),
                                (*sinks)[j].get_port(),
                                n);
                    assert(dl_id > 0);
                    assert(node == n);

                    _drop_locations_pernode[n][dl_id].input_stream = 
                                                                stream_name;
                    _drop_locations_pernode[n][dl_id].rate_coef = 1.0;
                    (iter2->second).dl_subtree.push_back(dl_id);

                    collect_dl_from_subtree_pernode(n, Name(stream_name),
                                ((*sinks)[j].get_port_box())->get_box_name(),
                                1.0);
                }
            }

            sort_dl_subtree_pernode(node, Name(stream_name));

            if ((iter2->second).reduced_load_coef > 0)
            {
                (iter2->second).rate_bound = 1.0/
                                         ((iter2->second).reduced_load_coef);
                DEBUG << "Input " << iter2->first << " rate_bound  " 
                      << (iter2->second).rate_bound;
            }
            else
            {
                WARN << "Invalid reduced load coefficient!";
            }
        }
    //}
}

// Merge-sort dl_subtree_sorted from all inputs into one sorted list.
//
void LoadShedder::determine_dl_sort_order_pernode(string node)
{
    _dl_sort_order[node].clear();

    // where we are on each input's dl_subtree_sorted
    map<string, unsigned int> pointers; 

    for (InputInfoMap::iterator iter = _input_info_pernode[node].begin();
         iter != _input_info_pernode[node].end(); iter++)
    {
        pointers[iter->first] = 0;
    }

    while (1)
    {
        string selected_input = "";
        double selected_ratio;

        for (InputInfoMap::iterator iter = _input_info_pernode[node].begin();
             iter != _input_info_pernode[node].end(); iter++)
        {
            if (pointers[iter->first] < 
                (iter->second).dl_subtree_sorted.size())
            {
                if (selected_input == "")
                {
                    selected_input = iter->first;
                    DropLocationPlus d;
                    d = _drop_locations_pernode[node]
                      [(iter->second).dl_subtree_sorted[pointers[iter->first]]];
                    selected_ratio = d.reduced_quality_score/
                                     d.reduced_load_coef;
                }
                else
                {
                    DropLocationPlus d;
                    d = _drop_locations_pernode[node]
                      [(iter->second).dl_subtree_sorted[pointers[iter->first]]];
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

        _dl_sort_order[node].push_back(
                            _input_info_pernode[node][selected_input].
                            dl_subtree_sorted[pointers[selected_input]]);
        DEBUG << "DL: " << _input_info_pernode[node][selected_input].
                           dl_subtree_sorted[pointers[selected_input]];
        pointers[selected_input] += 1;
    }
}

void LoadShedder::prepare_node_metadata()
{
    DEBUG << "sink and source servers per node...";
    _source_servers_pernode.clear();
    _sink_servers_pernode.clear();
    find_source_sink_servers();

    /*
    for (map<string, set<string> >::iterator 
         iter = _source_servers_pernode.begin(); 
         iter != _source_servers_pernode.end(); iter++)
    {
        DEBUG << "Source servers at Node = " << iter->first;
        for (set<string>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            DEBUG << *iter2;
        }
    }
    for (map<string, set<string> >::iterator 
         iter = _sink_servers_pernode.begin(); 
         iter != _sink_servers_pernode.end(); iter++)
    {
        DEBUG << "Sink servers at Node = " << iter->first;
        for (set<string>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            DEBUG << *iter2;
        }
    }
    */

    DEBUG << "drop locations per node...";
    _drop_locations_pernode.clear();
    find_drop_locations_pernode();

    /*
    for (map<string, DropLocationPlusMap>::iterator 
         iter = _drop_locations_pernode.begin(); 
         iter != _drop_locations_pernode.end(); iter++)
    {
        DEBUG << "Drop Locations at Node = " << iter->first;
        for (DropLocationPlusMap::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            DEBUG << iter2->first;
            DEBUG << (iter2->second).as_string();
        }
    }
    */

    DEBUG << "load coefs per node...";
    compute_load_coefs_pernode();

    /*
    for (map<string, DropLocationPlusMap>::iterator 
         iter = _drop_locations_pernode.begin(); 
         iter != _drop_locations_pernode.end(); iter++)
    {
        DEBUG << "Drop Locations at Node = " << iter->first;
        for (DropLocationPlusMap::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            DEBUG << iter2->first;
            DEBUG << (iter2->second).as_string();
        }
    }
    */

    DEBUG << "input info per node...";
    compute_input_info_pernode();

    /*
    DEBUG << "dl's per node...";
    collect_dl_for_inputs_pernode();
    */

    set<string>& node_addresses = *(_catalog.get_region_node());
    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (is_leaf_node(*iter))
        {
            /*
            DEBUG << "load coefs for node " << *iter << "...";
            compute_load_coefs_pernode(*iter);
            DEBUG << "input info for node " << *iter << "...";
            compute_input_info_pernode(*iter);
            */
            DEBUG << "dl's for node " << *iter << "...";
            collect_dl_for_inputs_pernode(*iter);
            DEBUG << "dl sort order for node " << *iter << "...";
            determine_dl_sort_order_pernode(*iter);
            DEBUG << "input spread for node " << *iter << "...";
            compute_input_spread_pernode(*iter);
        }
    }

    /*
    for (map<string, InputInfoMap>::iterator
         iter = _input_info_pernode.begin();
         iter != _input_info_pernode.end(); iter++)
    {
        NOTICE << "Input info at Node = " << iter->first;
        for (InputInfoMap::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            NOTICE << iter2->first;
            NOTICE << (iter2->second).as_string();
        }
    }
    */

    DEBUG << "node input load and rate coefs per node...";
    _node_input_load_coefs.clear(); 
    _node_input_rate_coefs.clear();
    generate_node_input_info();

    /*
    NOTICE << "Node input information:";
    NOTICE << "Load coefs:";
    for (map<string, map<string, double> >::iterator
         iter = _node_input_load_coefs.begin();
         iter != _node_input_load_coefs.end(); iter++)
    {
        NOTICE << "Node " << iter->first;
        for (map<string, double>::iterator iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            NOTICE << iter2->first << " -> " << iter2->second;
        }
    }
    NOTICE << "Estimated rate coefs:";
    for (map<string, map<string, map<string, double> > >::iterator
         iter = _node_input_rate_coefs.begin();
         iter != _node_input_rate_coefs.end(); iter++) 
    {
        NOTICE << "Node " << iter->first;
        for (map<string, map<string, double> >::iterator 
             iter2 = (iter->second).begin();
             iter2 != (iter->second).end(); iter2++)
        {
            NOTICE << "Node input " << iter2->first;
            for (map<string, double>::iterator
                 iter3 = (iter2->second).begin();
                 iter3 != (iter2->second).end(); iter3++)
            {
                NOTICE << iter3->first << " -> " << iter3->second;
            }
        }
    }
    */
}

double LoadShedder::compute_FIT_score(string node, 
                                      FeasibleInputTable::RateMap rates)
{
    double total_score = 0;

    for (FeasibleInputTable::RateMapIter iter = rates.begin();
         iter != rates.end(); iter++)
    {
        if (_input_info_pernode[node].find(iter->first)
            == _input_info_pernode[node].end())
        {
            WARN << "Input information for stream \"" << iter->first
                 << "\" does not exist.";
        }
        else
        {
            total_score += iter->second *
                    (_input_info_pernode[node][iter->first].total_quality);
        }
    }

    return total_score;
}

// Is the given rate combination at node feasible given the additional plan?
// This function is to be used at non-leaf nodes after children are merged.
//
bool LoadShedder::is_feasible_pernode2(string node,
                                       FeasibleInputTable::RateMap rates, 
                                       ptr<LSPlan> plan)
{
    if ((plan->_drop_insertion_plan).empty())
    {
        double excess;
        return (is_feasible_pernode(node, rates, &excess));
    }
    
    double total_load = 0;

    for (FeasibleInputTable::RateMapIter iter = rates.begin();
         iter != rates.end(); iter++)
    {
        CatalogStream *stream = _catalog.get_stream(Name(iter->first));

        double stream_load = 0;

        vector<BoxPort> *sinks = stream->get_sink_port();
        for (unsigned int i = 0; i < sinks->size(); i++)
        {
            Name next_box_name = ((*sinks)[i].get_port_box())->get_box_name();

            double load = 0; string n; int loc_id;

            compute_load_with_drops(node, plan, next_box_name, &load); 

            if ((loc_id = get_drop_location_id_pernode(Name(iter->first), 
                                                       next_box_name,
                                                       (*sinks)[i].get_port(),
                                                       n)))
            {
                string drop_box_name = (string("drop_box_") + loc_id);
                vector<LSPlan::drop_box>::iterator drop =
                                    plan->locate(Name(drop_box_name));
                if (drop != (plan->_drop_insertion_plan).end())
                {
                    double drop_rate = atof(((drop->box_params).get_parameter(
                                                        "drop_rate")).c_str());
                    load *= (1 - drop_rate);
                }
            }

            stream_load += load;
        }

        total_load += stream_load * (iter->second);
    }

    if (total_load > CAPACITY)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void LoadShedder::compute_load_with_drops(string node, ptr<LSPlan> plan, 
                                          Name box_name, double *load)
{
    CatalogBox *box = _catalog.get_box(box_name);

    if (box->get_box_node() != node)
    {
        return;
    }

    try
    {
        const FixLenTimeSeries &f = _statistics[node].get_statistics(
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
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
        return;
    }
    catch(TSStats::error e)
    {
        WARN << "TSStats::Error " << (int)e;
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
        return;
    }
    catch(...)
    {
        WARN << "Unknown stats exception";
        WARN << "Could not read \"Cost\" for box " << box_name.as_string();
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
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
                                            "Selectivity", selectivity_id);
            try
            {
                selectivity = f.get_average();
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
            Name next_box_name = ((*sinks)[j].get_port_box())->get_box_name();
            double load_down = 0; string n; int loc_id;
            compute_load_with_drops(node, plan, 
                                    next_box_name,
                                    &load_down);
            if ((loc_id = get_drop_location_id_pernode(
                                            out_stream->get_stream_name(), 
                                            next_box_name,
                                            (*sinks)[j].get_port(),
                                            n)))
            {
                string drop_box_name = (string("drop_box_") + loc_id);
                vector<LSPlan::drop_box>::iterator drop =
                                plan->locate(Name(drop_box_name));
                if (drop != (plan->_drop_insertion_plan).end())
                {
                    double drop_rate = atof(((drop->box_params).get_parameter(
                                                    "drop_rate")).c_str());
                    load_down *= (1 - drop_rate);
                }
            }

            (*load) += selectivity * load_down;
        }
    }
}

bool LoadShedder::is_feasible_pernode(string node, 
                                      FeasibleInputTable::RateMap rates,
                                      double *excess_load)
{
    double total_load = 0;

    for (FeasibleInputTable::RateMapIter iter = rates.begin();
         iter != rates.end(); iter++)
    {
        if (_input_info_pernode[node].find(iter->first) 
            == _input_info_pernode[node].end())
        {
            WARN << "Input information for stream \"" << iter->first
                 << "\" does not exist.";
        }
        else
        {
            total_load += iter->second * 
                  (_input_info_pernode[node][iter->first].total_load_coef);
        }
    }

    if (total_load > CAPACITY)
    {
        (*excess_load) = total_load - CAPACITY;
        return false;
    }
    else
    {
        (*excess_load) = total_load - CAPACITY; // can be negative..
        return true;
    }
}

bool LoadShedder::is_feasible_with_plan_pernode(string node,
                                              FeasibleInputTable::RateMap rates,
                                              double *excess_load)
{
    double total_load = 0;

    for (FeasibleInputTable::RateMapIter iter = rates.begin();
         iter != rates.end(); iter++)
    {
        if (_input_info_pernode[node].find(iter->first) 
            == _input_info_pernode[node].end())
        {
            WARN << "Input information for stream \"" << iter->first
                 << "\" does not exist.";
        }
        else
        {
            total_load += iter->second * 
                  (_input_info_pernode[node][iter->first].reduced_load_coef);
        }
    }

    if (total_load > CAPACITY)
    {
        (*excess_load) = total_load - CAPACITY;
        return false;
    }
    else
    {
        (*excess_load) = 0; // can be negative..
        return true;
    }
}

// Assumptions:
// 1. Each box has one output stream (which can split), 
// i.e., no case-like filter in the query network.
// 2. An output stream never splits into two different downstream servers.
//
double LoadShedder::compute_merged_rate_down_pernode(string node, 
                                            Name box_name,
                                            FeasibleInputTable::RateMap rates,
                                            ptr<LSPlan> plan)
{
    CatalogBox *box = _catalog.get_box(box_name);
    assert(box->get_box_node() == node);

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
            const FixLenTimeSeries &f = _statistics[node].get_statistics(
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

            if (next_box->get_box_node() != node) // node sink
            {
                DEBUG << "returning -1";
                return -1;
            }
            else 
            {
                r = compute_merged_rate_down_pernode(node, 
                                                     next_box->get_box_name(), 
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

                string n;
                int k = get_drop_location_id_pernode(
                                     out_stream->get_stream_name(), 
                                     (*sinks)[j].get_port_box()->get_box_name(),
                                     (*sinks)[j].get_port(),
                                     n);
                assert(n == node);

                double drop_rate;
                if (max > 0)
                {
                    drop_rate = 1.0 - temp_rates[j]/max;
                }
                else
                {
                    drop_rate = 0;
                }

                if (drop_rate <= 0.01) // it's not worth it
                {
                    continue;
                }

                LSPlan::drop_box d;
                d.box_name = (string("drop_box_") + k);
                d.box_type = _drop_locations_pernode[n][k].box_type;
                d.node = n;
                BoxParameter::ParameterMap pm;
                pm = *(_drop_locations_pernode[n][k].box_params.
                     get_parameter_map_const());
                ostringstream s;
                s << drop_rate;
                pm["drop_rate"] = s.str();
                d.box_params.set_parameter_map(&pm);
                d.in_stream_name = _drop_locations_pernode[n][k].in_stream_name;
                d.dest_box_name = _drop_locations_pernode[n][k].dest_box_name;
                d.dest_box_port = _drop_locations_pernode[n][k].dest_box_port;
                d.dest_node = n;
                (plan->_drop_insertion_plan).push_back(d);
            }

            DEBUG << "returning " << (max/selectivity);
            return (max/selectivity);
        }
    }

    DEBUG << "returning -1";
    return -1;
}

double LoadShedder::compute_merged_rate_pernode(string node, 
                                        string stream_name, 
                                        FeasibleInputTable::RateMap rates, 
                                        ptr<LSPlan> plan)
{
    // if there is no link, then we will return -1.
    double r, max = -1;
    vector<double> temp_rates;
    unsigned int index;

    CatalogStream *stream = _catalog.get_stream(Name(stream_name));
    
    vector<BoxPort> *sinks = stream->get_sink_port();
    for (unsigned int i = 0; i < sinks->size(); i++)
    {
        r = compute_merged_rate_down_pernode(node,
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

            string n;
            int k = get_drop_location_id_pernode(
                                     Name(stream_name), 
                                     (*sinks)[i].get_port_box()->get_box_name(),
                                     (*sinks)[i].get_port(), n);
            assert(n == node);

            double drop_rate;
            if (max > 0)
            {
                drop_rate = 1.0 - temp_rates[i]/max;
            }
            else
            {
                drop_rate = 0;
            }

            if (drop_rate <= 0.01) // it's not worth it
            {
                continue;
            }

            LSPlan::drop_box d;
            d.box_name = (string("drop_box_") + k);
            d.box_type = _drop_locations_pernode[n][k].box_type;
            d.node = n;
            BoxParameter::ParameterMap pm;
            pm = *(_drop_locations_pernode[n][k].box_params.
                 get_parameter_map_const());
            ostringstream s;
            s << drop_rate;
            pm["drop_rate"] = s.str();
            d.box_params.set_parameter_map(&pm);
            d.in_stream_name = _drop_locations_pernode[n][k].in_stream_name;
            d.dest_box_name = _drop_locations_pernode[n][k].dest_box_name;
            d.dest_box_port = _drop_locations_pernode[n][k].dest_box_port;
            d.dest_node = n;
            (plan->_drop_insertion_plan).push_back(d);
        }
    }

    return max;
}

FeasibleInputTable::FIT_Entry LoadShedder::merge_entry(
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

    // also add the remote plans.
    //
    for (map<string, ptr<LSPlan> >::const_iterator 
         iter = e1._remote_plans.begin(); 
         iter != e1._remote_plans.end(); iter++)
    {
        result._remote_plans[iter->first] = iter->second;
    }
    for (map<string, ptr<LSPlan> >::const_iterator 
         iter = e2._remote_plans.begin(); 
         iter != e2._remote_plans.end(); iter++)
    {
        result._remote_plans[iter->first] = iter->second;
    }

    DEBUG << "resulting FIT entry:";
    DEBUG << result.as_string();

    return result;
}

// merge the given fit into _node_fit[node].
//
void LoadShedder::merge_child_pernode(string node, FeasibleInputTable fit)
{
    vector<FeasibleInputTable::FIT_Entry> child_fit = fit._table;

    vector<FeasibleInputTable::FIT_Entry> current_fit = _node_fit[node]._table;

    if (_node_fit[node]._table.empty())
    {
        DEBUG << "empty, just copy from child";
        _node_fit[node]._table = child_fit;
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

    _node_fit[node]._table = merged_fit;
}

void LoadShedder::record_plan_point(int id, map<string, double> rates,
                                    double score)
{
    if (_fit_data)
    {
        _fit_data << id;
        _fit_data << " ";
        for (map<string, double>::iterator iter = rates.begin();
             iter != rates.end(); iter++)
        {
            _fit_data << iter->second;
            _fit_data << " ";
        }
        _fit_data << score;
        _fit_data << "\n";
    }
}

void LoadShedder::record_fit_point(int id, FeasibleInputTable::RateMap rates, 
                                   int state, double score)
{
    if (_fit_data)
    {
        _fit_data << id;
        _fit_data << " ";
        for (FeasibleInputTable::RateMapIter iter = rates.begin();
             iter != rates.end(); iter++)
        {
            _fit_data << iter->second;
            _fit_data << " ";
        }
        _fit_data << state;
        _fit_data << " ";
        _fit_data << score;
        _fit_data << "\n";
    }
}

ptr<LSPlan> LoadShedder::generate_FIT_plan(string node, 
                                           FeasibleInputTable::RateMap rates,
                                           double score)
{
    ptr<LSPlan> plan;
    double excess_load;

    if (is_feasible_pernode(node, rates, &excess_load))
    {
        plan.set(new LSPlan());
    }
    else
    {
        assert(is_feasible_with_plan_pernode(node, rates, &excess_load));

        vector<LSPlan::drop_box>::iterator drop_iter;
        vector<LSPlan::drop_box> drops;
        drops.clear();

        for (unsigned int j = 0; j < _dl_sort_order[node].size(); j++)
        {
            int k = _dl_sort_order[node][j];
            double rate = rates[_drop_locations_pernode[node][k].input_stream];
            double load_saved = rate * 
                    _drop_locations_pernode[node][k].rate_coef * 
                    _drop_locations_pernode[node][k].reduced_load_coef; 

            DEBUG << "load that can be saved by " << k << " = " << load_saved;

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
                d.box_type = _drop_locations_pernode[node][k].box_type;
                d.node = node;
                BoxParameter::ParameterMap pm;
                pm = *(_drop_locations_pernode[node][k].box_params.
                     get_parameter_map_const());
                ostringstream s;
                s << drop_rate;
                pm["drop_rate"] = s.str();
                d.box_params.set_parameter_map(&pm);
                d.in_stream_name = _drop_locations_pernode[node][k].
                                   in_stream_name;
                d.dest_box_name = _drop_locations_pernode[node][k].
                                  dest_box_name;
                d.dest_box_port = _drop_locations_pernode[node][k].
                                  dest_box_port;
                d.dest_node = node;
                drops.push_back(d);
                DEBUG << "Drop:" << d.as_string();

                score -= drop_rate * rate * 
                        _drop_locations_pernode[node][k].rate_coef *
                        _drop_locations_pernode[node][k].reduced_quality_score;

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
                d.box_type = _drop_locations_pernode[node][k].box_type;
                d.node = node;
                BoxParameter::ParameterMap pm;
                pm = *(_drop_locations_pernode[node][k].box_params.
                     get_parameter_map_const());
                ostringstream s;
                s << drop_rate;
                pm["drop_rate"] = s.str();
                d.box_params.set_parameter_map(&pm);
                d.in_stream_name = _drop_locations_pernode[node][k].
                                   in_stream_name;
                d.dest_box_name = _drop_locations_pernode[node][k].
                                  dest_box_name;
                d.dest_box_port = _drop_locations_pernode[node][k].
                                  dest_box_port;
                d.dest_node = node;
                drops.push_back(d);
                DEBUG << "Drop:" << d.as_string();

                score -= drop_rate * rate * 
                        _drop_locations_pernode[node][k].rate_coef *
                        _drop_locations_pernode[node][k].reduced_quality_score;

                excess_load -= load_saved;
            }
        }

        plan.set(new LSPlan(drops));
    }

    return plan;
}

// This function generates the boundary points for a given leaf node (i.e.,
// there is just 1 load constraint.
//
void LoadShedder::generate_FIT_points(string node, 
                                      InputInfoMap::iterator iter, 
                                      double capacity_budget, 
                                      FeasibleInputTable::RateMap &rates)
{
    InputInfoMap::iterator current = iter;
    InputInfoMap::iterator next = ++iter;
    double rate, max_rate, score, temp_budget;
    unsigned int num_points;

    if (capacity_budget <= 0.0001)
    {
        // rest of the input rates stays as 0, nothing further to do.
        //
        for (InputInfoMap::iterator it = current; 
             it != _input_info_pernode[node].end(); it++)
        {
            rates[it->first] = 0;
        }

        // insert rates to the plan table.
        //
        score = compute_FIT_score(node, rates);
        ptr<LSPlan> plan = generate_FIT_plan(node, rates, score); 
        FeasibleInputTable::FIT_Entry fit_entry(rates, score, plan);
        (_node_fit[node]._table).push_back(fit_entry);

        return;
    }

    if (next == _input_info_pernode[node].end())
    {
        // assign rest of the capacity_budget to current.
        //
        if ((current->second).reduced_load_coef > 0)
        {
            rate = capacity_budget/((current->second).reduced_load_coef);
        }
        else
        {
            WARN << "Zero reduced_load_coef !"; // not supposed to happen
            rate = 0; // ??
        }

        rates[current->first] = rate;
        
        // insert rates to the plan table.
        //
        double score = compute_FIT_score(node, rates);
        ptr<LSPlan> plan = generate_FIT_plan(node, rates, score);
        FeasibleInputTable::FIT_Entry fit_entry(rates, score, plan);
        (_node_fit[node]._table).push_back(fit_entry);

        return;
    }
    else
    {
        if ((current->second).reduced_load_coef > 0)
        {
            max_rate = capacity_budget/((current->second).reduced_load_coef);
        }
        else
        {
            WARN << "Zero reduced_load_coef !"; // not supposed to happen
            max_rate = 0; // ??
        }

        assert((current->second).spread > 0);

        num_points = int(ceil(max_rate/(current->second).spread)) + 1;

        for (unsigned int i = 0; i < num_points; i++)
        {
            temp_budget = capacity_budget;

            rate = max_rate - i*((current->second).spread);
            if (rate < 0)
            {
                break;
            }

            rates[current->first] = rate;

            // update the capacity_budget.
            //
            temp_budget -= rate * ((current->second).reduced_load_coef);
            assert(temp_budget >= 0);

            generate_FIT_points(node, next, temp_budget, rates);
        }
    }
}

// FIT generation at a leaf node.
//
void LoadShedder::generate_node_FIT(string node, double spread)
{
    NOTICE << node;
    //NOTICE << "Spread = " << spread;

    unsigned int fit_size = 1;

    for (InputInfoMap::iterator iter = _input_info_pernode[node].begin();
         iter != _input_info_pernode[node].end(); iter++)
    {
        DEBUG << "Input stream: " << iter->first;

        assert((iter->second).spread > 0);

        NOTICE << "Spread along " << iter->first 
               << " = " << (iter->second).spread;
        NOTICE << "Max = " << (iter->second).rate_bound;
        (iter->second).num_points = (int(ceil((iter->second).rate_bound/
                                              (iter->second).spread)) + 1);
        /*
        (iter->second).num_points = (int(ceil((iter->second).rate_bound/
                                              spread)) + 1);
        */

        DEBUG << "Number of points: " << (iter->second).num_points;

        fit_size *= (iter->second).num_points;
    }
        
    NOTICE << "FIT size = " << fit_size;

    _node_fit[node] = FeasibleInputTable(fit_size);

    unsigned int repeat_count = fit_size;
    unsigned int times;

    for (InputInfoMap::iterator iter = _input_info_pernode[node].begin();
         iter != _input_info_pernode[node].end(); iter++)
    {
        repeat_count = repeat_count/((iter->second).num_points);    

        times = fit_size/repeat_count;

        unsigned int k = 0;

        double rate = (iter->second).rate_bound;

        for (unsigned int i = 0; i < times; i++) 
        {
            for (unsigned int j = 0; j < repeat_count; j++)
            {
                //DEBUG << k << ": " << iter->first << " = " << rate;
                (_node_fit[node]._table[k]._rates)[iter->first] = rate; 
                ++k;
            }

            if (rate >= (iter->second).spread)
            {
                rate -= (iter->second).spread;
            }
            else if (abs(rate - 0) <= 0.00001) // practically zero
            {
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
            /*
            if (rate >= spread)
            {
                rate -= spread;
            }
            else if (abs(rate - 0) <= 0.00001) // practically zero
            {
                rate = (iter->second).rate_bound;
            }
            else if ((rate > 0) && (rate < spread))
            {
                rate = 0;
            }
            else 
            {
                assert(false); // complain (never should get here)
            }
            */
        }

        assert(k == fit_size);
    }

    // Next, eliminate the infeasible ones 
    // and enter scores for the feasible ones.
    // Also, compute additional local plan for an entry if 
    // feasible with a plan, but not feasible without a plan.
    //
    vector<FeasibleInputTable::FIT_Entry>::iterator fit_iter = 
                                       _node_fit[node]._table.begin();

    /*
    while (fit_iter != _node_fit[node]._table.end())
    {
        string s;
        for (FeasibleInputTable::RateMapIter it = (fit_iter->_rates).begin();
             it != (fit_iter->_rates).end(); it++)
        {
            s += it->second;
            s += " ";
        }
        DEBUG << s;
        ++fit_iter;
    }

    fit_iter = _node_fit[node]._table.begin();
    */

    int fit_count = 1;

    //_fit_data.open("/tmp/FIT", ios::out);

    while (fit_iter != _node_fit[node]._table.end())
    {
        DEBUG << fit_iter->as_string();

        double excess_load_w, excess_load;
    
        if (is_feasible_with_plan_pernode(node, fit_iter->_rates, 
                                          &excess_load_w))
        {
            DEBUG << "is feasible with plan";
            if (is_feasible_pernode(node, fit_iter->_rates, &excess_load))
            {
                //NOTICE << fit_count << ": feasible w/o plan";
                fit_iter->_score = compute_FIT_score(node, fit_iter->_rates);

                /*
                record_fit_point(fit_count, fit_iter->_rates, 0, 
                                 fit_iter->_score);
                */

                fit_iter->_additional_plan.set(new LSPlan());

                DEBUG << fit_iter->as_string();
            }
            else // additional local plan is needed.
            {
                DEBUG << "Additional local plan is needed";
                DEBUG << "actual excess load = " << excess_load;

                //NOTICE << fit_count << ": feasible w/ plan";

                fit_iter->_score = compute_FIT_score(node, fit_iter->_rates);

                /*
                record_fit_point(fit_count, fit_iter->_rates, 1, 
                                 fit_iter->_score);
                */

                vector<LSPlan::drop_box>::iterator drop_iter;
                vector<LSPlan::drop_box> drops;
                drops.clear();

                for (unsigned int j = 0; j < _dl_sort_order[node].size(); j++)
                {
                    int k = _dl_sort_order[node][j];
                    double rate = fit_iter->_rates[
                                 _drop_locations_pernode[node][k].input_stream];
                    double load_saved = rate * 
                           _drop_locations_pernode[node][k].rate_coef * 
                           _drop_locations_pernode[node][k].reduced_load_coef; 

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
                        d.box_type = _drop_locations_pernode[node][k].box_type;
                        d.node = node;
                        BoxParameter::ParameterMap pm;
                        pm = *(_drop_locations_pernode[node][k].box_params.
                             get_parameter_map_const());
                        ostringstream s;
                        s << drop_rate;
                        pm["drop_rate"] = s.str();
                        d.box_params.set_parameter_map(&pm);
                        d.in_stream_name = _drop_locations_pernode[node][k].
                                           in_stream_name;
                        d.dest_box_name = _drop_locations_pernode[node][k].
                                          dest_box_name;
                        d.dest_box_port = _drop_locations_pernode[node][k].
                                          dest_box_port;
                        d.dest_node = node;
                        drops.push_back(d);
                        DEBUG << "Drop:" << d.as_string();

                        fit_iter->_score -= drop_rate * rate * 
                         _drop_locations_pernode[node][k].rate_coef *
                         _drop_locations_pernode[node][k].reduced_quality_score;

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
                        d.box_type = _drop_locations_pernode[node][k].box_type;
                        d.node = node;
                        BoxParameter::ParameterMap pm;
                        pm = *(_drop_locations_pernode[node][k].box_params.
                             get_parameter_map_const());
                        ostringstream s;
                        s << drop_rate;
                        pm["drop_rate"] = s.str();
                        d.box_params.set_parameter_map(&pm);
                        d.in_stream_name = _drop_locations_pernode[node][k].
                                           in_stream_name;
                        d.dest_box_name = _drop_locations_pernode[node][k].
                                          dest_box_name;
                        d.dest_box_port = _drop_locations_pernode[node][k].
                                          dest_box_port;
                        d.dest_node = node;
                        drops.push_back(d);
                        DEBUG << "Drop:" << d.as_string();

                        fit_iter->_score -= drop_rate * rate * 
                         _drop_locations_pernode[node][k].rate_coef *
                         _drop_locations_pernode[node][k].reduced_quality_score;

                        excess_load -= load_saved;
                    }
                }

                fit_iter->_additional_plan.set(new LSPlan(drops));
            }

            ++fit_iter;
        }
        else
        {
            //NOTICE << fit_count << ": infeasible";
            //record_fit_point(fit_count, fit_iter->_rates, 2, 0);

            DEBUG << "is NOT feasible even with plan, erase..";
            fit_iter = _node_fit[node]._table.erase(fit_iter);
        }

        if (fit_iter == _node_fit[node]._table.end())
        {
            DEBUG << "No more entries to process..";
            break;
        }

        ++fit_count;
    }

    //_fit_data.close();
}

// FIT generation at an intermediate node.
//
void LoadShedder::generate_FIT_recursive(string node, double spread)
{
    NOTICE << node;

    if (is_leaf_node(node))
    {
        //generate_node_FIT(node, spread); // old way
        
        FeasibleInputTable::RateMap rates;
        rates.clear();
        generate_FIT_points(node, _input_info_pernode[node].begin(),
                            CAPACITY, rates);
    }
    else
    {
        for (set<string>::iterator iter = _sink_servers_pernode[node].begin();
             iter != _sink_servers_pernode[node].end(); iter++)
        {
            generate_FIT_recursive(*iter, spread);
        }

        // merge children
        //
        map<InetAddress, FeasibleInputTable> children;
        for (set<string>::iterator iter = _sink_servers_pernode[node].begin();
             iter != _sink_servers_pernode[node].end(); iter++)
        {
            DEBUG << "merging child " << *iter;

            vector<FeasibleInputTable::FIT_Entry> fit = _node_fit[*iter]._table;

            for (unsigned int i = 0; i < fit.size(); i++)
            {
                double score = fit[i]._score;
                ptr<LSPlan> plan;
                plan.set(new LSPlan()); // initialization

                FeasibleInputTable::RateMap rates;

                for (InputInfoMap::iterator 
                     iter2 = _input_info_pernode[node].begin();
                     iter2 != _input_info_pernode[node].end(); iter2++)
                {
                    double rate = compute_merged_rate_pernode(node,
                                                              iter2->first,
                                                              fit[i]._rates,
                                                              plan);
                    DEBUG << "merged rate = " << rate;
                    if (rate >= 0)
                    {
                        rates[iter2->first] = rate;
                    }
                }

                FeasibleInputTable::FIT_Entry entry(rates, score, plan);

                // also add the remote plans.
                //
                if (!(((fit[i]._additional_plan)->_drop_insertion_plan).
                                                            empty()))
                {
                    entry.add_remote_plan(*iter, fit[i]._additional_plan);
                }
                for (map<string, ptr<LSPlan> >::const_iterator 
                     iter2 = fit[i]._remote_plans.begin();
                     iter2 != fit[i]._remote_plans.end();
                     iter2++)
                {
                    entry.add_remote_plan(iter2->first, iter2->second);
                }

                DEBUG << "New entry: " << entry.as_string();

                children[*iter]._table.push_back(entry);
            }
        }

        NOTICE << "cross product children";

        _node_fit[node].clear();

        map<InetAddress, FeasibleInputTable>::iterator iter;
        for (iter = children.begin(); iter != children.end(); iter++)
        {
            merge_child_pernode(node, iter->second);
        }

        NOTICE << "eliminate infeasible ones";

        //double excess;
        vector<FeasibleInputTable::FIT_Entry>::iterator fit_iter = 
                                            _node_fit[node]._table.begin();
        while (fit_iter != _node_fit[node]._table.end())
        {
            if (is_feasible_pernode2(node, fit_iter->_rates, 
                                     fit_iter->_additional_plan))
            {
                ++fit_iter;
            }
            else
            {
                NOTICE << "erasing plan";
                fit_iter = _node_fit[node]._table.erase(fit_iter);
            }
        }
    }
}

// FIT generation at the roots (not time-driven, but spread-driven).
//
void LoadShedder::generate_FIT()
{
    double spread = FIXED_FIT_SPREAD;

    set<string>& node_addresses = *(_catalog.get_region_node());
    for (set<string>::const_iterator iter1 = node_addresses.begin();
         iter1 != node_addresses.end(); ++iter1)
    {
        if (is_root_node(*iter1))
        {
            for (set<string>::iterator 
                 iter2 = _sink_servers_pernode[*iter1].begin(); 
                 iter2 != _sink_servers_pernode[*iter1].end(); iter2++)
            {
                generate_FIT_recursive(*iter2, spread);
            }

            for (InputInfoMap::iterator 
                 iter3 = _input_info_pernode[*iter1].begin();
                 iter3 != _input_info_pernode[*iter1].end(); iter3++)
            {
                _max_rates[iter3->first] = 0;
            }

            // merge children
            //
            map<InetAddress, FeasibleInputTable> children;
            for (set<string>::iterator 
                 iter2 = _sink_servers_pernode[*iter1].begin();
                 iter2 != _sink_servers_pernode[*iter1].end(); iter2++)
            {
                DEBUG << "merging child " << *iter2;

                vector<FeasibleInputTable::FIT_Entry> fit = 
                                        _node_fit[*iter2]._table;

                for (unsigned int i = 0; i < fit.size(); i++)
                {
                    DEBUG << fit[i].as_string();

                    double score = fit[i]._score;
                    ptr<LSPlan> plan;
                    plan.set(new LSPlan()); // initialization

                    FeasibleInputTable::RateMap rates;

                    for (InputInfoMap::iterator 
                         iter3 = _input_info_pernode[*iter1].begin();
                         iter3 != _input_info_pernode[*iter1].end(); iter3++)
                    {
                        double rate = compute_merged_rate_pernode(*iter1,
                                                              iter3->first,
                                                              fit[i]._rates,
                                                              plan);
                        DEBUG << "merged rate = " << rate;
                        if (rate >= 0)
                        {
                            rates[iter3->first] = rate;
                        }
                    }

                    FeasibleInputTable::FIT_Entry entry(rates, score, plan);

                    // also add the remote plans.
                    //
                    if (!(((fit[i]._additional_plan)->_drop_insertion_plan).
                                                                empty()))
                    {
                        entry.add_remote_plan(*iter2, fit[i]._additional_plan);
                    }
                    for (map<string, ptr<LSPlan> >::const_iterator 
                         iter3 = fit[i]._remote_plans.begin();
                         iter3 != fit[i]._remote_plans.end();
                         iter3++)
                    {
                        entry.add_remote_plan(iter3->first, iter3->second);
                    }

                    DEBUG << "New entry: " << entry.as_string();

                    children[*iter2]._table.push_back(entry);
                }
            }

            DEBUG << "cross product children";

            _node_fit[*iter1].clear();

            map<InetAddress, FeasibleInputTable>::iterator iter2;
            for (iter2 = children.begin(); iter2 != children.end(); iter2++)
            {
                merge_child_pernode(*iter1, iter2->second);
            }

            DEBUG << "eliminate infeasible ones";

            //double excess;
            vector<FeasibleInputTable::FIT_Entry>::iterator fit_iter = 
                                            _node_fit[*iter1]._table.begin();
            while (fit_iter != _node_fit[*iter1]._table.end())
            {
                 DEBUG << fit_iter->as_string();
                if (is_feasible_pernode2(*iter1, fit_iter->_rates,
                                         fit_iter->_additional_plan))
                {
                    DEBUG << "ok";
                    for (FeasibleInputTable::RateMapIter 
                         it = (fit_iter->_rates).begin(); 
                         it != (fit_iter->_rates).end(); it++)
                    {
                        if (_max_rates[it->first] < it->second)
                        {
                            _max_rates[it->first] = it->second;
                        }
                    }

                    ++fit_iter;
                }
                else
                {
                    DEBUG << "erased";
                    fit_iter = _node_fit[*iter1]._table.erase(fit_iter);
                }
            }

            /*
            for (InputInfoMap::iterator 
                 iter3 = _input_info_pernode[*iter1].begin();
                 iter3 != _input_info_pernode[*iter1].end(); iter3++)
            {
                NOTICE << "max rate for " << iter3->first << " = " 
                       << _max_rates[iter3->first];
            }
            */

            DEBUG << "FIT at node = " << *iter1;
            DEBUG <<_node_fit[*iter1].as_string();

            // generate the actual FIT plans
            //
            generate_FIT_plans(*iter1);

            /*
            NOTICE << "For node = " << *iter1;
            NOTICE << "Number of plans = " << _node_plans[*iter1]._plans.size();
            */
            /*
            NOTICE << _node_plans[*iter1].as_string();
            */
        }
    }
}

// To be called for root nodes only.
// If multiple roots, we assume trees don't overlap.
// So, each tree has its own plan id space starting from id = 0.
//
void LoadShedder::generate_FIT_plans(string node)
{
    // default plan first
    //
    add_default_plan();

    int id = 1;

    vector<FeasibleInputTable::FIT_Entry> fit = _node_fit[node]._table;

    for (unsigned int i = 0; i < fit.size(); i++)
    {
        // own plan
        //
        _node_plans[node]._plans[id].set(new LSPlan());
        (_node_plans[node]._plans[id])->set_plan(
                            (fit[i]._additional_plan)->_drop_insertion_plan);
        DEBUG << "Plan " << id << " at node " << node << ":";
        DEBUG << (_node_plans[node]._plans[id])->as_string();

        // remote plans
        //
        map<string, ptr<LSPlan> > remote_plans = fit[i]._remote_plans;
        map<string, ptr<LSPlan> >::const_iterator iter;
        for (iter = remote_plans.begin(); iter != remote_plans.end(); iter++)
        {
            _node_plans[iter->first]._plans[id].set(new LSPlan());
            (_node_plans[iter->first]._plans[id])->set_plan(
                                iter->second->_drop_insertion_plan);
            DEBUG << "Plan " << id << " at node " << iter->first << ":";
            DEBUG << (_node_plans[iter->first]._plans[id])->as_string();
        }

        // rest of the nodes should get empty plans.
        //
        set<string>& node_addresses = *(_catalog.get_region_node());
        for (set<string>::const_iterator iter1 = node_addresses.begin();
             iter1 != node_addresses.end(); ++iter1)
        {
            if ((_node_plans[*iter1]._plans).find(id) == 
                (_node_plans[*iter1]._plans).end())
            {
                _node_plans[*iter1]._plans[id].set(new LSPlan());
                DEBUG << "Plan " << id << " at node " << *iter1 << ":";
                DEBUG << (_node_plans[*iter1]._plans[id])->as_string();
            }
        }

        // **** The bottom two can be problematic for multiple roots !!!
        //
        _precomputed_plan_ids[id] = fit[i]._rates;
        _plan_scores[id] = fit[i]._score;

        ++id;
    }
}

void LoadShedder::precompute_FIT_plans()
{
    generate_FIT(); // generate fits + node_plans

    /*
    Time start_time = Time::now();
    upload_plans(); // upload all
    Time stop_time = Time::now();
    double time_passed = static_cast<double>(
                         (stop_time - start_time).to_msecs());
    NOTICE << "*** Upload took = " << time_passed << " milliseconds.";
    */
}

void LoadShedder::print_plans()
{
    _fit_data.open("/tmp/plan", ios::out);

    set<string>& node_addresses = *(_catalog.get_region_node());
    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        if (!is_root_node(*iter))
        {
            continue;
        }

        NOTICE << "For node = " << *iter;
        NOTICE << "Number of plans = " << _node_plans[*iter]._plans.size();
        for (map<int, ptr<LSPlan> >::iterator 
             it = (_node_plans[*iter]._plans).begin();
             it != (_node_plans[*iter]._plans).end(); it++)
        {
             record_plan_point(it->first, _precomputed_plan_ids[it->first],
                               _plan_scores[it->first]);
        }
        /*
        for (map<int, ptr<LSPlan> >::iterator 
             it = (_node_plans[*iter]._plans).begin(); 
             it != (_node_plans[*iter]._plans).end(); it++)
        {
            NOTICE << "Plan " << it->first << ":";
            for (map<string, double>::iterator 
                 it2 = (_precomputed_plan_ids[it->first]).begin();
                 it2 != (_precomputed_plan_ids[it->first]).end();
                 it2++)
            {
                NOTICE << " " << it2->first << " = " << it2->second;
            }
            NOTICE << " score = " << _plan_scores[it->first];
            NOTICE << (it->second)->as_string();
        }
        */
    }

    _fit_data.close();
}

void LoadShedder::print_plan(int id)
{
    set<string>& node_addresses = *(_catalog.get_region_node());
    for (set<string>::const_iterator iter = node_addresses.begin();
         iter != node_addresses.end(); ++iter)
    {
        NOTICE << "For node = " << *iter;
        NOTICE << "Plan " << id << ":";
        for (map<string, double>::iterator 
             iter2 = (_precomputed_plan_ids[id]).begin();
             iter2 != (_precomputed_plan_ids[id]).end();
             iter2++)
        {
            NOTICE << " " << iter2->first << " = " << iter2->second;
        }
        NOTICE << " score = " << _plan_scores[id];
        NOTICE << (_node_plans[*iter]._plans[id])->as_string();
    }
}

/*
// TODO: additional rate cut
//
// Apply the additional local plan at the given FIT entry, and also 
// cut the current rates down to the rates at the given FIT entry.
// In this global version, we are also applying the remote plans.
//
void LoadShedder::apply_FIT_pernode(string node, 
                                    FeasibleInputTable::RateMap current_rates, 
                                    int fit_index)
{
    // If fit_index == -1, 
    // then simply remove all the drops from all the nodes.
    //
    if (fit_index == -1)
    {
        assert(_current_fit_index[node] != -1);

        NOTICE << "We will remove all the drops from all the nodes.";

        ptr<LSPlan> empty_plan;
        empty_plan.set(new LSPlan());

        map<string, ptr<LSPlan> > remote_plans =
            _node_fit[node]._table[_current_fit_index[node]]._remote_plans;
        map<string, ptr<LSPlan> >::const_iterator iter;
        for (iter = remote_plans.begin(); iter != remote_plans.end(); iter++)
        {
            // send empty_plan to iter->first
            //
            ptr<MedusaClient> client =
                            (ptr<MedusaClient>) new MedusaClient(iter->first);
            Remote<LocalLoadShedder> remote_local_ls =
                            client->local_ls_for((MedusaID)(iter->first));
            if (!remote_local_ls)
            {
                WARN << "No local load shedder for " << iter->first;
                continue;
            }
            NOTICE << "Sending plan to the remote local optimizer..";
            RPC<void> result = remote_local_ls.send_lsplan(empty_plan);
            NOTICE << "result : " << result;
            if (!result.stat())
            {
                WARN << "Load shedding at node " << iter->first
                     << " failed!";
                continue;
            }
        }

        // also, send empty_plan to node.
        //
        ptr<MedusaClient> client =
                (ptr<MedusaClient>) new MedusaClient(node);
        Remote<LocalLoadShedder> remote_local_ls =
                client->local_ls_for((MedusaID)node);
        if (!remote_local_ls)
        {
            WARN << "No local load shedder for " << node;
            return;
        }
        NOTICE << "Sending plan to the remote local optimizer..";
        RPC<void> result = remote_local_ls.send_lsplan(empty_plan);
        NOTICE << "result : " << result;
        if (!result.stat())
        {
            WARN << "Load shedding at node " << node << " failed!";
            return;
        }

        //
        // also, no additional plan due to rate cut.
        //
        //RPC<void> result = remote_local_ls.send_additional_plan(empty_plan);
        //NOTICE << "result : " << result;
        //if (!result.stat())
        //{
        //    WARN << "Additional plans for node " << node << " failed!";
        //    return;
        //}
        //

        return;
    }

    map<string, ptr<LSPlan> > remote_plans =
            _node_fit[node]._table[_current_fit_index[node]]._remote_plans;
    map<string, ptr<LSPlan> >::const_iterator iter;
    for (iter = remote_plans.begin(); iter != remote_plans.end(); iter++)
    {
           ptr<MedusaClient> client =
                          (ptr<MedusaClient>) new MedusaClient(iter->first);
           Remote<LocalLoadShedder> remote_local_ls =
                          client->local_ls_for((MedusaID)(iter->first));
           if (!remote_local_ls)
           {
               WARN << "No local load shedder for " << iter->first;
               continue;
           }
        NOTICE << "Sending plan to the remote local optimizer..";
        RPC<void> result = remote_local_ls.send_lsplan(iter->second);
        NOTICE << "result : " << result;
        if (!result.stat())
        {
            WARN << "Load shedding at node " << iter->first
                 << " failed!";
            continue;
        }
    }

    ptr<MedusaClient> client = (ptr<MedusaClient>) new MedusaClient(node);
    Remote<LocalLoadShedder> remote_local_ls = 
                             client->local_ls_for((MedusaID)node);
    if (!remote_local_ls)
    {
        WARN << "No local load shedder for " << node;
        return;
    }
    NOTICE << "Sending plan to the remote local optimizer..";
    RPC<void> result = remote_local_ls.send_lsplan(
            _node_fit[node]._table[_current_fit_index[node]]._additional_plan);
    NOTICE << "result : " << result;
    if (!result.stat())
    {
        WARN << "Load shedding at node " << node << " failed!";
        return;
    }

    //
    // also, no additional plan due to rate cut.
    //
    //RPC<void> result = remote_local_ls.send_additional_plan(empty_plan);
    //NOTICE << "result : " << result;
    //if (!result.stat())
    //{
    //    WARN << "Additional plans for node " << node << " failed!";
    //    return;
    //}
    //
}

int LoadShedder::find_best_FIT_pernode(string node, 
                                       FeasibleInputTable::RateMap rates)
{
    int    best_fit = -1;
    double max_score = 0;

    map<string, double> additional_reduction;
    additional_reduction.clear();

    for (unsigned int i = 0; i < _node_fit[node]._table.size(); i++)
    {
        FeasibleInputTable::RateMap fit_rates = 
                                    _node_fit[node]._table[i]._rates;

        bool found = true;
        additional_reduction.clear();

        for (FeasibleInputTable::RateMapIter iter = fit_rates.begin();
             iter != fit_rates.end(); iter++)
        {
            assert(rates.find(iter->first) != rates.end());

            if (iter->second > rates[iter->first]) // I can never adopt this.
            {
                found = false;
                break;
            }
            else
            {
                if (rates[iter->first] > 0)
                {
                     additional_reduction[iter->first] =
                             iter->second/rates[iter->first];
                }
            }
        }

        if (found) // found a point within the plan-feasible region
        {
            if (_node_fit[node]._table[i]._score > max_score) 
                                        // see if it is better
            {
                best_fit = i;
                max_score = _node_fit[node]._table[i]._score;
            }
        }
    }

    // create the additional plans from the additional reduction.
    //
    map<string, vector<LSPlan::drop_box> > drops; // per node
    drops.clear();
    // HERE.........

    return best_fit;
}

void LoadShedder::shed_load_FIT()
{
    set<string>& node_addresses = *(_catalog.get_region_node());
    for (set<string>::const_iterator iter1 = node_addresses.begin();
         iter1 != node_addresses.end(); ++iter1)
    {
        if (is_root_node(*iter1))
        {
            FeasibleInputTable::RateMap rates;
            InputInfoMap inputs = _input_info_pernode[*iter1];
            for (InputInfoMap::iterator i = inputs.begin(); i != inputs.end();
                 i++)
            {
                rates[i->first] = _input_rates[i->first];
            }

            double excess_load;
            if (is_feasible_pernode(*iter1, rates, &excess_load))
            {
                NOTICE << "FEASIBLE for " << *iter1;
                if (_current_fit_index[*iter1] != -1)
                {
                    apply_FIT_pernode(*iter1, rates, -1);
                    _current_fit_index[*iter1] = -1; 
                }
            }
            else
            {
                NOTICE << "NOT FEASIBLE for " << *iter1;
                NOTICE << "We will apply the best FIT.";

                int fit_index = find_best_FIT_pernode(*iter1, rates);

                if (fit_index >= 0)
                {
                    NOTICE << "Apply FIT " << fit_index;
                    NOTICE << _node_fit[*iter1]._table[fit_index].as_string();
                    apply_FIT_pernode(*iter1, rates, fit_index);
                    _current_fit_index[*iter1] = fit_index;
                }
                else
                {
                    NOTICE << "No best FIT.";
                }
            }
        }
    }
}
*/

/************************************************************************/

BOREALIS_NAMESPACE_END

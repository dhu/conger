
#include "LocalLoadShedder.h"

BOREALIS_NAMESPACE_BEGIN

LocalLoadShedder::LocalLoadShedder(string id)
                : BasicComponent(id, "LocalLoadShedder")
{
    _previous_plan_id = DEFAULT_PLAN_ID;
    _current_plan_id = DEFAULT_PLAN_ID;
    _ls_roadmap.clear();
    _additional_drops.clear();
}

void LocalLoadShedder::in_thread_init()
{
    /*
    (new CallbackTimer(_my_loop, wrap(this,&LocalLoadShedder::do_something)))
                                       ->arm(Time::now() + Time::msecs(1000));
    */
}

void LocalLoadShedder::do_something()
{
    /*
    DEBUG << "Doing something";
    (new CallbackTimer(_my_loop, wrap(this,&LocalLoadShedder::do_something)))
                                       ->arm(Time::now() + Time::msecs(1000));
    */
}

AsyncRPC<void> LocalLoadShedder::send_lsplans(LSPlanMessage plans)
{
    AsyncRPC<void> completion;

    NOTICE << "LocalLoadShedder received new plans...";
    DEBUG << plans.string_rep();

    NOTICE << "LocalLoadShedder will first reset current plan to default...";

    if (_current_plan_id != DEFAULT_PLAN_ID)
    {
        NOTICE << "Applying the default plan...";

        local_apply_lsplan(DEFAULT_PLAN_ID);

        _ls_roadmap = plans._plans;
    }
    else
    {
        _ls_roadmap = plans._plans;
        completion.post(true);
    }

    return completion;
}

RPC<void> LocalLoadShedder::send_plan_id(int plan_id)
{
    NOTICE << "LocalLoadShedder received plan_id " << plan_id;

    local_apply_lsplan(plan_id);

    return true;
}

RPC<void> LocalLoadShedder::send_additional_plan(LSPlan plan)
{
    NOTICE << "LocalLoadShedder received additional plan";

    local_apply_additional_lsplan(plan);

    return true;
}

/*
AsyncRPC<void> LocalLoadShedder::send_lsplan(LSPlan plan)
{
    unsigned int i;

    NOTICE << "LocalLoadShedder will apply plan: ";
    NOTICE << plan.string_rep();

    // First remove redundant drops if any.
    //
    DEBUG << "Checking for redundant drops...";

    vector<LSPlan::drop_box> current_dip = _current_plan._drop_insertion_plan;

    // due to normal plan
    //
    for (i = 0; i < current_dip.size(); i++)
    {
        if (!plan.find(current_dip[i].box_name))
        {
            DEBUG << "Disconnecting drop box: " << current_dip[i].box_name;

            string xml;
            xml = "<disconnect stream=\"";
            xml += current_dip[i].box_name;
            xml += "_out_stream"; 
            xml += "\" />";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                completion.post(RPCFault(result.stat()));
                return;
            }
        }
    }

    // due to additional plan
    //
    for (set<Name>::iterator iter = _additional_drops.begin(); 
         iter != _additional_drops.end(); iter++)
    {
        if (!plan.find(*iter))
        {
            DEBUG << "Disconnecting drop box: " << *iter;

            string xml;
            xml = "<disconnect stream=\"";
            xml += *iter;
            xml += "_out_stream"; 
            xml += "\" />";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                completion.post(RPCFault(result.stat()));
                return;
            }
        
            _additional_drops.erase(*iter);
        }
    }

    // Apply new drops.
    //
    DEBUG << "Applying new drops...";

    vector<LSPlan::drop_box> new_dip = plan._drop_insertion_plan;

    for (i = 0; i < new_dip.size(); i++)
    {
        // due to normal plan
        //
        if (_current_plan.find(new_dip[i].box_name))
        {
            DEBUG << "Adjusting drop parameters for box: "
                  << new_dip[i].box_name;

            string xml;
            xml = "<modify box=\"";
            xml += new_dip[i].box_name;
            xml += "\" >\n";
            xml += " ";
            xml += new_dip[i].box_params.get_parameter_xml();
            xml += "\n";
            xml += "</modify>";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                completion.post(RPCFault(result.stat()));
                return;
            }
            continue;
        }

        // due to additional plan
        //
        if (_additional_drops.find(new_dip[i].box_name) 
            != _additional_drops.end())
        {
            DEBUG << "Adjusting drop parameters for box: "
                  << new_dip[i].box_name;

            string xml;
            xml = "<modify box=\"";
            xml += new_dip[i].box_name;
            xml += "\" >\n";
            xml += " ";
            xml += new_dip[i].box_params.get_parameter_xml();
            xml += "\n";
            xml += "</modify>";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                completion.post(RPCFault(result.stat()));
                return;
            }
            _additional_drops.erase(new_dip[i].box_name);
            continue;
        }

        DEBUG << "Connecting drop box: " << new_dip[i].box_name;

        string xml;
        xml = "<connect box=\"";
        xml += new_dip[i].dest_box_name;
        xml += "\" port=\"";
        xml += new_dip[i].dest_box_port;
        xml += "\" node=\"";
        xml += new_dip[i].dest_node;
        xml += "\">\n";    
        xml += "  <box name=\"";
        xml += new_dip[i].box_name;
        xml += "\" type=\"";
        xml += new_dip[i].box_type;
        xml += "\">\n";
        xml += "    <in  stream=\"";
        xml += new_dip[i].in_stream_name;
        xml += "\" />\n";
        xml += "    <out stream=\"";
        xml += new_dip[i].box_name;
        xml += "_out_stream";
        xml += "\" />\n";
        xml += "   ";
        xml += new_dip[i].box_params.get_parameter_xml();
        xml += "\n";
        xml += "  </box>\n";
        xml += "</connect>";
        DEBUG << "xml=\n" << xml;
        RPC<void> result = _qp.add_xml_string(xml);

        DEBUG << "result : " << result;
        if (!result.stat())
        {
            completion.post(RPCFault(result.stat()));
            return;
        }
    }

    DEBUG << "Done send_lsplan()...";
}
*/

void LocalLoadShedder::local_apply_lsplan(int plan_id)
{
    NOTICE << " LocalLoadShedder::local_apply_lsplan (plan_id = "
           << plan_id << ")";

    if (_ls_roadmap.size() == 0)
    {
        NOTICE << "No plans, do nothing..";

        return;
    }

    if (_ls_roadmap.find(plan_id) != _ls_roadmap.end())
    {
        if (_current_plan_id == plan_id)
        {
            NOTICE << "Same plan, nothing to do..";

            return;
        }

        _current_plan_id = plan_id;

        apply_lsplan();

        _previous_plan_id = _current_plan_id;
    }
}

void LocalLoadShedder::local_apply_additional_lsplan(LSPlan plan)
{
    vector<LSPlan::drop_box> drops = plan._drop_insertion_plan;

    vector<LSPlan::drop_box> current_dip = _ls_roadmap[_current_plan_id]
                                           ->_drop_insertion_plan;

    for (unsigned int i = 0; i < drops.size(); i++)
    {
        if (_ls_roadmap[_current_plan_id]->find(drops[i].box_name))
        {
            double cur_drop_rate = atof(((_ls_roadmap[_current_plan_id]
                        ->locate(drops[i].box_name))
                        ->box_params.get_parameter("drop_rate")).c_str());
            double new_add_rate = atof((drops[i].box_params.
                        get_parameter("drop_rate")).c_str());
            double new_drop_rate = 1 - ((1 - cur_drop_rate)*(1 - new_add_rate));

            if (_additional_drops.find(drops[i].box_name.as_string())
                                            != _additional_drops.end())
            {
                double cur_add_rate = 
                        _additional_drops[drops[i].box_name.as_string()];
                double cur_total_rate = 
                        1 - ((1 - cur_drop_rate)*(1 - cur_add_rate));
                if (abs(new_drop_rate - cur_total_rate) < 0.01)
                {
                    // _additional_drops[drops[i].box_name.as_string()] stays
                    // as it is.
                    //
                    NOTICE << drops[i].box_name.as_string() 
                           << " not worth modify";
                    continue;
                }
            }
            else
            {
                if (abs(new_drop_rate - cur_drop_rate) < 0.01)
                {
                    NOTICE << drops[i].box_name.as_string() 
                           << " not worth modify";
                    _additional_drops[drops[i].box_name.as_string()] = 0;
                    continue;
                }
            }

            NOTICE << "Modifying " << drops[i].box_name;

            string xml;
            xml = "<modify box=\"";
            xml += drops[i].box_name;
            xml += "\" >\n";
            xml += " ";
            BoxParameter bp = (_ls_roadmap[_current_plan_id]
                              ->locate(drops[i].box_name))->box_params;
            ostringstream s; 
            s << new_drop_rate; 
            bp.set_parameter("drop_rate", s.str()); 
            xml += bp.get_parameter_xml();
            xml += "\n";
            xml += "</modify>";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                return;
            }
            _additional_drops[drops[i].box_name.as_string()] = new_add_rate;
            continue;
        }
        else if (_additional_drops.find(drops[i].box_name.as_string()) 
                            != _additional_drops.end())
        {
            double new_drop_rate = atof((drops[i].box_params.
                                        get_parameter("drop_rate")).c_str());
            double old_drop_rate = 
                            _additional_drops[drops[i].box_name.as_string()];

            if (abs(new_drop_rate - old_drop_rate) < 0.01)
            {
                NOTICE << drops[i].box_name.as_string() << " not worth modify";
                continue;
            }

            NOTICE << "Modifying " << drops[i].box_name;

            string xml;
            xml = "<modify box=\"";
            xml += drops[i].box_name;
            xml += "\" >\n";
            xml += " ";
            BoxParameter bp = drops[i].box_params; 
            /*
            ostringstream s; 
            s << new_drop_rate; 
            bp.set_parameter("drop_rate", s.str()); 
            */
            xml += bp.get_parameter_xml();
            xml += "\n";
            xml += "</modify>";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                return;
            }
            _additional_drops[drops[i].box_name.as_string()] = new_drop_rate;
            continue;
        }
        else
        {
            NOTICE << "Connecting drop box: " << drops[i].box_name;
        
            string xml;
            xml = "<connect box=\"";
            xml += drops[i].dest_box_name;
            xml += "\" port=\"";
            xml += drops[i].dest_box_port;
            xml += "\" node=\"";
            xml += drops[i].dest_node;
            xml += "\">\n";    
            xml += "  <box name=\"";
            xml += drops[i].box_name;
            xml += "\" type=\"";
            xml += drops[i].box_type;
            xml += "\">\n";
            xml += "    <in  stream=\"";
            xml += drops[i].in_stream_name;
            xml += "\" />\n";
            xml += "    <out stream=\"";
            xml += drops[i].box_name;
            xml += "_out_stream";
            xml += "\" />\n";
            xml += "   ";
            xml += drops[i].box_params.get_parameter_xml();
            xml += "\n";
            xml += "  </box>\n";
            xml += "</connect>";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                return;
            }

            _additional_drops[drops[i].box_name.as_string()] = 
                atof((drops[i].box_params.get_parameter("drop_rate")).c_str());
        }
    }
    
    // Now remove the ones from _additional_drops that are not in drops.
    // (Better, modify drop_rate to 0.)
    //
    for (map<string, double>::iterator iter = _additional_drops.begin(); 
         iter != _additional_drops.end(); iter++)
    {
        if (!(plan.find(Name(iter->first))))
        {
            if (_ls_roadmap[_current_plan_id]->find(Name(iter->first)))
            {
                // it is there due to the normal plan, not additional.
                //
                _additional_drops[iter->first] = 0;
                continue;
            }

            NOTICE << "Modifying " << iter->first;

            string xml;
            xml = "<modify box=\"";
            xml += iter->first;
            xml += "\" >\n";
            xml += " ";
            BoxParameter bp; 
            ostringstream s; 
            s << 0; 
            bp.set_parameter("drop_rate", s.str()); 
            xml += bp.get_parameter_xml();
            xml += "\n";
            xml += "</modify>";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                return;
            }
            _additional_drops[iter->first] = 0;
        }
    }
}

// This is a complete plan of its own.
// inits all the overlapping additional drops to 0.
//
void LocalLoadShedder::apply_lsplan()
{
    unsigned int i;

    NOTICE << "LocalLoadShedder will apply plan # " << _current_plan_id;

    // First remove redundant drops if any.
    //
    DEBUG << "Checking for redundant drops...";
    DEBUG << "Previous plan:";
    DEBUG << _ls_roadmap[_previous_plan_id]->string_rep();
    DEBUG << "Current plan:";
    DEBUG << _ls_roadmap[_current_plan_id]->string_rep();

    vector<LSPlan::drop_box> previous_dip =
                         _ls_roadmap[_previous_plan_id]->_drop_insertion_plan;

    // due to normal plan
    //
    for (i = 0; i < previous_dip.size(); i++)
    {
        if (!_ls_roadmap[_current_plan_id]->find(previous_dip[i].box_name))
        {
            if (_additional_drops.find(previous_dip[i].box_name.as_string()) 
                                      == _additional_drops.end())
            {
                NOTICE << "Disconnecting drop box: " 
                       << previous_dip[i].box_name;

                string xml;
                xml = "<disconnect stream=\"";
                xml += previous_dip[i].box_name;
                xml += "_out_stream"; 
                xml += "\" />";
                DEBUG << "xml=\n" << xml;
                RPC<void> result = _qp.add_xml_string(xml);

                DEBUG << "result : " << result;
                if (!result.stat())
                {
                    return;
                }
            }
            else // modify drop_rate=0 and let the new additional drops fix it.
            {
                NOTICE << "Modifying " << previous_dip[i].box_name;

                string xml;
                xml = "<modify box=\"";
                xml += previous_dip[i].box_name;
                xml += "\" >\n";
                xml += " ";
                BoxParameter bp; 
                ostringstream s; 
                s << 0; 
                bp.set_parameter("drop_rate", s.str()); 
                xml += bp.get_parameter_xml();
                xml += "\n";
                xml += "</modify>";
                DEBUG << "xml=\n" << xml;
                RPC<void> result = _qp.add_xml_string(xml);

                DEBUG << "result : " << result;
                if (!result.stat())
                {
                    return;
                }
                _additional_drops[previous_dip[i].box_name.as_string()] = 0;
            }
        }
    }

    /*
    // due to additional plan
    //
    for (set<Name>::iterator iter = _additional_drops.begin(); 
         iter != _additional_drops.end(); iter++)
    {
        if (!_ls_roadmap[_current_plan_id]->find(*iter))
        {
            DEBUG << "Disconnecting drop box: " << *iter;

            string xml;
            xml = "<disconnect stream=\"";
            xml += *iter;
            xml += "_out_stream"; 
            xml += "\" />";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                //completion.post(RPCFault(result.stat()));
                return;
            }
        
            _additional_drops.erase(*iter);
        }
    }
    */

    // Apply new drops.
    //
    DEBUG << "Applying new drops...";

    vector<LSPlan::drop_box> current_dip =
                        _ls_roadmap[_current_plan_id]->_drop_insertion_plan;

    for (i = 0; i < current_dip.size(); i++)
    {
        // due to normal plan
        //
        if (_ls_roadmap[_previous_plan_id]->find(current_dip[i].box_name))
        {
            DEBUG << "Adjusting drop parameters for box: "
                  << current_dip[i].box_name;

            double cur_drop_rate = atof((current_dip[i].box_params.
                                         get_parameter("drop_rate")).c_str());

            double prev_drop_rate = atof(((_ls_roadmap[_previous_plan_id]
                    ->locate(current_dip[i].box_name))
                    ->box_params.get_parameter("drop_rate")).c_str());

            _additional_drops[current_dip[i].box_name.as_string()] = 0;

            if (abs(cur_drop_rate - prev_drop_rate) < 0.01)
            {
                NOTICE << current_dip[i].box_name.as_string() 
                       << " not worth modify";
                continue;
            }

            NOTICE << "Modifying " << current_dip[i].box_name;

            string xml;
            xml = "<modify box=\"";
            xml += current_dip[i].box_name;
            xml += "\" >\n";
            xml += " ";
            xml += current_dip[i].box_params.get_parameter_xml();
            xml += "\n";
            xml += "</modify>";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                //completion.post(RPCFault(result.stat()));
                return;
            }
            continue;
        }

        // due to additional plan
        //
        if (_additional_drops.find(current_dip[i].box_name.as_string()) 
            != _additional_drops.end())
        {
            DEBUG << "Adjusting drop parameters for box: "
                  << current_dip[i].box_name;

            double cur_drop_rate = atof((current_dip[i].box_params.
                                         get_parameter("drop_rate")).c_str());

            double add_drop_rate = 
                    _additional_drops[current_dip[i].box_name.as_string()];

            if (abs(cur_drop_rate - add_drop_rate) < 0.01)
            {
                NOTICE << current_dip[i].box_name.as_string() 
                       << " not worth modify";
                _additional_drops[current_dip[i].box_name.as_string()] = 0;
                continue;
            }

            NOTICE << "Modifying " << current_dip[i].box_name;

            string xml;
            xml = "<modify box=\"";
            xml += current_dip[i].box_name;
            xml += "\" >\n";
            xml += " ";
            xml += current_dip[i].box_params.get_parameter_xml();
            xml += "\n";
            xml += "</modify>";
            DEBUG << "xml=\n" << xml;
            RPC<void> result = _qp.add_xml_string(xml);

            DEBUG << "result : " << result;
            if (!result.stat())
            {
                return;
            }
            _additional_drops[current_dip[i].box_name.as_string()] = 0;
            continue;
        }

        NOTICE << "Connecting drop box: " << current_dip[i].box_name;

        string xml;
        xml = "<connect box=\"";
        xml += current_dip[i].dest_box_name;
        xml += "\" port=\"";
        xml += current_dip[i].dest_box_port;
        xml += "\" node=\"";
        xml += current_dip[i].dest_node;
        xml += "\">\n";    
        xml += "  <box name=\"";
        xml += current_dip[i].box_name;
        xml += "\" type=\"";
        xml += current_dip[i].box_type;
        xml += "\">\n";
        xml += "    <in  stream=\"";
        xml += current_dip[i].in_stream_name;
        xml += "\" />\n";
        xml += "    <out stream=\"";
        xml += current_dip[i].box_name;
        xml += "_out_stream";
        xml += "\" />\n";
        xml += "   ";
        xml += current_dip[i].box_params.get_parameter_xml();
        xml += "\n";
        xml += "  </box>\n";
        xml += "</connect>";
        DEBUG << "xml=\n" << xml;
        RPC<void> result = _qp.add_xml_string(xml);

        DEBUG << "result : " << result;
        if (!result.stat())
        {
            return;
        }
    }

    DEBUG << "Done applyLSPlan()...";
}

BOREALIS_NAMESPACE_END

//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_LocalLoadShedder.h"
#undef NMSTL_RPC_DEFINE
//---

#include "ContractBasedOptimizer.h"
#include <xercesc/dom/DOM.hpp>
#include "xercesDomUtil.h"

BOREALIS_NAMESPACE_BEGIN

// --------------------------------------------------
ContractBasedOptimizer::ContractBasedOptimizer(string id)
    : NHOptimizer(id), _total_cpu(0.0), _total_bw(0.0),
      _bw_utilization(0.0), _cpu_utilization(0.0), _load_level(0.0),
      _marginal_cost(0.0), _offer_pending(false), _movement_in_progress(false)
      {

    // Xerces is NOT thread safe so we must perform this init in
    // the constructor before starting our own thread
    //init_contracts();

}

// --------------------------------------------------
/// Accumulates load offers from partners... will look at them later
void
ContractBasedOptimizer::handle_load_offer(AsyncRPC< ptr<LoadOffer> > completion, ptr<LoadOffer> offer)
{
    _load_offers.push_back(offer);
    _load_offer_completions[offer] = completion;
}

// --------------------------------------------------
void
ContractBasedOptimizer::in_thread_init()
{
/*
    (new CallbackTimer(m_my_loop,wrap(this,&ContractBasedOptimizer::update_stats)))->arm(Time::now() + Time::msecs(PERFMON_INTERVAL));
*/
}


/***********************************
// --------------------------------------------------
/// Reads list of contracts from a config file
void
ContractBasedOptimizer::init_contracts()
{

    try
    {

        INFO << "Filename is " << _config_file_name;
        ptr<DOMDocument> doc = parse_xml_file(_config_file_name);
        const DOMElement *root = doc->getDocumentElement();

        xml_expect_tag(root,"medusaconfig");

        vector<DOMElement*> contract_list;
        vector<DOMElement*>::iterator i;
        xml_child_elements(contract_list,root,"load_management");

        for (i = contract_list.begin(); i != contract_list.end(); ++i)
        {

            vector<DOMElement*> contracts;
            vector<DOMElement*>::iterator j;
            xml_child_elements(contracts,*i,"contract");
            for (j = contracts.begin(); j != contracts.end(); ++j)
            {
                string src,dst;
                get_dom_attribute((**j),"node1",src);
                if ( src == to_string(_id) )
                {
                    get_dom_attribute((**j),"node2",dst);
                    double min,max;
                    get_dom_attribute((**j),"min_price",min);
                    get_dom_attribute((**j),"max_price",max);
                    Contract c(dst,min,max);
                    _contracts.push_back(c);
                    INFO << "Added contract " << c;
                }
            }
        }
    } catch (  AuroraException& e)
    {
        ERROR <<  e.what();
    }

}
*******************************/


// --------------------------------------------------
void
ContractBasedOptimizer::update_stats()
{

    vector<Name> list_hunks;
    DEBUG << "-> Updating stats ";
    _operator_stats.clear();
    _qp.get_stats(wrap(this,&ContractBasedOptimizer::update_stats_2));
}


// --------------------------------------------------
void
ContractBasedOptimizer::update_stats_2(RPC<StatsList> result)
{

    static int iterations = 0;

    DEBUG << "-> Updating stats 2";

    double agg_input_rate = 0.0;
    if (!result.valid())
        ERROR << result.stat();
    else
    {
        _total_cpu = 0.0;
        _total_bw = 0.0;
        _bw_utilization = 0.0;
        _operator_stats = *result;
        for ( StatsList::iterator s = _operator_stats.begin();
              s != _operator_stats.end(); ++s)
              {
            DEBUG << *s;
            agg_input_rate += s->_bw_in;
            _total_cpu += s->_cpu_usage;
            _total_bw += s->_bw_out;
            _bw_utilization += s->_bw_usage;
        }
    }

    _cpu_utilization = _total_cpu;
    if ( _bw_utilization >= 1 || _cpu_utilization >= 1)
        _load_level = MAX_UTILIZATION;
    else
        _load_level = _cpu_utilization / ( 1 - _cpu_utilization)
            + _bw_utilization / ( 1 - _bw_utilization);
    _marginal_cost = 1.0 / (2 * ( 1 - _cpu_utilization)*(1 - _cpu_utilization))
        + 1.0/ (2 * ( 1 - _bw_utilization)*(1-_bw_utilization));
    INFO  << "Current bw utilization " << 100*_bw_utilization
          << "% and CPU utilization " << 100*_cpu_utilization << "%";
    INFO  << "Current total load level is " << _load_level
          << " and MC is " << _marginal_cost ;
    iterations++;
    INFO << "Iteration number is " << iterations;
    if ( iterations >= SHED_LOAD_PERIODS)
    {
        iterations = 0;
        INFO << "Will try to send load offers ";
        send_out_load_offers();
        examine_load_offers();
    }
    (new CallbackTimer(_my_loop,wrap(this,&ContractBasedOptimizer::update_stats)))->arm(Time::now() + Time::msecs(PERFMON_INTERVAL));
}

// --------------------------------------------------
/// Utility function: estimates the MC of adding or removing a box
double
ContractBasedOptimizer::compute_avg_marginal_cost(double cpu_usage, double delta_cpuusage,
                                   double bw_usage, double delta_bwusage)
                                   {

    if ( cpu_usage >= 1 || bw_usage  >= 1)
        throw OverloadedException("Overloaded!");

    double cost_before = cpu_usage / ( 1 - cpu_usage) + bw_usage / ( 1 - bw_usage);

    double cpu_after = cpu_usage + delta_cpuusage;
    double bw_after = bw_usage + delta_bwusage;

    if ( bw_after >= 1 || cpu_after >= 1)
        throw OverloadedException("Overloaded!");

    double cost_after = cpu_after / ( 1 - cpu_after) + bw_after / ( 1 - bw_after);

    return (cost_after - cost_before)/(delta_cpuusage + delta_bwusage);

}

// --------------------------------------------------
void ContractBasedOptimizer::reset_partners()
{

    for ( Contracts::iterator c = _contracts.begin(); c != _contracts.end(); ++c)
    {
        (*c)._last_offer_rejected = false;
        if ( (*c)._broken)
        {
            clear_endpoints_for((*c)._partner);
            (*c)._broken = false;
        }
    }
}


// --------------------------------------------------
/// Offers load to partners if marginal cost is high
void
ContractBasedOptimizer::send_out_load_offers()
{

    INFO << "Perhaps we could offload something...";
    if ( _offer_pending || _movement_in_progress )
    {
        INFO << "An offer is already pending or a load movement is in progress";
        return;
    }

    //INFO << "Examining contracts";

    // Find contract with current_price < marginal cost
    _contracts.sort();

    // If all partners either rejected our last offer or were broken, clear and try everyone again
    if ( _contracts.begin()->_last_offer_rejected || _contracts.begin()->_broken)
    {
        reset_partners();
    }

    Contracts::iterator i = _contracts.begin();
    while ( i != _contracts.end() )
    {

        INFO << _id << " found contract at cost " <<(*i)._current_price
             << " while MC is " << _marginal_cost;

        // pick a random set of boxes that together represent enough load
        StatsList boxes;
        pick_boxes(_operator_stats,boxes,(*i)._current_price);
        if ( boxes.size() <= 0 )
        {
            reset_partners();
            break;
        }

        // Submit offer
        ptr<LoadOffer> p(new LoadOffer(_id,boxes,(*i)._current_price));

        INFO << "Sending to " << (*i)._partner << ", load offer " << p->repr();
        Remote<NHOptimizer> dst_nhopt = nhopt_for((*i)._partner);
        if ( ! dst_nhopt )
        {
            ERROR << "Can not contact partner " << (*i)._partner;
            (*i)._broken = true;
            continue;
        } else
        {
            _offer_pending = true;
            dst_nhopt.handle_load_offer( wrap(this,&ContractBasedOptimizer::handle_load_offer_response,
                                              (*i)._partner, p),p);
            break;
        }
        ++i;
    }

}

// --------------------------------------------------
/// Selects boxes to offload
void
ContractBasedOptimizer::pick_boxes(StatsList& src, StatsList& dst, double threshold)
{

    double total_delta_cpu = 0.0;
    double total_delta_bw = 0.0;

    // Compute cumulative marginal cost, one box at the time
    for ( StatsList::iterator i = src.begin(); i != src.end(); ++i )
    {

        if ( (*i)._cpu_usage > 0 )
        {

            double marginal_cost;
            bool overload = false;
            try
            {
                marginal_cost = compute_avg_marginal_cost(_cpu_utilization-total_delta_cpu,-(*i)._cpu_usage,
                                                          _bw_utilization-total_delta_bw,-(*i)._bw_usage);
            } catch(OverloadedException e)
            {
                INFO << "Node is totally overloaded, offering all the boxes";
                overload = true;
                marginal_cost = -1;
            }
            if ( marginal_cost > threshold + EPSILON_COST || overload)
            {
                dst.push_back((*i));
                total_delta_cpu += (*i)._cpu_usage;
                total_delta_bw += (*i)._bw_usage;
                DEBUG << "Marginal cost for offloading box " << (*i).repr() << " is " << marginal_cost;
                DEBUG << "Adding box " << (*i) << " to offered boxes ";
            } else
            {
                DEBUG << "Marginal cost for offloading box " << (*i).repr() << " is " << marginal_cost;
            }
        }
    }

}

// --------------------------------------------------
void
ContractBasedOptimizer::examine_load_offers()
{

    double total_accepted_cpu = 0.0;
    double total_accepted_bw = 0.0;

    INFO << "Examining load offers received";
    _load_offers.sort();
    LoadOffers::reverse_iterator i;
    for ( i = _load_offers.rbegin(); i != _load_offers.rend(); ++i )
    {

        DEBUG << "Examining load offer " << (*i);
        if ( _movement_in_progress )
        {
            DEBUG << "Rejecting load offer because a load movement is already in progress";
            (*i)->_counter_offer = REJECT;
            _load_offer_completions[*i].post(*i);
        } else
        {
            try
            {
                pick_new_boxes((*i)->_boxes,(*i)->_accepted_boxes,(*i)->_offer,
                               total_accepted_cpu,total_accepted_bw);
                if ( (*i)->_accepted_boxes.size() > 0 )
                    (*i)->_counter_offer = (*i)->_offer;
                else // Using fixed min price only for now
                    (*i)->_counter_offer = REJECT;
            } catch ( OverloadedException e)
            {
                DEBUG << "Node is totally overloaded, should not accept boxes";
                (*i)->_counter_offer = REJECT;
            }
            INFO << "Posting response to load offer" << (*i)->repr();
            _load_offer_completions[*i].post(*i);
        }
    }
    _load_offers.clear();
    _load_offer_completions.clear();

}

// --------------------------------------------------
/// Selects boxes that can be accepted
void
ContractBasedOptimizer::pick_new_boxes(StatsList& src, StatsList& dst, double threshold,
                        double& accepted_delta_cpu, double& accepted_delta_bw)
                        {

    // Compute cumulative marginal cost, one box at the time
    for ( StatsList::iterator i = src.begin(); i != src.end(); ++i )
    {

        double marginal_cost;
        bool overload = false;
        try
        {
            marginal_cost = compute_avg_marginal_cost(_cpu_utilization+accepted_delta_cpu,(*i)._cpu_usage,
                                                      _bw_utilization+accepted_delta_bw,(*i)._bw_usage);
        } catch(OverloadedException e)
        {
            INFO << "Node is totally overloaded, offering all the boxes";
            overload = true;
            marginal_cost = -1;
        }
        if ( marginal_cost < threshold - EPSILON_COST )
        {
            dst.push_back((*i));
            accepted_delta_cpu += (*i)._cpu_usage;
            accepted_delta_bw += (*i)._bw_usage;
            INFO << "Marginal cost for adding box " << (*i).repr() << " is " << marginal_cost;
            INFO << "Adding box " << (*i) << " to accepted boxes ";
        } else
        {
            INFO << "Marginal cost for adding box " << (*i).repr() << " is " << marginal_cost;
        }
    }
}

// --------------------------------------------------
vector<Name>
ContractBasedOptimizer::extract_names(vector<Stats> boxes)
{

    vector<Name> names;
    for ( vector<Stats>::iterator i = boxes.begin(); i != boxes.end(); ++i)
    {
        names.push_back((*i)._name);
    }
    return names;

}

// --------------------------------------------------
void
ContractBasedOptimizer::handle_load_offer_response(string partner, ptr<LoadOffer> original_offer,
                                    RPC< ptr<LoadOffer> > counter_offer)
                                    {

    bool rejected = false;

    if (!counter_offer.valid())
    {
        ERROR << "Partner failed while handling load offer " << *original_offer;
        for ( Contracts::iterator c = _contracts.begin(); c != _contracts.end(); ++c)
        {
            if ( (*c)._partner == partner)
            {
                (*c)._broken = true;
                (*c)._last_offer_rejected = false;
            }
        }
    } else
    {
        DEBUG << "Handling load offer response: " << (**counter_offer).repr();
        if ( ((*counter_offer)->_counter_offer == (*counter_offer)->_offer) &&
             ((*counter_offer)->_accepted_boxes.size() > 0) )
             {
            vector<Name> box_names = extract_names((*counter_offer)->_accepted_boxes);
            DEBUG << "Offer " << (**counter_offer) << " was accepted... launching load movement";
            // First start movement, then clear offer pending
            _movement_in_progress = true;

            /************************* yna create_query
            _qp.move_load_without_state( wrap(this,&ContractBasedOptimizer::done_moving_load),
                                             box_names,partner);
            ************************************/
        } else
        {
            DEBUG << "The offer was rejected... ";
            rejected = true;
        }
        for ( Contracts::iterator c = _contracts.begin(); c != _contracts.end(); ++c)
        {
            if ( (*c)._partner == partner)
            {
                (*c)._last_offer_rejected = rejected;
                (*c)._broken = false;
            }
        }
    }
    _offer_pending = false;
}

// --------------------------------------------------
void
ContractBasedOptimizer::done_moving_load(RPC<void> result)
{

    if ( !result.valid() )
        INFO << "Load movement failed " << result.stat();
    else
    {
        INFO << "Load movement completed successfully.";
        _load_moves.push_back(Time::now());
    }

    _movement_in_progress = false;

}

BOREALIS_NAMESPACE_END

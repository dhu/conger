#include "QOSOptimizer.h"
//#include <xercesc/dom/DOM.hpp>
//#include "xercesDomUtil.h"

BOREALIS_NAMESPACE_BEGIN

// --------------------------------------------------
QOSOptimizer::QOSOptimizer(string id)
    : BasicComponent(id, "QOSOptimizer"), _total_cpu(0.0), _total_bw(0.0),
      _bw_utilization(0.0), _cpu_utilization(0.0), _load_level(0.0),
      _marginal_cost(0.0), _offer_pending(false), _movement_in_progress(false)
{

    // Xerces is NOT thread safe so we must perform this init in
    // the constructor before starting our own thread
    //init_contracts();
  _lifetime=200000;
  _sel = 0;
  counter = 0;
  where_join=true;
  switch_loc = false;
}

RPC<int> QOSOptimizer::check_switch_loc()
{
  if (switch_loc)
  {
    switch_loc = false;
    return 1;
  }
  else
    return 0;
}

RPC<void> QOSOptimizer::do_update(double life, double tp)
{

  double alpha = 0.3;

  NOTICE << "life is: " << life << "\n";
  _lifetime = (alpha*_lifetime) + ((1 - alpha) * life);
  NOTICE << "m_lifetime is: " << _lifetime << "\n";
  if (where_join)
    _qp.get_sel(wrap(this,&QOSOptimizer::getsel));
  else
    update_sel(tp);
  return true;

}

void QOSOptimizer::update_sel(double sel)
{
  NOTICE << "sel is: " << sel << " " << counter;
    if (where_join == true)
    {
    if (sel <=.5)
    {
      counter++;
    }
    else
    {
      //counter = 0;
    }
  }
  else
  {
    if (sel >.5)
    {
      counter++;
    }
    else
    {
      //counter = 0;
    }
    }
    //counter++;

  if ( counter == 10)
  {
    counter = 0;
    switch_loc = true;
    //m_completion.post(true);
    where_join = !where_join;
  }

}

void
QOSOptimizer::getsel(RPC<double> result)
{
  double ans = *result;
  NOTICE << "selectivity is: " << ans;
  update_sel(ans);
}

// --------------------------------------------------
/// Return the currents statistics for the list of queries
RPC<vector<Stats> > QOSOptimizer::get_stat()
{

    vector<Stats> list_stats;

    // for each query load is aggregate tuple input rate times PER_TUPLE_COST
    //QueryStates::iterator i;
    //for ( i = m_queries.begin(); i != m_queries.end(); ++i)
    //{
    //    ptr<QueryState> qs = i->second;

        // Input streams

    Stats stats; //(/*i->first*/);
    //double agg_input_rate = 0.0;
    /*    const vector<Name> input_streams = qs->m_query.get_input_streams();
        vector<Name>::const_iterator j;
        for (j = input_streams.begin(); j != input_streams.end(); ++j)
        {
            ptr<StreamState> stream_state = lookup(m_streams,*j);
            if ( stream_state )
            {
                agg_input_rate += stream_state->m_stream->m_stream_stats.m_tuple_rate;
                stats.m_bw_in += stream_state->m_stream->m_stream_stats.m_byte_rate;
            }
        }

        stats.m_cpu_usage = agg_input_rate * PER_TUPLE_COST;

        // Output streams
        const vector<Name> output_streams = qs->m_query.get_non_input_streams();
        vector<Name>::const_iterator k;
        for (k = output_streams.begin(); k != output_streams.end(); ++k)
        {
            ptr<StreamState> stream_state = lookup(m_streams,*k);
            if ( stream_state )
                stats.m_bw_out += stream_state->m_stream->m_stream_stats.m_byte_rate;
        }
        stats.m_bw_usage = stats.m_bw_out / MAX_BW;

        DEBUG << "Computed stats for query : \n >>> " << stats.repr();*/
    stats._bw_in=_lifetime;
        list_stats.push_back(stats);
    //}

    return list_stats;

}


// --------------------------------------------------
/// Accumulates load offers from partners... will look at them later
void
QOSOptimizer::handle_load_offer(AsyncRPC< ptr<LoadOffer> > completion, ptr<LoadOffer> offer)
{
    _load_offers.push_back(offer);
    _load_offer_completions[offer] = completion;
}

// --------------------------------------------------
void
QOSOptimizer::in_thread_init()
{
/*
    (new CallbackTimer(m_my_loop,wrap(this,&QOSOptimizer::update_stats)))->arm(Time::now() + Time::msecs(PERFMON_INTERVAL));
*/
}


// --------------------------------------------------
/// Reads list of contracts from a config file

/*void
ContractBasedOptimizer::init_contracts()
{

    try
    {

        WARN << "Filename is " << m_configFileName;
        ptr<DOMDocument> doc = parseXmlFile(m_configFileName);
        const DOMElement *root = doc->getDocumentElement();

        xmlExpectTag(root,"medusaconfig");

        vector<DOMElement*> contract_list;
        vector<DOMElement*>::iterator i;
        xmlChildElements(contract_list,root,"load_management");

        for (i = contract_list.begin(); i != contract_list.end(); ++i)
        {

            vector<DOMElement*> contracts;
            vector<DOMElement*>::iterator j;
            xmlChildElements(contracts,*i,"contract");
            for (j = contracts.begin(); j != contracts.end(); ++j)
            {
                string src,dst;
                getDomAttribute((**j),"node1",src);
                if ( src == to_string(m_id) )
                {
                    getDomAttribute((**j),"node2",dst);
                    double min,max;
                    getDomAttribute((**j),"min_price",min);
                    getDomAttribute((**j),"max_price",max);
                    Contract c(dst,min,max);
                    m_contracts.push_back(c);
                    WARN << "Added contract " << c;
                }
            }
        }
    } catch (  AuroraException& e)
    {
        ERROR <<  e.what();
    }

    }*/

// --------------------------------------------------
void
QOSOptimizer::update_stats()
{

    vector<Name> list_hunks;
    DEBUG << "-> Updating stats ";
    _operator_stats.clear();
    _qp.get_stats(wrap(this,&QOSOptimizer::update_stats_2));

}


// --------------------------------------------------
void
QOSOptimizer::update_stats_2(RPC<StatsList> result)
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
    (new CallbackTimer(_my_loop,wrap(this,&QOSOptimizer::update_stats)))->arm(Time::now() + Time::msecs(PERFMON_INTERVAL));
}

// --------------------------------------------------
/// Utility function: estimates the MC of adding or removing a box
double
QOSOptimizer::compute_avg_marginal_cost(double cpu_usage, double delta_cpuusage,
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
void QOSOptimizer::reset_partners()
{
  /*
    for ( Contracts::iterator c = m_contracts.begin(); c != m_contracts.end(); ++c)
    {
        (*c).m_last_offer_rejected = false;
        if ( (*c).m_broken)
        {
            clear_endpoints_for((*c).m_partner);
            (*c).m_broken = false;
        }
    }
  */
}


// --------------------------------------------------
/// Offers load to partners if marginal cost is high
void
QOSOptimizer::send_out_load_offers()
{
  /*
    INFO << "Perhaps we could offload something...";
    if ( m_offerPending || m_movementInProgress )
    {
        WARN << "An offer is already pending or a load movement is in progress";
        return;
    }

    //WARN << "Examining contracts";

    // Find contract with current_price < marginal cost
    m_contracts.sort();

    // If all partners either rejected our last offer or were broken, clear and try everyone again
    if ( m_contracts.begin()->m_last_offer_rejected || m_contracts.begin()->m_broken)
    {
        reset_partners();
    }

    Contracts::iterator i = m_contracts.begin();
    while ( i != m_contracts.end() )
    {

        INFO << m_id << " found contract at cost " <<(*i).m_current_price
             << " while MC is " << m_marginal_cost;

        // pick a random set of boxes that together represent enough load
        StatsList boxes;
        pick_boxes(m_operatorStats,boxes,(*i).m_current_price);
        if ( boxes.size() <= 0 )
        {
            reset_partners();
            break;
        }

        // Submit offer
        ptr<LoadOffer> p(new LoadOffer(m_id,boxes,(*i).m_current_price));

        WARN << "Sending to " << (*i).m_partner << ", load offer " << p->repr();
        Remote<NHOptimizer> dst_nhopt = nhopt_for((*i).m_partner);
        if ( ! dst_nhopt )
        {
            ERROR << "Can not contact partner " << (*i).m_partner;
            (*i).m_broken = true;
            continue;
        } else
        {
            m_offerPending = true;
            dst_nhopt.handle_load_offer( wrap(this,&ContractBasedOptimizer::handle_load_offer_response,
                                              (*i).m_partner, p),p);
            break;
        }
        ++i;
    }
  */
}

// --------------------------------------------------
/// Selects boxes to offload
void
QOSOptimizer::pick_boxes(StatsList& src, StatsList& dst, double threshold)
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
                WARN << "Node is totally overloaded, offering all the boxes";
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
QOSOptimizer::examine_load_offers()
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
            WARN << "Posting response to load offer" << (*i)->repr();
            _load_offer_completions[*i].post(*i);
        }
    }
    _load_offers.clear();
    _load_offer_completions.clear();

}

// --------------------------------------------------
/// Selects boxes that can be accepted
void
QOSOptimizer::pick_new_boxes(StatsList& src, StatsList& dst, double threshold,
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
            WARN << "Node is totally overloaded, offering all the boxes";
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
QOSOptimizer::extract_names(vector<Stats> boxes)
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
QOSOptimizer::handle_load_offer_response(string partner, ptr<LoadOffer> original_offer,
                                    RPC< ptr<LoadOffer> > counter_offer)
                                    {
  /*
    bool rejected = false;

    if (!counter_offer.valid())
    {
        ERROR << "Partner failed while handling load offer " << *original_offer;
        for ( Contracts::iterator c = m_contracts.begin(); c != m_contracts.end(); ++c)
        {
            if ( (*c).m_partner == partner)
            {
                (*c).m_broken = true;
                (*c).m_last_offer_rejected = false;
            }
        }
    } else
    {
        DEBUG << "Handling load offer response: " << (**counter_offer).repr();
        if ( ((*counter_offer)->m_counter_offer == (*counter_offer)->m_offer) &&
             ((*counter_offer)->m_accepted_boxes.size() > 0) )
             {
            vector<Name> box_names = extract_names((*counter_offer)->m_accepted_boxes);
            DEBUG << "Offer " << (**counter_offer) << " was accepted... launching load movement";
            // First start movement, then clear offer pending
            m_movementInProgress = true;
            m_admin.move_load( wrap(this,&ContractBasedOptimizer::done_moving_load),
                               box_names,partner);
        } else
        {
            DEBUG << "The offer was rejected... ";
            rejected = true;
        }
        for ( Contracts::iterator c = m_contracts.begin(); c != m_contracts.end(); ++c)
        {
            if ( (*c).m_partner == partner)
            {
                (*c).m_last_offer_rejected = rejected;
                (*c).m_broken = false;
            }
        }
    }
    m_offerPending = false;
  */
}

// --------------------------------------------------
void
QOSOptimizer::done_moving_load(RPC<void> result)
{
  /*
    if ( !result.valid() )
        WARN << "Load movement failed " << result.stat();
    else
    {
        WARN << "Load movement completed successfully.";
        m_load_moves.push_back(Time::now());
    }

    m_movementInProgress = false;
  */
}

BOREALIS_NAMESPACE_END

//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_QOSOptimizer.h"
#undef NMSTL_RPC_DEFINE
//---

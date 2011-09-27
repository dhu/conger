#ifndef MEDUSA_CONTRACTBASEDOPT_H
#define MEDUSA_CONTRACTBASEDOPT_H

#include "NHOptimizer.h"

MEDUSA_NAMESPACE_BEGIN


// --------------------------------------------------
/**
 * Represents one contract between this node and another one
 */
class Contract
{
 public:
    Contract(string partner = "", double min_price = 0.0, double max_price = 0.0)
        : _partner(partner), _min_price(min_price),
        _current_price(min_price), _max_price(max_price), _last_offer_rejected(false), _broken(false)  {}
    string _partner;
    double _min_price;
    double _current_price;
    double _max_price;
    bool _last_offer_rejected;
    bool _broken; // Indicates if there is something wrong with talking to this partner
    bool operator<(const Contract& c) const
    {

        // Broken/unresponsive partners go last
        if ( c._broken != _broken)
        {
            return  c._broken;
        }

        // Before last go all partners that last rejected our offer
        if ( c._last_offer_rejected != _last_offer_rejected)
        {
            return  c._last_offer_rejected;
        }

        // Finally we sort on price
        return ( _current_price < c._current_price);

        // If prices are the same, this contract is lower if didn't reject while partner did
        //if ( c.m_current_price == m_current_price)
        //{
        //  return (!m_last_offer_rejected) && c.m_last_offer_rejected;
        //} else
        //{
        //   return ( m_current_price < c.m_current_price);
        //}
    }

    NMSTL_SIMPLY_SERIALIZABLE(Contract, << _partner << _min_price << _max_price);

    string as_string() const
    {
        stringstream out;
        out.precision(3);
        out << "{Contract with " << _partner << "[" << _min_price << ":"
            << _current_price << ":" << _max_price << "]";

        return out.str();
    }

};
NMSTL_TO_STRING(Contract);


// --------------------------------------------------
/**
 * Contract-based optimizer
 */
class ContractBasedOptimizer : public NHOptimizer
{

 public:
    ContractBasedOptimizer(string id);
    ~ContractBasedOptimizer() {}

 protected:
    void in_thread_init();
    void handle_load_offer(AsyncRPC< ptr<LoadOffer> > completion, ptr<LoadOffer> offer);

 private:

    class OverloadedException
    {
    public:
        string _message;
        OverloadedException(string message): _message(message) { }
    };

    //static const double EPSILON_COST = 0.002;
    static const double EPSILON_COST = 0.1;

    // Cached version of latest stats
    typedef vector< Stats > StatsList;
    vector<Stats> _operator_stats;
    double _total_cpu; // Total used in percent
    double _total_bw; // Total used in MBytes
    double _bw_utilization; // Fraction total used (ratio)
    double _cpu_utilization; // Fraction total used (ratio)
    double _load_level;
    double _marginal_cost;


    typedef list< Contract > Contracts;
    Contracts _contracts;

    /// Initialize all contracts from config file
    //void init_contracts();

    /// Periodically request statistics about local load
    void update_stats();
    void update_stats_2(RPC<StatsList> result);

    /// Compute marginal cost based given current load conditions and proposed load variation
    double compute_avg_marginal_cost(double cpu_usage, double delta_cpu, double bw_usage, double delta_bw);

    // Giving load
    bool _offer_pending;

    /**
     * Reset all flags indicating that some partners failed or refused
     * our last load offer.
     */
    void reset_partners();
    void send_out_load_offers();
    void pick_boxes(StatsList& src, StatsList& dst, double threshold);
    void handle_load_offer_response(string partner, ptr<LoadOffer> original_offer,
                                    RPC< ptr<LoadOffer> > counter_offer);
    void done_moving_load(RPC<void> stat);
    vector<Name> extract_names(vector<Stats> boxes);

    // Taking load
    bool _movement_in_progress;
    typedef list< ptr<LoadOffer> > LoadOffers;
    typedef map < ptr<LoadOffer>, AsyncRPC< ptr<LoadOffer> > > LoadOfferCompletions;
    LoadOffers _load_offers;
    LoadOfferCompletions _load_offer_completions;
    void examine_load_offers();
    void pick_new_boxes(StatsList& src, StatsList& dst, double threshold,
                        double& accepted_delta_cpu, double& accepted_delta_bw);


};

MEDUSA_NAMESPACE_END

#endif

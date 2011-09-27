#ifndef BOREALIS_QOSOPT_H
#define BOREALIS_QOSOPT_H

#include "BasicComponent.h"

BOREALIS_NAMESPACE_BEGIN


// --------------------------------------------------
/**
 * QOS optimizer
 */
class QOSOptimizer : public BasicComponent
{

 public:
    QOSOptimizer(string id);
    ~QOSOptimizer() {}

    // RPC: #include "Stats.h"

    NMSTL_RPC_OBJECT(QOSOptimizer);

    RPC<void> do_update(double life, double tp);
    RPC<int> check_switch_loc();
    RPC<vector<Stats> > get_stat();


 protected:
    void in_thread_init();
    void handle_load_offer(AsyncRPC< ptr<LoadOffer> > completion, ptr<LoadOffer> offer);

 private:

    // Completion for initial request
    AsyncRPC<void> _completion;


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

    double _lifetime;
    double _sel;
    int counter;
    bool where_join;
    bool switch_loc;

    //typedef list< Contract > Contracts;
    //Contracts m_contracts;

    /// Initialize all contracts from config file
    //void init_contracts();

    /// Periodically request statistics about local load
    void update_stats();
    void update_stats_2(RPC<StatsList> result);
    void getsel(RPC<double> result);
    void update_sel(double sel);

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

BOREALIS_NAMESPACE_END

#endif

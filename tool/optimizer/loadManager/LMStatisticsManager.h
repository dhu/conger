#ifndef LMStatisticsManager_H
#define LMStatisticsManager_H

#include "LMCommon.h"
#include "LMStatisticsPool.h"
#include "LMParams.h"
#include "LMParamTypes.h"
#include "LMQueryNetwork.h"

BOREALIS_NAMESPACE_BEGIN

using namespace StatisticsTools;

/**
 * The LMStatisticsManager maintains and computes the statistics of the system.
 */
class LMStatisticsManager
{

public:
    /**
     * Statistics names mained by the LMStatisticsManager
     */
    static const string STREAM_RATE;
    static const string SELECTIVITY;
    static const string BOX_COST;
    static const string BOX_LOAD;
    static const string NODE_LOAD;

public:
    /**
     * Returns the singleton of the LMStatisticsManager
     */
    static LMStatisticsManager* get_singleton()
    {
        if (_singleton == 0)
          _singleton = new LMStatisticsManager();
        return _singleton;
    }

    /**
     * Set load evaluation parameters
     */
    void set_parameters(LoadDistributionPeriod period);

    /**-----------------------------------------------------
     * Function that reads the statistics Timeseries
     *-----------------------------------------------------*/

    const TimeSeries& get_time_series(
            const string& statistics_type, 
            const IdType& id)
    {
        return _statistics_pool.get_statistics(statistics_type, id);
    }


    /**
     * Returns the Timeseries of the load of an LMBox
     * @param box the given LMBox
     */
    TimeSeries& get_box_load_time_series(const IdType& box_id)
    {
      return _statistics_pool.get_statistics(BOX_LOAD, box_id);}

    /**
     * Returns the total laod TimeSeries of a set of aurora boxes
     * @param box_ids the IDs of the given boxes
     */
    TimeSeries get_boxes_load_time_series(const IdSet& box_ids);


    /**
     * Returns the Timeseries of the load of a Node
     * @param node the given Node
     */
    const TimeSeries& get_node_load_time_series(const IdType& node_id)
    {
      return _statistics_pool.get_statistics(NODE_LOAD, node_id);}

    /**
     * Returns the total load of the system
     */
    TimeSeries get_system_total_load_time_series();

    /**
     * Returns the TimeSeries of the bandwidth an LMArc
     * @param arc the given LMArc
     */
    TimeSeries get_arc_bandwidth_time_series(const IdType& arc_id)
    {
      return _statistics_pool.get_statistics(STREAM_RATE, arc_id) *
             LMQueryNetwork::get_singleton()->get_arc_tuple_size(arc_id); }

    /**
     * Returns the total bandwidth TimeSeries of a set of arcs
     * @param arc_ids the IDs of the given arcs
     */
    TimeSeries get_arcs_bandwidth_time_series(const IdSet& arc_ids);

    /**-----------------------------------------------------
     * Function that compute the statistics value
     *-----------------------------------------------------*/
    /**
     * Returns the average load of a TimeSeries
     * @return the average load of a TimeSeries
     * @param ts the given TimeSeries
     */
    double get_average_load(const TimeSeries& ts)
    {
      return ts.get_latest_average(_load_evaluation_interval_num); }

    /**
     * Returns the maximum load of a TimeSeries
     * @return the maximum load of a TimeSeries
     * @param ts the given TimeSeries
     */
    double get_max_load(const TimeSeries& ts)
    {
      return ts.get_maximum(); }

    /**
     * Returns the divergent load of a TimeSeries
     * @return the divergent load of a TimeSeries
     * @param ts the given TimeSeries
     */
    double get_divergent_load(const TimeSeries& ts)
    {
      return
        ts.get_latest_average(_load_evaluation_interval_num) +
        LMParams::distribution_param.divergent_coefficient *
        ts.get_standard_deviation(); }

    /**
     * Returns the average Bandwith of a TimeSeries
     * @return the average Bandwith of a TimeSeries
     * @param ts the given TimeSeries
     */
    double get_average_bandwidth(const TimeSeries& ts)
    {
      return ts.get_latest_average(_bandwidth_evaluation_interval_num); }

    /**
     * Returns the average load of a LMBox
     * @return the average load of a LMBox
     * @param box the given LMBox
     */
    double get_box_average_load(const IdType& box_id)
    {
      return get_average_load(get_box_load_time_series(box_id)); }

    /**
     * Returns the average load of a set of LMBoxes
     * @return the average load of a set of LMBoxes
     * @param box the given LMBox
     */
    double get_boxes_average_load(const IdSet& box_ids)
    {
      return get_average_load(get_boxes_load_time_series(box_ids)); }

    /**
     * Returns the maximum load of a set of LMBoxes
     * @return the maximum load of a set of LMBoxes
     * @param box the given LMBox
     */
    double get_boxes_max_load(const IdSet& box_ids)
    {
      return get_max_load(get_boxes_load_time_series(box_ids)); }

    /**
     * Returns the average load of a Node
     * @return the average load of a Node
     * @param node the given Node
     */
    double get_node_average_load(const IdType& node_id)
    {
      return get_average_load(get_node_load_time_series(node_id)); }

    /**
     * Returns the average load of a Node
     * @return the average load of a Node
     * @param node the given Node
     */
    double get_node_divergent_load(const IdType& node_id)
    {
      return get_divergent_load(get_node_load_time_series(node_id)); }

    /**
     * Returns the average bandwidth of an LMArc
     * @return the average bandwidth of an LMArc
     * @param arc the given LMArc
     */
    double get_arc_average_bandwidth(const IdType& arc_id)
    {
      return get_average_bandwidth(get_arc_bandwidth_time_series(arc_id)); }


    /**
     * Returns the total load of the system
     */
    double get_system_total_load() ;

    /**
     * Return the average load per node
     */
    double get_average_node_load() ;

    /**
     * Return the average load per node
     */
    double get_current_average_node_load()
    {
      return get_system_total_load_time_series().get_last_sample(); }

    /**
     * TODO
     * Returns the average bandwidth of a LMSuperEdge
     * @return the average bandwidth of a LMSuperEdge
     * @param super_edge the given LMSuperEdge
     */
    void get_lbnodes_bandwidth_time_series(TimeSeries& ts)
    {
     cerr << "TODO: getLBNodesBandwidthTimeSeries" << endl;
   //   getSuperEdgesTotalBandwidthTimeSeries(ts);
    //  ts += _io_arcs_bandwidth_time_series;
    }

    /**
     * TODO
     * Returns the average bandwidth of a LMSuperEdge
     * @return the average bandwidth of a LMSuperEdge
     * @param super_edge the given LMSuperEdge
     */
    double get_lbnodes_average_bandwith()
    {
     // return getSuperEdgesTotalAverageBandwidth() + getIOArcsAverageBandwidth();
     cerr << "TODO: getLBNodesAverageBandwith" << endl;
     return 0;
    }

    /**
     * Returns the average bandwidth of a LMSuperEdge
     * @return the average bandwidth of a LMSuperEdge
     * @param super_edge the given LMSuperEdge
     */
    double get_lbnodes_divergent_bandwith()
    {
      TimeSeries ts;
      get_lbnodes_bandwidth_time_series(ts);
      return get_divergent_bandwidth(ts); }

    double get_divergent_bandwidth(TimeSeries ts)
    {
      return get_average_bandwidth(ts) +
        LMParams::distribution_param.divergent_coefficient *
        ts.get_standard_deviation(); }

    /**-----------------------------------------------------
     * Function that used to update the statistics
     *-----------------------------------------------------*/
    /**
     * Update Statistics based on the number of new samples that has been updated
     */
    void update_statistics(int number_of_new_samples);

    /**
     * Update data box cost statistics
     * @param box_id ID of the box
     * @param value value of the box cost
     */
    void update_box_cost(const IdType& box_id, double value)
    {
      _statistics_pool.get_statistics(BOX_COST, box_id).slide_sample(value); }

    /**
     * Update arc rate statistics
     * @param arc_id ID of the arc
     * @param value value of the arc rate
     */
    void update_arc_rate(const IdType& arc_id, double value)
    {
      _statistics_pool.get_statistics(STREAM_RATE, arc_id).slide_sample(value); }

    /**
     * Update box selectivity statistics
     * @param arc_id ID of the box
     * @param port output port of the box
     * @param value value of the arc rate
     */
    void update_box_selectivity(const IdType& box_port, double value)
    {
      _statistics_pool.get_statistics(SELECTIVITY, box_port).slide_sample(value); }

    /**
     * Update the load statistics of the nodes when
     * a set of LMBoxes is moved from one node to the other
     */
    void clear_node_load_statistics(const IdType& node_id)
    {
      _statistics_pool[NODE_LOAD].erase(node_id); }

    /**
     * Update the load statistics of the nodes when
     * a set of LMBoxes is moved from one node to the other
     */
    void clear_all_node_load_statistics()
    {
      _statistics_pool.erase(NODE_LOAD); }

    /**
     * Update the load statistics of the nodes when
     * a set of LMBoxes is moved from one node to the other
     */
    void update_node_load_statistics_for_adding_boxes(
        const IdType& receiver_id,
        const IdSet& box_ids);

    /**
     * Update the load statistics of the nodes when
     * a set of LMBoxes is moved from one node to the other
     */
    void update_node_load_statistics_for_box_migration(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids);


    /**
     * Print all the statistics managed
     */
    friend ostream& operator<<(ostream& os, const LMStatisticsManager& statistics_manager);

    /**
     * print statistics of a given name
     */
    void output_statistics(
            ostream& os, 
            const string& statistics_name, 
            bool output_average_statistics = true);

    /**
     * print average statistics
     */
    void output_average_statistics(ostream& os);

    /**
     * print statistics of to files
     */
    void print_statistics_to_files();

private:
    /**
     * Delete the statistics if the box or arc do not exist any more
     */
    void delete_obsolete_statistics();


    /**
     * Update the Ideal stream rate of the arcs
     */
    void update_ideal_stream_rate_and_box_load_statistics(int number_of_new_samples);

    /**
     * Update Node Load Statistics
     */
    void update_node_load_statistics();

    /**
     * Returns the stream rate of a stream with given ID
     * @param id Stream ID
     * @return the stream rate of a stream with given ID
     */
    double get_current_stream_rate(const IdType& id)
    {
      return _statistics_pool.get_statistics(STREAM_RATE, id).get_last_sample(); }

private:
    /**
     * Creates the LMStatisticsManager
     */
    LMStatisticsManager();

    /**
     * The pointer to the LMStatisticsManager
     */
    static LMStatisticsManager* _singleton;

    //------------------------------------------------
    // Statistics evaluation parameters
    //------------------------------------------------
    /**
     * Load evealution interval
     */
    int _load_evaluation_interval_num;

    /**
     * Bandwidth evealution interval
     */
    int _bandwidth_evaluation_interval_num;

    //------------------------------------------------
    // Statistics constainers
    //------------------------------------------------
    /**
     * Statistics pool
     */
    LMStatisticsPool _statistics_pool;

};



BOREALIS_NAMESPACE_END
#endif


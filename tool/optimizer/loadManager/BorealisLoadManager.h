#ifndef BorealisLoadManager_H
#define BorealisLoadManager_H

#include "RegionalCatalog.h"
#include <rpc_QueryProcessor.h> // RPC<Statistics>
#include <NMSTL/ntime> // for Time

#include "LMCommon.h"
#include "LMLoadManager.h"
#include "LMNodeManager.h"
#include "LMStatisticsManager.h"
#include "BorealisCatalogReader.h"
#include "BorealisStatisticsReader.h"


BOREALIS_NAMESPACE_BEGIN

/**
 * The BorealisLoadManager class implements the component that controls the process
 * of load managerment in Borealis
 *
 * @author Ying Xing (yx@cs.brown.edu)
 * @version 2.0 02/01/2004
 */
class BorealisLoadManager : public LMLoadManager
{

  // Method(s)
public:
    /**
     * Creates the BorealisLoadManager
     */
    BorealisLoadManager(
        double update_interval,
        RegionalCatalog& catalog,
        map<string, Statistics>& statistics);

    /**
     * Redistribute load
     */
    virtual void redistribute_load();

    /**
     * Returns the average load
     */
    double get_system_average_node_load()
    {
        return LMStatisticsManager::get_singleton()->get_average_node_load();
    }

    /**
     * Add a node to be ignored in load distribution
     */
    void add_ignore_node(const IdType& node_id)
    {
        LMNodeManager::get_singleton()->add_ignore_node(node_id);
    }

    /**
     * Read command from standard input
     */
    void start_input_reading();

    /**
     * Virtual destructor
     */
    virtual ~BorealisLoadManager(){}

protected:
    /**
     * Update nodes and querynetwork
     */
    virtual bool update_topology();

    /**
     * Update Statistics
     */
    virtual bool update_statistics();


    /**
     * Returns the current time
     */
    virtual double get_current_time()
    {
        return (double)(Time::now().to_usecs()) / 1000000.0; }

    /**
     * Mover operators
     * @params sender_id ID of the sender node
     * @params receiver_id ID of the receiver node
     * @params box_ids ID of the boxes to be moved
     */
    virtual void move_boxes_between_nodes(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids);


private:
    /*
     * The last time that some box are moved
     */
    double _last_box_migration_time;

    /*
     * The handle of the reginal catalog
     */
    RegionalCatalog& _catalog;

    /*
     * The handle of the statistics
     */
    map<string, Statistics>& _statistics;

    void display_info();
    bool _accept_input_command;
    static void accept_input_command(BorealisLoadManager* lm);
    static void stop_all_display();

};

BOREALIS_NAMESPACE_END
#endif


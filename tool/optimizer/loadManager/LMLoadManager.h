#ifndef LMLoadManager_H
#define LMLoadManager_H

#include "LMCommon.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The LMLoadManager class implements the component that controls the process
 * of load managerment
 *
 * @author Ying Xing (yx@cs.brown.edu)
 * @version 2.0 02/01/2004
 */
class LMLoadManager
{

  // Method(s)
public:
    /**
     * Creates the LMLoadManager
     */
    LMLoadManager(){};

    /**
     * Virtual destructor
     */
    virtual ~LMLoadManager(){}

    /**
     * Update topology and statitics
     */
    virtual void update();

    /**
     * Redistribute load
     */
    virtual void redistribute_load();

protected:
    /**
     * Update nodes and querynetwork
     */
    virtual bool update_topology() = 0;

    /**
     * Update Statistics
     */
    virtual bool update_statistics() = 0;

    /**
     * Returns the current time
     */
    virtual double get_current_time() = 0;

    /**
     * Mover operators
     * @params lmnodes_snapshot the LMNodeVect that stores the locations
     * of the boxes before migration
     */
    void move_boxes();

    /**
     * Mover operators
     * @params sender_id ID of the sender node
     * @params receiver_id ID of the receiver node
     * @params box_ids ID of the boxes to be moved
     */
    virtual void move_boxes_between_nodes(
        const IdType& sender_id,
        const IdType& receiver_id,
        const IdSet& box_ids) = 0;

protected:
    /*
     * The map from box ID to its host node ID before redistribution
     */
    map<IdType, IdType> _box_node_map_before_redistribition;


};

BOREALIS_NAMESPACE_END
#endif


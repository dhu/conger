#ifndef HA_SEGMENT_H
#define HA_SEGMENT_H

#include "HAUnit.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * An HASegment Summary.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 09/03/2005
 */
class HASegment
{
  protected:
  
    /**
     * The backup server.
     */
    MedusaID _backup_server;
  
    /**
     * The HAUnits Associated.
     */
    HAUnits _ha_units;     
    
    Time _last_push_time;
    
    /**
     * The SuperBoxes on the backup segment.
     */
//    TIME _last_expected_recovery_time;

    /**
     * The sum of the recovery times so far.
     */
//    TIME _sum_recovery_times;

    /**
     * The statistics on the recovery time.
     */
//    SlidingWindowStatistics _recovery_time_statistics;
    
    /**
     * The Recovery Time of this SuperBox.
     */
    RecoveryTime _recovery_time;
    
    
  public:
  
    /**
     * Constructs a HASegment.
     */
    HASegment(MedusaID backup_server);

    void add(ptr<HAUnit> ha_unit);

    MedusaID get_backup_server();

    void set_last_push_time() {_last_push_time = Time::now(); };

    Time get_last_push_time() {return _last_push_time; };

    void update_statistics();

    bool has_boxes();

    RecoveryTime& get_recovery_time();

    Time get_expected_recovery_time(ptr<HAUnit> ha_unit, Time target_time);

    Time get_expected_staleness(ptr<HAUnit> ha_unit, Time target_time);

    Time get_last_effective_capturing_start_time();

    HAUnits& get_ha_units();

//    TIME get_expected_recovery_time(SuperBox* box, TIME end_time);

};

typedef map<MedusaID, ptr<HASegment> > HASegments;

BOREALIS_NAMESPACE_END

#endif

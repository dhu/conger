#ifndef HA_UNIT_H
#define HA_UNIT_H

#include <sstream>
#include "common.h"
#include "Catalog.h"
#include "BoxPackage.h"
#include "MedusaID.h"
#include "RecoveryTime.h"
#include "SlidingWindowStatistics.h"

BOREALIS_NAMESPACE_BEGIN

NMSTL_TO_STRING(State);

class QBox;

typedef map<Name, ptr<QBox> > Name2QBox;

class HAUnit
{

  public:
  
    /**
     * The name of this HAUnit.
     */
    string _name;
    
    /**
     * A map from box names to boxes.
     */
    Name2QBox _boxes;
  
    /**
     * The backup server dedicated for this HAUnit.
     */
    MedusaID _backup_server;
    
    /**
     * The start time of this HAUnit.
     */
    Time _start_time;
    
    /**
     * Is ready for checkpoint.
     */
    bool _ready_for_checkpoint;
    
    /**
     * Whether to do the delta checkpoiniting.
     */
    bool _delta_checkpointing;
    
    /**
     * The last time when the primary began checkpointing this HAUnit.
     */
    Time _last_capturing_start_time;

    /**
     * The last time when the primary began the checkpoint for this HAUnit that has left the current backup image.
     */
    Time _last_effective_capturing_start_time;

    /**
     * The last time when the primary began checkpointing this HAUnit.
     */
    Time _last_capturing_end_time;

    /**
     * The last time when the backup began updating this HAUnit.
     */
    Time _last_update_start_time;
    
    /**
     * The last time when the backup completed updating this HAUnit.
     */
    Time _last_update_end_time;

    /**
     * The accumulated capture cost.
     */
    Time _accumulated_capture_cost;
    
    /**
     * The accumulated paste cost.
     */
    Time _accumulated_paste_cost;
    
    /**
     * The most recent capture cost.
     */
    Time _last_capture_cost;
    
    /**
     * The most recent paste cost.
     */
    Time _last_paste_cost;
    
    /**
     * The accumulated capture cost.
     */
    Time _accumulated_expected_capture_cost;
    
    /**
     * The accumulated expected paste cost.
     */
    Time _accumulated_expected_paste_cost;
    
    /**
     * The Recovery Time of this HAUnit.
     */
    RecoveryTime _recovery_time;
    
    bool _passive;
    
    /**
     * The processing times.
     */
    SlidingWindowStatistics _processing_times;
    
    Time _last_processing_time;
    
    Mutex _lock; // Thread safety
    
  public:
  
    /**
     * Constructs an HAUnit based on the default setting.
     */
    HAUnit();
    
    /**
     * Constructs an HAUnit based on the specified arguments.
     */
    HAUnit(string name, MedusaID backup_server);
    
    /**
     * Destroys an HAUnit.
     */
    ~HAUnit();
    
    /**
     * Set this HAUnit as a passive entity.
     */
    void set_passive();
    
    /**
     * Resets the statistics on this HAUnit.
     */
    void reset_statistics();
    
    /**
     * Returns the name of this HAUnit.
     */
    string get_name();
    
    /**
     * Returns the id of the dediciated backup server.
     */
    MedusaID get_backup_server();
    
    /**
     * Returns the recovery time.
     */
    RecoveryTime& get_recovery_time();
    
    /**
     * Adds the specified box to this HAUnit.
     */
    void add(ptr<QBox> box);
    
    /**
     * Adds to this HAUnit the boxes of the specified HAUnit.
     */
    void add(ptr<HAUnit> ha_unit);
    
    /**
     * Removes the specified box from this HAUnit.
     */
    void remove(ptr<QBox> box);
    
    /**
     * Retuns the number of boxes of this HAUnit.
     */
    unsigned int get_number_of_boxes();

    /**
     * Retuns the string representation of this HAUnit.
     */
    string as_string();
    
    /**
     * Determines if this HAUnit is ready for checkpoint.
     */
    bool is_ready_for_checkpoint();
    
    /**
     * Returns the time when the last checkpoint for this HA Unit began.
     */
    Time get_last_capturing_start_time();
  
    /**
     * Begins capturing this HAUnit.
     */
    void start_capturing();

    /**
     * Ends capturing this HAUnit.
     */
    void end_capturing();

    /**
     * Begins updating this HAUnit.
     */
    void start_update();

    /**
     * Completes updating this HAUnit.
     */
    void end_update();

    /**
     * Completes checkpointing this HAUnit.
     */
    void end_checkpoint();

    /**
     * Returns the current expected recovery time of this HAUnit.
     */
    Time get_expected_recovery_time(Time target_time, Time capturing_start_time = Time::secs(-1));

    void add_processing_time(Time processing_time);

    void set_processing_times(SlidingWindowStatistics& processing_times);

    ptr<SlidingWindowStatistics> get_processing_times();

    /**
     * Returns the expected capture cost for this HAUnit. 
     */
    Time get_expected_capture_cost();
    
    /**
     * Returns the expected paste cost for this HAUnit.
     */
    Time get_expected_paste_cost();
    
    Time get_last_capture_cost();
    
    Time get_last_paste_cost();
    
    void add_capture_cost(Time capture_cost);
    
    void add_paste_cost(Time past_cost);
    
    void add_expected_capture_cost(Time cost);
    
    void add_expected_paste_cost(Time cost);
    
    double get_processing_load();
    
    double get_capture_load();
    
    double get_paste_load();
    
    double get_expected_capture_load();
    
    double get_expected_paste_load();
    
    void put_recovery_time(Time time = Time::now());
    
    void update_statistics();
    
    bool is_delta_checkpointing();
    
    void activate_delta_checkpointing();
    
    void deactivate_delta_checkpointing();
    
    Name2QBox& get_boxes();
    
    double get_staleness();

    Time get_expected_staleness(Time target_time, Time capturing_start_time = Time::secs(-1));

    /**
     * Updates the statistics of this HAUnit based on the HAUnit specified.
     */
    void update_statistics(ptr<HAUnit> ha_unit);

    void set(ptr<HAUnit> ha_unit);

    NMSTL_SIMPLY_SERIALIZABLE(HAUnit, << _name << _delta_checkpointing 
            << _last_capturing_start_time << _last_capturing_end_time << _last_update_start_time << _last_update_end_time
            << _last_effective_capturing_start_time
            << _last_capture_cost << _last_paste_cost
            << _accumulated_paste_cost << _start_time << _processing_times << _recovery_time);

};

NMSTL_TO_STRING(HAUnit);

typedef map<string, ptr<HAUnit> > HAUnits;

BOREALIS_NAMESPACE_END

#endif

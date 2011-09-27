#ifndef HA_H
#define HA_H

#include "BasicComponent.h"
#include "CheckpointMessage.h"
#include "QBox.h"
#include "HASegment.h"

BOREALIS_NAMESPACE_BEGIN


/**
 * The HA component monitors statically assigned primary
 * HA components at remote nodes.
 * If a primary fails, the HA component takes over the
 * failed query: (1) It does a lookup in the catalog to
 * figure out what was running at the primary, (2) it instantiates
 * all queries and stream subscriptions locally, and (3) it updates
 * the catalog
 */
class HA : public SchedulerEventHandler
{
//
 public:

    static bool random_backup_distribution;

    static bool ready;

    static Time last_end_time;

    static void set_HA_method(HAMethod HA_method);

    static HAMethod get_HA_method();

    static void set_checkpoint_method(CheckpointMethod checkpoint_method);

    static void set_checkpoint_interval(int checkpoint_interval);

    static void set_decremental_checkpoint_interval();

    static void set_checkpoint_slack(int checkpoint_slack);

    static double get_checkpoint_interval();

    static int get_checkpoint_slack();

    static void add_ha_partner(MedusaID ha_partner);

    Mutex _lock; // Thread safety

    HA(string id, QueryProcessor& qp);

    ~HA() {}

public:

    /**
     * Determinses the right HA memebership of the box specified.
     */
    void form_ha_unit(ptr<QBox> box, bool primary);

    /**
     * Enqueues the checkpoint message in the queue of pending checkpoint messages (from primary to backup).
     */
    void enqueue(ptr<CheckpointMessage> cm);

    /**
     * Completes the checkpoint (from backup to primary).
     */
    void end_checkpoint(ptr<CheckpointMessage> cm);

    /**
     * Is invoked when the checkpoint message has safely arrived at the backup (from backup to primary)
     */
    void checkpoint_message_enqueued(MedusaID backup);

    double get_backup_staleness();

    void update_processing_times(MedusaID primary, vector<string> ha_units, vector<ptr<SlidingWindowStatistics> > processing_times);


protected:

    /**
     * Executes the QueryProcessor's handle_idle_time() method.
     */
    virtual void handle_idle_time();

    /**
     * Updates the related statistics.
     */
    virtual void update_statistics(QBox* box, Time time_span);

    /**
     * Sets up HA-related things.
     */
    void setup_ha();

    /**
     * Ends the current epoch.
     */
    void end_epoch();

    /**
     * Forms a checkpoint message.
     */
    ptr<CheckpointMessage> form_checkpoint_message(ptr<HAUnit> ha_unit);

    /**
     * Forms a checkpoint message.
     */
    ptr<CheckpointMessage> form_checkpoint_message();

    void instantiate_boxes(ptr<CheckpointMessage> cm);    

    void consume(ptr<CheckpointMessage> cm);

    /**
     * Re-sets up the remanining checkpoints.
     */
    void setup_remaining_checkpoints();

    /**
     * Finds the best primary-side HA unit.
     */
    ptr<HAUnit> get_best_ha_unit_primary_round_robin(Time& expected_capture_cost, Time& expected_recovery_time);

    ptr<HAUnit> get_best_ha_unit_primary_min_max(Time& expected_capture_cost, Time& expected_recovery_time, bool try_best);

    ptr<HAUnit> get_oldest_checkpointable();

    string get_checkpointables(ptr<HASegment> ha_segment);

    /**
     * Finds the best HA unit.
     */
    ptr<HAUnit> get_best_ha_unit();

    ptr<HAUnit> get_best_ha_unit_round_robin();

    ptr<HAUnit> get_best_ha_unit_min_max();

    int find_best_checkpoint_message(Time& max_recovery_time);

    Time get_expected_recovery_time_primary(ptr<HAUnit> ha_unit, Time target_time);

    /**
     * Updates the capture load.
     */
    void add_expected_capture_cost(ptr<HAUnit> ha_unit);

    /**
     * Updates the paste load.
     */
    void add_expected_paste_cost(ptr<HAUnit> ha_unit);

    void add_xml_strings(string box_name, ptr<CheckpointMessage> cm);

    void add(string box_name, ptr<CheckpointMessage> cm, bool delta_checkpointing);

    /**
     * Does the remote checkpoint.
     */
    void do_remote_checkpoint();

    /**
     * Creates an HAUnit.
     * @param name the name of the HAUnit.
     */
    ptr<HAUnit> create_ha_unit(string name);

    /**
     * Moves boxes from HAUnit source to HAUnit target and removes HAUnit source.
     */
    void merge(HAUnit* source, HAUnit* target);

    //void init_config();
    //void init_global(const DOMElement* ha_config);
    //void init_process_pairs(const DOMElement* ha_config);
    //void init_replicas(const DOMElement* ha_config);

    /**
     * Prints HA-related statistics to a text file.
     */
    void print_statistics_to_file();

    /**
     * Updates the statistics of this QueryProcessor.
     */
    void update_statistics();

    void schedule_ha_task();

    void do_ha_task();

    void schedule_remote_checkpoint();

    void push_processing_times();

 private:

    /**
     * The HA method.
     */
    static HAMethod _HA_method;

    /**
     * The checkpoint method.
     */
    static CheckpointMethod _checkpoint_method;

    /**
     * The checkpoint interval.
     */
    static double _checkpoint_interval;

    static bool _decremental_checkpoint_interval;
    
    /**
     * The checkpoint slack.
     */
    static int _checkpoint_slack;

    /**
     * The HA partners.
     */
    static vector<MedusaID> _ha_partners;

    /**
     * The identifier.
     */
    string _id;

    /**
     * The query processor.
     */
    QueryProcessor& _qp;

    /**
     * The current epoch.
     */
    int _epoch;

    /**
     * The tentative epoch.
     */
    int _tentative_epoch;

    /**
     * The collection of HASegments (id -> ha_segment).
     */
    HASegments _primary_segments;   

    /**
     * The collection of HASegments (id -> ha_segment).
     */
    HASegments _backup_segments;   

    /**
     * The map between the ids and the primary HA units.
     */
    HAUnits _ha_units_primary;

    /**
     * The map between the ids and the backup HA units.
     */
    HAUnits _ha_units_backup;

    /**
     * The pending checkpoint messages.
     */
    vector<ptr<CheckpointMessage> > _pending_checkpoint_messages;

    /**
     * The number of checkpoints to do (per HAUnit) to close the current epoch.
     */
    map<string, int> _remaining_checkpoints;

    /**
     * The time when this QueryProcessor started.
     */
    Time _start_time;

    /**
     * The time for the next checkpoint.
     */
    Time _next_checkpoint_time;

    /**
     * The time when this QueryProcessor started the current checkpoint.
     */
    Time _current_checkpoint_start_time;

    /**
     * The time when this QueryProcessor started the last checkpoint.
     */
    Time _last_checkpoint_start_time;

    /**
     * The output stream for statistics.
     */
    ptr<ofstream> _ostream_statistics;

    /**
     * The output stream for recovery_time.
     */
    ptr<ofstream> _ostream_recovery_time;

    /**
     * The recovery_time.
     */
    RecoveryTime _recovery_time;

    static double _push_interval;

    Time _last_push_time;

};

BOREALIS_NAMESPACE_END
#endif                          // HA_H


/*
    AsyncRPC<void> init_ha(int recovery_method, bool is_primary, MedusaID secondary, string partner, vector<MedusaID> replicas);

    AsyncRPC<void> set_ha(int recovery_method, bool is_primary, MedusaID secondary);

    RPC<void> set_recovery_method(int method);
    RPC<void> set_primary_status(bool status);
    RPC<void> set_secondary(MedusaID secondary);
    RPC<void> set_replicas(vector<MedusaID> replicas);

*/

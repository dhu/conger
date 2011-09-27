#ifndef BOREALIS_CONSISTENCYMNGR_H
#define BOREALIS_CONSISTENCYMNGR_H

#include "common.h"
#include "Diagram.h"
#include "StreamEvent.h"
#include "Timestamp.h"
#include "Tuple.h"
#include "ConsistencyState.h"

#include <NMSTL/tqueuerpc>
#include <NMSTL/tcprpc>
#include <NMSTL/xmlrpc>
#include <fstream>

BOREALIS_NAMESPACE_BEGIN;

class QueryProcessor;


// Type of microbenchmark that we are running
class MicroBenchmarkType
{

 public:

    enum Type
    {
        // Micro benchmarking the operator (one item at the time)
        NONE = 0,     // SJoin without anything else (like an ordinary join)
        TUPLE_TYPES,  // SJoin that looks at the message types before processing them
        UNDO_LOG,     // SJoin that looks keeps an undo log of variable size
        CHECKPOINT,   // SJoin interrupted by checkpoints

        // Micro benchmarking the sunion/soutput operators
        SUNION,       // Just add an sunion in front of an sjoin
        SUNION_BOUNDARY,    // Just sunion and boundary
        SOUTPUT,      // Just add an soutput at the end

        BLOCK,  // Never process any tentative bucket

        // Micro benchmarking the approach as a whole but don't move subscriptions
        ALL_TOGETHER_UNDO_REDO_NO_MOVE,
        ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE,

        // Micro benchmarking the approach as a whole
        //ALL_TOGETHER_UNDO_REDO,
        //ALL_TOGETHER_CHECKPOINT_REDO,

        // Finally, the complete adaptive algorithm
        //ADAPTIVE


    };

    bool do_keep_undo_log()
    { return ((_type == UNDO_LOG)
              || (_type == ALL_TOGETHER_UNDO_REDO_NO_MOVE));
              //|| (_type == ALL_TOGETHER_UNDO_REDO)
              //|| (_type == ADAPTIVE));
    }

    bool do_serialize()
    { return ((_type == SUNION) || (_type == SUNION_BOUNDARY) || (_type == BLOCK)
              || (_type == ALL_TOGETHER_UNDO_REDO_NO_MOVE) || (_type == ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE));
              //|| (_type == ALL_TOGETHER_UNDO_REDO) || (_type == ALL_TOGETHER_CHECKPOINT_REDO)
              //|| (_type == ADAPTIVE));
    }

    bool do_boundary()
    { return ((_type == SUNION_BOUNDARY) || (_type == BLOCK)
              || (_type == ALL_TOGETHER_UNDO_REDO_NO_MOVE) || (_type == ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE));
              //|| (_type == ALL_TOGETHER_UNDO_REDO) || (_type == ALL_TOGETHER_CHECKPOINT_REDO)
              //|| (_type == ADAPTIVE));

    }

    bool do_checkpoint()
    { return ((_type == CHECKPOINT)
              || (_type == ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE)
              //|| (_type == ALL_TOGETHER_CHECKPOINT_REDO)
              //|| (_type == ADAPTIVE)
              );
    }

    bool do_block()
    { return (_type == BLOCK);
    }

    //bool do_force_move_subscriptions()
    //{ return ( (_type == ALL_TOGETHER_UNDO_REDO) || (_type == ALL_TOGETHER_CHECKPOINT_REDO));
    //}

    bool do_undo_reconciliation()
    { return ((_type == ALL_TOGETHER_UNDO_REDO_NO_MOVE)); //|| (_type == ALL_TOGETHER_UNDO_REDO));
    }

    // bool do_adaptive()
    //{ return ( _type == ADAPTIVE);
    // }

    MicroBenchmarkType(Type t = ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE)
        : _type(t)
    {
    }

    MicroBenchmarkType(string type)
    {
       if ( type == "none" || type == "NONE" )
           _type = NONE;

       else if ( type == "tuple_types" || type == "TUPLE_TYPES" )
           _type = TUPLE_TYPES;

       else if ( type == "undo_log" || type == "UNDO_LOG" )
           _type = UNDO_LOG;

       else if ( type == "checkpoint" || type == "CHECKPOINT" )
           _type = CHECKPOINT;

       else if ( type == "sunion" || type == "SUNION" )
           _type = SUNION;

       else if ( type == "sunion_boundary" || type == "SUNION_BOUNDARY" )
           _type = SUNION_BOUNDARY;

       else if ( type == "soutput" || type == "SOUTPUT" )
           _type = SOUTPUT;

       else if ( type == "block" || type == "BLOCK" )
           _type = BLOCK;

       //else if ( type == "all_together_checkpoint_redo" || type == "ALL_TOGETHER_CHECKPOINT_REDO" )
       //  _type = ALL_TOGETHER_CHECKPOINT_REDO;

       else if ( type == "all_together_checkpoint_redo_no_move" || type == "ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE" )
           _type = ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE;

       //else if ( type == "all_together_undo_redo" || type == "ALL_TOGETHER_UNDO_REDO" )
       //  _type = ALL_TOGETHER_UNDO_REDO;

       else if ( type == "all_together_undo_redo_no_move" || type == "ALL_TOGETHER_UNDO_REDO_NO_MOVE" )
           _type = ALL_TOGETHER_UNDO_REDO_NO_MOVE;
       
       else _type = ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE;
       //else _type = ADAPTIVE;

   }

    string as_string( ) const;
    //{
      //  switch (_type)
        //{
        //case NONE:         return string("none");
        //case TUPLE_TYPES : return string("tuple_types");
        //case UNDO_LOG:     return string("undo_log");
        //case CHECKPOINT:   return string("checkpoint");
        //case SUNION:       return string("sunion");
        //case SUNION_BOUNDARY:           return string("sunion_boundary");
        //case SOUTPUT:      return string("soutput");
        //case BLOCK:        return string("block");
        //case ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE: return string("all_together_checkpoint_redo_no_move");
          //  //case ALL_TOGETHER_CHECKPOINT_REDO: return string("all_together_checkpoint_redo");
        //case ALL_TOGETHER_UNDO_REDO_NO_MOVE  : return string("all_together_undo_redo_no_move");
          //  //case ALL_TOGETHER_UNDO_REDO  : return string("all_together_undo_redo");
          //  //case ADAPTIVE :    return string("adaptive");
        //default:           return string("all_together_checkpoint_redo_no_move");
       // }
    //}

    Type _type;

};

NMSTL_TO_STRING(MicroBenchmarkType);





/**
 * The ConsistencyMngr listens to control streams coming from sunion
 * boxes. When sunion boxes indicate that they need to reconcile, the
 * ConsistencyMngr manages the state reconciliation of all the
 * boxes in the local query network.
 */
class ConsistencyMngr
{
 public:

    /**
     * Constructor
     * The ConsistencyMngr is a part of the query processor so it accesses
     * the query processor's state.
     */
    ConsistencyMngr(QueryProcessor& qp);

    /**
     * The destructor writes all stats to a log file
     */
    ~ConsistencyMngr();

    /**
     * Examines if a query has an sunion box with a control output stream
     * If so, then starts to monitor the output of the query
     */
    void check_for_control_stream(const CatalogBox* box);

    /**
     * Selects the best replica to use as upstream neighbor
     * If two replicas are equivalent, prefers to stay with the current one
     * @param current Current upstream neighbor and current state of the stream
     *                it is producing
     * @param background Stream with background corrections, if one exists
     * @param stream_replicas All replicas of the stream including current
     * @return The replica to use as upstream neighbor
     */
    pair<InetAddress,ConsistencyState> pick_best_input_replica(pair<InetAddress,ConsistencyState> current, 
                                                               pair<InetAddress,ConsistencyState> background, 
                                                               PerNodeStreamStates& stream_replicas);

    /**
     * Given algorithm for selecting the best replica to use as input, we have a second
     * algorithm to select a background stream of corrections when appropriate
     */
    InetAddress pick_corrections_input_replica(pair<InetAddress,ConsistencyState> current, 
                                               pair<InetAddress,ConsistencyState> background, 
                                               PerNodeStreamStates& stream_replicas);


    /**
     * Process request from a replica to become temporarily unavailable
     */
    bool authorize_reconciliation(MedusaID replica);

 private:

    /// Given a list of replicas, find a replica in each possible consistency state
    void find_different_replicas(PerNodeStreamStates& stream_replicas, 
                                 InetAddress& stable, 
                                 InetAddress& up_failure, 
                                 InetAddress& stabilization);

    /// Find the control stream of a box
    Name find_control_stream_name(const CatalogBox* box);

    /// Remember the first input stream of box that produces given output control stream
    void map_output_to_input_stream(const CatalogBox* box, Name output_stream_name);

    /// Subscribe to receive control messages from an sunion operator
    void monitor_control_stream(Name control_stream_name);

    /// Process a tuple on the control stream
    Status handle_control_tuples(ptr<StreamEvent> event);

    /// Process a RECONCILIATION_DONE eventw
    Status process_rec_done(ptr<StreamEvent> event);

    /// Send a control tuple with given type and timestamp
    void send_control_tuple(Name output_stream_name, TupleType type, Timestamp since);

    void start_checkpointing();

    /// Ask all boxes and queues to checkpoint their state
    void checkpoint();

    bool can_reconcile();
    void request_authorization();
    void request_authorization_response(RPC<bool> result);
    void launch_reconciliation();
    void finish_reconciliation();

    /// Update the state of this node and if necessary that of local streams
    void update_state(ConsistencyState new_state);

    /// Parent query processor
    QueryProcessor& _qp;

    /// Sibling replicas
    vector<MedusaID> _replicas;

    /**
     * For each sunion box, remember the name of the output stream for control message
     * Also remember the the corresponding name and schema size for the input stream
     * on which ConsistencyMngr sends control messages
     */
    typedef map< Name, pair<Name, int> > iocontrol;
    iocontrol _io_control;

    // The consistency state of the query processor
    ConsistencyState _state;

    // Time of last boundary should be less than 2*boundary interval;
    map<Name,Timestamp> _input_sunions_last_boundary;
    long _max_boundary_interval; // In msec, maximum delay between boundary tuples

    // List of sunions that have signaled upstream failure
    map<Name,Timestamp> _sunions_in_failure;

    // List of sunions that have signaled readiness for reconciliation
    map<Name,Timestamp> _sunions_ready_to_reconcile;

    // List of input sunions that have reconciled their states
    map<Name,Timestamp> _reconciled_inputs;


    // Set of all SOutputs in the query diagram
    set<Name> _soutput_set;

    // List of SOutputs that have produced a REC_DONE
    map<Name,Timestamp> _reconciled_outputs;

    /// Period between random requests to authorize reconciliation
    static const int REC_REQUEST_INTERVAL = 500; // Default reconciliation request interval
    int _rec_request_interval;

    // Indicates if a reconciliation is currently in progress
    bool _reconciliation_in_progress;
    
    // Did we request authorization to reconcile
    bool _pending_reconcile_authorization;
    // Did we authorize a partner while requesting authorization ourselves
    bool _authorized_partners_while_awaiting_response;

    // Below is information about checkpointing
    static const int CHECKPOINT_INTERVAL = 1000; // Default checkpointing interval
    int _checkpoint_interval;                    // Actual checkpointing interval
    bool _checkpointing_started;
    bool _suspend_checkpoints;


    // All the stuff below is used to evaluate the approach
    MicroBenchmarkType _benchmark;

};

BOREALIS_NAMESPACE_END;

#endif

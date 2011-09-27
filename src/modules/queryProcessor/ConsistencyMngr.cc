#include "ConsistencyMngr.h"
#include "QueryProcessor.h"

BOREALIS_NAMESPACE_BEGIN

string 
MicroBenchmarkType::as_string( ) const
{
    switch (_type)
    {
    case NONE:         return string("none");
    case TUPLE_TYPES : return string("tuple_types");
    case UNDO_LOG:     return string("undo_log");
    case CHECKPOINT:   return string("checkpoint");
    case SUNION:       return string("sunion");
    case SUNION_BOUNDARY:           return string("sunion_boundary");
    case SOUTPUT:      return string("soutput");
    case BLOCK:        return string("block");
    case ALL_TOGETHER_CHECKPOINT_REDO_NO_MOVE: return string("all_together_checkpoint_redo_no_move");
    //case ALL_TOGETHER_CHECKPOINT_REDO: return string("all_together_checkpoint_redo");
    case ALL_TOGETHER_UNDO_REDO_NO_MOVE  : return string("all_together_undo_redo_no_move");
    //case ALL_TOGETHER_UNDO_REDO  : return string("all_together_undo_redo");
    //case ADAPTIVE :    return string("adaptive");
    default:           return string("all_together_checkpoint_redo_no_move");
    
    }

}


// --------------------------------------------------
ConsistencyMngr::ConsistencyMngr(QueryProcessor& qp)
    : _qp(qp),
      _state(ConsistencyState::STABLE),
      _rec_request_interval(REC_REQUEST_INTERVAL),
      _reconciliation_in_progress(false),
      _checkpoint_interval(CHECKPOINT_INTERVAL),
      _checkpointing_started(false),
      _suspend_checkpoints(false)
{
}

// --------------------------------------------------
ConsistencyMngr::~ConsistencyMngr()
{
}

// --------------------------------------------------
pair<InetAddress,ConsistencyState>
ConsistencyMngr::pick_best_input_replica(pair<InetAddress,ConsistencyState> current, 
                                         pair<InetAddress,ConsistencyState> background, 
                                         PerNodeStreamStates& stream_replicas)
{
    InetAddress stable, up_failure, stabilization;
    find_different_replicas(stream_replicas,stable,up_failure,stabilization);

    // Always prefer a stable input
    if ( current.second == ConsistencyState::STABLE )
    { return current;
    }

    // Remember, there may be NO background corrections stream
    if ( (background.first) && (background.second == ConsistencyState::STABLE) )
    { return background;
    }

    // Try to find an alternate STABLE replica
    if ( stable )
    { return pair<InetAddress,ConsistencyState>(stable,ConsistencyState::STABLE);
    }

    // No stable replica, then pick one in upstream failure
    if ( current.second == ConsistencyState::UP_FAILURE )
    { return current;
    }
    // Try to find an alternate UP_FAILURE replica
    if ( up_failure )
    { return pair<InetAddress,ConsistencyState>(up_failure,ConsistencyState::UP_FAILURE);
    }

    // Next best thing is a replica in STABILIZATION. If we had a corrections stream 
    // then we should stick with that stream
    if ( background.first )
    { return background;
    }

    if ( current.second == ConsistencyState::STABILIZATION )
    { return current;
    }

    // Try to find any alternate STABILIZATION replica
    if ( stabilization )
    { return pair<InetAddress,ConsistencyState>(stabilization,ConsistencyState::STABILIZATION);
    }

    // If all replica are in total failure, then do nothing
    return current;
    
}

// --------------------------------------------------
InetAddress
ConsistencyMngr::pick_corrections_input_replica(pair<InetAddress,ConsistencyState> current, 
                                                pair<InetAddress,ConsistencyState> background, 
                                                PerNodeStreamStates& stream_replicas)
{

    // Because of above algo for selecting the main input,
    // We only use background input when main input is in UP_FAILURE
    if ( current.second == ConsistencyState::UP_FAILURE )
    {

        // Prefer to stick with the same stream of background corrections
        if ( background.first && ( background.second == ConsistencyState::STABILIZATION )  )
        { return background.first;
        }

        InetAddress stable, up_failure, stabilization;
        find_different_replicas(stream_replicas,stable,up_failure,stabilization);

        // Try to find a replica that is producing corrections
        if ( stabilization )
        { return stabilization;
        }

    }

    // In all other cases, we don't want any background corrections
    return InetAddress();
    
}



// --------------------------------------------------
void ConsistencyMngr::find_different_replicas(PerNodeStreamStates& stream_replicas, 
                                              InetAddress& stable, 
                                              InetAddress& up_failure, 
                                              InetAddress& stabilization)
{

    PerNodeStreamStates::iterator i;
    for ( i = stream_replicas.begin(); i != stream_replicas.end(); ++i )
    {
        InetAddress replica = i->first;
        ConsistencyState stream_state = i->second;
        if ( stream_state == ConsistencyState::STABLE )
        { stable = replica;
        }
        else if ( stream_state == ConsistencyState::UP_FAILURE )
        { up_failure = replica;
        }
        else if ( stream_state == ConsistencyState::STABILIZATION )
        { stabilization = replica;
        }
    }

}

// --------------------------------------------------
bool 
ConsistencyMngr::authorize_reconciliation(MedusaID replica)
{
    //INFO << " Checking condition state is stabilization:  " <<  ( _state == ConsistencyState::STABILIZATION );
    //INFO << " Checking condition pending reconcile autho: " <<  ( _pending_reconcile_authorization );
    //INFO << " Checking condition replica has greater id:  " <<  ( replica < _qp._id);

    // Refuse requestn when node is already in stabilization
    // of it has a pending request and the ID is lower than the replica ID
    if ( ( _state == ConsistencyState::STABILIZATION ) ||        
         ( ( _pending_reconcile_authorization ) && ( replica < _qp._id ) ) )
    {
        INFO << "Rejecting request to authorize reconciliation at time " << Timestamp::now();
        return false;
    }
    
    INFO << "Accepting request to authorize reconciliation at time " << Timestamp::now();
    if ( _pending_reconcile_authorization )
    { _authorized_partners_while_awaiting_response = true;
    }
    return true;
}


// --------------------------------------------------
void
ConsistencyMngr::request_authorization()
{

    if ( _pending_reconcile_authorization )
    {
        DEBUG << "Need to request authorization to reconcile";

        vector<string> replicas = *(_qp.get_replica_set());
        if ( replicas.size() == 0 )
        { 
            INFO << "No replicas";
            RPC<bool> fake_result(true);
            request_authorization_response(fake_result);
            return;
        }

        MedusaID chosen = _qp._id;
        int32 nb_replicas = replicas.size();

        while ( chosen == _qp._id )
        { 
            int32 random_index = rand() % nb_replicas;
            chosen = replicas[random_index];
            DEBUG << "Chosing random replica " << chosen;
        }

        Remote<QueryProcessor> replica_qp = _qp.qp_for(MedusaID(to_string(chosen)));
        if ( ! replica_qp )
        { ERROR << "QueryProcessor for " << chosen << " is gone??";
        } 
        else 
        { replica_qp.authorize_reconciliation( wrap(this, &ConsistencyMngr::request_authorization_response),_qp._id);
        }

        // Reschedule authorization request
        (new CallbackTimer(_qp._my_loop,wrap(this,&ConsistencyMngr::request_authorization)))->arm(Time::now()+Time::msecs(_rec_request_interval)); 

    }

}

// --------------------------------------------------
void
ConsistencyMngr::request_authorization_response(RPC<bool> result)
{
    if ( ! result.valid() )
    { 
        ERROR << "Failed contacting partner about authorization " << result.stat();
        return;
    }


    if ( _authorized_partners_while_awaiting_response && result.valid() && (*result == true))
    { 
        WARN << "Concurrently authorized partner already so ignoring this response ";
        return;
    }

    _authorized_partners_while_awaiting_response = false;

    // If got a positive response and we still need one
    if ( result.valid() && (*result == true) && _pending_reconcile_authorization)
    {

        update_state(ConsistencyState::STABILIZATION);
        _pending_reconcile_authorization = false;

        INFO << "Got authorization to reconcile... launching reconciliation at time " << Timestamp::now();
        _reconciliation_in_progress = true;

        // First announce the fact that we are entering STABILIZATION. Then give some time
        // to downstream neighbors to realize what is happening and re-connect to the appropriate other nodes
        // Only then we can actually start to reconcile our state.
        launch_reconciliation();
        //(new CallbackTimer(_qp._my_loop,wrap(this,&ConsistencyMngr::launch_reconciliation)))->arm(Time::now()+Time::msecs(100)); 
        //Thread::sleep(Time::msecs(1000)); 

    }

}

// --------------------------------------------------
void ConsistencyMngr::check_for_control_stream(const CatalogBox* box)
{

    // Check for soutput boxes
    if ( box->get_box_type() == "soutput")
    {
        Name control_stream_name = find_control_stream_name(box);
        monitor_control_stream(control_stream_name);
        _soutput_set.insert(control_stream_name);
        INFO << "Set of all SOutputs is now " << _soutput_set;
    }

    // Check for sunion boxes
    else if ( box->get_box_type() == "sunion")
    {        

        // Get parameters
        string benchmark;
        string location;
        string boundary_interval_param;
        string checkpoint_interval_param;

        try 
        { 
            CatalogBox* not_const_box = (CatalogBox*)box;
            benchmark = not_const_box->get_box_parameter()->parameter("benchmark");
            location = not_const_box->get_box_parameter()->parameter("location");
            boundary_interval_param = not_const_box->get_box_parameter()->parameter("interval");
            checkpoint_interval_param = not_const_box->get_box_parameter()->parameter("checkpoint");   
        }
        catch (AuroraException e) 
        { ERROR << "Failed getting parameters from box " << e;
        }


        _benchmark = MicroBenchmarkType(benchmark);
       
        // To make linker happy?
        DEBUG << _benchmark;
    
        // Find the name of the second output stream
        Name control_stream_name = find_control_stream_name(box);
        monitor_control_stream(control_stream_name);
        map_output_to_input_stream(box,control_stream_name);
        
        if ( location == "input")
        {  
            INFO << "Initializing last boundary for sunion " << control_stream_name;
            _input_sunions_last_boundary[control_stream_name] = Timestamp::now();
        }
        
        // Check how often boundaries are supposed to appear        
        if ( boundary_interval_param.size() > 0 )
        { _max_boundary_interval = 2 * atoi(boundary_interval_param.c_str());
        }
        
        if ( _benchmark.do_checkpoint() )
        {
            if ( checkpoint_interval_param.size() > 0 )
            { _checkpoint_interval = atoi(checkpoint_interval_param.c_str());
            }
            DEBUG << "Checkpoint interval is " << _checkpoint_interval;
            start_checkpointing();
        }
    }

}




// --------------------------------------------------
/// Get input stream to box that produces given output control stream
void ConsistencyMngr::map_output_to_input_stream(const CatalogBox* box, Name output_stream_name)
{

    Name input_stream_name = box->get_in_stream(0)->get_stream_name();
    int tuple_size = box->get_in_stream(0)->get_stream_schema()->get_size();

    INFO << "Mapping " << output_stream_name << " onto input " << input_stream_name << " of size " << tuple_size;

    _io_control[output_stream_name] = pair<Name,int>(input_stream_name,tuple_size);

}



Name ConsistencyMngr::find_control_stream_name(const CatalogBox* box)
{
    // Assumption: The control stream is ALWAYS the first output stream!
    INFO << "Assuming the control output stream is " << box->get_out_stream(0)->get_stream_name();
    return box->get_out_stream(0)->get_stream_name();
}
 

// --------------------------------------------------
void ConsistencyMngr::monitor_control_stream(Name control_stream_name)
{

    // Dequeue the control messages for this operator
    _qp._data_path.add_output_path(control_stream_name);

    // Subscribe to receive the output stream
    _qp._data_path.add_local_output_path_client(control_stream_name, wrap(this,&ConsistencyMngr::handle_control_tuples));


}

// --------------------------------------------------
void ConsistencyMngr::start_checkpointing()
{
    if ( !_checkpointing_started )
    {
        _checkpointing_started = true;
        checkpoint();
    }
}

// --------------------------------------------------
void ConsistencyMngr::checkpoint()
{

    if ( ! _suspend_checkpoints)
    {
        vector<long long> useless_measurements;
        _qp._aurora.checkpoint(useless_measurements);
        (new CallbackTimer(_qp._my_loop,wrap(this,&ConsistencyMngr::checkpoint)))->arm(Time::now()+Time::msecs(_checkpoint_interval));
    }
}


// --------------------------------------------------
Status ConsistencyMngr::handle_control_tuples(ptr<StreamEvent> event)
{

    // Currently all tuples just ask to reconcile state starting from some timestamp
    int index = 0;
    DEBUG << "State of query process is " << _state;

    for ( int i = 0; i < event->_inserted_count; i++)
    {
        Tuple control_tuple = Tuple::factory(&event->_bin_tuples[index]);
        TupleType type = control_tuple.get_tuple_type();
        Timestamp time = control_tuple.get_tuple_stime();
        index += HEADER_SIZE; // Control tuples have no fields

        if ( (type == TupleType::RECONCILIATION_REQUEST) && (!_reconciliation_in_progress) )
        {
            DEBUG << "Got reconciliation request from " << event->_stream;
            _sunions_ready_to_reconcile[event->_stream] = time;
            if (can_reconcile() && (!_pending_reconcile_authorization ))
            {
                INFO << "All failed SUnions ready to reconcile, requesting replica authorization at time " << Timestamp::now();
                _pending_reconcile_authorization = true;
                _authorized_partners_while_awaiting_response = false;
                request_authorization();
            }
        }
        else if ( (type == TupleType::RECONCILIATION_DONE))
        {
            DEBUG << "Got reconciliation done from " << event->_stream;
            // Processing a REC_DONE is a bit tricky so we use a helper method
            process_rec_done(event);
        }         
        else if ( type == TupleType::UP_FAILURE )
        {
            update_state(ConsistencyState::UP_FAILURE);
            _suspend_checkpoints = true;
            _sunions_in_failure[event->_stream] = time; // This structure is not used right now
        }

        // When was the last boundary produced by this Sunion operator
        else if ( type == TupleType::BOUNDARY )
        {
            if ( _input_sunions_last_boundary.find(event->_stream) != _input_sunions_last_boundary.end())
            { _input_sunions_last_boundary[event->_stream] = Timestamp::now();
            }
        }
    }

    return true;


}

// --------------------------------------------------
Status ConsistencyMngr::process_rec_done(ptr<StreamEvent> event)
{

    // If REC_DONE from SUNION
    if ( _input_sunions_last_boundary.find(event->_stream) != _input_sunions_last_boundary.end()) 
    {

        DEBUG << "REC_DONE came from an SUnion";

        // SUnion reconciled so remove it from the list
        _reconciled_inputs[event->_stream] = Timestamp::now();
        if ( _reconciled_inputs.size() == _input_sunions_last_boundary.size() )
        {
            DEBUG << "All failed SUnions reconciled";
            // No. We should wait until REC_DONE appears on the output stream before changing the state and finishing reconciliation
            //update_state(ConsistencyState::STABLE);
            //_reconciliation_in_progress = false;
            //finish_reconciliation();
            _sunions_in_failure.clear();
            _sunions_ready_to_reconcile.clear();
            _reconciled_inputs.clear();
        }
    }

    // Else REC_DONE from SOutput
    else
    {
        DEBUG << "REC_DONE came from an SOutput";

        _reconciled_outputs[event->_stream] = Timestamp::now();
        if ( _reconciled_outputs.size() == _soutput_set.size())
        {
            // If the REC_DONE comes from SOUtput -> change state to either STABLE or UP_FAILURE           
            //if ( _sunions_in_failure.size() > 0 )
            //{ update_state(ConsistencyState::UP_FAILURE);
            //}
            //else
            //{ update_state(ConsistencyState::STABLE);
            //}
            // Always go to STABLE state first, if there's a failure we'll go straight to UP_FAILURE at next message from SUnion
            update_state(ConsistencyState::STABLE);
            finish_reconciliation();
            _reconciliation_in_progress = false;
            _reconciled_outputs.clear();                    
        }
    }

    return true;
}



// --------------------------------------------------
void  ConsistencyMngr::send_control_tuple(Name       stream_name,
                                          TupleType  type,
                                          Timestamp  since)
{

    Name input_stream_name = _io_control[stream_name].first;
    int tuple_size = _io_control[stream_name].second;

    dynbuf tuple(tuple_size);
    Tuple t = Tuple::factory(tuple.data());
    t.clear_tuple(tuple_size);
    t.set_default_header();

    t.set_tuple_type(type);
    t.set_tuple_stime(since);

    ptr<StreamEvent> event(new StreamEvent(input_stream_name,
                                           string(tuple.data(), tuple_size),
                                           1));

    // Blocking call using enqueue thread method
    _qp._data_path.enqueue(event);
}

// --------------------------------------------------
bool  ConsistencyMngr::can_reconcile()
{
    bool ok_to_reconcile = true;

    Timestamp current_time = Timestamp::now();
    map<Name,Timestamp>::iterator j;
    for ( j = _input_sunions_last_boundary.begin(); j != _input_sunions_last_boundary.end(); ++j)
    {
        Name stream_name = j->first;
        long boundary_delay = (current_time - j->second).to_msecs();

        bool not_ready = (_sunions_ready_to_reconcile.find(stream_name) == _sunions_ready_to_reconcile.end());
        bool no_boundary = boundary_delay > _max_boundary_interval;
        DEBUG << "SUnion " << stream_name << " not ready to reconcile " << not_ready << " no boundary " << no_boundary;

        if ( (_sunions_ready_to_reconcile.find(stream_name) == _sunions_ready_to_reconcile.end()) &&
             (boundary_delay > _max_boundary_interval))
        { 
            DEBUG << "Cannot reconcile because " << stream_name << " is not ready";
            ok_to_reconcile = false;
        }
    }
    return ok_to_reconcile;
}

// --------------------------------------------------
void ConsistencyMngr::launch_reconciliation()
{
    // Now reconcile without waiting for an answer about the subscriptions
    // because we already suspended sending data downstream anyways
    if ( _benchmark.do_undo_reconciliation() )
    {
        // Testing undo/redo
        map<Name,Timestamp>::iterator i;
        for ( i = _sunions_in_failure.begin(); i != _sunions_in_failure.end(); ++i)
        {
            Name control_stream = i->first;
            Timestamp time = i->second;
            send_control_tuple(control_stream,TupleType::UNDO_REDO_START,time);
        }
    } 

    else
    {
        // Testing checkpoint redo
        _qp._aurora.recover(); // updte the aurora node based on the last checkpoint.
    }

}


// --------------------------------------------------
void ConsistencyMngr::finish_reconciliation()
{

    INFO << "Got ok to finish reconciliation";

    _suspend_checkpoints = false;
    if ( _benchmark.do_checkpoint())
    { checkpoint();
    }

}


// --------------------------------------------------
void ConsistencyMngr::update_state(ConsistencyState new_state)
{

    // Update the state only when it actually changes
    if ( new_state != _state )
    {
        INFO << "Changing state from " << _state << " to " << new_state;
        _state = new_state;
        _qp.update_local_stream_states(_state);
    }

}


BOREALIS_NAMESPACE_END


// // --------------------------------------------------
// void ConsistencyMngr::check_for_control_streams(Query& query)
// {

//     const vector<Box>& b = query.get_boxes();
//     vector<Box>::const_iterator i = b.begin();

//     // Check if any of the boxes is an sunion box
//     for ( i = b.begin(); i != b.end(); ++i)
//     {
//         // Get the state size from the join (ONLY for EXPERIMENTS)
//         if ( i->get_type() == "sjoin")
//         {
//             int left_buffer_size = atoi(i->get_param("left-buffer-size").c_str());
//             int right_buffer_size = atoi(i->get_param("right-buffer-size").c_str());
//             _state_size = left_buffer_size + right_buffer_size;
//         }

//         // Check for soutput boxes
//         else if ( i->get_type() == "soutput")
//         {
//             Name control_stream_name = find_control_stream_name(i->get_outputs());
//             monitor_control_stream(control_stream_name);
//         }

//         // Check for sunion boxes
//         else if ( i->get_type() == "sunion")
//         {
//             _benchmark = MicroBenchmarkType(i->get_param("benchmark"));

//             // Find the name of the second output stream
//             Name control_stream_name = find_control_stream_name(i->get_outputs());
//             monitor_control_stream(control_stream_name);
//             get_first_input_stream(query, *i,control_stream_name);
             
//             if ( i->get_param("location") != "middle")
//             {  INFO << "Initializing last boundary for sunion " << control_stream_name;
//                 _input_sunions_last_boundary[control_stream_name] = Timestamp::now();
//             }

//             // Check how often boundaries are supposed to appear
//             string boundary_interval_param = i->get_param("interval");
//             if ( boundary_interval_param.size() > 0 )
//             { _max_boundary_interval = 2 * atoi(boundary_interval_param.c_str());
//             }

//             if ( _benchmark.do_checkpoint() )
//             {
//                 string checkpoint_param = i->get_param("checkpoint");
//                 if ( checkpoint_param.size() > 0 )
//                 { _checkpoint_interval = atoi(checkpoint_param.c_str());
//                 }
//                 DEBUG << "Checkpoint interval is " << _checkpoint_interval;
//                 start_checkpointing();
//             }
//         }
//     }

// }


// // --------------------------------------------------
// /// Get input stream to box that produces given output control stream
// void ConsistencyMngr::get_first_input_stream(const Query& query, const Box& b, Name output_stream_name)
// {
//     Name input_stream_name;
//     int tuple_size;


//     // Get the name and size of first input stream: we will use it to send control msgs
//     Box::StreamBindings inputs = b.get_inputs();
//     Box::StreamBindings::iterator k;
//     for ( k = inputs.begin(); k != inputs.end(); ++k)
//     {
//         if ( k->first == 1)
//         {
//             input_stream_name = k->second.get_name();
//             tuple_size = query.get_stream_def(input_stream_name)->get_schema()._td.get_size();
//             INFO << "Tuple size is " << tuple_size << " and input stream name is " << input_stream_name;
//             break;
//         }
//     }
//     _io_control[output_stream_name] = pair<Name,int>(input_stream_name,tuple_size);

// }        

// --------------------------------------------------
// Name ConsistencyMngr:: find_control_stream_name(const Box::StreamBindings& outputs)
// {
//     Name control_stream_name;
//     Box::StreamBindings::const_iterator i;
//     for ( i = outputs.begin(); i != outputs.end(); ++i)
//     {
//         // First output is the control output
//         if ( i->first == 1)
//         {
//             // Subscribe to the control stream
//             control_stream_name = i->second.get_name();
//         }
//     }
//     return control_stream_name;
// }

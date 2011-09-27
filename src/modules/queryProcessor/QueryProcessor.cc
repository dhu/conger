#include "QueryProcessor.h"
#include "CatalogSubscription.h" // Temporary until we get rid of old catalog

BOREALIS_NAMESPACE_BEGIN

// --------------------------------------------------
QueryProcessor::QueryProcessor(string id, InetAddress data_add) :

    BasicComponent(id,"QueryProcessor"),
    _head_client(NULL),
    _aurora(_local_catalog, id),
    _aurora_started(false),
    _data_add(data_add),
    _data_path(id,_aurora, data_add),
    _constman(*this),
    _ongoing_dynamic_modification(false)
{}


// --------------------------------------------------
QueryProcessor::~QueryProcessor()
{
    // Stop pushing data into Aurora
    _data_path.stop_enqueue();

    try
    {   if (_aurora_started)
        {   _aurora.shutdown();
        }
    }
    catch ( AuroraException e)
    {
        ERROR << "Failed shutting down Aurora: " << e;
    }

    _streams.clear();
}

// --------------------------------------------------
void QueryProcessor::init() {}

// --------------------------------------------------
void QueryProcessor::in_thread_init()
{
    DEBUG << "Initialized Aurora query processor";

    try
    {   _aurora.start();
        set_scheduler_event_handler();
        _aurora_started = true;
    }
    catch ( AuroraException e)
    {
        ERROR << "Failed shutting down Aurora: " << e;
    }

    _data_path.init();
    Time now = Time::now();

    /******************************************* yna create_query
    // yna -- no stats since we never create any StreamStates.

    // Start updating stats periodically
    _next_time_stats_computed = now + Time::msecs(STATS_UPDATE_INTERVAL);
    (new CallbackTimer(_my_loop,wrap(this,&QueryProcessor::update_stats,now)))->arm(_next_time_stats_computed);

    // If they both fall at the same time, then the second timer task will not work
    Time next_time_ha_runs = now + Time::msecs(_ha_interval);
    if ( next_time_ha_runs == _next_time_stats_computed)
    {
        next_time_ha_runs += Time::msecs(25);
    }


    // XXX DISABLING ALL OLD HA TASKS FOR NOW XXX
    // Start doing the HA tasks.
    // (new CallbackTimer(_my_loop,wrap(this,&QueryProcessor::do_ha_tasks)))->arm(next_time_ha_runs);
    ********************************************/


    // We register a callback with the AvailabilityMonitor. When interesting things happen
    // we will get notified. If we do not use any monitoring, then we will never get
    // notifications.
    _avail_mon.set_callback( wrap (this, &QueryProcessor::update_input_stream_states) );

    // Periodically print local stats. -- tatbul
    //
    if (STATS_UPDATE_INTERVAL > 0)
    {
        (new CallbackTimer(_my_loop, wrap(this,&QueryProcessor::print_stats)))
                    ->arm(Time::now() + Time::msecs(STATS_UPDATE_INTERVAL));
    }
}

// Before shutting down, wait Aurora finish its processing. -- tatbul
//
void QueryProcessor::finish_up()
{

    // _aurora._monitor.done(&_aurora);

    /************* remove???
    while (_ongoing_dynamic_modification)
    {
        NOTICE << "Aurora is currently performing a dynamic modification."
               << "Check back 1 minute later...";

        Thread::sleep(Time::msecs(60000));
    }
    ************/

    _aurora.shutdown();

    NOTICE << "Aurora is done...";
}

// Periodically print local stats. -- tatbul
//
void QueryProcessor::print_stats()
{
    DEBUG << _aurora.get_stats_object().as_string();

    (new CallbackTimer(_my_loop, wrap(this,&QueryProcessor::print_stats)))
                    ->arm(Time::now() + Time::msecs(STATS_UPDATE_INTERVAL));

}



// --------------------------------------------------
// All methods for managing statistics
// --------------------------------------------------

RPC<double> QueryProcessor::get_sel() { return _aurora.selval; }

// --------------------------------------------------
/// Return the currents statistics for the list of queries
RPC<vector<Stats> > QueryProcessor::get_stats()
{

    vector<Stats> list_stats;

    // for each query load is aggregate tuple input rate times PER_TUPLE_COST
    QueryStates::iterator i;
    for ( i = _queries.begin(); i != _queries.end(); ++i)
    {
        ptr<QueryState> qs = i->second;

        // Input streams
        Stats stats(i->first);
        double agg_input_rate = 0.0;
        double agg_running_input_rate = 0.0;
        const vector<Name> input_streams = qs->_query.get_input_streams();
        vector<Name>::const_iterator j;
        for (j = input_streams.begin(); j != input_streams.end(); ++j)
        {
            ptr<StreamState> stream_state = lookup(_streams,*j);
            if ( stream_state )
            {
                agg_input_rate += stream_state->_stream_stats._tuple_rate;
                agg_running_input_rate +=
                            stream_state->_stream_stats._running_tuple_rate;
                stats._bw_in += stream_state->_stream_stats._byte_rate;
            }
        }

        DEBUG << "aggregate input rate = " << agg_input_rate;
        DEBUG << "aggregate running input rate = " << agg_running_input_rate;
        stats._cpu_usage = agg_input_rate * PER_TUPLE_COST;

        // Output streams
        const vector<Name> output_streams = qs->_query.get_non_input_streams();
        vector<Name>::const_iterator k;
        for (k = output_streams.begin(); k != output_streams.end(); ++k)
        {
            ptr<StreamState> stream_state = lookup(_streams,*k);
            if ( stream_state )
                stats._bw_out += stream_state->_stream_stats._byte_rate;
        }
        stats._bw_usage = stats._bw_out / MAX_BW;

        DEBUG << "Computed stats for query : \n >>> " << stats.repr();
        list_stats.push_back(stats);
    }

    return list_stats;

}

////////////////////////////////////////////////////////////////////////////////
// Statistics Manager access.
//
/**
 * Return the current statistics structure from the StatManager.
 * This returns the TSStats (time series).
 * Added by Olga @ 04/22/2005
 */
RPC<Statistics> QueryProcessor::get_statistics()
{

    INFO << "Get all statistics...";
    return _aurora.get_stats_object();
    
}


/** Returns the statistics with a given name
 * Added by Olga@ 04/26/2005
 */
RPC<TSStats> QueryProcessor ::get_statistics_name(StatisticsName name)
{

    INFO << "Get statistics for:" << name;
    return _aurora.sm.get_stats(name);
}

/**
 * Returns the Cpu stats from AuroraNode
 * olga 27/04/05
 */

RPC<double> QueryProcessor ::get_cpu_statistics()
{
    INFO << "Get CPU statistics";
    return _aurora.get_cpu_statistics();
}

/**
 * Contacts the region server to get the regional statistics
 * It only contacts the 1st region. if we want
 */
RPC<map<string,Statistics> > QueryProcessor ::get_region_statistics()
{
    INFO << "Get Regional Statistics" ;
    if (_region_client.size()>0)
    {
        UpdateClient *client = _region_client[0];
        return client->get_region_stats();
    }
    else
    {
        return RPCFault("MetaOptimizer not started");
    }
}

RPC<map < string, map<string,double> > > QueryProcessor ::get_latency_statistics()
{
    INFO << "Get latency stats";
    if (_region_client.size()>0)
    {
        UpdateClient *client = _region_client[0];
        return client->get_latency_stats();
    }
    else
    {
        return RPCFault("MetaOptimizer not started");
    }
}


RPC<map<string, map<string, double> > > QueryProcessor ::get_tuple_delivery_statistics()
{
    INFO << "Get Tuple Delivery Statistics" ;
    if (_region_client.size()>0)
    {
        UpdateClient *client = _region_client[0];
        return client->get_tuple_delivery_stats();
    }
    else
    {
        return RPCFault("MetaOptimizer not started");
    }
}


RPC<map<string,double> > QueryProcessor ::get_region_cpu_statistics()
{
    INFO << "Get Region Cpu Statistics" ;
    if (_region_client.size()>0)
    {
        UpdateClient *client = _region_client[0];
        return client->get_cpu_stats();
    }
    else
    {
        return RPCFault("MetaOptimizer not started");
    }
}


// END HANDLING STATISTICS



// --------------------------------------------------
// All methods for Input and Output queue management
// --------------------------------------------------

// --------------------------------------------------
void
QueryProcessor::send_downstream(vector<StreamEvent>& tuples_to_send)
{
    if ( tuples_to_send.size() > 0 )
    {
        _data_path.send_downstream(tuples_to_send);
    } else
    {
        DEBUG << "Nothing to send downstream this iteration";
    }
}


void
QueryProcessor::ack_upstream(vector<StreamEvent>& tuples_to_ack)
{
    NOTICE << "ack_upstream does it handle last_tuple correctly even for 0?";

    QueueTrimmingMessages node_2qm;

    // Now treat all tuples as if they were acknowledged by the downstream node
    for ( vector<StreamEvent>::iterator i = tuples_to_ack.begin();
          i != tuples_to_ack.end(); ++i)
          {
        ptr<StreamState> stream_state = lookup(_streams,i->_stream);
        DEBUG << i->_stream;
        if ( stream_state )
        {
            StreamID id = stream_state->get_stream_def()->get_id();
            string last_tuple = i->last_tuple();
            for (QueryStates::iterator k = _queries.begin(); k != _queries.end(); ++k) { // for each query
                ptr<QueryState> qs = k->second;

                // If the given stream is part of the query non-input streams
                bool is_output = qs->_query.find(id.get_name(), qs->_query.get_non_input_streams());
                if ( is_output )
                {

                    // For each input stream of that query
                    const vector<Name> input_streams = qs->_query.get_input_streams();
                    vector<Name>::const_iterator j;
                    for (j = input_streams.begin(); j != input_streams.end(); ++j)
                    {
                        ptr<StreamState> in_state = lookup(_streams, *j);
                        if ( !in_state)
                        {
                            ERROR << "The stream state should be set at this point";
                        } else
                        {
                            MedusaID destination = in_state->get_stream_def()->get_partition().get_owner();
                            if (node_2qm.find(destination) == node_2qm.end())
                            {
                                node_2qm[destination].set_sender(_id);
                            }
                            QueueTrimmingMessage& m =  node_2qm[destination];
                            id = in_state->get_stream_def()->get_id();
                            QueueTrimmingMessageEntry e(id, last_tuple);
                            m.add(e);
                            DEBUG << "will send a queue trimming message for stream " << id << " to node " << destination;
                        }
                    }
                }
            }
        }
    }

    for (map<MedusaID, QueueTrimmingMessage>::iterator j = node_2qm.begin(); j != node_2qm.end(); j++)
    {
        DEBUG << "sending queue trimming message to " << j->first;
        send_queue_trimming_message(j->first, j->second);
    }
}

// --------------------------------------------------
/**
 * This method is invoked when the node receives a trimming message
 * from one of its downstream neighbors
 */
RPC<void> QueryProcessor::trim(QueueTrimmingMessage message)
{
    DEBUG << message;

    try
    {
        for (QueueTrimmingMessageEntries::iterator i = message._entries.begin();
            i != message._entries.end(); i++)
            trim(i->_id, i->_last_tuple);
    } catch (AuroraBadEntityException& e)
    {
        return RPCFault(e.what());
    }
    HAMethod rm = _data_path.get_recovery_method();
    if (rm == ACTIVE_STANDBY) {  // if active standby
        QueueTrimmingMessage m(_id);
        for (QueueTrimmingMessageEntries::iterator i = message._entries.begin();
            i != message._entries.end(); i++)
            {
                QueueTrimmingMessageEntry e(i->_id, i->_last_tuple);
                m.add(e);
            }
        if (_secondary)
        {
            send_queue_trimming_message(_secondary, m);
        }
    } else if (rm == PASSIVE_STANDBY)
    {
    }

    DEBUG << "Trim request from node " <<  message.get_sender() << " returns";

    return true;

}

QueueTrimmingMessages QueryProcessor::get_ack_messages()
{

    QueueTrimmingMessages node_2qm;

    // Queue trimming is triggered by periodically acknowledging tuple reception
    TrimPairs tuples_to_trim = _data_path.last_input_tuples();

    // for each input stream
    for ( TrimPairs::iterator i =  tuples_to_trim.begin(); i != tuples_to_trim.end(); ++i)
    {
        ptr<StreamState> st = i->first;
        MedusaID destination = st->get_stream_def()->get_partition().get_owner();
        if (node_2qm.find(destination) == node_2qm.end())
        {
            node_2qm[destination].set_sender(_id);
        }
        QueueTrimmingMessage& m =  node_2qm[destination];

        StreamID stream_id = st->get_stream_def()->get_id();
        string tuple = i->second;
        ////////////
        TupleDescription tdesc(_aurora.get_local_stream(stream_id.get_name())->get_schema());
        DEBUG << "ack: " << stream_id.get_name() << " - " << tdesc.tuple_to_string(tuple.c_str());
        ////////////
        QueueTrimmingMessageEntry e(stream_id, tuple);
        m.add(e);
    }
    return node_2qm;
}


void QueryProcessor::send_acks(QueueTrimmingMessages& acks)
{
    DEBUG << "Sending acks";
    for (QueueTrimmingMessages::iterator i = acks.begin(); i != acks.end(); i++)
    {
        send_ack(i->first, i->second);
    }
}

// --------------------------------------------------
void QueryProcessor::send_acks()
{
    QueueTrimmingMessages acks = get_ack_messages();
    send_acks(acks);

}

// --------------------------------------------------
/**
 * This is trivial queue trimming for experimental
 * purposes only. Assumes one output stream per query only and
 * query composed only of filters.
 */
void
QueryProcessor::send_ack(MedusaID node, QueueTrimmingMessage& message)
{

    // Default value will be false
    if ( _pending_ack[node] )
    {
        WARN << "ACK already in flight... skipping";
        return;
    }

    if (node != _id)
    {

        Remote<QueryProcessor> qp = qp_for(node);
        if (qp)
        {
            _pending_ack[node] = true;
//            int index = HEADER_SIZE;
//            int seq = *((int*)(&(tuple[index])));
//            DEBUG << "Acknowledging reception of tuple " << seq << " on stream " << stream_id;
            HAMethod rm = _data_path.get_recovery_method();
            if (rm == UPSTREAM_BACKUP)
            {
                // send ACK0's.
//                qp.ack(wrap(this, &QueryProcessor::handle_ack_response,node), message);
            }
            else if (rm == ACTIVE_STANDBY)
            {
                // always we send a queue-trimming message.
                qp.trim(wrap(this, &QueryProcessor::handle_ack_response,node),  message);
            }
            else if (rm == PASSIVE_STANDBY)
            {
                // always we send a queue-trimming message.
                qp.trim(wrap(this, &QueryProcessor::handle_ack_response,node), message);
            }
        }
    }
}


// --------------------------------------------------
void QueryProcessor::handle_ack_response(MedusaID node, RPC<void> result)
{

    _pending_ack[node] = false;
    if ( !result.valid() )
    {
        ERROR << "Ack failed for node " << node << " because " << result.stat();
    }

}

// --------------------------------------------------
void QueryProcessor::handle_trim_response(MedusaID node, RPC<void> result)
{

    _pending_trim[node] = false;
    if ( !result.valid() )
    {
        ERROR << "Trim failed for node " << node << " because " << result.stat();
    }

}


// --------------------------------------------------
/**
 * This method gets invoked when the node receives and acknowledgement from
 * one of its downstream neighbors.
 * For simplicity of experimentation we first assume only one client
 * per output stream. We also assume a single output stream per query
 */
RPC<void> QueryProcessor::ack(MedusaID node, StreamID id, string tuple)
{

    INFO << "Got ack request from node " <<  node << " returns";

    // For each query...
    for (QueryStates::iterator k = _queries.begin(); k != _queries.end(); ++k)
    {

        ptr<QueryState> qs = k->second;

        // If the given stream is part of the query non-input streams
        bool is_output = qs->_query.find(id.get_name(),qs->_query.get_non_input_streams());
        if ( is_output )
        {

            // For each input stream of that query
            const vector<Name> input_streams = qs->_query.get_input_streams();
            vector<Name>::const_iterator j;
            for (j = input_streams.begin(); j != input_streams.end(); ++j)
            {
                ptr<StreamState> in_state = lookup(_streams,*j);
                if ( !in_state)
                {
                    ERROR << "The stream state should be set at this point";
                } else
                {
                    // Send queue trimming message to corresponding upstream neighbor
                    QueueTrimmingMessage m(_id);
                    QueueTrimmingMessageEntry e(in_state->get_stream_def()->get_id(), tuple);
                    m.add(e);
                    MedusaID destination = in_state->get_stream_def()->get_partition().get_owner();
                    DEBUG << "Sending queue trimming message message " << m << " to " << destination;
                    send_queue_trimming_message(destination, m);
                }
            }
        }
    }
    DEBUG << "Ack request from node " <<  node << " returns";
    return true;
}

// --------------------------------------------------
void
QueryProcessor::send_queue_trimming_message(MedusaID node, QueueTrimmingMessage& message)
{

    // Default value will be false
    if ( _pending_trim[node] )
    {
        WARN << "Trim already in flight... skipping";
        return;
    }

    if (node != message.get_sender())
    {

        Remote<QueryProcessor> qp = qp_for(node);
        if (qp)
        {
            DEBUG << "send_queue_trimming";
            _pending_trim[node] = true;
//            int index = HEADER_SIZE;
//            int seq = *((int*)(&(tuple[index])));
//            DEBUG << "Sending trim request to node " << node;
            qp.trim(wrap(this, &QueryProcessor::handle_trim_response,node),
                    message);
        }
    }
}


void QueryProcessor::trim(StreamID id, string last_tuple)
            throw(AuroraBadEntityException)
            {

    INFO << "Trimming stream " << id;
    ptr<StreamState> st = lookup(_streams,id.get_name());
    if (! st )
        Throw( AuroraBadEntityException,
            "Unknown stream : " + to_string(id) );

    int index = HEADER_SIZE;
    int seq = *((int*)(&(last_tuple[index])));
    DEBUG << "Trimming stream " << id << " at " << seq;

    st->_buffer.trim(last_tuple);
}

void QueryProcessor::schedule_wake_up(Time next_checkpoint_time)
{
    (new  CallbackTimer(_my_loop,wrap(this,&QueryProcessor::wake_up_scheduler)))->arm(next_checkpoint_time);
}

void QueryProcessor::wake_up_scheduler()
{
    _aurora.schedule(ptr<SchedulerTask>());
}

ptr<AbstractBoxState> QueryProcessor::get_box_state(string box_name)
{
    ptr<QBox> box = _aurora.get_box(box_name);
    if (box)
    {
        return box->pack_state(true);
    }
    return ptr<AbstractBoxState>();
}

void QueryProcessor::update_box_state(string box_name, ptr<AbstractBoxState> box_state)
{
    ptr<QBox> box = _aurora.get_box(box_name);
    if (box)
    {
//        box->unpack_queue(qs);
        box->unpack_state(box_state);
    }
}

vector<string> QueryProcessor::get_xml_strings(string box_name)
{
    vector<string> xml_strings;
    ptr<QBox> box = _aurora.get_box(box_name);
    if (box)
    {
        CatalogStream::StreamStar* ins = box->get_catalog_box()->get_box_in();
        CatalogStream::StreamStar::iterator in_it = ins->begin();
        CatalogStream::StreamStar::iterator in_end = ins->end();
        for( ; in_it != in_end; ++in_it)
        {
            CatalogBox* prior = (*in_it)->get_stream_source_box();
            if (!prior)
            {
                string schema = (*in_it)->get_schema()->form_schema_xml();
                xml_strings.push_back(schema);
                string input = (*in_it)->form_input_xml();
                xml_strings.push_back(input);
            }
        }
    
        string box_xml = box->get_catalog_box()->form_box_xml(HAType::BACKUP);
        xml_strings.push_back(box_xml);
    }
    return xml_strings;
}


////////////////////////////////////////////////////////////////////////////////
////////////////   Deprecated stuff below here   ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/******************************** yna create_query

// --------------------------------------------------
// --------------------------------------------------
/// Updating stats related to current load... not quite great
void
QueryProcessor::update_stats(Time start_time)
{
    Time now = Time::now();

    Time delta_time = now - start_time;

    // for each stream, close stats period
    StreamStates::iterator i;
    for ( i = _streams.begin(); i != _streams.end(); ++i )
    {
        Name stream_name = i->first;
        ptr<StreamState> stream_state = i->second;
        stream_state->end_stat_period(delta_time.to_msecs());
    }

    // We should do something similar for operators in queries... currently we use the input/output rates
    // as an approximation
    // Keep updating stats periodically
    _next_time_stats_computed = now + Time::msecs(STATS_UPDATE_INTERVAL);
    (new CallbackTimer(_my_loop,wrap(this,&QueryProcessor::update_stats,now)))->arm(_next_time_stats_computed);
}



// --------------------------------------------------
// All methods for query manipulation
// --------------------------------------------------

/// Defines a new locally owned stream
RPC<void> QueryProcessor::create_stream_qp(StreamDef streamdef)
{

    if (_streams.count(streamdef.get_name()))
        return RPCFault(to_string(streamdef) + " already exists locally");

    ptr<StreamDef> sd( new StreamDef(streamdef) );
    ptr<StreamState> st(new StreamState(sd->get_name(), sd, StreamState::INJECT));
    _streams[st->get_name()] = st;

    INFO << "Creating locally-owned " << streamdef.repr();

    // Also prepare the input path for the injected stream
    //_data_path.add_input_path(st);
    // XXX New API for new catalog. No particular source for this input stream
    _data_path.add_input_path(st->get_name());

    // Add stream to Aurora
    try
    {
        string network_modification_string = string("<modify><add>\n");
        bool with_schema = true;
        if ( _aurora.get_tuple_description(to_string(streamdef.get_schema().get_name())))
            with_schema = false;
        network_modification_string += sd->as_xml(with_schema);
        network_modification_string += string("</add></modify>\n");
        BoxPackage modified_boxes;

        DEBUG << "Modifying network with:\n " << network_modification_string;

        locking(_data_path._aurora_lock)
        {
            _aurora.modify_network(network_modification_string,modified_boxes);
        }
    } catch (AuroraException& e)
    {
        ERROR << "Failed setting up new input stream in Aurora " << e.what();
        return e.what();
    }

    INFO << "Created locally-owned " << streamdef.repr();
    return true;
}

// -------------------------------------------------
// Creates a local cpview.
RPC<void>
QueryProcessor::create_cpview(CPViewDescription view_desc, StreamDef streamdef)
{
    INFO << "Calling AuroraNode to create_cpview with name " << view_desc.get_name()
     << " on stream " << view_desc.get_stream_id().get_name().as_string();
    _aurora.create_cpview(view_desc, streamdef);
    return true; // make compiler happy and pretend it worked :)
}


// --------------------------------------------------
RPC<Query>
QueryProcessor::typecheck_and_setup(Query query)
{

    Status stat = local_typecheck(query);
    if (stat)
    {
        stat = local_setup_query(query);
    }
    if (stat) 
    {
        return query;
    }
    else
    {
        ERROR << "Typecheck failed in QP: " << to_string(stat);
        return RPCFault("Typecheck_and_setup failed in QP: " + to_string(stat));
    }

}

// --------------------------------------------------
/// Verifying validity of a query network
RPC<Query>
QueryProcessor::typecheck(Query query)
{

    Status stat = local_typecheck(query);
    if (!stat)
    {
        ERROR << "Typecheck failed in QP: " << to_string(stat);
        return RPCFault("Typecheck failed in QP: " + to_string(stat));
    }
    else return query;
}

// --------------------------------------------------
Status
QueryProcessor::local_typecheck(Query& new_query)
{

    INFO << "Typechecking query: ";

    try
    {

    // Select tables for whom to add schemas.
    vector<Schema> table_schemas;
    vector<Name> table_schema_names;

    vector<TableDescription>::const_iterator table_it = new_query.get_tables().begin();
    vector<TableDescription>::const_iterator table_end = new_query.get_tables().end();

    for (; table_it != table_end; ++table_it)
    {
        if ( !_aurora.get_tuple_description(to_string(table_it->get_schema_name())) )
        {
        table_schemas.push_back(table_it->get_schema());
        table_schema_names.push_back(table_it->get_schema_name());
        }
    }

    // Currently Aurora does not yet help with CP computation so we use the union
    vector<Name> cp_ids;

    // Determine which input streams not yet defined at this node
    vector< pair<Name,bool> > stream_names;

    // Guarantee only one instance of a schema will be defined in Aurora
    vector<Name> schema_names;

    // Select streams for whom to add schemas.
    const vector<Name> input_streams = new_query.get_input_streams();
    vector<Name>::const_iterator st;
    for (st = input_streams.begin(); st != input_streams.end(); ++st)
    {
        
        Name stream_name = *st;
        ptr<StreamDef> stream_def = new_query.get_stream_def(*st);
        vector<Name> current_cp_ids = stream_def->get_id().get_cp_ids();
        cp_ids.insert(cp_ids.end(),current_cp_ids.begin(),current_cp_ids.end());
        
        if (!_aurora.get_stream(to_string(stream_name)))
        {

            // Checking if schema already defined
            // Note: we now check if this schema is one of the table schemas above.
            bool with_schema=true;
            if ( stream_def &&
                 _aurora.get_tuple_description(to_string(stream_def->get_schema().get_name())) ||
                 (find(schema_names.begin(), schema_names.end(), stream_def->get_schema().get_name()) != schema_names.end()) ||
                 (find(table_schema_names.begin(), table_schema_names.end(), stream_def->get_schema().get_name())
                  != table_schema_names.end()) )
                with_schema = false;
            stream_names.push_back( pair<Name,bool>(stream_name,with_schema) );
            schema_names.push_back(stream_def->get_schema().get_name());
        }
    }
    
    // Typecheck only the new stuff otherwise Aurora will give an error (this includes schemas)
    string xmlquery = new_query.as_aurora_xml_with_schemas_and_streams(table_schemas, stream_names);
    map<string,TupleDescription> simple_stream_defs;

    DEBUG << "Calling typecheck on\n" << xmlquery;

    _aurora.typecheck(xmlquery,simple_stream_defs);

    // Typecheck completes the definitions of intermediate and output streams for this query hunk
    update_stream_defs(new_query,simple_stream_defs,cp_ids);
    update_intermediate_schemas(simple_stream_defs);

    } catch (AuroraException& e)
    {
        ERROR << "Failed setting-up query " << e.what();
        return e.what();
    }

    INFO << "Done typechecking query";
    return true;
}

// --------------------------------------------------
void QueryProcessor::update_stream_defs(Query& new_query,
                                        map<string,TupleDescription>& simple_stream_defs,
                                        vector<Name>& cp_ids)
                                        {

    map<string,TupleDescription>::const_iterator i;
    for ( i = simple_stream_defs.begin(); i != simple_stream_defs.end(); ++i)
    {

        if ( ! new_query.get_stream_def(Name(i->first)))
        {

            // IF Aurora does not return a name,
            // then construct a schema name from the stream name
            string schema_name = i->second.get_name();
            Schema schema;
            if ( schema_name.size() == 0 )
            {
                schema_name = i->first + "/schema";
                //schema = Schema(schema_name,TupleDescription(schema_name,i->second.getFields()));
                const vector<SchemaField>* schema_fields = i->second.get_schema_fields();
                schema = Schema(schema_name,TupleDescription(schema_name, schema_fields->begin(), schema_fields->end()));
            } else
            {
                schema = Schema(schema_name,i->second);
            }

            ptr<StreamDef> str_def(new StreamDef(StreamID(i->first,cp_ids),schema));
            new_query.set_stream_def(Name(i->first),str_def);
            INFO << "Setting stream definition for " << i->first << " def is " << str_def;

        }
        // Don't overwrite stream definitions, because they may contain partition information
        else
        {
            DEBUG << "NOT setting stream definition for " << i->first
                  << " old def was " << new_query.get_stream_def(Name(i->first));
        }
    }

}


// --------------------------------------------------
void QueryProcessor::update_intermediate_schemas(map<string, TupleDescription>& inferred_schemas)
{
    map<string, TupleDescription>::const_iterator i = inferred_schemas.begin();
    for ( ; i != inferred_schemas.end(); i++ )
    {
        CatalogStream* stream;
        CatalogSchema* schema;

        // If Aurora does not return a name,
        // then construct a schema name from the stream name, and allocate it in the catalog.
        string schema_name = i->second.get_name();

        if ( schema_name.size() == 0 )
        {
            schema_name = i->first + "-schema";
            _local_catalog.add_schema( Name(schema_name), *(i->second.get_schema()) );
        }

        schema = _local_catalog.get_schema(schema_name);

        if ( ( stream = _local_catalog.get_stream( Name(i->first) ) ) )
        {
            if ( !stream->get_stream_schema() )
            {   stream->set_stream_schema( schema );
            }
            else
            {
                // Don't overwrite stream definitions, because they may contain partition information
                DEBUG << "NOT setting stream definition for " << i->first;
            }
        }
        else
        {
            _local_catalog.set_stream_schema( Name(i->first), schema );
        }
    }
}


// --------------------------------------------------
/// Set-up a query LOCALLY
AsyncRPC<void>
QueryProcessor::setup_query(Query query)
{
    Status stat = local_setup_query(query);
    if ( !stat )
    {
        ERROR << "Query setup failed: " + to_string(stat);
        return RPCFault("Query setup failed: " + to_string(stat));
    }
    else
    {
        return true;
    }
}

// --------------------------------------------------
/// Set-up a query LOCALLY
Status
QueryProcessor::local_setup_query(Query& query)
{
    INFO << "Setting up query locally in QP";

    try
    {

        if (_queries.count(query.get_name()))
        {
            INFO << "Duplicate query " << query.get_name();
            return string() + "Already have a query named " + query.get_name();
        }

        // Set up state structure for this query
        ptr<QueryState> state(new QueryState(query, QueryStatus::NONE));
        _queries[query.get_name()] = state;

        // Use to guarantee only one instance of a schema.
        vector<Name> schema_names;

        string network_modification_string = string("<modify><add>\n");

        // Set up new table schemas.
        vector<TableDescription>::const_iterator table_it = query.get_tables().begin();
        vector<TableDescription>::const_iterator table_end = query.get_tables().end();

        for (; table_it != table_end; ++table_it)
        {

            if ( !_aurora.get_tuple_description(to_string(table_it->get_schema_name())) &&
                 (find(schema_names.begin(), schema_names.end(), table_it->get_schema_name()) == schema_names.end()) )
                network_modification_string += table_it->get_schema().as_xml();

            schema_names.push_back(table_it->get_schema_name());
        }


        // Set up new input streams.
        const vector<Name> input_streams = query.get_input_streams();
        vector<Name>::const_iterator i;

        for (i = input_streams.begin(); i != input_streams.end(); ++i)
        {

            // Add to Aurora unless already exists
            string stream_name = to_string(*i);
            if (!_aurora.get_stream(stream_name))
            {
                ptr<StreamDef> stream_def = query.get_stream_def(*i);
                if ( ! stream_def )
                    return string("Missing stream def for ") + *i;

                bool with_schema= true;

                if ( _aurora.get_tuple_description(to_string(stream_def->get_schema().get_name())) ||
                     (find(schema_names.begin(), schema_names.end(), stream_def->get_schema().get_name()) != schema_names.end()) )
                    with_schema = false;

                network_modification_string += stream_def->as_xml(with_schema);

                // Mark schema so we don't attempt to add it again.
                schema_names.push_back(stream_def->get_schema().get_name());

                // Need to subscribe to the input stream
                state->_new_inputs.push_back(stream_name);
            }
        }

        network_modification_string += query.as_xml_body();
        network_modification_string += string("</add></modify>\n");

        BoxPackage& packed_boxes = query.get_package();

        // Then add the query and streams to Aurora
        locking(_data_path._aurora_lock)
        {
            INFO << "Before setting up the new streams and query";
            DEBUG << "Modifying Aurora with " << network_modification_string;
            _aurora.modify_network(network_modification_string, packed_boxes);
            INFO << "New streams and queries are set up";

            // bloot bloot I /think/ this is where cp+sweeper should be added
        }

        state->_status = QueryStatus::SETUP;
        INFO << "Successfully set up query";

        // Each stream within the query is a locally owned stream
        Status stat = update_local_streams(state->_query);
        if ( ! stat )
            return stat;

    } catch (AuroraException& e)
    {
        return e.what();
    }

    // If the query has an sunion operator that produces control tuples, we will start
    // listening to it. Otherwise, nothing will happen
    _constman.check_for_control_streams(query);

    return true;
}

// --------------------------------------------------
/// All streams within Aurora are also local to this QP
Status
QueryProcessor::update_local_streams(Query& query)
{

    const vector<Name> input_streams = query.get_input_streams();
    const vector<Name> all_streams = query.get_all_streams();
    vector<Name>::const_iterator i;

    for (i = all_streams.begin(); i != all_streams.end(); ++i)
    {

        Name stream_name = *i;
        ptr<StreamState> st = lookup(_streams,stream_name);

        // If doesn't exist, then create a StreamState for this stream
        if ( !st )
        {

            ptr<StreamDef> sd = query.get_stream_def(stream_name);
            if ( !sd )
                return "Missing stream definition in query description";

            st = ptr<StreamState>(new StreamState(stream_name,sd,StreamState::OTHER));
            _streams[stream_name] = st;
        } else
        {
            // If we were subscribed to it, then we don't need that any longer
            // yna -- unless it's still an input stream!
            vector<Name>::const_iterator found =
                find(input_streams.begin(), input_streams.end(), stream_name);

            if ( st->_subscribed && found == input_streams.end())
            {
                DEBUG << "update_local_streams unsubscribing from " << stream_name;
                unsubscribe_self(st);
            }
        }

        // Increase the number of dependants on this stream
        st->_nb_queries++;

    }

    return true;
}


// --------------------------------------------------
/// Remove the given query.
RPC<void>
QueryProcessor::remove_query(Query query)
{

    INFO << "Removing query!";
    Status stat = local_remove_query(query);
    if ( !stat )
        return RPCFault("Query removal failed: " + to_string(stat));
    else
    {
        return true;
    }
}


// --------------------------------------------------
// Locally remove the query.
Status
QueryProcessor::local_remove_query(Query query)
{

    INFO << "Removing query from local QP!";

    remove_from_engine(query.as_xml_body());

    // Remove all the state associated with the query and its streams
    ptr<QueryState> qs = lookup(_queries, query.get_name());
    if (qs)
    {
        delete_query_state(qs);
    }
    return true;

}

// --------------------------------------------------
Status
QueryProcessor::delete_query_state(ptr<QueryState> qs)
{

    INFO << "Deleting query";

    // For all input streams. If no query needs it anymore, then stop pushing tuples
    // on that stream and also remove it from Aurora
    vector< pair<Name, bool> > xmlize_streams;

    //const vector<Name> input_streams = qs->m_query.get_input_streams();
    const vector<Name> all_streams = qs->_query.get_all_streams();
    vector<Name>::const_iterator i;
    for (i = all_streams.begin(); i != all_streams.end(); ++i)
    {

        Name stream_name = *i;
        ptr<StreamState> st = lookup(_streams,stream_name);

        if (st)
        {
            st->_nb_queries--;
            DEBUG << "DEL: Nb queries dependent on stream " << *i << " is now " << st->_nb_queries;
            if ( st->_nb_queries <= 0 && ! st->is_injected_stream() )
            {

                INFO << "Deleting input stream";
                //_data_path.remove_output_path(st);
                _data_path.remove_output_path(stream_name);
                xmlize_streams.push_back(pair<Name,bool>(stream_name, false));
                _streams.erase(stream_name);

                // If stream was a subscription, unsubscribe
                if ( st->_subscribed )
                {
                    unsubscribe_self(st);
                }
            }
        }
    }


    // Remove useless input streams from Aurora
    if ( xmlize_streams.size() > 0)
    {
        string streams_as_xml = qs->_query.as_xml_stream_defs(xmlize_streams);
        Status stat = remove_from_engine(streams_as_xml);
    if (!stat) return stat;
    }

    // We can forget about the query
    QueryStates::iterator q = _queries.find(qs->_query.get_name());
    if ( q != _queries.end())
        _queries.erase(q);

    return true;
}

// --------------------------------------------------
Status QueryProcessor::remove_from_engine(string as_xml)
{

    // Remove useless input streams from Aurora
    INFO << "Attempting to modify with: " << as_xml;
    string network_modification_string = string("<modify><remove>\n");
    network_modification_string += as_xml;
    network_modification_string += "</remove></modify>\n";
    locking(_data_path._aurora_lock)
    {
        INFO << "Before removing from Aurora " << network_modification_string;
        BoxPackage modified_boxes;
        try
        {
            _aurora.modify_network(network_modification_string,modified_boxes);
        } catch (AuroraException& e)
        {
            return e.what();
        }
        INFO << "Successfully removed from Aurora!";
    }
    return true;
}

// --------------------------------------------------
// Replace query hunk metadata
RPC<void>
QueryProcessor::replace_query(vector<Name> old_queries, vector<Query> new_queries)
{
    INFO << "Replacing query hunks!";

    Status stat = local_replace_query(old_queries, new_queries);
    if ( !stat )
    return RPCFault();
    else
    return true;
}

// --------------------------------------------------
// When a client requests to split or merge query hunks, we need to update two things:
// - Update the number of hunks that depend on a single stream
// - Update the list of queries that we know about
//
Status
QueryProcessor::local_replace_query(vector<Name> old_queries, vector<Query> new_queries)
{

    // For each old query,
    for(vector<Name>::iterator i = old_queries.begin(); i != old_queries.end(); i++)
    {

        // For each stream, decrease the number of dependents by one because the old hunk no longer exists
    vector<Name> query_streams = _queries[*i]->_query.get_all_streams();
    vector<Name>::iterator stream_it = query_streams.begin();
    for(; stream_it != query_streams.end(); stream_it++)
    {
        ptr<StreamState> str_state = lookup(_streams,*stream_it);
        if (str_state)
        {
        str_state->_nb_queries--;
        INFO << "OLD: Nb queries dependent on stream " << *stream_it << " : " << str_state->_nb_queries;
        }
    }

        // Remove the query from our state
    _queries.erase(*i);
    }

    // For each new query
    for(vector<Query>::iterator i = new_queries.begin(); i != new_queries.end(); i++)
    {

        // For each stream, increase the number of dependents by one
    vector<Name> query_streams = i->get_all_streams();
    vector<Name>::iterator stream_it = query_streams.begin();
    for(; stream_it != query_streams.end(); stream_it++)
    {
        ptr<StreamState> str_state = lookup(_streams,*stream_it);
        if (str_state)
        {
        str_state->_nb_queries++;
        INFO << "NEW: Nb queries dependent on stream " << *stream_it << " : " << str_state->_nb_queries;
        }
    }
        // Add the new query to local state
    _queries[i->get_name()] = ptr<QueryState>(new QueryState(*i, QueryStatus::RUNNING));
    }

    return true;

}


// --------------------------------------------------
RPC<void> QueryProcessor::set_query_status_qp(Name name, QueryStatus status)
{

    Status stat = local_set_query_status(name,status);
    if (!stat)
    {
        return RPCFault("Query status set failed: " + to_string(stat));
    }
    return true;

}

// --------------------------------------------------
RPC<void> QueryProcessor::set_queries_status_qp(vector<Name> names, QueryStatus status)
{

    Status stat = true;

    // When queries move, we need to remove them all first before adjusting dependencies
    if (status == QueryStatus::MOVED)
    {

        vector< ptr<QueryState> > query_states;

        // First remove all queries
        for ( vector<Name>::iterator i = names.begin(); i != names.end(); ++i)
        {
            ptr<QueryState> qs = lookup(_queries, *i);
            if (!qs)
                return RPCFault("Query not found locally " + to_string(*i));
            query_states.push_back(qs);
            stat = local_remove_query(qs->_query);
            if (!stat)
                return RPCFault(to_string(stat));
        }

        // THEN adjust all dependencies
        if ( status._new_location)
        {
            for ( vector< ptr<QueryState> >::iterator j = query_states.begin();
                  j != query_states.end(); ++j)
                  {
                check_dependencies_moved_query((*j)->_query,status._new_location);
            }
        }
    }

    // Otherwise, just do the usual stuff
    else
    {
        for ( vector<Name>::iterator k = names.begin(); k != names.end(); ++k)
        {
            stat = local_set_query_status(*k,status);
            if ( !stat )
                return RPCFault("Failed setting queries status " + to_string(stat));
        }
    }
    return true;

}

// --------------------------------------------------
Status
QueryProcessor::local_set_query_status(Name name, QueryStatus status)
{

    ptr<QueryState> qs = lookup(_queries, name);
    if (!qs)
    {
        ERROR << "Unknown query " + to_string(name);
        return "Unknown query " + to_string(name);
    }

    Status stat = true;

    // Handle deleting queries
    if (status == QueryStatus::DELETED)
    {
        stat = local_remove_query(qs->_query);
    }

    // Handle moved query
    else if (status == QueryStatus::MOVED)
    {
        stat = local_remove_query(qs->_query);
        if ( stat && status._new_location)
            check_dependencies_moved_query(qs->_query,status._new_location);
    }

    // Start a query
    else if (qs->_status == QueryStatus::SETUP && status == QueryStatus::RUNNING)
    {
        stat = local_start_query(*qs);
    } else
    {
        stat = Status("Changing query status failed: " + to_string(qs->_status) + "; cannot go to " + status);
    }

    return stat;

}

// --------------------------------------------------
/// When a local query moves, we may need to subscribe ourselves to its outputs
Status
QueryProcessor::check_dependencies_moved_query(Query query, MedusaID new_location)
{

    // We might need to subscribe to streams if we still need any of these
    bool with_history = false;
    const vector<Name> other_streams = query.get_non_input_streams();
    vector<Name>::const_iterator i;
    for (i = other_streams.begin(); i != other_streams.end(); ++i)
    {
        ptr<StreamState> st = lookup(_streams,*i);
        if ( st )
        {
            INFO << "For stream " << *i << " nb dependents is " << st->_nb_queries;
            if ( st->_nb_queries > 0 )
            {
                INFO << "Going to subscribe self to upstream hunk that just moved to " << new_location;
                //m_data_path.add_input_path(st); -> done in subscribe_self
                Status stat = subscribe_self(new_location,st,with_history);
                if ( ! stat )
                    return stat;
            }
        } else
        {
            // Whole stream is gone so we don't need to do anything
        }

    }
    return true;
}



// --------------------------------------------------
/// Start or delete queries
Status
QueryProcessor::local_start_query(QueryState& qs)
{

    DEBUG << "Changing query status from SETUP to RUNNING";
    const vector<Name> input_streams = qs._new_inputs;
    vector<Name>::const_iterator i;
    for (i = input_streams.begin(); i != input_streams.end(); ++i)
    {

        DEBUG << "Examining query input stream: " << *i;
        Name stream_name = *i;
        ptr<StreamState> st = lookup(_streams,stream_name);
        if ( !st  )
        {
            return "Missing stream state for input stream " + to_string(stream_name);
        }

        // If necessary subscribe to inputs streams
        ptr<StreamDef> sd = qs._query.get_stream_def(stream_name);
        MedusaID owner = sd->get_partition().get_owner();
        DEBUG << "Stream owner is: " << owner;

        if ( (owner != _id) && (!st->_subscribed) )
        {
            INFO << "With history is : " << qs._query.with_history();

            Status stat = subscribe_self(owner,st,qs._query.with_history());
            if ( ! stat )
            {
                return stat;
            }

            // TODO: remove when XML interface uses AdminCatalog -- yna
            Status local_sub_success = update_catalog_subscriptions(owner, stream_name);
            if ( !local_sub_success )
                return local_sub_success;
        }
    }

    qs._status = QueryStatus::RUNNING;
    qs._new_inputs.clear();
    return true;
}

// --------------------------------------------------
/// Update new catalog's subscriptions.
Status QueryProcessor::update_catalog_subscriptions(MedusaID owner, Name stream_name)
{
    try
    {
        CatalogStream *stream = _local_catalog.get_stream(stream_name);
        if ( stream )
        {
            InetAddress addr(owner.get_id());
            CatalogSubscription sub(addr, stream_name);
            _local_catalog.add_subscription(sub);

            // Magda added: For each new upstream neighbor, ask for its replica_set
            // If there is a replica_set, add that information to the new catalog
            // Ask the AvailabilityMonitor to monitor all the new nodes
            //_avail_mon.add_node(wrap(this, &QueryProcessor::avail_monitor_response,string("add_node")),
            //                  addr);
            request_replicas(addr,stream_name);

            CatalogSubscription *csub = _local_catalog.get_subscription(addr, stream_name);
            stream->subscribe_source(csub);
            DEBUG << "Update new catalog subscription with " << stream_name.as_string()
                  << "@" << addr.as_string();
        }
        else
        {
            DEBUG << "Could not add subscription, stream "
                  << stream_name << " not found in the catalog.";
            return ("Could not add subscription, stream "
                    + stream_name.as_string() + " not found in the catalog.");
        }
    } catch (AuroraBadEntityException& abe)
    {
        DEBUG << "Subscription to stream " << stream_name
              << " already exists in the catalog.";
    }

    return true;
}


// --------------------------------------------------
// Choke inputs on the given queries.
RPC< vector<Query> >
QueryProcessor::choke_queries( vector<Query> queries )
{

    for ( vector<Query>::iterator i = queries.begin(); i != queries.end(); ++i)
    {
        Status stat = local_choke_query(*i);
        if ( ! stat )
            return RPCFault("Choking inputs failed for query " + to_string(*i)
                            + "with error " + to_string(stat));
    }
    return queries;

}

// --------------------------------------------------
// Locally choke inputs.
Status
QueryProcessor::local_choke_query(Query& query)
{

    const vector<Box> & boxes = query.get_boxes();
    vector<string> box_names;

    for(vector<Box>::const_iterator i = boxes.begin(); i != boxes.end(); i++)
    box_names.push_back(to_string(i->get_name()));

    INFO << "Choking box inputs...";
    _aurora.choke_boxes(box_names);
    INFO << "Finished choking inputs!";
    return true;
}


// --------------------------------------------------
// Resume inputs on the given queries.
RPC<void>
QueryProcessor::resume_queries(vector<Query> queries)
{

    for ( vector<Query>::iterator i = queries.begin(); i != queries.end(); ++i)
    {
        Status stat = local_resume_query(*i);
        if ( ! stat )
            return RPCFault("Resuming inputs failed for query " + to_string(*i)
                            + "with error " + to_string(stat));
    }
    return true;

}

// --------------------------------------------------
Status
QueryProcessor::local_resume_query(Query& query)
{
    const vector<Box> & boxes = query.get_boxes();
    vector<string> box_names;

    for(vector<Box>::const_iterator i = boxes.begin(); i != boxes.end(); i++)
    box_names.push_back(to_string(i->get_name()));

    _aurora.resume_box_streams(box_names);
    return true;
}

// --------------------------------------------------
// Pack the given query.
RPC<Query>
QueryProcessor::pack_query(Query query)
{
    INFO << "Packing query...";
    Status stat = local_pack_query(query);
    if ( !stat )
    return RPCFault("Packing query failed: " + to_string(stat));
    else
    return query;
}


// --------------------------------------------------
RPC< vector<Query> >
QueryProcessor::pack_queries( vector<Query> queries)
{

    for ( vector<Query>::iterator i = queries.begin(); i != queries.end(); ++i)
    {
        Status stat = local_pack_query(*i);
        if ( ! stat )
            return RPCFault("Packing query failed for query " + to_string(*i)
                            + "with error " + to_string(stat));
    }
    return queries;
}

// --------------------------------------------------
// Locally pack the query.
Status
QueryProcessor::local_pack_query(Query& query)
{

    BoxPackage box_snapshot;

    string network_pack_string = "<pack>\n";
    network_pack_string += query.as_xml_body();
    network_pack_string += "</pack>\n";

    //locking(m_data_path.m_aurora_lock)
    //{
    INFO << "Before packing boxes...";
    _aurora.pack_boxes(network_pack_string, box_snapshot);
    INFO << "After packing boxes...";
    //}

    query.add_package(box_snapshot);
    INFO << "After adding package...";
    return true;

}

// --------------------------------------------------
RPC< vector<Query> >
QueryProcessor::get_queries()
{
    vector<Query> queries;
    for(LocalQueries::iterator i = _local_queries.begin(); i != _local_queries.end(); i++)
    {
        ptr<Query> query(i->second);
        queries.push_back(*query);
    }
    return queries;
}


// --------------------------------------------------
// All methods for managing subscriptions to streams
// --------------------------------------------------


// --------------------------------------------------
// Sets-up or removes a subscription for a stream
// @return the list of successfull subscription operations
//
RPC< vector<Subscription> >
QueryProcessor::subscribe_many_qp(vector<Subscription> sub, unsigned int add_or_remove)
{

    vector<Subscription> successful;
    vector<Subscription>::iterator i;
    for ( i = sub.begin(); i != sub.end(); ++i)
    {
        Status stat = do_subscribe(*i,add_or_remove);
        if ( stat )
        {
            successful.push_back(*i);
        } else
        {
            ERROR << "Subscription " << *i << " failed: " << to_string(stat);
        }
    }
    return successful;
}

// --------------------------------------------------
/// Sets-up or removes a subscription for a stream
RPC<void>
QueryProcessor::subscribe_qp(Subscription sub, unsigned int add_or_remove)
{

    Status stat = do_subscribe(sub,add_or_remove);
    if (!stat)
        return RPCFault("Subscription failed: " + to_string(stat));
    return true;
}

// --------------------------------------------------
/// Sets-up or removes a subscription for a stream
Status
QueryProcessor::do_subscribe(Subscription sub, unsigned int add_or_remove)
{

    if (add_or_remove != Subscription::ADD && add_or_remove != Subscription::REMOVE)
        return "Invalid argument to subscribe";

    ptr<StreamState> st = lookup(_streams, sub._stream);
    if (!st)
        return "Unknown stream \"" + to_string(sub._stream) + "\"";

    // Adding a subscription
    if (add_or_remove == Subscription::ADD)
    {

        // The consistency manager needs to know about all current subscriptions
        //_constman.add_subscription(sub);

        // Can't subscribe self
        if ( sub._dest == MedusaID(to_string(_data_add)) )
        {
            INFO << "We are the destination of this subscription, no need to do anything";
            return true;
        }

        st->_fast_subs.push_back(sub);

        // Make sure the stream is known to the query processor
        if ( _aurora.get_stream(to_string(sub._stream)) )
        {

            bool with_history = sub._with_history;
            // backup engines for active standby and passive standby must forward output tuples when they take over.

            HAMethod rm = _data_path.get_recovery_method();
            if (rm == ACTIVE_STANDBY && (!(_data_path.is_primary())))
            {
                with_history = true;
            }
            else if (rm == PASSIVE_STANDBY && (!(_data_path.is_primary())))
            {
                with_history = true;
            }
            //_data_path.add_output_path(st);
            //_data_path.set_data_path(sub._stream,sub._dest,
            //                       with_history, sub._first_tuple);

            // Temporary while we are still using old catalog
            DEBUG << "Still using the old catalog... during initial deployment ";
            
            // XXX This is code that is in the new subscribe_streams. Need it here to until
            // the new catalog takes over completely
            Name stream_name = sub._stream;
            InetAddress endpoint(to_string(sub._dest));
            bool should_forward_to_replicas = false;
            if ( !_data_path.has_output_path(stream_name) )
            {
                _data_path.add_output_path(stream_name);
                // Start to monitor the new output path
                _local_catalog.monitor_local_stream(stream_name);
                
                // Update the state advertised by the Availability monitor to include the new stream
                _avail_mon.set_state(wrap(this, &QueryProcessor::avail_monitor_response,string("update_state")),
                                     _local_catalog.get_local_stream_state_map());

                if ( endpoint )
                { should_forward_to_replicas = true;
                }
            }

            if ( endpoint )
            {
                DEBUG << "Valid endpoint, adding client";
                CatalogSubscription new_sub(endpoint,stream_name);
                _data_path.add_output_path_client(new_sub);

                if ( should_forward_to_replicas )
                {
                    vector<CatalogSubscription> list_subs;
                    list_subs.push_back(new_sub);
                    forward_subs_to_replicas(list_subs);
                } 
            }

        } else
        {
            return "Stream undefined in local query processor \"" + to_string(sub._stream) + "\"";
        }
    }

    // Removing a subscription
    else
    {

        // The consistency manager needs to know about all current subscriptions
        //_constman.remove_subscription(sub);

        vector<Subscription>::iterator s;
        for ( s = st->_fast_subs.begin(); s != st->_fast_subs.end(); ++ s )
        {
            if ( *s == sub )
            {
                s = st->_fast_subs.erase(s);
                break;
            }
        }        
        //_data_path.close_data_path(sub._stream,sub._dest);
        // Temporary while we are still using old catalog
        InetAddress endpoint(to_string(sub._dest));
        Name stream_name = sub._stream;
        CatalogSubscription delete_sub(endpoint,stream_name); 
        _data_path.remove_output_path_client(delete_sub);
    }
    return true;
}

// --------------------------------------------------
RPC< vector<Subscription> >
QueryProcessor::get_subscriptions(vector<Name> stream_names)
{

    vector<Subscription> subs;
    vector<Name>::iterator i;
    for ( i = stream_names.begin(); i != stream_names.end(); ++i)
    {
        ptr<StreamState> stream = lookup(_streams, *i);
        if ( !stream )
            return RPCFault("Stream " + to_string(*i) + " not found");
        else
        {
            vector<Subscription>::iterator s;
            for ( s = stream->_fast_subs.begin(); s != stream->_fast_subs.end(); ++s)
                subs.push_back(*s);
        }
    }
    return subs;
}

// --------------------------------------------------
RPC< vector<Subscription> >
QueryProcessor::get_subs()
{
    vector<Subscription> subs;
    for(LocalQueries::iterator i = _local_queries.begin(); i != _local_queries.end(); i++)
    {
        ptr<Query> query(i->second);
        // Adding all subscriptions to any non-input stream defined in the query
        const vector<Name> non_input_streams = query->get_non_input_streams();
        vector<Name>::const_iterator j;
        for (j = non_input_streams.begin(); j != non_input_streams.end(); ++j)
        {
            for ( SubscriptionMap::iterator k = _subscriptions[*j].begin();
                    k != _subscriptions[*j].end(); ++k)
                    {
                subs.push_back(*(k->second));
            }
        }
    }
    return subs;
}

// --------------------------------------------------
// // XXXX NOT Working well. Experimental XXX Change an input stream from one replica to another
// RPC<void> QueryProcessor::update_stream(StreamDef old_sd, StreamDef new_sd)
// {
//     NOTICE << "update_stream:: handle last_tuple correctly? Can it be 0?";

//     Name stream_name = old_sd.get_name();
//     ptr<StreamState> st = lookup(_streams,stream_name);
//     if (!st)
//         return RPCFault("Unknown stream \"" + to_string(stream_name) + "\"");

//     // Swap the stream definition first so we reject any new tuples that will arrive on the old stream
//     string last_tuple = _data_path.update_input_path(stream_name,new_sd);

//     // Unsubscribe from old stream (unreachable for now but should get through later)
//     Remote<QueryProcessor> old_admin = qp_for(st->_sub.first);
//     if (old_admin)
//     {
//         old_admin.subscribe(wrap((QueryProcessor*)this,
//                                  &QueryProcessor::handle_subscription_response,
//                                  ptr<StreamState>(), Subscription::REMOVE),
//                             st->_sub.second,
//                             Subscription::REMOVE);
//     }

//     // Subscribe to new stream (XXX Make that into a helper method)
//     INFO << "Will ask for history starting at tuple " << to_hex_string(last_tuple);
//     MedusaID new_owner = new_sd.get_partition().get_owner();
//     Remote<QueryProcessor> new_admin = qp_for(new_owner);
//     if (new_admin)
//     {
//         bool with_history = true;
//         Subscription subscription(stream_name,"tcp",MedusaID(to_string(_data_add)),
//                                   "QueryProcessorDataSink",
//                                   Subscription::FAST, with_history, last_tuple);
//         new_admin.subscribe(wrap((QueryProcessor*)this,
//                                  &QueryProcessor::handle_subscription_response,
//                                  st, Subscription::ADD), subscription,
//                             Subscription::ADD);
//         st->_subscribed = true;
//         st->_sub = pair<MedusaID,Subscription>(new_owner,subscription);
//     }


//     // XXX: Given that we're changing the identifier of an input stream, we need to
//     // recompute the identifiers of all intermediate and output streams.
//     //for (QueryStates::iterator i = m_queries.begin(); i != m_queries.end(); ++i)
//     //{
//     // If stream is one of input streams, then recompute the definitions of all streams
//     // Replicate the update in the StreamStates data structures
//     //}
//     // XXX: For demo, updating the definition of all streams
//     for (StreamStates::iterator i = _streams.begin(); i != _streams.end(); ++i)
//     {
//         ptr<StreamDef> sd_out = i->second->get_stream_def();
//         const StreamID& id = sd_out->get_id();
//         sd_out->set_id(StreamID(id.get_name(), new_sd.get_id().get_cp_ids()));
//     }

//     return true;
// }

// --------------------------------------------------
Status
QueryProcessor::subscribe_self(MedusaID owner, ptr<StreamState> st, bool with_history)
{

    // XXX Preparing to port to new catalog XXX
    //_data_path.add_input_path(st);
    _data_path.add_input_path(st->get_name(),
                              InetAddress(to_string(owner)));

    Remote<QueryProcessor> admin = qp_for(owner);
    if (!admin)
        return string() + "No admin for " + owner + "!";

    Subscription subscription(st->get_name(),"tcp",
                              MedusaID(to_string(_data_add)),
                              "QueryProcessorDataSink",
                              Subscription::FAST,
                              with_history);

    INFO << "Subscribing to remote stream from set_query_status";


    admin.subscribe(wrap((QueryProcessor*)this,
                         &QueryProcessor::handle_subscription_response,
                         st, Subscription::ADD), subscription,
                    Subscription::ADD);
    st->_subscribed = true;
    st->_sub = pair<MedusaID,Subscription>(owner,subscription);

    return true;
}

// --------------------------------------------------
Status QueryProcessor::unsubscribe_self(ptr<StreamState> st)
{

    // Stop accepting inputs on this path right away
    //_data_path.remove_input_path(st);
    // XXX Changing for new catalog
    _data_path.remove_input_path(st->get_name());

    Remote<QueryProcessor> admin = qp_for(st->_sub.first);
    if (!admin)
        return string() + "No admin for " + st->_sub.first + "!";

    admin.subscribe(wrap((QueryProcessor*)this,
                         &QueryProcessor::handle_subscription_response,
                         st, Subscription::REMOVE), st->_sub.second,
                    Subscription::REMOVE);
    return true;
}

void QueryProcessor::handle_subscription_response(ptr<StreamState> st,
                                                  unsigned int add_or_remove,
                                                  RPC<void> result)
                                                  {

    string un = add_or_remove == Subscription::REMOVE ? "un" : "";

    if (result.stat())
    {
        if ( (add_or_remove == Subscription::REMOVE)  && (st != NULL))
        {
            st->_subscribed = false;
            st->_sub = pair<MedusaID,Subscription>();
        }
    } else
    {
        if ( st )
        {
            WARN << "Unable to " << un << "subscribe to "
                 << st->get_name() << ": " << result.stat();
        } else
        {
            WARN << "Unable to unsubscribe because unsubscription response timed out";
        }
    }
}
*******************************************/



BOREALIS_NAMESPACE_END;

//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_QueryProcessor.h"
#undef NMSTL_RPC_DEFINE
//---

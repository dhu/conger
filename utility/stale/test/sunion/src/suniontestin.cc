#include "suniontest.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


using namespace Borealis;
using namespace NMSTL;


// --------------------------------------------------
// Continuously  produce data on input stream
class DataSource
{

public:

    static const int STREAM_ID_TO_FAIL = 1;
    static const int DELTA_SEND = 10; // msecs
    //static const int DELTA_SEND = 5; // msecs
    //static const int DELTA_PRODUCE = 10; // msecs
    static const int DELTA_PRODUCE = 10; // msecs
    static const int MAX_BATCH_STREAM_WITH_FAILURE = 100;
    static const int MAX_BATCH_STREAM_WITHOUT_FAILURE = 10;

    /// NULL replica means not to use the replica
    DataSource(MedusaClient* client, MedusaClient* replica, Name cp_name,
               vector<string> stream_names, int failure_duration, bool gui = false):
        _client(client), _replica(replica), _cp_name(cp_name),
        _stream_names(stream_names), _failure_duration(failure_duration),
        _total_tuples(0), _start_time(Time::now()), _gui(gui)
    {
        for ( unsigned int i = 0; i < stream_names.size(); i++)
        {
            _counters[i] = 0;
        }

        _failed[_client] = false;

        // Initialize lists of events
        _readyevents[_client];
        if ( _replica )
        {
            _readyevents[_replica];
            _failed[_replica] = false;
        }

        srand ( time(NULL) );
        //_failure_time = 20000 + ((rand() % 500));
        _failure_time = 10000 + ((rand() % 500));
        WARN << "Will wait " << _failure_time << " before failing if necessary";

    }

    void produce_tuples(int tuple_rate, int interval);
    void produce_boundarys(int interval);
    void send(int interval, int stream_id,bool failed_before);

    void send_batch(MedusaClient* client, int interval, int stream_id);
    int extra_sleep(int stream_id, bool previously_failed);
    Mutex _readyevents_lock; // Only needed for events for replica

private:

    void replicate_event(StreamEvent& event, int stream_id);

    MedusaClient* _client;
    MedusaClient* _replica;
    map <MedusaClient*,bool> _failed;

    Name _cp_name;
    vector<string> _stream_names;
    map<int,int> _counters;

    // Events ready for each stream
    typedef map<int, list< ptr<StreamEvent> > > ready_events;
    typedef map< MedusaClient*, ready_events > stream_events_map;
    stream_events_map _readyevents; // One list of events per client node

    int _failure_duration;

    int _total_tuples;
    Time _start_time;

    int _failure_time;
    bool _gui;
};

// --------------------------------------------------
class ReplicaDataSource : public Thread
{
public:
    ReplicaDataSource(bool gui = false, MedusaClient* replica = NULL)
        : Thread("ReplicaDataSource"), _replica(replica), _gui(gui) {}

    void set_replica(MedusaClient* replica)
    {
        _replica = replica;
    }

    void send(DataSource* ds, int interval, int stream_id);

protected:
    void run()
    {
        if ( _replica )
        { _replica->run();
        }
    }


private:
    MedusaClient* _replica;
    bool _gui;

};

// --------------------------------------------------
class SUnionTestIn : public SUnionTestInBase
{

public:

    SUnionTestIn(int argc, const char* argv[], bool gui=false)
        : SUnionTestInBase(argc,argv), _gui(gui) {}
    void start();

private:
    vector<string> create_streams();
    bool _gui;
};


// --------------------------------------------------
void
DataSource::send_batch(MedusaClient* client, int interval, int stream_id)
{
    list< ptr<StreamEvent> >& events = _readyevents[client][stream_id];
    list< ptr<StreamEvent> >::iterator start = events.begin();
    list< ptr<StreamEvent> >::iterator current;

    //INFO << "Have " <<  events.size()  << " events to send on stream  " << stream_id;
    if ( _failed[client] )
    {
        events.clear();
    }
    else
    {

        int max_nb_batches = MAX_BATCH_STREAM_WITHOUT_FAILURE;
        if ( stream_id == STREAM_ID_TO_FAIL &&  (_failure_duration > 0))
        { max_nb_batches = MAX_BATCH_STREAM_WITH_FAILURE;
        }

        for (current = events.begin() ; (current != events.end()) && (max_nb_batches > 0); ++current)
        {

            max_nb_batches --;
            Status stat = client->fast_post_event(*current);
            if ( stat )
            {
                // It worked go to next event
            } else if ( stat.as_string() == DataHandler::NO_SPACE )
            {
                WARN << "No space... will retry later for stream " << stream_id;
                break;
            } else
            {
                ERROR << "Connection closed... stopping event stream";
                _failed[client] = true;
                break;
            }
        }
        if ( current != events.end())
        {
            DEBUG << "Max batches or end of space before end of events";
        }
        events.erase(start,current);
    }

}

// --------------------------------------------------
void ReplicaDataSource::send(DataSource* ds, int interval, int stream_id)
{
    static bool previously_failed = false;

    locking ( ds->_readyevents_lock)
    { ds->send_batch(_replica,interval,stream_id);
    }

    // On stream with id STREAM_ID_TO_FAIL, after 10seconds simulate failure
    int extra_sleep = ds->extra_sleep(stream_id,previously_failed);

    bool failed = false;
    if ( extra_sleep > 0 )
    {
        previously_failed = true;
        failed = true;
    }

    (new CallbackTimer(_replica->get_loop(),wrap(this,&ReplicaDataSource::send,ds,interval,stream_id)))->arm(Time::now()+Time::msecs(interval+extra_sleep));

}

// --------------------------------------------------
void
DataSource::send(int interval, int stream_id, bool failed_before)
{
    if ( failed_before )
    {
        WARN << "Iteration after failure";
    }

    // Print a message for gui when stream is back up
    if ( _gui && failed_before)
    {
        cout << stream_id << " true" << endl;
    }

    static bool previously_failed = false;

    send_batch(_client,interval,stream_id);

    // On stream with id STREAM_ID_TO_FAIL, after 10seconds simulate failure
    failed_before = false;
    int extra_sleep_value = extra_sleep(stream_id,previously_failed);
    bool failed_now = false;
    if ( extra_sleep_value > 0 )
    {
        previously_failed = true;
        failed_before = true;
        failed_now = true;
    }

    // Print a message for gui when stream goes down
    if ( _gui && failed_now)
    {
        cout << stream_id << " false" << endl;
    }

    (new CallbackTimer(_client->get_loop(),wrap(this,&DataSource::send,interval,stream_id,failed_before)))->arm(Time::now()+Time::msecs(interval+extra_sleep_value));

}

// --------------------------------------------------
int DataSource::extra_sleep(int stream_id, bool previously_failed)
{

    int extra_sleep_value = 0;
    if ( (stream_id == STREAM_ID_TO_FAIL) && !(previously_failed) && (_failure_duration > 0) )
    {
        Time current = Time::now();
        long long delta_time = (current - _start_time).to_msecs();
        if ( delta_time > _failure_time)
        {
            WARN << "Simulating failure!";
            extra_sleep_value = _failure_duration;
        }
    }
    return extra_sleep_value;

}

// --------------------------------------------------
void
DataSource::produce_tuples(int tuple_rate, int interval)
{

    Time current_time = Time::now();
    int delta_time = (current_time - _start_time).to_msecs();
    long batch_size = 1;
    if ( delta_time > 1 )
    {
        long nb_tuples = (delta_time * tuple_rate) / 1000;
        batch_size = nb_tuples - _total_tuples;
    }
    _total_tuples += batch_size;

    if ( batch_size > 0 )
    {

        for ( int stream_id = 0; stream_id < (int)_stream_names.size(); ++stream_id)
        {

            ptr<StreamEvent> event(new StreamEvent(_stream_names[stream_id]));
            event->_inject = true;

            for ( int i = 0; i < batch_size; i++)
            {
                int id = 10 * _counters[stream_id] + stream_id;
                //int id = _counters[stream_id]*_stream_names.size() + stream_id;
                _counters[stream_id] += 1;
                PacketTuple tuple(Time::now().to_timeval(),id,TupleType::INSERTION,stream_id);
                DEBUG << "Sending tuple " << tuple;
                event->insert_bin(string((const char *)&tuple, sizeof(PacketTuple)));
            }

            _readyevents[_client][stream_id].push_back(event);
            if ( _replica && (stream_id != STREAM_ID_TO_FAIL) )
            { replicate_event(*event,stream_id);
            }
        }
    }

    (new CallbackTimer(_client->get_loop(),wrap(this,&DataSource::produce_tuples,tuple_rate,interval)))->arm(Time::now()+Time::msecs(interval));
}

// --------------------------------------------------
void
DataSource::replicate_event(StreamEvent& event, int stream_id)
{

    ptr<StreamEvent> event_replica(new StreamEvent(StreamID(event._stream,_cp_name),
                                                   event._bin_tuples,event._inserted_count));
    event_replica->_inject = event._inject;
    locking(_readyevents_lock)
    {
        _readyevents[_replica][stream_id].push_back(event_replica);
    }

}

// --------------------------------------------------
void
DataSource::produce_boundarys(int interval)
{
    static int counter = 0;

    DEBUG << "Producing boundary tuples";

    for ( int stream_id = 0; stream_id < (int)_stream_names.size(); ++stream_id)
    {
        ptr<StreamEvent> event(new StreamEvent(_stream_names[stream_id]));
        event->_inject = true;

        // Send boundary tuple on that stream
        //int id = 10 * counter + 5; // Ensuring that boundary tuples also have unique ids
        int id = 0; // Boundary tuples all have zero ids
        counter ++;
        // Because we produce data separately, we want to ensure boundary doesn't precede tuples with same timestamp
        PacketTuple tuple((Time::now()-Time::msecs(10)).to_timeval(),id,TupleType::BOUNDARY,stream_id);
        event->insert_bin(string((const char *)&tuple, sizeof(PacketTuple)));

        _readyevents[_client][stream_id].push_back(event);
        if ( _replica )
        { replicate_event(*event,stream_id);
        }
    }

    (new CallbackTimer(_client->get_loop(),wrap(this,&DataSource::produce_boundarys,interval)))->arm(Time::now()+Time::msecs(interval));

}


// --------------------------------------------------
vector<string> 
SUnionTestIn::create_streams()
{

    // -- Create schema -----------------------------------
    NOTICE << "Creating Aurora schema";
    string schema_name = "medusa://nms.lcs.mit.edu/ids/packet_header_schema";
    string schema_xml = string() +
        "<schema name=\"" + schema_name + "\">\n" +
        " <field name=\"join_value\" type=\"int\"/>\n" +
        " <field name=\"stream_id\" type=\"int\"/>\n" +
        "</schema>\n";

    //Schema schema;
    //Status s = schema.from_xml(schema_xml);
    //if ( !s ) FATAL << "Failed parsing schema from xml" << s;

    //RPC<void> r = _client.create_schema(schema);
    RPC<void> r = _client.add_schema_xml(schema_xml);
    NOTICE << "add_schema_xml: " << r;
    if (!r.stat()) exit(0);

    NOTICE << "Creating input streams";
    string input_stream_name_prefix = "medusa://nms.lcs.mit.edu/suniontest/packet_header/";
    vector<string> stream_names;

    for ( int i = 0; i < _info._nb_streams; i++)
    {
        string input_stream_name = input_stream_name_prefix + "/" + to_string(i);
        stream_names.push_back(input_stream_name);
        string input_stream_xml = string() +
            "<stream name=\"" + input_stream_name + "\" schema=\"" + schema_name + "\" />";
        INFO << "Input stream is " << input_stream_xml;
        //StreamDef input_stream_def(input_stream_name, schema);
        //r = _client.create_stream(input_stream_def);
        r = _client.add_stream_xml(input_stream_xml);
        NOTICE << "create_stream: " << r;
        if (!r.stat()) exit(0);

        // If there's a valid replica, then also define these streams at the replica
        if ( _valid_replica)
        {
            // XXX Potential problems here due to lack of versions
            string input_stream_xml_v2 = string() +
                "<stream name=\"" + input_stream_name + "\" schema=\"" + schema_name + "\" />";
            //StreamID input_stream_v2(input_stream_name, Name(CP_NAME));
            //StreamDef input_stream_def_v2(input_stream_v2, schema);
            //r = _replica.create_stream(input_stream_def_v2);
            r = _client.add_stream_xml(input_stream_xml_v2);
            NOTICE << "replica create_stream: " << r;
            if (!r.stat()) exit(0);
        }
    }

    return stream_names;
}


// --------------------------------------------------
void
SUnionTestIn::start()
{
    string query_name = "medusa://nms.lcs.mit.edu/uniontest/query";
    string output_stream = "medusa://nms.lcs.mit.edu/uniontest/query/output";
    vector<string> stream_names = create_streams();

    create_query(query_name,stream_names,output_stream);


    // -- Start generating input ------------------------
    if (! _client.set_data_path(SUnionTestParams::MAX_BUFFER,_info._node_ip,_info._node_port))
    { FATAL << "Failed setting data path";
    }

    // Will start the timer that we use to compute tuple input rate
    MedusaClient* replica = NULL;
    ReplicaDataSource rds(_gui);
    if ( _valid_replica )
    {
        replica = &_replica;
        if (! _replica.set_data_path(SUnionTestParams::MAX_BUFFER,_info._replica_ip,_info._replica_port))
        { FATAL << "Failed setting data path to replica";
        }
        rds.set_replica(replica);
        rds.detach();
        rds.start();
        DEBUG << "Set data path to replica at " << _info._replica_ip << ":" << _info._replica_port;
    }

    // Wait  a few seconds before starting to send any data so second client can start
    for ( int i = 0; i < _info._nb_nodes; i++)
    {
        Thread::sleep(Time::msecs(5000));
    }

    DataSource ds(&_client,replica,CP_NAME,stream_names,_info._failure_duration,_gui);

    // We never produce tuples more often than once every 5 msecs so...
    ds.produce_tuples(_info._tuple_rate,DataSource::DELTA_PRODUCE);

    if ( _info._benchmark.do_serialize())
    {
        ds.produce_boundarys(_info._delta_boundary);
        //ds.produceBoundarys(20); // For demo this will look better but only on fast machine
    }

    for ( unsigned int s = 0; s < stream_names.size(); s++)
    {
        // Print a message for gui to indicate stream presence
        if ( _gui )
        {
            cout << s << " true" << endl;
        }

        ds.send(DataSource::DELTA_SEND,s,false);
        //if ( _valid_replica && (s != (unsigned int)DataSource::STREAM_ID_TO_FAIL))
        if ( _valid_replica )
        { rds.send(&ds,DataSource::DELTA_SEND,s);
        }
    }

    SUnionTest::start();

}


// For the pipe between the C++ backend and Java front end
int fd_events[2];

// --------------------------------------------------
bool start_gui()
{

    WARN << "Starting Java GUI";

    // Starting the Java GUI
    pipe(fd_events); // Pipe from backend to GUI front end

    pid_t pid = fork();
    if ( pid < 0)
    {
        FATAL << "Failed forking process for GUI";

    } else if ( pid == 0)
    {

        dup2(fd_events[0],0); // read handle: stdin

        char* const gui_arguments[] = {"java","-cp", "SimpleGUI.jar", "SimpleGUI2"};
        execvp("java",gui_arguments);
        return false;

    } else
    {
        dup2(fd_events[1],1); // write handle: stdout
    }

    WARN << "Done starting Java GUI";
    return true;

}

// --------------------------------------------------
int main(int argc, const char* argv[])
{


    bool gui = false;
    for ( int i = 0; i < argc; i++ )
    {
        string option( argv[ i ]);
        if ( option == "gui" )
        {   gui = true;
        }
    }

    if ( gui )
    { start_gui();
    }

    SUnionTestIn client(argc,argv,gui);
    client.start(); // Doesn't return

}

#ifndef SUNIONTEST_H
#define SUNIONTEST_H

#include <fstream>
#include "MedusaClient.h"
#include "ConsistencyMngr.h" // For micro benchmark types
#include "Tuple.h"

using namespace Borealis;
using namespace NMSTL;

// --------------------------------------------------
/**
 * Represents one tuple in our test application
 */
struct PacketTuple : public TupleHeader
{

    int _join_value; // Using the tuple unique id here
    int _stream_id;  // Just to make our life easier in the experiments

    PacketTuple(timeval timestamp, int unique_id, TupleType type, int stream_id)
        : TupleHeader(),          // Default header values
          _join_value(unique_id), // Data
          _stream_id(stream_id)   // More data
    {

        // To measure processing times
        tv.tv_sec = timestamp.tv_sec;  
        tv.tv_usec = timestamp.tv_usec;

        // For serialization
        tuple_stime.tv_sec = timestamp.tv_sec;  // We want to set the tuple_stime
        tuple_stime.tv_usec = timestamp.tv_usec;

        tuple_id = unique_id;    // We want to set the tuple id
        tuple_type = type;       // We want to set the tuple type
    }

    string as_string() const;


} __attribute__((__packed__));

NMSTL_TO_STRING(PacketTuple);


// --------------------------------------------------
/**
 * Holds parameters for union test applications
 */
class SUnionTestParams
{

 public:
    static const int MAX_PRECISION = 15;

    // Query network properties
    static const int SIZE_STATE = 20; // 10 tuples on left and 10 tuples on right
    static const int TUPLE_RATE = 100; // Tuples/sec

    // Application properties
    static const int DELTA_BOUNDARY = 500; // msecs
    static const int MAX_DELAY = 2000;
    static const int CHECKPOINT_INTERVAL = 100;
    static const int MAX_HISTORY = 101001000;
    static const int NB_BOXES = 1;
    static const int NB_NODES = 1;
    static const int STAGGER = 1; // True/false value
    static const int REPLICA_STAGGER = 1; // True/false value

    // Maximum size of buffer with data awaiting transmission to Borealis
    //static const int MAX_BUFFER = 64000;
    //static const int MAX_BUFFER = 10000000; // 10 MB buffer to make sure it can hold complete bursts
    static const int MAX_BUFFER = 1000000; // 1 MB buffer

    SUnionTestParams(int argc, const char* argv[]);

    void runtime_args(int argc, const char* argv[],
                     string& config_file, int& line_nb,
                     string& stats_file, int& nb_nodes);

    string dump_params() const;
    string as_string() const;
    void init_from_file(string config_file, int line_nb);

    string _my_ip;
    int _my_port;

    // Primary Borealis node that we should talk to
    string _node_ip;
    int _node_port;

    // Secondary Borealis node where we should setup input streams and queries
    // and push input tuples
    string _replica_ip;
    int _replica_port;

    int _run_time;
    int _nb_streams;        // Nb streams to union
    MicroBenchmarkType _benchmark;      // Which benchmark to run
    bool _with_failure;     // True if we should simulate a failure
    int _failure_duration;  // How long should the failure last

    int _size_state;        // Size of state
    int _tuple_rate;        // Rate at which the state changes in tuples/sec

    int _delta_boundary; // How long to wait before send downstream
    int _max_delay;         // Max E2E processing delay
    int _checkpoint_interval; // Delta time between checkpoints
    int _max_history;       // How much history should operators hold for undo/redo
    int _nb_boxes;          // How many join boxes should there be

    int _stagger; // Should the primary always delay tentative tuples
    int _replica_stagger; // Should the replica always delay tentative tuples

    string _stats_file;     // Where to write stats information
    int _nb_nodes;          // When we experiment with networks of more than 1 node

};

NMSTL_TO_STRING(SUnionTestParams);

// --------------------------------------------------
/**
 * Basic class for union test
 */
class SUnionTest
{

public:
    SUnionTest(int argc, const char* argv[]);
    virtual ~SUnionTest() {}
    virtual void start();
    virtual void finish();

protected:
    static const Name CP_NAME;

    SUnionTestParams _info;
    MedusaClient _client;
    bool _valid_replica;
    MedusaClient _replica;
    ofstream _fd;

};


// --------------------------------------------------
/**
 * Basic class for union test
 */
class SUnionTestInBase : public SUnionTest
{

 public:
   SUnionTestInBase(int argc, const char* argv[])
       : SUnionTest(argc,argv) {}

 protected:
    // Produce the correct query and submit to node
   virtual void create_query(string query_base, vector<string> input_streams,string output_stream);


    virtual string create_query_boxes(string benchmark, string query_name,
                                    vector<string> stream_names, string output_stream,
                                    int nb_join_boxes, int stagger, bool reconcile);

    // Utility functions for various suniontest applications

    // For some benchmarks, we run a simplified query without the soutput operator
    // This method returns true when we should run these queries
    virtual bool no_soutput_query();

    // Produce XML for unionBox
    virtual string union_box(string benchmark, string box_base, vector<string> input_streams,
                            string output_stream, string control_output_stream,
                            int stagger, bool reconcile = true);

    // Produce XML for self-join
    virtual string self_join_box(string benchmark, string box_base,
                       string input_stream,string output_stream);

    // Produce XML for soutput box
    virtual string soutput_box(string box_base, string input_stream,
                              string output_stream, string control_output_stream);

    // self join increases tuple size by 2, so this map shrinks it back
    //virtual string mapBox(string box_base, string input_stream, string output_stream);


    ///  Utility function that sends the query_xml to a borealis node
    void submit_query(MedusaClient& client, string query_xml, string query_name);

    /**
     * Send an empty subscription for the output stream to force the Borealis
     * node to dequeue data and buffer it temporarily (won't be needed when we
     * switch to using connection points
     */
    void empty_subscribe(InetAddress borealis_node, StreamDef stream_def);

};

#endif

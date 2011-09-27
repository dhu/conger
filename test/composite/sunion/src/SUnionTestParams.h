#ifndef SUNIONTESTPARAMS_H
#define SUNIONTESTPARAMS_H

#include <fstream>

using namespace std;

// --------------------------------------------------
/**
 * Experiment parameters
 */
class SUnionTestParams
{

 public:

    static const string REPLICA_STRING;
    static const string NO_REPLICA_STRING;
    static const string STAGGER_STRING;
    static const string NO_STAGGER_STRING;

    static const int MAX_PRECISION = 15;

    // Query network properties
    static const int SIZE_STATE = 50; 
    static const int TUPLE_RATE = 1000; // Tuples/sec

    // Application properties
    static const int DELTA_BOUNDARY = 500; // msecs
    static const int MAX_DELAY = 2000;
    static const int CHECKPOINT_INTERVAL = 100;
    static const int NB_NODES = 1;

    // Maximum size of buffer with data awaiting transmission to Borealis
    static const int MAX_BUFFER = 1000000; // 1 MB buffer

    SUnionTestParams();

    SUnionTestParams(int argc, const char* argv[]);

    void   runtime_args(int argc, const char* argv[],
                        string& config_file, int& line_nb,
                        string& stats_file);

    string dump_params() const;
    void   init_from_file(string config_file, int line_nb);

    bool  _valid; // Are the paremeters valid?

    int    _run_time;             // Experiment duration
    string _replica_string;       // Should each node have a replica
    bool   _with_failure;         // Should we create a failure
    int    _failure_duration;     // How long should the failure last

    int    _size_state;           // Size of state
    int    _tuple_rate;           // Rate at which the state changes in tuples/sec

    int    _delta_boundary;       // Time period between boundary tuples
    int    _max_delay;            // Availability requirement
    int    _checkpoint_interval;  // Delta time between checkpoints

    string _stagger_string;       // Should nodes delay tentative tuples
    int    _nb_nodes;             // When we experiment with networks of more than 1 node

    string _stats_file;           // Where to write stats information

};

#endif

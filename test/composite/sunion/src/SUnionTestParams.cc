#include "SUnionTestParams.h"
#include <NMSTL/debug>

using namespace NMSTL;

const string SUnionTestParams::REPLICA_STRING = string("REPLICA");
const string SUnionTestParams::NO_REPLICA_STRING = string("NO_REPLICA");
const string SUnionTestParams::STAGGER_STRING = string("DELAY");
const string SUnionTestParams::NO_STAGGER_STRING = string("NO_DELAY");

SUnionTestParams::SUnionTestParams()
    : _valid(false)
{
}

// --------------------------------------------------
SUnionTestParams::SUnionTestParams(int argc, const char* argv[]) :
    _valid(true),
    _run_time(0), 
    _replica_string(REPLICA_STRING),
    _with_failure(false),
    _failure_duration(0),
    _size_state(SIZE_STATE), 
    _tuple_rate(TUPLE_RATE),
    _delta_boundary(DELTA_BOUNDARY), 
    _max_delay(MAX_DELAY),
    _checkpoint_interval(CHECKPOINT_INTERVAL),
    _stagger_string(STAGGER_STRING),
    _nb_nodes(NB_NODES),
    _stats_file("results.dat")
{

    // Parameters for the experiments
    string config_file;
    int line_nb = 0;
    runtime_args(argc,argv,config_file,line_nb,_stats_file);

    if ( config_file.size() > 0 )
    { init_from_file(config_file,line_nb);
    }
    else
    { _valid = false;
    }

    DEBUG << "Will run for " << _run_time;
    DEBUG << "Experiment parameters are: "
          << "\n- Failure duration " << _failure_duration
          << "\n- Replication " << _replica_string
          << "\n- Size state  " << _size_state << " tuples "
          << "\n- Tuple rate " << _tuple_rate << " tuples/sec "
          << "\n- Delta boundary " << _delta_boundary << " msecs "
          << "\n- Max delay " << _max_delay << " msecs "
          << "\n- Checkpoint interval " << _checkpoint_interval << " msecs "
          << "\n- Delaying " << _stagger_string
          << "\n- Stats file " << _stats_file
          << "\n- Nb nodes " << _nb_nodes; 

    // Decide if we should create a failure for this experiment based on the type of benchmark
    if ( _failure_duration > 0 )
    { _with_failure = true;
    }
}

// --------------------------------------------------
string
SUnionTestParams::dump_params() const
{
    string out = string()
        + _run_time + " "
        + _replica_string + " "
        + _nb_nodes + " "
        + _failure_duration + " "
        + _size_state + " "
        + _tuple_rate + " "
        + _delta_boundary + " "
        + _max_delay + " "
        + _checkpoint_interval + " "
        + _stagger_string + " ";

    return out;
}

// --------------------------------------------------
// XXX Add error checking
void
SUnionTestParams::init_from_file(string config_file, int line_nb)
{

    DEBUG << "Config information is in file " << config_file << " at line " << line_nb;

    ifstream in_fd(config_file.c_str());
    if ( in_fd.is_open() )
    {
        int line = 0;
        const int line_size = 1024;
        char line_data[line_size];
        while ( line < line_nb )
        {
            in_fd.getline(line_data,line_size);
            line++;
        }
        string benchmark_string;
        in_fd >> _run_time >> _replica_string >> _nb_nodes >> _failure_duration
              >> _size_state >> _tuple_rate 
              >> _delta_boundary >> _max_delay >> _checkpoint_interval              
              >> _stagger_string;
    } 
    else 
    { FATAL << "Did not find file with parameters: " << config_file;
    }
    in_fd.close();
}


// --------------------------------------------------
void
SUnionTestParams::runtime_args(int argc, 
                               const char* argv[],
                               string& config_file, 
                               int& line_nb, 
                               string& stats_file) 
                               

{

    // Getopt will modify argv so we have to make a copy. It won't modify the content, though,
    // just the position of the arguments. So we make a shallow copy
    const char* argv_copy[argc];
    for ( int i = 0; i < argc; i++)
    { argv_copy[i] = argv[i];
    }

    string extra_args("p:f:");
    int option_char;
    optind = 0;
    opterr = 0; // Ignore unknown options

    string pair;
    unsigned int pos;
    while ( (option_char = getopt (argc,(char**)argv_copy,extra_args.c_str())) != EOF )
    {
        switch (option_char)
        {
        case 'p':
            pair = string(optarg);
            pos = pair.find(":",0);
            if (pos != string::npos && pos > 0)
            {
                config_file = pair.substr(0,pos);
                line_nb = atoi( pair.substr(pos+1,pair.size()-pos-1).c_str());
            } else
            {
                ERROR << "-pconfig_file:line_nb";
            }
            break;
        case 'f':
            stats_file = string(optarg);
            WARN << "Stats file is set to: " << stats_file;
            break;
        default:
            // do nothing
            break;
        }
    }
}

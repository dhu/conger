#include "args.h"
#include "MedusaClient.h"
#include "suniontest.h"

using namespace Borealis;
using namespace NMSTL;

// --------------------------------------------------
string
PacketTuple::as_string() const
{
    string out("Tuple{header=");
    out << TupleHeader::as_string()
        << ",stream_id=" << _stream_id
        << ",join_value=" << _join_value
        << "}";
    return out;
}

// --------------------------------------------------
const Name SUnionTest::CP_NAME = Name("cp_v2");

// --------------------------------------------------
SUnionTestParams::SUnionTestParams(int argc, const char* argv[])

    : _my_ip("127.0.0.1"), _my_port(25000),
      _node_ip("127.0.0.1"), _node_port(15000),
      _replica_ip(string()), _replica_port(-1),

      _run_time(INIT), _nb_streams(3),
      _benchmark(MicroBenchmarkType()), _with_failure(false), _failure_duration(0),

      _size_state(SIZE_STATE), _tuple_rate(TUPLE_RATE),
      _delta_boundary(DELTA_BOUNDARY), _max_delay(MAX_DELAY),
      _checkpoint_interval(CHECKPOINT_INTERVAL),
      _max_history(MAX_HISTORY),
      _nb_boxes(NB_BOXES),
      _stagger(STAGGER),
      _replica_stagger(REPLICA_STAGGER),
      _stats_file("result.dat"),
      _nb_nodes(NB_NODES)
{

    client_args(argc,argv,_my_ip,_my_port);
    stdArgs(argc,argv,_node_ip,_node_port,&_run_time);

    // Parameters for the experiments
    string config_file;
    int line_nb = 0;
    runtime_args(argc,argv,config_file,line_nb,_stats_file,_nb_nodes);

    if ( config_file.size() > 0 )
    { init_from_file(config_file,line_nb);
    }

    DEBUG << "My ip is " << _my_ip << " and my port is " << _my_port;
    DEBUG << "Borealis ip is " << _node_ip << " and port is " << _node_port;
    DEBUG << "Replica ip is " << _replica_ip << " and port is " << _replica_port;
    DEBUG << "Will run for " << _run_time;
    DEBUG << "Experiment parameters are: "
          << "\n- Nb streams " << _nb_streams
          << "\n- MicroBenchmark " << _benchmark
          << "\n- Failure duration " << _failure_duration
          << "\n- Size state  " << _size_state << " tuples "
          << "\n- Delta tuple " << _tuple_rate << " tuples/sec "
          << "\n- Delta boundary " << _delta_boundary << " msecs "
          << "\n- Max delay " << _max_delay << " msecs "
          << "\n- Checkpoint interval " << _checkpoint_interval << " msecs "
          << "\n- Max history " << _max_history << " tuples "
          << "\n- Number of boxes " << _nb_boxes
          << "\n- Stagger " << _stagger
          << "\n- Replica stagger " << _replica_stagger
          << "\n- Stats file " << _stats_file
          << "\n- Nb nodes " << _nb_nodes; // As a separate runtime option because each time we invoke, we give the level of this node

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
        + _nb_streams + " "
        + to_string(_benchmark) + " "
        + to_string(_failure_duration) + " "

        + _size_state + " "
        + _tuple_rate + " "

        + _delta_boundary + " "
        + _max_delay + " "
        + _checkpoint_interval + " "
        + _max_history + " "
        + _nb_boxes + " "
        + _stagger + " "
        + _replica_stagger + " ";

    return out;
}

// --------------------------------------------------
string
SUnionTestParams::as_string() const
{
    string out = string("SUnionTestParams{")
        + "Run time=" + _run_time + ","
        + "Nb streams=" + _nb_streams + ","
        + "Benchrmark=" + to_string(_benchmark) + ","
        + "Failure duration=" + _failure_duration + ","

        + "Size state=" +_size_state + ","
        + "Tuple rate=" + _tuple_rate + ","

        + "Delta boundary=" + _delta_boundary + ","
        + "Max delay" + _max_delay + "}";

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
        in_fd >> _run_time >> _nb_streams >> benchmark_string >> _failure_duration
              >> _size_state >> _tuple_rate
              >> _delta_boundary >> _max_delay >> _checkpoint_interval
              >> _max_history >> _nb_boxes >> _stagger >> _replica_stagger;

        _benchmark = MicroBenchmarkType(benchmark_string);
    }
    in_fd.close();
}


// --------------------------------------------------
void
SUnionTestParams::runtime_args(int argc, const char* argv[],
                              string& config_file, int& line_nb, string& stats_file, int& nb_nodes)

{

    // Getopt will modify argv so we have to make a copy. It won't modify the content, though,
    // just the position of the arguments. So we make a shallow copy
    const char* argv_copy[argc];
    for ( int i = 0; i < argc; i++)
    { argv_copy[i] = argv[i];
    }

    string extra_args("p:f:r:n:");
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
            break;
        case 'r':
            parse(optarg,_replica_ip,_replica_port);
            break;
        case 'n':
            nb_nodes = atoi(optarg);
            break;
        default:
            // do nothing
            break;
        }
    }
}


// --------------------------------------------------
SUnionTest::SUnionTest(int argc, const char* argv[])
    : _info(argc,argv), _client(InetAddress(_info._node_ip,_info._node_port)),
      _valid_replica(false),
      _replica(InetAddress(_info._replica_ip,_info._replica_port))
{

    if ( _info._replica_port != -1)
    {
        _valid_replica = true;
    }

}

// --------------------------------------------------
void SUnionTest::start()
{

    string file_name = _info._stats_file;
    _fd.open(file_name.c_str(), fstream::app);
    if ( _fd.is_open() )
    {
        _fd.precision(SUnionTestParams::MAX_PRECISION);
    }

    if ( _info._run_time != INIT )
    {
        ( new CallbackTimer(_client.get_loop(),wrap(this,&SUnionTest::finish)) )->arm( Time::now() + Time::msecs(_info._run_time));
    }
    _client.run();
}

// --------------------------------------------------
void SUnionTest::finish()
{
    DEBUG << "Exiting abruptly";
    _fd.close();
    exit(0);
}



// --------------------------------------------------
string
SUnionTestInBase::create_query_boxes(string benchmark, string query_base,
                                   vector<string> stream_names, string output_stream,
                                   int nb_join_boxes, int stagger, bool reconcile)

{

    string intermediate_stream = query_base + "/unioned";
    string output_control_stream = query_base + "/unioned/control";
    string box_base = query_base + "/box";

    string query_xml = union_box(benchmark,box_base,stream_names,
                                intermediate_stream,output_control_stream,
                                stagger, reconcile);


    // At least one box
    string first_intermediate = intermediate_stream + "/0";
    string box_name = box_base + "/0";
    if ( nb_join_boxes <= 1 )
    {
        query_xml += self_join_box(benchmark,box_name,intermediate_stream,output_stream);
    } else
    {
        query_xml += self_join_box(benchmark,box_name,intermediate_stream,first_intermediate);

        string in_stream, out_stream = first_intermediate;
        for (int i = 1; i < nb_join_boxes-1; i++)
        {
            box_name = box_base + "/" + to_string(i);
            in_stream = intermediate_stream + "/" + to_string(i-1);
            out_stream = intermediate_stream + "/" + to_string(i);
            query_xml += self_join_box(benchmark,box_base,in_stream,out_stream);
        }

        box_name = box_base + "/" + to_string(nb_join_boxes-1);
        query_xml += self_join_box(benchmark, box_name,out_stream,output_stream);
    }
    return query_xml;

}


// --------------------------------------------------
// When to run the query without soutput operator
bool
SUnionTestInBase::no_soutput_query()
{
    MicroBenchmarkType::Type type = _info._benchmark._type;
    return  ( ( type == MicroBenchmarkType::NONE )
              || ( type == MicroBenchmarkType::TUPLE_TYPES )
              || ( type == MicroBenchmarkType::UNDO_LOG )
              || ( type == MicroBenchmarkType::CHECKPOINT )
              || ( type == MicroBenchmarkType::SUNION )
              || ( type == MicroBenchmarkType::SUNION_BOUNDARY )
              );
}


// --------------------------------------------------
string
SUnionTestInBase::union_box(string benchmark, string box_base, vector<string> input_streams,
                           string output_stream, string control_output_stream, int stagger,
                           bool reconcile)
{

    int reconcile_value = reconcile ? 1 : 0;

    // First box is the sunion
    string query_xml = string() +
        " <box name=\"" + box_base + "/sunion\" type = \"sunion\">\n" +
        " <parameter name=\"benchmark\" value=\"" + benchmark + "\"/>\n" +
        " <parameter name=\"interval\" value=\"" + _info._delta_boundary + "\"/>\n" +
        " <parameter name=\"stagger\" value=\"" + stagger + "\"/>\n" +
        " <parameter name=\"should-reconcile\" value=\"" + reconcile_value + "\"/>\n" +
        " <parameter name=\"checkpoint\" value=\"" + _info._checkpoint_interval + "\"/>\n";
        
    for ( int j = 0; j < (int)input_streams.size(); j++)
    { query_xml += " <parameter name=\"max-delay." + to_string(j) + "\" value=\"" + _info._max_delay + "\"/>\n";
    }

    for ( int i = 0; i < (int)input_streams.size(); i++)
    { query_xml += " <in stream=\"" + input_streams[i] + "\"/>\n";
    }

    query_xml += "  <out stream=\"" + control_output_stream + "\"/>\n" +
        "  <out stream=\"" + output_stream + "\"/>\n" +

    " </box>\n";

    return query_xml;
}

// --------------------------------------------------
string
SUnionTestInBase::self_join_box(string benchmark, string box_name,
                              string input_stream,string output_stream)
{

    // The size of the left buffer determines how expensive the
    // computation is. Let's keep it at 1/2 or max 50 tuples
    int size_buffer_left =  (int)(0.5*_info._size_state);
    if ( size_buffer_left > 50 )
    {    size_buffer_left = 50 + (int)(0.01*_info._size_state);
    }
    int size_buffer_right =  _info._size_state - size_buffer_left;

    // Second box is a self join box
    string query_xml = string() +
        " <box name=\"" + box_name + "/ejoin\" type = \"experimentaljoin\">\n" +
        "  <parameter name=\"benchmark\" value=\"" + benchmark + "\"/>\n" +
        //"  <parameter name=\"checkpoint\" value=\"" + (0.5*_info._checkpoint_interval) + "\"/>\n" +
        "  <parameter name=\"checkpoint\" value=\"" + (_info._checkpoint_interval) + "\"/>\n" +
        "  <parameter name=\"max-history\" value=\"" + _info._max_history + "\"/>\n" +
        "  <parameter name=\"predicate\" value=\"left.join_value=right.join_value\"/>\n"  +
        "  <parameter name=\"left-order-by\" value=\"TUPLES\"/>\n"  +
        "  <parameter name=\"right-order-by\" value=\"TUPLES\"/>\n"  +
        "  <parameter name=\"left-buffer-size\" value=\"" + size_buffer_left + "\"/>\n" +
        "  <parameter name=\"right-buffer-size\" value=\"" + size_buffer_right +  "\"/>\n" +
        // Get rid of all extra attibutes. Keep only data from left hand-side
        "  <parameter name=\"out-field.0\" value=\"left.join_value\" />\n" +
        "  <parameter name=\"out-field-name.0\" value=\"join_value\" />\n" +
        "  <parameter name=\"out-field.1\" value=\"left.stream_id\" />\n" +
        "  <parameter name=\"out-field-name.1\" value=\"stream_id\" />\n" +
        "  <in stream=\"" + input_stream + "\"/>\n" +
        "  <in stream=\"" + input_stream + "\"/>\n" +
        "  <out stream=\"" + output_stream + "\"/>\n" +
    " </box>\n";

    return query_xml;

}


// --------------------------------------------------
string
SUnionTestInBase::soutput_box(string box_base, string input_stream,
                             string output_stream, string control_output_stream)
{

    // Third box is an soutput
    string query_xml = string() +
        " <box name=\"" + box_base + "/soutput\" type = \"soutput\">\n" +
        "  <in stream=\"" + input_stream + "\"/>\n" +
        "  <out stream=\"" + control_output_stream + "\"/>\n" +
        "  <out stream=\"" + output_stream + "\"/>\n" +
    " </box>\n";
    return query_xml;
}

// --------------------------------------------------
void
SUnionTestInBase::submit_query(MedusaClient& client, string query_xml, string query_name)
{
    //RPC<void> r = client.create_query_xml(query_xml);
    RPC<void> r = client.add_query_xml(query_xml);
    NOTICE << "add_query_xml: " << r;
    if (!r.stat()) exit(0);
    NOTICE << "Starting queries";
    r = client.start_query(query_name);
    NOTICE << "start_query: " << r;
    if (!r.stat()) exit(0);

}

// --------------------------------------------------
void
SUnionTestInBase::empty_subscribe(InetAddress borealis_node, StreamDef stream_def)
{

    // Adming no longer exists. It has been merged with QueryProcessor
    //Remote<admin> admin = _client.admin_for(to_string(borealis_node));
    Remote<QueryProcessor> qp = _client.qp_for(to_string(borealis_node));
    if ( ! qp )
        FATAL << "Could not contact " << to_string(borealis_node);

    // An empty subscription has no destination (an empty MedusaID())
    Subscription empty(stream_def, "none",MedusaID(),"none");
    RPC<void> r = qp.subscribe(empty,Subscription::ADD);
    NOTICE << "empty subscription for output stream " << r;
    if ( !r.stat()) exit(0);

}

// --------------------------------------------------
void
SUnionTestInBase::create_query(string query_base, vector<string> input_streams, string output_stream)
{

    string benchmark = to_string(_info._benchmark);

    string intermediate_stream2 = query_base + "/joined/";
    string query_output_stream = no_soutput_query() ? output_stream : intermediate_stream2;

    string query_xml_1 = string() +
        "<query name=\"" + query_base + "\">\n";

    string query_xml_reconcile = create_query_boxes(benchmark,query_base,input_streams,query_output_stream,
                                                  _info._nb_boxes,_info._stagger,true);
    string query_xml_no_reconcile = create_query_boxes(benchmark,query_base,input_streams,query_output_stream,
                                                     _info._nb_boxes,_info._replica_stagger,false);

    // If need to add soutput box
    string query_xml_2;
    if ( query_output_stream == intermediate_stream2)
    {
        string control_output_stream = output_stream + "/control";
        query_xml_2 = soutput_box(query_base,intermediate_stream2,output_stream,control_output_stream);
    }

    query_xml_2 +=  "</query>\n";

    query_xml_reconcile = query_xml_1 + query_xml_reconcile + query_xml_2;
    query_xml_no_reconcile = query_xml_1 + query_xml_no_reconcile + query_xml_2;

    DEBUG << "Query is\n" << query_xml_reconcile;
    DEBUG << "Query no reconciliation is\n" << query_xml_no_reconcile;
    submit_query(_client,query_xml_reconcile,query_base);
    StreamDef output_stream_def(output_stream);
    empty_subscribe(InetAddress(_info._node_ip,_info._node_port), output_stream_def);

    if ( _valid_replica)
    {
        submit_query(_replica,query_xml_no_reconcile,query_base);
        output_stream_def = StreamDef(output_stream,CP_NAME);
        empty_subscribe(InetAddress(_info._replica_ip,_info._replica_port), output_stream_def);
    }

}




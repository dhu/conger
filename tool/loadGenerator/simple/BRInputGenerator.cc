#include "BRInputGenerator.h"
#include "TraceFunction.h"
#include "TCPTraceFunction.h"
#include "UniformFunction.h"
#include "ConstantFunction.h"
#include "RandomWalkFunction.h"
#include "RandomPulseFunction.h"
#include "BRIOParams.h"
#include <fstream>
#include <pthread.h>
#include "Timeval.h"

#define MAX_BUFFER      100000
//#define DEBUG_BANDWIDTH_USAGE
//#define DEBUG_DATA

BOREALIS_NAMESPACE_BEGIN

using namespace time_tools;


BRInputGenerator::BRInputGenerator(
        string cfg_filename)
{
    // For feasible set test, read the stream name and rate multipler from file
    map<string, double> stream_rate_multiplier_map;
    unsigned int count = 0;
    if (BRIOParams::get_singleton()->stream_rate_mode == 2) 
    {
        ifstream ifs;
        ifs.open(BRIOParams::rate_multiplier_file_name.c_str(), ios::in);
        if (!ifs) 
        {
            FATAL << "Can not open stream rate mulitplier file: " 
                  << BRIOParams::rate_multiplier_file_name; 
        }
        string stream_name;
        double rate_multipler;
        while (ifs >> stream_name >> rate_multipler )
        {
            stream_rate_multiplier_map[stream_name] = rate_multipler;
        }
    }

    // This number is used to generate trace functions
    int TCP_trace_function_index = 0;

    fstream of;
    of.open(cfg_filename.c_str(), ios::in);
    if (!of)
    {
        cerr << "can't open input configuration file: " << cfg_filename << endl;
        exit(1);
    }
    while (!of.eof())
    {
        string stream_name;
        string node_address;
        string begin;
        while (begin != "<publish" && !of.eof())
            of >> begin;
        if (of.eof())
            break;
        of >> stream_name >> node_address >> begin;

        int pos1 = stream_name.find("\"");
        int pos2 = stream_name.rfind("\"");
        stream_name = stream_name.substr(pos1 + 1, pos2 - pos1 - 1);

        pos1 = node_address.find("\"");
        pos2 = node_address.rfind("\"");
        node_address = node_address.substr(pos1 + 1, pos2 - pos1 - 1);


        NOTICE << "Setting data path for input stream "
                      << stream_name << " " << node_address;
        InetAddress addr(node_address);


        ptr<MedusaClient> stream_client = (ptr<MedusaClient>)new MedusaClient(addr);
        _stream_clients[stream_name] = stream_client;

#ifndef DEBUG_DATA
        if (!stream_client->set_data_path(
                    MAX_BUFFER, addr.get_host(), addr.get_port()))
        {
            FATAL << "Failed setting data path for node "  << node_address;
        }
        else
        {
            NOTICE << "Success.";
        }
#endif

        pthread_t thread;
        if (pthread_create(&thread,
                           0,
                           (void*(*)(void*))run_client,
                           (void*)&(stream_client->get_loop())) < 0)
        {
            WARN << "Can not input client thread.";
        }

        // input interarrival time
        NOTICE << "Generate input interarrivate time function for stream " 
               << stream_name;

        if (BRIOParams::get_singleton()->use_tcp_trace) 
        {
            // use the trace function from TCP
            ptr<Function> trace_function( 
                    new TCPTraceFunction(TCP_trace_function_index));
            _functions[stream_name] = trace_function;

            _input_data_functions[stream_name].push_back(trace_function);

            // increase trace index
            ++TCP_trace_function_index;
            NOTICE << "Generate TCP trace function";
        }
        else if (BRIOParams::get_singleton()->stream_rate_mode == 2) 
        {
            ++count;
            NOTICE << "Generate RandomPulseFunction " << count;
            _functions[stream_name] = ptr<Function>(new RandomPulseFunction(
                        stream_rate_multiplier_map.size(),
                        count,
                        stream_rate_multiplier_map[stream_name],
                        BRIOParams::get_singleton()->random_rate_duration,
                        BRIOParams::get_singleton()->idle_duration));

        }
        else if (drand48() < BRIOParams::get_singleton()->const_input_rate_prob)
        {
            _functions[stream_name] = ptr<Function>(new ConstantFunction(1));
            NOTICE << "Generate constant function";
        }
        else
        {
            _functions[stream_name] = ptr<Function>(new TraceFunction(5000));
            NOTICE << "Generate Trace function";
        }


        if (!BRIOParams::get_singleton()->use_tcp_trace)
        {
            for (int i = 0; i < BRIOParams::get_singleton()->number_of_fields; ++i)
            {
                if (drand48() < BRIOParams::get_singleton()->uniform_input_distr_prob)
                {
                    _input_data_functions[stream_name].push_back(
                        ptr<Function>(new UniformFunction(0, 100)));
                }
                else
                {
                    _input_data_functions[stream_name].push_back(
                        ptr<Function>(
                            new RandomWalkFunction(drand48() * 100, 0, 100, 1)));
                }
            }
        }

        double rate_multiplier =
                drand48() * (BRIOParams::get_singleton()->input_rate_multiplier_upper_bound -
                        BRIOParams::get_singleton()->input_rate_multiplier_lower_bound)
                + BRIOParams::get_singleton()->input_rate_multiplier_lower_bound;
        rate_multiplier *= BRIOParams::rate_multiplier;

        _input_rate_multipliers[stream_name] = rate_multiplier;

        if (BRIOParams::get_singleton()->stream_rate_mode == 2) 
        {
             _input_rate_multipliers[stream_name] = 1;
        }


        int batch_size = static_cast<int>(
            drand48() * (BRIOParams::get_singleton()->batch_size_upper_bound -
                    BRIOParams::get_singleton()->batch_size_lower_bound)
            + BRIOParams::get_singleton()->batch_size_lower_bound);

        _batch_size[stream_name] = batch_size;

    }

    if (BRIOParams::get_singleton()->use_tcp_trace)
    {
        TCPTraceFunction::init();
    }

    NOTICE << "input streams: ";
    for (map<string, ptr<Function> >::iterator it = _functions.begin();
            it != _functions.end();
            ++it) {
        cout << it->first << "  ";
    }
    cout << endl;
}


void BRInputGenerator::run()
{
    _start_time = Timeval::now().double_value();

    // generate the initial input events
    for (map<string, ptr<Function> >::iterator it = _functions.begin();
            it != _functions.end();
            ++it)
    {
        // make the starting time random
        double inter_arrival_time = drand48() * BRIOParams::get_singleton()->batch_interval;
        double next_sending_time =
            Timeval::now().double_value() + inter_arrival_time;
        _next_sending_time[it->first] = next_sending_time;

        (new CallbackTimer(_loop,
                           wrap( this, &BRInputGenerator::send_data, it->first )))
                            ->arm(Time::secs(next_sending_time ));
    }
    _loop.run();
}

void BRInputGenerator::send_data(string stream_name)
{
    // The StreamEvent object contains the data to be sent
    ptr<StreamEvent> event = ptr<StreamEvent>( new StreamEvent( stream_name ));
    event->_inject = true;

    // compute interarrival time and batch data in event
    double inter_arrival_time =
        _functions[stream_name]->get_next_value() /
        _input_rate_multipliers[stream_name];
    batch_data(event, stream_name);

    while (inter_arrival_time < BRIOParams::get_singleton()->batch_interval)
    {
        inter_arrival_time +=
            _functions[stream_name]->get_next_value() /
            _input_rate_multipliers[stream_name];
        batch_data(event, stream_name);
    }
        
//    ostringstream oss;
//    oss << "inter_arrival_time " << setprecision(4) << inter_arrival_time; 
//    DEBUG << oss.str();

    // compute the time to send next data
    double next_sending_time =
        _next_sending_time[stream_name] + inter_arrival_time;

    _next_sending_time[stream_name] = next_sending_time;

    // insert next sending event to loop
    (new CallbackTimer(_loop,
                       wrap( this, &BRInputGenerator::send_data, stream_name )))
                        ->arm(Time::secs(next_sending_time));

#ifndef DEBUG_DATA
    // Transmit data to the node.
    ptr<MedusaClient>& client = _stream_clients[stream_name];
    Status status = client->fast_post_event(event);
    if (!status)
    {
        if ( status.as_string() == DataHandler::NO_SPACE )
        {
            WARN << "sending data for stream: "
                 << stream_name << "Node buffer full";
        }
        else
        {
            ERROR << "Connection closed... stopping event stream";
            return;
        }
    }
#endif
//    DEBUG << "send batch: " << event;



#ifdef DEBUG_BANDWIDTH_USAGE
    const unsigned int report_iterval = 5;
    static unsigned int data_bytes = 0;
    static unsigned int tuples = 0;
    static Timeval last_tm = Timeval::now(); 
    Timeval tm = Timeval::now();
     
    if (tm.double_value() - last_tm.double_value() > report_iterval)
    {
        double data_rate = 
            static_cast<int>(
                    static_cast<double>(data_bytes) / 
                    static_cast<double>(report_iterval)  * 8.0 / 1000.0) 
            / 1000.0;
        double tuple_rate = 
            static_cast<int>(
                    static_cast<double>(tuples) / 
                    static_cast<double>(report_iterval)  * 1000.0) 
            / 1000.0;

        DEBUG << "Data Rate: " << data_rate << " Mbps"
             << "  Tuple Rate: " << tuple_rate  << " tuples/sec";
        data_bytes = 0;
        tuples = 0;
        last_tm = tm;
    }
    else {
        data_bytes += event->_bin_tuples.length();
        tuples += event->_inserted_count;
    }
#endif
}

void BRInputGenerator::batch_data(
        ptr<StreamEvent> event,
        const string& stream_name)
{
    int batch_size = _batch_size[stream_name];
    if (BRIOParams::get_singleton()->batch_size_increasing_interval > 0) 
    {
        batch_size *= 
            (int)ceil((Timeval::now().double_value() - _start_time) / 
            BRIOParams::get_singleton()->batch_size_increasing_interval);
    }

    TupleHeader header;

    for (int i = 0; i <  batch_size; ++i)
    {
        ostringstream oss;
        oss.clear();
        oss << "<" << stream_name << "> (";

        string buf = string((const char*)&header, sizeof(TupleHeader));

        /* No need to send timestamp
        // output timestamp
        double timestamp = Timeval::now().double_value();

        // use current time to compute an offset
        static double initial_time;
        static bool first = true;
        if (first)
        {
            initial_time = Timeval::now().double_value();
            first = false;
        }
        oss << "(" << setw(6) << setprecision(5) 
            << timestamp  - initial_time << ")"; 

        // compute sending integer timestamp based on offset
        // The unit of the integer timestamp is ms
        timestamp = timestamp - ((floor)(initial_time / 10000) * 10000);
        timestamp = floor(timestamp * 1000);
        int int_timestamp = (int)timestamp;

        oss << "(" << setw(8) << int_timestamp << ")"; 
        */

        // 
        if (BRIOParams::get_singleton()->use_tcp_trace)
        {
            ptr<Function>& input_data_function = 
                _input_data_functions[stream_name][0];
            buf += string(
                    input_data_function->get_data_handle(), 
                    input_data_function->get_data_size());
            oss << " " << *((TCPPacket*)(input_data_function->get_data_handle()));

         //   buf += string((const char*)&int_timestamp, sizeof(int));
        }
        else
        {
            for (int i= 0; i < BRIOParams::get_singleton()->number_of_fields; ++i)
            {
                int f = static_cast<int>(
                        _input_data_functions[stream_name][i]->get_next_value());
                buf += string((const char*)&f, sizeof(int));
                oss << f << " ";
            }
        }
        event->insert_bin(buf);
        oss << ")" << endl;
        DEBUG << oss.str();
    }

}


BOREALIS_NAMESPACE_END



#include "BROutputReceiver.h"
#include "BRIOParams.h"
#include <fstream>
#include "Timeval.h"
#include <iomanip>

//#define DEBUG_BANDWIDTH_USAGE
//#define NOTICE_OUTPUT


BOREALIS_NAMESPACE_BEGIN
using namespace time_tools;

int BROutputReceiver::_number_of_fields = 0;

unsigned int BROutputReceiver::_number_of_output_tuples = 0;
unsigned int BROutputReceiver::_number_of_output_tuples_per_window = 0;
double BROutputReceiver::_total_latency_per_window = 0;
double BROutputReceiver::_ssq_latency_per_window = 0;
double BROutputReceiver::_max_latency_per_window = 0;
double BROutputReceiver::_min_latency_per_window = 100000;
ofstream BROutputReceiver::_latency_fs;


BROutputReceiver::BROutputReceiver(
        const string& cfg_filename,
        const InetAddress& client_address,
        double statistics_window_size,
        const string& output_file_prefix):
    _statistics_window_size(statistics_window_size)
{
    if (BRIOParams::get_singleton()->use_tcp_trace)
    {
        _number_of_fields = 3;
    }
    else
    {
        _number_of_fields = BRIOParams::get_singleton()->number_of_fields;
    }

    _client.set(new MedusaClient(InetAddress()));

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
        while (begin != "<subscribe" && !of.eof())
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

        InetAddress addr(node_address);
        subscribe_output(stream_name, node_address, client_address);
    }

    if (pthread_create(
                &_thread,
                0,
                (void*(*)(void*))run,
                (void*)&(_client->get_loop())) < 0)
    {
        WARN << "Can not start load manager thread.";
    }


    _last_time_stamp = Timeval::now().double_value();

    if (output_file_prefix != "")
    {
        _avg_latency_fs.open((output_file_prefix + "avg_latency.dat").c_str(), ios::out);
        if (!_avg_latency_fs)
        {
            cerr << "can't open output file: " 
                 << output_file_prefix << "avg_latency.dat" << endl;
            exit(1);
        }

        _latency_fs.open((output_file_prefix + "latency.dat").c_str(), ios::out);
        if (!_latency_fs)
        {
            cerr << "can't open output file: " 
                 << output_file_prefix << "latency.dat" << endl;
            exit(1);
        }
    }


    (new CallbackTimer(_client->get_loop(),
                       wrap( this, &BROutputReceiver::record_statistics)))
           ->arm( Time::secs(_last_time_stamp +  2 * _statistics_window_size ));
}

void BROutputReceiver::join()
{
    if (pthread_join(_thread, 0)) 
    { 
        WARN << "Can not join the BROutputReceiver thread";
    } 
}

void  BROutputReceiver::subscribe_output(
        const string& stream_name,
        const string& node_address,
        const InetAddress& client_address)
{
    NOTICE << "Subscribing to output steam " << stream_name
                  << " on node " << node_address;

    // Setup the subscription request.
    Status  stat = _client->set_data_handler(
           client_address,
           wrap( &BROutputReceiver::output_handler ));

    if (stat)
        NOTICE << "Sucessful.";
    else
        NOTICE << "Failed:" << stat;
}

Status BROutputReceiver::output_handler( ptr<StreamEvent>  event )
{

    // get received time
    double receive_time = Timeval::now().double_value();

    string stream_name = event->_stream.as_string();
    unsigned int     offset = 0;
    for (int index = 0; index < event->_inserted_count; ++index)
    {
        ostringstream oss;
        oss.clear();
        oss << "<" << stream_name << "> ";

        // check whether data from a new data stream has been received.
        // If so, output stream name.
        static set<string> received_streams;
        if (received_streams.count(stream_name) == 0)
        {
             received_streams.insert(stream_name);
             NOTICE << "Recieved data from: " << received_streams.size() 
                    << " streams";
        }

        TupleHeader* header = (TupleHeader*)&event->_bin_tuples[offset];

        //TupleHeader* header = (TupleHeader*)&event->_bin_tuples[offset];
        offset += sizeof(TupleHeader);

        // output data
        for (int i = 0; i < _number_of_fields; ++i)
        {
            int* f = (int*)&event->_bin_tuples[offset];
            offset += sizeof(int);
            oss << "(" << *f << ") ";
        }


        Timeval send_time(header->tv);
        int latency = (int)((receive_time - send_time.double_value()) * 1000);

        oss << "(" << latency << " ms)" << endl;

#ifdef NOTICE_OUTPUT
        NOTICE << oss.str();
#else
        DEBUG << oss.str();
#endif

        // record latency
//        if (BROutputReceiver::_latency_fs.is_open())
//        {
//            BROutputReceiver::_latency_fs << latency << endl;
//        }

        // update statistics
        ++_number_of_output_tuples_per_window;
        ++_number_of_output_tuples;
        _total_latency_per_window += latency;
        _ssq_latency_per_window += latency * latency;
        if (latency > _max_latency_per_window)
        {
            _max_latency_per_window = latency;
        }
        if (latency < _min_latency_per_window)
        {
            _min_latency_per_window = latency;
        }

    }


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

        cout << "Data Rate: " << data_rate << " Mbps"
             << "  Tuple Rate: " << tuple_rate  << " tuples/sec" << endl;
        data_bytes = 0;
        tuples = 0;
        last_tm = tm;
    }
    else {
        data_bytes += event->_bin_tuples.length();
        tuples += event->_inserted_count;
    }
#endif
    return true;
}

void BROutputReceiver::record_statistics()
{
    (new CallbackTimer( _client->get_loop(),
                        wrap( this, &BROutputReceiver::record_statistics)))
                            ->arm(Time::secs(_last_time_stamp +  _statistics_window_size ));
    _last_time_stamp += _statistics_window_size;


    if (_number_of_output_tuples == 0)
    {
        return;
    }

    static long timestamp = 1;

    if (_avg_latency_fs.is_open())
    {
        double avg_latency_per_window = 0.0;
        if (_number_of_output_tuples_per_window != 0)
        {
            avg_latency_per_window = 
                _total_latency_per_window / 
                _number_of_output_tuples_per_window;
        }
        _avg_latency_fs << setw(6) << setprecision(4) 
                        << avg_latency_per_window << " ";
        _avg_latency_fs << setw(6) << _max_latency_per_window << " ";
        _avg_latency_fs << setw(6) << _min_latency_per_window << " ";
        _avg_latency_fs << setw(8) << _number_of_output_tuples_per_window << " ";
        _avg_latency_fs << setw(10) << _total_latency_per_window << " ";
        _avg_latency_fs << setw(10) << _ssq_latency_per_window << " ";
        _avg_latency_fs << endl;
    }



    _number_of_output_tuples_per_window = 0;
    _total_latency_per_window = 0;
    _ssq_latency_per_window = 0;
    _max_latency_per_window = 0;
    _min_latency_per_window = 100000.0;

    ++timestamp;
}

BOREALIS_NAMESPACE_END



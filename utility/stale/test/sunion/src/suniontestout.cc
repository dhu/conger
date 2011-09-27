#include "suniontest.h"

using namespace Borealis;
using namespace NMSTL;

#define REDUCTION_FACTOR 10000000

// --------------------------------------------------
// In this applications, we measure tuple processing 
// times using their tv fields in their headers. This works only 
// in our specific query setup. This is not a general approach
// --------------------------------------------------
class SUnionTestOut : public SUnionTest
{

public:
    SUnionTestOut(int argc, const char* argv[], bool gui=false)
        : SUnionTest(argc,argv),
          _max_delay(0),
          _total_delay(0),
          _total_recent_tuples(0),
          _total_tentative(0),
          _total_tuples(0),
          _prev_tuple_type(TupleType::INSERTION),
          _prev_tuple_id(0),
          _gui(gui)
    {
        for ( int i = 0; i < _info._nb_streams; i++)
            _last_tuples[i] = 0;
    }
    void start();
    void finish();

private:
    Status print_event(ptr<StreamEvent> event);
    void update_stats(PacketTuple& p, Time current_time);

    long _max_delay;     // Max delay seen on latest tuples
    unsigned long long _total_delay; // To compute average delay for non-duplicate tuples
    long _total_recent_tuples;
    map<long,long> _last_tuples;
    long _total_tentative;

    long _total_tuples;
    Time _start_time;
    TupleType _prev_tuple_type;
    int32 _prev_tuple_id;


    // We are going to remember the delays of all the tuples so we can plot them
    // on a 2D graph.

    // We keep a list or pairs. Each pair is a tupleID followed by the processing delay of that tuple
    typedef list< pair< int, long long > > Pairs;

    // We keep one list of tuple per stream
    typedef map< int, Pairs > TupleDelays;
    TupleDelays _all_tuples;


    bool _gui;
};


// --------------------------------------------------
Status
SUnionTestOut::print_event(ptr<StreamEvent> event)
{

    static Time last_print_time = Time::now();

    DEBUG << "Got " << event->_inserted_count << " tuples";

    Time current_time = Time::now();
    long delta_time = (current_time - _start_time).to_msecs();

    // Total tuples includes boundary tuples
    _total_tuples += event->_inserted_count;
    if ( (current_time - last_print_time).to_msecs() > 2000 )
    {
        INFO << "Rate is " << _total_tuples*1000 / delta_time << " tuples/sec";
        last_print_time = current_time;
    }

    int index = 0;

    for ( int i = 0; i < event->_inserted_count; i++)
    {

        PacketTuple p = *(PacketTuple*)(&event->_bin_tuples[index]);

        index += event->_tuple_size;

        DEBUG << "Tuple is " << p;
        if ( p.tuple_type != TupleType::BOUNDARY)
        {
            if (p.tuple_type != _prev_tuple_type)
            {
                INFO << "Tuple is: " << p << "\n  while prev tuple was " << _prev_tuple_type << "," << _prev_tuple_id;
                _prev_tuple_type = p.tuple_type;
            }
            _prev_tuple_id = p.tuple_id;
        }

        if ( (p.tuple_type != TupleType::BOUNDARY) && (delta_time > 2000) )
        { update_stats(p,current_time);
        }

        // GUI stuff
        if ( _gui )
        {
            Time init_time(p.tv);
            int delay = (int)(Time::now() - init_time).to_msecs() % REDUCTION_FACTOR;
            if (delay < 0)
                delay += REDUCTION_FACTOR;
            cout << p.tuple_id << " " << p.tuple_type << " " << event->_stream << " " << 1
                 << " " << delay << endl;
        }

    }

    return true;
}

// --------------------------------------------------
void
SUnionTestOut::update_stats(PacketTuple& p, Time current_time)
{

    Time init_time(p.tv);
    long tuple_proc_delay = (current_time - init_time).to_msecs();

    // If we got a new tuple
    if ( p.tuple_id > _last_tuples[p._stream_id] )
    {

        _last_tuples[p._stream_id] = p.tuple_id;
        // If tuple comes from a stream without failure and max delay is greater that what we had before
        if ( (tuple_proc_delay > _max_delay) && ( (p._stream_id != 1) || (!_info._with_failure)) )
        {
            _max_delay = tuple_proc_delay;
            INFO << "XXX Max delay is " << _max_delay << " for tuple " << p.tuple_id;
        }

        // If tuple comes from a stream without failure update average delay
        if ( (p._stream_id != 1)  || (!_info._with_failure) )
        {
            _total_recent_tuples++;
            _total_delay += tuple_proc_delay;
        }

        // Any new tuple goes into the full list of tuples
        _all_tuples[p._stream_id].push_back(pair<int,long>(p.tuple_id,tuple_proc_delay));
    }

    // Count nb tentative tuples
    if ( p.tuple_type == TupleType::TENTATIVE)
    { _total_tentative++;
    }

}


// --------------------------------------------------
void
SUnionTestOut::start()
{
    // If we run with one node
    string output_stream = "medusa://nms.lcs.mit.edu/uniontest/query/output";

    // If we have 2 nodes one downstream from the other
    string output_stream2 = string("medusa://nms.lcs.mit.edu/uniontestmiddle/query/output/") + _info._nb_nodes;

    _start_time = Time::now();

    // -- Subscribe to output ---------------------------
    Status stat = _client.set_data_handler(InetAddress(_info._my_ip,_info._my_port),
                                           wrap(this,&SUnionTestOut::print_event));
    if ( stat )
    {
        // First try the bottom most
        NOTICE << "Subscribing to " << output_stream2;
        if ( ! _client.fast_subscribe(output_stream2))
        {
            INFO << "Failed subscribing to : " << output_stream2 << " so will subscribe to " << output_stream;
            if ( ! _client.fast_subscribe(output_stream) )
            {    FATAL << "Failed subscribing to " << output_stream << " so nothing to do...exiting";
            }
        }
    }
    NOTICE << "Done subscribing";

    SUnionTest::start();

}

// --------------------------------------------------
void SUnionTestOut::finish()
{
    INFO << "Total delay is " << _total_delay;
    INFO << "Total recent tuples is " << _total_recent_tuples;

    if ( _fd )
    {
        _fd << _info.dump_params() << " " << _max_delay << " " << (_total_delay/_total_recent_tuples) << " " << _total_tentative << endl;
    }

    int counter = 0;
    WARN << "Printing tuple delays";
    for ( TupleDelays::iterator i = _all_tuples.begin(); i != _all_tuples.end(); ++i)
    {
        WARN << "Printing tuple delays for stream " << counter;
        string file_name = string("tuple.delays.") + counter;
        ofstream tuple_delays_fd;
        tuple_delays_fd.open(file_name.c_str(),fstream::out);
        if ( tuple_delays_fd.is_open() )
        {
            Pairs& list = i->second;
            for ( Pairs::iterator j = list.begin(); j != list.end(); ++j)
            {
                tuple_delays_fd << j->first << " " << j->second << endl;
            }
        }
        counter++;
    }

    SUnionTest::finish();
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

        char* const gui_arguments[] = {"java","-cp", "SimpleGUI.jar", "SimpleGUI"};
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

    SUnionTestOut client(argc,argv,gui);
    client.start(); // Doesn't return

}

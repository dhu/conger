#include "args.h"
#include "SunionsMarshal.h"
#include <fstream>

using namespace Borealis;

const uint32  SLEEP_TIME = 10;       // Delay between injections.
const int BOUNDARY_INTERVAL = 2;     // Make every 10th tuple a boundary (200msec boundary interval)

// Very crude setup. 
// Two experiments. Each experiment has a different failures
const int EXPERIMENT1 = 1;
const int EXPERIMENT2 = 2;
int experiment;

const int START_FAILURE1 = 600;
const int STOP_FAILURE1 = 1200;

const int START_FAILURE2_EXPERIMENT1 = 800;
const int STOP_FAILURE2_EXPERIMENT1 = 1600;

const int START_FAILURE2_EXPERIMENT2 = 1200;
const int STOP_FAILURE2_EXPERIMENT2 = 1600;


////////////////////////////////////////////////////////////////////////////////
/**
 * Sets tuple values to be monotonically increasing
 */
class TupleValues
{

public:
    TupleValues() : _counter(0) {}
    
    void set_value(int32& value, timeval& stime)
    {
        value = _counter;  
        _counter++;

        // Stime is set using real time
        timeval stime_value = Time::now().to_timeval();
        stime.tv_sec = stime_value.tv_sec;
        stime.tv_usec = stime_value.tv_usec;
    }

    bool set_boundary(int32 total_tuples, TupleType& tuple_type)
    {

        if (( (total_tuples % BOUNDARY_INTERVAL) == 0 ))
        { 
            tuple_type = TupleType::BOUNDARY;
            return true;
        }
        return false;

        
    }

private:
    int32 _counter; // Some number of miliseconds

};

// Making it global so all the methods to set tuple values can call it
TupleValues tuple_values;


////////////////////////////////////////////////////////////////////////////////
struct TupleInfo
{
    TupleInfo(Timestamp time, int32 v, TupleType t)
        : _arrival_time(time), _value(v), _type(t) {}
    
    Timestamp _arrival_time;
    int32 _value;
    TupleType _type;    
};
   
// Results from experiment
static list<TupleInfo> all_tuples;
Timestamp failure1_start, failure2_start, failure1_stop, failure2_stop;


////////////////////////////////////////////////////////////////////////////////
//
void  SunionsMarshal::receivedControloutput1( EmptyTuple  *tuple ) { return; }
void  SunionsMarshal::receivedControloutput2( EmptyTuple  *tuple ) { return; }
void  SunionsMarshal::receivedControloutput3( EmptyTuple  *tuple ) { return; }
void  SunionsMarshal::receivedControlunion1( EmptyTuple  *tuple ) { return; }
void  SunionsMarshal::receivedControlunion2( EmptyTuple  *tuple ) { return; }
void  SunionsMarshal::receivedControlunion3( EmptyTuple  *tuple ) { return; }
void  SunionsMarshal::receivedSoutput1( TrivialTuple  *tuple ) {}
void  SunionsMarshal::receivedSoutput2( TrivialTuple  *tuple ) {}




////////////////////////////////////////////////////////////////////////////////
void  SunionsMarshal::receivedSoutput3( TrivialTuple  *tuple, TupleType type )
{    
    if ( type != TupleType::BOUNDARY)
    {
        all_tuples.push_back( TupleInfo(Timestamp::now(), tuple->value, type) );
    }
    
}

void SunionsMarshal::schedule_exit(int32 duration)
{

    // Reschedule authorization request
    ( new CallbackTimer( _client->get_loop(),
                         wrap( this, &SunionsMarshal::write_output )))
        ->arm( Time::now() + Time::msecs( duration ));
 

}

////////////////////////////////////////////////////////////////////////////////
void SunionsMarshal::write_output()
{
    ofstream fd;

    if ( experiment == EXPERIMENT1 )
    { fd.open("results1.dat");
    } 
    else 
    { fd.open("results2.dat");
    }

    if ( !fd.is_open() )
    { FATAL << "Cannot write output results... exiting";
    }

    WARN << "Printing tuple delays";

    Timestamp first_time = all_tuples.begin()->_arrival_time;

    fd << "# Failure 1 start time " << (failure1_start - first_time).to_msecs() << endl;
    fd << "# Failure 1 stop  time " << (failure1_stop  - first_time).to_msecs() << endl;
    fd << "# Failure 2 start time " << (failure2_start - first_time).to_msecs() << endl;
    fd << "# Failure 2 stop  time " << (failure2_stop  - first_time).to_msecs() << endl;
   
    fd << "# Column 1: Arrival time";
    fd << "# Column 2: Tuple value";
    fd << "# Column 3: Tuple type";

    for ( list<TupleInfo>::iterator i = all_tuples.begin(); i != all_tuples.end(); ++i)
    {
        fd << (i->_arrival_time - first_time).to_msecs() << " " 
           << i->_value << " " 
           << i->_type << "\n";
    }

    fd.close();
    exit(0);
}


////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a bunch of tuples and a delay.
//
void  SunionsMarshal::sentInput1()
{
    static int total_tuples_sent = 0; 
    static bool suspend_boundaries = false;
    static bool already_failed = false;
    static bool already_ended_failure = false;

    total_tuples_sent++;
    Input1 tuple;

    tuple_values.set_value(tuple._data.value,tuple.tuple_stime);
    batchInput1( &tuple );

    // And add a boundary tuple from time to time
    if ( ! suspend_boundaries)
    { 
        if ( tuple_values.set_boundary(total_tuples_sent,tuple.tuple_type))
        { batchInput1( &tuple );
        }
    }

    if ( ( (total_tuples_sent % START_FAILURE1) == 0 )  && !already_failed)
    { 
        suspend_boundaries = true;
        already_failed = true;
        WARN << "Starting FAILURE";
        failure1_start = Timestamp::now();
    }

    if ( ((total_tuples_sent % STOP_FAILURE1) == 0) && !already_ended_failure )
    { 
        already_ended_failure = true;
        suspend_boundaries = false;
        WARN << "Stopping FAILURE";
        failure1_stop = Timestamp::now();
    }
    
    sendInput1( SLEEP_TIME );
    return;
}

void  SunionsMarshal::sentInput2()
{
    static int total_tuples_sent = 0;    
    total_tuples_sent++;
    Input2 tuple;
    tuple_values.set_value(tuple._data.value,tuple.tuple_stime);
    batchInput2( &tuple );

    if ( tuple_values.set_boundary(total_tuples_sent,tuple.tuple_type))
    { batchInput2( &tuple);
    }
    sendInput2( SLEEP_TIME );
    return;
}
void  SunionsMarshal::sentInput3()
{
    static int total_tuples_sent = 0;
    static bool suspend_boundaries = false;
    static bool already_failed = false;
    static bool already_ended_failure = false;

    total_tuples_sent++;
    Input3 tuple;
    tuple_values.set_value(tuple._data.value,tuple.tuple_stime);
    batchInput3( &tuple );

    if ( ! suspend_boundaries )
    { 
        if (tuple_values.set_boundary(total_tuples_sent,tuple.tuple_type))
        { batchInput3(&tuple);
        }
    }

    int start_failure_time = (experiment == EXPERIMENT1) ? START_FAILURE2_EXPERIMENT1 : START_FAILURE2_EXPERIMENT2;
    int stop_failure_time  = (experiment == EXPERIMENT1) ? STOP_FAILURE2_EXPERIMENT1 : STOP_FAILURE2_EXPERIMENT2;

    if ( ( (total_tuples_sent % start_failure_time) == 0 )  && !already_failed)
    { 
        suspend_boundaries = true;
        already_failed = true;
        WARN << "Starting FAILURE";
        failure2_start = Timestamp::now();
    }

    if ( ( (total_tuples_sent % stop_failure_time) == 0 ) && ! already_ended_failure )
    { 
        already_ended_failure = true;
        suspend_boundaries = false;
        WARN << "Stopping FAILURE";
        failure2_stop = Timestamp::now();
    }

    sendInput3( SLEEP_TIME );
    return;
}





////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    string  ip   = "127.0.0.1";     // client host ip.
    int     port = 15000;           // client (and input) port.

    int32           status;
    SunionsMarshal   marshal;        // Client and I/O stream state.
    //
    // Maximum size of buffer with data awaiting transmission to Borealis
    //..............................................................................
   
    //stdArgs( argc, argv, ip, port );

    // Very simple arguments: "1" means run experiment1 and "2" means run experiment2
    experiment = EXPERIMENT1;
    if ( argc > 1 )
    { experiment = atoi(argv[1]);        
    }

    INFO << "Running experiment " << experiment;

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open( ip, port );

    if ( status )
    {   
        WARN << "Could not deply the network.";
    }
    else
    {   
        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentInput1();
        marshal.sentInput2();
        marshal.sentInput3();

        //DEBUG  << "run the client event loop...";
        marshal.schedule_exit(30000);

        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return( status );
}

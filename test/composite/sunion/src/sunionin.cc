#include "QueryMarshal.h"
#include "SUnionTestParams.h"
#include <fstream>

using namespace Borealis;

const uint32  SLEEP_TIME = 20;  // Delay between injections.
int BOUNDARY_INTERVAL = 10;     // Make every 10th tuple a boundary (200msec boundary interval)
int BATCH_SIZE = 1;

// Actual failure tims is START_FAILURE*SLEEP_TIME
int START_FAILURE = 350;
int STOP_FAILURE = 650;

int START_FAILURE2 = 350;
int STOP_FAILURE2 = 650;

const int NB_STREAMS = 3;


////////////////////////////////////////////////////////////////////////////////
//
void  QueryMarshal::receivedOutput( TrivialTuple  *tuple )
{
    // Watch out, we do not call this method anymore
    //NOTICE << "Tuple: " << tuple->value;
}


////////////////////////////////////////////////////////////////////////////////
void QueryMarshal::write_output()
{
    ofstream fd;

    fd.open(_params._stats_file.c_str(),ios::app);
    
    if ( !fd.is_open() )
    { FATAL << "Cannot write output results... exiting";
    }

    fd << _params.dump_params() 
       << " Max_delay(msecs): " << _max_proc_time.to_msecs() 
       << " Nb_tentative " << _nb_tentative 
       << " Reconciliation_time(msecs) " << (_recovery_stop_time - _recovery_start_time).to_msecs()
       << endl;


    fd.close();
    WARN << "Wrote results to file " << _params._stats_file;
    exit(0);

}

////////////////////////////////////////////////////////////////////////////////
void QueryMarshal::schedule_exit(int32 duration)
{

    // Reschedule authorization request
    ( new CallbackTimer( _client->get_loop(),
                         wrap( this, &QueryMarshal::write_output)))
        ->arm( Time::now() + Time::msecs( duration ));

}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a bunch of tuples and a delay.
//
void  QueryMarshal::sentInput1_failure(Timestamp start, int iteration_number)
{
    static bool suspend_boundaries = false;
    static bool already_failed = false;

    iteration_number++;
    Input1 tuple;

    for ( int i = 0; i < BATCH_SIZE; i++) 
    {
        int32 tuple_number = (iteration_number*BATCH_SIZE+i);
        int32 unique_id = tuple_number*NB_STREAMS;
        tuple._data.value = unique_id;
        tuple.tuple_id = unique_id;

        //Timestamp tuple_timestamp = start;
        //tuple_timestamp += Timestamp(0,iteration_number*SLEEP_TIME*1000);
        Timestamp tuple_timestamp = Timestamp::now();
        tuple.tuple_stime.tv_sec = tuple_timestamp.to_timeval().tv_sec;
        tuple.tuple_stime.tv_usec = tuple_timestamp.to_timeval().tv_usec + 10*i;
        
        batchInput1( &tuple );
    }

    // And add a boundary tuple from time to time
    if (  (!suspend_boundaries) && ( (iteration_number % BOUNDARY_INTERVAL) == 0 ) )
    { 
        tuple.tuple_type = TupleType::BOUNDARY;
        batchInput1( &tuple );
    }

    if ( ( (iteration_number % START_FAILURE) == 0 )  && !already_failed )
    { 
        suspend_boundaries = true;
        already_failed = true;
        WARN << "Starting FAILURE";
    }

    if ( ( iteration_number % STOP_FAILURE) == 0 )
    { 
        suspend_boundaries = false;
        WARN << "Stopping FAILURE";
    }
    
    sendInput1_failure( SLEEP_TIME, start, iteration_number );
    return;
}

////////////////////////////////////////////////////////////////////////////////
void  QueryMarshal::sentInput1(Timestamp start, int iteration_number)
{

    static bool suspend_boundaries = false;
    static bool already_failed = false;

    iteration_number++;
    Input1 tuple;

    for ( int i = 0; i < BATCH_SIZE; i++) 
    {
        int32 tuple_number = (iteration_number*BATCH_SIZE+i);
        int32 unique_id = tuple_number*NB_STREAMS;
        tuple._data.value = unique_id;
        tuple.tuple_id = unique_id;

        //Timestamp tuple_timestamp = start;
        //tuple_timestamp += Timestamp(0,iteration_number*SLEEP_TIME*1000);
        Timestamp tuple_timestamp = Timestamp::now();
        tuple.tuple_stime.tv_sec = tuple_timestamp.to_timeval().tv_sec;
        tuple.tuple_stime.tv_usec = tuple_timestamp.to_timeval().tv_usec + 10*i;

        batchInput1( &tuple );
    }

    if (  (!suspend_boundaries) && ( ( iteration_number % BOUNDARY_INTERVAL) == 0 ) )
    { 
        tuple.tuple_type = TupleType::BOUNDARY;
        batchInput1( &tuple );
    }

    // This is going to be a failure forever
    if ( ( ( iteration_number % START_FAILURE2) == 0 )  && !already_failed)
    { 
        suspend_boundaries = true;
        already_failed = true;
        WARN << "Starting FAILURE 2";
    }

    if ( ( iteration_number % STOP_FAILURE2 ) == 0 )
    { 
        suspend_boundaries = false;
        WARN << "Stopping FAILURE 2";
    }

    sendInput1( SLEEP_TIME, start, iteration_number );
    return;
}

////////////////////////////////////////////////////////////////////////////////
void  QueryMarshal::sentInput2(Timestamp start, int iteration_number)
{

    iteration_number++;

    Input2 tuple;

    for ( int i = 0; i < BATCH_SIZE; i++) 
    {
        int32 tuple_number = (iteration_number*BATCH_SIZE+i);
        int32 unique_id = tuple_number*NB_STREAMS+1;
        tuple._data.value = unique_id;
        tuple.tuple_id = unique_id;

        //Timestamp tuple_timestamp = start;
        //tuple_timestamp += Timestamp(0,iteration_number*SLEEP_TIME*1000);
        Timestamp tuple_timestamp = Timestamp::now();
        tuple.tuple_stime.tv_sec = tuple_timestamp.to_timeval().tv_sec;
        tuple.tuple_stime.tv_usec = tuple_timestamp.to_timeval().tv_usec + 10*i;

        batchInput2( &tuple );
    }


    if ( ( iteration_number % BOUNDARY_INTERVAL) == 0 )
    { 
        tuple.tuple_type = TupleType::BOUNDARY;
        batchInput2( &tuple );
    }

    sendInput2( SLEEP_TIME, start, iteration_number );
    return;
}

////////////////////////////////////////////////////////////////////////////////
void  QueryMarshal::sentInput3(Timestamp start, int iteration_number)
{

    iteration_number++;
    Input3 tuple;

    for ( int i = 0; i < BATCH_SIZE; i++) 
    {
        int32 tuple_number = (iteration_number*BATCH_SIZE+i);
        int32 unique_id = tuple_number*NB_STREAMS+2;
        tuple._data.value = unique_id;
        tuple.tuple_id = unique_id;

        // Timestamp tuple_timestamp = start;
        //tuple_timestamp += Timestamp(0,iteration_number*SLEEP_TIME*1000);
        Timestamp tuple_timestamp = Timestamp::now();
        tuple.tuple_stime.tv_sec = tuple_timestamp.to_timeval().tv_sec;
        tuple.tuple_stime.tv_usec = tuple_timestamp.to_timeval().tv_usec + 10*i;

        batchInput3( &tuple );

    }


    if ( ( iteration_number % BOUNDARY_INTERVAL) == 0 )
    { 
        tuple.tuple_type = TupleType::BOUNDARY;
        batchInput3( &tuple );
    }

    sendInput3( SLEEP_TIME, start, iteration_number );
    return;
}


////////////////////////////////////////////////////////////////////////////////
class DataSource : public Thread
{
public:

    DataSource(string ip, int port, Timestamp start,  SUnionTestParams params = SUnionTestParams())
        : _marshal(ip,port,params), _start_time(start), _ip(ip), _port(port), _params(params)
    {
    }

    void deployDiagram(string diagram_file)
    {
        _marshal.deployDiagram(diagram_file);
    }

    virtual void run()
    { 
        WARN << "Starting a client ";
        _marshal.runClient();
    }

protected:
    QueryMarshal _marshal;
    Timestamp    _start_time;
    string       _ip;
    int          _port;
    SUnionTestParams _params;
};


////////////////////////////////////////////////////////////////////////////////
class DataSource1 : public DataSource
{
public:
    DataSource1(string ip, int port, Timestamp start,  SUnionTestParams params = SUnionTestParams())
        : DataSource(ip,port,start,params)
    {       
    }

    virtual void run()
    {
        _marshal.openInput1(_ip,_port);
        _marshal.openInput2(_ip,_port);
        _marshal.openInput3(_ip,_port);
        _marshal.sentInput1_failure(_start_time,0);
        _marshal.sentInput2(_start_time,0);
        _marshal.sentInput3(_start_time,0);
        
        if ( _params._valid )
        { _marshal.schedule_exit(_params._run_time);            
        }
 
        _marshal.runClient();
    }
    
};

////////////////////////////////////////////////////////////////////////////////
class DataSource2 : public DataSource
{
public:
    DataSource2(string ip, int port, Timestamp start,  SUnionTestParams params = SUnionTestParams())
        : DataSource(ip,port,start, params)
    {      
    }   

    virtual void run()
    {
        WARN << "Opening input path to: " << _ip << ":" << _port;
        _marshal.openInput1(_ip,_port);
        _marshal.openInput2(_ip,_port);
        _marshal.openInput3(_ip,_port);
        _marshal.sentInput1(_start_time,0);
        _marshal.sentInput2(_start_time,0);
        _marshal.sentInput3(_start_time,0);
        
        QueryMarshal::_replicating = true;

        _marshal.runClient();
    }

};

////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    //string  ip         = "128.30.76.89";   
    string  ip           = "127.0.0.1";   
    ip = Util::get_host_address( ip );

    int     port         = 17100;         

    //string  ip_replica   = "128.30.76.89";
    string  ip_replica   = "127.0.0.1";   
    ip_replica = Util::get_host_address( ip );
    
    int     port_replica = 17200;         


    // Initialize parameters for experiment from runtime arguments (which specify a file)
    SUnionTestParams parameters(argc,argv);

    Timestamp start = Timestamp::now();
    string filename = "Deployment.xml";
    if ( parameters._valid)
    { filename = "DeployVar.xml";
    }
    DataSource1 ds1(ip,port,start,parameters);
    DataSource2 ds2(ip_replica,port_replica,start,parameters);

    ds1.deployDiagram(filename);

    if ( parameters._valid )
    {
        
        // Failure starts at time 15 seconds
        START_FAILURE = 15000/SLEEP_TIME;
        STOP_FAILURE   = START_FAILURE + (parameters._failure_duration/SLEEP_TIME);

        // Want to make sure that clients stick with the original node, then as the 
        // original reconciles, they go to replica for most recent info and use original as background corrections
        START_FAILURE2 = START_FAILURE - 2;
        STOP_FAILURE2  = STOP_FAILURE + 2;
        WARN << "Start failure at iteration " << START_FAILURE     << " --> " << START_FAILURE*SLEEP_TIME << " msecs";
        WARN << "Stop  failure at iteration " << STOP_FAILURE      << " --> " << STOP_FAILURE*SLEEP_TIME << " msecs";

        //BOUNDARY_INTERVAL = parameters._delta_boundary/SLEEP_TIME;
        BOUNDARY_INTERVAL = 1; // Just send a boundary with every batch
        WARN << "Boundary interval is " << BOUNDARY_INTERVAL;
        WARN << "Boundary every iteration   " << BOUNDARY_INTERVAL << " --> " << BOUNDARY_INTERVAL*SLEEP_TIME << " msecs";

        BATCH_SIZE = parameters._tuple_rate*SLEEP_TIME/1000;
        WARN << "Batch size is " << BATCH_SIZE;

        ds1.start();

        if ( parameters._replica_string == SUnionTestParams::REPLICA_STRING)
        {            
            WARN << "Will also send data to replica";
            ds2.start();
        }
    }

    // The basic example without any fancy parameters
    else
    {
        ds1.start();
        ds2.start();
    }

    ds1.join();
}

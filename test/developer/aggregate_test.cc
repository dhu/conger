#include <iostream>
#include <fstream>

#include "args.h"
#include "Aggregate_TestMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 100;          // Delay between injections.
const uint32  BATCH_SIZE = 1;            // Number of input tuples per batch.

const Time  time0 = Time::now() - Time::msecs( 100 );

#define   DATA_FILE  "../../../data/test/aggregate_test.data"
#define   TEMP_BUF   4096

ifstream  DataFile;


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  Aggregate_TestMarshal::receivedOutput( OutputTuple  *tuple )
{
    //..............................................................................


    NOTICE << "Received Tuple ( time=" << tuple->time
           << ", value=" << tuple->value << " )";

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Read one line from input file if possible.
//  Throws true when nothing left to read.
//
Aggregate_TestMarshal::Input  read_one_tuple()
    throw( bool )
{
    Aggregate_TestMarshal::Input  p;
    char      buf[ TEMP_BUF ];
    //..............................................................................


    if ((  DataFile.good()  &&
           ( !DataFile.getline( buf, TEMP_BUF ).eof() )))
    {
        string time   = string( strtok( buf,  " " ));
        string quote  = string( strtok( NULL, " " ));
        string price  = string( strtok( NULL, "\n" ));

        p._data.time  = atoi( time.c_str() );
        p._data.price = atoi( price.c_str() );
        Aggregate_TestMarshal::setStringField( quote, p._data.quote, 4 );

        DEBUG << "Read time=" << p._data.time
              << " price=" << p._data.price
              << " quote=" << p._data.quote;
    }
    else
    {   throw( true );
    }

    return( p );
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  Aggregate_TestMarshal::sentInput()
{
    int32        timestamp;
    Time         current_time;
    //..............................................................................


    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    DEBUG  << "timestamp = " << timestamp << "  current_time = " << current_time;

    for ( uint32  i = 0; i < BATCH_SIZE; i++ )
    {   try
        {   Input  tuple = read_one_tuple();

            DEBUG  << "time=" << tuple._data.time
                   << "  price=" << tuple._data.price
                   << "  quote=" << tuple._data.quote;

            batchInput( &tuple );
        }
        catch( bool  end_of_data )
        {   WARN << "End of input data";
            return;
        }
    }

    // Send a batch of tuples and delay.
    //
    //DEBUG << "call sendInput...";
    sendInput( SLEEP_TIME );

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    string  ip   = "127.0.0.1";     // client host ip.
    int     port = 15000;           // client (and input) port.

    string    file_name;
    int32     status;
    Aggregate_TestMarshal   marshal;        // Client and I/O stream state.
    //
    // Maximum size of buffer with data awaiting transmission to Borealis
    //..............................................................................


    // need input file with schema (type,id,time,quote,price)
    //
    if ( argc > 1 )
    {   file_name = string() + argv[1];
    }
    else
    {   file_name = DATA_FILE;
    }

    DataFile.open( file_name.c_str(), ios::in );

    if ( !DataFile || !DataFile.good() )
    {   ERROR << "Could not open:  " << file_name;
        ERROR << "Specify input file with tuples in the format:";
        ERROR << "    time quote price";

        exit( 1 );
    }

    NOTICE << "Using data in:  " << file_name;

    stdArgs( argc, argv, ip, port );   // Conflicts with file name???

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open();

    if ( status )
    {   WARN << "Could not deply the network.";
    }
    else
    {   DEBUG  << "time0 = " << time0;

        // Send the first batch of tuples.  Queue up the next round with a delay.
        marshal.sentInput();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return( status );
}

#include <iostream>
#include <fstream>

#include "args.h"
#include "Upstreamdemo1Marshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 500;          // Delay between injections.
const uint32  BATCH_SIZE = 1;            // Number of input tuples per batch.
const uint32  PROTOCOL_SIZE = 4;         // Number of elements in PROTOCOL.

const string  PROTOCOL[] = { string( "dns" ),  string( "smtp" ),
                             string( "http" ), string( "ssh" )
};


const Time  time0 = Time::now() - Time::msecs( 100 );

#define   DATA_FILE  "../../../data/test/upstreamdemo.data"
#define   TEMP_BUF   4096

ifstream  DataFile;


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  Upstreamdemo1Marshal::receivedResult(  // PacketTuple  *tuple
                                           OutputTuple  *tuple
                                           )
{
    //..............................................................................


    // Hack:  Hard coded offsets into the tuple header.
    //
    int tid   = *(int *)( (int)tuple - HEADER_SIZE + 16 );
    int ttype = *(int *)( (int)tuple - HEADER_SIZE + 20 );
    int rev   = *(int *)( (int)tuple - HEADER_SIZE + 41 );
    int revid = *(int *)( (int)tuple - HEADER_SIZE + 24 );
    char quote[5];
    char rev_f[5];
    memset(quote, 0, 5);
    memset(rev_f, 0, 5);
    memcpy(quote, tuple->quote, 4);
    memcpy(rev_f, tuple->revised_value, 4);

    NOTICE << "\tOutput (tid=" << tid
           << ",ttype=" <<  (ttype == 0 ? "INSERTION"   :
                                        ttype == 1 ? "DELETION"    :
                                         ttype == 2 ? "REPLACEMENT" :
                                        ttype == 3 ? "HISTORICAL"  :
                                        ttype == 13? "NOTIFICATION" :
                                        ttype == 12? "REQUEST" :
                                        ttype == 11? "DELIMITER"   : "UNKNOWN")
         << ",revison_id=" << revid
           << ",rev_field=" << rev
           <<  ",time=" << tuple->time
           << ",quote=" << quote
           << ",price=" << tuple->price
           //<< ",agg_0=" << tuple->agg_0
           //<< ",foo=" << tuple->foo
           //<< ",bar=" << tuple->bar
           << ",revised_value=" << *(int *)rev_f << ")";
    //INFO << (char)getchar() << (char)8;
    return;
}

/*
void Demo1Marshal::receivedResult2( PacketTuple *tuple )
{
    receivedResult( tuple );
    NOTICE << "    to output Result2";
}

void Demo1Marshal::receivedResult3( PacketTuple *tuple )
{
    receivedResult( tuple );
    NOTICE << "    to output Result3";
}
*/

////////////////////////////////////////////////////////////////////////////////
//
//  Read one line from input file if possible
//  Throws true when nothing left to read
//
Upstreamdemo1Marshal::Packet  read_one_tuple()
    throw( bool )
{
    Upstreamdemo1Marshal::Packet  p;
    char      buf[ TEMP_BUF ];
    //..............................................................................


    if ((  DataFile.good()  &&
           ( !DataFile.getline( buf, TEMP_BUF ).eof() )))
    {
        string tuple_id    = string( strtok( buf,  "," ));
        string tuple_type  = string( strtok( NULL, "," ));
        string revi_id      = string( strtok( NULL, "," ));
        string revised     = string( strtok( NULL, "," ));
        string timex       = string( strtok( NULL, "," ));
        string quote       = string( strtok( NULL, "," ));
        string price       = string( strtok( NULL, "," ));
        string revised_val = string( strtok( NULL, "," ));

        p.tuple_id   = atoi( tuple_id.c_str() );
        p.tuple_type = (TupleType::Type)atoi( tuple_type.c_str() );
        p.rev_id = atoi( revi_id.c_str() );
        p.rev_field = atoi( revised.c_str() );

        p._data.time  = atoi( timex.c_str() );
        p._data.price = atoi( price.c_str() );
        Upstreamdemo1Marshal::setStringField( quote, p._data.quote, 4 );
//      Demo1Marshal::setStringField( revised_val, p._data.revised_field, 4 );
        *((int *)(p._data.revised_value)) = atoi( revised_val.c_str() );
        p.rev_id = -1;

        NOTICE << "Input (tid=" << p.tuple_id
              << ",type=" << ((p.tuple_type) == TupleType::INSERTION ? "INSERTION"   :
                             (p.tuple_type) == TupleType::DELETION ? "DELETION"    :
                             (p.tuple_type) == TupleType::REPLACEMENT ? "REPLACEMENT" :
                            (p.tuple_type) == TupleType::HISTORICAL ? "HISTORICAL"  :
                             (p.tuple_type) == TupleType::DELIMITER ? "DELIMITER"   :
                             (p.tuple_type) == TupleType::NOTIFICATION ? "NOTIFICATION" :
                             (p.tuple_type) == TupleType::REQUEST ? "REQUEST" : "UNKNOWN")
              << ",rev_id=" << p.rev_id
              << ",revised=" << p.rev_field
              << ",time=" << p._data.time
              << ",quote=" << p._data.quote[0] << p._data.quote[1]
                           << p._data.quote[2] << p._data.quote[3]
              << ",price=" << p._data.price
              << ",revised_value=" //<< p._data.revised_field[0]
                                   //<< p._data.revised_field[1]
                                   //<< p._data.revised_field[2] 
                                   //<< p._data.revised_field[3]
                                   << *(int *)p._data.revised_value << ")";
        INFO << (char)getchar() << (char)8;
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
void  Upstreamdemo1Marshal::sentPacket()
{
    int32     timestamp;
    Time      current_time;
    //..............................................................................


    current_time = Time::now();

    timestamp = (int32)( current_time - time0 ).to_secs();
    if ( timestamp < 0 ) timestamp = 0;
    //DEBUG  << "timestamp = " << timestamp << "  current_time = " << current_time;

    for ( uint32  i = 0; i < BATCH_SIZE; i++ )
    {   try
        {
            Packet  tuple = read_one_tuple();

            DEBUG  << "time=" << tuple._data.time
                   << "  price=" << tuple._data.price;

            batchPacket( &tuple );
        }
        catch ( bool end_of_data )
        {   WARN << "End of input data";
            return;
        }
    }

    // Send a batch of tuples and delay.
    //
    //DEBUG << "call sendPacket...";
    sendPacket( SLEEP_TIME );

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
    Upstreamdemo1Marshal   marshal;        // Client and I/O stream state.
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
        ERROR << "    tuple-id,tuple-type,time,quote,price";

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
        marshal.sentPacket();

        DEBUG  << "run the client event loop...";
        // Run the client event loop.  Return only on an exception.
        marshal.runClient();
    }

    return( status );
}

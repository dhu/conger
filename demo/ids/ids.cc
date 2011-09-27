#include "args.h"
#include "IdsMarshal.h"
#include "Diagram.h"

#include <iostream>
#include <fstream>
#include <string.h>

using namespace Borealis;

// XXX These should not be global but should belong to a class
const int PROT_SIZE = 16;
const int STATE_SIZE = 16;
const int FLAGS_SIZE = 16;
const int ADDR_ELEMENTS = 4;
const int BATCH_SIZE = 50;
const int SLEEP_TIME = 10;
const int INIT_VAL = -1;
const int TEMP_BUF = 4096;

// XXX These should not be global but should belong to a class
ifstream input_source;
double factor = 1.0;
Time start_time = Time::now();
long tuple_start_time = INIT_VAL;


////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  IdsMarshal::receivedSuspiciousconnections( Suspicious_schema  *tuple )
{

    NOTICE << "Suspicious remote IP addr " << "\x1B[7m" << tuple->remote_add_1 << "."
           << tuple->remote_add_1 << "." << tuple->remote_add_2 << "." << tuple->remote_add_3 << "\x1B[0m";
    NOTICE << "For time interval starting at "
           << tuple->start_time_sec << " number of protocols was " << tuple->agg_0;


}

// --------------------------------------------------
void  IdsMarshal::receivedSuspiciousports( Suspicious_schema  *tuple )
{

    NOTICE << "Suspicious remote IP addr " << "\x1B[7m" << tuple->remote_add_1 << "."
           << tuple->remote_add_1 << "." << tuple->remote_add_2 << "." << tuple->remote_add_3 << "\x1B[0m";
    NOTICE << "For time interval starting at "
           << tuple->start_time_sec << " number of ports was " << tuple->agg_0;


}

// --------------------------------------------------
void parse_remote (string remote, int remote_add[])
{

    // add_prefix is a parameter in the original application.
    static int add_prefix[] = { 18, 31, 1, 1 };

    char* p[ADDR_ELEMENTS];
    p[0] = strtok((char*)remote.c_str(),".");
    remote_add[0] = add_prefix[0];
    for ( int i = 1; i < ADDR_ELEMENTS; i++)
    {
        p[i] = strtok(NULL,".");
        remote_add[i] = add_prefix[i];
    }

    int index = ADDR_ELEMENTS-1;
    for ( int j = ADDR_ELEMENTS-1; j >= 0; j--)
    {
        if ( (p[j] != NULL ) )
        {
            remote_add[index] = atoi(p[j]);
            index--;
        }
    }
}


// --------------------------------------------------
/*
 * Throws true when nothing left to read
 */
IdsMarshal::Pre_Long_Packet_Header read_one_tuple() throw(bool)
{

    // Read one line from input file if possible
    char buf[TEMP_BUF];
    if ( input_source.good() && ( ! input_source.getline(buf,TEMP_BUF).eof()) )
    {

        // Get temp_start_time
        char* pch;
        pch = strtok(buf," ");
        double temp_start_time = atof(pch);
        int start_time_sec = (int)temp_start_time;
        int start_time_usec = (int)(USECS * (temp_start_time - start_time_sec));

        // Get duration, protocol, orig, rest bytes, local address, remote_add, state, and flags
        string duration = string(strtok (NULL," "));
        duration = ( duration == "?") ? "-1" : duration;
        string protocol = string(strtok (NULL," "));
        string orig_bytes = string(strtok (NULL," "));
        orig_bytes = ( orig_bytes == "?") ? "-1" : orig_bytes;
        string rest_bytes = string(strtok (NULL," "));
        rest_bytes = ( rest_bytes == "?") ? "-1" : rest_bytes;
        string local_add = string(strtok(NULL," "));
        string remote = string(strtok (NULL," "));
        int remote_add[ADDR_ELEMENTS];
        string state = string(strtok(NULL," "));
        string flags = string(strtok(NULL," "));

        parse_remote(remote,remote_add);
        IdsMarshal::Long_packet_header_schema p_data;
        p_data.start_time_sec = start_time_sec;
        p_data.start_time_usec = start_time_usec;
        p_data.duration = atof(duration.c_str()),
        memset(&p_data.protocol, 0, sizeof p_data.protocol);
        strncpy(p_data.protocol, protocol.c_str(), sizeof p_data.protocol);
        p_data.orig_bytes = atoi(orig_bytes.c_str());
        p_data.rest_bytes = atoi(rest_bytes.c_str());
        p_data.local_add = atoi(local_add.c_str());
        p_data.remote_add_1 = remote_add[0];
        p_data.remote_add_2 = remote_add[1];
        p_data.remote_add_3 = remote_add[2];
        p_data.remote_add_4 = remote_add[3];

        memset(&p_data.state, 0, sizeof p_data.state);
        strncpy(p_data.state, state.c_str(), sizeof p_data.state);

        memset(&p_data.flags, 0, sizeof p_data.flags);
        strncpy(p_data.flags, flags.c_str(), sizeof p_data.flags);

        IdsMarshal::Pre_Long_Packet_Header p;
        p._data = p_data;

        return p;
    } else
    {
        throw true;
    }
}


////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  IdsMarshal::sentPre_Long_Packet_Header()
{

    long tuple_current_time = INIT_VAL;

    //..............................................................................
    for (int i = 0; i < BATCH_SIZE; i++)
    {
        try
        {
            IdsMarshal::Pre_Long_Packet_Header tuple = read_one_tuple();

            batchPre_Long_Packet_Header( &tuple );
            if ( tuple_start_time == INIT_VAL )
                tuple_start_time = (long)(MSECS*tuple._data.start_time_sec / factor);
            tuple_current_time = (long)(MSECS*tuple._data.start_time_sec / factor);
        } catch (bool end_of_data)
        {
            WARN << "End of input data";
            return;
        }
    }

    // Send a batch of tuples and delay.
    //
    //DEBUG << "call sendPacket...";
    long tuple_time = tuple_current_time - tuple_start_time;
    long actual_time = (long)(Time::now()-start_time).to_msecs();
    long time_to_next_batch = tuple_time - actual_time;
    if (time_to_next_batch <= 0)
        time_to_next_batch = 1;

    sendPre_Long_Packet_Header( time_to_next_batch );

    return;
}




////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    string  ip   = "127.0.0.1";     // client host ip.
    int     port = DEFAULT_BOREALIS_PORT;           // client (and input) port.

    int32        status;
    IdsMarshal   marshal;        // Client and I/O stream state.

    //
    // Maximum size of buffer with data awaiting transmission to Borealis
    //..............................................................................


    stdArgs( argc, argv, ip, port );

    factor = 1.0;
    string file_name = "";

    int option_char;
    while ( (option_char = getopt (argc,(char**)argv,"r:f:")) != EOF )
    {
        switch (option_char)
        {
        case 'r':
            factor = atof(optarg);
            // Normally we adjust the parameters of the query network
            // for the speedup we cannot do that if
            // we use the new API... so good to leave old API around
            break;
        case 'f':
            file_name = optarg;
            break;
        default:
            break;
        }
    }

    // Run the front-end, open a client, subscribe to outputs and inputs.
    status = marshal.open();

    if ( status )
    {   WARN << "Could not deply the network.";
    }
    else
    {

        input_source.open(file_name.c_str(),ios::in);
        if ( input_source && input_source.good())
        {

            // Send the first batch of tuples.  Queue up the next round with a delay.
            marshal.sentPre_Long_Packet_Header();

            DEBUG  << "run the client event loop...";
            // Run the client event loop.  Return only on an exception.
            marshal.runClient();

        }
    }

    return( status );
}

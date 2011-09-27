#include <iostream>
#include <fstream>

#include "Validate_Upstream_RevisionaggregateMarshal.h"

using namespace Borealis;

const uint32  SLEEP_TIME = 500;          // Delay between injections.
const uint32  BATCH_SIZE = 1;            // Number of input tuples per batch.

const Time  time0 = Time::now() - Time::msecs( 100 );

Validate_Upstream_RevisionaggregateMarshal  marshal;        // Client and I/O stream state.

#define   DATA_FILE  "revisions_validation.data"
#define   TEMP_BUF   4096

ifstream  DataFile;

////////////////////////////////////////////////////////////////////////////////
//
// Print the content of received tuples.
//
void  Validate_Upstream_RevisionaggregateMarshal::receivedResult(OutputTuple  *tuple)
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

  NOTICE << "Output (tid=" << tid
	 << ",ttype=" <<  TupleType( (TupleType::Type)ttype )
	 << ",revison_id=" << revid
	 << ",rev_field=" << rev
	 << ",time=" << tuple->time
	 << ",quote=" << quote
	 << ",price=" << tuple->agg_0
	 << ",count=" << tuple->agg_1
	 << ",rev_val=" << *(int *)rev_f << ")";

  if (tid == 20)
    {
      NOTICE << "Test completed successfully.";
      NOTICE << "Terminating ...";
      marshal.terminateClient();
    }
     
  return;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Read one line from input file if possible
//  Throws true when nothing left to read
//
Validate_Upstream_RevisionaggregateMarshal::Packet  read_one_tuple()
  throw ( bool )
{
  Validate_Upstream_RevisionaggregateMarshal::Packet  p;
  char      buf[ TEMP_BUF ];
  //..............................................................................

  if (( DataFile.good()  &&
	( !DataFile.getline( buf, TEMP_BUF ).eof() )))
    {
      string tuple_id    = string( strtok( buf,  "," ));
      string tuple_type  = string( strtok( NULL, "," ));
      string rev_id      = string( strtok( NULL, "," ));
      string revised     = string( strtok( NULL, "," ));
      string timex       = string( strtok( NULL, "," ));
      string quote       = string( strtok( NULL, "," ));
      string price       = string( strtok( NULL, "," ));
      string revised_val = string( strtok( NULL, "," ));

      p.tuple_id   = atoi( tuple_id.c_str() );
      p.tuple_type = (TupleType::Type)atoi( tuple_type.c_str() );
      p.rev_id = atoi( rev_id.c_str() );
      p.rev_field = atoi( revised.c_str() );

      p._data.time  = atoi( timex.c_str() );
      p._data.price = atoi( price.c_str() );
      Validate_Upstream_RevisionaggregateMarshal::setStringField( quote, p._data.quote, 4 );
      *((int *)(p._data.revised_value)) = atoi( revised_val.c_str() );
      p.rev_id = -1;

      NOTICE << "Input (tid=" << p.tuple_id
	     << ",type=" << p.tuple_type
	     << ",rev_id=" << p.rev_id
	     << ",rev_field=" << p.rev_field
	     << ",time=" << p._data.time
	     << ",quote=" << p._data.quote[0] << p._data.quote[1]
	     << p._data.quote[2] << p._data.quote[3]
	     << ",price=" << p._data.price
	     << ",revised_val=" << *(int *)p._data.revised_value 
	     << ")";
      INFO << (char)getchar() << (char)8;
    }
  else
    {   
      throw( true );
    }

  return( p );
}



////////////////////////////////////////////////////////////////////////////////
//
// Return here after sending a packet and a delay.
//
void  Validate_Upstream_RevisionaggregateMarshal::sentPacket()
{
  int32     timestamp;
  Time      current_time;
  //..............................................................................

  current_time = Time::now();

  timestamp = (int32)( current_time - time0 ).to_secs();
  if ( timestamp < 0 ) 
    timestamp = 0;

  for ( uint32  i = 0; i < BATCH_SIZE; i++ )
    {   
      try
	{
	  Packet  tuple = read_one_tuple();
	  batchPacket( &tuple );
	}
      catch ( bool end_of_data )
	{   
	  WARN << "End of input data";
	  return;
	}
    }
  
  // Send a batch of tuples and delay.
  //
  sendPacket( SLEEP_TIME );

  return;
}



////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
  //..............................................................................

  // need input file with schema (tid,type,rev_id,rev_field,time,quote,price,revised_val)
  DataFile.open( DATA_FILE, ios::in );

  if ( !DataFile || !DataFile.good() )
    {   
      ERROR << "Could not open:  " << DATA_FILE;
      ERROR << "Input file with tuples must have the following format:";
      ERROR << "    tid,type,rev_id,rev_field,time,quote,price,revised_val";      
      exit( 1 );
    }

  NOTICE << "Using data in:  " << DATA_FILE;

  // Run the front-end, open a client, subscribe to outputs and inputs.
  int32 status = marshal.open();

  if ( status )
    {   
      WARN << "Could not deply the network.";
    }
  else
    {
      DEBUG  << "time0 = " << time0;

      // Send the first batch of tuples.  Queue up the next round with a delay.
      marshal.sentPacket();
      
      DEBUG  << "run the client event loop...";
      // Run the client event loop.  Return only on an exception.
      marshal.runClient();
    }

  return( status );

}

#include "LmtestMarshal.h"

using namespace Borealis;


const string ip   = "127.0.0.1";     // client host ip.
const int port = 15000;           // client (and input) port.

uint32 batch_size = 10;           // Number of input tuples per batch.
uint32 base_sleep_time = 100;


// send an random input
void  LmtestMarshal::sentSi_B1_P1()
{
  static long old_time = Time::now().to_usecs();
  static int count = 0;
  static uint32 sleep_time = base_sleep_time * 1;

  long now = Time::now().to_usecs();
  if (now - old_time > 1000000)
  {
    ++count;
    if (count == 10)
    {
      sleep_time /= 3;
    }
    if (count == 20)
    {
      sleep_time *= 3;
      count = 0;
    }
    old_time = now;
  }

  for (unsigned int i = 0; i < batch_size; ++i)
  {
    Si_B1_P1  tuple;
    tuple._data.f1 = (int)(drand48() * 10);
    tuple._data.f2 = (int)(drand48() * 10);
    batchSi_B1_P1( &tuple );
  }

  sendSi_B1_P1( sleep_time );
}

// send an random input
void  LmtestMarshal::sentSi_B3_P1()
{
  static long old_time = Time::now().to_usecs();
  static int count = 0;
  static uint32 sleep_time = base_sleep_time;

  long now = Time::now().to_usecs();
  if (now - old_time > 1000000)
  {
    ++count;
    if (count == 4)
    {
      sleep_time *= 1;
    }
    if (count == 8)
    {
      sleep_time /= 1;
      count = 0;
    }
    old_time = now;
  }

  for (unsigned int i = 0; i < batch_size; ++i)
  {
    Si_B3_P1  tuple;
    tuple._data.f1 = (int)(drand48() * 10);
    tuple._data.f2 = (int)(drand48() * 10);
    batchSi_B3_P1( &tuple );
  }

  sendSi_B3_P1( sleep_time );
}


// Print the content of received tuples.
void  LmtestMarshal::receivedSo_B4_P1( intstuple  *tuple )
{
  static int count = 0;
  static long old_time = Time::now().to_usecs();

  ++count;
  long now = Time::now().to_usecs();
  if (now - old_time > 1000000)
  {
    cout << "Stream rate of so_b4_p1 is " << count << "." << endl;
    old_time = now;
    count = 0;
  }

}

// Print the content of received tuples.
void  LmtestMarshal::receivedSo_B2_P1( intstuple  *tuple )
{
  static int count;
  static long old_time = Time::now().to_usecs();

  ++count;
  long now = Time::now().to_usecs();
  if (now - old_time > 1000000)
  {
    cout << "Stream rate of so_b4_p1 is " << count << "." << endl;
    old_time = now;
    count = 0;
  }
}


////////////////////////////////////////////////////////////////////////////////
//
int  main( int  argc, const char  *argv[] )
{
    LmtestMarshal   marshal;        // Client and I/O stream state.

    if (argc == 2)
      batch_size = atoi(argv[1]);

    /*
    INFO << "Sending: " << deploy_xml;
    string command = string() + "BigGiantHead " + query_xml + " " + deploy_xml;
    int32           status;
    status  = system( command.c_str() );

    if ( status )
    {
        WARN << "Could not deploy " + query_xml + " " + deploy_xml;
        return status;
    }
    */

    marshal.open();

    // Send the first batch of tuples.  Queue up the next round with a delay.
    marshal.sentSi_B1_P1();
    marshal.sentSi_B3_P1();

    DEBUG  << "run the client event loop...";
    // Run the client event loop.  Return only on an exception.
    marshal.runClient();

}

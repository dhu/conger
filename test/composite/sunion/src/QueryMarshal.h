#ifndef  QUERYMARSHAL_H
#define  QUERYMARSHAL_H

#include  "HeadClient.h"
#include  "Diagram.h"
#include  "MedusaClient.h"
#include  "TupleHeader.h"
#include  "Timestamp.h"
#include  "SUnionTestParams.h"


#define   HEAD_NODE   "localhost"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Query program.
//..............................................................................


using namespace Borealis;

class QueryMarshal
{
  public:

    QueryMarshal(string ip, uint32 port, SUnionTestParams params);
   ~QueryMarshal() {};

    /// Run the client event loop.
    /// This does not return unless there is an exception.
    ///
    void  runClient();

    /// Schedule the end of execution
    void schedule_exit(int32 duration);

    ///  Copy a string value to a fixed length array and zero fill.
    ///
    static void  setStringField( string  value,
                                   char  field[],
                                 uint32  length )
                          throw( AuroraException );

  public:

    struct TrivialTuple
    {
        int32    value;
    } __attribute__((__packed__));

    struct  Input1 : public TupleHeader
    {
        TrivialTuple   _data;
    } __attribute__((__packed__));

    void openInput1(string ip, uint32 port);

    /// The sentInput1  method must be defined by the application.
    /// It is called after sendInput1 is done and a pause.
    ///
    void  sentInput1(Timestamp start, int total_tuples_sent);
    void  sentInput1_failure(Timestamp start, int total_tuples_sent);

    /// Enque a Input1 for input.
    ///
    void  batchInput1( Input1  *tuple);

    /// Send enqued Input1 inputs.
    ///
    void  sendInput1( uint32  sleep, Timestamp start, int total_tuples_sent);
    void  sendInput1_failure( uint32  sleep, Timestamp start, int total_tuples_sent);

  private:

    ///  Resume here.  Extend with a user callback.
    void  delayInput1(Timestamp start, int total_tuples_sent);
    void  delayInput1_failure(Timestamp start, int total_tuples_sent);

  public:

    struct  Input2 : public TupleHeader
    {
        TrivialTuple   _data;
    } __attribute__((__packed__));

    void openInput2(string ip, uint32 port);

    /// The sentInput2  method must be defined by the application.
    /// It is called after sendInput2 is done and a pause.
    ///
    void  sentInput2(Timestamp start, int total_tuples_sent);

    /// Enque a Input2 for input.
    ///
    void  batchInput2( Input2  *tuple );

    /// Send enqued Input2 inputs.
    ///
    void  sendInput2( uint32  sleep, Timestamp start, int total_tuples_sent);

  private:

    /// Connect the Input2 input stream.
    ///
    void    connectInput2();

    ///  Resume here.  Extend with a user callback.
    void  delayInput2(Timestamp start, int total_tuples_sent);


  public:

    struct  Input3 : public TupleHeader
    {
        TrivialTuple   _data;
    } __attribute__((__packed__));

    void openInput3(string ip, uint32 port);

    /// The sentInput2  method must be defined by the application.
    /// It is called after sendInput2 is done and a pause.
    ///
    void  sentInput3(Timestamp start, int total_tuples_sent);

    /// Enque a Input2 for input.
    ///
    void  batchInput3( Input3  *tuple );

    /// Send enqued Input2 inputs.
    ///
    void  sendInput3( uint32  sleep, Timestamp start, int total_tuples_sent);

  private:

    /// Connect the Input2 input stream.
    ///
    void    connectInput3();

    ///  Resume here.  Extend with a user callback.
    void  delayInput3(Timestamp start, int total_tuples_sent);



  public:
    void deployDiagram(string filename);

  private:

    /// Handler to dispatch tuples received.
    ///
    static Status outputHandler( ptr<StreamEvent> event );


  public:

    struct EmptyTuple
    {
    } __attribute__((__packed__));

    /// The receivedControloutput method must be defined by the application.
    /// It is called after a(n) EmptyTuple is received.
    ///
    static void  receivedControloutput( EmptyTuple  *tuple );

  public:

    /// The receivedOutput method must be defined by the application.
    /// It is called after a(n) TrivialTuple is received.
    ///
    static void  receivedOutput( TrivialTuple  *tuple );

  private:

    /// Handler to receive tuples from the Output stream.
    ///
    static Status  OutputHandler( ptr<StreamEvent>  event );

  private:

    void write_output();

    /// Client connections to Borealis nodes.
    ///
    MedusaClient      *_client;


    /// Event state for input streams.
    /// These are declared with a smart pointer as fast_post_event requires it.
    ///
    ptr<StreamEvent>   _eventInput1;
    ptr<StreamEvent>   _eventInput2;
    ptr<StreamEvent>   _eventInput3;

    SUnionTestParams   _params;

    // These must be static because they are called from the static data handlers 
    static int _nb_tentative;
    static Timestamp _max_proc_time;
    static Timestamp _recovery_start_time;
    static Timestamp _recovery_stop_time;

 public:
    static bool _replicating;

};

#endif                         // QUERYMARSHAL_H

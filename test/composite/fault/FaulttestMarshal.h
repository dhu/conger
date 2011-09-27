#ifndef  FAULTTESTMARSHAL_H
#define  FAULTTESTMARSHAL_H

#include  "MedusaClient.h"
#include  "TupleHeader.h"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Faulttest program.
//..............................................................................


using namespace Borealis;

class FaulttestMarshal
{
  public:

    FaulttestMarshal() {};
   ~FaulttestMarshal() {};

    /// Activate the front-end and subscribe to streams.
    /// Returns 0 if okay; else an error occured.
    ///
    int32  open( string  ip,       // Local medusa client ip.
                 uint32  port );   // Local medusa client port.

    /// Connect to inputs and subscribe to streams only.
    /// Similar to open() but does not load the query.
    ///
    void  openInputOutput( string  ip,       // Local medusa client ip.
                           uint32  port );   // Local medusa client port.


    /// Run the client event loop.
    /// This does not return unless there is an exception.
    ///
    void  runClient();


    ///  Copy a string value to a fixed length array and zero fill.
    ///
    static void  setStringField( string  value,
                                   char  field[],
                                 uint32  length )
                          throw( AuroraException );


  public:

    struct TrivialTuple
    {
        int32    time;
        int32    join_attribute;
    } __attribute__((__packed__));

    struct  Trivial : public TupleHeader
    {
        TrivialTuple   _data;
    } __attribute__((__packed__));

    /// The sentTrivial  method must be defined by the application.
    /// It is called after sendTrivial is done and a pause.
    ///
    void  sentTrivial();

    /// Enque a Trivial for input.
    ///
    void  batchTrivial( Trivial  *tuple );

    /// Send enqued Trivial inputs.
    ///
    void  sendTrivial( uint32  sleep );

  private:

    /// Connect the Trivial input stream.
    ///
    void    connectTrivial();

    ///  Resume here.  Extend with a user callback.
    void  delayTrivial();

  public:

    struct  Trivial2 : public TupleHeader
    {
        TrivialTuple   _data;
    } __attribute__((__packed__));

    /// The sentTrivial2  method must be defined by the application.
    /// It is called after sendTrivial2 is done and a pause.
    ///
    void  sentTrivial2();

    /// Enque a Trivial2 for input.
    ///
    void  batchTrivial2( Trivial2  *tuple );

    /// Send enqued Trivial2 inputs.
    ///
    void  sendTrivial2( uint32  sleep );

  private:

    /// Connect the Trivial2 input stream.
    ///
    void    connectTrivial2();

    ///  Resume here.  Extend with a user callback.
    void  delayTrivial2();

  private:

    /// Handler to dispatch tuples received.
    ///
    static Status outputHandler( ptr<StreamEvent> event );


  public:

    struct EmptyTuple
    {
    } __attribute__((__packed__));

    /// The receivedControloutput1 method must be defined by the application.
    /// It is called after a(n) EmptyTuple is received.
    ///
    static void  receivedControloutput1( EmptyTuple  *tuple );

  private:

    /// Subscribe to the Controloutput1 output stream.
    ///
    void    subscribeControloutput1();

    /// Handler to receive tuples from the Controloutput1 stream.
    ///
    static Status  Controloutput1Handler( ptr<StreamEvent>  event );

  public:

    /// The receivedControloutput2 method must be defined by the application.
    /// It is called after a(n) EmptyTuple is received.
    ///
    static void  receivedControloutput2( EmptyTuple  *tuple );

  private:

    /// Subscribe to the Controloutput2 output stream.
    ///
    void    subscribeControloutput2();

    /// Handler to receive tuples from the Controloutput2 stream.
    ///
    static Status  Controloutput2Handler( ptr<StreamEvent>  event );

  public:

    /// The receivedControlunion1 method must be defined by the application.
    /// It is called after a(n) EmptyTuple is received.
    ///
    static void  receivedControlunion1( EmptyTuple  *tuple );

  private:

    /// Subscribe to the Controlunion1 output stream.
    ///
    void    subscribeControlunion1();

    /// Handler to receive tuples from the Controlunion1 stream.
    ///
    static Status  Controlunion1Handler( ptr<StreamEvent>  event );

  public:

    /// The receivedControlunion2 method must be defined by the application.
    /// It is called after a(n) EmptyTuple is received.
    ///
    static void  receivedControlunion2( EmptyTuple  *tuple );

  private:

    /// Subscribe to the Controlunion2 output stream.
    ///
    void    subscribeControlunion2();

    /// Handler to receive tuples from the Controlunion2 stream.
    ///
    static Status  Controlunion2Handler( ptr<StreamEvent>  event );

  public:

    /// The receivedControlunion3 method must be defined by the application.
    /// It is called after a(n) EmptyTuple is received.
    ///
    static void  receivedControlunion3( EmptyTuple  *tuple );

  private:

    /// Subscribe to the Controlunion3 output stream.
    ///
    void    subscribeControlunion3();

    /// Handler to receive tuples from the Controlunion3 stream.
    ///
    static Status  Controlunion3Handler( ptr<StreamEvent>  event );

  public:

    struct AggregateTuple
    {
        int32    time;
        int32    max;
    } __attribute__((__packed__));

    /// The receivedSoutput1 method must be defined by the application.
    /// It is called after a(n) AggregateTuple is received.
    ///
    static void  receivedSoutput1( AggregateTuple  *tuple );

  private:

    /// Subscribe to the Soutput1 output stream.
    ///
    void    subscribeSoutput1();

    /// Handler to receive tuples from the Soutput1 stream.
    ///
    static Status  Soutput1Handler( ptr<StreamEvent>  event );

  public:

    struct JoinTuple
    {
        int32    time1;
        int32    time2;
        int32    join_attribute;
    } __attribute__((__packed__));

    /// The receivedSoutput2 method must be defined by the application.
    /// It is called after a(n) JoinTuple is received.
    ///
    static void  receivedSoutput2( JoinTuple  *tuple );

  private:

    /// Subscribe to the Soutput2 output stream.
    ///
    void    subscribeSoutput2();

    /// Handler to receive tuples from the Soutput2 stream.
    ///
    static Status  Soutput2Handler( ptr<StreamEvent>  event );

  private:

    /// Launch the Borealis front-end.
    ///
    static int32  launchDiagram( string  xml,   // Path of an initial xml file.
                                 string  ip,    // Local medusa client ip.
                                 uint32  port   // Local medusa client port.
                                );

    /// Client connections to Borealis nodes.
    ///
    MedusaClient      *_client;


    /// Event state for input streams.
    /// These are declared with a smart pointer as fast_post_event requires it.
    ///
    ptr<StreamEvent>   _eventTrivial;
    ptr<StreamEvent>   _eventTrivial2;
};

#endif                         // FAULTTESTMARSHAL_H

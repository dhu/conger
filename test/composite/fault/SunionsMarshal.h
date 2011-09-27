#ifndef  SUNIONSMARSHAL_H
#define  SUNIONSMARSHAL_H

#include  "MedusaClient.h"
#include  "TupleHeader.h"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Sunions program.
//..............................................................................


using namespace Borealis;

class SunionsMarshal
{
  public:

    SunionsMarshal() {};
   ~SunionsMarshal() {};

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

    /// The sentInput1  method must be defined by the application.
    /// It is called after sendInput1 is done and a pause.
    ///
    void  sentInput1();

    /// Enque a Input1 for input.
    ///
    void  batchInput1( Input1  *tuple );

    /// Send enqued Input1 inputs.
    ///
    void  sendInput1( uint32  sleep );

  private:

    /// Connect the Input1 input stream.
    ///
    void    connectInput1();

    ///  Resume here.  Extend with a user callback.
    void  SunionsMarshal::delayInput1();

  public:

    struct  Input2 : public TupleHeader
    {
        TrivialTuple   _data;
    } __attribute__((__packed__));

    /// The sentInput2  method must be defined by the application.
    /// It is called after sendInput2 is done and a pause.
    ///
    void  sentInput2();

    /// Enque a Input2 for input.
    ///
    void  batchInput2( Input2  *tuple );

    /// Send enqued Input2 inputs.
    ///
    void  sendInput2( uint32  sleep );

  private:

    /// Connect the Input2 input stream.
    ///
    void    connectInput2();

    ///  Resume here.  Extend with a user callback.
    void  SunionsMarshal::delayInput2();

  public:

    struct  Input3 : public TupleHeader
    {
        TrivialTuple   _data;
    } __attribute__((__packed__));

    /// The sentInput3  method must be defined by the application.
    /// It is called after sendInput3 is done and a pause.
    ///
    void  sentInput3();

    /// Enque a Input3 for input.
    ///
    void  batchInput3( Input3  *tuple );

    /// Send enqued Input3 inputs.
    ///
    void  sendInput3( uint32  sleep );

  private:

    /// Connect the Input3 input stream.
    ///
    void    connectInput3();

    ///  Resume here.  Extend with a user callback.
    void  SunionsMarshal::delayInput3();

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

    /// The receivedControloutput3 method must be defined by the application.
    /// It is called after a(n) EmptyTuple is received.
    ///
    static void  receivedControloutput3( EmptyTuple  *tuple );

  private:

    /// Subscribe to the Controloutput3 output stream.
    ///
    void    subscribeControloutput3();

    /// Handler to receive tuples from the Controloutput3 stream.
    ///
    static Status  Controloutput3Handler( ptr<StreamEvent>  event );

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

    /// The receivedSoutput1 method must be defined by the application.
    /// It is called after a(n) TrivialTuple is received.
    ///
    static void  receivedSoutput1( TrivialTuple  *tuple );

  private:

    /// Subscribe to the Soutput1 output stream.
    ///
    void    subscribeSoutput1();

    /// Handler to receive tuples from the Soutput1 stream.
    ///
    static Status  Soutput1Handler( ptr<StreamEvent>  event );

  public:

    /// The receivedSoutput2 method must be defined by the application.
    /// It is called after a(n) TrivialTuple is received.
    ///
    static void  receivedSoutput2( TrivialTuple  *tuple );

  private:

    /// Subscribe to the Soutput2 output stream.
    ///
    void    subscribeSoutput2();

    /// Handler to receive tuples from the Soutput2 stream.
    ///
    static Status  Soutput2Handler( ptr<StreamEvent>  event );

  public:

    /// The receivedSoutput3 method must be defined by the application.
    /// It is called after a(n) TrivialTuple is received.
    ///
    static void  receivedSoutput3( TrivialTuple  *tuple, TupleType type );

  private:

    /// Subscribe to the Soutput3 output stream.
    ///
    void    subscribeSoutput3();

    /// Handler to receive tuples from the Soutput3 stream.
    ///
    static Status  Soutput3Handler( ptr<StreamEvent>  event );

  private:

    /// Launch the Borealis front-end.
    ///
    static int32  launchDiagram( string  xml,   // Path of an initial xml file.
                                 string  ip,    // Local medusa client ip.
                                 uint32  port   // Local medusa client port.
                                );

    /// Write the results of the experiment to file
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

  
    
};

#endif                         // SUNIONSMARSHAL_H

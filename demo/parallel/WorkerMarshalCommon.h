#ifndef  WORKERMARSHAL_H
#define  WORKERMARSHAL_H

#include  "MedusaClient.h"
#include  "TupleHeader.h"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Worker program.  Modified:  WorkerMarshal.h
//..............................................................................


using namespace Borealis;

class WorkerMarshal
{
  public:

    WorkerMarshal() {};
   ~WorkerMarshal() {};

    /// Activate the front-end and subscribe to streams.
    /// Returns 0 if okay; else an error occured.
    ///
    ////int32  open();


    /// Connect to inputs and subscribe to streams only.
    /// Similar to open() but does not load the query.
    ///
    void  openInputOutput(uint16    instance,
                          string    endpoint);


    /// Run the client event loop.
    /// This does not return unless terminated or on an exception.
    ///
    void  runClient();


    /// Terminate the client event loop.
    ///
    void  terminateClient();


    /// Copy a string value to a fixed length array and zero fill.
    ///
    static void  setStringField(string  value,
                                  char  field[],
                                uint32  length)
                          throw(AuroraException);


    /// Get the timestamp for a tuple.
    ///
    static timeval   getTimeValue(uint8  *tuple)
    {
       return(*((timeval *)(tuple - HEADER_SIZE)));
    }


  public:

    struct tile
    {
        uint8    *array;
    } __attribute__((__packed__));

    struct  Gather1 : public TupleHeader
    {
        tile   _data;
    } __attribute__((__packed__));

    /// The sentGather1  method must be defined by the application.
    /// It is called after sendGather1 is done and a pause.
    ///
    void  sentGather1();

    /// Enque a Gather1 for input.
    ///
    void  batchGather1(Gather1  *tuple);

    /// Send enqued Gather1 inputs.
    ///
    void  sendGather1(uint32  sleep);

  private:

    /// Connect the Gather1 input stream.
    ///
    void  connectGather1(uint16    instance);

    ///  Resume here.  Extend with a user callback.
    void  delayGather1();

  private:

    /// Handler to dispatch tuples received.
    ///
    static Status outputHandler(ptr<StreamEvent> event);


  public:

    /// The receivedScatter1 method must be defined by the application.
    /// It is called after a(n) tile is received.
    ///
    static WorkerMarshal::tile  *receivedScatter1(tile  *tuple);

  private:

    /// Subscribe to the Scatter1 output stream.
    ///
    void  subscribeScatter1(uint16    instance,
                            string    endpoint);

    /// Handler to receive tuples from the Scatter1 stream.
    ///
    static Status  Scatter1Handler(ptr<StreamEvent>  event);

  private:

    /// Launch the Borealis front-end.
    ///
    static int32  launchDiagram(string  xml);  // Path of an initial xml file.

    /// Client connections to Borealis nodes.
    ///
    MedusaClient      *_client;


    /// Event state for input streams.
    /// These are declared with a smart pointer as fast_post_event requires it.
    ///
    ptr<StreamEvent>   _eventGather1;


  public:

    /// Events in progress.  Added code.
    ///
    static  queue< ptr<StreamEvent> >  _active_event;

    static  string                     _gather;
    static  string                     _scatter;

};

#endif                         // WORKERMARSHAL_H

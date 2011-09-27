#ifndef  WORKERMARSHAL_H
#define  WORKERMARSHAL_H

#include  "MedusaClient.h"
#include  "TupleHeader.h"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Worker program.
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
    int32  open();


    /// Connect to inputs and subscribe to streams only.
    /// Similar to open() but does not load the query.
    ///
    void  openInputOutput();


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

    struct  Gather : public TupleHeader
    {
        tile   _data;
    } __attribute__((__packed__));

    /// The sentGather  method must be defined by the application.
    /// It is called after sendGather is done and a pause.
    ///
    void  sentGather();

    /// Enque a Gather for input.
    ///
    void  batchGather(Gather  *tuple);

    /// Send enqued Gather inputs.
    ///
    void  sendGather(uint32  sleep);

  private:

    /// Connect the Gather input stream.
    ///
    void  connectGather();

    ///  Resume here.  Extend with a user callback.
    void  delayGather();

  private:

    /// Handler to dispatch tuples received.
    ///
    static Status outputHandler(ptr<StreamEvent> event);


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
    ptr<StreamEvent>   _eventGather;
};

#endif                         // WORKERMARSHAL_H

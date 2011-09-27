#ifndef  PASSMARSHAL_H
#define  PASSMARSHAL_H

#include  "MedusaClient.h"
#include  "TupleHeader.h"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Pass program.
//..............................................................................


using namespace Borealis;

class PassMarshal
{
  public:

    PassMarshal() {};
   ~PassMarshal() {};

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

    struct frame
    {
        uint8    *array;
    } __attribute__((__packed__));

    struct  Frame_In : public TupleHeader
    {
        frame   _data;
    } __attribute__((__packed__));

    /// The sentFrame_In  method must be defined by the application.
    /// It is called after sendFrame_In is done and a pause.
    ///
    void  sentFrame_In();

    /// Enque a Frame_In for input.
    ///
    void  batchFrame_In(Frame_In  *tuple);

    /// Send enqued Frame_In inputs.
    ///
    void  sendFrame_In(uint32  sleep);

  private:

    /// Connect the Frame_In input stream.
    ///
    void  connectFrame_In();

    ///  Resume here.  Extend with a user callback.
    void  delayFrame_In();

  private:

    /// Handler to dispatch tuples received.
    ///
    static Status outputHandler(ptr<StreamEvent> event);


  public:

    /// The receivedFrame_Out method must be defined by the application.
    /// It is called after a(n) frame is received.
    ///
    static void  receivedFrame_Out(frame  *tuple);

  private:

    /// Subscribe to the Frame_Out output stream.
    ///
    void  subscribeFrame_Out();

    /// Handler to receive tuples from the Frame_Out stream.
    ///
    static Status  Frame_OutHandler(ptr<StreamEvent>  event);

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
    ptr<StreamEvent>   _eventFrame_In;
};

#endif                         // PASSMARSHAL_H

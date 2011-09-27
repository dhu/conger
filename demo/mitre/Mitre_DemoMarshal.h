#ifndef  MITRE_DEMOMARSHAL_H
#define  MITRE_DEMOMARSHAL_H

#include  "MedusaClient.h"
#include  "TupleHeader.h"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Mitre_Demo program.
//..............................................................................


using namespace Borealis;

class Mitre_DemoMarshal
{
  public:

    Mitre_DemoMarshal() {};
   ~Mitre_DemoMarshal() {};

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

    struct Element
    {
        int32    timestamp;
        int32    id;
        int32    x;
        int32    y;
        int32    color;
        int32    heading;
    } __attribute__((__packed__));

    struct  Elementinput : public TupleHeader
    {
        Element   _data;
    } __attribute__((__packed__));

    /// The sentElementinput  method must be defined by the application.
    /// It is called after sendElementinput is done and a pause.
    ///
    void  sentElementinput();

    /// Enque a Elementinput for input.
    ///
    void  batchElementinput(Elementinput  *tuple);

    /// Send enqued Elementinput inputs.
    ///
    void  sendElementinput(uint32  sleep);

  private:

    /// Connect the Elementinput input stream.
    ///
    void  connectElementinput();

    ///  Resume here.  Extend with a user callback.
    void  delayElementinput();

  public:

    struct ImageData
    {
        int32    timestamp;
        int32    frame_index;
        int32    row_index_begin;
        int32    row_index_end;
        char    image_data[ 352 ];
    } __attribute__((__packed__));

    struct  Videoinput : public TupleHeader
    {
        ImageData   _data;
    } __attribute__((__packed__));

    /// The sentVideoinput  method must be defined by the application.
    /// It is called after sendVideoinput is done and a pause.
    ///
    void  sentVideoinput();

    /// Enque a Videoinput for input.
    ///
    void  batchVideoinput(Videoinput  *tuple);

    /// Send enqued Videoinput inputs.
    ///
    void  sendVideoinput(uint32  sleep);

  private:

    /// Connect the Videoinput input stream.
    ///
    void  connectVideoinput();

    ///  Resume here.  Extend with a user callback.
    void  delayVideoinput();

  private:

    /// Handler to dispatch tuples received.
    ///
    static Status outputHandler(ptr<StreamEvent> event);


  public:

    struct AppElement
    {
        int32    timestamp;
        int32    id;
        int32    x;
        int32    y;
        int32    color;
    } __attribute__((__packed__));

    /// The receivedEnemytanksoutput method must be defined by the application.
    /// It is called after a(n) AppElement is received.
    ///
    static void  receivedEnemytanksoutput(AppElement  *tuple);

  private:

    /// Subscribe to the Enemytanksoutput output stream.
    ///
    void  subscribeEnemytanksoutput();

    /// Handler to receive tuples from the Enemytanksoutput stream.
    ///
    static Status  EnemytanksoutputHandler(ptr<StreamEvent>  event);

  public:

    /// The receivedAnalysisoutput method must be defined by the application.
    /// It is called after a(n) AppElement is received.
    ///
    static void  receivedAnalysisoutput(AppElement  *tuple);

  private:

    /// Subscribe to the Analysisoutput output stream.
    ///
    void  subscribeAnalysisoutput();

    /// Handler to receive tuples from the Analysisoutput stream.
    ///
    static Status  AnalysisoutputHandler(ptr<StreamEvent>  event);

  public:

    /// The receivedAcrossthelineoutput method must be defined by the application.
    /// It is called after a(n) AppElement is received.
    ///
    static void  receivedAcrossthelineoutput(AppElement  *tuple);

  private:

    /// Subscribe to the Acrossthelineoutput output stream.
    ///
    void  subscribeAcrossthelineoutput();

    /// Handler to receive tuples from the Acrossthelineoutput stream.
    ///
    static Status  AcrossthelineoutputHandler(ptr<StreamEvent>  event);

  public:

    /// The receivedEnemyaircraftoutput method must be defined by the application.
    /// It is called after a(n) AppElement is received.
    ///
    static void  receivedEnemyaircraftoutput(AppElement  *tuple);

  private:

    /// Subscribe to the Enemyaircraftoutput output stream.
    ///
    void  subscribeEnemyaircraftoutput();

    /// Handler to receive tuples from the Enemyaircraftoutput stream.
    ///
    static Status  EnemyaircraftoutputHandler(ptr<StreamEvent>  event);

  public:

    /// The receivedVideooutput method must be defined by the application.
    /// It is called after a(n) ImageData is received.
    ///
    static void  receivedVideooutput(ImageData  *tuple);

  private:

    /// Subscribe to the Videooutput output stream.
    ///
    void  subscribeVideooutput();

    /// Handler to receive tuples from the Videooutput stream.
    ///
    static Status  VideooutputHandler(ptr<StreamEvent>  event);

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
    ptr<StreamEvent>   _eventElementinput;
    ptr<StreamEvent>   _eventVideoinput;
};

#endif                         // MITRE_DEMOMARSHAL_H

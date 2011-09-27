#ifndef  GLOBALVIEW_H
#define  GLOBALVIEW_H

#include  "RegionServer.h"
#include  "HeadClient.h"

BOREALIS_NAMESPACE_BEGIN


// Id used to bind RPCs to the server side of the Global view.
//
#define  GLOBAL_SERVER  "GlobalServer"



////////////////////////////////////////////////////////////////////////////////
//
//  The GlobalView program manages a global catalog for a Borealis system.
//..............................................................................


class GlobalView
{
  public:

    GlobalView() {};

    ///  Launch the Global client and server; request the catalog from the Head.
    ///
    Status  open(/// Used by the Global client to connect to the Head.
                 string       head_point,

                 /// Port for the Viewer to receive updates from the head.
                 string       view_point);


    ///  Periodic client operation.
    ///
    void  write_global_catalog();


    ///  Start the event loop for the Global client.
    ///
    void  run();


  private:

    /// This is declared static so that it can be used in a pthread call.
    ///
    static void start_server(IOEventLoop* loop)
    { 
        _global_server->start(GLOBAL_SERVER);
    }


    pthread_t              _server_thread;


    /// Receive XML from the borealis Head via:  RegionServer::add_xml_string
    ///
    static  RegionServer  *_global_server;


    /// Used to run the event loop for the Global Viewer and to
    /// send requests to the Borealis Head.
    ///
    HeadClient            *_head_client;
};


BOREALIS_NAMESPACE_END

#endif                      // GLOBALVIEW_H

#ifndef HEADSERVER_H
#define HEADSERVER_H

#include  "DeployDiagram.h"
#include  "UpdateClient.h"

#include <NMSTL/rpc>

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  The Head receives XML passed via HeadClient and performs the request.
//
//..............................................................................


class HeadServer : public RPCObject
{
  public:

    HeadServer() : _send_catalog(0),
                   _global_client(NULL)
    {}


    ///  Parse a Borealis Network Diagram represented as an XML string into the
    ///  global catalog.  The updates are not deployed.
    ///
    RPC<void>  add_xml_string(string   update_xml);


    ///  Parse a Borealis Network Diagram represented as an XML string into the
    ///  global catalog and deploy it.
    ///
    RPC<void>  deploy_xml_string(string   xml_string);


    ///  Parse a Borealis Network Diagram represented as an XML file into the
    ///  global catalog and deploy it.
    ///
    RPC<void>  deploy_xml_file(string    xml_file);


    ///  Establish the port the Head uses to receive XML.
    ///  
    void  set_head(int  port)
    {   _global_catalog.set_head(":" + to_string(port));
    }


    ///  Get the global catalog maintained in the Head.
    ///
    DeployDiagram  &get_global_catalog()
    {   return  _global_catalog;
    }


    ///  Get the client for a Global component.
    ///
    UpdateClient  *get_global_client()
    {   return  _global_client;
    }


  private:

    /// State of a catalog being copied to a Global Component.
    ///
    #define  SEND_CLEAR    0            // Not sending the catalog.
    #define  SEND_RUNNING  1            // Sending the catalog.
    #define  SEND_CHANGED  2            // The catalog changed while sending.

    uint8           _send_catalog;


    /// The global catalog maintained by the Head.
    ///
    DeployDiagram   _global_catalog;


    /// The client used to connect to Global Components.
    /// NYI:  Multiple global clients???
    ///
    UpdateClient   *_global_client;


    ///  Disconnect the client for a Global component on an error.
    ///
    void  disconnect_global_client()
    {
        _send_catalog = SEND_CLEAR;

        if (_global_client)
        {   delete  _global_client;
            _global_client = NULL;
        }
    }


    /// Change the location of a box in the global catalog.
    ///
    Status  add_move_element(const DOMElement  *move);


    /// Accept a request to feed the global catalog to a global component.
    ///
    Status  add_global_element(const DOMElement  *global);


    /// Thread to send an initial global catalog to a Global Component.
    ///
    static void    send_global_catalog(HeadServer  *server);


    /// Send an update XML element to a Global Component.
    ///
    static Status  send_update_xml(HeadServer  *server,
                                   string       update_xml);


    ///  A catalog update is imminent, stop sending.
    ///
    void  send_catalog_changed();


    /// Reflect a catalog update to any global components.
    ///
    Status  reflect_global_update(string  update_xml);


    NMSTL_RPC_OBJECT(HeadServer);
};

BOREALIS_NAMESPACE_END

#endif                     // HEADSERVER_H

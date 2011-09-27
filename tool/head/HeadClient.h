#ifndef HEADCLIENT_H
#define HEADCLIENT_H

#include "NodeStatus.h"
#include "common.h"
#include "MedusaID.h"
#include "rpc_HeadServer.h"
#include <NMSTL/tcprpc>
#include "DataHandler.h"

#define  HEAD_PORT     DEFAULT_HEAD_PORT
#define  HEAD_BINDING  "BigGiantHead"


////////////////////////////////////////////////////////////////////////////////
//
//  An application includes HeadClient.h to send Xml to the borealis head.
//
//  HeadClient  *client = (HeadClient *)new HeadClient( 
//                        Util::form_endpoint(HEAD_NODE, DEFAULT_HEAD_PORT));
//
//  RPC<void>  rpc = client->deploy_xml_file( xml );
//  Status  status = rpc.stat();
//..............................................................................


BOREALIS_NAMESPACE_BEGIN

class HeadClient : private Remote<HeadServer>
{
    InetAddress     _remote;                 // Address of remote Medusa node.
    IOEventLoop     _loop;
    TCPRPCEndpoint  _endpoint;

    typedef map<MedusaID, ptr<TCPRPCEndpoint> > Endpoints;
    Endpoints       _endpoints;


  public:

    HeadClient(InetAddress  remote) :
        _remote(remote),
        _endpoint(_loop, TCPSocket(remote))
    {
        // Cast this into the Remote<T> and then assign the stub to
        // the casted object (i.e, to the correct part of the whole)
        //
        *static_cast<Remote<HeadServer>*>(this)
                      = _endpoint.get<HeadServer>(HEAD_BINDING);

        _endpoints[MedusaID(to_string(remote))]
                      = ptr<TCPRPCEndpoint>::unowned(&_endpoint);
    }


    IOEventLoop  &get_loop()
    {   return _loop;
    }


    void run()
    {   _loop.run();
    }


    void terminate()
    {   _loop.terminate();
    }


    IOEventLoop& loop()
    {   return _loop;
    }


    TCPRPCEndpoint  *get_endpoint()
    {
        return  &_endpoint;
    }
 

    ptr<TCPRPCEndpoint> endpoint_for(MedusaID  remote)
    {
        ptr<TCPRPCEndpoint>& ep = _endpoints[remote];

        if (!ep)
        {   InetAddress  addr(to_string(remote));

            if (!addr)
            {
                WARN << "MedusaID cannot be converted to address: " << remote;
                return ptr<TCPRPCEndpoint>();
            }

            ep.set(new TCPRPCEndpoint(_loop, TCPSocket(addr)));
        }

        return  ep;
    }


    Remote<HeadServer>  head_for(MedusaID  remote)
    {
        ptr<TCPRPCEndpoint> ep = endpoint_for(remote);

        if (!ep)
        {   return Remote<HeadServer>();
        }

        return  ep->get<HeadServer>(HEAD_BINDING);
    }


    using Remote<HeadServer>::deploy_xml_file;
    using Remote<HeadServer>::deploy_xml_string;

    using Remote<HeadServer>::add_xml_string;

    string as_string()
    {   return(_remote.as_string());
    }
};

BOREALIS_NAMESPACE_END

#endif                         // HEADCLIENT_H

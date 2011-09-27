#ifndef UPDATECLIENT_H
#define UPDATECLIENT_H

#include "NodeStatus.h"
#include "common.h"
#include "MedusaID.h"
#include "rpc_UpdateServer.h"
#include <NMSTL/tcprpc>
#include "DataHandler.h"



////////////////////////////////////////////////////////////////////////////////
//
//  An application includes UpdateClient.h to send Xml to the borealis head.
//
//..............................................................................


BOREALIS_NAMESPACE_BEGIN

class UpdateClient : private Remote<UpdateServer>
{
    InetAddress     _remote;           // Address of the component sending XML.
    IOEventLoop     _loop;
    TCPRPCEndpoint  _endpoint;

    typedef map<MedusaID, ptr<TCPRPCEndpoint> >  Endpoints;
    Endpoints       _endpoints;


  public:
    UpdateClient(InetAddress  remote,
                 string       binding = "RegionServer") :

        _remote(remote),
        _endpoint(_loop, TCPSocket(remote))
    {
        // Cast this into the Remote<T> and then assign the stub to
        // the casted object (i.e, to the correct part of the whole)
        //
        *static_cast<Remote<UpdateServer>*>(this)
                          = _endpoint.get<UpdateServer>(binding);

        _endpoints[ MedusaID(to_string(remote)) ]
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



    Boolean  is_connected()
    {   return  _endpoint.is_connected();
    }



    ptr<TCPRPCEndpoint> endpoint_for(MedusaID remote)
    {
        ptr<TCPRPCEndpoint>& ep = _endpoints[remote];
        if (ep) return ep;

        InetAddress addr(to_string(remote));
        if (!addr)
        {
            WARN << "MedusaID cannot be converted to address: " << remote;
            return ptr<TCPRPCEndpoint>();
        }

        ep.set(new TCPRPCEndpoint(_loop, TCPSocket(addr)));
        return ep;
    }



    Remote<UpdateServer> head_for(MedusaID  remote,
                                  string    binding = "RegionServer")
    {
        ptr<TCPRPCEndpoint> endpoint = endpoint_for(remote);

        if (!endpoint)
        {   return Remote<UpdateServer>();
        }

        return(endpoint->get<UpdateServer>(binding));
    }

    using Remote<UpdateServer>::add_xml_string;
    using Remote<UpdateServer>::get_region_stats; // get Statistics from the region server (it is called by the QP)
    using Remote<UpdateServer>::get_tuple_delivery_stats;
    using Remote<UpdateServer>::get_cpu_stats;
    using Remote<UpdateServer>::get_latency_stats;


    string as_string()
    {   return( _remote.as_string() );
    }
};

BOREALIS_NAMESPACE_END

#endif                         // UPDATECLIENT_H

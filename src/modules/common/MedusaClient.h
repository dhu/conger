#ifndef MEDUSACLIENT_H
#define MEDUSACLIENT_H

#include "rpc_QueryProcessor.h"
#include "rpc_LocalLoadShedder.h"
#include "rpc_NHLoadShedder.h"
#include <NMSTL/tcprpc>
#include "DataHandler.h"

BOREALIS_NAMESPACE_BEGIN

class MedusaClient :
    private  Remote<QueryProcessor>,
    private  Remote<LocalLoadShedder> // -- tatbul
{
    InetAddress     _remote; // Address of remote Medusa node
    IOEventLoop     _loop;
    TCPRPCEndpoint  _endpoint;

    typedef    map<MedusaID, ptr<TCPRPCEndpoint> >  Endpoints;
    Endpoints  _endpoints;


    // Fast outgoing data
    //
    ptr<DataHandler>          _data_handler;
    ptr<Subscription>         _fast_subscription;
    ptr<CatalogSubscription>  _subscription;


    // Fast incoming data
    //
    ptr< TCPAcceptor<DataHandler, ptr<DataHandler::DHArgs> > > _acceptor;


  public:

    MedusaClient(InetAddress  remote) :
        _remote(remote),
        _endpoint(_loop, TCPSocket(remote))
    {
        // Cast this into the Remote<T> and then assign the stub to
        // the casted object (i.e, to the correct part of the whole)
        //
        *static_cast<Remote<QueryProcessor>*>(this)   = _endpoint.get<QueryProcessor>("QueryProcessor");
        *static_cast<Remote<LocalLoadShedder>*>(this) = _endpoint.get<LocalLoadShedder>("LocalLoadShedder"); // -- tatbul

        _endpoints[ MedusaID(to_string(remote)) ] = ptr<TCPRPCEndpoint>::unowned(&_endpoint);
    }



    IOEventLoop  &get_loop()   { return _loop; }

    void  run()                { _loop.run(); }

    void  run_once()           { _loop.run_once(); }
  
    void  terminate()          { _loop.terminate(); }

    IOEventLoop  &loop()       { return  _loop; }

    string  as_string()        { return _remote.as_string(); }



    ////////////////////////////////////////////////////////////////////////////
    //
    ptr<TCPRPCEndpoint> endpoint_for(MedusaID remote)
    {
    //..........................................................................


        ptr<TCPRPCEndpoint>  &ep = _endpoints[remote];

        if (!ep)
        {   InetAddress addr(to_string(remote));

            if (!addr)
            {   WARN << "MedusaID cannot be converted to address: " << remote;
                return ptr<TCPRPCEndpoint>();
            }

            ep.set(new TCPRPCEndpoint(_loop, TCPSocket(addr)));
        }

        return ep;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    Remote<QueryProcessor>  qp_for(MedusaID remote)
    {
    //..........................................................................


        ptr<TCPRPCEndpoint> ep = endpoint_for(remote);

        if (!ep)  return Remote<QueryProcessor>();

        return  ep->get<QueryProcessor>("QueryProcessor");
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // -- tatbul
    //
    Remote<LocalLoadShedder> local_ls_for(MedusaID remote)
    {
    //..........................................................................


        ptr<TCPRPCEndpoint> ep = endpoint_for(remote);

        if (!ep) return Remote<LocalLoadShedder>();

        return  ep->get<LocalLoadShedder>("LocalLoadShedder");
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // -- tatbul
    //
    Remote<NHLoadShedder> nh_ls_for(MedusaID remote)
    {
    //..........................................................................


        ptr<TCPRPCEndpoint> ep = endpoint_for(remote);

        if (!ep) return  Remote<NHLoadShedder>();

        return  ep->get<NHLoadShedder>("NHLoadShedder");
    }


    using Remote<QueryProcessor>::add_conger_string;


    // Update XML
    //
    using Remote<QueryProcessor>::add_xml_string;
    using Remote<QueryProcessor>::move_boxes;


    // statistics
    //
    using Remote<QueryProcessor>::get_stats;
    using Remote<QueryProcessor>::get_statistics; //Added by Olga @ 25/04/2005
    using Remote<QueryProcessor>::get_statistics_name; //Added by Olga @ 26/04/2005
    using Remote<QueryProcessor>::get_cpu_statistics; //Added by Olga @ 27/04/2005
    using Remote<QueryProcessor>::get_region_statistics;
    using Remote<QueryProcessor>::get_tuple_delivery_statistics;
    using Remote<QueryProcessor>::get_region_cpu_statistics;
    using Remote<QueryProcessor>::get_latency_statistics;


    // subscriptions
    //
    using Remote<QueryProcessor>::subscribe_streams;
    using Remote<QueryProcessor>::unsubscribe_stream;


    // local load shedder interface -- tatbul
    //
    using Remote<LocalLoadShedder>::send_lsplans;
    using Remote<LocalLoadShedder>::send_plan_id;


    bool has_data_path() { return _data_handler; }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Setup data path connection for sending data to Borealis.
    //
    Status  set_data_path(/// Maximum bytes that can be sent.
                          ///
                          int     max_buffer,

                          /// Ip address of the recipient.
                          ///
                          string  ip,

                          /// Recipient socket port number without any offset.
                          ///
                          int     port,

                          /// Default socket flags if true (not in effect???).
                          ///
                          bool    blocking = true,

                          /// Method to call when ready to send more data.
                          ///
         ptr<DataHandler::dhconnect_callback>  conn_cb
                              = ptr<DataHandler::dhconnect_callback>())
    {
    //..........................................................................


        // Actual data path is expected at a data port.
        //
        InetAddress  offset_dst(ip, port + DATA_PORT_OFFSET);

        // Well... this doesn't really change anything
        //
        Socket::flags flags = Socket::nonblocking;

        if (blocking)
        {   flags = Socket::none;
        }

        ptr<DataHandler::DHArgs> args(new  DataHandler::DHArgs(max_buffer));

        TCPSocket  sock(offset_dst, flags);
    
        if (!_data_handler)
        {   INFO << "Setting new data handler to " << offset_dst << "...";
            _data_handler.set(new DataHandler(_loop, sock, args, conn_cb));
        }
        else
        {   INFO << "Updating data handler to " << offset_dst << "...";
            _data_handler->set_socket(sock);
        }
    
        INFO << "Data handler set to send to " << offset_dst << "!";

        if (!_data_handler)
        {   DEBUG << "Failed connecting";

            return  "Failed connecting to " + ip + ":"
                        + to_string(port + DATA_PORT_OFFSET);
        }

        return  true;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    Status set_data_initializer(ptr<DataHandler::dhconnect_callback> conn_cb)
    {
    //..........................................................................


        if (_data_handler)
        {   return  _data_handler->set_connect_callback(conn_cb);
        }

        return  DataHandler::NO_CONNECTION;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Send StreamEvents on the fast data path.
    //
    Status fast_post_event(ptr<StreamEvent>  event)
    {
    //..........................................................................


        if (_data_handler)
        {   return  _data_handler->send_data(event);
        }

        WARN << "DataHandler::NO_CONNECTION";
        return  DataHandler::NO_CONNECTION;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    /// Set up a port to listen for incoming data from Borealis to a clinet.
    ///
    Status set_data_handler(/// Address to receive data.  Currently no offset???
                            ///
                            InetAddress              listen,

                            /// Method to receive data.
                            ///
                            DataHandler::dhcallback  cb)
    {
    //..........................................................................


        if (!_acceptor || !(_acceptor->stat()))
        {   ptr<DataHandler::DHArgs>  args( new  DataHandler::DHArgs(MAX_SOCKET_BUFFER, cb ));

            _acceptor.set(new
                TCPAcceptor<DataHandler, ptr<DataHandler::DHArgs> > (_loop,
                                                                     listen,
                                                                     args));

            if (!_acceptor || !(_acceptor->stat()))
            {   return  "Failed setting-up fast data path: "
                      + to_string(_acceptor->stat());
            }

            INFO << "Client is listening at:  " << listen;
        }

        return  true;
    }



    ////////////////////////////////////////////////////////////////////////////
    //
    // Catalog based subscription interface.
    //
    Status subscribe(Name  stream_name)
    {
    //..........................................................................


        if (!_acceptor || !(_acceptor->stat()))
        {   return  "You must call set_data_handler first";
        }

        InetAddress listen(_acceptor->get_socket().getsockname());
        
        DEBUG << "Subscribing with " << listen << " to " << to_string(_remote);

        _subscription.set(new CatalogSubscription(listen, stream_name));

        DEBUG << "Setting the sink flag";
        _subscription->set_sink_flag(); // -- tatbul

        vector<CatalogSubscription> subs;
        subs.push_back(*_subscription);

        RPC<void>  r = subscribe_streams(subs);

        if (!r.stat())
        {   return  "Failed to add TCP subscription: " + to_string(r.stat());
        }

        return  true;
    }

 

    ////////////////////////////////////////////////////////////////////////////
    //
    Status unsubscribe()
    {
    //..........................................................................


        if (!_subscription)
        {   return "No subscription to remove";
        }

        RPC<void> r = unsubscribe_stream(*_subscription);;

        if (!r.stat())
        {   return  "Failed to remove TCP subscription: " + to_string(r.stat());
        }

        return  true;
    }
};


BOREALIS_NAMESPACE_END

#endif                             // MEDUSACLIENT_H

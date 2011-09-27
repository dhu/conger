#ifndef BASICCOMPONENT_H
#define BASICCOMPONENT_H

#include "common.h"
#include "rpc_LocalLoadShedder.h"
#include "rpc_NHLoadShedder.h"
#include "rpc_NHOptimizer.h"
#include "rpc_QOSOptimizer.h"
//#include "rpc_HA.h"
#include "rpc_AvailabilityMonitor.h"
#include "rpc_QueryProcessor.h"
//#include "rpc_Lookup.h"

#include <NMSTL/tqueuerpc>
#include <NMSTL/tcprpc>
#include <NMSTL/xmlrpc>

BOREALIS_NAMESPACE_BEGIN

class BasicComponent : public Thread, public RPCObject
{

  public:
    BasicComponent(string id, string name) :
        Thread(name), _id(id), _name(name), _endpoint(_my_loop), _status(true)
    {
        _bindings.bind(*this, name);
        _endpoint.set_bindings(_bindings);
    }

    virtual ~BasicComponent() {}

  protected:
    // My node ID
    MedusaID _id;

    string _name;
    RPCBindings _bindings;
    IOEventLoop _my_loop;
    TQueueRPCEndpoint _endpoint;
    Status _status;

    /// Initialization; called before run() in *component* thread
    /// (c.f. init(), called in *main* thread)
    virtual void in_thread_init() {}

    virtual void finish_up() {} // -- tatbul

    void run()
    {
        NOTICE << _name << " starting";
        in_thread_init();
        _my_loop.run();
    }

    void set_error(Status status)
    {
        ERROR << status;

        if (_status) 
        {
            _status = status;
        }
    }

    RPCBindings& bindings()
    {
        return _bindings;
    }

    //-- formerly from ComponentContext
//    ptr<RPCClient> _ha_client;
//    Remote<HA> _ha;

    ptr<RPCClient> _avail_mon_client;
    Remote<AvailabilityMonitor> _avail_mon;

    ptr<RPCClient> _local_ls_client;
    Remote<LocalLoadShedder> _local_ls;

    ptr<RPCClient> _nh_ls_client;
    Remote<NHLoadShedder> _nh_ls;

    ptr<RPCClient> _nhopt_client;
    Remote<NHOptimizer> _nhopt;

    ptr<RPCClient> _qosopt_client;
    Remote<QOSOptimizer> _qosopt;

    ptr<RPCClient> _qp_client;
    Remote<QueryProcessor> _qp;

    // Port base
    int _port_base;

    // My InetAddress for TCP endpoint
    InetAddress _addr;

    // Main event loop
    IOEventLoop *_loop;

    // MedusaID + transport ("tcp" or "xml")
    typedef pair<MedusaID, string> EndpointID;

    typedef map<EndpointID, ptr<RPCClient> > Endpoints;
    Endpoints _endpoints;

    void clear_endpoints_for(MedusaID remote, string transport = "tcp")
    {
        _endpoints.erase(EndpointID(remote, transport));
    }

    ptr<RPCClient> endpoint_for(MedusaID remote, string transport = "tcp")
    {
        InetAddress addr(to_string(remote));

        if (!addr)
        {
            WARN << "MedusaID cannot be converted to address: " << remote;
            return ptr<RPCClient>();
        }

        ptr<RPCClient>& ep = _endpoints[EndpointID(remote, transport)];

        if (ep) return ep;

        if (transport == "xml")
        {
            ep.set(new XMLRPCClient(_my_loop, addr));
        }
        else if (transport == "tcp")
        {
            ep.set(new TCPRPCEndpoint(_my_loop, TCPSocket(addr)));
        }
        else
        {
            WARN << "endpoint_for with invalid transport " << transport;
        }

        return ep;
    }


    template <typename T>
    Remote<T> proxy_for(MedusaID       remote,
                        rpc_object_id  id,
                        string         transport = "tcp")
    {
        ptr<RPCClient> ep = endpoint_for(remote, transport);
        if (!ep)
        {
            return Remote<T>();
        }

        return ep->template get<T>(id);
    }


    template <typename T>
    Remote<T> proxy_for(InetAddress    remote,
                        rpc_object_id  id,
                        string         transport = "tcp")
    {
        return proxy_for<T>( MedusaID(to_string(remote)), id, transport );
    }


    Remote<QueryProcessor> qp_for(MedusaID remote)
    {
        if (remote == _id)
        {
            return _qp;
        }

        return proxy_for<QueryProcessor>(remote, "QueryProcessor");
    }

/*    
    Remote<HA> ha_for(MedusaID remote)
    {
      if (remote == _id)
          return _ha;
      return proxy_for<HA>(remote, "HA");
    }
*/

    Remote<AvailabilityMonitor> avail_mon_for(MedusaID remote)
    {
        if (remote == _id)
        {
            return _avail_mon;
        }

        return proxy_for<AvailabilityMonitor>(remote, "AvailabilityMonitor");
    }

    Remote<LocalLoadShedder> local_ls_for(MedusaID remote)
    {
        if (remote == _id)
        {
            return _local_ls;
        }

        return proxy_for<LocalLoadShedder>(remote, "LocalLoadShedder");
    }

    Remote<NHLoadShedder> nh_ls_for(MedusaID remote)
    {
        if (remote == _id)
        {
            return _nh_ls;
        }

        return proxy_for<NHLoadShedder>(remote, "NHLoadShedder");
    }

    Remote<NHOptimizer> nhopt_for(MedusaID remote)
    {
        if (remote == _id)
        {
            return _nhopt;
        }

        return proxy_for<NHOptimizer>(remote, "NHOptimizer");
    }

    Remote<QOSOptimizer> qosopt_for(MedusaID remote)
    {
        if (remote == _id)
        {
            return _qosopt;
        }

        return proxy_for<QOSOptimizer>(remote, "QOSOptimizer");
    }

  public:
    TQueueRPCEndpoint& endpoint() { return _endpoint; }

    /// Initialize; called before thread is launched, from main() thread
    /// (c.f. in_thread_init(), called from *component* thread)
    virtual void init() {}

    void terminate()
    {
        NOTICE << "Terminating " << _name;
        finish_up(); // -- tatbul
        _my_loop.terminate();
    }

    Status stat()
    {
        return _status;
    }

    friend struct medusa_components;
};

BOREALIS_NAMESPACE_END
#endif                          // BASICCOMPONENT_H

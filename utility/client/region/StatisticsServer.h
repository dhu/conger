#ifndef STATISTICSSERVER_H
#define STATISTICSSERVER_H

#include <NMSTL/rpc>
#include <NMSTL/text>
#include <NMSTL/tcprpc>
#include <NMSTL/xmlrpc>
#include "Statistics.h"

BOREALIS_NAMESPACE_BEGIN
/**
 * The StatisticsServer creates an RPC server.
 *
 * It is used by the statistics gui to get the regional statistics of all the
 * nodes in the network. The server stores the regional statistics of all the
 * nodes and provides an RPC call returning them.
 */
class StatisticsServer : public RPCObject
{

public:
    /**
     * Creates an RPC Server for the given port
     *
     * @param port the port the RPC server listens to
     *
     */
    StatisticsServer(int port): _port(port)
    {
    }

    /**
     * Creates the acceptors for the RPC call.
     *
     * There servers accepts regular RPC call and XML RPC calls.
     *
     */
    void  setacceptors();
    
    /**
     * An RPC call returning the regional statistics of the nodes in the network
     * @return the data structure with statistics of all nodes
     */
    RPC <map <string, Statistics> > get_region_statistics() {return(_region_statistics);}

    /**
     * Set the statistics of all the nodes
     *
     * @param s the mapping of node IP to their statistics
     */
    void set_region_statistics(map <string, Statistics> s) { _region_statistics=s; }

    /**
     * The loop that the StatisticsServer uses
     */
    ptr<IOEventLoop>        _loop;
    /**
     * The data structure containing the statistics of all nodes
     */
    map<string, Statistics>     _region_statistics;

private:
    /**
     * Port of the RPC server
     */
    int         _port;
    /**
     * PRC Binding of the RPC server
     */
    RPCBindings         _bindings;

    NMSTL_RPC_OBJECT(StatisticsServer);

};

BOREALIS_NAMESPACE_END

#endif                     // STATISTICSSERVER_H

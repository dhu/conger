#include <NMSTL/tcprpc>
#include <NMSTL/xmlrpc>
#include <NMSTL/text>

#include "RegionServer.h"
#include "WriteRegion.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Start the event loop for the Region Server.
//
void  RegionServer::start(string  id)
{
//..............................................................................


    _bindings.bind(*this, id);
    TCPRPCAcceptor acceptor(_loop, _endpoint, _bindings);

    if (!acceptor)
    {   FATAL << "Unable to accept TCP/RPC connections: " << acceptor.stat();
    }

    NOTICE << "Accepting Regional TCP/RPC connections on "
           << acceptor.get_socket().getsockname();

    XMLRPCAcceptor  xacceptor(_loop,
                              Util::fudge_port(_endpoint, XML_RPC_PORT_OFFSET),
                              _bindings);

    if (!xacceptor)
    {   FATAL << "Unable to accept XML/RPC connections: " << xacceptor.stat();
    }

    NOTICE << "Accepting Regional XML/RPC connections on "
           << xacceptor.get_socket().getsockname();


    //  This does not return unless terminated or on an exception.
    _loop.run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void RegionServer::terminate()
{
//..............................................................................


    NOTICE << "Terminating the RegionServer";
    _loop.terminate();
}



////////////////////////////////////////////////////////////////////////////////
//
//  Parse a Borealis Network Diagram represented as an XML string into a
//  catalog structure.
//
RPC<void>  RegionServer::add_xml_string(// A string containing an XML network diagram.
                                        string   xml_string)
{
//..............................................................................


    DEBUG << "Parsing... " << xml_string;
    Status  status = _regional_catalog.parse_update_xml(xml_string);

    if ( !status )
    {   WARN << "Invalid update:  status=" << status << "\n" << xml_string;
        return  RPCFault(status);
    }

    _catalog_changed = True;
    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Get the regional catalog as an XML string.
//  Returns an empty string if the catalog has not been loaded or
//  has not changed since the last time it was loaded.
//
RPC<string>  RegionServer::load_regional_catalog()
{
//  Note:  This implementation assumes there is only one component that
//         is requesting the catalog.
//..............................................................................


    if (!_regional_catalog.is_region_loaded())
    {   return  string();
    }
    else if (!_catalog_changed)
    {   return  string();
    }

    _catalog_changed = False;

    return  WriteRegion::form_region(_regional_catalog);
}



////////////////////////////////////////////////////////////////////////////////
//
RPC<map <string, map<string,double> > >  RegionServer::get_latency_stats()
{
//..............................................................................


    map <string, map <string, double > > stats;
    INFO << "Get Latency Statistics for :" ;
    typedef map <string, Statistics>::iterator nodes_it;

    for (nodes_it i=_region_statistics.begin(); i!=_region_statistics.end();++i) 
    {   map <string, double> streamlatency;

        if (i->second.has_tsstats("OutLatency") && i->second.has_tsstats("TupleNum"))
        {   TSStats ts_latency = i->second.get_tsstats("OutLatency");
            TSStats ts_tuples = i->second.get_tsstats("TupleNum");
            typedef map <string, FixLenTimeSeries> ::iterator stats_it;

            for (stats_it j =ts_latency._tsstats.begin();j!=ts_latency._tsstats.end(); ++j)
            {   double latency =i->second.get_last_sample("OutLatency", j->first);
                int tuplenum = static_cast<int>(i->second.get_last_sample("TupleNum", j->first));
                streamlatency[j->first] = (tuplenum ? latency/ static_cast<double>(tuplenum) : -1);
            }

            stats[i->first]= streamlatency;
        }
    }

    return  stats;
}



////////////////////////////////////////////////////////////////////////////////
//
// Sets the tuple deliver statistics for the output streams
//
void  RegionServer::set_tuple_delivery(
                       map<string, map<string, double> >  tuple_delivery)
{
    map<string, map<string, double> >::iterator  i;
    map<string, double>::iterator                j;
//............................................................................


    _tuple_delivery = tuple_delivery;
    DEBUG << "Tuple Delivery stats:";

    for (i = _tuple_delivery.begin(); i != _tuple_delivery.end(); i++)
    {
        for (j = (i->second).begin(); j != (i->second).end(); j++)
        {
            DEBUG << i->first << " : " << j->first
                  <<  " -> "  << j->second << "%";
        }
    }

    return;
}


BOREALIS_NAMESPACE_END

//--- Added by nmstl-rpcgen; do not modify.  Leave at end of file!
#define NMSTL_RPC_DEFINE
#include "rpc_RegionServer.h"
#undef NMSTL_RPC_DEFINE
//---

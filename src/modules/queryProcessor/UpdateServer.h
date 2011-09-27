#ifndef UPDATESERVER_H
#define UPDATESERVER_H

#include "NodeStatus.h"
#include "Diagram.h"
#include "Statistics.h"

#include <NMSTL/rpc>

////////////////////////////////////////////////////////////////////////////////
//
//  UpdateClient includes UpdateServer.h to send Xml to the borealis head.
//  It is a stub corresponding to the actual server methods.
//  For regional components these are in RegionServer.
//  For the Head these are in HeadServer.
//..............................................................................


BOREALIS_NAMESPACE_BEGIN

class UpdateServer : public RPCObject
{
  public:

    RPC<void>  add_xml_string(string  xml_string);

    RPC<map <string, Statistics> > get_region_stats(); // get region statistics from region server
    RPC<map <string, map<string, double> > > get_tuple_delivery_stats();
    RPC<map <string, double> > get_cpu_stats();
    RPC<map <string, map<string,double> > > get_latency_stats();

    NMSTL_RPC_OBJECT( UpdateServer );
};

BOREALIS_NAMESPACE_END

#endif                     // UPDATESERVER_H

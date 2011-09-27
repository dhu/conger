#ifndef REGIONSERVER_H
#define REGIONSERVER_H

#include "NodeStatus.h"
#include "RegionalCatalog.h"
#include "Statistics.h"
#include "TSStats.h"
#include <NMSTL/rpc>

BOREALIS_NAMESPACE_BEGIN


class RegionServer : public RPCObject
{
  public:

    /**
     * Creates a RegionServer at a given port on the localhost.
     * @param port the port this RegionServer is listening on.
     */
    RegionServer(uint16  port): _catalog_changed(False)
    {
        _endpoint = InetAddress("localhost:" + to_string(port));
    }


    /**
     * Creates a RegionServer at a given endpoint.
     * @param port the port this RegionServer is listening on.
     */
    RegionServer(string  endpoint)
        : _catalog_changed(False)
    {
        _endpoint = InetAddress(endpoint);
    }


    /**
     * Returns the Regional Catalog
     * @return the Regional Catalog
     */
    RegionalCatalog  &get_regional_catalog()
    {
        return  _regional_catalog;
    }


    /**
     * @return True if the Regional Catalog is ready to use.
     */
    Boolean  is_region_loaded()
    {   return  _regional_catalog.is_region_loaded();
    }


    /**
     * Returns the IOEventLoop of this RegionServer
     * @return the IOEventLoop of this RegionServer
     */
    IOEventLoop  &get_ioevent_loop()
    {
        return  _loop;
    }


    /**
     * Sets the tuple deliver statistics for the output streams
     */
    void  set_tuple_delivery(map<string, map<string, double> > tuple_delivery);


    /**
     * Start the RegionServer
     * (this function call will run the loop and will not return)
     * @param id Identifier used for binding RPCs for the RegionServer.
     */
    void start(string  id = "RegionServer");


    /**
     * Stop the RegionServer
     */
     void terminate();


    /**
     * Parse a Borealis Network Diagram represented as an XML string into a catalog structure.
     * @param xml_string the input XML string to be parsed
     */
    RPC<void>  add_xml_string(string  xml_string);


    /**
     *   Get the regional catalog as an XML string.
     */
    RPC<string>  load_regional_catalog();


    /**
     *  Return the regional statistics maintained in the regional server
     *  @return the regional statistics data structure
     */
    RPC<map <string, Statistics> > get_region_stats()
    {
        return  _region_statistics;
    }


    /**
     * Return the tuple delivery statistics maintained in the regional server
     * @return the tuple delivery statistics data structure
     */
    RPC<map <string, map<string, double> > > get_tuple_delivery_stats()
    {
        return  _tuple_delivery;
    }


    /**
     * Returns the cpu statistics     * 
     * @return the cpu statistics data structure
     */
    RPC<map <string, double> > get_cpu_stats()
    {
        return  _region_cpu_statistics;
    }


    /**
     * Returns the latency statistics
     * @return the latency statistics data structure
     */
    RPC<map <string, map<string,double> > > get_latency_stats();


    /**
     * The data structure for the statistics of the nodes in the region
     */
    map <string, Statistics>   _region_statistics;


    /**
     * The data structure for the cpu statistics of the nodes in the region
     */
    map <string, double>       _region_cpu_statistics;


  private:

    /**
     * A regional catalog.
     */    
    RegionalCatalog  _regional_catalog;


    /**
     * True when the Regional Catalog has been updated since the last call
     * to load_regional_catalog.
     */    
    Boolean          _catalog_changed;


    /**
     * The endpoint of the RegionServer
     */    
    InetAddress      _endpoint;


    /**
     * The loop that RegionServer use
     */    
    IOEventLoop      _loop;


    /**
     * RPC binding
     */    
    RPCBindings      _bindings;


    /**
     * Tuple delivery stats for output streams.
     */   
    map<string, map<string, double> >  _tuple_delivery;

    NMSTL_RPC_OBJECT( RegionServer );
};

BOREALIS_NAMESPACE_END

#endif                     // REGIONSERVER_H

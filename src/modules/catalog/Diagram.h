#ifndef  DIAGRAM_H
#define  DIAGRAM_H

#include <xercesc/dom/DOM.hpp>
#include "xercesDomUtil.h"

#include <NMSTL/ptr>

#include "Catalog.h"
#include "util.h"

BOREALIS_NAMESPACE_BEGIN

class Diagram : public Catalog
{
  public:
    Diagram() : _deploy_local(False),
                _add_node(""),
                _replica_query("")
    {};

    virtual ~Diagram() {};


    ///  Parse a Diagram XML file into a catalog structure.
    ///
    Status  parse_file(/// A Borealis network diagram in XML.
                       string     file);


    ///  Parse a Diagram XML string into a catalog structure.
    ///
    Status  parse_string(/// A Borealis network diagram in XML.
                         string    diagram);


    ///  Parse a single XML schema as a string.
    ///
    Status  parse_xml_schema(/// An XML schema to be parsed.
                             string    schema_xml,

                             /// Return the schema from the catalog.
                             CatalogSchema  **schema);


    ///  Parse a single XML query as a string.
    ///
    Status  parse_xml_query(/// An XML query to be parsed.
                            string     query_xml,

                            /// Return the query from the catalog.
                            CatalogQuery  **query,

                            /// Whether this is a dynamic query addition.
                            Boolean dynamic = false);


    ///  Parse a single XML connection point view as a string.
    ///
    Status  parse_xml_view(/// An XML connection point view to be parsed.
                           string    view_xml,

                           /// Return the connection point view from the catalog.
                           CatalogView  **view);


    ///  Parse a single XML stream as a string.
    ///
    Status  parse_xml_stream(/// An XML stream to be parsed.
                             string    stream_xml,

                             /// Return the stream from the catalog.
                             CatalogStream  **stream);


    ///  Parse a single XML subscribe element as a string.
    ///
    Status  parse_xml_subscribe(// An XML subscribe element to be parsed.
                                string    subscribe_xml,

                                // Return a copy of the subscription.
                                CatalogSubscription  *subscription);


    ///  Retreive the name of a query after calling parseXmlQuery.
    ///
    Name    get_query_name() { return(_add_query->get_query_name()); }


    ///  Parse a single XML element as a string.
    ///
    Status  parse_element(/// An element to be parsed.
                          string    element);


    ///  Return the parsed element.
    ///
    const DOMElement  *get_element()
    {   return(_root);
    }


    ///  Add a single parsed array type declaration to a map.
    ///
    void  add_type_element(const DOMElement  *type)
                     throw(AuroraBadEntityException);


    ///  Add a single parsed schema into a catalog structure.
    ///
    CatalogSchema  *add_schema_element(const DOMElement  *schema)
                                 throw(AuroraBadEntityException);


    Boolean  is_borealis_diagram()
    {   return(_borealis);
    }


    /// Force all components to be deployed locally.  Used for development.
    ///
    void  deploy_local()
    {   _deploy_local = True;
    }


    /// Add replica to a set for the local catalog.
    ///
    void  add_replica(/// Endpoint for the node to be added.
                      string   node)
    {
        _replica_set.push_back(node);
    }


    /// Access the set of replicas in the local catalog.
    ///
    vector<string>  *get_replica_set()
    {   return(&_replica_set);
    }


    ///  Add a single subscribe element top the catalog.
    ///
    CatalogSubscription  add_subscribe_element(const DOMElement *subscribe)
                                         throw(AuroraBadEntityException);


    ///  Automatically fill in deployment for a simple network.
    ///
    Status  generate_deployment();


    ///  Add a single parsed box into a catalog structure.
    ///
    virtual  CatalogBox  *add_box_element(const DOMElement  *box,
                                                   Boolean   dynamic = False)
                                    throw(AuroraBadEntityException);


    ///  Add a single parsed table into a catalog structure.
    ///
    CatalogTable  *add_table_element(const DOMElement  *table)
                               throw(AuroraBadEntityException);


    ///  Add a single parsed input or output stream into a catalog structure.
    ///
    CatalogStream  *add_input_element(const DOMElement  *input)
                                throw(AuroraBadEntityException);


    ///  Add a single parsed connection point view into a catalog structure.
    ///
    CatalogView  *add_view_element(const DOMElement  *view)
                             throw(AuroraBadEntityException);


    ///  Add parameters to a box or table.
    ///
    void  add_parameter_element(const DOMElement  *root)
                          throw(AuroraBadEntityException);


    /// Modify parameters of an existing box or table.
    ///
    void  add_modify_element(const DOMElement  *modify)
                             throw(AuroraBadEntityException);


  protected:

    ///  Parse a Borealis Diagram XML file into a catalog structure.
    ///
    void  parse_xml()
              throw(AuroraBadEntityException);


    ///  Parse a Diagram XML file into a catalog structure.
    ///
    void  parse_xml_borealis(const DOMElement   *root)
                       throw(AuroraBadEntityException);


    ///  Parse XML for inputs into a catalog structure.
    ///
    void  parse_input(const DOMElement   *root)
                throw(AuroraBadEntityException);


    ///  Parse XML for inputs into a catalog structure.
    ///
    void  parse_output(const DOMElement   *root)
                 throw(AuroraBadEntityException);


    ///  Parse XML for boxes into a catalog structure.
    ///  yna -- added dynamic flag to support dynamic insertions into the engine.
    ///
    void  parse_box(const DOMElement  *root,
                    Boolean            dynamic = false)
              throw(AuroraBadEntityException);


    ///  Parse tables in XML into a catalog structure.
    ///
    void  parse_table(const DOMElement   *root)
                throw(AuroraBadEntityException);


    ///  Parse modify element to change parameters of an existing box or table.
    ///  Not yet in use.
    ///
    void  parse_modify(const DOMElement   *root)
                       throw(AuroraBadEntityException);


    ///  Parse XML for array type declarations into a map.
    ///
    void  parse_type(const DOMElement   *root)
               throw(AuroraBadEntityException);


    ///  Parse XML for schemas into a catalog structure.
    ///
    void  parse_schema(const DOMElement   *root)
                 throw(AuroraBadEntityException);


    ///  Parse XML for connection point views into a catalog structure.
    ///
    void  parse_view(const DOMElement   *root)
               throw(AuroraBadEntityException);


    ///  Parse XML for queries into a catalog structure.
    ///
    void  parse_query(const DOMElement   *root)
                throw(AuroraBadEntityException);


    ///  Add a single parsed query into a catalog structure.
    ///
    CatalogQuery  *add_query_element(const DOMElement  *query,

                                     /// Records if the query is dynamically being added.
                                     Boolean dynamic = false)
                               throw(AuroraBadEntityException);


    ///  Parse XML to delete queries from a catalog structure.
    ///
    void  parse_delete(const DOMElement  *root)
                 throw(AuroraBadEntityException);


    ///  Parse a Deployment XML file into a catalog structure.
    ///
    void  parse_xml_deploy(const DOMElement  *root)
                     throw(AuroraBadEntityException);


    ///  Parse a replica_set element to locates queries on a set of nodes.
    ///
    void  parse_replica(const DOMElement   *root)
                  throw(AuroraBadEntityException);


    ///  Parse a Borealis Diagram XML file into a catalog structure.
    ///
    void  parse_node(const DOMElement   *root)
               throw(AuroraBadEntityException);


    ///  Parse a Region configuration into a catalog structure.
    ///
    void  parse_region(const DOMElement   *root)
                 throw(AuroraBadEntityException);


    ///  Associate publish and subscribe elements with clients.
    ///
    void  parse_client(const DOMElement   *root)
                 throw(AuroraBadEntityException);


    ///  Parse publish elements to deploy inputs into a catalog structure.
    ///
    void  parse_publish(const DOMElement   *root)
                  throw(AuroraBadEntityException);


    ///  An application monitor subscribes to an output stream.
    ///
    void  parse_subscribe(const DOMElement   *root)
                    throw(AuroraBadEntityException);


    ///  An application monitor removes a subscription to an output stream.
    ///
    void  parse_unsubscribe(const DOMElement   *root)
                      throw(AuroraBadEntityException);


    ///  Parse XML to parse update queue and key attributes.
    ///
    CatalogBox::InQueue  parse_in_queue(const DOMElement    *root,

                                        // Default queue options.
                                        CatalogBox::InQueue  in_queue)
                                  throw(AuroraBadEntityException);


    ///  Assign a query's node to each box contained in the query.
    ///
    void  locate_query(/// Name of the query.
                       Name   name)
                 throw(AuroraBadEntityException);


    ///  Return either a <host>:<port> for global deployment or
    ///  localhost:<port> for local deployment.
    ///
    string  form_endpoint(/// An endpoint as:  [<host>][:[<port>]]
                          string   endpoint,

                          /// A default port number.
                          uint16   Port);


    ///  See if a host name is a non-local dotted IP address.
    ///
    static Boolean  is_dotted_host(string  host);



    ////////////////////////////////////////////////////////////////////////////
    //
    Boolean            _borealis;         // True  - Parsing a borealis diagram.
                                          // False - Parsing a deployment file.

    Boolean            _deploy_local;     // Force local deployment.

    const DOMElement  *_root;             // Parsing root.

    ptr<DOMDocument>   _doc;              // Parsed document.

    string             _default_recovery; // A default High Availability method.

    string             _add_node;         // Node for a query being added.

    string             _replica_query;    // Query being replicated.

    string             _client_monitor;   // Endpoint for a client element.

    typedef  map<string, string>  TypeArray;

    TypeArray          _array_type;       // Aliases for arrays.


    /// Map the first replica in a set to the rest.
    ///
    typedef   map<string, set<string> >       ReplicaMap;

    ReplicaMap            _replica_map;      // Node endpoint -> replicas.

    vector<string>        _replica_set;      // Replica set for this node.


    /// Map a node's canonnical endpoint to names of regions that include the node.
    ///
    typedef   map<string, set<string> >   RegionMap;

    RegionMap             _region_map;       // Node endpoint -> regions.

    map<uint16, string>   _local_endpoint;   // Map port -> host to keep ports
                                             // unique with local deployment.


    ////////////////////////  Experimental

    /// Map partitioned outputs from a scatter box to worker endpoints.
    ///
    map<string, string>   _partition_output;


    /// Map partitioned inputs to a gather box to the number of paritions.
    ///
    map<string, uint16>   _partition_input;
};


BOREALIS_NAMESPACE_END
#endif                       // DIAGRAM_H

#ifndef  DEPLOYDIAGRAM_H
#define  DEPLOYDIAGRAM_H

#include  "RegionalCatalog.h"
#include  "MedusaClient.h"


BOREALIS_NAMESPACE_BEGIN

class DeployDiagram : public RegionalCatalog
{
  public:
    DeployDiagram() {};
   ~DeployDiagram() {};

    ///  Deploy an initial Diagram on Borealis nodes.
    ///
    Status  deploy();


    /// Establish the listener endpoint for the Head.
    ///
    void  set_head( string  endpoint )
    {   _head = form_endpoint( endpoint, DEFAULT_HEAD_PORT );
    }


    /// Get the listener endpoint for the Head (empty if not established).
    ///
    string  get_head()
    {   return  _head;
    }


    ///  Virtual override to bypass the version in the RegionalCatalog class.
    ///
    CatalogBox  *add_box_element(const DOMElement  *box,
                                          Boolean   dynamic = False)
                           throw(AuroraBadEntityException);

    void deploy_conger(string conger_config)
    {
        _node->_client->add_conger_string(conger_config);
    }

  private:

    typedef map<Name, CatalogSubscription> DeployOutput;

    /// Elements pending deployment.
    ///
    struct  DeployNode
    {
        list<Name>             _box_infer;  // Boxes to be inferred.
        list<Name>             _box_deploy; // Boxes in deployment order.
        vector<CatalogView *>  _view;       // Connection Point Views per node.
        DeployOutput           _output;     // Subscriptions on each node.
        ptr<MedusaClient>      _client;     // Current client for deployment.
        Boolean                _region;     // Some regions contain this node.
    };

    typedef map<string, DeployNode> DeployMap;      // node -> DeployNode.

    typedef set<CatalogSchema*>     SchemaSet;

    typedef map<string, SchemaSet>  NodeSchemaSet;  // schema -> SchemaSet

    typedef set<CatalogStream*>     StreamSet;

    typedef map<string, StreamSet>  NodeStreamSet;  // stream -> StreamSet



    ////////////////////////////////////////////////////////////////////////////
    ///
    ///  Determine the node for which each element is to be deployed.
    ///
    Status  locate();


    ///  Determine the schemas for intermediate streams.
    ///  List boxes in upstream deployment order.
    ///  Check for cyclic deployment.
    ///
    Status  infer_schema();


    ///  Deploy all pending elements to nodes.
    ///
    Status  deploy_pending_node( DeployMap::iterator  Node );


    ///  Connect to a client node and establish reflections to the Head.
    ///
    Status  open_client( /// Target node in _pending.
                         string   endpoint);


    ///  Disconnect a client node.
    ///
    Status  close_client();


    ///  Deploy all regions including a node.
    ///
    Status  deploy_region(/// Target node in _deploy.
                          string   endpoint);


    ///  Deploy schemas for all input streams to a single node (in _node).
    ///
    Status  deploy_input(/// Primary or first node in a replicated set.
                         string   endpoint);


    ///  Deploy a single schema a single node (in _node).
    ///
    Status  deploy_schema(/// Schema definition in the catalog.
                          CatalogSchema  *schema );


    ///  Send a single schema as update XML.
    ///
    Status  send_schema(/// Schema definition in the catalog.
                        CatalogSchema  *schema );


    ///  Deploy a single schema a single node (in _node).
    ///
    Status  deploy_table_schema(// Target node in _pending.
                                string   endpoint);


    ///  Deploy all boxes on a node and start them. Endpoint refers to replica
    ///
    Status  deploy_box_node(DeployNode  *deploy, string endpoint);


    ///  Deploy a single box to a node and start it.
    ///  On the first time connect the node and deploy schemas and views.
    ///
    Status  deploy_box( // Box to deploy.
                        CatalogBox  *box,

                        // This is needed with replica_set
                        //
                        string       endpoint);


    ///  Deploy schemas for all output streams to a single node (in _node).
    ///
    Status  deploy_output();


    ///  Deploy all connection point views for a single node.
    ///
    Status  deploy_view();


    /// Create an InetAddress from string argument.
    /// Add a default port if none specified.
    ///
    InetAddress  node_address(/// A node as:  <ip address>[:<port>]
                              string  node );




    ////////////////////////////////////////////////////////////////////////////
    //
    DeployMap             _pending;          // Node names -> objects on each.
    DeployNode           *_node;             // Current node being deployed.
    string                _head;             // Endpoint for the head.

    set<CatalogBox *>     _deployed_box;     // Boxes that have been deployed.

    set<CatalogTable *>   _deployed_table;   // Tables that have been deployed.

    set<CatalogView *>    _deployed_view;    // Views that have been deployed.

    NodeSchemaSet         _deployed_schema;  // Schemas that have been deployed.
                                             //   (node -> schemaset)

    NodeStreamSet         _deployed_stream;  // Intermediate edge streams.
                                             //   (node -> streamset)

};

BOREALIS_NAMESPACE_END
#endif                       // DEPLOYDIAGRAM_H

#ifndef  CATALOG_H
#define  CATALOG_H

#include  "common.h"
#include  "Name.h"
#include  "DataType.h"
#include  "NodeType.h"
#include  "HAMethod.h"

#include  "ConsistencyState.h"

#include  "CatalogQuery.h"
#include  "CatalogView.h"
#include  "CatalogSubscription.h"

#include  <NMSTL/net>

BOREALIS_NAMESPACE_BEGIN


/**
 *  The catalog holds configuration data for each node.
 *  The Catalog class defines the data held in the catalog.
 *  State data resides in node objects and network statistics are managed by
 *  the StatsManager.
 */


class Catalog
{

  public:

    ////////////////////////////////////////////////////////////////////////////
    //
    //  Methods to build a catalog from elements.
    //
    //..........................................................................


    /// Constructor.  Initially in the load state.
    Catalog();


    /// Destructor.
    ~Catalog() {};


    /// Begin a sequence of calls to create a new schema.
    ///
    void  begin_schema(/// Schema to create.
                       ///
                       Name  name)
                 throw(AuroraBadEntityException);



    /// Add a field to a schema in a catalog structure.
    ///
    void  add_field(/// Name of the field.
                    ///
                    string    name,

                    /// The type of the field.
                    ///
                    DataType  type,

                    /// Size is required for string types.
                    ///
                    uint16    size)
             throw(AuroraBadEntityException);


    /// Add a an array definition to a field to a schema in a catalog structure.
    ///
    void  add_array(/// Array declaration.
                    ///
                    string     type,

                    /// Maximum size if a string element; else "0".
                    ///
                    string     size)
              throw(AuroraBadEntityException);


    /// Add a dataflow stream to the catalog.
    ///
    CatalogStream  *add_stream(/// Stream to create.
                               ///
                               Name    name,

                               /// Data types in the stream.
                               ///
                               Name    schema )
                         throw(AuroraBadEntityException);


    /// Add an input stream to the catalog.
    ///
    void  add_input(/// Stream to connect to input.
                    ///
                    string  stream)
              throw(AuroraBadEntityException);


    ///  Add publication of an input stream to the catalog.
    ///
    void  add_publish(/// Name of the input stream.
                      ///
                      string  stream,

                      /// Node receiving the input.
                      ///
                      string  node)
                throw(AuroraBadEntityException);


    CatalogSubscription  add_subscribe(/// Stream to connect to output.
                                       ///
                                       string  stream,

                                       /// Enpoint of the application monitor.
                                       ///
                                       string  endpoint,

                                       /// Number of consecutive tuples that can be lost.
                                       /// Use 0xFFFF for unlimited tuple loss.
                                       ///
                                       uint16  gap)
                                 throw(AuroraBadEntityException);


    /// Add a connection point view to a catalog structure.
    ///
    CatalogView  *add_view(/// Name of the connection point view.
                           ///
                           Name    name,

                           /// Name of the stream to connect to.
                           ///
                           Name    stream,

                           /// Name of a field to order by.
                           /// An empty string if ordering by tuple.
                           ///
                           string  field,

                           /// Field order value or start value of a range.
                           ///
                           uint32  start,

                           /// End value of a range.
                           /// Ignored if ordering by field.
                           ///
                           uint32  end,

                           ///Radius value of the ConnectionPoint
                           ///Ignored if not set
                           ///
                           uint32  radius)
                     throw(AuroraBadEntityException);


    /// Add a connection point view to a catalog structure.
    /// Sets the BoxPort for the view
    /// view is "incomplete" until the other fields have been set
    ///
    CatalogView *add_view(/// Name of the connection  point view.
                          ///
                          Name name,

                          /// Name of the box that this view supplies tuples to.
                          ///
                          Name box,

                          /// Name of the stream to connect to.
                          ///
                          Name stream)
                    throw(AuroraBadEntityException);


    /// What is the BoxPort in stream that feeds into box
    /// Used to figure out the BoxPort for cpview
    /// 
    BoxPort  find_box_port_for_view(CatalogStream  *stream, Name box);


    /// Begin a sequence of calls to add a box.
    /// To add a box call:  addBox, (addIn | addOut | addParameter | addTable )+
    ///
    void  begin_box(/// Name of the box.
                    ///
                    Name    name,

                    /// Box operation.
                    ///
                    string  type)
              throw(AuroraBadEntityException);


    /// Connect a stream to the next box input.
    /// Streams must be connect in the order that input ports are assigned.
    ///
    void  add_in(/// Name of the stream to connect.
                 ///
                 Name  stream,

                 /// Queue options.
                 ///
                 CatalogBox::InQueue  in_queue,

                 /// Input is being added as part of a dynamic modification
                 ///
                 Boolean dynamic);


    /// Connect a stream to the next box output.
    /// Streams must be connect in the order that output ports are assigned.
    ///
    void  add_out(/// Name of the stream to connect.
                  ///
                  Name  stream);


    /// Assign a value to a box parameter.
    //
    void  add_parameter(/// Name of the box parameter.
                        ///
                        string  parameter,

                        /// Value to assign the parameter.
                        ///
                        string  value)
                  throw(AuroraBadEntityException);


    /// Associate a table with a box.
    ///
    void  add_table(/// Name of the table.
                    ///
                    Name   table )
              throw(AuroraBadEntityException);


    ///  Access a box and optionally change its location.
    ///
    Status  modify_box_node(/// Name of a box in the catalog.
                            ///
                            Name    name,

                            /// Destination node.
                            ///
                            string  node = "");


    /// Begin a sequence of calls to add a table.
    /// To add a table call:  beginTable, (addKey | addIndex | addParameter )+
    ///
    void  begin_table(/// Name of the table.
                      ///
                      Name    name,

                      /// Schema of the table contents.
                      ///
                      Name    schema)
                throw(AuroraBadEntityException);


    /// Add a field to the key.
    ///
    void  add_key(/// Name of a field include in the key.
                  ///
                  Name    field);


    /// Index a table field.
    ///
    void  add_index(/// Name of a field to be indexed.
                    ///
                    Name  field);


    ///  Access a table and optionally change its location.
    ///
    Status  modify_table_node(/// Name of a table in the catalog.
                              ///
                              Name    name,

                              /// Destination node.
                              ///
                              string  node = "");


    /// Begin a sequence of calls to create a query.
    /// To add a query call:  addQuery, <add query elements>, endQuery
    ///
    void  begin_query(/// Name of the query.
                      ///
                      Name     name)
                throw(AuroraBadEntityException);


    /// Last in a sequence of calls to create a query.
    ///
    void  end_query()
              throw(AuroraBadEntityException);


    /// Just after adding a query, reteive it.
    ///
    CatalogQuery  *get_add_query()
    {   return  _add_query;
    }


    ///  Signal there is no wrapper query for a Box or Table.
    ///
    void  clear_add_query()
    {   _add_query = NULL;
    }
    
    
    ////////////////////////////////////////////////////////////////////////////
    //
    //  Methods to validate elements.
    //
    //..........................................................................


    /// If a prior declaration can be completed.
    ///
    Status  prior_element();


    /// Close any prior declarations.
    ///
    Status  close_prior();


    /// See if a schema can be added to a catalog structure.
    ///
    Status  validate_schema(/// Schema to create.
                            ///
                            Name      name);


    /// See if a field can be added to a schema in a catalog structure.
    ///
    Status  validate_field(/// Name of the field.
                           ///
                           string     name,

                           /// The type of the field.
                           ///
                           DataType  type,

                           /// Size is required for string types.
                           ///
                           uint16    size);


    /// See if a dataflow stream can be added to the catalog.
    ///
    Status  validate_stream(/// Stream to create.
                            ///
                            Name    name,

                            /// Data types in the stream.
                            ///
                            Name    schema);


    /// See if an input or output stream can be added to the catalog.
    ///
    Status  validate_input_output(/// Stream to connect to input or output.
                                  ///
                                  string  stream);


    // anjali - begin
    //
    /// See if the number of input streams passed to a given box are correct.
    ///
    Status  validate_number_of_in();


    /// See if schema for all input streams  match.
    ///
    Status  validate_more_in(/// First input stream connected to box.
                             ///
                             Name  stream);


    /// See if number of output streams exceed the allowed limit
    ///
    Status  validate_number_of_out();


    /// See if schema for output stream matches with the schema of input stream
    ///
    Status  validate_out(/// Name of the output stream.
                         ///
                         Name  stream);


    /// See if the table tage is present for box types insert, update or select
    ///
    Status validate_table_present();


    /// See if the number of parameter passed to a box are correct.
    ///
    Status  validate_number_of_param();


    /// See if the parameter passed is appropriate for the given box
    ///
    Status  validate_parameter(/// Parameter name.
                               ///
                               string  parameter,

                               /// Value of a parameter.
                               ///
                               string  value);


    /// See if the required parameters for a specific box are present.
    ///
    Status  validate_parameter_present();

    //end


    /// See if a connection point view can be added to a catalog structure.
    ///
    Status  validate_view(/// Name of the connection point view.
                          ///
                          Name    name,

                          /// Name of the stream to connect to.
                          ///
                          Name    stream,

                          /// Name of a file to order by.
                          /// An empty string if ordering by tuple.
                          ///
                          string   field,

                          /// Field order value or start value of a range.
                          ///
                          uint32   start,

                          /// End value of a range.
                          /// Ignored if ordering by field.
                          ///
                          uint32   end);


    /// See if a connection point view can be added to a catalog structure.
    ///
    Status validate_view(/// Name of the connection point view.
                         ///
                         Name    name);


    /// See if a box can be added to the catalog.
    ///
    Status  validate_box(/// Name of the box.
                         ///
                         Name    name,

                         /// Box operation.
                         ///
                         string  type);


    /// See if a table can be added to the catalog.
    ///
    Status  validate_table(/// Name of the table.
                           ///
                           Name    name,

                           /// Schema for the table elements.
                           ///
                           Name    schema);


    /// See if a a query can be added to the catalog.
    ///
    Status  validate_query(/// Name of the query.
                           ///
                           Name    name);



    ////////////////////////////////////////////////////////////////////////////
    //
    //  Catalog data structures.
    //
    //..........................................................................


    typedef   map<Name, CatalogStream >      StreamMap;
    typedef   map<Name, CatalogBox    >      BoxMap;
    typedef   map<Name, CatalogTable  >      TableMap;
    typedef   map<Name, CatalogView   >      ViewMap;
    typedef   map<Name, CatalogQuery  >      QueryMap;


    /// Configuration of the query diagram.
    ///
    struct Topology
    {
        CatalogSchema::SchemaMap    _schema_map;       // Tuple Description
        StreamMap       _stream_map;       // Stream
        BoxMap          _box_map;          // Box
        TableMap        _table_map;        // Tables
        QueryMap        _query_map;        // Queries - Move to DeployDiagram.
    };


    // Network oriented approach to inter-site connections.
    typedef   map<Name, CatalogSubscription>            SubscriptionMap;
    typedef   map<InetAddress, SubscriptionMap>         StreamLinkMap;


    // Magda added - Application-level state information
    // These are all defined inside ConsistencyState.h
    //typedef   map<Name,ConsistencyState>              PerOutputStreamStates;
    //typedef   map<InetAddress,ConsistencyState>         PerNodeStreamStates;
    //typedef   map<Name, PerNodeStreamStates  >          StreamStatesIndex;
    //typedef   map<InetAddress, PerOutputStreamStates >  RStreamStatesIndex;


    /// Network connections to other entities.
    ///
    struct Connections
    {
        StreamLinkMap      _publisher_map;    // Subscriptions (input streams)
        StreamLinkMap      _subscription_map; // Subscriptions (input paths)
        StreamLinkMap      _subscriber_map;   // Subscribers  (output paths)

        /// Because a node has only one subscription per input stream, we can have
        /// a direct access from a stream name to a subscription
        ///
        SubscriptionMap    _direct_subscription_map; 

        /// Magda added
        /// For each stream, keep the state of the stream per replica that produces it
        /// The global catalog can keep, if needed, the info about all streams
        /// Individual nodes only need to keep info about 
        /// the output streams of their upstream neighbors (and their replicas)
        ///
        StreamStatesIndex   _stream_states_map;

        /// For each processing node, keep the states of all its output streams
        /// The global catalog can keep, if needed, the info about all processing nodes
        /// Individual nodes only need to keep info about 
        /// the output streams of their upstream neighbors
        ///
        //RStreamStatesIndex  _rstream_states_map; XXX Can get away without this

        /// The states of local outut streams
        ///
        PerOutputStreamStates _local_stream_states_map;
    };


    ////////////////////////////////////////////////////////////////////////////
    //
    //  Getters and setters.
    //..........................................................................


    CatalogSchema::SchemaMap  *get_schema_map()
    {   return  &(_topology._schema_map);
    }


    /// Makes a Copy of the given schema and adds it to the catalog.
    /// Used to populate intermediate streams.
    ///
    void add_schema(Name schema_name, CatalogSchema  &Schema)
    {    _topology._schema_map[schema_name].clone(Schema);
    }


    CatalogSchema *get_schema(Name schema_name)
    {
        if (_topology._schema_map.find(schema_name) !=
            _topology._schema_map.end())
        {
            return  &(_topology._schema_map[schema_name]);
        }

        return  NULL;
    }


    /// Needed to update intermediate streams' schemas.
    ///
    CatalogStream  *get_stream( Name  stream_name )
    {
        if ( _topology._stream_map.find( stream_name )
          != _topology._stream_map.end() )
        {
            return &(_topology._stream_map[stream_name]);
        }

        return  NULL;
    }


    StreamMap  *get_stream_map()
    {   return  &(_topology._stream_map);
    }


    CatalogStream  *set_stream_schema(Name stream_name,
                                      CatalogSchema  *stream_schema)
    {
        CatalogStream& stream = _topology._stream_map[stream_name];
        stream.set_stream_name(stream_name);
        stream.set_stream_schema(stream_schema);

        return  &stream;
    }


    /// Access all output stream subscriptions.
    ///
    StreamLinkMap  *get_subscriber_map()
    {
        return  &_overlay._subscriber_map;
    }


    /// Access all input stream publications.
    ///
    StreamLinkMap  *get_publisher_map()
    {
        return  &_overlay._publisher_map;
    }


    /// Returns a copy of the map with the states of all local streams
    ///
    PerOutputStreamStates  get_local_stream_state_map()
    {
        return  _overlay._local_stream_states_map;
    }


    /// Returns a reference to all replicas of the given stream
    /// Precondition: stream_name must already be known!
    ///
    PerNodeStreamStates  &get_all_input_stream_replicas(Name stream_name)
    {
        return _overlay._stream_states_map[stream_name];
    }


    /// Return the consistency state for a stream produced by a given node
    /// of UNKNOWN if nothing is known about the replica
    /// Precondition: stream name must already be known
    ///
    ConsistencyState  get_input_stream_state(Name stream_name, InetAddress src_node)
    {
        if (_overlay._stream_states_map.find(stream_name) != _overlay._stream_states_map.end())
        {   return  _overlay._stream_states_map[stream_name][src_node];
        } 
        else
        {   return  ConsistencyState::UNKNOWN;            
        }
    }


    /// Add a new stream to the list of remotely located streams
    /// that need to be monitored
    ///
    void  add_remote_input_stream(Name stream_name, InetAddress src_node);


    /// Start monitoring a local stream that either is used as an 
    /// output stream for some downstream clients or *could* be used
    /// as an output stream at some point and node needs to be ready
    ///
    void  monitor_local_stream(Name stream_name);


    /// Update the consistency of ALL locally running streams.
    /// We currently set all the states to the same value so we don't need
    /// to be able to set them one at the time
    ///
    void  update_local_stream_states(ConsistencyState new_state);


    /// Update the consistency of remotely produced streams used as input
    /// Returns true if the update was performed.
    /// Does nothing if the stream is not already known and returns false
    ///
    bool  update_input_stream_state(Name stream_name, InetAddress src_node, ConsistencyState new_state);


    /// Query accessors.
    ///
    QueryMap  *get_query_map()
    {   return  &(_topology._query_map);
    }


    CatalogQuery  *get_query(Name  query_name)
    {   if (_topology._query_map.find(query_name) != _topology._query_map.end())
        {   return  &(_topology._query_map[query_name]);
        }

        return  NULL;
    }


    /// Box accessors.
    ///
    BoxMap  *get_box_map()
    {   return  &(_topology._box_map);
    }


    CatalogBox *get_box(Name  box_name)
    {
        if ( _topology._box_map.find(box_name) != _topology._box_map.end() )
        {    return  &(_topology._box_map[box_name]);
        }

        return  NULL;
    }


    /// Add a box.
    ///
    CatalogBox *add_box(CatalogBox &box) throw(AuroraBadEntityException);


    /// Remove a box.
    ///
    void remove_box(Name box_name, set<Name> streams_to_preserve = set<Name>())
              throw(AuroraBadEntityException);


    /// Remove a filter box from the catalog.
    ///
    void  remove_filter(Name   box_name)
                  throw(AuroraBadEntityException);


    /// Table accessors
    ///
    TableMap *get_table_map()
    {   return  &(_topology._table_map);
    }


    CatalogTable *get_table(Name table_name)
    {
        TableMap::iterator found_table = _topology._table_map.find(table_name);

        if (found_table != _topology._table_map.end())
        {   return  &(found_table->second);
        }
        
        return  NULL;
    }


    /// View accessors
    ///
    ViewMap  *get_view_map()
    {   return  &_view_map;
    }


    /// Subscription accessors.
    ///
    CatalogSubscription  *get_subscription(Name  stream_name)
    {
        if ( _overlay._direct_subscription_map.find(stream_name) !=
             _overlay._direct_subscription_map.end())
        {
            return  &(_overlay._direct_subscription_map[stream_name]);
        }

        return  NULL;
    }


    CatalogSubscription  *get_subscription(InetAddress  subscription_address,
                                           Name stream_name)
    {
        if (_overlay._subscription_map.find(subscription_address) !=
            _overlay._subscription_map.end() )
        {
            SubscriptionMap &site_in =
                _overlay._subscription_map[subscription_address];

            if ( site_in.find(stream_name) != site_in.end() )
            {
                return( &(site_in[stream_name]) );
            }
            else
            {   return  NULL;
            }
        }

        return  NULL;
    }


    /// Add a subscription to a remote node's stream.
    ///
    void  add_subscription(CatalogSubscription& subscription)
                     throw(AuroraBadEntityException);


    /// Remove a subscription to a remote node's stream.
    ///
    void  remove_subscription(CatalogSubscription  *subscription)
                        throw(AuroraBadEntityException);


    /// Subscriber accessors
    ///
    CatalogSubscription  *get_subscriber(InetAddress  subscriber_address,
                                                Name  stream_name)
    {
        if (_overlay._subscriber_map.find(subscriber_address) !=
            _overlay._subscriber_map.end())
        {
            SubscriptionMap &site_out =
                _overlay._subscriber_map[subscriber_address];

            if (site_out.find(stream_name) != site_out.end())
            {
                return  &(site_out[stream_name]);
            }
        }

        return  NULL;
    }


    /// Add a subscriber to whom we'll send tuples.
    ///
    void add_subscriber(CatalogSubscription   &subscription)
                  throw(AuroraBadEntityException);


    /// Remove a subscriber so we no longer send them tuples.
    ///
    Status  remove_subscriber(CatalogSubscription  *subscription);


    /// Output the box names and stream names in the catlog
    ///
    string  as_string() const;


  protected:

    CatalogBox::InQueue   _in_queue;     // Default for boxes in this network.


    /// Streams, boxes, schemas, and queries.
    ///
    Topology        _topology;


    /// Connection point views.
    ///
    ViewMap         _view_map;


    /// Network connections.
    ///
    Connections     _overlay;


    ////////////////////////////////////////////////////////////////////////////
    /// The remaining variables maintain the state of additions to the catalog.
    ///.........................................................................


    /// Current schema being added.
    ///
    CatalogSchema          *_add_schema;


    /// Current field index (zero-based).
    ///
    uint16                  _add_field;


    /// Current box being added.
    ///
    CatalogBox             *_add_box;


    /// Current table being added.
    ///
    CatalogTable           *_add_table;


    /// Current view being added.
    ///
    CatalogView            *_add_view;


    /// Current box input number (one-based).
    ///
    uint16                  _add_in;


    /// Current box output number (one-based).
    ///
    uint16                  _add_out;


    /// Current query being created.
    ///
    CatalogQuery           *_add_query;
};


BOREALIS_NAMESPACE_END
#endif                       // CATALOG_H

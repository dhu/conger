#ifndef  CATALOGBOX_H
#define  CATALOGBOX_H

#include "BoxParameter.h"
#include "CatalogStream.h"
#include "CatalogTable.h"
#include "HAType.h"

BOREALIS_NAMESPACE_BEGIN

class CatalogQuery;


/// Configuration data for a box.
///
class CatalogBox
{
  public:
    /// A collection of boxes; either a query.
    /// A query is a set of boxed that can be collectively administered.
    ///
    typedef  vector<CatalogBox *>      BoxStar;


    /// Update queue used only by the Load Shedder.
    ///
    struct InQueue
    {
        // This flag shows if queues on arcs should have update semantics or not.
        // This flag applies to the whole borealis diagram unless overwritten
        // by another flag down in the diagram hierarchy.
        // If the flag is 0, they have the regular append semantics.
        // If the flag is omitted, then it is set to 0 by default.
        // See the application guide for a detailed description.
        //
        Boolean        _update_queue;

        // Type for an update queue indicates its preemption policy.
        // See:  queryProcessor/storageMgr/UpdateQueue.h
        //
        // 0: FIFO, 1: INPLACE,
        // 2: LINECUT_UF_FRONT, 3: LINECUT_UF_MIDDLE,
        // 4: LINECUT_DF_FRONT, 5: LINECUT_DF_MIDDLE,
        // 6: LINECUT_UF_MIDDLE_DETERMINISTIC, 7: LINECUT_DF_MIDDLE_DETERMINISTIC,
        // 8: LINECUT_PRIOR
        //
        int            _queue_type;

        // Name of the key field for an update queue.
        //
        Name           _key;

        InQueue(Boolean  update_queue = False, int queue_type = 0,
                Name     key = "")
            :
            _update_queue(update_queue),
            _queue_type(queue_type),
            _key(key)
        {}
    };


    CatalogBox() : 
        _query(0),  
        _table(0), 
        _all_done(false) {}

    void shallow_copy(const CatalogBox &box);


    /// Name of a box as a string.
    ///
    Name get_box_name() const
    {  return( _name );
    }


    /// Type of box.
    ///
    string  get_box_type()  const
    {   return( _type );
    }


    BoxParameter  *get_box_parameter()
    {   return( &_parameter );
    }


    BoxParameter::ParameterMap  *get_box_parameter_map()
    {   return( _parameter.get_parameter_map() );
    }


    const BoxParameter::ParameterMap  *get_box_parameter_map_const() const
    {   return( _parameter.get_parameter_map_const() );
    }

    CatalogTable  *get_box_table()  const
    {   return(_table);
    }

    void  set_box_table(CatalogTable  *table)
    {   _table = table;
    }

    CatalogStream::StreamStar  *get_box_in()
    {   return(&_in);
    }

    CatalogSchema  *get_in_schema(uint16  port)  const
    {   return(_in[port]->get_stream_schema());
    }

    CatalogStream  *get_in_stream( uint16  port )  const
    {   return(_in[port]);
    }

    CatalogStream::StreamStar  *get_box_out()
    {   return(&_out);
    }

    CatalogSchema  *get_out_schema(uint16  port)  const
    {   return( _out[ port ]->get_stream_schema() );
    }

    CatalogStream  *get_out_stream(uint16  port)  const
    {   return(_out[port]);
    }

    void add_in_stream(CatalogStream  *stream,
                       InQueue         in_queue);

    void reset_in_stream(uint16 port, CatalogStream *stream);

    void remove_in_stream(Name stream);

    void add_out_stream(CatalogStream *stream);

    void remove_out_stream(Name stream);

    Status get_in_stream_index(CatalogStream* in, uint16& idx);

    Status get_out_stream_index(CatalogStream* out, uint16& idx);

    /// See if an In port has an update queue.
    ///
    Boolean  is_update_queue(uint16   port)
    {   return(_in_queue[port]._update_queue);
    }

    // Get the queue type for an In port update queue.
    //
    int get_in_queue_type(uint16 port)
    {
        return(_in_queue[port]._queue_type);
    }

    /// Get the key for an In port update queue.
    ///
    Name  get_in_queue_key(uint16   port)
    {   return(_in_queue[port]._key);
    }


    void  set_out_schema(uint16          port,
                         CatalogSchema  *schema)
    {
        _out[ port ]->set_stream_schema(schema);
        return;
    }

    void  set_box_name_type(Name     name,
                            string   type)
    {
        _name = name;
        _type = type;

        _node  = "";         // Empty for now.  Needs to be set???
        _table = NULL;
        _query = NULL;
    }

    void  set_box_query( CatalogQuery  *query )
    {   _query = query;
    }

    CatalogQuery *get_box_query() const;

    void  set_box_node( string   node )
    {   _node = node;
    }

    string  get_box_node()  const
    {   return( _node );
    }

    /// Establish default queue parameters for a box.
    ///
    void  set_box_in_queue(/// Default queue parameters for the box.
                           CatalogBox::InQueue  in_queue)
    {
       _box_in_queue = in_queue;
    }

    /// Get the default queue parameters for a box.
    ///
    InQueue  get_box_in_queue()
    {
       return(_box_in_queue);
    }

    void remove_in_sink_port(Name in_stream, uint16 port_num);

    void remove_in_sink_port(Name in_stream);

    ///  Infer the type of Box out streams from in streams.
    ///  If an out stream already has a schema it is validated.
    ///  Upon entry, all in streams must have schemas and the box is validated.
    ///
    Status  infer_box_out(/// Map to save the composite schema.
                          CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schemas for a filter box.
    ///
    Status  infer_filter_out(/// Only In stream.
                             CatalogStream  *in,

                             ///  In - First Out stream; schema is optional.
                             /// Out - Unchanged or schema is added.
                             CatalogStream  *out,
 
                             /// Map to save the composite schema.
                             CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schema for a join box.
    ///
    Status  infer_join_out(///  In - Only Out stream; schema is optional.
                           /// Out - Unchanged or schema is added.
                           CatalogStream  *out,

                           /// Map to save the composite schema.
                           CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schema for a map box.
    ///
    Status  infer_map_out(/// Only In stream.
                          CatalogStream  *in,

                          ///  In - Only Out stream; schema is optional.
                          /// Out - Unchanged or schema is added.
                          CatalogStream  *out,

                          // Map to save the composite schema.
                          CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schema for a revisionmap box.
    ///
    Status  infer_revisionmap_out(/// Only In stream.
                          CatalogStream  *in,

                          ///  In - Only Out stream; schema is optional.
                          /// Out - Unchanged or schema is added.
                          CatalogStream  *out,

                          // Map to save the composite schema.
                          CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schema for an aggregate box.
    ///
    Status  infer_aggregate_out(/// Only In stream.
                                CatalogStream  *in,

                                ///  In - Only Out stream; schema is optional.
                                /// Out - Unchanged or schema is added.
                                CatalogStream  *out,

                                /// Map to save the composite schema.
                                CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schema for a revisionaggregate box.
    ///
    Status  infer_revisionaggregate_out(/// Only In stream.
                                CatalogStream  *in,

                                ///  In - Only Out stream; schema is optional.
                                /// Out - Unchanged or schema is added.
                                CatalogStream  *out,

                                /// Map to save the composite schema.
                                CatalogSchema::SchemaMap  *schema_map);


    ///  Infer the out stream schema for an soutput box.
    ///
    Status  infer_soutput_out(/// Only In stream.
                              CatalogStream  *in,

                              /// Control Out stream; schema is empty.
                              CatalogStream  *out,

                              /// Map to save the composite schema.
                              CatalogSchema::SchemaMap  *schema_map);


    /// Infer the output stream schemas for an sunion box
    ///
    Status  infer_sunion_out();
                                 

    ///  Infer the out stream schema for an sql update box.
    ///
    Status  infer_udb_out(/// Map to save the composite schema.
                          CatalogSchema::SchemaMap  *schema_map,

                          /// Only In stream.
                          CatalogStream  *in,

                          ///  In - Optional Out stream; schema is optional.
                          /// Out - Unchanged or schema is added.
                          CatalogStream  *out);


    ///  Infer the out stream schema for Berkely Db box.
    ///
    Status  infer_bdb_out(/// Map to save the composite schema.
                          CatalogSchema::SchemaMap  *schema_map,

                          /// Only In stream.
                          CatalogStream  *in,

                          ///  In - Optional Out stream; schema is optional.
                          /// Out - Unchanged or schema is added.
                          CatalogStream  *out);



    ///  Infer the out stream schema for array processing boxes.
    ///
    Status  infer_array_out(/// Map to save the composite schema.
                            CatalogSchema::SchemaMap  *schema_map,

                            /// Only In stream.
                            CatalogStream  *in,

                            ///  In - Optional Out stream; schema is optional.
                            /// Out - Unchanged or schema is added.
                            CatalogStream  *out);


    ///  Match a stream to the reference stream (unchanged).
    ///  If the stream argument has no schema it is assigned the schema.
    ///
    Status  match_composite_schema(/// Output stream to validate.
                                   CatalogStream  *out,

                                   /// Expected out schema.
                                   CatalogSchema  *composite,

                                   /// Map to save the composite scheam.
                                   CatalogSchema::SchemaMap  *schema_map);


    ///  Match a box Out stream to the reference In stream.
    ///  If the stream argument has no schema it is assigned the schema.
    ///
    Status  match_box_schema(/// Stream to match up.
                             CatalogStream  *in,

                             ///  In - First Out stream; schema is optional.
                             /// Out - Unchanged or schema is added.
                             CatalogStream  *out);
   Status  match_box_schema_two_in(/// Stream to match up.
                             CatalogStream  *in1,
                             CatalogStream  *in2,
                             ///  In - First Out stream; schema is optional.
                             /// Out - Unchanged or schema is added.
                             CatalogStream  *out);


    string as_string() const
    {
        string s;
        s += (_name.as_string() + ", " + _type + ", " + _node);
        return s;
    }
    
    /**
     * Returns the CatalogBoxes directly upstream from this CatalogBox.
     */
    //vector<CatalogBox*> get_upstream_boxes();
    
    /**
     * Returns the CatalogBoxes directly downstream from this CatalogBox.
     */
    //vector<CatalogBox*> get_downstream_boxes();
    
    /**
     * Returns the CatalogBoxes directly upstream/downstream from this CatalogBox.
     */
    //vector<CatalogBox*> get_neighboring_boxes();
    
    /// Returns the name of the CP that is on the input stream
    ///
    string get_cpview(string stream)
    {   return _cpview[stream];
    }

    /// Set the name of the CP on stream to view
    ///
    void set_cpview(string stream, string view)
    {   _cpview[stream] = view;
    }

    /// Modes of processing revision
    /// NONE: default, mode not set yet
    /// DOWNSTREAM: use downstream CP 
    /// UPSTREAM: use upstream CP
    ///
    enum processing_mode 
    {   NONE, DOWNSTREAM, UPSTREAM 
    };

    /// Is this box invertible or not
    /// Invertible is if input tuple(s) can be reconstructed from output tuple
    ///
    //bool is_invertible();

    /// Returns the window_size for Aggreate, 0 for others
    /// 
    //int get_window_size();
    
    /// Returns the anchor CP
    ///
    string get_anchor_cp() 
    {   return _anchor_cp;
    }

    /// Sets the anchor CP by concatenating to existing anchors
    ///
    void set_anchor_cp(string anchor_cp)
    {
    if (_anchor_cp == "") { _anchor_cp = anchor_cp; }
    else { _anchor_cp += "," + anchor_cp; }
    }

    /// Removes anchor CP(s)
    ///
    void clear_anchor_cp()
    {   _anchor_cp = "";
    }
    
    /// Returns the revision processing mode
    ///
    processing_mode get_mode()
    {   return _mode;
    }

    /// Returns the revision processing mode as a string
    ///
    string get_mode_string()
    {
    if (_mode == DOWNSTREAM) { return "DOWNSTREAM"; }
    else if (_mode == UPSTREAM) { return "UPSTREAM"; }
    else { return "NONE"; }
    }

    /// Sets the revision processing mode
    ///
    void set_mode(processing_mode mode)
    {   _mode = mode;
    }

    /// Returns true if the revision processing mode has been set, false otherwise
    ///
    bool is_mode_set() 
    {
    if (_mode != NONE) { return true; }
    else { return false; }
    }
    
    /// Has this CatalogBox been completely set up or not
    ///
    bool is_done() 
    {   return _all_done;
    }

    /// Sets this CatalogBox as not set up yet
    ///
    void not_done()
    {   _all_done = false;
    }

    /// Sets this CatalogBox as set up
    ///
    void done()
    {   _all_done = true;
    }

    ///  Create an XML string for a box.
    ///
    string  form_box_xml(HAType ha_type = HAType::PRIMARY);


    NMSTL_SERIALIZABLE(CatalogBox);
    void freeze(::NMSTL::OSerial &out) const;
    void unfreeze(::NMSTL::ISerial &in);
    NMSTL_DUMMY_SERIALREG(CatalogBox);


  private:
    Name           _name;                // Box name
    string         _type;                // Box type
    string         _node;                // Location of the box.

    CatalogQuery  *_query;               // Query membership or NULL.

    vector<InQueue>    _in_queue;        // Update queue info for Nesime.
    InQueue            _box_in_queue;    // Default for this box.

    CatalogStream::StreamStar     _in;   // Input stream connections.
    CatalogStream::StreamStar     _out;  // Output stream connections.
    BoxParameter   _parameter;           // Box parameters.
    CatalogTable  *_table;               // Associated table, i.e. read/write boxes.
    

    map<string,string>         _cpview;   // view on the input stream, stream name maps to cpview name
                                          // stored here so that the box xml can be recreated later

    processing_mode    _mode;             // Revision processing mode

    string             _anchor_cp;        // anchor CP's delimited by ','

    bool               _all_done;         // is this CatalogBox completely setup or not

        
};

BOREALIS_NAMESPACE_END
#endif                       // CATALOGBOX_H

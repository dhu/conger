#ifndef QUERY_H
#define QUERY_H

#include <NMSTL/serial>
#include <NMSTL/text>
#include <NMSTL/util>

#include "Name.h"
#include "StreamDef.h"
#include "Subscription.h"
#include "Box.h"
#include "TableDescription.h"
#include "BoxPackage.h"

BOREALIS_NAMESPACE_BEGIN

typedef  vector<Box>::const_iterator  box_iterator;
typedef  map<Name, ptr<Object> >      StreamDefs;


/**
 * A query, containing a query name, a vector of boxes, and a list of streams connecting these boxes
 */
class Query : public Object
{
  public:
    static const string OBJECT_TYPE;

  protected:

    MedusaID                   _owner;  // Location where query hunk is running.
    vector<Box>                _boxes;
    vector<TableDescription>   _tables;

    vector<Name>               _input_streams;
    vector<Name>               _output_streams;
    vector<Name>               _intermediate_streams;


    /// RPCs have an error: one can not serialize ptr<StreamDef> only
    /// the pointer to the superclass Object
    ///
    StreamDefs                 _stream_defs;

    /// When this query is setup, please process all possible input history
    ///
    bool                       _with_history;

    BoxPackage                 _snapshot;


  public:

    /// Constructor
    ///
    explicit Query(Name name = Name())
                  : Object(OBJECT_TYPE,name),
                    _with_history(false)  {}


    /// Returns the id of the node running the query.
    ///
    MedusaID get_owner() const { return _owner; }


    /// Sets the id of the node running the query
    ///
    void set_owner(MedusaID owner) { _owner = owner; }


    /// Return the query network.
    ///
    const vector<Box>& get_boxes() const { return _boxes; }


    /// Add a box to this query
    ///
    void add_box(Box b);


    bool used_by_box(Name stream_name);


    /// Remove a box from this query.
    ///
    void remove_box(Name box_name);


    /// Return all tables in the query network.
    ///
    vector<TableDescription>& get_tables() { return _tables; }


    /// Add a table to this query.
    ///
    void add_table(TableDescription t);


    /// Return the existing snapshot, if any.
    ///
    BoxPackage& get_package() { return _snapshot; }


    /// Add a state snapshot to this query.
    ///
    void add_package(BoxPackage box_package) { _snapshot = box_package; }


    void clear_stream_defs() { _stream_defs.clear(); }


    /// Associate a schema with a stream
    ///
    void set_stream_def(Name stream, ptr<StreamDef> s)
    {
        _stream_defs[stream] = s;
    }


    ptr<StreamDef> get_stream_def(Name stream) const
    {
        ptr<Object> obj = lookup(_stream_defs,stream);
        return obj.static_cast_to<StreamDef>();
    }


    /// Returns the set of input streams of this query
    ///
    const vector<Name>  get_input_streams() const
    {   return _input_streams;
    }


    const vector<Name>  get_output_streams() const
    {   return _output_streams;
    }


    const vector<Name>  get_intermediate_streams() const
    {   return _intermediate_streams;
    }


    const vector<Name>  get_non_input_streams() const;
    const vector<Name>  get_all_streams() const;

    bool  with_history()     { return _with_history; }
    void  process_history()  { _with_history = true; }


    /// Find a stream name in one of the sets of streams
    ///
    bool  find(Name what, vector<Name> where) const;


    /// Make a deep copy of the Query.  (Normally, copying makes only a shallow copy.)
    ///
    Query  clone() const;


    /// Split query
    ///
    static vector<Query> split_query(Query      query_to_split,
                                     map< Name, vector<Name> >  boxes_per_hunk);


    /// Merge query
    ///
    static Query merge_query(vector<Query> queries, Name new_query_name);


    NMSTL_SERIAL_SUBCLASS(Query, Object,
              << _owner << _boxes << _tables << _snapshot
              << _input_streams << _output_streams << _intermediate_streams
              << _stream_defs << _with_history);
};



/// A query status - whether the query is closed, setup, running, stopped, deleted, etc.
///
struct QueryStatus
{
    enum State
    {
        NONE,
        CLOSED,
        SETUP,
        RUNNING,
        STOPPED,
        DELETED,
        MOVED
    };

    int       _state;


    /// When State is moved, argument can indicate new location
    ///
    MedusaID  _new_location;


    string as_string() const
    {
        switch (_state)
        {
            case NONE:    return "NONE";
            case CLOSED:  return "CLOSED";
            case SETUP:   return "SETUP";
            case RUNNING: return "RUNNING";
            case STOPPED: return "STOPPED";
            case DELETED: return "DELETED";
            case MOVED: return "MOVED";
        }

        return "QueryStatus{???}";
    }

    QueryStatus(State state = NONE, MedusaID new_location = MedusaID())
        : _state(state),
         _new_location(new_location)
    {}

    NMSTL_SIMPLY_SERIALIZABLE(QueryStatus, << _state << _new_location);
    NMSTL_COMPARE_OPS(QueryStatus, _state);
};

BOREALIS_NAMESPACE_END

#endif                              // QUERY_H

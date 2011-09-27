#ifndef LMQueryNetwork_H
#define LMQueryNetwork_H

#include "LMCommon.h"
#include "LMBox.h"
#include "LMArc.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * A LMQueryNetwork class implements a continuouse query query graph.
 * The vetices in this graph are LMBoxes and the arcs in this graph are LMArcs.
 */
class LMQueryNetwork
{
public:
    typedef enum
    {
        INVALID_ID = 0
    } Exception;


public:
    /**
     * Returns the singleton of the LMQueryNetwork
     */
    static LMQueryNetwork* get_singleton()
    {
        if (_singleton == 0)
          _singleton = new LMQueryNetwork();
        return _singleton;
    }

    /**
     * Destroy the LMBoxes and LMArcs when the LMQueryNetwork is deleted
     */
    ~LMQueryNetwork();

    /**
     * Returns all LMBoxes in this QueryNetwork.
     * @return all LMBoxes in this QueryNetwork.
     */
    const LMBoxMap& get_boxes() const {return _box_map;}

    /**
     * Returns all LMArcs in this LMQueryNetwork.
     * @return all LMArcs in this LMQueryNetwork.
     */
    const LMArcMap& get_arcs() const {return _arc_map;}

    /**
     * Returns a pointer to a LMBox with given ID
     * Returns 0 if no LMBox has the given ID
     * @param id the ID of the box
     * @return a pointer to a LMBox with given ID, 0 if no LMBox has the given ID
     */
    LMBox* get_box(const IdType& id) const;

    /**
     * Returns an arc with given ID
     * Returns 0 if no arc has the given ID
     * @param id the ID of the arc
     * @return an arc with given ID
     *         0 if no arc has the given ID
     */
    LMArc* get_arc(const IdType& id) const;

    /**
     * Returns the input arcs.
     * @return the input arcs.
     */
    const LMArcMap& get_input_arcs() const {return _input_arcs;};

    /**
     * Returns the output arcs.
     * @return the output arcs.
     */
    const LMArcMap& get_output_arcs() const {return _output_arcs;};

    /**
     * Returns true if the arc with given ID is an input arc
     * @param id ID of the given arc
     * @return true if the arc with given ID is an input arc
     */
    bool is_input_arc(const IdType& id) const
    {
        return _input_arcs.find(id) != _input_arcs.end();
    }

    /**
     * Returns true if the arc with given ID is an output arc
     * @param id ID of the given arc
     * @return true if the arc with given ID is an output arc
     */
    bool is_output_arc(const IdType& id) const
    {
        return _output_arcs.find(id) != _output_arcs.end();
    }

    /**
     * Returns true if the stream with given ID is an output stream
     * @param id ID of the given stream
     * @return true if the stream with given ID is an output stream
     */
    bool is_output_stream(const IdType& id) const;

    /**
     * Returns true if two given LMBoxes are connected
     * @param box1_id ID of the first LMBox
     * @param box2_id ID of the second LMBox
     * @return true if two given LMBoxes are connected
     */
    bool is_connected(const IdType& box1_id, const IdType& box2_id) const;

    /**
     * Returns the tuple size of a given LMArc
     * Returns 0 if the arc is not found
     * @param id ID of the given LMArc
     * @return the tuple size of a given LMArc
     */
    unsigned int get_arc_tuple_size(const IdType& id) const;

    /**
     * Returns the IDs of all arcs that correspond to the same stream
     * @param stream_id ID of the stream
     * @return the IDs of all arcs that correspond to the same stream
     * @throws INVALID_ID if the given stream ID is not valid
     */
    const IdSet& get_arc_ids(const IdType& stream_id) const throw(Exception);

    /**
     * Adds a new LMBox to this QueryNetwork.
     * @param box new LMBox to be added
     */
    void add_box(LMBox* box) { _box_map[box->get_id()] = box; }

    /**
     * Adds a new LMArc to this QueryNetwork.
     * @param arc new LMArc to be added
     */
    void add_arc(LMArc* arc);

    /*
     * Add an entry in stream ID to arc ID map
     * Each stream can map to multiple arcs. 
     * This mapping is mained by the LMQueryNetwork
     * @param stream_id ID of a stream.
     * @param arc_id ID of an arc
     */
    void add_stream_idarc_idmap_entry(
            const IdType& stream_id, const IdType& arc_id)
    {
        _stream_id_arc_id_map[stream_id].insert(arc_id);
    }

    /**
     * Clear all LMBoxes and LMArcs
     */
    void clear();


    /**
     * Output a LMQueryNetwork to an output stream
     * @param os output stream
     * @param query_network given QueryNetwork
     * @return os output stream
     */
    friend ostream& operator<<(ostream& os, const LMQueryNetwork& query_network);

private:
    /**
     * Creates an empty query network. 
     * This constructor is private for the singleton implementation
     */
    LMQueryNetwork() {}

    /**
     * The singleton of the LMQueryNetwork
     */
    static LMQueryNetwork* _singleton;

    /**
     * A map of LMBoxes.
     */
    LMBoxMap _box_map;

    /**
     * A map of LMArcs.
     */
    LMArcMap _arc_map;

    /**
     * A map of InputArcs
     */
    LMArcMap _input_arcs;

    /**
     * A map of OutputArcs
     */
    LMArcMap _output_arcs;

    /**
     * A map from stream ID to all the ID of all the arcs that maps to it
     */
    map<IdType, IdSet> _stream_id_arc_id_map;

};

BOREALIS_NAMESPACE_END
#endif



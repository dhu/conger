#ifndef LMArc_H
#define LMArc_H

#include "LMCommon.h"

BOREALIS_NAMESPACE_BEGIN

class LMBox;

/**
 * An LMArc is a directed link between two LMBoxes 
 */
class LMArc
{

public:

    /**
     * Constructs a new LMArc.
     * @param id the identifier of this LMArc.
     * @param source_box the source box of this LMArc
     * @param sourcePort the port index of the source box.
     * @param destination_box the destination box of this LMArc
     * @param destination_port the port index of the destination box.
     * @param tuple_size the size (bytes) of the tuples to be transfered on this arc
     */
    LMArc(
            const IdType& id,
            LMBox* source_box,
            PortId source_port,
            LMBox* destination_box,
            PortId destination_port,
            unsigned int tuple_size):
        _id(id),
        _source_box(source_box),
        _source_port(source_port),
        _destination_box(destination_box),
        _destination_port(destination_port),
        _tuple_size(tuple_size){}

    /**
     * Returns the identifier of this LMArc.
     * @return the identifer of this LMArc.
     */
    const IdType& get_id() const {return _id;};

    /**
     * Returns the source box of this LMArc.
     * @return the source box of this LMArc.
     */
    LMBox* get_source_box() const {return _source_box;};

    /**
     * Returns the destination box of this LMArc.
     * @return the destination box of this LMArc.
     */
    LMBox* get_destination_box() const {return _destination_box;};

    /**
     * Returns the port index of the source box.
     * @return the port index of the source box.
     */
    const PortId& get_source_port() const {return _source_port;};

    /**
     * Returns the port index of the destination box.
     * @return the port index of the destination box.
     */
    const PortId& get_destination_port() const {return _destination_port;};

    /**
     * Returns the tuple size
     * @return the tuple size
     */
    unsigned int get_tuple_size() const { return _tuple_size; }


    /**
     * Output to ostream
     */
    friend ostream& operator<<(ostream& os, const LMArc& arcs);

private:

    /**
     * The id of this LMArc.
     */
    IdType _id;

    /**
     * The source box of this LMArc.
     */
    LMBox* _source_box;

    /**
     * The port index of the source box.
     */
    PortId _source_port;

    /**
     * The destination box of this LMArc.
     */
    LMBox* _destination_box;

    /**
     * The port index of the destination box.
     */
    PortId _destination_port;

    /**
     * The size of the tuples on this LMArc
     */
    unsigned int _tuple_size;

};

typedef vector<LMArc*> LMArcVect;

typedef map<IdType, LMArc*> LMArcMap;

typedef set<LMArc*> LMArcSet;

/**
 * Output a vector of LMArcs to an output stream
 * @param os output stream
 * @param arcs a vector of LMArcs 
 * @return os output stream
 */
ostream& operator<<(ostream& os, const LMArcVect& arcs);

/**
 * Output a map of LMArcs to an output stream
 * @param os output stream
 * @param arcs a map of LMArcs 
 * @return os output stream
 */
ostream& operator<<(ostream& os, const LMArcMap& arcs);

/**
 * Output a set of LMArcs to an output stream
 * @param os output stream
 * @param arcs a set of LMArcs 
 * @return os output stream
 */
ostream& operator<<(ostream& os, const LMArcSet& arcs);

BOREALIS_NAMESPACE_END
#endif



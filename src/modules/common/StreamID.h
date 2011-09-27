#ifndef MEDUSA_STREAMID_H
#define MEDUSA_STREAMID_H

#include <xercesc/dom/DOM.hpp>
#include "xercesDomUtil.h"
#include "Name.h"
#include "MedusaID.h"
#include "Object.h"

MEDUSA_NAMESPACE_BEGIN

/**
 * Immutable unique stream identifier.
 * Identifies a stream with a Name and
 * a set of connection point identifiers
 * that uniquely identify the connection points
 * directly upstream from this stream
 */
class StreamID
{

 public:

    StreamID(Name name = Name(), vector<Name> cp = vector<Name>())
        : _name(name), _cp_ids(cp)
        {
    }

    StreamID(Name name, Name cp)
        : _name(name)
        {
        _cp_ids.push_back(cp);
    }

    operator const void *() const
    {
        return _name ? 0 : this;
    }


    const Name get_name() const { return _name; }

    const vector<Name> get_cp_ids() const { return _cp_ids; }

    string as_string() const
    {
        return string() +
            "StreamID{name=" + _name +
            ",cp_ids=" + _cp_ids + "}";
    }

    bool operator != (const StreamID& other) const
    {
        return ! (*this == other);
    }

    bool operator == (const StreamID& other) const
    {
        if ((_name != other.get_name())
            || (_cp_ids.size() != other.get_cp_ids().size()))
            {
            return false;
        }

        for (int i = 0; i < (int)_cp_ids.size(); ++i)
        {
            if ( _cp_ids[i] != other.get_cp_ids()[i] )
            {
                return false;
            }
        }
        return true;

    }


    /// This method is needed to use StreamID as key in a map
    bool operator < (const StreamID& other) const
    {

        // First try to order on stream names alone
        if ( _name != other.get_name() )
        {
            return ( _name < other.get_name() );
        }

        // If the same, then order on cp ids
        for (int i = 0; i < (int)_cp_ids.size(); ++i)
        {
            if ( _cp_ids[i] != other.get_cp_ids()[i] )
            {
                return _cp_ids[i] < other.get_cp_ids()[i];
            }
        }
        return false;

    }


    NMSTL_SIMPLY_SERIALIZABLE(StreamID, << _name << _cp_ids);

 private:
    Name _name;
    vector<Name> _cp_ids;


};
NMSTL_TO_STRING(StreamID)

MEDUSA_NAMESPACE_END

#endif

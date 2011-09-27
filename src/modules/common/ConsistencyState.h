#ifndef CONSISTENCYSTATE_H
#define CONSISTENCYSTATE_H

#include <NMSTL/net>

BOREALIS_NAMESPACE_BEGIN

/**
 * Possible consistency states. These states
 * are assigned to streams. Currently, we also
 * use a ConsistencyState to denote the state of
 * the node as a whole, but the stream state give us more
 * fine grained information.
 */
class ConsistencyState
{
 public:
    enum State
    {
        STABLE = 0,
        UP_FAILURE,
        FAILURE,
        STABILIZATION,
	UNKNOWN
    };

    ConsistencyState(State s = STABLE)
        : _state(s)
    {
    }

    string as_string( ) const
    {
        switch (_state)
        {
        case STABLE: 
            return string("stable");
        case UP_FAILURE:
            return string("up_failure");
        case FAILURE:
            return string("failure");
        case STABILIZATION:
            return string("stabilization");

        default: return string("unknown state");

        }
    }

    bool operator==(const ConsistencyState& other_state) const
    {
        return (other_state._state == _state);
    }

    bool operator!=(const ConsistencyState& other_state) const
    {
        return (other_state._state != _state);
    }


    NMSTL_SIMPLY_SERIALIZABLE(ConsistencyState, << _state);

 private:
    //State _state;
    int32 _state; // Cannot use the enumeration as type because of serialization limitations
    
};


/// Data structures used to hold state information indexed first by stream then by
/// replica producing it or the other way around.
typedef map<Name,ConsistencyState>                PerOutputStreamStates;
typedef map<InetAddress,ConsistencyState>         PerNodeStreamStates;

typedef map<Name, PerNodeStreamStates  >          StreamStatesIndex;
typedef map<InetAddress, PerOutputStreamStates >  RStreamStatesIndex;


NMSTL_TO_STRING(ConsistencyState);

BOREALIS_NAMESPACE_END

#endif

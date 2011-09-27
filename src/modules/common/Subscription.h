
#ifndef MEDUSA_SUBSCRIPTION_H
#define MEDUSA_SUBSCRIPTION_H

#include "Name.h"
#include "MedusaID.h"
#include "Object.h"
#include "StreamDef.h"

MEDUSA_NAMESPACE_BEGIN;

/// A subscription to a remote stream.
class Subscription : public Object
{
 public:
    static const unsigned int ADD = 1;
    static const unsigned int REMOVE = 2;

    // Type of subscription
    static const unsigned int RPC = 1;
    static const unsigned int FAST = 2;

    static const string OBJECT_TYPE;

    /// The name of the stream for backward compatibility
    Name _stream;
    /// A more complete stream description (including upstream CPs)
    StreamDef _stream_def;

    /// The transport mechanism to use for this stream ("tcp" or "xml").
    string _transport; // TCP or XML

    /// The Medusa ID of the node to receive events.
    MedusaID _dest;

    /// The name of the EventSink to receive the events (i.e., the name
    /// to which the EventSink is bound via RPC)
    string _name;

    /// Whether a plain TCP or an RPC subscription
    unsigned int _type;

    /**
     * True if client desires to get all the history on this stream,
     * false if client wants to start getting data from the most recent tuple
     * Default value is false
     */
    bool _with_history;

    /**
     * If client desires the history, it may specify the upstream bound
     * of the history that it wants. Maybe the client wants only the last
     * few tuples rather than all tuples in our buffer
     */
    string _first_tuple;

    /// Null constructor.
    Subscription() : Object(OBJECT_TYPE) {}

    /// Constructor.  (See description of fields for details.)
    Subscription(Name stream, string transport, MedusaID dest, string name,
                 int type = RPC, bool hist = false, string first_tuple = string()) :
        Object(OBJECT_TYPE, Name(to_string(stream)+string("-")+to_string(dest)+to_string("-")+transport+to_string("-")+name)),
        _stream(stream), _stream_def(stream), _transport(transport),
        _dest(dest), _name(name), _type(type), _with_history(hist), _first_tuple(first_tuple)
        {
    }

    Subscription(StreamDef stream_def, string transport, MedusaID dest, string name,
                 int type = RPC, bool hist = false, string first_tuple =  string()) :
        Object(OBJECT_TYPE, Name(to_string(stream_def.get_name())+string("-")+to_string(dest)+to_string("-")+transport+to_string("-")+name)),
        _stream(stream_def.get_name()), _stream_def(stream_def), _transport(transport),
        _dest(dest), _name(name), _type(type), _with_history(hist), _first_tuple(first_tuple)
        {
    }

    /// Returns a string representation of the subscription.
    string as_string() const
    {
        return string() + "Subscription{stream_def=" + _stream_def + "; transport=" + _transport +
            "; dest=" + _dest + "; name=" + _name + "; type=" + _type + "}";
    }

    NMSTL_COMPARE_FIELDS_5(Subscription, _stream, _transport, _dest, _name, _type);
    NMSTL_SERIAL_SUBCLASS(Subscription, Object, << _stream << _stream_def << _transport << _dest << _name << _type << _with_history << _first_tuple);
};



MEDUSA_NAMESPACE_END;

#endif

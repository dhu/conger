#ifndef MEDUSA_STREAMDEF_H
#define MEDUSA_STREAMDEF_H

#include "Name.h"
#include "MedusaID.h"
#include "Object.h"
#include "Schema.h"
#include "Partition.h"
#include "StreamID.h"

MEDUSA_NAMESPACE_BEGIN

// --------------------------------------------------
/**
 * StreamDefs are mutable.
 * Defines the properties of a stream. These properties
 * include the StreamID (name and upstream CPs), the
 * schema of the tuples on that stream, and the partition
 * (i.e., currently it is the stream location)
 */
class StreamDef : public Object
{
  public:
    static const string OBJECT_TYPE; // Value of this const is set in Object.cc

    StreamDef() : Object(OBJECT_TYPE) {}

    explicit StreamDef(Name name, Schema schema = Schema(),
                       const Partition& p = Partition()) :
        Object(OBJECT_TYPE, name),
        _id(StreamID(name)),
        _schema(schema),
        _partition(p)
        {
    }

    explicit StreamDef(StreamID stream_id,
                       Schema schema = Schema(),
                       const Partition& p = Partition()) :
        Object(OBJECT_TYPE, stream_id.get_name()),
        _id(stream_id),
        _schema(schema),
        _partition(p)
        {
    }

    const Partition& get_partition() const
    {
        return _partition;
    }

    void set_partition(const Partition& p)
    {
        _partition = p;
    }

    const Schema& get_schema() const
    {
        return _schema;
    }

    void set_schema(const Schema& sch)
    {
        _schema = sch;
    }

    const StreamID& get_id() const
    {
        return _id;
    }

    void set_id(const StreamID& id)
    {
        _id = id;
    }

    string as_string() const;
    string repr() const;
    string as_xml(bool with_schema=true) const;
    string as_aurora_xml(bool with_schema=true) const;

    bool operator == (const StreamDef& other) const
    {
        return (_id == other.get_id());
    }

    bool operator != (const StreamDef& other) const
    {
        return (_id != other.get_id());
    }

    NMSTL_SERIAL_SUBCLASS(StreamDef, Object, << _id << _schema << _partition);

 private:
    StreamID _id;
    Schema _schema;
    Partition _partition;

};

NMSTL_TO_STRING(StreamDef);

// When notifying observers that a stream definition changes we use this as a return
// type. It gives the new stream definition as well as a list of replicas for this stream
typedef pair<StreamDef, vector< ptr<Object> > > InputStreamInfo;


MEDUSA_NAMESPACE_END

#endif

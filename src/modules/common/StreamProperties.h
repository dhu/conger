#ifndef STREAMPROPERTIES_H
#define STREAMPROPERTIES_H

#include "TupleDescription.h"

BOREALIS_NAMESPACE_BEGIN

/// Properties of a single stream.
struct StreamProperties
{
  public:
    /// Constructor.
    StreamProperties() {}

    /// Constructor.
    StreamProperties( string  name, TupleDescription  schema ) :
                        _name(name), _schema(schema) {}

    /// Returns the name of the stream.
    string get_name() const;

    /// Returns the schema of the stream.
    TupleDescription get_schema() const;

  private:
    string           _name;
    TupleDescription _schema;
};

BOREALIS_NAMESPACE_END
#endif                           // STREAMPROPERTIES_H

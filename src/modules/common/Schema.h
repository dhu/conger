#ifndef SCHEMA_H
#define SCHEMA_H

#include "Object.h"
#include "TupleDescription.h"
#include  <NMSTL/net>

BOREALIS_NAMESPACE_BEGIN

class Schema : public Object
{
  public:
    static const string OBJECT_TYPE; // Value of this const is set in Object.cc

    Schema(Name name = Name()):
        Object(OBJECT_TYPE,name),
        is_set(false),
        _td(TupleDescription()) {}

    Schema(Name name, TupleDescription td):
        Object(OBJECT_TYPE,name),
        is_set(true),
        _td(td) {}

    bool is_set;
    TupleDescription _td;

    NMSTL_SERIAL_SUBCLASS( Schema, Object, << is_set << _td );

    /// Fill in a Schema object from a CatalogSchema structure.
    ///
    void  from_catalog( CatalogSchema   *schema );

    operator const void* () const
    {   return (is_set) ? this : 0;
    }

    string as_string() const;

    string repr() const
    {   return as_string();
    }

    // Deprecated, but still in use by Query.cc
    string as_xml() const throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif                            // SCHEMA_H

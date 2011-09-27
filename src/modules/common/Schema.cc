#include "Schema.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Fill in a Schema object from a CatalogSchema structure.
//
void  Schema::from_catalog( CatalogSchema   *schema )
{
//..............................................................................


    _td = TupleDescription( schema );

    set_name( _td.get_name() );          // use schema->_name???
    is_set = true;

    return;
}


string Schema::as_string() const
{
    string out =  "{schema name=\""+get_name().as_string() + "\"";
    if ( is_set )
        out += _td.as_string();
    out += "\"/}";
    return out;
}


// Deprecated, but still in use by Query.cc
string Schema::as_xml() const throw (AuroraException)
{
    if ( is_set )
        return _td.as_xml();
    else return "<schema name=\""+get_name().as_string()+"\"/>";
}

BOREALIS_NAMESPACE_END

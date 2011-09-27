#include "CatalogQuery.h"
#include "CatalogTable.h"


BOREALIS_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////
// 
void CatalogTable::shallow_copy(const CatalogTable &table)
{
    BoxParameter bp;
//..............................................................................


    _name = table.get_table_name();
    _node = table.get_table_node();

    for (unsigned int i = 0; i < (table.get_table_key_const())->size(); i++)
    {
        _key.push_back((*(table.get_table_key_const()))[i]);
    }

    for (unsigned int i = 0; i < (table.get_table_index_const())->size(); i++)
    {
        _index.push_back((*(table.get_table_index_const()))[i]);
    }

    bp.set_parameter_map(table.get_table_parameter_map_const());
    _parameter = bp;
    _query = NULL;
    _schema = NULL;
}



////////////////////////////////////////////////////////////////////////////////
// 
// Get table parameters.
//
const BoxParameter::ParameterMap *CatalogTable::get_table_parameter_map_const()
      const
{
//..............................................................................


    return(_parameter.get_parameter_map_const());
}


////////////////////////////////////////////////////////////////////////////////
//
// Set table schema.
//
void CatalogTable::set_table_schema(CatalogSchema *schema)
{
//..............................................................................


    _schema = schema;

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Get table node.
//
string CatalogTable::get_table_node() const
{
//..............................................................................

    return(_node);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Create an XML string for a table.
//
string  CatalogTable::form_table_xml()
{
    string     element;

    BoxParameter::ParameterMap           *params;
    BoxParameter::ParameterMap::iterator  parameter;
                  vector<Name>::iterator  field;
//
//..............................................................................


    element += "<table name=\"" + Util::full_name(get_table_name().as_string())
               + "\" schema=\"" + Util::full_name(get_table_schema_name().as_string())
                                + "\">\n";

    // Fields in the table key.
    //
    for (field  = get_table_key()->begin();
         field != get_table_key()->end(); field++)
    {
        element += " <key   field=\"" + field->as_string() + "\"/>\n";
    }

    // Indexed fields in the table.
    //
    for (field  = get_table_index()->begin();
         field != get_table_index()->end(); field++)
    {
        element += " <index field=\"" + field->as_string() + "\"/>\n";
    }

    // Table parameters.
    //
    params = get_table_parameter_map();

    for (parameter  = params->begin();
         parameter != params->end(); parameter++)
    {
        // Escape the parameter value for the engine's parser.
        //
        element += " <parameter  name=\"" + parameter->first
                          + "\" value=\"" + xml_escape(parameter->second)
                          + "\"/>\n";
    }

    element += "</table>\n";
    DEBUG << element;

    return(element);
}



void CatalogTable::freeze(::NMSTL::OSerial &out) const
{
    bool shallow_freeze = false;

    out && out.begin_container(*this) && (out << shallow_freeze << _name)
    && freeze_ptr_shallow_impl(out, _query)
    && freeze_ptr_impl(out, _schema)
    && (out << _node << _key << _index << _parameter)
    && out.end_container(*this);
}

void CatalogTable::shallow_freeze(::NMSTL::OSerial &out) const
{
    bool shallow_freeze = true;

    out && out.begin_container(*this)
    && (out << shallow_freeze << _name)
    && out.end_container(*this);
}

void CatalogTable::unfreeze(::NMSTL::ISerial &in)
{
    bool shallow_freeze;

    in && in.begin_container(*this)
    && (::NMSTL::__iserial_adaptor(in) << shallow_freeze << _name);

    if (!shallow_freeze)
    {
    unfreeze_ptr_impl(in, _query)
        && unfreeze_ptr_impl(in, _schema)
        && (::NMSTL::__iserial_adaptor(in) << _node << _key << _index << _parameter);
    }

    in.end_container(*this);
}

BOREALIS_NAMESPACE_END

#include "TableDescription.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
void TableDescription::add_index(string field)
{
    TableIndexes::iterator index_it = _indexes.begin();
    TableIndexes::iterator indexes_end = _indexes.end();

    Boolean  found = False;
//
//..............................................................................


    while (!found  &&  index_it != indexes_end)
    {   if (*index_it == field)
        {   found = True;
        }
    }

    if (!found)
    {   _indexes.push_back(field);
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void TableDescription::add_param(string name, string value)
{
    TableParams::iterator param_it   = _params.begin();
    TableParams::iterator params_end = _params.end();

    Boolean  found = False;
//
//..............................................................................


    while (!found  &&  param_it != params_end)
    {   if (param_it->first == name)
        {
            param_it->second = value;
            found = True;
        }

        param_it++;
    }

    if (!found)
    {   _params.push_back(pair<string, string>(name, value));
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  TableDescription::from_catalog(CatalogTable   *table)
{
    string    value;

    vector<Name>::iterator  field;

    BoxParameter::ParameterMap           *parameter_map;
    BoxParameter::ParameterMap::iterator  parameter;
//
//..............................................................................


    _name = table->get_table_name();
    _schema_name = table->get_table_schema_name();

    // Fields in the table key.
    //
    for (field  = table->get_table_key()->begin();
         field != table->get_table_key()->end(); ++field)
    {
        if (!value.empty())
        {   value = value + ",";
        }

        value = value + (*field);
    }

    add_param("key", value);


    // Indexed fields in the table.
    //
    for (field  = table->get_table_index()->begin();
         field != table->get_table_index()->end(); ++field)
    {
        add_index(to_string(*field));
    }


    // Table parameters.
    //
    parameter_map = table->get_table_parameter_map();

    for (parameter  = parameter_map->begin();
         parameter != parameter_map->end(); ++parameter)
    {
        add_param((*parameter).first,
                  (*parameter).second);
    }

    return;
}


BOREALIS_NAMESPACE_END

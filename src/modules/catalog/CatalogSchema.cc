#include "CatalogSchema.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Return True if two schemas have the same field names and types.
//
Boolean  CatalogSchema::equal_schema( // Schema to compare.
                                     CatalogSchema  *match
                                   )
{
    Boolean   status = False;
    string    name;
    uint16    index;
//
// This needs to be extended to match arrays in schemas???
//..............................................................................


    index = get_schema_field().size();
    name  = get_schema_name();

    if (name.empty())
    {   name = "unnamed composite";
    }

    DEBUG << "Compare " << to_string(index) << " fields in ("
          << name << ") to (" << match->get_schema_name() << ")";

    if ( index == match->get_schema_field().size() )
    {   status = True;

        while ( index )
        {   index--;

            if (( get_schema_field(index)->get_type()
                      != match->get_schema_field(index)->get_type() )  ||
                ( get_field_name(index) != match->get_field_name(index)))
            {
                status = False;

                DEBUG << "Mismatched field:  (" << get_field_name(index) << "  "
                      << get_schema_field(index)->get_type() << ")  ("
                      << match->get_field_name(index) << "  "
                      << match->get_schema_field(index)->get_type() << ")";

                break;
            }
        }
    }
    
    if ( !status )
    {
        DEBUG << "Match failed!";
        DEBUG << "Current: " << as_string();
        DEBUG << "Match:   " << match->as_string();
    }

    return(status);
}



////////////////////////////////////////////////////////////////////////////////
//
//  Create an XML string for a schema in the catalog.
//
string  CatalogSchema::form_schema_xml()
{
    string     element;
    DataType   type;

    vector<SchemaField>  *field;
    SchemaArray          *array;

    vector<SchemaField>::iterator  f;
//
//..............................................................................


    element = string() + "<schema  name=\""
                       + Util::full_name(get_schema_name()) + "\">\n";

    field   = &get_schema_field();

    for (f = field->begin(); f != field->end(); f++)
    {
        element += " <field  name=\"" + f->get_name() + "\"  type=\"";
        type     = f->get_type();

        if (type == DataType::ARRAY)
        {   
            array    = f->get_array();
            element += array->array_type();

            if (array->get_element_type() == DataType::STRING)
            {   element += "\"  size=\"" + to_string(array->get_element_size());
            }
        }
        else
        {   element += type;

            if (type == DataType::STRING)
            {   element += "\"  size=\"" + to_string(f->get_size());
            }
        }

        element += "\"/>\n";
    }

    element += "</schema>\n";
    DEBUG << element;

    return(element);
}


BOREALIS_NAMESPACE_END

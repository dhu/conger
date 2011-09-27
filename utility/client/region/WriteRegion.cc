#include <fstream>

#include "WriteRegion.h"

#define BOREALIS_DTD  "http://www.cs.brown.edu/research/borealis/borealis.dtd"

BOREALIS_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////
//
//  Dump a regional catalog to a file as Xml.
//
void  WriteRegion::write_region(RegionalCatalog  &catalog,
                                         string   file)
{
//..............................................................................


    ofstream  out( file.c_str() );

    if (out.is_open())
    {   out << "<?xml version=\"1.0\"?>\n";

        // I am getting rid of these comment lines to make visualizer's life
        // easier. -- tatbul
        //out << "<!DOCTYPE borealis SYSTEM \"" << BOREALIS_DTD << "\">\n\n";

        out << "    <!-- Generated Borealis query diagram.  File:  "
                                                         << file << " -->\n\n";

        out << "<borealis>\n";

        out << form_schema(catalog);
        out << form_stream(catalog);
        out << form_query(catalog);
        out << form_view(catalog);

        out << "</borealis>\n";
        out.close();
    }
    else
    {   NOTICE << "Can't open the output file:  " << file;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Convert a regional catalog to a string as Xml.
//
string  WriteRegion::form_region(RegionalCatalog  &catalog)
{
    string    xml;
//..............................................................................


    xml  = "<?xml version=\"1.0\"?>\n";
    xml += "<!DOCTYPE borealis SYSTEM \"" BOREALIS_DTD "\">\n\n";
    xml += "<borealis>\n";

    xml += form_schema(catalog);
    xml += form_stream(catalog);
    xml += form_query(catalog);
    xml += form_view(catalog);

    xml += "</borealis>\n";

    return  xml;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Format schemas in a regional catalog as an Xml string.
//
string  WriteRegion::form_schema(RegionalCatalog  &catalog)
{
    string   xml = "";

    CatalogSchema::SchemaMap           *map;
    CatalogSchema::SchemaMap::iterator  schema;
//
//..............................................................................


    map = catalog.get_schema_map();

    for ( schema = map->begin(); schema != map->end(); ++schema )
    {   xml += (*schema).second.form_schema_xml() + "\n";
    }

    return  xml;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Format streams in a regional catalog as an Xml string.
//
string  WriteRegion::form_stream(RegionalCatalog  &catalog)
{
    string   xml = "";

    Catalog::StreamMap           *map;
    Catalog::StreamMap::iterator  stream;
//
//..............................................................................


    map = catalog.get_stream_map();

    for (stream = map->begin(); stream != map->end(); stream++)
    {   if ((*stream).second.is_input_stream())
        {   xml += (*stream).second.form_input_xml();
        }
        else if ((*stream).second.is_output_stream())
        {   xml += (*stream).second.form_output_xml();
        }
    }

    xml += form_subscribe(catalog);

    return  xml;    
}



////////////////////////////////////////////////////////////////////////////////
//
//  Format subscriptions to outputs in a regional catalog as an Xml string.
//
string  WriteRegion::form_subscribe(RegionalCatalog  &catalog)
{
    string   xml = "";

    Catalog::StreamLinkMap             *subcriber;
    Catalog::StreamLinkMap::iterator    subcribe;

    Catalog::SubscriptionMap           *output_map;
    Catalog::SubscriptionMap::iterator  output;
//
//     <subscribe  stream="<stream name>"  [gap="<gap size>"] 
//              [endpoint="[<mip>:]mport"] />
//..............................................................................


    subcriber = catalog.get_subscriber_map();

    for (subcribe  = subcriber->begin();
         subcribe != subcriber->end(); subcribe++)
    {
        output_map = &(*subcribe).second;

        for (output  = output_map->begin();
             output != output_map->end(); output++)
        {
            xml += (*output).second.form_subscribe_xml();
        }
    }

    return  xml;    
}



////////////////////////////////////////////////////////////////////////////////
//
//  Get all queries in a regional catalog as an Xml string.
//
string  WriteRegion::form_query(RegionalCatalog  &catalog)
{
    string   xml = "";

    Catalog::QueryMap             *map;
    Catalog::QueryMap::iterator    query;

    CatalogBox::BoxStar           *query_box;
    CatalogBox::BoxStar::iterator  box;

    CatalogTable::TableStar           *query_table;
    CatalogTable::TableStar::iterator  table;
//
//..............................................................................


    map = catalog.get_query_map();

    for (query = map->begin(); query != map->end(); ++query)
    {   xml += "\n<query  name=\"" + to_string((*query).second.get_query_name())
                                   + "\" >\n";

        query_box   = (*query).second.get_query_box();
        query_table = (*query).second.get_query_table();

        //if ((query_box.size() == 1)  &&  ((query_table.size() == 0)

        for ( box = query_box->begin(); box !=query_box->end(); box++)
        {   xml += (*box)->form_box_xml();
        }


        for (table = query_table->begin(); table !=query_table->end(); table++)
        {   xml += (*table)->form_table_xml();
        }

        xml += "</query>\n";
    }

    return  xml;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Format connection point views in a regional catalog as an Xml string.
//
string  WriteRegion::form_view(RegionalCatalog  &catalog)
{
    string   xml = "";

    Catalog::ViewMap           *map;
    Catalog::ViewMap::iterator  view;
//
//..............................................................................


    map = catalog.get_view_map();

    for (view = map->begin(); view != map->end(); view++)
    {   xml += (*view).second.form_view_xml();
    }

    return  xml;
}

BOREALIS_NAMESPACE_END

//
////////////////////////////////////////////////////////////////////////////////

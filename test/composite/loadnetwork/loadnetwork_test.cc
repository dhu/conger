
#include "MedusaClient.h"
#include "args.h"

#include <NMSTL/xmlrpc>
#include <NMSTL/tcprpc>

#include <xercesDomUtil.h>
#include <XmlTempString.h>

using namespace Borealis;

int main(int argc, const char* argv[])
{
    // Our Borealis node.
    string node1_ip = "127.0.0.1";
    int node1_port = 15000;

    stdArgs(argc, argv, node1_ip, node1_port);

    string xml_file_path = "";
    file_args(argc, argv, xml_file_path);

    if (xml_file_path == "")
    FATAL << "No xml query network specified!";

    // Client for Borealis node
    ptr<MedusaClient> client(new MedusaClient(InetAddress(node1_ip,node1_port)));

    ptr<DOMDocument> doc;
    try
    {
    doc = parse_xml_file(xml_file_path);
    } catch (AuroraException& e)
    {
    FATAL << "Error parsing file " << xml_file_path << " : " << e.what();
    }
    DOMElement *root = doc->getDocumentElement();

    XmlTempString xschemas("schema");
    DOMNodeList *schema_nodes = root->getElementsByTagName(xschemas.to_xml());

    map<string, Schema> schemas;

    for( unsigned int i = 0; i < schema_nodes->getLength(); ++i )
    {
    string schema_xml = xml_serialize(static_cast<const DOMElement*>(schema_nodes->item(i)));

    DEBUG << "Setting up schema: " << schema_xml;

    Schema schema;
    Status s = schema.from_xml(schema_xml);
    if ( !s ) FATAL << "Failed parsing schema from xml" << s;

    RPC<void> r = client->create_schema(schema);
    NOTICE << "create_schema: " << r;
    if (!r.stat()) return 1;

    schemas[to_string(schema.get_name())] = schema;
    }

    XmlTempString xstreams("stream");
    DOMNodeList *stream_nodes = root->getElementsByTagName(xstreams.to_xml());

    for( unsigned int i = 0; i < stream_nodes->getLength(); ++i)
    {
    const DOMElement* stream_el = static_cast<const DOMElement*>(stream_nodes->item(i));
    string input_stream_name = xml_attribute(stream_el, "name", ATTR_NON_EMPTY);
    string input_stream_type = xml_attribute(stream_el, "type", ATTR_NON_EMPTY);

    DEBUG << "Setting up stream: " << input_stream_name;

    map<string, Schema>::iterator schemas_it = schemas.find(input_stream_type);
    if (schemas_it != schemas.end())
    {
        StreamDef input_stream_def(input_stream_name, schemas_it->second);
        RPC<void> r = client->create_stream(input_stream_def);
        if ( !r.stat() ) FATAL << "Unable to create stream " << input_stream_name << " : " << r;
    } else
    {
        FATAL << "No type defined for input stream " << input_stream_name;
    }
    }

    XmlTempString xqueries("query");
    DOMNodeList *query_nodes = root->getElementsByTagName(xqueries.to_xml());

    for( unsigned int i = 0; i < query_nodes->getLength(); ++i)
    {
    const DOMElement* query_el = static_cast<const DOMElement*>(query_nodes->item(i));
    string query_name = xml_attribute(query_el, "name", ATTR_NON_EMPTY);
    string query_xml = xml_serialize(query_el);

    DEBUG << "Setting up query: " << query_xml;

    RPC<void> r = client->create_query_xml(query_xml);
    if (!r.stat()) FATAL << "Unable to create query: " << r;

    r = client->start_query(query_name);
    if (!r.stat()) FATAL << "Unable to start query: " << r;
    }

    return 0;
}

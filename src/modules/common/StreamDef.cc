
#include "StreamDef.h"
#include <NMSTL/text>

MEDUSA_NAMESPACE_BEGIN

// --------------------------------------------------
string
StreamDef::as_string() const
{
    return string("StreamDef{id=")+_id+"; schema="
        +_schema+"; partition="+_partition + "}";
}

// --------------------------------------------------
string
StreamDef::repr() const
{
    return
        "streamdef " + to_string(_id) + " {\n" +
        indent(_schema.repr(), 4) +
        "    partition=" + _partition + ";\n" +
        "}\n";
}

// --------------------------------------------------
string
StreamDef::as_xml(bool with_schema) const
{

    string out;
    try
    {
        // Schema first
        if (with_schema)
            out << _schema.as_xml();

        // Stream second
        string schema_name = to_string(_schema.get_name());
        out << "\n<stream name=\"" << get_name() << "\" schema=\"" << schema_name << "\"/>\n";

    } catch (AuroraException& e)
    {
        ERROR << "Failed to transform stream " << get_name()
              << " into xml because: " << e.what();
    }
    return out;
}

// --------------------------------------------------
string
StreamDef::as_aurora_xml(bool with_schema) const
{
    string out = string("<modify><add>\n");
    out += as_xml(with_schema);
    out += "</add></modify>\n";
    return out;
}

MEDUSA_NAMESPACE_END

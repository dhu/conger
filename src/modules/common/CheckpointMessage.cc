#include "CheckpointMessage.h"

BOREALIS_NAMESPACE_BEGIN;

CheckpointMessage::CheckpointMessage():
_primary_server(""),
_box_package(new BoxPackage())
{
}

CheckpointMessage::CheckpointMessage(string primary_server, ptr<HAUnit> ha_unit):
_primary_server(primary_server),
_ha_unit(ha_unit),
_box_package(new BoxPackage())
{
}

string CheckpointMessage::as_string() const
{
    string s;
    return s;
}

string CheckpointMessage::get_primary_server()
{
    return _primary_server;
}

ptr<HAUnit> CheckpointMessage::get_ha_unit()
{
    return _ha_unit;
}

void CheckpointMessage::add(string xml_string)
{
    _xml_strings.push_back(xml_string);
}

vector<string> CheckpointMessage::get_xml_strings()
{
    return _xml_strings;
}

ptr<BoxPackage> CheckpointMessage::get_box_package()
{
    return _box_package;
}

BOREALIS_NAMESPACE_END;

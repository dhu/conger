#ifndef MEDUSA_NODESTATUS_H
#define MEDUSA_NODESTATUS_H

#include <sstream>
#include "common.h"
#include "Name.h"
#include "HAMethod.h"

MEDUSA_NAMESPACE_BEGIN

class NodeStatus
{

 public:
    Name _name;
    bool _is_primary;
    int _ha_method;
    string _partner;
    vector<string> _query_xmls;
    string _subscription_xml;

    NodeStatus(Name name = Name("..."))
        : _name(name), _is_primary(false), _ha_method(NO_HA), _partner(""), _subscription_xml("") {}

    string as_string() const
    {
        string out("{Node Status:"+ to_string(_name) + ", is_primary = " + _is_primary +
            ", HA method = " + _ha_method + ", partner = " + _partner + ", queries = " + _query_xmls + ", subscription = " + _subscription_xml +"}");
        return out;
    }

    string repr() const
    {
        stringstream out;
        out.precision(3);
        out << "{Node Status: " << _name << ", is_primary = " << _is_primary <<
            ", HA method = " << _ha_method << ", partner = " << _partner << ", queries = " << _query_xmls << ", subscription = " << _subscription_xml << "}";
        return out.str();
    }
    NMSTL_SIMPLY_SERIALIZABLE(NodeStatus, << _name << _is_primary << _ha_method << _partner << _query_xmls << _subscription_xml);

};

NMSTL_TO_STRING(NodeStatus);

MEDUSA_NAMESPACE_END

#endif

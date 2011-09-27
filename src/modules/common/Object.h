
#ifndef MEDUSA_OBJECT_H
#define MEDUSA_OBJECT_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <iterator>
#include <algorithm>

#include <NMSTL/debug>
#include <NMSTL/ptr>

#include "common.h"
#include "Name.h"

MEDUSA_NAMESPACE_BEGIN;

class Object
{
    string type;
    Name name;

    string localhost;

  public:
    Object(const Object& other);
    Object& operator = (const Object& other);

    Object(string type = string(), Name name = Name());
    virtual ~Object();

    Name get_name() const { return name; }
    void set_name(Name name) { this->name = name; }
    string get_type() const { return type; }

    string as_string() const;

    void set_localhost(string localhost) { this->localhost = localhost; }
    string get_localhost() { return localhost; }

    virtual string repr() const;

    //Name anon(string tag) const;

    NMSTL_SERIAL_BASE(Object, long, << name);
};
NMSTL_TO_STRING(Object);

MEDUSA_NAMESPACE_END;

#endif


#include "Object.h"
#include "Objects.h"

MEDUSA_NAMESPACE_BEGIN;

NMSTL_SERIAL_DEFINE(AbstractQueueState,    6);
NMSTL_SERIAL_DEFINE(AbstractBoxState,      7);

const string StreamDef::OBJECT_TYPE = "streamdef";
const string Schema::OBJECT_TYPE = "schema";
const string Query::OBJECT_TYPE = "query";
const string Subscription::OBJECT_TYPE = "sub";
const string CPViewDescription::OBJECT_TYPE = "cpviewdescription";

NMSTL_SERIAL_DEFINE(Object,                1);
NMSTL_SERIAL_DEFINE(StreamDef,             2);
NMSTL_SERIAL_DEFINE(Schema,                3);
NMSTL_SERIAL_DEFINE(Query,                 4);
NMSTL_SERIAL_DEFINE(Subscription,          5);
NMSTL_SERIAL_DEFINE(CPViewDescription,     6);

Object::Object(const Object& other) : type(other.type), name(other.name), localhost(other.localhost) {}

Object& Object::operator = (const Object& other)
{
    type = other.type;
    name = other.name;
    localhost = other.localhost;
    return *this;
}

Object::Object(string type, Name name) : type(type), name(name)
{
    DEBUG(5) << "Creating " << type << " " << name;
}

Object::~Object()
{
    if (type != "schema")
        DEBUG(5) << "Deleting " << type << " " << name;
}

string Object::as_string() const
{
    string out = type + " " + to_string(name);
    if (localhost.length())
        out += " (on " + localhost + ")";
    return out;
}

string Object::repr() const
{
    return as_string();
}

//Name Object::anon(string tag) const
//{
//  return Name::anon(get_name().get_host(), tag);
//}

MEDUSA_NAMESPACE_END;

#ifndef HA_TYPE_H
#define HA_TYPE_H

#include  "common.h"
#include  "Exceptions.h"

BOREALIS_NAMESPACE_BEGIN

/**
* Typesafe enumeration listing the different types of nodes
* we might want to monitor
*/
class HAType
{
    public:
    static const HAType PRIMARY;
    static const HAType BACKUP;

    bool operator==(const HAType& t) const
    {
        return (t._type == _type);
    }

    bool operator!=(const HAType& t) const
    {
        return !(t._type == _type);
    }

    string as_string() const { return _type; }
    // Ideally the constructor should be private to
    // ensure that no one creates new instances of PrimaryType
    HAType(string type) : _type(type) {}

    private:
    string _type;
};

BOREALIS_NAMESPACE_END
#endif                        // HA_TYPE_H

#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include  "common.h"
#include  "Exceptions.h"

BOREALIS_NAMESPACE_BEGIN

/**
* Typesafe enumeration listing the different types of nodes
* we might want to monitor
*/
class NodeType
{
    public:
    static const NodeType PRIMARY;
    static const NodeType REPLICA;
    static const NodeType PROVIDER;
    static const NodeType NONE;

    bool operator==(const NodeType& t) const
    {
        return (t._type == _type);
    }

    string as_string() const { return _type; }
    // Ideally the constructor should be private to
    // ensure that no one creates new instances of PrimaryType
    NodeType(string type) : _type(type) {}

    private:
    string _type;
};

BOREALIS_NAMESPACE_END
#endif                        // RECOVERY_H

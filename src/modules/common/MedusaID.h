#ifndef MEDUSA_MEDUSAID_H
#define MEDUSA_MEDUSAID_H

#include "common.h"

MEDUSA_NAMESPACE_BEGIN

// --------------------------------------------------
/// In Medusa, all objects are identified with MedusaIDs
class MedusaID
{

 public:
    MedusaID(string id = string());

    string get_id() const { return id; }

    bool operator==(const MedusaID& m) const
    {
        return (id == m.id);
    }

    bool operator!=(const MedusaID& m) const
    {
        return (id != m.id);
    }

    bool operator<(const MedusaID& m) const
    {
        return (id < m.id);
    }

    operator const void *() const
    {
        return id.size() == 0 ? 0 : this;
    }

    string as_string() const;
    string repr() const { return as_string(); }
    NMSTL_SIMPLY_SERIALIZABLE(MedusaID, << id);
    NMSTL_SERIAL_NO_TAG(MedusaID);

 private:
    string id;

};


NMSTL_TO_STRING(MedusaID);

MEDUSA_NAMESPACE_END


#endif

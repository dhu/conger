#ifndef MEDUSA_PARTITION_H
#define MEDUSA_PARTITION_H

#include "common.h"
#include "MedusaID.h"

MEDUSA_NAMESPACE_BEGIN

class Partition
{

 public:
    Partition(MedusaID ow = MedusaID()) : owner(ow) {}
    ~Partition() {}

    const MedusaID get_owner() const { return owner; }
    string repr() const { return as_string(); }
    string as_string() const { return owner.as_string(); }
    operator const void* () const
    {
        return (owner) ? this : 0;
    }
    NMSTL_SIMPLY_SERIALIZABLE(Partition, << owner);

 protected:
    MedusaID owner;
};

NMSTL_TO_STRING(Partition);

MEDUSA_NAMESPACE_END


#endif

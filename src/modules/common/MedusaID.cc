#include "MedusaID.h"

MEDUSA_NAMESPACE_BEGIN

// --------------------------------------------------
MedusaID::MedusaID(string new_id) :id(new_id)
{
}

// --------------------------------------------------
string
MedusaID::as_string() const
{
    return id;
}


MEDUSA_NAMESPACE_END

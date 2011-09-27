#include "Tuple.h"

MEDUSA_NAMESPACE_BEGIN

string Tuple::as_string() const
{
    return string("")+get_timestamp();
}

MEDUSA_NAMESPACE_END

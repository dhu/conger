#include "CatalogQuery.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Remove the given box from this query.
//
void CatalogQuery::remove_query_box(Name box_name)
{
    CatalogBox::BoxStar::iterator i;
//..............................................................................


    for (i = _box.begin(); i != _box.end(); i++)
    {
        if ((*i)->get_box_name() == box_name)
        {
            break;
        }
    }

    if (i != _box.end())
    {
        _box.erase(i);
    }
    else
    {   DEBUG << "No such box in this query!";
    }

    return;
}


BOREALIS_NAMESPACE_END

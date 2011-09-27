#include "QBox.h"

#include "ScatterQBox.h"
#include "GatherQBox.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Create a new instance of a user defined box.
//
QBox  *QBox::instantiate_array(// Type name of the box.
                               const string  &type)
{
//..............................................................................


    if      (type == "scatter")             return  new ScatterQBox;
    else if (type == "gather")              return  new GatherQBox;

    return  NULL;
}


BOREALIS_NAMESPACE_END

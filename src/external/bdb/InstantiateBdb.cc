#include "DeleteQBox.h"
#include "ExistsQBox.h"
#include "InsertQBox.h"
#include "SelectQBox.h"
#include "UpdateQBox.h"

BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
// Create a new instance of a user defined box.
//
QBox  *QBox::instantiate_bdb(// Type name of the box.
                             const string  &type)
{
//..............................................................................


    if (type == "delete")       return(new DeleteQBox);
    else if (type == "exists")  return(new ExistsQBox);
    else if (type == "insert")  return(new InsertQBox);
    else if (type == "select")  return(new SelectQBox);
    else if (type == "update")  return(new UpdateQBox);

    return  NULL;
}


BOREALIS_NAMESPACE_END 

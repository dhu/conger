#include "QBox.h"
#include "NArgs.h"
#include "Aggregate.h"


BOREALIS_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
//
//  Stub to exclude user defined boxes.
//
QBox  *QBox::instantiate_udb(const string  &type)
{
//..............................................................................


    return NULL;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Stub to exclude user defined math functions.
//
Function  *Function::instantiate_udb(const string  &name)
{
//..............................................................................


    return NULL;
}




////////////////////////////////////////////////////////////////////////////////
//
//  Stub to exclude user defined aggregate functions.
//
Aggregate  *Aggregate::instantiate_udb(const string  &name)
{
//..............................................................................


    return NULL;
}


BOREALIS_NAMESPACE_END 

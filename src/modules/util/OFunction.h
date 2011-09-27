#ifndef OFUNCTION_H
#define OFUNCTION_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include "common.h"

//using namespace std;
BOREALIS_NAMESPACE_BEGIN

class OFunction
{

public:
    virtual ~OFunction() {}

    virtual char* evaluate( char *tuple ) = 0;
    virtual char* evaluate( char *tuple1, char *tuple2 ) = 0;
    virtual int32 get_returned_size() = 0;

    virtual vector<int32>  get_return_sizes()
    {  // default, say for map boxes, is to return just the one output size
       return vector<int32>( 1, get_returned_size() );
    }
};

BOREALIS_NAMESPACE_END
#endif                 // OFUNCTION_H

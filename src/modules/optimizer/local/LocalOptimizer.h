
#ifndef LOCAL_OPTIMIZER_H
#define LOCAL_OPTIMIZER_H

#include "common.h"
#include "BasicComponent.h"

BOREALIS_NAMESPACE_BEGIN

class LocalOptimizer : public BasicComponent
{

  public:

    LocalOptimizer(string id) : BasicComponent(id, "LocalOptimizer") {}

    ~LocalOptimizer() {}

};

BOREALIS_NAMESPACE_END

#endif

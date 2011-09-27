#include <PtCondition.h>

BOREALIS_NAMESPACE_BEGIN;

PtCondition::PtCondition()
  throw (std::exception)
{
  int rc = pthread_cond_init(& _cond, NULL);
  if (rc != 0)
    {
        Throw(AuroraException, "Failed call to pthread_cond_init(...)");
    }
}

//===============================================================================

PtCondition::~PtCondition()
  throw (std::exception)
{
  int rc = pthread_cond_destroy(& _cond);
  if (rc != 0)
    {
        Throw(AuroraException, "Failed call to pthread_cond_destroy(...)");
    }
}

//===============================================================================

void PtCondition::signal()
  throw (std::exception)
{
  int rc = pthread_cond_signal(& _cond);
  if (rc != 0)
    {
        Throw(AuroraException, "Failed call to pthread_cond_signal(...)");
    }
}

//===============================================================================

void PtCondition::broadcast()
  throw (std::exception)
{
  int rc = pthread_cond_broadcast(& _cond);
  if (rc != 0)
    {
        Throw(AuroraException, "Failed call to pthread_cond_broadcast(...)");
    }
}

//===============================================================================

BOREALIS_NAMESPACE_END;

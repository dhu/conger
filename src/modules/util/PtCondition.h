#ifndef PtCondition_H
#define PtCondition_H

#include "Exceptions.h"
#include <exception>

#include <pthread.h>

BOREALIS_NAMESPACE_BEGIN;

class PtMutex;

class PtCondition
{
public:
  PtCondition()
      throw (::std::exception);

  virtual ~PtCondition()
      throw (::std::exception);

  void signal()
      throw (::std::exception);

  void broadcast()
      throw (::std::exception);
private:
  friend class PtMutex;

  pthread_cond_t _cond;
};

BOREALIS_NAMESPACE_END;

#endif

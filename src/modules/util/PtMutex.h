#ifndef PtMutex_H
#define PtMutex_H

#include "ILockable.h"
#include "Exceptions.h"
#include "PtCondition.h"

#include <pthread.h>

BOREALIS_NAMESPACE_BEGIN;

class PtMutex : public ILockable
{
public:
  PtMutex()
      throw (::std::exception);

  virtual ~PtMutex()
      throw (::std::exception);

  void lock()
      throw (::std::exception);

  void unlock()
      throw (::std::exception);

  bool try_lock()
      throw (::std::exception);

  void wait_cond(PtCondition & cond)
      throw (::std::exception);

#ifdef SM_VERIFY_LOCKS
  // Returns true iff mutex is held. If held, will set tid to the thread ID of
  // the holder.
  bool get_holder(pthread_t & tid)
    throw (exception);

  // If the caller doesn't hold this mutex, this will do something violent.
  // 'doAbort == false' --> throw an exception
  // 'doAbort == true'  --> call abort()
  void ensure_held_by_caller(bool do_abort) const
    throw (exception);
#endif

private:
  pthread_mutex_t _mtx;

#ifdef SM_VERIFY_LOCKS
  pthread_mutex_t _guard;
  pthread_t _holder;
  bool _held;
#endif
};

BOREALIS_NAMESPACE_END;

#endif


#ifdef SM_VERIFY_LOCKS
#define VERIFY_CALLER_HOLDS_LOCK(M) M.ensureHeldByCaller(true);
#else
#define VERIFY_CALLER_HOLDS_LOCK(M)
#endif

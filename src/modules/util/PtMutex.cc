#include <PtMutex.h>
#include <sstream>

#include <iostream>

BOREALIS_NAMESPACE_BEGIN;

PtMutex::PtMutex()
  throw (std::exception)
{
  int rc = pthread_mutex_init(& _mtx, NULL);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_init(...)");
    }

#ifdef SM_VERIFY_LOCKS
  rc = pthread_mutex_init(& _guard, NULL);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_init(...)");
    }
#endif
}

//===============================================================================

PtMutex::~PtMutex()
  throw (std::exception)
{
  int rc = pthread_mutex_destroy(& _mtx);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_destroy(...)");
    }

#ifdef SM_VERIFY_LOCKS
  rc = pthread_mutex_destroy(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_destroy(...)");
    }
#endif
}

//===============================================================================

void PtMutex::lock()
  throw (std::exception)
{
  int rc = pthread_mutex_lock(& _mtx);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_lock(...)");
    }

#ifdef SM_VERIFY_LOCKS
  rc = pthread_mutex_lock(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_lock(...)");
    }

  _holder = pthread_self();
  _held = true;

  rc = pthread_mutex_unlock(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_unlock(...)");
    }
#endif
}

//===============================================================================

void PtMutex::unlock()
  throw (std::exception)
{
  int rc;

#ifdef SM_VERIFY_LOCKS
  rc = pthread_mutex_lock(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_lock(...)");
    }

  _held = false;

  rc = pthread_mutex_unlock(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_unlock(...)");
    }
#endif

  rc = pthread_mutex_unlock(& _mtx);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_unlock(...)");
    }
}


//===============================================================================

bool PtMutex::try_lock()
    throw (std::exception)
{
  int rc = pthread_mutex_trylock(& _mtx);
  if (rc != 0) { return false; }

  if ( errno == EBUSY )
      return false;

#ifdef SM_VERIFY_LOCKS
  rc = pthread_mutex_lock(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_lock(...)");
    }

  _holder = pthread_self();
  _held = true;

  rc = pthread_mutex_unlock(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_unlock(...)");
    }
#endif

  return true;
}

//===============================================================================

void PtMutex::wait_cond(PtCondition & cond)
  throw (std::exception)
{
  int rc = pthread_cond_wait(& cond._cond, & _mtx);
  if (rc  != 0)
    {
      Throw(AuroraException, "Failed call to pthread_cond_wait(...)");
    }
}



#ifdef SM_VERIFY_LOCKS

//===============================================================================

bool PtMutex::get_holder(pthread_t & tid)
  throw (exception)
{
  int rc = pthread_mutex_lock(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_lock(...)");
    }

  bool return_val = _held;

  if (_held)
    {
      tid = _holder;
    }

  rc = pthread_mutex_unlock(& _guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_unlock(...)");
    }

  return return_val;
}

//===============================================================================

void PtMutex::ensure_held_by_caller(bool do_abort) const
  throw (exception)
{
  //  cout << "Entering PtMutex::ensureHeldByCaller" << endl;
  bool held_by_caller;

  // Fool the compiler into letting this method me const...
  const pthread_mutex_t * alias = & _guard;
  pthread_mutex_t * p_non_const_guard = const_cast<pthread_mutex_t *>(alias);

  int rc = pthread_mutex_lock(p_non_const_guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_lock(...)");
    }

  held_by_caller = ((_held) && (_holder == pthread_self()));
  //  cout << "_held =  " << _held << endl
  //       << "_holder = " << _holder << endl
  //       << "caller = " << pthread_self() << endl
  //       << "heldByCaller = " << heldByCaller << endl << endl;

  rc = pthread_mutex_unlock(p_non_const_guard);
  if (rc != 0)
    {
      Throw(AuroraException, "Failed call to pthread_mutex_unlock(...)");
    }

  if (! held_by_caller)
    {
      if (do_abort)
    {
      abort();
    }
      else
    {
      ostringstream os;
      os << "Lock not held by caller thread: " << pthread_self();
      Throw(AuroraException, os.str());
    }
    }
  //  cout << "Exiting PtMutex::ensureHeldByCaller" << endl;
}

#endif

BOREALIS_NAMESPACE_END;

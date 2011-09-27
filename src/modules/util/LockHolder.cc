#include <LockHolder.h>
#include <Exceptions.h>
#include <assert.h>

BOREALIS_NAMESPACE_BEGIN;

//================================================================================

LockHolder::LockHolder(ILockable & lock_to_hold)
  throw (std::exception)
  : _lock(lock_to_hold),
    _presently_held(true)
{
  _lock.lock();
}

//================================================================================

LockHolder::LockHolder(ILockable & lock_to_hold, bool try_lock)
    throw (std::exception)
    : _lock(lock_to_hold)
{
    if (try_lock)
    _presently_held = _lock.try_lock();
    else
    {
    _presently_held = true;
    _lock.lock();
    }
}

//================================================================================

LockHolder::~LockHolder()
{
  try
    {
      if (_presently_held)
    {
      _lock.unlock();
    }
    }
  catch (std::exception & e)
    {
      // Scott Meyers makes a good argument why we should never throw an
      // exception from a destructor, unfortunately.
      assert(false);
    }
}

//================================================================================

void LockHolder::release()
  throw (exception)
{
  if (_presently_held)
    {
      _presently_held = false;
      _lock.unlock();
    }
  else
    {
        Throw(AuroraException, "Lock not presently held by this LockHolder");
    }
}

//================================================================================

void LockHolder::reacquire()
  throw (exception)
{
  if (! _presently_held)
    {
      _presently_held = true;
      _lock.lock();
    }
  else
    {
        Throw(AuroraException, "Lock already held by this LockHolder");
    }
}

//================================================================================

bool LockHolder::is_held()
{
    return _presently_held;
}

//================================================================================

BOREALIS_NAMESPACE_END;

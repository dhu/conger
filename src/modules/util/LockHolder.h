#ifndef LOCKHOLDER_H
#define LOCKHOLDER_H

#include "ILockable.h"

BOREALIS_NAMESPACE_BEGIN;

// For the entire time this object is instantiated, it holds a lock
// on the specified lock.
class LockHolder
{
public:
  LockHolder(ILockable & lock_to_hold)
    throw (exception);

  LockHolder(ILockable & lock_to_hold, bool try_lock)
    throw (exception);

  virtual ~LockHolder();

  // Convenience methods to let owner of this LockHolder temporarily give up the
  // lock. In some cases, this can result in much cleaner code.
  void release()
    throw (exception);

  void reacquire()
    throw (exception);

  bool is_held();
private:
  ILockable & _lock;
  bool _presently_held;
};

BOREALIS_NAMESPACE_END;

#endif

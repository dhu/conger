#ifndef LOCK_SET_H
#define LOCK_SET_H

#include <ext/hash_map>

#include "common.h"

BOREALIS_NAMESPACE_BEGIN;

class LockSet
{
  public:
    ILockable &get_lock() { return _lock; }

    bool take_lock(int key)
    {
        VERIFY_CALLER_HOLDS_LOCK(_lock);
        if (_locked[key])
            return false;
        else
        {
            _locked[key] = true;
            return true;
        }
    }

    void release_lock(int key)
    {
        VERIFY_CALLER_HOLDS_LOCK(_lock);
        if (!_locked[key])
            ERROR << "Extra release on key " << key;
        _locked[key] = false;
    }

  private:
    // Any thread that's accessing _locked must hold this lock.
    PtMutex _lock;
    __gnu_cxx::hash_map<int, bool> _locked;
};

BOREALIS_NAMESPACE_END;

#endif

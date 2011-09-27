#ifndef ILOCKABLE_H
#define ILOCKABLE_H

#include <exception>

#include "common.h"

BOREALIS_NAMESPACE_BEGIN

/**
 This is an interface that defines something as lockable. It's meant to let a
 variety of classes be served by the LockHolder class.
 */
class ILockable
{
  public:
    virtual ~ILockable() {};

    virtual void lock()
        throw (::std::exception) = 0;

    virtual void unlock()
        throw (::std::exception) = 0;

    virtual bool try_lock()
        throw (::std::exception) = 0;
};

BOREALIS_NAMESPACE_END

#endif                             // ILOCKABLE_H

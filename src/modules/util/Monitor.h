#ifndef MONITOR_H
#define MONITOR_H

#include <PtMutex.h>
#include <LockHolder.h>
#include <PtCondition.h>

/*
  Bundles together everything you need for a monitor variable.

  Note on setting values:
  There are two different, valid ways to change the Monitor's value. They are:

  (A) Call setValue(...), or

  (B) Have code like this:
      Monitor m(...);
      ...
      {
         LockHolder lh(m);
         ...
         (do modifying operations on m._value)
         ...
         m._cond.broadcast();
      }

  Remember, though, that the idea of this Monitor is that any state change should
  be theoretically apparent using == or != comparisons. This class is not intended
  to just be a general mutex-protector of any old interesting class. It's meant to
  be a low-level primitive to wrap things like strings, ints, sets, etc.
 */
template<typename T>
class Monitor
{
public:
  Monitor(const T & initial_value)
    throw (exception);

  Monitor()
    throw (exception);

  virtual ~Monitor()
    throw (exception);

  // These provide the raw building blocks from which a Monitor is built...
  PtMutex _mtx;
  PtCondition _cond;
  T _value;

  //-----------------------------------------------------------------------------
  // The following helper method can be used *in place of* the raw building
  // blocks listed above. THE CALLER MUST *NOT* HOLD A LOCK ON _MTX WHEN CALLING
  // ANY OF THESE FUNCTIONS...
  //-----------------------------------------------------------------------------

  void set_value(const T & v)
    throw (exception);

  // Returns a snapshot of _value's current state. If you don't wany copying to
  // occur (as this method must do), you should instead write code that looks
  // like the following:
  //    Monitor m(...);
  //    ...
  //
  {
  //       LockHolder lh(m);
  //       // make extensive, read-only use of m._value
  //    }
  T get_value_copy()
    throw (exception);

  // Blocks until (_value == v). Uses operator==.
  void await_value_equals(const T & v)
    throw (exception);

  // Blocks until (_value != v). Uses operator!=.
  void await_value_not_equals(const T & v)
    throw (exception);

  // Like awaitValueNotEquals(...), but also returns the new value.
  T get_value_when_not_equals(const T & v)
    throw (exception);
};

//===============================================================================

template<typename T> Monitor<T>::Monitor(const T & initial_value)
  throw (exception) :
  _value(initial_value)
{
}

//===============================================================================

template<typename T> Monitor<T>::Monitor()
  throw (exception)
{
}

//===============================================================================

template<typename T> Monitor<T>::~Monitor()
    throw (exception)
{
}

//===============================================================================

template<typename T> void Monitor<T>::set_value(const T & v)
    throw (exception)
{
  LockHolder lh(_mtx);
  _value = v;
  _cond.broadcast();
}

//===============================================================================

template<typename T> T Monitor<T>::get_value_copy()
    throw (exception)
{
  LockHolder lh(_mtx);
  return _value;
}

//===============================================================================

template<typename T> void Monitor<T>::await_value_equals(const T & v)
    throw (exception)
{
  while (true)
    {
      LockHolder lh(_mtx);

      if (_value == v)
    {
      return;
    }
      else
    {
      _mtx.wait_cond(_cond);
    }
    }
}

//===============================================================================

template<typename T> void Monitor<T>::await_value_not_equals(const T & v)
    throw (exception)
{
  while (true)
    {
      LockHolder lh(_mtx);

      if (_value != v)
    {
      return;
    }
      else
    {
      _mtx.wait_cond(_cond);
    }
    }
}

//===============================================================================

template<typename T> T Monitor<T>::get_value_when_not_equals(const T & v)
    throw (exception)
{
  while (true)
    {
      LockHolder lh(_mtx);

      if (_value != v)
    {
      return _value;
    }
      else
    {
      _mtx.wait_cond(_cond);
    }
    }
}


//===============================================================================

#endif

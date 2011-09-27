#ifndef RandomWalkFunction_H
#define RandomWalkFunction_H

#include "Function.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The RandomWalkFunction class implements a burst function.
 * This function has two values. and each value last for a certain
 * amount of time. The time can be periodic or random.
 */
class RandomWalkFunction: public Function
{
public:

    /**
     * RandomWalkFunction
     */
    RandomWalkFunction(
        double starting_point,
        double lower_bound,
        double upper_bound,
        double step):
      _value(starting_point),
      _lower_bound(lower_bound),
      _upper_bound(upper_bound),
      _step(step) {}

    /**
     * Returns the next value
     */
    double get_next_value()
    {
      if (drand48() > 0.5)
        _value += _step;
      else
        _value -= _step;

      if (_value > _upper_bound)
        _value = _upper_bound;
      if (_value < _lower_bound)
        _value = _lower_bound;

      return _value;
    }

    /**
     * Returns the average value
     */
    double get_average() const { return (_upper_bound - _lower_bound) / 2.0 ; }

private:

    double _value;
    double _lower_bound;
    double _upper_bound;
    double _step;

};

BOREALIS_NAMESPACE_END
#endif



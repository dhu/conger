#ifndef UniformFunction_H
#define UniformFunction_H

#include "Function.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The UniformFunction class implements a burst function.
 * This function has two values. and each value last for a certain
 * amount of time. The time can be periodic or random.
 */
class UniformFunction: public Function
{
public:

    /**
     * UniformFunction
     */
    UniformFunction(double lower_bound, double upper_bound):
      _lower_bound(lower_bound), _upper_bound(upper_bound) {}

    /**
     * Returns the next value
     */
    double get_next_value()
    {
      return drand48() * (_upper_bound - _lower_bound) + _lower_bound;
    }

    /**
     * Returns the average value
     */
    double get_average() const { return (_upper_bound - _lower_bound) / 2.0 ; }

private:

    double _lower_bound;
    double _upper_bound;

};

BOREALIS_NAMESPACE_END
#endif



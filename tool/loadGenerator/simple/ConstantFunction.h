#ifndef ConstantFunction_H
#define ConstantFunction_H

#include "Function.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The ConstantFunction class implements a burst function.
 * This function has two values. and each value last for a certain
 * amount of time. The time can be periodic or random.
 *
 * @author Ying Xing
 * @version 2.0 04/30/2004
 */
class ConstantFunction: public Function
{
   // Method(s)
public:

    /**
     * Constructors
     */
    ConstantFunction(double value):_value(value) {}

    /**
     * Returns the next value
     */
    double get_next_value() { return _value; }

    /**
     * Returns the average value
     */
    double get_average() const { return _value; }

private:

    double _value;

};

BOREALIS_NAMESPACE_END
#endif



#ifndef RandomPulseFunction_H
#define RandomPulseFunction_H

#include "Function.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The TraceFunction class returns values from a web trace
 */
class RandomPulseFunction: public Function
{
public:

    /**
     * Constructor
     */
    RandomPulseFunction();

    /**
     * Constructor 
     * @param dimension number of dimensions
     * @param dimension_index the of dimension index of this function
     * @param multiplier every data will be multiplied by the multiplier
     */
    RandomPulseFunction(
            unsigned int dimension, 
            unsigned int dimension_index, 
            double multiplier,
            double busy_period,
            double idle_peirod);

    /**
     * Returns the next value
     */
    double get_next_value();

    /**
     * Returns the average value
     */
    double get_average() const;

private:
    unsigned int _dimension;
    unsigned int _dimension_index;
    double _multiplier;
    double _busy_period;
    double _idle_peirod;

    unsigned int _current_index;
    double _accumulated_busy_period;
    double _value;

    static vector<vector<double> > _data;

    void init();

};

BOREALIS_NAMESPACE_END
#endif



#ifndef TraceFunction_H
#define TraceFunction_H

#include "Function.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The TraceFunction class returns values from a web trace
 */
class TraceFunction: public Function
{
public:

    /**
     * Constructors
     */
    TraceFunction();

    /**
     * Construct a trace function that starts from the trace data
     * from the given offset
     */
    TraceFunction(int offset);

    /**
     * Returns the next value
     */
    double get_next_value();

    /**
     * Returns the average value
     */
    double get_average() const;

private:
    int _current_index;
    int _start_index;
    int _unit;

    static vector<double> _data;
    static int _shift;

    void init();

};

BOREALIS_NAMESPACE_END
#endif



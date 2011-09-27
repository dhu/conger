#ifndef Function_H
#define Function_H

#include "common.h"

BOREALIS_NAMESPACE_BEGIN

/**
 * The Function class implements functions.
 */
class Function
{

public:
    virtual ~Function() {}

    /**
     * Returns the next value.
     */
    virtual double get_next_value() = 0;

    /**
     * Returns the average value
     */
    virtual double get_average() const = 0;

    /**
     * Returns the address of the data generated
     */
    virtual const char* get_data_handle() const { return 0; }

    /**
     * Returns the size of the data generated
     */
    virtual unsigned int get_data_size() const { return sizeof(double); }
};

BOREALIS_NAMESPACE_END
#endif



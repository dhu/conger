#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "WrapperConfig.h"
#include <string>

namespace tiny_dbwrapper
{
class Constraint
{

    friend class Scores;

public:
    Constraint();
    Constraint(double d_soft_value, double d_hard_value);
    Constraint(const Constraint& r_constraint);

    Constraint& operator=(const Constraint& r_constraint);

    double          get_soft_value();
    double          get_hard_value();
    string          get_xml();

protected:
    void            set_soft_value(double d_soft_value);
    void            set_hard_value(double d_hard_value);

private:
    double          _d_soft_value;
    double          _d_hard_value;

};

}
#endif //CONSTRAINT_H

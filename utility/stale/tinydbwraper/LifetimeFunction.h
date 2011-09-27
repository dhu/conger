#ifndef LIFETIMEFUNCTION_H
#define LIFETIMEFUNCTION_H

#include "WrapperConfig.h"
#include "ScoreFunction.h"
#include <string>
#include <vector>

namespace tiny_dbwrapper
{
class LifetimeFunction : public ScoreFunction
{

public:
    bool    is_valid();
    double  calculate_score(vector<double>& rv_metric_values);
    string  get_xml();

};

}
#endif // LIFETIMEFUNCTION_H

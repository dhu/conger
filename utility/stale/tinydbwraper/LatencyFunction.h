#ifndef LATENCYFUNCTION_H
#define LATENCYFUNCTION_H

#include "WrapperConfig.h"
#include "ScoreFunction.h"
#include <string>
#include <vector>

namespace tiny_dbwrapper
{
class LatencyFunction : public ScoreFunction
{

public:
    virtual bool    is_valid();
    virtual double  calculate_score(vector<double>& rv_metric_values);
    virtual string  get_xml();

};

}
#endif // LATENCYFUNCTION_H

#ifndef SCOREFUNCTION_H
#define SCOREFUNCTION_H

#include "WrapperConfig.h"
#include <string>
#include <vector>

namespace tiny_dbwrapper
{
class ScoreFunction
{

public:
    virtual double  calculate_score(vector<double>& rv_metric_values) = 0;
    virtual string  get_xml() = 0;

};

}
#endif // SCOREFUNCTION_H

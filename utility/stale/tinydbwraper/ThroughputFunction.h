#ifndef THROUGHPUTFUNCTION_H
#define THROUGHPUTFUNCTION_H

#include "WrapperConfig.h"
#include "ScoreFunction.h"
#include <string>
#include <vector>

namespace tiny_dbwrapper
{
class ThroughputFunction : public ScoreFunction
{

public:
    bool    is_valid();
    double  calculate_score(vector<double>& rv_metric_values);
    string  get_xml();

};

}
#endif // THROUGHPUTFUNCTION_H

#include "ThroughputFunction.h"

bool TinyDBWrapper::ThroughputFunction::is_valid()
{
    return true;
}

double TinyDBWrapper::ThroughputFunction::calculate_score(vector<double>& rv_metric_values)
{
    return rv_metric_values.at(0);
}

string TinyDBWrapper::ThroughputFunction::get_xml()
{
    return "throughput";
}

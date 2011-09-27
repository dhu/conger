#include "LatencyFunction.h"

bool TinyDBWrapper::LatencyFunction::is_valid()
{
    return true;
}

double TinyDBWrapper::LatencyFunction::calculate_score(vector<double>& rv_metric_values)
{
    return rv_metric_values.at(0);
}

string TinyDBWrapper::LatencyFunction::get_xml()
{
    return "latency";
}

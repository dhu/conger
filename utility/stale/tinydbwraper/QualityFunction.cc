#include "QualityFunction.h"

bool TinyDBWrapper::QualityFunction::is_valid()
{
    return true;
}

double TinyDBWrapper::QualityFunction::calculate_score(vector<double>& rv_metric_values)
{
    if (rv_metric_values.at(0) == 0)
    {
        return 0;
    }
    return rv_metric_values.at(1)*100/rv_metric_values.at(0);
}

string TinyDBWrapper::QualityFunction::get_xml()
{
    return "received*100/sent";
}

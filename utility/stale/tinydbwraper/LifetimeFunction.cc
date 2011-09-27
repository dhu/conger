#include "LifetimeFunction.h"

bool TinyDBWrapper::LifetimeFunction::is_valid()
{
    return true;
}

double TinyDBWrapper::LifetimeFunction::calculate_score(vector<double>& rv_metric_values)
{
  if ((rv_metric_values.at(1) == 0) || (rv_metric_values.at(1) == -1))
    {
      return -1;
    }
  return rv_metric_values.at(0) / rv_metric_values.at(1);
}

string TinyDBWrapper::LifetimeFunction::get_xml()
{
    return "transmissions_left / transmissions";
}

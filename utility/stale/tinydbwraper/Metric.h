#ifndef METRIC_H
#define METRIC_H

#include "WrapperConfig.h"
#include <string>

namespace tiny_dbwrapper
{
class Metric
{

    friend class Scores;
    friend class DataThread;

public:
    Metric();
    Metric(string str_name, double d_estimate_value);
    Metric(const Metric& r_metric);

    Metric& operator=(const Metric& r_metric);

    string          get_name();
    double          get_estimate_value();
    double          get_current_value();
    string          get_xml();

protected:
    void            set_name(string str_name);
    void            set_estimate_value(double d_estimate_value);
    void            set_current_value(double d_current_value);

private:
    string          _str_name;
    double          _d_estimate_value;
    double          _d_current_value;
};

}
#endif //METRIC_H

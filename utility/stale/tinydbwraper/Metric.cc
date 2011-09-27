#include "Metric.h"

TinyDBWrapper::Metric::Metric()
{
    _str_name           = "";
    _d_estimate_value    = 0.0;
    _d_current_value     = 0.0;
}

TinyDBWrapper::Metric::Metric(string str_name, double d_estimate_value)
{
    _str_name           = str_name;
    _d_estimate_value    = d_estimate_value;
    _d_current_value     = 0.0;
}

TinyDBWrapper::Metric::Metric(const Metric& r_metric)
{
    _str_name           = r_metric._str_name;
    _d_estimate_value    = r_metric._d_estimate_value;
    _d_current_value     = r_metric._d_current_value;
}

TinyDBWrapper::Metric& TinyDBWrapper::Metric::operator=(const Metric& r_metric)
{
    _str_name           = r_metric._str_name;
    _d_estimate_value    = r_metric._d_estimate_value;
    _d_current_value     = r_metric._d_current_value;
    return *this;
}

string TinyDBWrapper::Metric::get_name()
{
    return _str_name;
}

double TinyDBWrapper::Metric::get_estimate_value()
{
    return _d_estimate_value;
}

double TinyDBWrapper::Metric::get_current_value()
{
    return _d_current_value;
}

/*
<metric name="m1" estimatvalue="5.0"/>
*/
string TinyDBWrapper::Metric::get_xml()
{
    char pc_buffer[256];
    string str_xml = "<metric name=\"" + get_name();
    str_xml.append("\" estimatvalue=\"");
    sprintf(pc_buffer, "%f", get_estimate_value());
    str_xml.append(pc_buffer);
    str_xml.append("\"/>\n");
    return str_xml;
}

void TinyDBWrapper::Metric::set_name(string str_name)
{
    _str_name = str_name;
}

void TinyDBWrapper::Metric::set_estimate_value(double d_estimate_value)
{
    _d_estimate_value = d_estimate_value;
}

void TinyDBWrapper::Metric::set_current_value(double d_current_value)
{
    _d_current_value = d_current_value;
}

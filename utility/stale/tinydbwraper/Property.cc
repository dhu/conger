#include "Property.h"

TinyDBWrapper::Property::Property()
{
    _ut_usage_type           = ut_invalid;
    _d_estimate_usage_value   = 0.0;
}

TinyDBWrapper::Property::Property(Score& r_score, usage_type ut_usage_type, double d_estimate_usage_value)
{
    _score                 = r_score;
    _ut_usage_type           = ut_usage_type;
    _d_estimate_usage_value   = d_estimate_usage_value;
}

TinyDBWrapper::Property::Property(const Property& r_property)
{
    _score                 = r_property._score;
    _ut_usage_type           = r_property._ut_usage_type;
    _d_estimate_usage_value   = r_property._d_estimate_usage_value;
}

TinyDBWrapper::Property& TinyDBWrapper::Property::operator=(const Property& r_property)
{
    _score                 = r_property._score;
    _ut_usage_type           = r_property._ut_usage_type;
    _d_estimate_usage_value   = r_property._d_estimate_usage_value;
    return *this;
}

TinyDBWrapper::Score* TinyDBWrapper::Property::get_score()
{
    return &_score;
}

usage_type TinyDBWrapper::Property::get_usage_type()
{
    return _ut_usage_type;
}

double TinyDBWrapper::Property::get_estimate_usage_value()
{
    return _d_estimate_usage_value;
}

void TinyDBWrapper::Property::set_score(Score& r_score)
{
    _score = r_score;
}

void TinyDBWrapper::Property::set_usage_type(usage_type ut_usage_type)
{
    _ut_usage_type = ut_usage_type;
}

void TinyDBWrapper::Property::set_estimate_usage_value(double d_estimate_usage_value)
{
    _d_estimate_usage_value = d_estimate_usage_value;
}

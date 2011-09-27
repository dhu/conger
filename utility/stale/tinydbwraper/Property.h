#ifndef PROPERTY_H
#define PROPERTY_H

#include "WrapperConfig.h"
#include "Score.h"

namespace tiny_dbwrapper
{
class Property
{

public:
    Property();
    Property(Score &r_score, usage_type ut_usage_type, double d_estimate_usage_value);
    Property(const Property& r_property);

    Property& operator=(const Property& r_property);

    Score*      get_score();
    usage_type   get_usage_type();
    double      get_estimate_usage_value();

protected:
    void        set_score(Score &r_score);
    void        set_usage_type(usage_type ut_usage_type);
    void        set_estimate_usage_value(double d_estimate_usage_value);

private:
    Score       _score;
    usage_type   _ut_usage_type;
    double      _d_estimate_usage_value;

};

}
#endif //PROPERTY_H

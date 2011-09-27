#ifndef PARAMETER_H
#define PARAMETER_H

#include "WrapperConfig.h"
#include <string>

namespace tiny_dbwrapper
{
class Parameter
{

public:
    Parameter();
    Parameter(string str_name, string str_value="");
    Parameter(const Parameter& r_parameter);

    Parameter& operator=(const Parameter& r_parameter);

    string          get_name();
    string          get_value();
    string          get_xml();

protected:
    void            set_name(string str_name);
    void            set_value(string str_value);

private:
    string          _str_name;
    string          _str_value;

};

}
#endif //PARAMETER_H

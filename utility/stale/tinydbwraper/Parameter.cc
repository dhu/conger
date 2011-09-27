#include "Parameter.h"

TinyDBWrapper::Parameter::Parameter()
{
    _str_name   = "";
    _str_value  = "";
}

TinyDBWrapper::Parameter::Parameter(string str_name, string str_value)
{
    _str_name   = str_name;
    _str_value  = str_value;
}

TinyDBWrapper::Parameter::Parameter(const Parameter& r_parameter)
{
    _str_name   = r_parameter._str_name;
    _str_value  = r_parameter._str_value;
}

TinyDBWrapper::Parameter& TinyDBWrapper::Parameter::operator=(const Parameter& r_parameter)
{
    _str_name   = r_parameter._str_name;
    _str_value  = r_parameter._str_value;
    return *this;
}

string TinyDBWrapper::Parameter::get_name()
{
    return _str_name;
}

string TinyDBWrapper::Parameter::get_value()
{
    return _str_value;
}

/*
<param name="foo" value="borkbork"/>
*/
string TinyDBWrapper::Parameter::get_xml()
{
    string str_xml = "<param name=\"" + get_name() + "\" value=\"" + get_value() + "\"/>\n";

    return str_xml;
}

void TinyDBWrapper::Parameter::set_name(string str_name)
{
    _str_name = str_name;
}

void TinyDBWrapper::Parameter::set_value(string str_value)
{
    _str_value = str_value;
}

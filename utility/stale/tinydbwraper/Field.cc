#include "Field.h"

TinyDBWrapper::Field::Field()
{
    _str_name       = "";
    _dt_data_type    = Borealis::DataType::NONE;
    _i_size         = 0;
    _d_cost         = 0.0;
}

TinyDBWrapper::Field::Field(string str_name, Borealis::DataType dt_data_type, int i_size, double d_cost)
{
    _str_name       = str_name;
    _dt_data_type    = dt_data_type;
    _i_size         = i_size;
    _d_cost         = d_cost;
}

TinyDBWrapper::Field::Field(const Field& r_field)
{
    _str_name       = r_field._str_name;
    _dt_data_type    = r_field._dt_data_type;
    _i_size         = r_field._i_size;
    _d_cost         = r_field._d_cost;
}

TinyDBWrapper::Field& TinyDBWrapper::Field::operator=(const Field& r_field)
{
    _str_name       = r_field._str_name;
    _dt_data_type    = r_field._dt_data_type;
    _i_size         = r_field._i_size;
    _d_cost         = r_field._d_cost;
    return *this;
}

string TinyDBWrapper::Field::get_name()
{
    return _str_name;
}

Borealis::DataType TinyDBWrapper::Field::get_data_type()
{
    return _dt_data_type;
}

int TinyDBWrapper::Field::get_size()
{
    return _i_size;
}

double TinyDBWrapper::Field::get_cost()
{
    return _d_cost;
}

/*
<field name="f1" type="int"/>\\
<field name="f2" type="string" size="4"/>\\
*/
string TinyDBWrapper::Field::get_xml()
{
    string str_xml;

    if (get_data_type() == Borealis::DataType::STRING)
    {
        char pc_buffer[256];
        sprintf(pc_buffer, "%d", get_size());
        str_xml = "<field name=\"" + get_name() + "\" type=\"" + get_data_type().as_string() + "\" size=\"" + pc_buffer + "\"/>\n";
    }
    else
    {
        str_xml = "<field name=\"" + get_name() + "\" type=\"" + get_data_type().as_string() + "\"/>\n";
    }
    return str_xml;
}

void TinyDBWrapper::Field::set_name(string str_name)
{
    _str_name = str_name;
}

void TinyDBWrapper::Field::set_data_type(Borealis::DataType dt_data_type)
{
    _dt_data_type = dt_data_type;
}

void TinyDBWrapper::Field::set_size(int i_size)
{
    _i_size = i_size;
}

void TinyDBWrapper::Field::set_cost(double d_cost)
{
    _d_cost = d_cost;
}

#include "Constraint.h"

TinyDBWrapper::Constraint::Constraint()
{
    _d_soft_value    = 0.0;
    _d_hard_value    = 0.0;
}

TinyDBWrapper::Constraint::Constraint(double d_soft_value, double d_hard_value)
{
    _d_soft_value    = d_soft_value;
    _d_hard_value    = d_hard_value;
}

TinyDBWrapper::Constraint::Constraint(const Constraint& r_constraint)
{
    _d_soft_value    = r_constraint._d_soft_value;
    _d_hard_value    = r_constraint._d_hard_value;
}

TinyDBWrapper::Constraint& TinyDBWrapper::Constraint::operator=(const Constraint& r_constraint)
{
    _d_soft_value    = r_constraint._d_soft_value;
    _d_hard_value    = r_constraint._d_hard_value;
    return *this;
}

double TinyDBWrapper::Constraint::get_soft_value()
{
    return _d_soft_value;
}

double TinyDBWrapper::Constraint::get_hard_value()
{
    return _d_hard_value;
}

/*
<constraint softvalue="0.3" hardvalue="0.1"/>
*/
string TinyDBWrapper::Constraint::get_xml()
{
    char pc_buffer[256];
    string str_xml = "<constraint softvalue=\"";
    sprintf(pc_buffer, "%f", get_soft_value());
    str_xml.append(pc_buffer);
    str_xml.append("\" hardvalue=\"");
    sprintf(pc_buffer, "%f", get_hard_value());
    str_xml.append(pc_buffer);
    str_xml.append("\"/>\n");
    return str_xml;
}

void TinyDBWrapper::Constraint::set_soft_value(double d_soft_value)
{
    _d_soft_value = d_soft_value;
}

void TinyDBWrapper::Constraint::set_hard_value(double d_hard_value)
{
    _d_hard_value = d_hard_value;
}

#include "Box.h"
#include <stdlib.h>

using namespace std;

TinyDBWrapper::Box::Box()
{
    _str_name = "";
    _str_type = "";
    _v_inputs.erase(_v_inputs.begin(), _v_inputs.end());
    _v_outputs.erase(_v_outputs.begin(), _v_outputs.end());
    _v_parameters.erase(_v_parameters.begin(), _v_parameters.end());
    _v_properties.erase(_v_properties.begin(), _v_properties.end());
}

TinyDBWrapper::Box::Box(string str_name, string str_type, vector<Stream>& rv_inputs, vector<Stream>& rv_outputs, vector<Parameter>& rv_parameters, vector<Property>& rv_properties)
{
    _str_name = str_name;
    _str_type = str_type;
    _v_inputs.erase(_v_inputs.begin(), _v_inputs.end());
    _v_outputs.erase(_v_outputs.begin(), _v_outputs.end());
    _v_parameters.erase(_v_parameters.begin(), _v_parameters.end());
    _v_properties.erase(_v_properties.begin(), _v_properties.end());
    _v_inputs.assign(rv_inputs.begin(), rv_inputs.end());
    _v_outputs.assign(rv_outputs.begin(), rv_outputs.end());
    _v_parameters.assign(rv_parameters.begin(), rv_parameters.end());
    _v_properties.assign(rv_properties.begin(), rv_properties.end());
}

TinyDBWrapper::Box::Box(const Box& r_box)
{
    _str_name = r_box._str_name;
    _str_type = r_box._str_type;
    _v_inputs.erase(_v_inputs.begin(), _v_inputs.end());
    _v_outputs.erase(_v_outputs.begin(), _v_outputs.end());
    _v_parameters.erase(_v_parameters.begin(), _v_parameters.end());
    _v_properties.erase(_v_properties.begin(), _v_properties.end());
    _v_inputs.assign(r_box._v_inputs.begin(), r_box._v_inputs.end());
    _v_outputs.assign(r_box._v_outputs.begin(), r_box._v_outputs.end());
    _v_parameters.assign(r_box._v_parameters.begin(), r_box._v_parameters.end());
    _v_properties.assign(r_box._v_properties.begin(), r_box._v_properties.end());
}

TinyDBWrapper::Box& TinyDBWrapper::Box::operator=(const Box& r_box)
{
    _str_name = r_box._str_name;
    _str_type = r_box._str_type;
    _v_inputs.erase(_v_inputs.begin(), _v_inputs.end());
    _v_outputs.erase(_v_outputs.begin(), _v_outputs.end());
    _v_parameters.erase(_v_parameters.begin(), _v_parameters.end());
    _v_properties.erase(_v_properties.begin(), _v_properties.end());
    _v_inputs.assign(r_box._v_inputs.begin(), r_box._v_inputs.end());
    _v_outputs.assign(r_box._v_outputs.begin(), r_box._v_outputs.end());
    _v_parameters.assign(r_box._v_parameters.begin(), r_box._v_parameters.end());
    _v_properties.assign(r_box._v_properties.begin(), r_box._v_properties.end());
    return *this;
}

string TinyDBWrapper::Box::get_name()
{
    return _str_name;
}

string TinyDBWrapper::Box::get_type()
{
    return _str_type;
}

unsigned int TinyDBWrapper::Box::get_input_count()
{
    return _v_inputs.size();
}

TinyDBWrapper::Stream* TinyDBWrapper::Box::get_input(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_inputs.size())
    {
        cerr << "access error input " << i_index << ", 0..." << _v_inputs.size() << endl;
        return NULL;
    }

    return &(_v_inputs.at(i_index));
}

unsigned int TinyDBWrapper::Box::get_output_count()
{
    return _v_outputs.size();
}

TinyDBWrapper::Stream* TinyDBWrapper::Box::get_output(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_outputs.size())
    {
        cerr << "access error output " << i_index << ", 0..." << _v_outputs.size() << endl;
        return NULL;
    }

    return &(_v_outputs.at(i_index));
}

unsigned int TinyDBWrapper::Box::get_parameter_count()
{
    return _v_parameters.size();
}

TinyDBWrapper::Parameter* TinyDBWrapper::Box::get_parameter(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_parameters.size())
    {
        cerr << "access error parameter " << i_index << ", 0..." << _v_parameters.size() << endl;
        return NULL;
    }

    return &(_v_parameters.at(i_index));
}

unsigned int TinyDBWrapper::Box::get_property_count()
{
    return _v_properties.size();
}

TinyDBWrapper::Property* TinyDBWrapper::Box::get_property(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_properties.size())
    {
        cerr << "access error property " << i_index << ", 0..." << _v_properties.size() << endl;
        return NULL;
    }

    return &(_v_properties.at(i_index));
}

/*
<box name="name-of-box" type="filter">\\
    <input port="1" stream="name-of-first-input-stream"/>\\
    <input port="2" stream="name-of-second-input-stream"/>\\
    <output port="1" stream="name-of-first-output-stream"/>\\

    <param name="foo" value="borkbork"/>\\
    <param name="bar" value="aoeuhtns"/>\\
</box>
*/
string TinyDBWrapper::Box::get_xml()
{
    char c_buffer[256];
    string str_xml = "<box name=\"" + get_name() + "\" type=\"" + get_type() + "\">\n";
    for (unsigned int i=0; i<_v_inputs.size(); i++)
    {
        sprintf(c_buffer, "%d", _v_inputs.at(i).get_port());
        str_xml.append("\t<input port=\"");
        str_xml.append(c_buffer);
        str_xml.append("\" stream=\"");
        str_xml.append(_v_inputs.at(i).get_name());
        str_xml.append("\"/>\n");
    }
    for (unsigned int i=0; i<_v_outputs.size(); i++)
    {
        sprintf(c_buffer, "%d", _v_outputs.at(i).get_port());
        str_xml.append("\t<output port=\"");
        str_xml.append(c_buffer);
        str_xml.append("\" stream=\"");
        str_xml.append(_v_outputs.at(i).get_name());
        str_xml.append("\"/>\n");
    }
    for (unsigned int i=0; i<_v_parameters.size(); i++)
    {
        str_xml.append("\t");
        str_xml.append(_v_parameters.at(i).get_xml());
        str_xml.append("\"/>");
    }
    str_xml.append("</box>\n");
    return str_xml;
}

void TinyDBWrapper::Box::set_name(string str_name)
{
    _str_name = str_name;
}

void TinyDBWrapper::Box::set_type(string str_type)
{
    _str_type = str_type;
}

void TinyDBWrapper::Box::add_input(Stream& r_input)
{
    _v_inputs.insert(_v_inputs.end(), r_input);
}

void TinyDBWrapper::Box::add_output(Stream& r_output)
{
    _v_outputs.insert(_v_outputs.end(), r_output);
}

void TinyDBWrapper::Box::add_parameter(Parameter& r_parameter)
{
    _v_parameters.insert(_v_parameters.end(), r_parameter);
}

void TinyDBWrapper::Box::add_property(Property& r_property)
{
    _v_properties.insert(_v_properties.end(), r_property);
}

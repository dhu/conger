#ifndef BOX_H
#define BOX_H

#include "WrapperConfig.h"
#include <string>
#include <vector>
#include "Stream.h"
#include "Parameter.h"
#include "Property.h"

namespace tiny_dbwrapper
{
class Box
{

    friend class Catalog;
    friend class Wrapper;

public:
    Box();
    Box(string str_name, string str_type, vector<Stream>& rv_inputs, vector<Stream>& rv_outputs, vector<Parameter>& rv_parameters, vector<Property>& rv_properties);
    Box(const Box& r_box);

    Box&            operator=(const Box& r_box);

    string          get_name();
    string          get_type();
    unsigned int    get_input_count();
    Stream*         get_input(unsigned int i_index);
    unsigned int    get_output_count();
    Stream*         get_output(unsigned int i_index);
    unsigned int    get_parameter_count();
    Parameter*      get_parameter(unsigned int i_index);
    unsigned int    get_property_count();
    Property*       get_property(unsigned int i_index);
    string          get_xml();

protected:
    void            set_name(string str_name);
    void            set_type(string str_type);
    void            add_input(Stream& r_input);
    void            add_output(Stream& r_output);
    void            add_parameter(Parameter& r_parameter);
    void            add_property(Property& r_property);

private:
    string              _str_name;
    string              _str_type;
    vector<Stream>      _v_inputs;
    vector<Stream>      _v_outputs;
    vector<Parameter>   _v_parameters;
    vector<Property>    _v_properties;

};

}
#endif //BOX_H

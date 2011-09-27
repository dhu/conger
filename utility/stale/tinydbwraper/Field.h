#ifndef FIELD_H
#define FIELD_H

#include "WrapperConfig.h"
#include <string>

using namespace std;

namespace tiny_dbwrapper
{
/**
 * Attribute of an relational or stream schema in Borealis
 */
class Field
{

    friend class Catalog;

public:
    Field();
    Field(string str_name, Borealis::DataType dt_data_type, int i_size=0, double d_cost=0.0);
    Field(const Field& r_field);

    Field& operator=(const Field& r_field);

    string      get_name();
    Borealis::DataType    get_data_type();
    int         get_size();
    double      get_cost();
    string      get_xml();

protected:
    void        set_name(string str_name);
    void        set_data_type(Borealis::DataType dt_data_type);
    void        set_size(int i_size);
    void        set_cost(double d_cost);

private:
    string      _str_name;
    Borealis::DataType  _dt_data_type;
    int         _i_size;
    double      _d_cost;

};

}
#endif //FIELD_H

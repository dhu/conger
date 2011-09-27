#ifndef SCHEMA_H
#define SCHEMA_H

#include "WrapperConfig.h"
#include <string>
#include <vector>
#include "Field.h"

namespace tiny_dbwrapper
{
class Schema
{

    friend class Catalog;

public:
    Schema();
    Schema(string str_name, vector<Field>& rv_fields, schema_type st_schema_type);
    Schema(const Schema& r_schema);

    Schema& operator=(const Schema& r_schema);

    string          get_stream_name();
    string          get_name();
    schema_type      get_schema_type();
    unsigned int    get_field_count();
    Field*          get_field(unsigned int i_index);
    string          get_xml();

protected:
    void            set_stream_name(string str_stream_name);
    void            set_name(string str_name);
    void            set_schema_type(schema_type st_schema_type);
    void            add_field(Field& r_field);

private:
    string          _str_stream_name;
    string          _str_name;
    schema_type      _st_schema_type;
    vector<Field>   _v_fields;

};

}
#endif //SCHEMA_H

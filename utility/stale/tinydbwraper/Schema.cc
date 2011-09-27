#include "Schema.h"

TinyDBWrapper::Schema::Schema()
{
    _str_stream_name = "";
    _str_name       = "";
    _st_schema_type  = st_invalid;
    _v_fields.erase(_v_fields.begin(), _v_fields.end());
}

TinyDBWrapper::Schema::Schema(string str_name, vector<Field>& rv_fields, schema_type st_schema_type)
{
    _str_stream_name = str_name + "-stream";
    _str_name       = str_name;
    _st_schema_type  = st_schema_type;
    _v_fields.erase(_v_fields.begin(), _v_fields.end());
    _v_fields.assign(rv_fields.begin(), rv_fields.end());
}

TinyDBWrapper::Schema::Schema(const Schema& r_schema)
{
    _str_stream_name = r_schema._str_stream_name;
    _str_name       = r_schema._str_name;
    _st_schema_type  = r_schema._st_schema_type;
    _v_fields.erase(_v_fields.begin(), _v_fields.end());
    _v_fields.assign(r_schema._v_fields.begin(), r_schema._v_fields.end());
}

TinyDBWrapper::Schema& TinyDBWrapper::Schema::operator=(const Schema& r_schema)
{
    _str_stream_name = r_schema._str_stream_name;
    _str_name       = r_schema._str_name;
    _st_schema_type  = r_schema._st_schema_type;
    _v_fields.erase(_v_fields.begin(), _v_fields.end());
    _v_fields.assign(r_schema._v_fields.begin(), r_schema._v_fields.end());
    return *this;
}

string TinyDBWrapper::Schema::get_stream_name()
{
    return _str_stream_name;
}

string TinyDBWrapper::Schema::get_name()
{
    return _str_name;
}

schema_type TinyDBWrapper::Schema::get_schema_type()
{
    return _st_schema_type;
}

unsigned int TinyDBWrapper::Schema::get_field_count()
{
    return _v_fields.size();
}

TinyDBWrapper::Field* TinyDBWrapper::Schema::get_field(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_fields.size())
    {
        cerr << "access error field " << i_index << ", 0..." << _v_fields.size() << endl;
        return NULL;
    }

    return &(_v_fields.at(i_index));
}

/*
<schema name="MyType">
    <field name="f1" type="int"/>
    <field name="f2" type="string" size="4"/>
</schema>
<stream name="name-of-stream" schema="name-of-schema"/>
*/
string TinyDBWrapper::Schema::get_xml()
{
    string str_xml = "<schema name=\"" + get_name() + "\">\n";

    for (unsigned int i=0; i<_v_fields.size(); i++)
    {
        str_xml.append("\t");
        str_xml.append(_v_fields.at(i).get_xml());
    }
    str_xml.append("</schema>\n");
    str_xml.append("<stream name=\"" + get_stream_name() + "\" ");
    str_xml.append("schema=\"" + get_name() + "\"/>\n");

    return str_xml;
}

void TinyDBWrapper::Schema::set_stream_name(string str_stream_name)
{
    _str_stream_name = str_stream_name;
}

void TinyDBWrapper::Schema::set_name(string str_name)
{
    _str_name = str_name;
}

void TinyDBWrapper::Schema::set_schema_type(schema_type st_schema_type)
{
    _st_schema_type = st_schema_type;
}

void TinyDBWrapper::Schema::add_field(Field& r_field)
{
    _v_fields.insert(_v_fields.end(), r_field);
}

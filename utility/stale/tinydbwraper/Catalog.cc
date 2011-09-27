#include "Catalog.h"
#include "interfaces/SchemaType.h"

TinyDBWrapper::Catalog::Catalog()
{
    _v_schemata.erase(_v_schemata.begin(), _v_schemata.end());
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());
}

TinyDBWrapper::Catalog::Catalog(vector<Schema>& rv_schemata, vector<Box>& rv_boxes)
{
    _v_schemata.erase(_v_schemata.begin(), _v_schemata.end());
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());
    _v_schemata.assign(rv_schemata.begin(), rv_schemata.end());
    _v_boxes.assign(rv_boxes.begin(), rv_boxes.end());
}

TinyDBWrapper::Catalog::Catalog(const Catalog& r_catalog)
{
    _v_schemata.erase(_v_schemata.begin(), _v_schemata.end());
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());
    _v_schemata.assign(r_catalog._v_schemata.begin(), r_catalog._v_schemata.end());
    _v_boxes.assign(r_catalog._v_boxes.begin(), r_catalog._v_boxes.end());
}

TinyDBWrapper::Catalog& TinyDBWrapper::Catalog::operator=(const Catalog& r_catalog)
{
    _v_schemata.erase(_v_schemata.begin(), _v_schemata.end());
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());
    _v_schemata.assign(r_catalog._v_schemata.begin(), r_catalog._v_schemata.end());
    _v_boxes.assign(r_catalog._v_boxes.begin(), r_catalog._v_boxes.end());
    return *this;
}

unsigned int TinyDBWrapper::Catalog::get_schema_count()
{
    return _v_schemata.size();
}

TinyDBWrapper::Schema* TinyDBWrapper::Catalog::get_schema(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_schemata.size())
    {
        cerr << "access error schema " << i_index << ", 0..." << _v_schemata.size() << endl;
        return NULL;
    }

    return &(_v_schemata.at(i_index));
}

string TinyDBWrapper::Catalog::get_schemata_xml()
{
    string str_xml = "\n<!-- schemata -->\n";

    for (unsigned int i=0; i<_v_schemata.size(); i++)
    {
        str_xml.append(_v_schemata.at(i).get_xml());
    }

    return str_xml;
}

unsigned int TinyDBWrapper::Catalog::get_box_count()
{
    return _v_boxes.size();
}

TinyDBWrapper::Box* TinyDBWrapper::Catalog::get_box(unsigned int i_index)
{
    if (i_index < 0 || i_index > _v_boxes.size())
    {
        cerr << "access error box " << i_index << ", 0..." << _v_boxes.size() << endl;
        return NULL;
    }

    return &(_v_boxes.at(i_index));
}

string TinyDBWrapper::Catalog::get_boxes_xml()
{
    string str_xml = "\n<!-- boxes -->\n";

    for (unsigned int i=0; i<_v_boxes.size(); i++)
    {
        str_xml.append(_v_boxes.at(i).get_xml());
    }

    return str_xml;
}

bool TinyDBWrapper::Catalog::retrieve_catalog(JSocket& r_cmd_socket)
{
    r_cmd_socket.write_byte(CMD_GETCATALOG);
    r_cmd_socket.flush();

    BYTE* pb_buffer = NULL;
    int i_len, i_size;
    BYTE b_type;

    // get schemata
    int i_no_of_schemata = r_cmd_socket.read_int();
    for (int i_s=0; i_s<i_no_of_schemata; i_s++)
    {
        Schema new_schema;

        i_len = r_cmd_socket.read_int();
        pb_buffer = new BYTE[i_len+1];
        r_cmd_socket.read_byte_array(pb_buffer, i_len);
        pb_buffer[i_len] = '\0';
        new_schema.set_name(string((char*)pb_buffer));
        new_schema.set_stream_name(string((char*)pb_buffer) + string("-stream"));
        delete [] pb_buffer;
        new_schema.set_schema_type(st_stream);

        // get attributes for each schema
        int i_no_of_attrs = r_cmd_socket.read_int();
        for (int i_a = 0; i_a < i_no_of_attrs; i_a++)
        {
            Field new_field;

            i_len = r_cmd_socket.read_int();
            pb_buffer = new BYTE[i_len+1];
            r_cmd_socket.read_byte_array(pb_buffer, i_len);
            pb_buffer[i_len] = '\0';
            new_field.set_name(string((char*)pb_buffer));
            delete [] pb_buffer;
            b_type = r_cmd_socket.read_byte();
            new_field.set_data_type(get_borealis_type(b_type));
            i_size = r_cmd_socket.read_int();
            new_field.set_size(i_size);
            i_len = r_cmd_socket.read_int();
            pb_buffer = new BYTE[i_len+1];
            r_cmd_socket.read_byte_array(pb_buffer, i_len);
            pb_buffer[i_len] = '\0';
            new_field.set_cost(atof((char*)pb_buffer));
            delete [] pb_buffer;

            new_schema.add_field(new_field);
        }

        add_schema(new_schema);
    }

    // get operators
    int i_no_of_ops = r_cmd_socket.read_int();
    for (int i_o=0; i_o<i_no_of_ops; i_o++)
    {
        Box new_box;
        // type
        i_len = r_cmd_socket.read_int();
        pb_buffer = new BYTE[i_len+1];
        r_cmd_socket.read_byte_array(pb_buffer, i_len);
        pb_buffer[i_len] = '\0';
        new_box.set_type(string((char*)pb_buffer));
        delete [] pb_buffer;
        // name
        i_len = r_cmd_socket.read_int();
        pb_buffer = new BYTE[i_len+1];
        r_cmd_socket.read_byte_array(pb_buffer, i_len);
        pb_buffer[i_len] = '\0';
        new_box.set_name(string((char*)pb_buffer));
        delete [] pb_buffer;

        add_box(new_box);
    }

    return true;
}

void TinyDBWrapper::Catalog::clear()
{
    _v_schemata.erase(_v_schemata.begin(), _v_schemata.end());
    _v_boxes.erase(_v_boxes.begin(), _v_boxes.end());;
}

Borealis::DataType TinyDBWrapper::Catalog::get_borealis_type(BYTE b_type)
{
    switch (b_type)
    {
    case INT8: //INTONE
        return Borealis::DataType::INT;
        break;
    case UINT8: //UINTONE
        return Borealis::DataType::INT;
        break;
    case INT16: //INTTWO
        return Borealis::DataType::INT;
        break;
    case UINT16: //UINTTWO
        return Borealis::DataType::INT;
        break;
    case INT32: //INTFOUR
        return Borealis::DataType::INT;
        break;
    case UINT32: //UINTFOUR
        return Borealis::DataType::INT;
        break;
    case TIMESTAMP: //TIMESTAMP c
        return Borealis::DataType::TIMESTAMP;
        break;
    case STRING: //STRING
        return Borealis::DataType::STRING;
        break;
    case BYTES: //BYTES
        return Borealis::DataType::STRING;
        break;
    case COMPLEX_TYPE: //COMPLEX_TYPE (not yet supported)
        return Borealis::DataType::STRING;
        break;
    case 11: //UNKNOWN_TYPE
    default:
        return Borealis::DataType::STRING;
        break;
    }
    return Borealis::DataType::NONE;
}

void TinyDBWrapper::Catalog::add_schema(Schema& r_schema)
{
    _v_schemata.insert(_v_schemata.end(), r_schema);
}

void TinyDBWrapper::Catalog::add_box(Box& r_box)
{
    _v_boxes.insert(_v_boxes.end(), r_box);
}

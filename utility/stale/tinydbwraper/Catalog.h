#ifndef CATALOG_H
#define CATALOG_H

#include "WrapperConfig.h"
#include <string>
#include <vector>
#include "Schema.h"
#include "Box.h"
#include "JSocket.h"

namespace tiny_dbwrapper
{
class Catalog
{

    friend class Wrapper;

public:
    Catalog();
    Catalog(vector<Schema>& rv_schemata, vector<Box>& rv_boxes);
    Catalog(const Catalog& r_catalog);

    Catalog& operator=(const Catalog& r_catalog);

    unsigned int    get_schema_count();
    Schema*         get_schema(unsigned int i_index);
    string          get_schemata_xml();
    unsigned int    get_box_count();
    Box*            get_box(unsigned int i_index);
    string          get_boxes_xml();

protected:
    bool            retrieve_catalog(JSocket& r_cmd_socket);
    void            clear();
    Borealis::DataType      get_borealis_type(BYTE b_type);
    void            add_schema(Schema& r_schema);
    void            add_box(Box& r_box);

private:
    vector<Schema>  _v_schemata;
    vector<Box>     _v_boxes;

};

}
#endif //CATALOG_H

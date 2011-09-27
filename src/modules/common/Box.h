#ifndef MEDUSA_BOX_H
#define MEDUSA_BOX_H

#include "Name.h"
#include "StreamID.h"
#include "Catalog.h"

#include <NMSTL/serial>
#include <NMSTL/text>
#include <NMSTL/util>

BOREALIS_NAMESPACE_BEGIN

class Box
{
    Name _name;
    string _type;

  public:
    // Inputs and outputs of this box
    typedef vector< pair<int,StreamID> > StreamBindings;

    // Pair of name and values
    typedef vector< pair<string,string> > BoxParams;

  protected:
    StreamBindings _inputs;
    StreamBindings _outputs;
    BoxParams _params;

    // Table accessed by this box
    Name _table;

  public:
    Box() {}
    Box(Name name, string type) : _name(name), _type(type) {}

    Name get_name() const { return _name; }
    string get_type() const { return _type; }
    const StreamBindings get_inputs() const { return _inputs; }
    const StreamBindings get_outputs() const { return _outputs; }
    const BoxParams get_params() const { return _params; }
    string get_param(string name) const;
    const Name get_table() const { return _table; }

    void add_input(int port, StreamID stream_name);
    void add_output(int port, StreamID stream_name);
    void add_param(string name, string value);
    void add_table(string table_name);

    string as_string() const { return "Box{name=" + to_string(_name) + "}"; }
    string repr() const;

    //void   fromCatalog( CatalogBox  *box );
    void   from_catalog( CatalogBox  *box );
    string as_xml() const;

    operator const void *() const { return _name ? 0 : this; }

    unsigned int num_inputs() const { return _inputs.size(); }
    unsigned int num_outputs() const { return _outputs.size(); }

    NMSTL_SIMPLY_SERIALIZABLE(Box, << _name << _type << _inputs << _outputs << _params << _table);
    NMSTL_COMPARE_FIELDS_2(Box, _name, _type);
};

BOREALIS_NAMESPACE_END

#endif

#ifndef TABLEDESCRIPTION_H
#define TABLEDESCRIPTION_H

#include  "Name.h"
#include  "Schema.h"
#include  "Catalog.h"

#include  <NMSTL/serial>
#include  <NMSTL/text>
#include  <NMSTL/util>

BOREALIS_NAMESPACE_BEGIN

class TableDescription
{
  public:

    /// Pair of name and values.
    ///
    typedef vector< pair<string,string> > TableParams;
    typedef vector<string> TableIndexes;


    TableDescription() {}

    TableDescription(Name name, Schema  schema)
                    : _name(name), _schema(schema) {}

    TableDescription(Name name, Name    schema_name)
                    : _name(name), _schema_name(schema_name) {}

    const Name          get_name()         const { return _name; }
    const Name          get_schema_name()  const { return _schema_name; }
    const Schema       &get_schema()       const { return _schema; }
    const TableParams   get_params()       const { return _params; }
    const TableIndexes  get_indexes()      const { return _indexes; }

    void set_schema(const Schema  &schema) { _schema = schema; }

    void add_index(string field);
    void add_param(string name, string value);

    string as_string() const
    {   return("TableDescription{name=" + to_string(_name)
                           + " schema=" + to_string(_schema_name) + "}");
    }

    void  from_catalog(CatalogTable   *table);

    operator const void *() const { return _name ? 0 : this; }

    NMSTL_SIMPLY_SERIALIZABLE(TableDescription,
         << _name << _schema_name << _schema << _params << _indexes);

    NMSTL_COMPARE_FIELDS_2(TableDescription, _name, _schema_name);


  protected:

    TableParams    _params;
    TableIndexes   _indexes;


  private:

    Name     _name;
    Name     _schema_name;
    Schema   _schema;             // XXX where do we get this from?
};

BOREALIS_NAMESPACE_END

#endif                            // TABLEDESCRIPTION_H

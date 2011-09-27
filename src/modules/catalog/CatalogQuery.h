#ifndef  CATALOGQUERY_H
#define  CATALOGQUERY_H

#include  "CatalogBox.h"

BOREALIS_NAMESPACE_BEGIN

/// Query declaration.
///
class CatalogQuery
{
  public:

    CatalogQuery() {}

    /// Name of a query as a string.
    ///
    Name  get_query_name() const
    {   return( _name );
    }

    void  set_query_name( Name   name )
    {   _name = name;
    }

    CatalogTable::TableStar  *get_query_table()
    {   return( &_table );
    }

    CatalogBox::BoxStar  *get_query_box()
    {   return( &_box );
    }

    void  add_query_table( CatalogTable  *table )
    {   _table.push_back( table );
    }

    /// A query is located where the first box or table is located.
    ///
    string  get_query_node()
    {   if (!_box.empty()) 
        {   return(_box[0]->get_box_node());
        }
        else
        {   return(_table[0]->get_table_node());
        }
    }


    /// Establish default queue parameters for boxes in this query.
    ///
    void  set_in_queue(CatalogBox::InQueue  in_queue)
    {
        _in_queue = in_queue;
    }


    /// Get default queue parameters for boxes in this query.
    ///
    CatalogBox::InQueue  get_in_queue()
    {
        return(_in_queue);
    }


    void remove_query_box(Name box_name);

    string as_string() const
    {
        string r;
        r = _name.as_string();
        return r;
    }


    NMSTL_SERIALIZABLE(CatalogQuery);

    void freeze(::NMSTL::OSerial &out) const
    {
    bool shallow_freeze = false;

    out && out.begin_container(*this)
        && (out << shallow_freeze << _name)

#ifdef ENABLE_CONTAINER_SERIALIZATION
    && freeze_ptr_impl(out, _box)
    && freeze_ptr_impl(out, _table)
#endif

    && out.end_container(*this);
    }

    void unfreeze(::NMSTL::ISerial &in)
    {
    bool shallow_freeze;

    in && in.begin_container(*this)
        && (::NMSTL::__iserial_adaptor(in) << shallow_freeze << _name);

#ifdef ENABLE_CONTAINER_SERIALIZATION
    if (!shallow_freeze)
    {
        unfreeze_ptr_impl(in, _box)
        && unfreeze_ptr_impl(in, _table);
    }
#endif

    in.end_container(*this);
    }

    void shallow_freeze(::NMSTL::OSerial &out) const
    {
    bool shallow_freeze = true;

    out && out.begin_container(*this)
        && (out << shallow_freeze << _name)
        && out.end_container(*this);
    }

    NMSTL_DUMMY_SERIALREG(CatalogQuery);


  private:

    CatalogBox::InQueue      _in_queue;     // Default for boxes in this query.
    Name                     _name;         // Query name.
    CatalogBox::BoxStar      _box;          // Collection of boxes.
    CatalogTable::TableStar  _table;        // Collection of tables.
};

BOREALIS_NAMESPACE_END
#endif                       // CATALOGQUERY_H

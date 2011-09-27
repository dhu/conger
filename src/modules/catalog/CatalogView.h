#ifndef  CATALOGVIEW_H
#define  CATALOGVIEW_H

#include  "CatalogStream.h"
#include  "BoxParameter.h"

BOREALIS_NAMESPACE_BEGIN

/// Connection point view declaration.
///
class CatalogView
{
  public:
    /// Name of a connection point view as a string.
    ///
    Name  get_view_name() const
    {   return( _name );
    }


    CatalogStream  *get_view_stream() const
    {   return( _stream );
    }


    Name  get_view_stream_name() const
    {   return( _stream->get_stream_name() );
    }


    Boolean  is_order_by_field()
    {   return( !_field.empty() );
    }


    string  get_view_field() const
    {   return( _field );
    }


    uint32  get_view_start() const
    {   return( _start );
    }


    uint32  get_view_end() const
    {   return( _end );
    }

    uint32  get_view_radius() const
    {   return( _radius );
    }


    void  set_view_name( Name  name )
    {   _name = name;
    }

    void set_view_radius(uint32 radius)
    {   _radius = radius;
    }

    void  set_view_window(string   field,
                          uint32   start,
                          uint32   end)
    {
        _field = field;
        _start = start;
        _end   = end;
    }

    void  set_view_stream( Name   name,
                CatalogStream  *stream )
    {
        _stream = stream;
        _stream->set_stream_name( name );
    }


    ///  Create an XML string for a connection point view.
    ///
    string  form_view_xml();


    string as_string() const
    {
        string r = string() + "CatalogView{name=" + _name
      + ",stream=" + _stream->as_string() + ",field=" + _field
      + ",start=" + _start + ",end=" + _end
      + ",radius=" + _radius + ",box_port=" + _box_port.as_string()
      + "}" ;
    return r;
    }

    void set_box_port( BoxPort box_port )
    {   _box_port = box_port;
    }

    BoxPort *get_box_port()
    {   return  &_box_port;
    }

    BoxParameter  *get_view_parameters()
    {   return  &_parameter;
    }

    BoxParameter::ParameterMap  *get_view_parameter_map()
    {   return  _parameter.get_parameter_map();
    }

    const BoxParameter::ParameterMap *get_view_parameter_map_const() const
    {   return  _parameter.get_parameter_map_const();
    }

    Boolean is_done()
    {   return _all_done;
    }

    void done() 
    {   _all_done = true;
    }
    
    void not_done()
    {   _all_done = false;
    }


    NMSTL_SERIALIZABLE(CatalogView);

    void freeze(::NMSTL::OSerial &out) const
    {
        out && out.begin_container(*this)
      && (out << _name)
      && freeze_ptr_impl(out, _stream)
      && (out << _field << _start << _end)
      && out.end_container(*this);
    }

    void unfreeze(::NMSTL::ISerial &in)
    {
        in && in.begin_container(*this)
      && (::NMSTL::__iserial_adaptor(in) << _name)
      && unfreeze_ptr_impl(in, _stream)
      && (::NMSTL::__iserial_adaptor(in) << _field << _start << _end)
      && in.end_container(*this);
    }

    NMSTL_DUMMY_SERIALREG(CatalogView);



  private:
    Name             _name;         // Connection point view name.

    CatalogStream   *_stream;       // Stream to connect to.

    string           _field;        // Name of a field to order by.
                                    // Empty if ordering by tuple.
                                    // SchemaField Pointer???

    uint32           _start;        // Field order size or
                                    //       start value of a range.

    uint32           _end;          // End value of a range (0 if moving).
                                    // Ignored if ordering by field.

    uint32           _radius;       // Radius of the CPView for upstream revision processing

    BoxPort          _box_port;     // BoxPort that gets tuples from this view

    Boolean          _all_done;     // true if all variables have been set (because _box_port and rest are known separately)

    BoxParameter     _parameter;    // View parameters.

};

BOREALIS_NAMESPACE_END
#endif                       // CATALOGVIEW_H

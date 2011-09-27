#include <xercesc/dom/DOM.hpp>
#include "xercesDomUtil.h"

#include "CPViewDescription.h"
#include "parseutil.h"

BOREALIS_NAMESPACE_BEGIN

const  CPViewDescription::CPViewType  CPViewDescription::CPViewType::FIXED  =  CPViewDescription::CPViewType("FIXED");
const  CPViewDescription::CPViewType  CPViewDescription::CPViewType::MOVING =  CPViewDescription::CPViewType("MOVING");

const  CPViewDescription::OrderByMethod  CPViewDescription::OrderByMethod::FIELD    =  CPViewDescription::OrderByMethod("FIELD");
const  CPViewDescription::OrderByMethod  CPViewDescription::OrderByMethod::TUPLENUM =  CPViewDescription::OrderByMethod("TUPLENUM");

#define FIXED  CPViewDescription::CPViewType::FIXED
#define MOVING CPViewDescription::CPViewType::MOVING

#define FIELD    CPViewDescription::OrderByMethod::FIELD
#define TUPLENUM CPViewDescription::OrderByMethod::TUPLENUM

CPViewDescription::CPViewDescription( Borealis::Name  name,
                                  Borealis::StreamID  stream_id,
                                                 int  first,
                                                 int  last,
                                              string  order_by_field_string,
                       OrderByMethod  order_by_method,
                                 int  radius,
                              BoxPort box_port
                                    )
                            : Object( OBJECT_TYPE, name )
{
    DEBUG << "CPViewDescription constructor : with first=" << first << " last=" << last << " - thus FIXED window";
    _stream_id = stream_id;
    _first = first;
    _last = last;
    _cp_view_type = FIXED;
    _order_by_field_string = order_by_field_string;
    _order_by_method = order_by_method;
    _radius = radius;
    _box_port = box_port;
    DEBUG << "THE RADIUS IN CPVIEW IS "<<_radius;
}

CPViewDescription::CPViewDescription( Borealis::Name  name,
                                  Borealis::StreamID  stream_id,
                                                 int  size,
                                              string  order_by_field_string,
                       OrderByMethod  order_by_method,
                                 int  radius,
                             BoxPort  box_port
                                    )
                            : Object( OBJECT_TYPE, name )
{
  DEBUG << "CPViewDescription constructor : with size=" << size << " - thus MOVING window";
  _stream_id = stream_id;
  _size = size;
  _cp_view_type = MOVING;
  _order_by_field_string = order_by_field_string;
  _order_by_method = order_by_method;
  _radius = radius;
  _box_port = box_port;
  DEBUG << "THE RADIUS IN CPVIEW IS "<<_radius;
}


// copy constructor
CPViewDescription::CPViewDescription( const CPViewDescription  &other_cp_desc )
                            : Object( OBJECT_TYPE, other_cp_desc.get_name() )
{
  DEBUG << "CPViewDescription copy constructor";
  _cp_view_type = other_cp_desc._cp_view_type;
  DEBUG << "IN THE COPY CONSTRUCTER, setting the radius = to " << other_cp_desc._radius;
  _order_by_method = other_cp_desc._order_by_method;
  _first = other_cp_desc._first;
  _last = other_cp_desc._last;
  _size = other_cp_desc._size;
  _order_by_field_string = other_cp_desc._order_by_field_string;
  _stream_id = other_cp_desc._stream_id;
  _radius = other_cp_desc._radius;
  _box_port = other_cp_desc._box_port;
  DEBUG << "IN THE COPY CONSTRUCTER, setting the radius = to " << other_cp_desc._radius;

}

string CPViewDescription::get_order_by_field_string()
{
  return _order_by_field_string;
}

int CPViewDescription::get_first()
{
  if (_cp_view_type == FIXED)
    {
      return _first;
    }
  else
    {
      ERROR << "CPVIEW is MOVING. No _first available";
      return -1;
  }
}

const int CPViewDescription::get_radius()
{
  return _radius;
}

void CPViewDescription::set_radius(int new_rad)
{
  _radius = new_rad;
}


BoxPort *CPViewDescription::get_box_port()
{
  return &_box_port;
}

void CPViewDescription::set_box_port(BoxPort box_port)
{
  _box_port = box_port;
}


int CPViewDescription::get_last()
{
  if (_cp_view_type == FIXED)
    {
      return _last;
    }
  else
    {
      ERROR << "CPVIEW is MOVING. No _last available";
      return -1;
  }
}

int CPViewDescription::get_size()
{
  if (_cp_view_type == MOVING)
    {
      return _size;
    }
  else
    {
      ERROR << "CPVIEW is FIXED. No _size available";
      return -1;
    }
}



////////////////////////////////////////////////////////////////////////////////
//
void  CPViewDescription::from_catalog( CatalogView   *view )
{
//..............................................................................

    // CPViewDescription SETUP REASONING
    // An empty string if ordering by tuple.
    // field -> ordering field, empty if ordering by TUPLE
    // start -> start value of range or size of window
    // end -> end value of range, ignored if ordering by TUPLE

    set_name( view->get_view_name() );
    DEBUG << "Connection point view name is " << get_name();

    _stream_id = StreamID( view->get_view_stream_name() );
    DEBUG << "Stream is " << _stream_id;

    set_radius( view->get_view_radius() );
    DEBUG << "Radius is " << _radius;

    if ( view->is_order_by_field() )
    {   _order_by_method = FIELD;             // Ordering by field.
        _order_by_field_string = view->get_view_field();
        DEBUG << "Order on field " << _order_by_field_string;

        _size = view->get_view_start();
        _cp_view_type = MOVING;
        DEBUG << "Moving window size is " << _size;
    }
    else
    {   _order_by_method = TUPLENUM;          // Ordering by tuple.
        _order_by_field_string = "";

        if ( view->get_view_end() )
        {   _first = view->get_view_start();
            _last  = view->get_view_end();
            _cp_view_type = FIXED;
            DEBUG << "Fixed window range is " << _first << " to " << _last;
        }
        else
        {   _size  = view->get_view_start();
            _cp_view_type = MOVING;
        }
    }
    _box_port = *(view->get_box_port());

    return;
}



/////////////////////////////// deprecated
//
// Parse xml for a connection point view.
//
Status CPViewDescription::from_xml( string  xml )
{
    ptr<DOMDocument>   doc;
    const DOMElement  *root, *order, *size, *range;
//
//  <connection_point_view name={view name} stream={stream name} >
//      <order field={field name} />?
//    ( <size  value={number of tuples} />
//    | <range start={start tuple}  end={end tuple} />
//    )
//      <function name={predication function name} />  (proposed; NYI)
//  </connection_point_view>
//..............................................................................


DEBUG << xml;

try
{   doc  = parseXmlString( xml );
    root = doc->getDocumentElement();

    xml_expect_tag( root, "connection_point_view" );
    set_name( xml_attribute( root, "name", ATTR_REQUIRED ));
    DEBUG << "Connection point view name is " << get_name();

    _stream_id = StreamID( xml_attribute( root, "stream", ATTR_REQUIRED ));
    DEBUG << "Stream is " << _stream_id;

    order = xml_only_tag( root, "order", ATTR_NOT_REQUIRED );

    if ( order )
    {   xml_expect_tag( order, "order" );

        _order_by_field_string = xml_attribute( order, "field", ATTR_REQUIRED );
        _order_by_method = FIELD;

        DEBUG << "Order on field " << _order_by_field_string;
    }
    else
    {   _order_by_field_string = "";
        _order_by_method = TUPLENUM;
    }

    size = xml_only_tag( root, "size", ATTR_NOT_REQUIRED );

    if ( size )
    {   xml_expect_tag( size, "size" );

        _size = string_to_int( xml_attribute( size, "value", ATTR_REQUIRED ));
        _cp_view_type = MOVING;

        DEBUG << "Moving window size is " << _first;
    }

    range = xml_only_tag( root, "range", ATTR_NOT_REQUIRED );

    if ( range )
    {   xml_expect_tag( range, "range" );

        _first = string_to_int( xml_attribute( range, "start", ATTR_REQUIRED ));
        _last  = string_to_int( xml_attribute( range, "end",   ATTR_REQUIRED ));
        //_cp_view_type = FIXED;

        DEBUG << "Fixed window range is " << _first << " to " << _last;
    }

    /************************************ proposed???
    function = xmlOnlyTag( root, "function", ATTR_NOT_REQUIRED );

    if ( function )
    {   xmlExpectTag( function, "function" );

        name = xmlAttribute( function, "name", ATTR_REQUIRED );
        DEBUG << "Predication function is " << name;
    }
    **************************************/
}
catch ( AuroraException  &e )
{    return( e.what() );
}

return( true );
}


BOREALIS_NAMESPACE_END

#ifndef CP_VIEW_DESCRIPTION_H
#define CP_VIEW_DESCRIPTION_H

#include "common.h"
#include "Object.h"
#include "StreamID.h"
#include "CatalogView.h"

BOREALIS_NAMESPACE_BEGIN

class CPViewDescription : public Borealis::Object
{
 public:

  // because enums can't be serialized
  //  enum CPViewType { FIXED, MOVING };
  //  enum OrderByMethod { FIELD, TUPLENUM };
  class CPViewType
  {
  public:
    static const CPViewType FIXED;
    static const CPViewType MOVING;
    bool operator==(const CPViewType& T)
    {
      return (T.view_type == view_type);
    }
    string as_string() const { return view_type; }
    CPViewType(string type) : view_type(type) {}
    CPViewType() {}
    NMSTL_SIMPLY_SERIALIZABLE(CPViewType, << view_type);
  private:
    string view_type;
  };

  class OrderByMethod
  {
  public:
    static const OrderByMethod FIELD;
    static const OrderByMethod TUPLENUM;
    bool operator==(const OrderByMethod& T)
    {
      return (T.order_by_type == order_by_type);
    }
    string as_string() const { return order_by_type; }
    OrderByMethod(string type) : order_by_type(type) {}
    OrderByMethod() {}
    NMSTL_SIMPLY_SERIALIZABLE(OrderByMethod, << order_by_type);
  private:
    string order_by_type;
  };

  static const string OBJECT_TYPE; // Value of this const is set in Object.cc

  // constructors
  // when t1 and t2 are fixed (like 1pm to 2pm) = FIXED
  CPViewDescription(Borealis::Name cpview_name, Borealis::StreamID stream_id,
                    int first, int last, string order_by_field_string, OrderByMethod order_by_method, int radius, BoxPort box_port);

  // when the window is moving (like last 1hr) = MOVING
  CPViewDescription(Borealis::Name cpview_name, Borealis::StreamID stream_id,
                    int size,  string order_by_field_string, OrderByMethod order_by_method, int radius, BoxPort box_port);

  CPViewDescription(Borealis::Name cpview_name = Borealis::Name())
    : Object(OBJECT_TYPE,cpview_name) {}

  // copy constructor
  CPViewDescription(const CPViewDescription &other_cp_desc);

  // destructor
  ~CPViewDescription() {}


  CPViewType get_cpview_type() { return _cp_view_type; }
  OrderByMethod get_order_by_method() { return _order_by_method; }
  void set_order_by_method(OrderByMethod order_by_method) { _order_by_method = order_by_method; }

  string get_order_by_field_string();
  int get_first();
  int get_last();
  int get_size();

  const int get_radius();
  void set_radius(int new_rad);

  BoxPort *get_box_port();
  void set_box_port(BoxPort box_port);

  string as_string() const
  {
    return string() + "CPViewDescription{" +
      "name=" + get_name() +
      ",stream=" +_stream_id + ",viewType=" + _cp_view_type +
      ",size=" + _size + ",first=" + _first + ",last=" + _last +
      ",order-by-field=" + _order_by_field_string + ",order-by-method=" + _order_by_method +
      ",box-port=" + _box_port.as_string() +
      "}";
    }

  // No need, we're going to use the Name that is set in parent class, Object
  //string get_view_id() { return _view_id; }
  //void set_view_id(string id) { _view_id = id; }

  // parsing xml stuff -- TODO
  void  from_catalog( CatalogView   *view );
  Status from_xml(string in);           // deprecated
  Status to_xml();                      // deprecated

  Borealis::StreamID get_stream_id() { return _stream_id; }

  NMSTL_SERIAL_SUBCLASS(CPViewDescription, Object, << _stream_id << _cp_view_type
            << _order_by_method << _order_by_field_string << _first << _last << _size << _box_port);

 private:

  CPViewType _cp_view_type;
  OrderByMethod _order_by_method;

  int _first;
  int _last;
  int _size;
  int _radius;
  string _order_by_field_string;

  //string _view_id;
  Borealis::StreamID _stream_id;

  BoxPort _box_port;

};

NMSTL_TO_STRING(CPViewDescription);

BOREALIS_NAMESPACE_END

#endif

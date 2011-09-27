#ifndef REGRID_QBOX_H
#define REGRID_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN

// takes in lat/long; produces pixel points for where they lie on 640x480 US grid
// places it at where it would be if it is off the grid
class RegridQBox : public QBox
{
  friend class FilterBoxState;

private:
    EvalContext _ctxt;
    unsigned int _output_tuple_size;
    single _start_lat, _start_lon;
    single _start_lat_deg, _start_lon_deg;
    string _x_field_name, _y_field_name;
    string _array_name;
    uint32 _array_offset, _x_offset, _y_offset;
    single distance_between_two_points(single, single, single, single);
    // denotes whether to do transformation for rhs of us map
    bool _rhs;
    single _miles_per_pixel;
    SchemaArray *_array_info;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);

    // packing and unpacking states
    void set_pending_box_state(ptr<AbstractBoxState> packed_box);
    ptr<AbstractBoxState> pack_state(bool delta_only = false);
    void unpack_state(ptr<AbstractBoxState> box_state) {};
    void *create_output_tuple(const void *data, int i);
    DynamicArray *clone_array(DynamicArray *orig);
    void *iterate_over_and_null_vals(const void *data, int i);
};



class FilterBoxState : public AbstractBoxState
{
 public:
  FilterBoxState() {}
  ~FilterBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif

#ifndef DELTAAIS_QBOX_H
#define DELTAAIS_QBOX_H

#include "QBox.h"
#include "Expression.h"
#include "DynamicArray.h"
#include "RenderQBox.h" // to get ship tuple struct
BOREALIS_NAMESPACE_BEGIN


class DeltaAisQBox : public QBox
{
  friend class DeltaAisBoxState;

private:
    EvalContext _ctxt;
    unsigned int _output_tuple_size;
    single _start_lat, _start_lon;
    single _start_lat_deg, _start_lon_deg;
    string _x_field_name, _y_field_name;
    string _array_name;
    uint32 _array_offset, _x_offset, _y_offset;
    single distance_between_two_points(ship_tuple, ship_tuple);
    void build_new_updates(DynamicArray *positions);
    void cluster_points();
    void flatten_points(DynamicArray *);
    // denotes whether to do transformation for rhs of us map
    map<int, ship_tuple> _ship_cache;
    single _update_threshold, _compression_threshold;
    SchemaArray *_array_info;
    vector<ship_tuple> _new_values;
    vector<ship_tuple> _spatially_compressed;

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



class DeltaAisBoxState : public AbstractBoxState
{
 public:
  DeltaAisBoxState() {}
  ~DeltaAisBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif

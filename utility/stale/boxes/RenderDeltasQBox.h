#ifndef RENDER_DELTAS_QBOX_H
#define RENDER_DELTAS_QBOX_H
#include <string>
#include "QBox.h"
#include "Expression.h"
#include "DynamicArray.h"
#include "RenderQBox.h"


BOREALIS_NAMESPACE_BEGIN


class RenderDeltasQBox : public QBox
{
  friend class RenderDeltasBoxState;

private:
	ship_cache _cached_ships;
	DynamicArray *_map_cache;
	uint8 _original_map[640*480*3];
	string _filename;
	uint32 MAP_WIDTH, MAP_HEIGHT, MAP_DEPTH, MAP_SIZE;

    void load_map();

    EvalContext _ctxt;
    unsigned int _output_tuple_size;
    single _start_lat, _start_lon;
    uint32 _x_chunks, _y_chunks, _out_array_offset, _num_chunks, _chunk_height, _chunk_width;
    string _array_name;

    SchemaArray *_tuple_schema;
    uint32 _array_offset, _x_offset, _y_offset, _cluster_threshold;
    ship_cache update_cache(DynamicArray *updates);
    void render_points(DynamicArray *out_array, ship_cache points);
	void draw_id(DynamicArray *out_array, string id, uint32 start_x, uint32 start_y);
	void fill_point(DynamicArray *array, uint32 x, uint32 y);
	void set_pixel_red(uint32 x, uint32 y, DynamicArray *the_out);
	void erase_record(uint32 id, ship_position record);
	    
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



class RenderDeltasBoxState : public AbstractBoxState
{
 public:
  RenderDeltasBoxState() {}
  ~RenderDeltasBoxState() {}
};

BOREALIS_NAMESPACE_END

#endif

#include "SControlQBox.h"

BOREALIS_NAMESPACE_BEGIN

// --------------------------------------------------
void
SControlQBox::setup_impl() throw (AuroraException)
{
    // set the control output stream as well
    set_out_description(CONTROL_STREAM, create_control_output_description());
}

// --------------------------------------------------
void
SControlQBox::init_impl() throw (AuroraException)
{
    _output_tuple_size[DATA_STREAM] = get_out_schema(DATA_STREAM)->get_size() + HEADER_SIZE;
    _output_tuple_size[CONTROL_STREAM] = get_out_schema(CONTROL_STREAM)->get_size() + HEADER_SIZE;

    // _output indicates if we have produced any output tuples this iteration
    for ( int i = 0; i < TWO_OUTPUTS; i++)
    { _output[i] = false;
    }

}

// --------------------------------------------------
TupleDescription
SControlQBox::create_control_output_description()
{
    vector<SchemaField> fields;
    return TupleDescription(fields.begin(), fields.end());

}

// --------------------------------------------------
// Just forward a tuple on the data output stream
void
SControlQBox::emit_tuple(const Tuple& src_tuple)
{
    EnqIterator my_enq_iterator = enq(DATA_STREAM);
    Tuple dst_t = Tuple::factory(my_enq_iterator.tuple());
    dst_t.copy_tuple(src_tuple,_output_tuple_size[DATA_STREAM]);
    ++my_enq_iterator;
    _output[DATA_STREAM] = true;

}

// --------------------------------------------------
// Often used to change tuples from INSERTION to TENTATIVE or to UNDO
// All other fields remain as they are
void SControlQBox::emit_tuple_with_different_type(const Tuple& tuple, TupleType type)
{
    // Write the values
    EnqIterator my_enq_iterator = enq(DATA_STREAM);
    char* tuple_start = (char*)my_enq_iterator.tuple();
    Tuple dst_tuple = Tuple::factory(tuple_start);
    dst_tuple.copy_tuple(tuple,_output_tuple_size[DATA_STREAM]);
    dst_tuple.set_tuple_type(type);

    ++my_enq_iterator;
    _output[DATA_STREAM] = true;
}

// --------------------------------------------------
// Create and emit a control tuples on the CONTROL_STREAM
void
SControlQBox::emit_control_tuple(TupleType tuple_type, Timestamp tuple_data)
{
    emit_control_tuple(CONTROL_STREAM,tuple_type,tuple_data);
}

// --------------------------------------------------
// Create a control tuple and emit on either the CONTROL_STREAM or the DATA_STREAM
// Control tuples in our case use only two fields: tuple_type and tuple_stime
// The other fields are not set (they can take any random value)
void
SControlQBox::emit_control_tuple(int stream_id, TupleType tuple_type, Timestamp tuple_data)
{
    // Write the values
    EnqIterator my_enq_iterator = enq(stream_id);
    Tuple t = Tuple::factory(my_enq_iterator.tuple());

    // All fields (header+data) are zeroed out and header values are set to default values
    t.clear_tuple(_output_tuple_size[stream_id]);
    t.set_default_header();

    // Header: only tuple_type and tuple_stime have meaningful values
    t.set_tuple_type(tuple_type);
    t.set_tuple_stime(tuple_data);

    ++my_enq_iterator;

    _output[stream_id] = true;
}


// --------------------------------------------------
// Transform an insertion tuple into a control undo tuple
// send that tuple on the data stream
void
SControlQBox::emit_undo_tuple(const Tuple& src_tuple)
{

    INFO << "Produced undo tuple " << src_tuple.get_tuple_id();

    // Write the values
    EnqIterator my_enq_iterator = enq(DATA_STREAM);
    Tuple dst_tuple = Tuple::factory(my_enq_iterator.tuple());

    dst_tuple.copy_tuple(src_tuple,_output_tuple_size[DATA_STREAM]);
    dst_tuple.set_tuple_type(TupleType::UNDO);

    ++my_enq_iterator;
    _output[DATA_STREAM] = true;
}


// --------------------------------------------------
void
SControlQBox::notify_outputs()
{

    if ( _output[DATA_STREAM] )
    {
        _output[DATA_STREAM] = false;
        get_output(DATA_STREAM).notify_enq();
    }

    if ( _output[CONTROL_STREAM] )
    {
        _output[CONTROL_STREAM] = false;
        get_output(CONTROL_STREAM).notify_enq();
    }

}

BOREALIS_NAMESPACE_END

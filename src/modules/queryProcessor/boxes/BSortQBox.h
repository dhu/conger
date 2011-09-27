#ifndef BSORT_QBOX_H
#define BSORT_QBOX_H

#include "QBox.h"

#include "BufferList.h"
#include "FieldExt.h"
#include "HashForBufferList.h"

BOREALIS_NAMESPACE_BEGIN

class BSortQBox : public QBox
{
private:

    void add_to_buffer( char *tuple );
    void add_single_to_buffer( char *att_value, BufferList::node new_node );
    void add_double_to_buffer( char *att_value, BufferList::node new_node );
    void add_int_8to_buffer(   char *att_value, BufferList::node new_node );
    void add_int_16to_buffer(  char *att_value, BufferList::node new_node );
    void add_int_32to_buffer(  char *att_value, BufferList::node new_node );
    void add_int_64to_buffer(  char *att_value, BufferList::node new_node );
    void add_string_to_buffer( char *att_value, BufferList::node new_node );

    // something has to be passed to this method that will initialize everything

    void emit_lowest(BufferList *buffer,EnqIterator);
    void emit_tuple(char *tuple,EnqIterator);

    void parse_group_by(char *atts);

    HashForBufferList *_buffer_hash;
    BufferList     *_buffer; // this should be called "currentBuffer or something"
    char           *_group_by_atts;
    int           _slack;

    FieldExt       *_field_att;
    FieldExt       **_field_group;
    int            _num_group_by_atts;
    // This (nul-terminated) string is a concatenation (0x01 delimited)
    //of the group by attribute values from the current tuple.
    char           *_group_by_values_str;

    int            _num_tuples_emitted;
    int            _output_tuple_size;
    char           *_curr_tuple;
    char           *_output_tuple_ptr;

protected:
    void setup_impl() throw (AuroraException);
    void init_impl() throw (AuroraException);
    void run_impl(QBoxInvocation&) throw (AuroraException);
};

BOREALIS_NAMESPACE_END

#endif

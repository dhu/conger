#include "DynamicArray.h"

BOREALIS_NAMESPACE_BEGIN



////////////////////////////////////////////////////////////////////////////////
//
//  Allocate an array header followed by the array data.
//
DynamicArray  *DynamicArray::allocate_array(// Number of data bytes in the array
                                            // rounded up to a multiple of 4.
                                            //
                                            uint32   size)
{
    uint8  *head = NULL;

    SizeBlock::iterator  list;
//
// New and malloc are not thread safe:
//   data = malloc(DynamicArray::header_size() + size);
//   data = new uint8[DynamicArray::header_size() + size];
//
// Allocates:  <size 4> DynamicArray <pad to 4> <Array data> <pad to 4>
//  Consider:  <size 8> DynamicArray <pad to 8> <Array data> <pad to 8>
//..............................................................................


    //DEBUG  << "allocate size=" << size << " base @ " << DynamicArray::_free_block;
    size = (size + 3) & -4;

    // Attempt to allocate a block of the same size from a prior free.
    //
    list = _size_block.find(size);

    if (list != _size_block.end())
    {
        if ((*list).second != NULL)
        {   head = (*list).second;
            //DEBUG  << "prior free head @ " << head;
            (*list).second = (uint8  *)(*((uint32 *)head));
        }
    }
    
    // If no prior block, allocate from available space.
    //
    if (head == NULL)
    {   if (size + DynamicArray::header_size() + 4 > _more_block)
        {   FATAL << "Out of array memory.  Requested " << size << " bytes.";
        }

        head = &DynamicArray::_free_block[ARRAY_MEMORY - _more_block];
        _more_block = _more_block - size - DynamicArray::header_size() - 4;

        *(uint32 *)head = size;
        head += 4;
    }

    DEBUG  << "head @ " << head;    
    return  (DynamicArray *)head;
};



////////////////////////////////////////////////////////////////////////////////
//
// Scan the schema for arrays and copy array data to the output StreamEvent.
//
void  DynamicArray::array_deallocator(// Schema for the tuples.
                                      //
                                      TupleDescription  &td,

                                      // Tuple data.
                                      //
                                      void              *tuple,

                                      // Number of tuples buffered.
                                      //
                                      uint16       num_tuples,

                                      // out - array data
                                      //
                                      string      &array)
{
    CatalogSchema  *schema;
    DynamicArray   *header;
    uint32         *element;
    uint32          size;
    uint32          head;
    uint16          offset;
    uint16          tuple_length;

    vector<SchemaField>::iterator  f;
//..............................................................................


    schema       = td.get_schema();
    tuple_length = td.get_size();
    //DEBUG << "tuple_length=" << tuple_length << " num_tuples=" << num_tuples;

    vector<SchemaField>  &field = schema->get_schema_field();

    

    for (f = field.begin(); f != field.end(); f++)
    {   if (f->get_type() == DataType::ARRAY)
        {   offset = f->get_offset();
            //DEBUG << "offset=" << offset << " num_tuples=" << num_tuples;

            for (uint16 i = 0; i < num_tuples; i++)
            {
                // Extract the dynamic array header reference from the tuple.
                //
                element = (uint32 *)((uint8 *)tuple + offset);
                header  = (DynamicArray *)(*element);
                size    = header->array_size(f->get_array());
                head    = DynamicArray::header_size();

                DEBUG << "Array field offset=" << offset << " size=" << size
                      << " header size=" << head;

                // Append the array data to the output buffer.
                //
                array += string((const char *)((uint8 *) header + head),
                                (size_t)size);

                // Insert the array size in the array tuple.
                //
                *element = size;

                // Deallocate the array if no more references.
                //
                header->modify_array_reference(-1);

                offset += tuple_length;
            }
        }
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Increase or decrease and array reference count.
//  The array is freed if the reference count changes to zero.
//
void  DynamicArray::modify_array_reference(int16   delta)
{
    uint32  size;
    uint8  *tail = NULL;

    SizeBlock::iterator  list;
//
// not thread safe:  free((void *)this);
//..............................................................................


    if (_reference_count < delta)
    {   FATAL << "Array reference count can't go below zero.  delta=" << delta
              << "  _reference_count=" << _reference_count;
    }

    _reference_count += delta;

    if (_reference_count == 0)
    {
      //uint8* elements = ((uint8 *) this) + header_size();
      //DEBUG << "Dealloc sees aray starting with " << to_hex_string(elements, 20);
        size = *((uint32 *)((uint8 *)this - 4));
        DEBUG << "Deallocate array.  size = " << size << "   data @ " << this;
		list = _size_block.find(size);

        if (list != _size_block.end())
        {   tail = (*list).second;
        }

        (*list).second  = (uint8 *)this - 4;
        *(uint32  *)((*list).second) = (uint32)tail;
    }

    return;
};



////////////////////////////////////////////////////////////////////////////////
//
// Static members need to be initialized or referenced outside the class.
//
//..............................................................................


// Available memory for arrays.
//
uint8         DynamicArray::_free_block[ARRAY_MEMORY];


// Amount of space remaining.
//
uint32        DynamicArray::_more_block = ARRAY_MEMORY;


// Free blocks in linked lists by the array data size.
//
DynamicArray::SizeBlock     DynamicArray::_size_block;



BOREALIS_NAMESPACE_END

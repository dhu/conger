#include "IOQueue.h"

BOREALIS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
IOQueue::~IOQueue()
{
//..............................................................................


    if (_last_insertion_tuple)
    { 
        delete _last_insertion_tuple;
    }
}



////////////////////////////////////////////////////////////////////////////////
//
string   IOQueue::get_last_insertion_tuple_as_string()
{
//..............................................................................


    if (!_last_insertion_tuple)
    { 
        return  string();
    }

    return  string(_last_insertion_tuple->data(),_tuple_size);
}




////////////////////////////////////////////////////////////////////////////////
//
// Find the last INSERTION tuple and remember it
// Note if there are any TENTATIVE tuples after the lst INSERTION
// Return true if there are any TENTATIVE or UNDO tuples to clean
//
bool  IOQueue::inspect_data(char* buf, int length, size_t tuple_size)
{
    int32  index = 0;
    int32  last_insertion_tuple_index = -1;

    bool   needs_cleaning = false;
    bool   found_insertion_tuple = false;
//..............................................................................


    // DEBUG  << "length=" << length << "  tuple_size=" << tuple_size;

    while (index < length)
    {
        Tuple tuple = Tuple::factory((const char*)(buf + index));
        
        // If we find an INSERTION tuple, we are done.
        //
        if (tuple.get_tuple_type() == TupleType::INSERTION)
        { 
            last_insertion_tuple_index = index;
            found_insertion_tuple = true;
        }
        else if (tuple.get_tuple_type() == TupleType::TENTATIVE)
        {   // If we find a TENTATIVE tuple before we are done, then remember it.
            //
            _tentative = true;
            needs_cleaning = true;
        }
        else if (tuple.get_tuple_type() == TupleType::RECONCILIATION_DONE)
        {
            _tentative = false;
            _stabilizing = false;
        }
        else if (tuple.get_tuple_type() == TupleType::UNDO)
        {
            INFO << "Saw an UNDO go by";
            _stabilizing = true;
            needs_cleaning = true;
        }

        index += tuple_size;
    }

    if (found_insertion_tuple)
    { 
        Tuple  tuple = Tuple::factory((const char*)(buf + last_insertion_tuple_index));
        update_last_insertion_tuple(tuple, tuple_size);
    }

    return  needs_cleaning;

}



////////////////////////////////////////////////////////////////////////////////
//
// Remove all TENTATIVE and UNDO tuples from the buffer 
// Assume they can appear anywhere inside the buffer
//
int  IOQueue::clean_data(char   *src_buf, int    src_length, 
                         char   *dst_buf, int    dst_length, 
                         size_t  tuple_size)
{
//..............................................................................

    
    char* begin = src_buf;
    char* end = src_buf + src_length;

    char* copy_start = begin;
    char* copy_end = copy_start;

    char* dst_index = dst_buf;

    // Continue until we get to the end of the buffer
    while (copy_end < end)
    {
        // Find next stable INSERTION
        Tuple tuple = Tuple::factory(copy_start);
        while ( ( tuple.get_tuple_type() != TupleType::INSERTION ) && ( copy_start < end) )
        {
            copy_start += tuple_size;
            tuple = Tuple::factory(copy_start);
        }

        // Find next TENTATIVE or UNDO or RECONCILIATION_DONE
        copy_end = copy_start + tuple_size;
        while ( ( tuple.get_tuple_type() != TupleType::TENTATIVE ) && 
                ( tuple.get_tuple_type() != TupleType::UNDO ) && 
                //( tuple.get_tuple_type() != TupleType::RECONCILIATION_DONE ) &&  --> No, we store RECONCILIATION_DONE
                ( copy_end < end ) )
        {
            copy_end += tuple_size;
            tuple = Tuple::factory(copy_end);
        }

        // Now copy over the selection
        if ( ( copy_start < end ) && ( copy_end <= end) )
        {
            int copy_length = copy_end - copy_start;
            memcpy(dst_index,copy_start,copy_length);
            dst_index += copy_length;
        }

        // Prepare to find the next set of tuples to copy
        copy_start = copy_end;

    }


    // DEBUGGING
    //DEBUG << "Checking original buffer of size " << src_length/tuple_size;
    //for ( char* i = begin; i < end; i += tuple_size )
    //{
    //  Tuple t = Tuple::factory(i);
    //  DEBUG << "(" << t.get_tuple_type() << "," << t.get_tuple_id() << ")";
    //}
    //DEBUG << "End checking original buffer";

    //DEBUG << "Checking the cleaned-up buffer of size " << (dst_index - dst_buf)/tuple_size;
    //for ( char* i = dst_buf; i < dst_index; i += tuple_size )
    //{
    //  Tuple t = Tuple::factory(i);
    //  DEBUG << "(" << t.get_tuple_type() << "," << t.get_tuple_id() << ")";
    //}
    //DEBUG << "End checking cleaned-up buffer";


    // return the updated length of the destination buffer
    return  dst_index - dst_buf;
}


////////////////////////////////////////////////////////////////////////////////
//
void  IOQueue::update_last_insertion_tuple(const Tuple& src_tuple, size_t tuple_size)
{
//..............................................................................


    if (!_last_insertion_tuple)
    {
        _tuple_size = tuple_size;
        _last_insertion_tuple = new dynbuf(tuple_size);
        memset(_last_insertion_tuple->data(),0,tuple_size);
    }

    if (tuple_size != _tuple_size)
    {   FATAL << "The tuple size should remain constant on a stream.";
    }

    Tuple dst_tuple = Tuple::factory(_last_insertion_tuple->data());
    dst_tuple.copy_tuple(src_tuple,tuple_size);

    return;
}


BOREALIS_NAMESPACE_END

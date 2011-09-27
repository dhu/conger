#ifndef BOREALIS_DYNAMICARRAY_H
#define BOREALIS_DYNAMICARRAY_H

#include  "TupleDescription.h"

BOREALIS_NAMESPACE_BEGIN


/// Amount of space initially allocated for arrays.
///
#define  ARRAY_MEMORY   1000000000


class DynamicArray
{

  public:           // Should be private???

    /// Frame bit tells us when it is the
    /// last chunk in an array being passed through
    ///
    uint8      _frame_bit;


    /// Chunk-specific entries - Experimental???
    ///
    uint32     _start_coord[3];
    uint32     _dim_length[3];


  private:

    /// Position
    ///
    struct Position
    {
        uint8  *sparse;
        uint8  *sparse_end;
        uint8  *dense;
        uint8  *dense_end;
    };


    /// The leftmost dimension of an array that may vary in size.
    /// Set to 1 if the array has all static dense dimensions.
    /// If the array has sparse dimensions this is the number of
    /// sparse elements.
    ///
    int32      _variable_size;


    /// Reference counter containing the number of downstream references.
    /// When it goes to zero the array is deallocated.
    /// Currently only input and output transitions are supported.
    ///
    uint16     _reference_count;



    ////////////////////////////////////////////////////////////////////////
    //
    //  Memory management hack for arrays.
    //  These variables are initialized in:  DynamicArray.cc
    //......................................................................


    /// Available memory for arrays.
    ///
    static   uint8         _free_block[ARRAY_MEMORY];


    /// Amount of space remaining.
    ///
    static   uint32        _more_block;    // = ARRAY_MEMORY;


    /// Free blocks in linked lists by the array data size.
    /// Each free entry has:  @next, <free space>
    /// If in use they have:  size, header, <array data>
    ///
    typedef  map<uint32, uint8 *>  SizeBlock;

    static   SizeBlock     _size_block;


    /// Position for a single iterator.
    ///
    static   Position      _position;

	uint8 _jump_size;
	uint8 *_current_pos;
	uint8 *_end_boundary;
	
  public:


    /// A single array index.
    ///
    union Index
    {
        int32   Integer;      // Integer index.
        uint8  *String;       // Null terminated string (truncated to max).
    };


    /// Access the first element in an array.
    /// Only does dense arrays for now
    ///
    uint8 *first_position()
    {
    	_current_pos = (uint8 *) this;
		int s_jump = header_size();
		_current_pos += s_jump;
    	return _current_pos;
    } 

	// must be called at least once before iterator can be run
	void save_jump(SchemaArray *array_information)
	{
		if(array_information->get_element_type() == DataType::NONE)
		{
			
			_jump_size = array_information->get_tuple()->get_size();
			
		}
		else
			_jump_size = array_information->get_element_size();
	}
       
	uint32 get_jump()
	{
		return _jump_size;
	}
	// Save off size for boundary calculations
	void set_boundary(int array_size)
	{
		_end_boundary = (uint8 *) this + array_size + header_size();
	}	
	
	uint8 * get_boundary()
	{
		return  _end_boundary;
	}
	/// Access the next element in an arbitrary (storage) order.
    /// Returns NULL when the end is reached.
    ///
    
	uint8 *get_iterator_element()
	  {
	  	// if any part of the element is out of bounds  
	    if(_current_pos + _jump_size >= _end_boundary)
	      return 0;
	    else
	      return _current_pos;
	  }
    
    void increment_iterator()
    {

    	_current_pos += _jump_size;
    	
    } 


    /// Index to an element and return the address of the element.
    /// Returns null if a sparse element is not present.
    /// An exception is thrown if a dense dimension index is out of bounds.
    /// Undefined behavior if the index types do not correspond to the
    /// schema.
    ///
    uint8  *index_element(/// One index for each dimension.
                          ///
                          Index  *index[])
                    throw(AuroraBadEntityException);


    /// Index to an element or the next one after by dimension.
    /// Only for sparse indecies.
    /// Returns null if no next element is present in the given dimension.
    /// An exception is thrown if a dense dimension index is out of bounds
    /// or the dimension does not specify a sparse index.
    /// Undefined behavior if the index types do not correspond to the
    /// schema.
    ///
    uint8  *index_next_element(/// One index for each dimension.
                               /// Updated to the position if successful.
                               ///
                               Index  *index[],

                               /// Position within sparse indecies.
                               /// Updated to the next position if successful.
                               ///
                               uint8  *position,

                               /// Sparse dimension (rightmost = 0).
                               ///
                               uint8   dimension = 0)
                         throw(AuroraBadEntityException);


    /// Index to an element or the prior one by dimension.
    /// Only for sparse indecies.
    /// Returns null if no prior element is present in the given dimension.
    /// An exception is thrown if a dense dimension index is out of bounds
    /// or the dimension does not specify a sparse index.
    /// Undefined behavior if the index types do not correspond to the
    /// schema.
    ///
    uint8  *index_prior_element(/// One index for each dimension.
                                ///
                                Index  *index[],

                                /// Position within sparse indecies.
                                /// Updated to the next position if successful.
                                ///
                                uint8  *position,

                                /// Sparse dimension (rightmost = 0).
                                ///
                                uint8   dimension = 0)
                          throw(AuroraBadEntityException);


    /// Compute the step between elements for a dense dimension.
    /// This is used as an argument to next_element and prior_element.
    ///
    uint32  dimension_step(/// Dimension (rightmost = 0).
                           ///
                           uint8  dimension = 0);


    /// Next element in a sparse dimension.
    /// Returns null if no next element is present in the given dimension.
    ///
    uint8  *next_element(/// Position within sparse indecies.
                         /// Updated to the next position if successful.
                         ///
                         uint8  *position,

                         /// Dimension (rightmost = 0).
                         ///
                         uint8   dimension = 0);


    /// Prior element in a sparse dimension.
    /// Returns null if no prior element is present in the given dimension.
    ///
    uint8  *prior_element(/// Position within sparse indecies.
                          /// Updated to the next position if successful.
                          ///
                          uint8  *position,

                          /// Dimension (rightmost = 0).
                          ///
                          uint8  dimension = 0);


    /// Get the index associated with a sparse dimension.
    ///
    Index  get_index(/// Position within sparse indecies.
                     /// Updated to the next position if successful.
                     ///
                     uint8  *position,

                     /// Dimension (rightmost = 0).
                     ///
                     uint8  dimension = 0);


    /// Establish the array reference count and any variable dimension size.
    ///
    void  set_count_size(/// Initial number of tuples referencing an array.
                         uint16  count = 1,

                         /// Variable array size (1 if constant).
                         /// For arrays with sparse indecies, this is the
                         /// number of sparse elements.
                         ///
                         uint32  size  = 1)
    {
        _reference_count = count;
        _variable_size   = size;
    }


    /// Return the size of an array; which may be determined dynamically.
    ///
    uint32  array_size(SchemaArray  *array)
    {   
    	return  _variable_size * array->array_size();
    }


    ///  Allocate an array header followed by the array data.
    ///
    static DynamicArray  *allocate_array(/// Number of data bytes in the array.
                                         ///
                                         uint32   size);


    /// Scan the schema for arrays and copy array data to the output StreamEvent.
    ///
    static void  array_deallocator(/// Schema for the tuples.
                                   ///
                                   TupleDescription  &td,

                                   /// Tuple data.
                                   ///
                                   void        *tuple,

                                   /// Number of tuples buffered.
                                   ///
                                   uint16       num_tuples,

                                   /// out - array data
                                   ///
                                   string      &array);


    ///  Increase or decrease and array reference count.
    ///  The array is freed if the reference count changes to zero.
    ///
    void  modify_array_reference(int16  delta);


    /// Size of this header rounded up to a 4 byte boundary.
    ///
    static uint32  header_size()
    {
        return  (sizeof(DynamicArray) + 3) & -4;
    }
};


BOREALIS_NAMESPACE_END

#endif                          // BOREALIS_DYNAMICARRAY_H

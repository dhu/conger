#ifndef  SCHEMAARRAY_H
#define  SCHEMAARRAY_H

#include  "common.h"
#include  "DataType.h"

BOREALIS_NAMESPACE_BEGIN

class  CatalogSchema;

/// Define an array type.
///
class SchemaArray
{
  public:

    struct DimensionInformation
    {
        /// Lower bound of the dimension.
        ///
        int32    _lower;


        /// Upper bound of the dimension.
        /// If this is the leftmost dimension of a variable sized array,
        /// set _upper to _lower and set _variable_size to True.
        ///
        int32    _upper;


        /// Index type for sparse dimensions.  0 if dense.
        /// For a sparse integer index set this to -1.
        /// Set to the maximum string size for a sparse string index.
        /// Also, if any dimension is sparse, set _variable_size to True.
        /// 
        int16    _sparse;


        DimensionInformation(/// Lower bound of a dense dimension.
                             /// 
                             int32    lower,

                             /// Upper bound of a dense dimension.
                             /// 
                             int32    upper,

                             /// Index type for sparse dimensions.  0 if dense.
                             /// 
                             int16    sparse = 0
                            )
            :
                _lower(lower),
                _upper(upper),
                _sparse(sparse)
        {
            if (sparse)
            {   _lower = 0;        // Clear just in case the caller messed up.
                _upper = 0;
            }
        }
    };



    SchemaArray(DataType        type  = DataType::NONE,
                uint16          size  = 0,
                CatalogSchema  *tuple = NULL)
        :
            _element_type(type),
            _element_size(size),
            _element_tuple(tuple),
            _variable_size(False),
            _irregular_stride(False)
    {}


    /// Add each dimension from left to right.  DEPRECATED
    ///
    void  add_dimension(/// Min bound:  ['+' | '-'] <digit>+
                        /// or Sparse index type:  int | string '[' <digit>+ ']'
                        ///
                        int32    lower,

                        /// Max bound:  {(['+' | '-'] <digit>+) | '*'}
                        /// ignored if sparse.
                        ///
                        int32    upper,

                        /// Index type for sparse dimensions.  0 if dense.
                        /// 
                        int16    sparse = 0
                       )
                  throw(AuroraBadEntityException)
    {
        _array_specification.push_back(DimensionInformation(lower,
                                                            upper, sparse));
    }



    /// Add each dimension from left to right.  DEPRECATED
    ///
    void  add_dimension(DimensionInformation  dimension)
                  throw(AuroraBadEntityException)
    {
        _array_specification.push_back(dimension);
    }



    /// Parse indexies in a single dimension from left to right.
    ///
    DimensionInformation  parse_dimension
                       (
                        /// Min bound:  ['+' | '-'] <digit>+
                        /// or Sparse index type:  int | string '[' <digit>+ ']'
                        ///
                        string    lower,

                        /// Max bound:  {(['+' | '-'] <digit>+) | '*'}
                        /// ignored if sparse.
                        ///
                        string    upper
                       )
                  throw(AuroraBadEntityException);



    /// Return the number of bytes used to store array data.
    /// If the array size can vary this returns a value where the
    /// leftmost dimension is one element wide or there is one sparse element.
    /// Use DataPath::DynamicArray::array_size() to get the actual array size.
    ///
    uint32  array_size();



    /// Return the array type with bounds as:  <type> ( <bound>, ... )
    ///
    string  array_type();



    /// Return the size of each element.
    ///
    uint16  get_element_size()
    {
        return  _element_size;
    }



    /// Return the type of each element.
    ///
    DataType  get_element_type()
    {
        return  _element_type;
    }



    /// Return the schema of array elements which are tuples.
    /// Null if the elements are intrinsic types.
    /// 
    CatalogSchema  *get_element_schema()
    {
        if (_element_type != DataType::NONE)
        {
            return  NULL;
        }

        return  _element_tuple;
    }



    /// Return all dimensions for an array.
    ///
    vector<DimensionInformation>  &get_all_dimensions()
    {
        return  _array_specification;
    }



    ///
    ///
    void  clone_dimensions(vector<DimensionInformation> dimensions)
    {
        _array_specification.clear();
        vector<DimensionInformation>::iterator d = dimensions.begin();

        while(d != dimensions.end())
        {
            _array_specification.push_back(*d);
            ++d;
        }
    }


    ///  True if the array size varies on a per tuple basis.
    ///
    Boolean  is_variable()
    {
        return  _variable_size;
    }


    
    /// Returns tuple if index values are not scalar
    /// else returns null
    ///
    CatalogSchema * get_tuple()
    {
        return  _element_tuple;
    }



  private:

    /// Scalar type of array elements; NONE if the elements are tuples.
    ///
    DataType        _element_type;


    /// Number of bytes in each element.
    ///
    uint16          _element_size;


    /// At the tuple schema when array elements are tuples.
    /// If elements are scalar this field is undefined.
    ///
    CatalogSchema  *_element_tuple;


    ///  True if the array size varies on a per tuple basis.
    ///
    Boolean         _variable_size;


    ///  NYI:  True if array strides are irregular; False if regular.
    ///
    Boolean         _irregular_stride;


    /// Characteristics of each dimension in the array.
    /// Element 0 corresponds to the low order dimension.
    ///
    vector<DimensionInformation>  _array_specification;
};


BOREALIS_NAMESPACE_END
#endif                           // SCHEMAARRAY_H

#ifndef FIELDEXT_H
#define FIELDEXT_H

#include <vector>
#include "OFunction.h"
#include "DataType.h"

//using namespace std;
BOREALIS_NAMESPACE_BEGIN

class FieldExt : public OFunction
{
  public:
    FieldExt( int32 port, char type, int32 offset, int32 size );
    FieldExt( const char*   ident_string );
    FieldExt( const string  ident_string );
    virtual ~FieldExt();
    FieldExt() { COREDUMP << "Should not be called!"; };

    virtual char* evaluate( char  *tuple );

    virtual char* evaluate_as_char( char   *tuple,
                                  int32  &return_size );

    virtual char* evaluate_as_char( char   *tuple,
                                  int32  &return_size,
                                  bool    addnull );

    virtual char* evaluate( char  *tuple1, char  *tuple2 );

    /// Evaluate the FieldExt and put the value into the position
    /// specified by loc. Loc should be large enough.
    /// This is used by SimpleAggBox.
    void evaluate_into_location( char  *tuple, char  *loc );

    int32  get_returned_size();
    char   getType();
    int32  get_offset();
    void   set_port( int32  i );

    vector<DataType> get_return_types();

    /// Returns a string representation of a field.
    string as_string() const;


private:
    void init_from_string( const char*  ident_string );

   int32   _port;
   char    _type;
   int32   _offset;
   int32   _size;
};

BOREALIS_NAMESPACE_END
#endif                         // FIELDEXT_H

#ifndef  TUPLEGENERATOR_H
#define  TUPLEGENERATOR_H

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "TupleDescription.h"

BOREALIS_NAMESPACE_BEGIN
//using namespace std;

class TupleGenerator
{
  public:
    static void  fill_with_int_8(  char  *p );
    static void  fill_with_int_16(  char  *p );
    static void  fill_with_int_32(  char  *p );
    static void  fill_with_int_64(  char  *p );
    static void  fill_with_single( char  *p );
    static void  fill_with_double( char  *p );
    static void  fill_with_timestamp( char  *p );
    static void  fill_with_tuple_type( char  *p );
    static void  fill_with_uint_16( char  *p );
    static void  fill_with_bool( char  *p );

    static void  print_int_8( const char  *p );
    static void  print_int_16( const char  *p );
    static void  print_int_32( const char  *p );
    static void  print_int_64( const char  *p );
    static void  print_single( const char  *p );
    static void  print_double( const char  *p );

    static char* generate_tuple( const char  *format);
    static char* generate_tuple( TupleDescription *td );

    static void  print_tuple( const  char* t, const char  *format );
    static void  print_tuple( const  char* t, TupleDescription *td );

  private:
    static void  fill_with_char( char  *p );

    static void  print_char( const char  *p );
    static void  print_timestamp( const char  *p );
};

BOREALIS_NAMESPACE_END
#endif                        // TUPLEGENERATOR_H

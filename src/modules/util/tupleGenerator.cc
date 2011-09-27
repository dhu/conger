#include "tupleGenerator.h"

BOREALIS_NAMESPACE_BEGIN


void TupleGenerator::fill_with_int_8( char  *p )
{
    int8 i = rand() % 65536;

    *( (int8*)p ) = i;
}


void TupleGenerator::fill_with_int_16( char  *p )
{
    int16 i = rand() % 65536;

    *( (int16*)p ) = i;
}


void TupleGenerator::fill_with_int_32( char  *p )
{
    int32 i = rand() % 65536;

    *( (int32*)p ) = i;
}


void TupleGenerator::fill_with_int_64( char  *p )
{
    int64 i = rand() % 65536;

    *( (int64*)p ) = i;
}



void TupleGenerator::fill_with_timestamp( char  *p )
{
  timeval  t;

  gettimeofday( &t, NULL );
  *( (timeval*)p ) = t;

  /**
   // Deprecated -- avoid (it should be a timeval, not a time_t
   // In the meantime, just make two ints (timeval = two ints for us)
   time_t t;
   time( &t );

   printf( "[tupleGenerator] WARNING: fillWithTimestamp is deprecated (not updated to timeval)\n" );
   *( (time_t*)p ) = t;
   */
}


void TupleGenerator::fill_with_single( char  *p )
{
    int i = rand() % 4;
    int j = 1 + (int) (10.0*rand() / (RAND_MAX+1.0));
    single  d = i + (single)(j / 100.0);

    *( (single*)p ) = d;
}

void TupleGenerator::fill_with_double( char  *p )
{
    int i = rand() % 4;
    int j = 1 + (int) (10.0*rand() / (RAND_MAX+1.0));
    double  d = i + (double) (j / 100.0);

    *( (double*)p ) = d;
}



void TupleGenerator::fill_with_char( char  *p )
{
    char c;

    c = (char) (rand() % 4) + 97; // a through d
    *(p) = c;
}


void TupleGenerator::print_int_32( const char  *p )
{   printf( "%d", (int)*((const int32*)p));
}


void TupleGenerator::print_int_8( const char  *p )
{   printf( "%d", (int)*((const int8*)p));
}


void TupleGenerator::print_int_16( const char  *p )
{   printf( "%d", (int)*((const int16*)p));
}


void TupleGenerator::print_int_64( const char  *p )
{   printf( "%" I64(d), *((const int64*)p));
}


void TupleGenerator::print_single(const char  *p)
{   printf( "%f", *( (const single*)p ));
}


void TupleGenerator::print_double( const char  *p )
{   printf( "%f", *( (const double*)p ));
}


void TupleGenerator::print_char( const char  *p )
{
    printf( "%c",*( p ));
}


// a timeval is printed as two ints(longs really)
void TupleGenerator::print_timestamp( const char  *p )
{
    print_int_32( p );
    printf( "][" );
    print_int_32( p + sizeof( int32 ));
}


char* TupleGenerator::generate_tuple( const char  *format )
{
    int memsize = 0;

    // Find out the memory we need
    for ( int i = 0; format[ i ] != '\0'; i++ )
    {   if      ( format[ i ] == 'i' ) memsize += sizeof( int32  );
        else if ( format[ i ] == 'L' ) memsize += sizeof( int64  );
        else if ( format[ i ] == 'B' ) memsize += sizeof( int16  );
        else if ( format[ i ] == 'S' ) memsize += sizeof( int8   );
        else if ( format[ i ] == '1' ) memsize += sizeof( single );
        else if ( format[ i ] == '2' ) memsize += sizeof( double );
        else if ( format[ i ] == 'c' ) memsize += sizeof( char );
        else if ( format[ i ] == 't' ) memsize += sizeof( timeval );
        else
        {   printf( "generateTuple: Unexpected format character [%c]!\n",
                    format[ i ]);

            abort();
        }
    }

    // Now get memory
    char* t = (char*) malloc( memsize );

    // Now actually create the stuff inside
    int offset = 0;

    for ( int i = 0; format[ i ] != '\0'; i++ )
    {
        if ( format[ i ] == 'i' )
        {   fill_with_int_32( t + offset ); offset += sizeof( int32 );
        }
        else if ( format[ i ] == 'L' )
        {   fill_with_int_64( t + offset ); offset += sizeof( int64 );
        }
        else if ( format[ i ] == 'B' )
        {   fill_with_int_8( t + offset ); offset += sizeof( int8 );
        }
        else if ( format[ i ] == 'S' )
        {   fill_with_int_16( t + offset ); offset += sizeof( int16 );
        }
        else if ( format[ i ] == '1' )
        {   fill_with_single( t + offset ); offset += sizeof( single );
        }
        else if ( format[ i ] == '2' )
        {   fill_with_double( t + offset ); offset += sizeof( double );
        }
        else if ( format[ i ] == 'c' )
        {   fill_with_char( t + offset ); offset += sizeof( char );
        }
        else if ( format[ i ] == 't' )
        {   fill_with_timestamp( t + offset ); offset += sizeof( timeval );
        }
        else
        {   printf( "generateTuple: Unexpected format character [%c]!\n",
                    format[ i ]);

            abort();
        }
    }

    return( t );
}


void TupleGenerator::print_tuple( const char  *t, TupleDescription  *td )
{
    bool in_string = false;

    printf( "Tuple: " );
    int offset = 0;

    // First and foremost, the timestamp and sid
    printf( "[" );

    print_timestamp( t + offset );
    offset += sizeof( timeval );

    printf( "][" );

    print_int_32( t + offset );
    offset += sizeof( int32 );

    printf( "]" );

    // The fields
    for (unsigned int i = 0; i < td->get_num_fields(); i++)
    {
        if ( !in_string ) printf( "[" );

        if ( td->get_schema_field(i)->get_type() == DataType::INT )
        {   if ( in_string )
            {
                printf( "]" );
                in_string = false;
                printf( "[" );
            }

            print_int_32( t + offset );
            offset += sizeof( int32 );
            printf( "]" );
        }
        else if ( td->get_schema_field(i)->get_type() == DataType::LONG )
        {   if ( in_string )
            {
                printf( "]" );
                in_string = false;
                printf( "[" );
            }

            print_int_64( t + offset );
            offset += sizeof( int64 );
            printf( "]" );
        }
        else if ( td->get_schema_field(i)->get_type() == DataType::SINGLE )
        {   if ( in_string )
            {   printf("]");
                in_string = false;
                printf("[");
            }

            print_single( t + offset );
            offset += sizeof( single );
            printf( "]" );
        }
        else if ( td->get_schema_field(i)->get_type() == DataType::DOUBLE )
        {   if ( in_string )
            {   printf("]");
                in_string = false;
                printf("[");
            }

            print_double( t + offset );
            offset += sizeof( double );
            printf( "]" );
        }
        else if ( td->get_schema_field(i)->get_type() == DataType::STRING )
        {   in_string = true;

            for ( unsigned int j = 0; j < td->get_schema_field(i)->get_size(); j++ )
            {   print_char( t + offset );
                offset += sizeof(char);
            }
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::TIMESTAMP )
        {   if ( in_string )
            {
                printf("]" );
                in_string = false;
                printf( "[" );
            }

            print_timestamp( t + offset );
            offset += sizeof( timeval );
            printf( "]" );
        }
        else
        {   cout << "generateTuple: Unexpected type ["
                 << td->get_schema_field(i)->get_type() << "]!";

            abort();
        }
    }

    if ( in_string ) printf( "]" );
    printf("\n");
}


void TupleGenerator::print_tuple( const char  *t,
                                 const char  *format )
{
    bool in_string = false;
    printf( "Tuple [%s]: ", format );
    int offset = 0;

    for ( uint32  i = 0; format[i] != '\0'; i++ )
    {
        if ( !in_string ) printf( "[" );

        if ( format[ i ] == 'i')
        {   if ( in_string )
            {   printf( "]" );
                in_string = false;
                printf( "[" );
            }

            print_int_32( t + offset ); offset += sizeof( int32 );
            printf("]");
        }
        else if ( format[ i ] == 'L')
        {   if ( in_string )
            {   printf( "]" );
                in_string = false;
                printf( "[" );
            }

            print_int_64( t + offset ); offset += sizeof( int64 );
            printf( "]" );
        }
        else if ( format[ i ] == '1' )
        {   if ( in_string )
            {   printf( "]" );
                in_string = false;
                printf( "[" );
            }

            print_single( t + offset );
            offset += sizeof( single );
            printf( "]" );
        }
        else if ( format[ i ] == '2' )
        {   if ( in_string )
            {   printf( "]" );
                in_string = false;
                printf( "[" );
            }

            print_double( t + offset );
            offset += sizeof( double );
            printf( "]" );
        }
        else if ( format[ i ] == 'c' )
        {   print_char( t + offset );
            offset += sizeof( char ); in_string = true;
        }
        else if ( format[ i ] == 't' )
        {   if ( in_string )
            {   printf( "]" );
                in_string = false;
                printf( "[" );
            }

            print_timestamp( t + offset );
            offset += sizeof( timeval );
            printf( "]" );
        }
        else
        {   printf( "generateTuple: Unexpected format character [%c]!\n",
            format[ i ]);

            abort();
        }
    }

    if ( in_string ) printf( "]" );
    printf( "\n" );
}

// This version will generate a tuple to fit the description passed
char* TupleGenerator::generate_tuple( TupleDescription  *td )
{
    int memsize = 0;

    // Find out the memory we need (you could do td->getSize() you know?)
    // NOTE: TupleDescription does NOT INCLUDE: Timestamp (timeval) and sid (int)
    memsize += sizeof( timeval ) + sizeof( int32 );

    for ( uint32  i = 0; i < td->get_num_fields(); i++ )
    {   if ( td->get_schema_field( i )->get_type() == DataType::INT )
        {   memsize += sizeof( int32 );
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::LONG )
        {   memsize += sizeof( int64 );
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::SINGLE )
        {   memsize += sizeof( single );
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::DOUBLE )
        {   memsize += sizeof( double );
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::STRING )
        {   memsize += sizeof( char ) * td->get_schema_field( i )->get_size();
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::TIMESTAMP )
        {   memsize += sizeof( timeval );
        }
        else
        {   cout << "generateTuple: Unexpected type ["
                 << td->get_schema_field( i )->get_type() << "]!";

            abort();
        }

    }

    // Now get memory
    char* t = (char*) malloc(memsize);

    // Now actually create the stuff inside
    int offset = 0;

    // Timestamp
    fill_with_timestamp( t + offset );
    offset += sizeof( timeval );

    // Sid - hard code to 1
    *((int*)(t+offset)) = 1;

    // The fields now
    offset += sizeof( int32 );

    for ( uint32 i = 0; i < td->get_num_fields(); i++ )
    {   if ( td->get_schema_field(i)->get_type() == DataType::INT )
        {   fill_with_int_32( t + offset );
            offset += sizeof( int32 );
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::LONG )
        {   fill_with_int_64( t + offset );
            offset += sizeof( int64 );
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::SINGLE )
        {   fill_with_single( t + offset );
            offset += sizeof( single );
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::DOUBLE )
        {   fill_with_double( t + offset );
            offset += sizeof( double );
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::STRING )
        {   for ( uint32  j = 0; j < td->get_schema_field( i )->get_size(); j++ )
            {   fill_with_char( t + offset );
                offset+= sizeof( char );
            }
        }
        else if ( td->get_schema_field( i )->get_type() == DataType::TIMESTAMP )
        {   fill_with_timestamp( t + offset );
            offset += sizeof( timeval );
        }
        else
        {   cout << "generateTuple: Unexpected type ["
                 << td->get_schema_field(i)->get_type() << "]!";

            abort();
        }
    }

    return( t );
}


BOREALIS_NAMESPACE_END

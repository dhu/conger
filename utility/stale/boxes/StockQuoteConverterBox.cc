#include "StockQuoteConverterBox.h"
#include "Catalog.h"

BOREALIS_NAMESPACE_BEGIN

AURORA_DEFINE_CONVERTER_BOX( StockQuoteConverterBox );

//const char *StockQuoteConverterBox::SCHEMA =
//    "<schema>"
//    "  <field name='symbol' type='string' size='8'/>"
//    "  <field name='value' type='single'/>"
//    "  <field name='time' type='int'/>"
//    "</schema>";

const char *StockQuoteConverterBox::REGEX =
    "^([A-Z0-9]+):([0-9.]+)@([0-9]+)$";

// This regex matches
//
//    a ticker symbol [A-Z0-9]+
//  + a colon
//  + a floating-point tick value [0-9.]+
//  + an at sign
//  + a time in seconds [0-9]+
//
// Note that the regex is permissive with respect to the tick value:
// it will accept things that are not numbers (like "11.3...5").
// These get weeded out by convertTupleFromRegex, below.

CatalogSchema   quote_schema;


//  Create a schema for a stock quote.
//
CatalogSchema  *set_quote_schema()
{
    SchemaField   field1( "symbol", DataType::STRING, 8 );
    SchemaField   field2( "value",  DataType::SINGLE, sizeof( single ) );
    SchemaField   field3( "time",   DataType::INT, sizeof(int32) );

   quote_schema._name = "";

   quote_schema._field.push_back( field1 );
   quote_schema._field.push_back( field2 );
   quote_schema._field.push_back( field3 );

   return( &quote_schema );
}

StockQuoteConverterBox::StockQuoteConverterBox() :
    RegexConverterBox( TupleDescription::TupleDescription( set_quote_schema() ),
                       '\n', REGEX )
{}


bool StockQuoteConverterBox::convert_tuple_from_regex( Subexpr   match,
                                      const vector<Subexpr>  &subexprs,
                                                       char  *out
                                                  )
                                             throw( AuroraException )
{
    // In-memory format of stock quote tuples (must match schema)
    struct stock_quote_tuple
    {
        char    header[12];
        char    symbol[8];
        single  value;
        int     time;
    };

    // Output buffer has structure described by StockQuoteTuple
    stock_quote_tuple *tuple = (stock_quote_tuple*)out;

    // Copy ticker symbol
    if (!subexprs[0].to_string_field( tuple->symbol ))
    {
        // Ticker symbol too long to copy!  Return false
        // to indicate that the tuple wasn't converted (we
        // could also throw an exception to shut down the stream)
        //
        return( false );
    }

    try
    {   // Convert tick value and time
        tuple->value = subexprs[1].to_single();
        tuple->time  = subexprs[2].to_int();
    }
    catch ( AuroraException  &e )
    {
        // Format conversion failed!  Alternatively, could simply
        // propagate the AuroraException to shut down the stream.
        //
        return( false );
    }

    return( true );
}

BOREALIS_NAMESPACE_END

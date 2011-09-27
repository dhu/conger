
#ifndef CONVERTER_BOX_H
#define CONVERTER_BOX_H

#include <NMSTL/io>

#include "TupleDescription.h"
#include "Stream.h"
#include "Registry.h"

BOREALIS_NAMESPACE_BEGIN;

/// Converts data received on a network port to tuples on a single
/// stream.
///
/// There are three ways to write a converter box to handle
/// custom-formatted data from the network.  In order of decreasing
/// generality but increasing ease of use:
///
/// <ul>
///
/// <li>To receive and process arbitrary bytes directly from the
/// network, create a subclass of ConverterBox and implement the
/// ConverterBox::handleData() method.  handleData() is called
/// whenever new data arrive on the network.
///
/// <li>To process delimited or fixed-size records, create a subclass
/// of SimpleConverterBox and implement the
/// SimpleConverterBox::convertTuple() method.  convertTuple() is
/// called whenever a single, complete input record is received.
///
/// <li>To process records which match POSIX extended regular
/// expressions, create a subclass of RegexConverterBox and implement
/// the RegexConverterBox::convertTupleFromRegex() method.
/// convertTupleFromRegex() is called whenever a single, complete
/// input record matching the regex is received; the method is
/// supplied the subexpressions matched by the regex.
///
/// </ul>
///
/// Include the
/// <code>AURORA_DECLARE_CONVERTER_BOX(<i>ClassName</i>)</code> macro
/// inside the class declaration for your converter box, and
/// <code>AURORA_DEFINE_CONVERTER_BOX(<i>ClassName</i>)</code> in the
/// definition of your box (<i>ClassName.C</i>).
///
/// Once you have written a converter box, you may add a line like the
/// following to your Aurora configuration file, to bind your
/// converter box to a particular network port and tuple stream:
///
/// <pre>
/// &lt;converter-box class='StockQuoteConverterBox'
///                port='30000'
///                stream='stock-quotes'/&gt;
/// </pre>
///
/// This listens for stock quotes on port 30000, and uses
/// <code>StockQuoteConverterBox</code> to convert input records to
/// tuples on the <code>stock-quotes</code> stream.

class ConverterBox
{
  protected:
    /// Create a converter box that generates tuples with a given
    /// schema.
    ConverterBox(TupleDescription schema);

  public:
    /// Destructor.
    virtual ~ConverterBox();

    /// Returns the schema describing tuples generated by this
    /// converter box.  (This is the schema provided to the
    /// constructor.)
    TupleDescription get_schema() const { return _schema; }

    /// Handles incoming data to be converted.  Must be implemented by
    /// the subclass; the subclass should examine the incoming bytes,
    /// and enqueue data as necessary on the stream returned by the
    /// getStream() method.
    ///
    /// @param input data to be handled by the converter box.
    /// @return the number of bytes consumed by the converter box.
    ///     Any remaining bytes will be buffered and presented
    ///     to the box again when more bytes arrive.
    virtual unsigned int handle_data(constbuf input)
        throw (AuroraException) = 0;

    /// Sets the stream that will receive data generated by this
    /// converter box.
    void set_stream(ptr<Stream> stream);

    /// Returns the stream that will receive data generated by
    /// this converter box.
    ptr<Stream> get_stream() { return _stream; }

  private:
    TupleDescription _schema;
    ptr<Stream> _stream;
};

#define AURORA_DECLARE_CONVERTER_BOX(ClassName) AURORA_DECLARE_REG_CLASS(ConverterBox, ClassName)
#define AURORA_DEFINE_CONVERTER_BOX(ClassName) AURORA_DEFINE_REG_CLASS(ConverterBox, ClassName)

BOREALIS_NAMESPACE_END;

#endif

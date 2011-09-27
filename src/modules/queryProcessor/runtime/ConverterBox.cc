
#include "ConverterBox.h"

BOREALIS_NAMESPACE_BEGIN;

ConverterBox::ConverterBox(TupleDescription schema) :
    _schema(schema)
{
}

ConverterBox::~ConverterBox()
{
}

void ConverterBox::set_stream(ptr<Stream> stream)
{
    _stream = stream;
    ASSERT(_stream->get_tuple_description().get_size() == _schema.get_size());
}

BOREALIS_NAMESPACE_END;

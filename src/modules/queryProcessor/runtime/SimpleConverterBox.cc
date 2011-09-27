
#include "SimpleConverterBox.h"

BOREALIS_NAMESPACE_BEGIN;

SimpleConverterBox::SimpleConverterBox(TupleDescription schema, char delim) :
    ConverterBox(schema),
    _delim(delim),
    _length(0),
    _output((char*)malloc(schema.get_size()))
{
}

SimpleConverterBox::SimpleConverterBox(TupleDescription schema, unsigned int length) :
    ConverterBox(schema),
    _delim(0),
    _length(length),
    _output((char*)malloc(schema.get_size()))
{
}

SimpleConverterBox::~SimpleConverterBox()
{
    free(_output);
}

unsigned int SimpleConverterBox::handle_data(constbuf input)
    throw (AuroraException)
{
    const char *p = input.begin();
    const char *end = input.end();
    Stream::EnqIterator enq = get_stream()->enq_iterator();
    int tuple_size = get_schema().get_size();
    bool have_any = false;

    DEBUG << "SimpleConverterBox: processing \"" << to_escaped_string(input) << "\"";

    if (_length)
    {
        // Fixed-size chunks

        while (p + _length < end)
        {
            DEBUG << " - one fixed-length tuple: \"" << to_escaped_string(p, _length) << "\"";

            bzero(enq.tuple(), tuple_size);
            if (convert_tuple(p, _length, (char*)enq.tuple()))
            {
                DEBUG << "   - converted to " << get_schema().tuple_to_string(enq.tuple());
                ++enq;
                have_any = true;
            } else
                DEBUG << "   - not handled";

            p += _length;
        }
    } else
    {
        // Delimited chunks

        while (1)
        {
            const char *next = find(p, end, _delim);
            if (next == end)
                break;

            DEBUG << " - one delimited tuple: \"" << to_escaped_string(p, next - p) << "\"";

            unsigned int length = next - p;
            if (_delim == '\n' && length > 0 && *(next - 1) == '\r')
                --length;

            bzero(enq.tuple(), tuple_size);
            if (convert_tuple(p, length, (char*)enq.tuple()))
            {
                DEBUG << "   - converted to " << get_schema().tuple_to_string(enq.tuple());
                ++enq;
                have_any = true;
            } else
                DEBUG << "   - not handled";

            p = next + 1;
        }
    }

    if (have_any)
        get_stream()->notify_enq();

    return p - input.begin();
}

BOREALIS_NAMESPACE_END;

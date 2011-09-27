#include "BoxPackage.h"

BOREALIS_NAMESPACE_BEGIN;

// return the number of serialized tuples for a stream.
unsigned int AbstractQueueState::number_of_tuples(string stream_name, unsigned int tuple_size)
{
    ptr<ISerialString> iss = _batch_inputs[stream_name];

    if ( ! iss )
    iss = _batch_inputs[stream_name] = ptr<ISerialString>(new ISerialString(_serialized_queues[stream_name], ISerial::binary));

    return iss->remainder().size() / (tuple_size + TUPLE_SERIAL_HEADER);
}

// return whether we successfully got a tuple.
bool AbstractQueueState::get_tuple_from_queue(char* dest, string stream_name, unsigned int tuple_size)
{
    ptr<ISerialString> iss = _batch_inputs[stream_name];

    if ( ! iss )
    iss = _batch_inputs[stream_name] = ptr<ISerialString>(new ISerialString(_serialized_queues[stream_name], ISerial::binary));

    if ( iss->remainder().size() < (tuple_size + TUPLE_SERIAL_HEADER))
    return false;

    string tuple(tuple_size, ' ');
    *iss >> tuple;

    memcpy(dest, tuple.c_str(), tuple_size);
    return true;
}

// return whether we succesfully got the requested number of tuples.
bool
AbstractQueueState::get_tuple_batch_from_queue(char* dest, string stream_name,
                       unsigned int tuple_size, unsigned int num_tuples)
{
    ptr<ISerialString> iss = _batch_inputs[stream_name];

    if ( ! iss )
    iss = _batch_inputs[stream_name] = ptr<ISerialString>(new ISerialString(_serialized_queues[stream_name], ISerial::binary));

    if ( iss->remainder().size() < num_tuples * (tuple_size + TUPLE_SERIAL_HEADER) )
    return false;

    string tuple(tuple_size, ' ');
    char* tuple_dest = dest;
    for(unsigned int i = 0; i < num_tuples; i++)
    {
    *iss >> tuple;
    memcpy(tuple_dest, tuple.data(), tuple_size);
    tuple_dest += tuple_size;
    }

    return true;
}

// return whether we successfully got a tuple.
bool AbstractQueueState::getTuple(char* dest, string& serialized_queue, unsigned int tuple_size)
{
    if (serialized_queue.size() < tuple_size)
    return false;

    string tuple(tuple_size, ' ');
    ISerialString iss(serialized_queue);

    iss >> tuple;
    memcpy(dest, tuple.c_str(), tuple_size);
    return true;
}

BOREALIS_NAMESPACE_END;

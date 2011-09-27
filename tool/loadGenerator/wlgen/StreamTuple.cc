#include "StreamTuple.h"
#include <cassert>


BOREALIS_NAMESPACE_BEGIN

StreamTuple::StreamTuple()
{
}

StreamTuple::StreamTuple(
						 AggStreamID        agg_stream_id,
						 const Timeval&     timestamp,
						 int            data_size):
_agg_stream_id(agg_stream_id),
_timestamp(timestamp),
_data_len(data_size)
{
	assert(data_size > 0);
	_data = new char[_data_len];
}

StreamTuple::~StreamTuple()
{
	delete[] _data;
}

AggStreamID& StreamTuple::getAggStreamID()
{
	return _agg_stream_id;
}

Timeval& StreamTuple::getTimestamp()
{
	return _timestamp;
}

void StreamTuple::changeTimestamp(Timeval newTimestamp)
{

    _timestamp = newTimestamp;
}


int StreamTuple::getDataSize() const
{
	return _data_len;
}

char* StreamTuple::getDataHandle() const
{
	return _data;
}


BOREALIS_NAMESPACE_END

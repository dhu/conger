#include "SingleSource.h"

BOREALIS_NAMESPACE_BEGIN

SingleSource::SingleSource(
						   source_stream_id_t    id,
						   agg_stream_id_t       add_stream_id,
						   const distribution_t& timstamp_generator_distribution,
						   single_sources_t      single_sources,
						   int           tuple_num):
_stream_id(id),
_agg_stream_id(add_stream_id),
_timestamp_generator(timstamp_generator_distribution),
_single_sources(single_sources),
_tuple_data_size(0),
_tuple_num(tuple_num),
_generated_tuple_num(0)
{
	for (single_sources_it_t it = _single_sources.begin();
		it != _single_sources.end();
		++it)
	{
		_tuple_data_size += (*it).getDataSize();
		(*it).initialize(_timestamp_generator.getTimestamp());
	}
};
SingleSource::~SingleSource()
{
}

StreamTuple* SingleSource::getNextTuple()
{
	//create a new tuple
	StreamTuple* new_tuple =
		new StreamTuple(
		_agg_stream_id,
		_timestamp_generator.getTimestamp(),
		_tuple_data_size);
	char* data = new_tuple->getDataHandle();

	//set data
	for (single_sources_it_t it = _single_sources.begin();
		it != _single_sources.end();
		++it)
	{
		int len = (*it).getNextData(_timestamp_generator.getTimestamp(), data);
		data += len;
	}

	//go to next timestamp
	_timestamp_generator.tick();

	//record number of tuples generated so far
	++_generated_tuple_num;

	return new_tuple;
}

void SingleSource::getTuplesUntil(const Timeval& tm, tuplelist_t &tuples)
{
	while (_timestamp_generator.getTimestamp() < tm &&
		((_generated_tuple_num < _tuple_num) ||
		(_tuple_num < 0)))
	{
		tuples.push_back(getNextTuple());
	}
}

void SingleSource::changeRate(double ratio)
{
	_timestamp_generator.changeRatio(ratio);
}

void SingleSource::changeTimestampGenerator(
	const distribution_t& timstamp_generator_distribution)
{
	_timestamp_generator.changeIntervalGenerator(
		timstamp_generator_distribution);
}

BOREALIS_NAMESPACE_END

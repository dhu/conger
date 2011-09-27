#include "TimeStampGenerator.h"

#include <sys/time.h>           // for gettimeofday
#include <common.h>

BOREALIS_NAMESPACE_BEGIN

TimeStampGenerator::TimeStampGenerator(
									   const distribution_t& random_number_generator_par):
_ratio(1)
{
	_interval_generator = new RandomNumGenerator(random_number_generator_par);
	gettimeofday(&_timestamp,0);
}

TimeStampGenerator::~TimeStampGenerator()
{
	delete _interval_generator;
}

Timeval& TimeStampGenerator::getTimestamp()
{
	return _timestamp;
}

void TimeStampGenerator::setTimeStamp(const timeval &t)
{
	_timestamp = t;
}

void TimeStampGenerator::tick()
{
	double interval;
	interval = _interval_generator->getRandNum();
	if (interval <= 0)
		interval = MIN_INTERVAL;
	interval /= _ratio;
	_timestamp += Timeval(interval);
}

void TimeStampGenerator::changeRatio(double ratio)
{
	_ratio = ratio;
}

void TimeStampGenerator::changeIntervalGenerator(
	const distribution_t& random_number_generator_par)
{
	delete _interval_generator;
	_interval_generator = new RandomNumGenerator(random_number_generator_par);
}

BOREALIS_NAMESPACE_END

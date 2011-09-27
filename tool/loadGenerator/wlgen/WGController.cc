#include "WGController.h"

// C++ headers
#include <ctime>            // for timeval

// Unix headers
#include <unistd.h>         // for sleep, usleep
#include <sys/time.h>           // for gettimeofday

// Local headers
#include "SingleSource.h"       // for SingleSource

BOREALIS_NAMESPACE_BEGIN

WGController::WGController():
_check_ahead_interval(
					  DEFAULT_CHECK_AHEAD_INTERVAL_SEC,
					  DEFAULT_CHECK_AHEAD_INTERVAL_USEC)
{
}

WGController::WGController(long check_ahead_sec, long check_ahead_usec):
_check_ahead_interval(check_ahead_sec, check_ahead_usec)
{
}

void WGController::initialize(
							  SourcePool* source_pool,
							  StreamBuffer* stream_buffer)
{
	_source_pool = source_pool;
	_stream_buffer = stream_buffer;

	if (pthread_create(&_generator_thread,
		0,
		(void*(*)(void*))generatorHandler,
		(void*)this) < 0)
		perror("WGController: creating generator thread");
}

Timeval& WGController::getLastCheckTime()
{
	return _last_check_time;
}

void WGController::setCheckInterval(const Timeval& check_interval)
{
	_check_ahead_interval = check_interval;
}

void WGController::generatorHandler(WGController* me)
{

	while(1)
	{
		// scan stream source pool, generate tuples
		gettimeofday(&me->_last_check_time, 0);
		me->_last_check_time += me->_check_ahead_interval;

		tuplelist_t* all_tuples = new tuplelist_t;
		me->_source_pool->lock();
		for (me->_source_pool->begin();
			!me->_source_pool->end();
			me->_source_pool->step())
		{
			SingleSource& src = me->_source_pool->getSource();
			tuplelist_t tmp_tuples;
			src.getTuplesUntil(me->_last_check_time, tmp_tuples);
			all_tuples->merge(tmp_tuples);
			if (!src.active())
				me->_inactive_sources.push_back(src.getID());
		}
		me->_source_pool->unlock();

		// add generated data to buffer
		me->_stream_buffer->addNewTuples(all_tuples);

		// cleanup inactive sources
		if (me->_inactive_sources.size())
			me->_source_pool->delSources(me->_inactive_sources);

		// wait
		sleep((int)(ceil((double)(me->_check_ahead_interval.tv_sec)/2.0)));
		usleep(me->_check_ahead_interval.tv_usec);
	}
}

//temp
bool test(StreamBuffer *stream_buffer)
{return true;}


BOREALIS_NAMESPACE_END

#ifndef WGController_H
#define WGController_H


#include <pthread.h>            // for pthread*

#include "StreamTuple.h"        // for StreamTuple, tuplelist_t
#include "SourcePool.h"         // for SourcePool, sources_t
//     sources_iter_t
#include "StreamBuffer.h"       // for StreamBuffer
#include "Timeval.h"            // for Timeval
#include <common.h>


BOREALIS_NAMESPACE_BEGIN

const long DEFAULT_CHECK_AHEAD_INTERVAL_SEC = 0;
const long DEFAULT_CHECK_AHEAD_INTERVAL_USEC = 10000;

/**Workload Generator Controller
*/
class WGController
{
public:
	/**Construct the WGController
	*/
	WGController();

	/**Construct the WGController with check ahead time specificaiton. The controller always generate tuples until the next check ahead time.
	*/
	WGController(long check_ahead_sec, long check_ahead_usec);

	/**Initialize the controller with the source pool and stream buffer
	*/
	void initialize( SourcePool* pool, StreamBuffer* stream_buffer);

	/**Returns the last check time
	*/
	Timeval& getLastCheckTime();

	/**Set the check interval
	*/
	void     setCheckInterval(const Timeval& check_interval);

    //temp
    bool test(StreamBuffer *stream_buffer);
    
private:
	SourcePool*      _source_pool; /**< The source pool */
	StreamBuffer*    _stream_buffer; /**< The stream buffer */

	pthread_t        _generator_thread; 
	Timeval      _last_check_time;
	Timeval      _check_ahead_interval;
	tuplelist_t      _tmp_tuples;
	stream_ids_t     _inactive_sources;
	static void      generatorHandler(WGController* me);

};

BOREALIS_NAMESPACE_END

#endif

#ifndef SourcePool_H
#define SourcePool_H

#include <map>              // for map
#include <cstdio>           // for perror
#include <cstring>          // for strcmp

#include <pthread.h>            // for pthread_*
#include <common.h>

#include "SingleSource.h"       // for SingleSource

BOREALIS_NAMESPACE_BEGIN


typedef map<source_stream_id_t, SingleSource*>       source_pool_t;
typedef map<source_stream_id_t, SingleSource*>::iterator source_pool_iter_t;

typedef vector<SingleSource*>       sources_t;
typedef vector<SingleSource*>::iterator sources_iter_t;

/** This class implements a pool that contains all the stream sources
*/
class SourcePool
{
public:

	/** construct an empty pool
	*/
	SourcePool();

	/** Destructor
	*/
	~SourcePool(){};

	/** Lock the pool
	*/
	void            lock();


	/** UnLock the pool
	*/
	void            unlock();

	//--------------------------------------------------
	// Need to lock/unlock before/after using these functions

	/** Go to the beginning of the pool;
	*/
	void            begin();

	/** Is true if the end of the pool has been reached
	*/
	bool            end() const;

	/** Go to the next position
	*/
	void            step();

	/** Returns the source of in the current location
	*/
	SingleSource&   getSource() const;

	/** Returns all source with given AggStream ID
	* @param agg_id ID of the aggregate stream 
	* @param result_sources the returned sources
	* @return all souce with given AggStream ID
	*/
	void        getSources(
		const agg_stream_id_t agg_id,
		sources_t& result_sources);


	//--------------------------------------------------
	//Not need to lock.
	/** Add a new source
	* @param source the new source
	*/
	void     addSource(SingleSource* source);

	/** Add new sources
	* @param sources the new sources
	*/
	void     addSources(sources_t sources);

	/** Delete sources
	* @_sources the sources to be deleted
	*/
	void     delSources(stream_ids_t &_sources);

	/** Delete all sources belongs to an AggStream
	* @param agg_id ID of the aggregate stream to be deleted
	*/
	void     delAggSources(const agg_stream_id_t& agg_id);

	/** Delete all sources in the pool
	*/
	void     emptyPool();

	/** Change the rate of all sources in the pool
	* @param ratio ratio > 1 increase rate
	*/
	void     changeRate(double ratio);

private:

	source_pool_t      _pool; /**< The pool for all source */

	source_pool_iter_t _pos; /**< The current postion of the source */

	pthread_mutex_t    _mutex; /**< mutex to protest the pool */ 

	/** Delete a source
	* @param source_id ID of the source
	*/
	void            delSource(const source_stream_id_t source_id);
};

BOREALIS_NAMESPACE_END
#endif

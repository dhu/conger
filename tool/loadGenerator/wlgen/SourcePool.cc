#include "SourcePool.h"

#include <iostream>         // for cout

BOREALIS_NAMESPACE_BEGIN

SourcePool::SourcePool()
{
	if (pthread_mutex_init(&_mutex,0) < 0)
		perror("SourcePool pthread_mutex_init");
	_pos = _pool.begin();
}

void SourcePool::lock()
{
	if (pthread_mutex_lock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");
}
void SourcePool::unlock()
{
	if (pthread_mutex_unlock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");
}

void SourcePool::begin()
{
	_pos = _pool.begin();
}

bool SourcePool::end() const
{
	return _pos == _pool.end();
}

void SourcePool::step()
{
	++_pos;
}

SingleSource& SourcePool::getSource() const
{
	return *(_pos->second);
}

void SourcePool::addSource(SingleSource* source)
{
	if (pthread_mutex_lock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");
	_pool[source->getID()] = source;
	if (pthread_mutex_unlock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_unlock");
}

void SourcePool::addSources(sources_t sources)
{
	if (pthread_mutex_lock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");
	for (sources_iter_t iter = sources.begin();
		iter != sources.end();
		++iter)
	{
		_pool[(*iter)->getID()] = *iter;
	}
	if (pthread_mutex_unlock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_unlock");
}

void SourcePool::delSource(const source_stream_id_t source_id)
{
	if (pthread_mutex_lock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");
	source_pool_iter_t pos = _pool.find(source_id);
	delete pos->second;
	_pool.erase(pos);
	if (pthread_mutex_unlock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_unlock");
}

void SourcePool::delSources(stream_ids_t &sources)
{
	if (pthread_mutex_lock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");
	while (!sources.empty())
	{
		source_pool_iter_t pos = _pool.find(sources.back());
		if (pos != _pool.end())
		{
			delete pos->second;
			_pool.erase(pos);
		}
		sources.pop_back();
	}
	if (pthread_mutex_unlock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_unlock");
}

void SourcePool::delAggSources(const agg_stream_id_t& agg_id)
{
	bool found = false;
	if (pthread_mutex_lock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");
	source_pool_iter_t it = _pool.begin();
	while (it != _pool.end())
	{
		if (((it->second)->getAggStreamID().numberID == agg_id.numberID) ||
			!strcmp((it->second)->getAggStreamID().stringID, agg_id.stringID))
		{
			source_pool_iter_t pos = it;
			++it;
			delete pos->second;
			_pool.erase(pos);
			found = true;
		}
		else
			++it;
	}

	if (pthread_mutex_unlock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_unlock");

	if (found)
		NOTICE << "Aggregate stream source: "
		<< agg_id.stringID
		<< "(" << agg_id.numberID <<") "
		<< "has been deleted.";
	else
		NOTICE << "Can not find aggregate stream source: "
		<< agg_id.stringID
		<< "(" << agg_id.numberID <<") ";

}

void SourcePool::getSources(
							const agg_stream_id_t agg_id,
							sources_t& result_sources)
{
	for(source_pool_iter_t it = _pool.begin();
		it != _pool.end();
		++it)
	{
		if (((it->second)->getAggStreamID().numberID == agg_id.numberID) ||
			!strcmp((it->second)->getAggStreamID().stringID, agg_id.stringID))
		{
			result_sources.push_back(it->second);
		}
	}
}

void SourcePool::emptyPool()
{
	if (pthread_mutex_lock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");
	source_pool_iter_t it = _pool.begin();
	while (it != _pool.end())
	{
		source_pool_iter_t pos = it;
		++it;
		delete pos->second;
		_pool.erase(pos);
	}

	if (pthread_mutex_unlock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_unlock");
}

void SourcePool::changeRate(double ratio)
{
	if (ratio <= 0)
	{
		NOTICE << "SourcePool: wrong parameter to change rate:"
			<< ratio;
		return;
	}

	if (pthread_mutex_lock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_lock");

	for (source_pool_iter_t iter = _pool.begin();
		iter != _pool.end();
		++iter)
	{
		(iter->second)->changeRate(ratio);
	}

	if (pthread_mutex_unlock(&_mutex) < 0)
		perror("SourcePool pthread_mutex_unlock");

}

BOREALIS_NAMESPACE_END

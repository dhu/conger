#ifndef StreamIDGenerator_H
#define StreamIDGenerator_H

// Local header
#include "Types.h"      // for source_stream_id_t
#include <common.h>

BOREALIS_NAMESPACE_BEGIN


class StreamIDGenerator
{
public:
	static source_stream_id_t getID(
		source_stream_id_t agg_stream_id,
		int count)
	{
		source_stream_id_t id = agg_stream_id << 16;
		id += count;
		return id;
	}
};

BOREALIS_NAMESPACE_END
#endif

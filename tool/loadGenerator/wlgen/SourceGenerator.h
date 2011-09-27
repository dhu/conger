#ifndef SourceGenerator_H
#define SourceGenerator_H

#include "SourcePool.h"         // for SourcePool, sources_t
#include "AggStreamSource.h"        // for AggStreamSource
#include "Types.h"          // for {source_stream_id_t, parlist_t distribution_t}
#include "StreamIDGenerator.h"          // for StreamIDGenerator
#include "RandomNumGenerator.h"     // for RandomNumGenerator
#include <common.h>

BOREALIS_NAMESPACE_BEGIN

typedef vector<RandomNumGenerator> rand_num_generator_vect_t; /**Random number generator (vector) */

/** The source generator generate new sources and add them to the source pool
*/
class SourceGenerator
{
public:

	/** Initialie the SourceGenerator with source pool
	* @param source_pool the source pool
	*/
	void initialize(SourcePool* source_pool);

	/** Generate new sources according to the agg_source specification
	* @param agg_source the aggregate stream
	*/
	bool newSources(AggStreamSource& agg_source);

	/** Change the source rate
	* @param agg_source the aggegate stream
	*/
	bool changeSourceRate(AggStreamSource& agg_source);

private:
	SourcePool  *_source_pool;/**< the source pool */


	/**generate a new source
	* @param attrdef attribute definition
	* @param randnum_generators random number generator
	* @param single_sources single sources
	*/
	bool newSingleSource(
		AttributeDef                &attrdef,
		vector<RandomNumGenerator>  &randnum_generators,
		single_sources_t            &single_sources);
};

BOREALIS_NAMESPACE_END
#endif

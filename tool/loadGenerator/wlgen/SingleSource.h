#ifndef SingleSource_H
#define SingleSource_H

// C++ headers
#include <vector>               // for vector

// Local headers
#include "StreamTuple.h"        // for StreamTuple, tuplelist
#include "TimeStampGenerator.h" // for TimeStampGenerator
#include "AttributeSource.h"    // for AttributeSource
#include "Timeval.h"            // for Timeval

#include <common.h>

BOREALIS_NAMESPACE_BEGIN

using namespace time_tools;


typedef vector<AttributeSource>     single_sources_t;
typedef vector<AttributeSource>::iterator   single_sources_it_t;

/** Single Stream Source
* The definition of the Single stream.
*/
class SingleSource
{
public:
	/**Default Constructor
	*/
	SingleSource(){}


	/**Construct a stream source with parameters
	* @param id ID of this source
	* @param agg_stream_id ID of the AggStream this source belongs to
	* @param timestamp_generator_distribution the distribution used to generate the timestamps (inter arrival time)
	* @param single_sources attribute single sources
	* @param tuple_num number of tuples to generate
	*/
	SingleSource(
		source_stream_id_t     id,
		agg_stream_id_t        agg_stream_id,
		const distribution_t&      timestamp_generator_distribution,
		single_sources_t       single_sources,
		int                tuple_num);

	/**Destructor
	*/
	~SingleSource();

	/**Get the ID of this stream source
	* @return the ID of this stream source
	*/
	int          getID() const { return _stream_id; }

	/**Get the ID of the AggStream
	* @return the ID of the AggStream
	*/
	agg_stream_id_t& getAggStreamID() {return _agg_stream_id;}

	/**Get the number of the tuples to generate
	* @return the number of tuples to generate
	*/
	int          getTupleNumber() const {return _tuple_num; }

	/**Get the tuple data size (all attributes)
	* @return the tuple data size
	*/
	int          getTupleDataSize() const { return _tuple_data_size;}

	/**Generate a new tuple
	*/
	StreamTuple* getNextTuple();

	/**Generate tuples until a given time
	* @param tm timestamp
	* @param tuple the generated tuple
	*/
	void         getTuplesUntil(const Timeval& tm, tuplelist_t &tuples);

	/**Generate "tuple_num" number of tuples and returns them in "tuples"
	* @param tuple_num the number of tuples
	* @tuples the generated tuples
	*/
	void         getNTuples(const int tuple_num, tuplelist_t &tuples);

	/**Is true if the source still need to generate tuples
	*/
	bool active() const {return _generated_tuple_num != _tuple_num;}

	/**Change the rate of the source
	* @param ratio the rate of the source
	*/
	void changeRate(double ratio);

	/**Change the timestamp generator of this tuple
	* @param timestamp_generator_distribution the new timestamp generator distribution
	*/
	void changeTimestampGenerator(
		const distribution_t& timstamp_generator_distribution);

private:

	source_stream_id_t   _stream_id; /**< this is for individual source */
	agg_stream_id_t  _agg_stream_id; /**< the ID of the aggStream this individual stream belongs to */

	TimeStampGenerator   _timestamp_generator; /**< timstamp_generator that generator the timestamps of the tuple */

	single_sources_t _single_sources; /**< the sources for the attributes */
	int          _tuple_data_size; /**< the size of the tuple */
	int          _tuple_num; /**< the number of the tuple */
	int          _generated_tuple_num; /**< the generated number of the tuple */
};

BOREALIS_NAMESPACE_END
#endif

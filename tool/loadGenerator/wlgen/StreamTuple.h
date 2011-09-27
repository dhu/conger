#ifndef StreamTuple_H
#define StreamTuple_H


#include "Types.h"          // for agg_stream_id
#include "Timeval.h"        //for Timeval
#include <common.h>


BOREALIS_NAMESPACE_BEGIN

using namespace time_tools;
class StreamTuple;
typedef list<StreamTuple*> tuplelist_t;
typedef list<StreamTuple*>::iterator tuplelist_iter_t;

/** Stream Tuple
*/
class StreamTuple
{
public:

    StreamTuple();
    
    /**Construct a tuple with given ID timestamp and data_size
	* @param agg_stream_id ID of the aggregate stream
	* @param timestamp time stamp
	* @param data_size the size of the data
	*/
	StreamTuple(
		AggStreamID        agg_stream_id,
		const Timeval&     timestamp,
		int            data_size);
	/**Destructor
	*/
	~StreamTuple();

	/**Returns the ID for the AggStream
	*/
	AggStreamID&     getAggStreamID();

	/**Returns the timestamp
	*/
	Timeval&     getTimestamp();

    /**Change the timestamp
     */
    void    changeTimestamp(Timeval newTimestamp);

    
	/**Returns the size of the data
	*/
	int          getDataSize() const;

	/**Returns the handle of the data
	*/
	char*        getDataHandle() const;


	/**Comparison operator: compare tuples by their timestamps
	*/
	bool operator<(StreamTuple &t)
	{
		return _timestamp < t.getTimestamp();
	}

private:

	AggStreamID      _agg_stream_id; /**< ID of the stream */

	Timeval              _timestamp; /**< Time stamp */

	char                 *_data; /**< data buffer */

	int                  _data_len; /**< buffer size */
};


BOREALIS_NAMESPACE_END
#endif

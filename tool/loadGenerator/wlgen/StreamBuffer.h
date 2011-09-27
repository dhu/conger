#ifndef StreamBuffer_H
#define StreamBuffer_H

#include <pthread.h>            // for pthread_*
#include <common.h>

#include "StreamSender.h"       // for StreamSender
#include "StreamTuple.h"        // for tuplelist_t
#include "eventqueue.h"         // for EventQueue

BOREALIS_NAMESPACE_BEGIN

/** This class implements a buffer for all the tuples generated
*/
class StreamBuffer
{
public:
	/** Construct an empty buffer
	*/
	StreamBuffer(){}

	/** Delete the buffer
	*/
	~StreamBuffer(){}

	/** Initialize the buffer with the consumer for this buffer
	* @param sender the StreamSender
	*/
	void     initialize(StreamSender* sender);

	/** Add new tuples
	*/
	void     addNewTuples(tuplelist_t*);

    /** Add a exist tuple
     */
    void    addExistTuple(StreamTuple *stream_tuple);
    
	/** For debug
	*/
	void     printQueue();

private:

	threadtools::EventQueue     _buffer; /**< A event queue */
	StreamSender                *_sender; /**< The sender that sends tuples out */
};

BOREALIS_NAMESPACE_END
#endif

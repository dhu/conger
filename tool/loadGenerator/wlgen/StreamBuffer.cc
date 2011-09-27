#include "StreamBuffer.h"
#include "TuplesSave.h"

// C++ headers
#include <iostream>         // for cout


BOREALIS_NAMESPACE_BEGIN

void StreamBuffer::initialize(StreamSender  *sender)
{
	_sender = sender;
}

void StreamBuffer::addNewTuples(tuplelist_t    *tuple_list)
{
	if (tuple_list->size() == 0)
		return;
	send_item_t *item = new send_item_t(tuple_list);

    //save new tuples
    TuplesSave *tuples_save = new TuplesSave();
    tuples_save->saveTuples(item);

    //send new tuples
    _buffer.AddEvent(
		string("Send tuple"),
		tuple_list->back()->getTimestamp(),
		reinterpret_cast<threadtools::eventfunc_t>(_sender->sendTuples),
		item,
		0);
}


void StreamBuffer::addExistTuple(StreamTuple *stream_tuple)
{

    //send a exist tuple
    _buffer.AddEvent(
		string("Send tuple"),
		stream_tuple->getTimestamp(),
		reinterpret_cast<threadtools::eventfunc_t>(_sender->sendATuple),
		stream_tuple,
		0);


}

void StreamBuffer::printQueue()
{
	_buffer.DumpEventQueue(cout);
}

BOREALIS_NAMESPACE_END


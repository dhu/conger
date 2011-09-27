#ifndef AURORA_QBOXSTATE_H
#define AURORA_QBOXSTATE_H

#include <vector>
#include <map>
#include <NMSTL/serial>
#include <NMSTL/iserial>
#include <NMSTL/oserial>

#include "Stream.h"
#include "TupleQueue.h"
#include "BoxPackage.h"

BOREALIS_NAMESPACE_BEGIN;

class QueueState : public AbstractQueueState
{
 public:
    QueueState(){}

    QueueState(const QueueState& qs) { _serialized_queues = qs._serialized_queues; }

    QueueState(vector<ptr<TupleQueue> > queues)
    {
    for(vector<ptr<TupleQueue> >::iterator i = queues.begin(); i != queues.end(); i++)
    {
        Stream* stream = i->get()->get_stream();
            // XXX Because we use this for checkpoints, we need to use the seek iterator instead of the dequeue iterator
        //TupleQueue::DeqIterator dq = i->get()->deq_iterator();
            TupleQueue::SeekIterator dq = i->get()->seek_iterator();

        unsigned int tuple_size = i->get()->get_tuple_size();
        unsigned int packed_tuples = 0;

        DEBUG << "Queue for stream " << stream->get_name() << " has "
          << i->get()->size() << " tuples "
          << " of size " << i->get()->get_tuple_size();

        OSerialString serial_queue(OSerial::binary|OSerial::nosignature);

        while(dq.avail())
        {
        const void* data = dq.tuple();
        string tuple((const char*) data, tuple_size);
        serial_queue << tuple;
        ++dq;
        ++packed_tuples;
        }

        _serialized_queues[stream->get_name()] = serial_queue.str();

        DEBUG << "Packed " << packed_tuples << " tuples for stream: " << stream->get_name();
    }
    }

    ~QueueState() {}

    NMSTL_SERIAL_SUBCLASS(QueueState, AbstractQueueState, );
};

BOREALIS_NAMESPACE_END;

#endif

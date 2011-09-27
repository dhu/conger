#ifndef QUEUE_TRIMMING_MESSAGE_H
#define QUEUE_TRIMMING_MESSAGE_H

#include "StreamID.h"

BOREALIS_NAMESPACE_BEGIN;

class QueueTrimmingMessageEntry
{

  public:

    StreamID _id;
    string _last_tuple;

    QueueTrimmingMessageEntry() {}

    QueueTrimmingMessageEntry(StreamID id, string last_tuple):
        _id(id), _last_tuple(last_tuple) {}

    NMSTL_SIMPLY_SERIALIZABLE(QueueTrimmingMessageEntry, << _id << _last_tuple);

    string as_string() const
    {
        return string(_id.as_string());
    }

};


typedef vector<QueueTrimmingMessageEntry> QueueTrimmingMessageEntries;

class QueueTrimmingMessage
{

    public:

    MedusaID _sender;
    QueueTrimmingMessageEntries _entries;

    QueueTrimmingMessage()
    {
    }

    QueueTrimmingMessage(MedusaID sender):_sender(sender)
    {
    }

    void set_sender(MedusaID sender)
    {
        _sender = sender;
    }

    MedusaID get_sender()
    {
        return _sender;
    }

    void add(QueueTrimmingMessageEntry& entry)
    {
        _entries.push_back(entry);
    }

    string as_string() const;

    NMSTL_SIMPLY_SERIALIZABLE(QueueTrimmingMessage, << _sender << _entries);

};

BOREALIS_NAMESPACE_END;

#endif // QUEUE_TRIMMING_MESSAGE_H

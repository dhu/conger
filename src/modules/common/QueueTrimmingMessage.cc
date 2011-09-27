#include "QueueTrimmingMessage.h"

BOREALIS_NAMESPACE_BEGIN;

string QueueTrimmingMessage::as_string() const
{
    string s = _sender.as_string();
    s += " ";
    s += _entries.size();

//    for (QueueTrimmingMessageEntries::iterator i = _entries.begin(); i != _entries.end(); i++)
//    {   s += ", ";
//        s += (i->as_string());
//    }
    return s;
}


BOREALIS_NAMESPACE_END;

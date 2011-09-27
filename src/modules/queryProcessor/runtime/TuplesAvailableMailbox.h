#ifndef TUPLESAVAILABLEMAILBOX_H
#define TUPLESAVAILABLEMAILBOX_H

#include <set>
#include <pthread.h>

#include "common.h"

BOREALIS_NAMESPACE_BEGIN;

class TupleQueue;

typedef TupleQueue* tuple_queue_ptr;

/**
   This is a flag system that notifies an observer when a particular
   TupleQueue goes from non-empty to empty.

   The observer doesn't have to be looking at the time such a transition
   occurs - it's remembered until the observer picks up the notification.
 */
class TuplesAvailableMailbox
{
public:
    TuplesAvailableMailbox();
    virtual ~TuplesAvailableMailbox();

    // Call this when the specified TupleQueue has gone from being empty to
    // being non-empty.
    //
    // Don't call this any time after you've started calling close().
    void on_non_empty(TupleQueue * p_tq);

    // Call this method when this data structure is all done.  This will cause
    // any currently blocking call to getEvent() to unblock.
    void close();

    // If this returns true, then pTQ has been set to the address of a TupleQueue
    // that has become non-empty.  Concurrently pending non-empty reports form
    // the same TupleQueue will be reported just once to the caller.
    //
    // If this returns false, then close() has been called.
    //
    // This method blocks until an event exists to report or close() has been
    // called.
    bool get_event(tuple_queue_ptr & p_tq);

private:
    set<TupleQueue*> _unreceived_reports;
    bool _closed;
    pthread_cond_t _cond;
    pthread_mutex_t _mtx;
};

BOREALIS_NAMESPACE_END;

#endif

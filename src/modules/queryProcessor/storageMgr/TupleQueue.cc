
#include <TupleQueue.h>
#include <Stream.h>
#include <LockHolder.h>

#include <stdlib.h>
#include <malloc.h>
#include <sys/mman.h>
#include <errno.h>
#include <iostream>

BOREALIS_NAMESPACE_BEGIN;

TupleQueue::TupleQueue(PagePool &pool,
                       unsigned int tuple_size,
                       bool update_queue_option,
                       int queue_type,
                       string key_field,
                       string window_field) :
    _p_mailbox(NULL),
    _pool(pool), _tuple_size(tuple_size),
    _enq_count(0), _deq_count(0),
    _stream(0)
{
    _tuples_per_page = (pool.page_size() - sizeof(PageHdr)) / _tuple_size;
    assert(_tuples_per_page > 0);

    _enq_page = _deq_page = (PageHdr*)_pool.alloc();
    _enq_page->next = 0;

    _enq_location = _deq_location = (char *)_enq_page + sizeof(PageHdr);

    // Optionally, an UpdateQueue. -- tatbul
    //
    if (update_queue_option)
    {
        _update_queue = new UpdateQueue(*this, 
                                        queue_type, 
                                        key_field, 
                                        window_field);
    }
    else 
    {
        _update_queue = NULL;
    }
}

TupleQueue::~TupleQueue()
{
    PageHdr *next;
    for (PageHdr *page = _deq_page; page; page = next)
    {
        // Have to set next now (before free); free clobbers page data.
        //
        next = page->next;
        _pool.free(page);
    }
}

void TupleQueue::dump() const
{
    cerr << "TupleQueue {\n"
         << "    _tuple_size = " << _tuple_size << "\n"
         << "    _tuples_per_page = " << _tuples_per_page << "\n"
         << "    _enq_page = " << _enq_page << "\n"
         << "    _enq_location = " << _enq_location << "\n"
         << "    _deq_page = " << _deq_page << "\n"
         << "    _deq_location = " << _deq_location << "\n"
         << "    size() = " << size() << "\n"
         << "    pages = {\n";

    for (PageHdr *page = _deq_page; page; page = page->next)
    {
        cerr << "        " << (void*)page << " = \"";
        const char *data = (const char *)page;
        for (unsigned int i = 0; i < _pool.page_size(); ++i)
        {
            cerr << ((data[i] >= ' ' && data[i] <= '~') ? data[i] : '.');
        }
        cerr << "\"\n";
    }

    cerr << "    }\n"
         << "}"
         << endl;
}

void TupleQueue::set_enq_notify_target(TuplesAvailableMailbox * p_mailbox)
{
    _p_mailbox = p_mailbox;
}

void TupleQueue::notify_enq()
{
    // TODO: Smarten this up so that it will only actually call
    // onNonEmpty() if new tuples have been enqueued since either
    // (the notify target was set) or (the last time this method
    // was called).  This will reduce overhead of spurious notifications.
    // -cjc
    for (Listeners::const_iterator i = _listeners.begin(); 
         i != _listeners.end(); ++i)
    {
        DEBUG << " Notifying of the " << _listeners.size();
        (*i)->notify(*this);
    }

    if (_p_mailbox != NULL)
    {
        DEBUG << "notifyEnq: notifying mailbox";
        _p_mailbox->on_non_empty(this);
    }
}

TupleQueueListener::TupleQueueListener()
{
    // NOTICE << "Constructing TupleQueueListener at " << ((void*)this);
}

TupleQueueListener::~TupleQueueListener()
{
    // NOTICE << "Destroying TupleQueueListener at " << ((void*)this);
}

// Free up all the existing pages.
//
void TupleQueue::empty()
{
    DEBUG << "Emptying the TupleQueue";
    
    PageHdr *next;
    for (PageHdr *page = _deq_page; page; page = next)
    {
        // Have to set next now (before free); free clobbers page data.
        //
        next = page->next;
        _pool.free(page);
    }

    // Then reset all pointers, as they were when empty TupleQueue was created.
    //
    _enq_count = _deq_count = 0;
    _enq_page = _deq_page = (PageHdr*)_pool.alloc();
    _enq_page->next = 0;
    _enq_location = _deq_location = (char *)_enq_page + sizeof(PageHdr);
    DEBUG << "The size of tupleQueue now is " << size();
}

void TupleQueue::enqueue_multiple(vector<uint8*> tuples)
{
  DEBUG << "Enqueuing " << tuples.size() << " tuples";

  _lock.lock();  // lock first
  TupleQueue::EnqIterator e = enq_iterator();
  for (int unsigned i = 0; i < tuples.size(); i++)
    {
      memcpy(e.tuple(), tuples.at(i), _tuple_size);
      ++e;
    }
  _lock.unlock();  // all copied ... unlock

  notify_enq(); // the box on this queue can be scheduled
}

vector<uint8 *> TupleQueue::dequeue_all()
{
    vector<uint8 *> tuples(0);
    TupleQueue::DeqIterator d = deq_iterator();

    while(d.avail())
    {
        uint8 *tuple = new uint8[_tuple_size];
        memcpy(tuple, d.tuple(), _tuple_size);
        tuples.push_back(tuple);
        ++d;
    }

    return tuples;
}

size_t TupleQueue::dequeue_to_buffer(void *buffer, size_t buffer_size, 
                                     TupleDescription tuple_desc)
{
    TupleQueue::DeqIterator d = deq_iterator();
    size_t count = 0;

    size_t tuple_size = tuple_desc.get_size();
    size_t max_tuples = buffer_size/tuple_size;
    if (max_tuples == 0)
    {
        Throw(AuroraException, 
              "Buffer not big enough to dequeue even a single tuple");
    }

    while(count < max_tuples && d.avail())
    {
        memcpy(buffer, d.tuple(), tuple_size);
        buffer = ((char*)buffer) + tuple_size;
        ++d;
        ++count;
    }

    return count;
}

double TupleQueue::get_staleness()
{
    if (_last_timestamp == Timestamp())
        return 0;
    return (Timestamp::now() - _last_timestamp).to_msecs();
}

Timestamp TupleQueue::DeqIterator::get_timestamp(void* tuple_location)
{
    return Timestamp(*((timeval*)((char*)tuple_location + 0)));
}

BOREALIS_NAMESPACE_END;

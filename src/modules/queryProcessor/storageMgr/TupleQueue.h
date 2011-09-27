
#ifndef TUPLEQUEUE_H
#define TUPLEQUEUE_H

#include "PagePool.h"
#include "TuplesAvailableMailbox.h"
#include "TupleDescription.h"
#include <UpdateQueue.h>

BOREALIS_NAMESPACE_BEGIN;

class Stream;
class QBox;

/// A listener providing an abstract interface for a TupleQueue to be
/// notified when a TupleQueue becomes non-empty.
class TupleQueueListener
{
  public:
    /// Destructor.
    virtual ~TupleQueueListener();

    /// Invoked by a TupleQueue when its notifyEnq() method is called.
    virtual void notify(const TupleQueue& stream) = 0;

    QBox* get_box() { return _box_to_run; }
    void set_box(QBox* box) { _box_to_run = box; }

    void suspend() { _suspended = true; }
    void resume () { _suspended = false; }

  protected:
    TupleQueueListener();
    QBox* _box_to_run;
    bool _suspended;
};

/// A tuple queue with a single EnqIterator (for writing into the
/// queue) and a single DeqIterator (for reading from the queue).  The
/// iterators need not be used from the same thread.
class TupleQueue
{
  public:
    /// Constructor.  The pool's page size must be at least big enough
    /// to hold a tuple plus a small amount of accounting data.
    TupleQueue(PagePool& pool, unsigned int tuple_size, 
               bool update_queue_option = false, 
               int queue_type = 0,            // default is FIFO 
               string key_field = "",         // default is no-group
               string window_field = "time"); // default is time

    /// Destructor.
    ~TupleQueue();

    /// Associated stream (for bookkeeping).
    void set_stream(Stream *stream) 
    { 
        _stream = stream; 

        // If update queue, then set its key info. -- tatbul
        //
        if (_update_queue)
        {
            _update_queue->set_field_info(_stream);
        }
    }
    Stream *get_stream() { return _stream; }

    /*
      Added for Connection point support
    */
    // empties the tuple queue by freeing all pages and resetting pointers
    void empty();

    // puts the vector of tuples in the tuple queue
    void enqueue_multiple(vector<uint8*> tuples);

    // returns all the tuples in the tuple queue as a vector of tuples
    vector<uint8 *> dequeue_all();

    // copies tuples to the provided buffer till either no more tuples left 
    // on the queue or till buffer is full
    size_t dequeue_to_buffer(void *buffer, size_t buffer_size, 
                             TupleDescription _tuple_desc);

    /// An iterator used to write data into a TupleQueue.
    class EnqIterator
    {
      public:
        /// Dummy constructor.
        EnqIterator() : _q(0) {}

        /// Returns a pointer to a tuple-sized chunk of memory
        /// to which data may be written.
        void *tuple() const
        {
            assert(_q);
            return _q->_enq_location;
        }

        /// Indicates that the tuple to which this iterator points has
        /// been populated.
        EnqIterator& operator ++ ();

      private:
        explicit EnqIterator(TupleQueue &q) : _q(&q) {}

        /// Prohibit postfix increment; you can't advance the iterator
        /// until you've populated the tuple.
        EnqIterator operator ++ (int);

        TupleQueue *_q;

        friend class TupleQueue;
    };
    friend class EnqIterator;

    /// An iterator used to read data from a TupleQueue.
    class DeqIterator
    {
      public:
        /// Dummy constructor.
        DeqIterator() : _q(0) {}

        /// Returns true iff there is a valid tuple to dequeue.
        bool avail() const
        {
            assert(_q);
            return _q->_deq_location != _q->_enq_location;
        }

        /// Returns a pointer to a tuple to be consumed.  Valid
        /// only if avail().
        const void *tuple() const
        {
            assert(avail());
            
            // If update_queue is set, return UpdateQueue::_head.
            // -- tatbul
            //
            if (_q->_update_queue)
            {
                if (_q->_update_queue->get_type() == UpdateQueue::LINECUT_PRIOR)
                {
                    return _q->_update_queue->get_prior_location();
                }
                else if (_q->_update_queue->get_head_location())
                {
                    return _q->_update_queue->get_head_location();
                }
                else
                {
                    WARN << "No prior OR head !"; // This should never happen.
                }
            }

            return _q->_deq_location;
        }

        /// Consumes a tuple.
        DeqIterator& operator ++ ();

      private:
        explicit DeqIterator(TupleQueue &q) : _q(&q) {}

        /// Prohibit postfix increment.
        DeqIterator operator ++ (int);

        TupleQueue *_q;
        
        Timestamp get_timestamp(void* tuple_location);    // Jeong-Hyon: to figure out the staleness of the TupleQueue

        friend class TupleQueue;
    };
    friend class DeqIterator;

    class SeekIterator
    {
      public:
        /// Dummy constructor.
        SeekIterator() : _q(0) {}

        bool avail() const
        {
            assert(_q);
            return _seek_location != _q->_enq_location;
        }

        // Returns true if there is a valid tuple to dequeue.
        // in SeekIterator this would not work right. Not sure if I can remove
        // this function.
        //bool avail() const
        //{
        //    assert(_q);
        //    return true; //_q->_deq_location != _q->_enq_location;
        //}

        /// Returns a pointer to a tuple to be read.  Valid
        /// only if avail(). Does not check avail.
        const void *tuple() const
        {
            return _seek_location;
        }

        /// Reads a tuple.
        SeekIterator& operator ++ ();
        SeekIterator& operator -- ();

      private:
        explicit SeekIterator(TupleQueue &q) : _q(&q)
        {
            _seek_location = _q->_deq_location;
            _seek_page = _q->_deq_page;

            size_t page_size = _q->_pool.page_size();
            size_t tuple_size = _q->_tuple_size;
            int tuples_per_page = (page_size - sizeof(PageHdr)) / tuple_size;
            _offset_last_tuple_on_page = sizeof(PageHdr) + 
                                         tuple_size*(tuples_per_page-1);
        }

        /// Prohibit postfix increment.
        SeekIterator operator ++ (int);

        /// Prohibit postfix decrement.
        SeekIterator operator --(int);

        TupleQueue *_q;

        // this is where our seek iterator is pointing
        void *_seek_location;
        /// The page we're set to dequeue data from
        void *_seek_page;

        size_t _offset_last_tuple_on_page;
        friend class TupleQueue;
    };
    friend class SeekIterator;

    /**
     * Same as a seek iterator but in reverse order
     */
    class RSeekIterator
    {
      public:
        /// Dummy constructor.
        RSeekIterator() : _q(0) {}

        bool avail() const
        {
            assert(_q);
            return _seek_location != _q->_deq_location;
        }

        /// Returns a pointer to a tuple to be read.  Valid
        /// only if avail(). Does not check avail.
        const void *tuple() const
        {
            return _seek_location;
        }

        /// Reads a tuple.
        RSeekIterator& operator ++ ();
        RSeekIterator& operator -- ();

      private:
        explicit RSeekIterator(TupleQueue &q) : _q(&q)
        {
            size_t page_size = _q->_pool.page_size();
            size_t tuple_size = _q->_tuple_size;
            int tuples_per_page = (page_size - sizeof(PageHdr)) / tuple_size;
            _offset_last_tuple_on_page = sizeof(PageHdr) + 
                                         tuple_size*(tuples_per_page-1);

            _seek_location = (char*)_q->_enq_location - tuple_size;
            _seek_page = _q->_enq_page;

            if ((char*)_seek_location < (char*)_seek_page + sizeof(PageHdr))
            {
                if (_seek_page != _q->_deq_page)
                {
                    _seek_page = ((PageHdr*)_seek_page)->prev;
                    _seek_location = (char*)_seek_page + 
                                     _offset_last_tuple_on_page;
                } 
                else 
                { // Nothing there
                    _seek_page = _q->_deq_page;
                    _seek_location = _q->_deq_location;
                }
            }
        }

        /// Prohibit postfix increment.
        RSeekIterator operator ++ (int);

        /// Prohibit postfix decrement
        RSeekIterator operator -- (int);

        TupleQueue *_q;

        // this is where our seek iterator is pointing
        void *_seek_location;
        /// The page we're set to dequeue data from
        void *_seek_page;

        size_t _offset_last_tuple_on_page;
        friend class TupleQueue;
    };

    RSeekIterator rseek_iterator() { return RSeekIterator(*this); }

    SeekIterator seek_iterator() { return SeekIterator(*this); }

    /// Obtains an enqueue iterator for the TupleQueue.  Only a single
    /// EnqIterator (and a single DeqIterator, not necessarily in the
    /// same thread) should ever be outstanding.
    EnqIterator enq_iterator() { return EnqIterator(*this); }

    /// Obtains an dequeue iterator for the TupleQueue.  Only a single
    /// DeqIterator (and a single EnqIterator, not necessarily in the
    /// same thread) should ever be outstanding.
    DeqIterator deq_iterator() { return DeqIterator(*this); }

    /// Dumps information about the queue to standard error.
    void dump() const;

    /// Returns the number of tuples currently contained within the queue.
    int size() const { return _enq_count - _deq_count; }

    /// Returns the number of tuples that have ever been dequeued from this
    /// TupleQueue.
    /// THREAD SAFETY NOTE:  This method must not be run concurrently with
    /// the dequeueing of tuples from this TupleQueue. (This is fixable, but
    /// I saw no need to write that code yet. -cjc)
    size_t get_num_tuples_dequeued() const { return _deq_count; }

    /// Returns the number of tuples that have ever been enqueued to this
    /// TupleQueue.
    /// (presumably the above safety note applies to this as well -alexr)
    /// THREAD SAFETY NOTE:  This method must not be run concurrently with
    /// the dequeueing of tuples from this TupleQueue. (This is fixable, but
    /// I saw no need to write that code yet. -cjc)
    size_t get_num_tuples_enqueued() const { return _enq_count; }

    /// Adds a listener to the queue.  The listener's notify() method
    /// will be invoked whenever notifyEnq() is called.
    void add_listener(TupleQueueListener* listener)
    {
        _listeners.push_back(listener);
    }

    /// Removes a listener from the queue.
    void remove_listener(TupleQueueListener* listener)
    {
        erase_in(_listeners, listener);
    }

    /// Returns the size of tuples in this queue.
    unsigned int get_tuple_size() const
    {
        return _tuple_size;
    }
    
    double get_staleness();

   /// Sets the mailbox whose onNonEmpty() method will be called
   /// whenever this TupleQueue's notifyEnq() method is called.
   ///
   /// Passing a NULL pointer to this method terminates the
   /// notification mechanism.
   void set_enq_notify_target(TuplesAvailableMailbox * p_mailbox);

   /// Calls the mailbox's onNonEmpty() method and each listener's
   /// notify() method.
   void notify_enq();

   /// lock so that only one thing is writing to the queue.
   /// Use it around the enq_iterator(), memcpy(), ++e
   /// Could be made private with a function to return it, but ...
   PtMutex _lock;

  private:
    TuplesAvailableMailbox * _p_mailbox;

    struct PageHdr
    {
        PageHdr *next;
        PageHdr *prev;
    };

    PagePool &_pool;
    unsigned int _tuple_size;
    unsigned int _tuples_per_page;

    unsigned int _enq_count;
    unsigned int _deq_count;
    Timestamp _last_timestamp;

    /// The page we're set to enqueue data on
    PageHdr *_enq_page;

    /// The location we're set to enqueue data on.  We can always
    /// enqueue data here
    void *_enq_location;

    /// The page we're set to dequeue data from
    PageHdr *_deq_page;

    /// The location we're set to dequeue data from.  We can always
    /// dequeue unless _deq_page_index == _enq_page_index
    void *_deq_location;

    /// The associated stream (optional; just for bookkeeping)
    Stream *_stream;

    typedef vector<TupleQueueListener*> Listeners;
    Listeners _listeners;

    // Optional UpdateQueue index. -- tatbul
    //
    UpdateQueue  *_update_queue;

    friend class UpdateQueue;
};

inline TupleQueue::EnqIterator& TupleQueue::EnqIterator::operator ++ ()
{
    assert(_q);

    // UpdateQueue index maintenance -- tatbul
    //
    if (_q->_update_queue)
    {
        DEBUG << "Incrementing an update queue, requires index maintenance..";

        if (_q->_update_queue->get_type() == UpdateQueue::FIFO)
        {
            _q->_update_queue->enqueue_fifo();
        }
        else if ((_q->_update_queue->get_type() == UpdateQueue::INPLACE) ||
                 (_q->_update_queue->get_type() == UpdateQueue::LINECUT_PRIOR))
        {
            DEBUG << "enq = " << _q->_enq_location;
            _q->_update_queue->enqueue_inplace();
        }
        else if ((_q->_update_queue->get_type() == 
                                UpdateQueue::LINECUT_UF_FRONT) ||
                 (_q->_update_queue->get_type() == 
                                UpdateQueue::LINECUT_UF_MIDDLE) ||
                 (_q->_update_queue->get_type() == 
                                UpdateQueue::LINECUT_DF_FRONT) ||
                 (_q->_update_queue->get_type() == 
                                UpdateQueue::LINECUT_DF_MIDDLE) ||
                 (_q->_update_queue->get_type() == 
                                UpdateQueue::LINECUT_UF_MIDDLE_DETERMINISTIC) ||
                 (_q->_update_queue->get_type() ==
                                UpdateQueue::LINECUT_DF_MIDDLE_DETERMINISTIC))
        {
            _q->_update_queue->enqueue_linecut();
        }
        else
        {
            WARN << "Invalid queue type : " 
                 << (int)(_q->_update_queue->get_type());
        }
    }

    void *next_enq_location = (char*)_q->_enq_location + _q->_tuple_size;
    if ((char*)next_enq_location + _q->_tuple_size > 
                                (char*)_q->_enq_page + _q->_pool.page_size())
    {
        // Allocate new page. Note that the DeqIterator
        // cannot ever advance to the new page until we set
        // _enq_location (at the very end of this method)
        //
        PageHdr *new_page = (PageHdr*)_q->_pool.alloc();
        new_page->next = 0;

        new_page->prev = _q->_enq_page;
        _q->_enq_page->next = new_page;
        _q->_enq_page = new_page;

        next_enq_location = (char*)new_page + sizeof(PageHdr);
    }

    _q->_enq_location = next_enq_location;
    DEBUG << "enq = " << _q->_enq_location;
    ++(_q->_enq_count);
    return *this;
}

inline TupleQueue::DeqIterator& TupleQueue::DeqIterator::operator ++ ()
{
    assert(avail());

    // If update_queue is set, advance UpdateQueue::_head, garbage collect,
    // and adjust _deq_location accordingly. -- tatbul
    //
    if (_q->_update_queue)
    {
        void *next_deq_location = NULL;
        if (_q->_update_queue->get_type() == UpdateQueue::LINECUT_PRIOR)
        {
            _q->_update_queue->dequeue_prior();
        }
        else
        {
            next_deq_location = _q->_update_queue->dequeue();
        }

        void *current_deq_location = _q->_deq_location;
        DEBUG << current_deq_location;

        set<void *> removed; 
        removed.clear();

        while (1)
        {
            if ((_q->_update_queue->_garbage).find(current_deq_location)
                != (_q->_update_queue->_garbage).end())
            {
                DEBUG << "dequeue one";
                removed.insert(current_deq_location);
                _q->_last_timestamp = get_timestamp(current_deq_location);
                current_deq_location = (char*)current_deq_location
                                       + _q->_tuple_size;
                ++(_q->_deq_count);

                if ((char*)current_deq_location + _q->_tuple_size > 
                    (char*)_q->_deq_page + _q->_pool.page_size())
                {
                    // Advance to the next page; can free this page.
                    //
                    DEBUG << "page advance";
                    PageHdr *next_page = _q->_deq_page->next;
                    _q->_pool.free(_q->_deq_page);
                    _q->_deq_page = next_page;

                    current_deq_location = (char*)next_page + sizeof(PageHdr);
                }
                DEBUG << current_deq_location;
            }
            else
            {
                break;
            }
        }

        _q->_deq_location = current_deq_location;

        set<void *>::iterator iter;
        for (iter = removed.begin(); iter != removed.end(); iter++)
        {
            DEBUG << "erasing from the garbage set : " << *iter;
            (_q->_update_queue->_garbage).erase(*iter);
        }

        DEBUG << "new garbage set :";
        for (iter = (_q->_update_queue->_garbage).begin();
             iter != (_q->_update_queue->_garbage).end(); iter++)
        {
            DEBUG << *iter;
        }

        if (next_deq_location == NULL)
        {
            DEBUG << "empty or dequeue_prior";
            /*
            if (_q->_deq_location != _q->_enq_location)
            {
                WARN << "Not clear if the update queue is empty or not !";
            }
            //assert(_q->_deq_location == _q->_enq_location);
            //assert(_q->_deq_count == _q->_enq_count);
            */
        }

        return *this;
    }
    else // If not an update queue, simply increment. 
    {
        void *next_deq_location = (char*)_q->_deq_location + _q->_tuple_size;
        if ((char*)next_deq_location + _q->_tuple_size > 
                                (char*)_q->_deq_page + _q->_pool.page_size())
        {
            // Advance to the next page; can free this page.
            //
            PageHdr *next_page = _q->_deq_page->next;
            _q->_pool.free(_q->_deq_page);
            _q->_deq_page = next_page;

            next_deq_location = (char*)next_page + sizeof(PageHdr);
        }
        _q->_last_timestamp = get_timestamp(_q->_deq_location);
        _q->_deq_location = next_deq_location;
        ++(_q->_deq_count);

        return *this;
    }
}

/// Reads a tuple.
inline TupleQueue::SeekIterator& TupleQueue::SeekIterator::operator ++ ()
{
    void *next_seek_location = (char*)_seek_location + _q->_tuple_size;
    if ((char*)next_seek_location + _q->_tuple_size > 
                                    (char*)_seek_page + _q->_pool.page_size())
    {
        _seek_page = ((PageHdr *)_seek_page)->next;
        next_seek_location = (char*)_seek_page + sizeof(PageHdr);
    }

    _seek_location = next_seek_location;
    return *this;
}

/// Reads a tuple.
inline TupleQueue::SeekIterator& TupleQueue::SeekIterator::operator -- ()
{
    void *next_seek_location = (char*)_seek_location - _q->_tuple_size;
    if ((char*)next_seek_location < (char*)_seek_page + sizeof(PageHdr))
    {
        _seek_page = ((PageHdr *)_seek_page)->prev;
        next_seek_location = (char*)_seek_page + _offset_last_tuple_on_page;
    }

    _seek_location = next_seek_location;
    return *this;
}

/// Reads a tuple.
inline TupleQueue::RSeekIterator& TupleQueue::RSeekIterator::operator ++ ()
{
    void *next_seek_location = (char*)_seek_location - _q->_tuple_size;
    if ((char*)next_seek_location < (char*)_seek_page + sizeof(PageHdr))
    {
        _seek_page = ((PageHdr *)_seek_page)->prev;
        next_seek_location = (char*)_seek_page + _offset_last_tuple_on_page;
    }

    _seek_location = next_seek_location;
    return *this;
}

/// Reads a tuple.
inline TupleQueue::RSeekIterator& TupleQueue::RSeekIterator::operator -- ()
{
    void *next_seek_location = (char*)_seek_location + _q->_tuple_size;
    if ((char*)next_seek_location + _q->_tuple_size > 
                                    (char*)_seek_page + _q->_pool.page_size())
    {
        _seek_page = ((PageHdr *)_seek_page)->next;
        next_seek_location = (char*)_seek_page + sizeof(PageHdr);
    }

    _seek_location = next_seek_location;
    return *this;
}

BOREALIS_NAMESPACE_END;

#endif

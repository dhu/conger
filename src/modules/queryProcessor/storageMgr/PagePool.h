
#ifndef PAGEPOOL_H
#define PAGEPOOL_H

#include <PtMutex.h>

BOREALIS_NAMESPACE_BEGIN;

/// Thread-safe page pool.
class PagePool
{
  public:
    /// Allocates <i>capacity</i> page-aligned blocks, each
    /// <i>page_size</i> bytes long, all locked into main memory (if <i>locked</i>).
    /// (One great big memalign call!)  Throws an exception if unable
    /// to allocate and lock all that memory.
    ///
    /// <i>page_size</i> should generally be a power of two and
    /// multiple of the system page size.
    PagePool(unsigned int page_size, unsigned int capacity,
             bool locked = true);

    /// Destructor.
    ~PagePool();

    /// Allocates and returns a single page.  Throws an exception
    /// if there are no free blocks left.
    void *alloc();

    /// Frees a page.
    void free(void *);

    /// Returns the page size for the pool (i.e., the page_size
    /// argument to the constructor).
    unsigned int page_size() const { return _page_size; }

    /// Returns the pool capacity (i.e., the capacity argument to the
    /// constructor).
    unsigned int capacity() const { return _capacity; }

    /// Returns the number of pages currently allocated.
    unsigned int alloced_pages() const { return _alloced_pages; }

    /// Returns the number of pages currently free.
    unsigned int free_pages() const { return _capacity - _alloced_pages; }

    /// Dumps some state to stderr.
    void dump() const;
    void dump_no_free_list() const;

  private:
    struct FreeNode
    {
        FreeNode *next;
    };

    unsigned int _page_size;
    unsigned int _capacity;
    unsigned int _alloced_pages;

    void *_block;
    unsigned int _block_size;
    bool _locked;
    FreeNode *_free_list;

    PtMutex _mutex;
};

BOREALIS_NAMESPACE_END;

#endif

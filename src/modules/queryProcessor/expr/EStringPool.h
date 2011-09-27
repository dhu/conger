#ifndef ESTRINGPOOL_H
#define ESTRINGPOOL_H

//using namespace Borealis;        // ???
BOREALIS_NAMESPACE_BEGIN

/// A memory pool for string data.
///
/// You may allocate a chunk of memory using the alloc() method; this
/// memory is guaranteed to be reserved until the EStringPool is
/// destroyed or the next time reset() is called.
///
/// Chunks are guaranteed to be aligned suitably for any kind of
/// variable (currently hardcoded to 8 bytes).
class EStringPool
{
  public:
    /// Constructs a pool with the specified initial size.
    /// The pool will double in size as necessary to accomodate
    /// allocation requests.
    EStringPool(unsigned int initial_size = DEFAULT_INITIAL_SIZE) :
        _data(static_cast<char*>(malloc(initial_size))),
        _offset(0),
        _size(initial_size)
    {
        ASSERT(_size);
    }

    /// Destroys the pool, invalidating any allocated buffers.
    ~EStringPool()
    {
        reset();
        free(_data);
    }

    /// Invalidates any allocated buffers.
    void reset()
    {
        if (!_to_delete.empty())
        {
            for (vector<char*>::iterator i = _to_delete.begin(); i < _to_delete.end(); ++i)
                free(*i);

            _to_delete.clear();
        }
    }

    /// Allocates a chunk of memory.  Chunks are guaranteed to be
    /// aligned suitably for any kind of variable (currently hardcoded
    /// to 8 bytes).  The chunk is valid only until the next reset()
    /// call or the destruction of the EStringPool object.
    char *alloc(unsigned int size)
    {
   // Round size up to multiple of 8 bytes
   size = (size + 7) & ~7;

        if (_offset + size > _size)
        {
            _to_delete.push_back(_data);

            do
            {
                _size <<= 1;
            } while (_size < 2 * (_offset + size));

            _data = static_cast<char*>(malloc(_size));
            _offset = size;

            return _data;
        } else
        {
            char *ret = _data + _offset;
            _offset += size;
            return ret;
        }
    }

    /// The default initial size of a string pool.
    static const unsigned int DEFAULT_INITIAL_SIZE = 10240;

  private:
    char *_data;
    unsigned int _offset;
    unsigned int _size;

    vector<char*> _to_delete;
};

BOREALIS_NAMESPACE_END
#endif                     // ESTRINGPOOL_H

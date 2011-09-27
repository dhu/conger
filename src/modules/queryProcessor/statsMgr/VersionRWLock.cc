/**
 * VersionRWLock.cc:
 *
 * Defines a read write Lock that will not block the writers. The
 * writers can always read, but the readers can only read if no writer
 * is writing.
 *
 * While the reader is reading, the writer can write at the same time,
 * thus the user must check whether the version is valid after reading.
 *
 */

#include  "VersionRWLock.h"

BOREALIS_NAMESPACE_BEGIN;

//  Constructor
//
VersionRWLock::VersionRWLock() :
    _version( 0 ),
    _writers( 0 )
{
    if ( pthread_mutex_init ( &mutex, 0 ) < 0 )
    {
        perror( "mutex_init" );
    }

    if ( pthread_cond_init( &read_ok, 0 ) < 0 )
    {
        perror( "cond_init readOK" );
    }
}


// Gets read permit and returns the current version.
//
int32  VersionRWLock::read_version()
{
    int32  v;

    if ( pthread_mutex_lock(  &mutex ) < 0 )
    {
        perror( "readlock mutex_lock" );
    }

    if ( _writers > 0 )
    {
        if ( pthread_cond_wait( &read_ok, &mutex ) < 0 )
        {
            perror( "readlock cond_wait" );
        }
    }

    v = _version;

    if ( pthread_mutex_unlock( &mutex ) < 0 )
    {
        perror( "readlock mutex_unlock" );
    }

    return( v );
}


// verify whether the given version and the current version is the same.
// Returns true if so.
//
bool  VersionRWLock::verify_version( int32  version )
{
    return( version == _version );
}


// Write lock the structure (no readers are alowed) and update the version.
//
void  VersionRWLock::write_lock()
{
    if ( pthread_mutex_lock( &mutex ) < 0 )
    {
        perror( "writelock mutex_lock" );
    }

    ++_writers;
    ++_version;

    if ( pthread_mutex_unlock( &mutex ) < 0 )
    {
        perror( "writelock mutex_lock" );
    }
}


// Write unlock the structure. If there are no writers, then wakeup the
// waiting readers
//
void  VersionRWLock::write_un_lock()
{
    if ( pthread_mutex_lock( &mutex ) < 0 )
    {
        perror( "writeunlock mutex_lock" );
    }

    --_writers;

    if ( _writers == 0 )
    {
        if ( pthread_cond_broadcast( &read_ok ) < 0 )
        {
            perror( "writeunlock cond_broadcast" );
        }
    }

    if ( pthread_mutex_unlock( &mutex ) < 0 )
    {
        perror( "writeunlock mutex_unlock" );
    }
}

BOREALIS_NAMESPACE_END;

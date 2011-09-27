/**
 * RWlock.cc
 *
 * Defines a readwriteLock template to help user protect data items.
 * User is resonsible for enforcing semantics.
 *
 * This lock favors writers
 *
 */


#include  "RWlock.h"


// Construct a RWLock
//
RWLock::RWLock():
    writers( 0 ),
    readers( 0 ),
    waiting( 0 )
{
    pthread_mutexattr_init( &mutexattr );

    if ( pthread_mutex_init( &mutex, &mutexattr ) < 0 )
    {
        perror("mutex_init");
    }

    if ( pthread_cond_init( &write_ok, 0 ) < 0 )
    {
        perror("cond_init writeOK");
    }

    if ( pthread_cond_init( &read_ok, 0 ) < 0 )
    {
        perror( "cond_init readOK" );
    }
}


// Obtains the read permission.
// The given parameter id can be used for debugging.
//
void RWLock::readlock( int32  d )
{
    if ( pthread_mutex_lock( &mutex ) < 0 )
    {
        perror( "readlock mutex_lock" );
    }

    if ( writers || waiting )
    {
        do
        {
            #ifdef DEBUG
                std::cerr << "reader " << d << " blocked." << std::endl;
            #endif

            if ( pthread_cond_wait ( &read_ok, &mutex ) < 0 )
            {   perror( "readlock cond_wait" );
            }

            #ifdef DEBUG
                std::cerr << "reader " << d << " unblocked." << std::endl;
            #endif

        } while ( writers );
    }

    ++readers;

    if ( pthread_mutex_unlock( &mutex ) < 0 )
    {
        perror( "readlock mutex_unlock" );
    }
}



// Obertains the write permission.
// The given parameter id can used for debugging.
//
void RWLock::writelock( int32  d )
{
    if ( pthread_mutex_lock( &mutex ) <0 )
    {
        perror( "writelock mutex_lock" );
    }

    ++waiting;

    while ( readers || writers )
    {
        #ifdef DEBUG
            std::cerr << "writer " << d << " blocked." << std::endl;
        #endif

        if ( pthread_cond_wait( &write_ok, &mutex ) <0 )
        {
            perror( "writelock cond_wait" );
        }

        #ifdef DEBUG
            std::cerr << "writer " << d << " unblocked." << std::endl;
        #endif
    }

    --waiting;
    ++writers;

    if ( pthread_mutex_unlock( &mutex ) < 0 )
    {
        perror( "writelock mutex_lock" );
    }
}


// Release the read lock
//
void RWLock::readunlock()
{
    if ( pthread_mutex_lock ( &mutex ) < 0 )
    {
        perror( "readunlock mutex_lock" );
    }

    --readers;

    if ( pthread_cond_signal( &write_ok ) < 0 )
    {
        perror( "readunlock cond_signal" );
    }

    if ( pthread_mutex_unlock( &mutex ) < 0 )
    {
        perror( "readunlock mutex_unlock" );
    }
}


// Release the write lock
//
void RWLock::writeunlock()
{
    if ( pthread_mutex_lock( &mutex ) < 0 )
    {
        perror( "writeunlock mutex_lock" );
    }

    --writers;

    if ( waiting == 0 )
    {
        if ( pthread_cond_broadcast( &read_ok ) < 0 )
        {
            perror( "writeunlock cond_broadcast" );
        }
        else if ( pthread_cond_signal( &write_ok ) <0 )
        {
            perror( "writeunlock cond_signal" );
        }
    }

    if ( pthread_mutex_unlock( &mutex ) < 0 )
    {
        perror( "writeunlock mutex_unlock" );
    }
}

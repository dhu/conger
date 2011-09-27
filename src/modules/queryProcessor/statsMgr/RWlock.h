

#ifndef RWLOCK_H
#define RWLOCK_H

#include  <iostream>
#include  <pthread.h>
#include  <stdio.h>

#include  "common.h"

using namespace Borealis;


/**
 * RWLock class implements a readwriteLock template to help user
 * protect data items. User is resonsible for enforcing semantics.
 *
 * This lock favors writers
 */
class RWLock
{
public:
    /**
     * Construct a RWLock
     */
    RWLock();

    /**
     * Obtains the read permission.
     * If the thread is blocked and the DEBUG is defined, then a
     * message will be ouptut saying that reader at id is blocked.
     * @param id The given parameter id can be used for debugging.
     */
    void readlock( int32  id = 0 );

    /**
     * Obertains the write permission.
     * If the thread is blocked and the DEBUG is defined, then a
     * message will be ouptut saying that reader at id is blocked.
     * @param id The given parameter id can be used for debugging.
     */
    void writelock( int32  id = 0 );

    /**
     * Release the read lock
     */
    void readunlock();

    /**
     * Release the write lock
     */
    void writeunlock();

private:
    pthread_mutex_t      mutex;
    pthread_mutexattr_t  mutexattr;
    pthread_cond_t       write_ok;
    pthread_cond_t       read_ok;

    int32  writers;
    int32  readers;
    int32  waiting;
};

#endif                            // RWLOCK_H

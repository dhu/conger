#ifndef VERSIONRWLOCK_H
#define VERSIONRWLOCK_H

#include  <iostream>
#include  <pthread.h>
#include  <stdio.h>

#include  "common.h"

BOREALIS_NAMESPACE_BEGIN;

/**
 *
 * The VersionRWLoc class implements a read write Lock that will not block the writers.
 * The writers can always read, but the readers can only read if no writer
 * is writing.
 *
 * When the reader is reading, the writer can write at the same time.
 * But the writer always update the version after before it write the data.
 * Thus the reaser can check the version before and after read action to
 * check whether the data has been updated.
 */
class VersionRWLock
{
public:
    /**
     * Constructor the lock
     */
    VersionRWLock();

    /**
     * Gets read permit and returns the current version
     */
    int32  read_version();

    /**
     * verify whether the given version and the current version is the same.
     * Returns true if so.
     * @param version the version to be verified
     */
    bool  verify_version( int32  version );

    /**
     * Write lock the structure (no readers are alowed) and update the version.
     */
    void  write_lock();

    /**
     * Write unlock the structure. If there are no writers, then wakeup the
     * waiting readers
     */
    void  write_un_lock();

private:
    pthread_mutex_t  mutex;
    pthread_cond_t   read_ok;

    int32  _version;
    int32  _writers;
};

BOREALIS_NAMESPACE_END;

#endif                          // VERSIONRWLOCK_H

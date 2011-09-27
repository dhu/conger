/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_mutex.h,v 1.3 2005-07-27 12:26:53 bb Exp $";
#endif

#ifndef _SDL_mutex_h
#define _SDL_mutex_h

/* Functions to provide thread synchronization primitives

    These are independent of the other SDL routines.
*/

#include "SDL_main.h"
#include "SDL_types.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

/* Synchronization functions which can time out return this value
   if they time out.
*/
#define SDL_MUTEX_TIMEDOUT  1

/* This is the timeout value which corresponds to never time out */
#define SDL_MUTEX_MAXWAIT   (~(Uint32)0)


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Mutex functions                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* The SDL mutex structure, defined in SDL_mutex.c */
struct sdl_mutex;
typedef struct sdl_mutex sdl_mutex;

/* Create a mutex, initialized unlocked */
extern DECLSPEC sdl_mutex * SDLCALL sdl_create_mutex(void);

/* Lock the mutex  (Returns 0, or -1 on error) */
#define SDL_LockMutex(m)    SDL_mutexP(m)
extern DECLSPEC int SDLCALL sdl_mutex_p(sdl_mutex *mutex);

/* Unlock the mutex  (Returns 0, or -1 on error)
   It is an error to unlock a mutex that has not been locked by
   the current thread, and doing so results in undefined behavior.
 */
#define SDL_UnlockMutex(m)  SDL_mutexV(m)
extern DECLSPEC int SDLCALL sdl_mutex_v(sdl_mutex *mutex);

/* Destroy a mutex */
extern DECLSPEC void SDLCALL sdl_destroy_mutex(sdl_mutex *mutex);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Semaphore functions                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* The SDL semaphore structure, defined in SDL_sem.c */
struct sdl_semaphore;
typedef struct sdl_semaphore sdl_sem;

/* Create a semaphore, initialized with value, returns NULL on failure. */
extern DECLSPEC sdl_sem * SDLCALL sdl_create_semaphore(uint_32 initial_value);

/* Destroy a semaphore */
extern DECLSPEC void SDLCALL sdl_destroy_semaphore(sdl_sem *sem);

/* This function suspends the calling thread until the semaphore pointed
 * to by sem has a positive count. It then atomically decreases the semaphore
 * count.
 */
extern DECLSPEC int SDLCALL sdl_sem_wait(sdl_sem *sem);

/* Non-blocking variant of SDL_SemWait(), returns 0 if the wait succeeds,
   SDL_MUTEX_TIMEDOUT if the wait would block, and -1 on error.
*/
extern DECLSPEC int SDLCALL sdl_sem_try_wait(sdl_sem *sem);

/* Variant of SDL_SemWait() with a timeout in milliseconds, returns 0 if
   the wait succeeds, SDL_MUTEX_TIMEDOUT if the wait does not succeed in
   the allotted time, and -1 on error.
   On some platforms this function is implemented by looping with a delay
   of 1 ms, and so should be avoided if possible.
*/
extern DECLSPEC int SDLCALL sdl_sem_wait_timeout(sdl_sem *sem, uint_32 ms);

/* Atomically increases the semaphore's count (not blocking), returns 0,
   or -1 on error.
 */
extern DECLSPEC int SDLCALL sdl_sem_post(sdl_sem *sem);

/* Returns the current count of the semaphore */
extern DECLSPEC uint_32 SDLCALL sdl_sem_value(sdl_sem *sem);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Condition variable functions                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* The SDL condition variable structure, defined in SDL_cond.c */
struct sdl_cond;
typedef struct sdl_cond sdl_cond;

/* Create a condition variable */
extern DECLSPEC sdl_cond * SDLCALL sdl_create_cond(void);

/* Destroy a condition variable */
extern DECLSPEC void SDLCALL sdl_destroy_cond(sdl_cond *cond);

/* Restart one of the threads that are waiting on the condition variable,
   returns 0 or -1 on error.
 */
extern DECLSPEC int SDLCALL sdl_cond_signal(sdl_cond *cond);

/* Restart all threads that are waiting on the condition variable,
   returns 0 or -1 on error.
 */
extern DECLSPEC int SDLCALL sdl_cond_broadcast(sdl_cond *cond);

/* Wait on the condition variable, unlocking the provided mutex.
   The mutex must be locked before entering this function!
   Returns 0 when it is signaled, or -1 on error.
 */
extern DECLSPEC int SDLCALL sdl_cond_wait(sdl_cond *cond, sdl_mutex *mut);

/* Waits for at most 'ms' milliseconds, and returns 0 if the condition
   variable is signaled, SDL_MUTEX_TIMEDOUT if the condition is not
   signaled in the allotted time, and -1 on error.
   On some platforms this function is implemented by looping with a delay
   of 1 ms, and so should be avoided if possible.
*/
extern DECLSPEC int SDLCALL sdl_cond_wait_timeout(sdl_cond *cond, sdl_mutex *mutex, uint_32 ms);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_mutex_h */

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
 "@(#) $Id: SDL_thread.h,v 1.3 2005-07-27 12:26:53 bb Exp $";
#endif

#ifndef _SDL_thread_h
#define _SDL_thread_h

/* Header for the SDL thread management routines

    These are independent of the other SDL routines.
*/

#include "SDL_main.h"
#include "SDL_types.h"

/* Thread synchronization primitives */
#include "SDL_mutex.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

/* The SDL thread structure, defined in SDL_thread.c */
struct sdl_thread;
typedef struct sdl_thread sdl_thread;

/* Create a thread */
extern DECLSPEC sdl_thread * SDLCALL sdl_create_thread(int (*fn)(void *), void *data);

/* Get the 32-bit thread identifier for the current thread */
extern DECLSPEC uint_32 SDLCALL sdl_thread_id(void);

/* Get the 32-bit thread identifier for the specified thread,
   equivalent to SDL_ThreadID() if the specified thread is NULL.
 */
extern DECLSPEC uint_32 SDLCALL sdl_get_thread_id(sdl_thread *thread);

/* Wait for a thread to finish.
   The return code for the thread function is placed in the area
   pointed to by 'status', if 'status' is not NULL.
 */
extern DECLSPEC void SDLCALL sdl_wait_thread(sdl_thread *thread, int *status);

/* Forcefully kill a thread without worrying about its state */
extern DECLSPEC void SDLCALL sdl_kill_thread(sdl_thread *thread);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_thread_h */

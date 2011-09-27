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
 "@(#) $Id: SDL_rwops.h,v 1.3 2005-07-27 12:26:53 bb Exp $";
#endif

/* This file provides a general interface for SDL to read and write
   data sources.  It can easily be extended to files, memory, etc.
*/

#ifndef _SDL_RWops_h
#define _SDL_RWops_h

#include <stdio.h>

#include "SDL_types.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

/* This is the read/write operation structure -- very basic */

typedef struct sdl_rwops
{
    /* Seek to 'offset' relative to whence, one of stdio's whence values:
        SEEK_SET, SEEK_CUR, SEEK_END
       Returns the final offset in the data source.
     */
    int (*seek)(struct sdl_rwops *context, int offset, int whence);

    /* Read up to 'num' objects each of size 'objsize' from the data
       source to the area pointed at by 'ptr'.
       Returns the number of objects read, or -1 if the read failed.
     */
    int (*read)(struct sdl_rwops *context, void *ptr, int size, int maxnum);

    /* Write exactly 'num' objects each of size 'objsize' from the area
       pointed at by 'ptr' to data source.
       Returns 'num', or -1 if the write failed.
     */
    int (*write)(struct sdl_rwops *context, const void *ptr, int size, int num);

    /* Close and free an allocated SDL_FSops structure */
    int (*close)(struct sdl_rwops *context);

    uint_32 type;
    union
    {
        struct
        {
        int autoclose;
        FILE *fp;
        } stdio;
        struct
        {
        uint_8 *base;
        uint_8 *here;
        uint_8 *stop;
        } mem;
        struct
        {
        void *data1;
        } unknown;
    } hidden;

} sdl_rwops;


/* Functions to create SDL_RWops structures from various data sources */

extern DECLSPEC sdl_rwops * SDLCALL sdl_rwfrom_file(const char *file, const char *mode);

extern DECLSPEC sdl_rwops * SDLCALL sdl_rwfrom_fp(FILE *fp, int autoclose);

extern DECLSPEC sdl_rwops * SDLCALL sdl_rwfrom_mem(void *mem, int size);
extern DECLSPEC sdl_rwops * SDLCALL sdl_rwfrom_const_mem(const void *mem, int size);

extern DECLSPEC sdl_rwops * SDLCALL sdl_alloc_rw(void);
extern DECLSPEC void SDLCALL sdl_free_rw(sdl_rwops *area);

/* Macros to easily read and write from an SDL_RWops structure */
#define SDL_RWseek(ctx, offset, whence) (ctx)->seek(ctx, offset, whence)
#define SDL_RWtell(ctx)         (ctx)->seek(ctx, 0, SEEK_CUR)
#define SDL_RWread(ctx, ptr, size, n)   (ctx)->read(ctx, ptr, size, n)
#define SDL_RWwrite(ctx, ptr, size, n)  (ctx)->write(ctx, ptr, size, n)
#define SDL_RWclose(ctx)        (ctx)->close(ctx)


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_RWops_h */

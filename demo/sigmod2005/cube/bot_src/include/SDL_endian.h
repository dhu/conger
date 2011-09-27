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
 "@(#) $Id: SDL_endian.h,v 1.3 2005-07-27 12:26:53 bb Exp $";
#endif

/* Functions for reading and writing endian-specific values */

#ifndef _SDL_endian_h
#define _SDL_endian_h

/* These functions read and write data of the specified endianness,
   dynamically translating to the host machine endianness.

   e.g.: If you want to read a 16 bit value on big-endian machine from
         an open file containing little endian values, you would use:
        value = SDL_ReadLE16(rp);
         Note that the read/write functions use SDL_RWops pointers
         instead of FILE pointers.  This allows you to read and write
         endian values from large chunks of memory as well as files
         and other data sources.
*/

#include <stdio.h>

#include "SDL_types.h"
#include "SDL_rwops.h"
#include "SDL_byteorder.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

/* The macros used to swap values */
/* Try to use superfast macros on systems that support them */
#ifdef linux
#include <asm/byteorder.h>
#ifdef __arch__swab16
#define SDL_Swap16  __arch__swab16
#endif
#ifdef __arch__swab32
#define SDL_Swap32  __arch__swab32
#endif
#endif /* linux */
/* Use inline functions for compilers that support them, and static
   functions for those that do not.  Because these functions become
   static for compilers that do not support inline functions, this
   header should only be included in files that actually use them.
*/
#ifndef SDL_Swap16
static __inline__ uint_16 sdl_swap_16(uint_16 D)
{
    return((D<<8)|(D>>8));
}
#endif
#ifndef SDL_Swap32
static __inline__ uint_32 sdl_swap_32(uint_32 D)
{
    return((D<<24)|((D<<8)&0x00FF0000)|((D>>8)&0x0000FF00)|(D>>24));
}
#endif
#ifdef SDL_HAS_64BIT_TYPE
#ifndef SDL_Swap64
static __inline__ uint_64 sdl_swap_64(uint_64 val)
{
    uint_32 hi, lo;

    /* Separate into high and low 32-bit values and swap them */
    lo = (uint_32)(val&0xFFFFFFFF);
    val >>= 32;
    hi = (uint_32)(val&0xFFFFFFFF);
    val = sdl_swap_32(lo);
    val <<= 32;
    val |= sdl_swap_32(hi);
    return(val);
}
#endif
#else
#ifndef SDL_Swap64
/* This is mainly to keep compilers from complaining in SDL code.
   If there is no real 64-bit datatype, then compilers will complain about
   the fake 64-bit datatype that SDL provides when it compiles user code.
*/
#define SDL_Swap64(X)   (X)
#endif
#endif /* SDL_HAS_64BIT_TYPE */


/* Byteswap item from the specified endianness to the native endianness */
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SDL_SwapLE16(X) (X)
#define SDL_SwapLE32(X) (X)
#define SDL_SwapLE64(X) (X)
#define SDL_SwapBE16(X) SDL_Swap16(X)
#define SDL_SwapBE32(X) SDL_Swap32(X)
#define SDL_SwapBE64(X) SDL_Swap64(X)
#else
#define SDL_SwapLE16(X) SDL_Swap16(X)
#define SDL_SwapLE32(X) SDL_Swap32(X)
#define SDL_SwapLE64(X) SDL_Swap64(X)
#define SDL_SwapBE16(X) (X)
#define SDL_SwapBE32(X) (X)
#define SDL_SwapBE64(X) (X)
#endif

/* Read an item of the specified endianness and return in native format */
extern DECLSPEC uint_16 SDLCALL sdl_read_le16(sdl_rwops *src);
extern DECLSPEC uint_16 SDLCALL sdl_read_be16(sdl_rwops *src);
extern DECLSPEC uint_32 SDLCALL sdl_read_le32(sdl_rwops *src);
extern DECLSPEC uint_32 SDLCALL sdl_read_be32(sdl_rwops *src);
extern DECLSPEC uint_64 SDLCALL sdl_read_le64(sdl_rwops *src);
extern DECLSPEC uint_64 SDLCALL sdl_read_be64(sdl_rwops *src);

/* Write an item of native format to the specified endianness */
extern DECLSPEC int SDLCALL sdl_write_le16(sdl_rwops *dst, uint_16 value);
extern DECLSPEC int SDLCALL sdl_write_be16(sdl_rwops *dst, uint_16 value);
extern DECLSPEC int SDLCALL sdl_write_le32(sdl_rwops *dst, uint_32 value);
extern DECLSPEC int SDLCALL sdl_write_be32(sdl_rwops *dst, uint_32 value);
extern DECLSPEC int SDLCALL sdl_write_le64(sdl_rwops *dst, uint_64 value);
extern DECLSPEC int SDLCALL sdl_write_be64(sdl_rwops *dst, uint_64 value);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_endian_h */

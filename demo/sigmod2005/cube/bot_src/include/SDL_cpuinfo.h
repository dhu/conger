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
 "@(#) $Id: SDL_cpuinfo.h,v 1.3 2005-07-27 12:26:53 bb Exp $";
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* CPU feature detection for SDL                                       */

#ifndef _SDL_cpuinfo_h
#define _SDL_cpuinfo_h

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

/* This function returns true if the CPU has the RDTSC instruction
 */
extern DECLSPEC sdl_bool SDLCALL sdl_has_rdtsc();

/* This function returns true if the CPU has MMX features
 */
extern DECLSPEC sdl_bool SDLCALL sdl_has_mmx();

/* This function returns true if the CPU has MMX Ext. features
 */
extern DECLSPEC sdl_bool SDLCALL sdl_has_mmxext();

/* This function returns true if the CPU has 3DNow features
 */
extern DECLSPEC sdl_bool SDLCALL sdl_has_3dnow();

/* This function returns true if the CPU has 3DNow! Ext. features
 */
extern DECLSPEC sdl_bool SDLCALL sdl_has_3dnow_ext();

/* This function returns true if the CPU has SSE features
 */
extern DECLSPEC sdl_bool SDLCALL sdl_has_sse();

/* This function returns true if the CPU has SSE2 features
 */
extern DECLSPEC sdl_bool SDLCALL sdl_has_sse2();

/* This function returns true if the CPU has AltiVec features
 */
extern DECLSPEC sdl_bool SDLCALL sdl_has_alti_vec();

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_cpuinfo_h */

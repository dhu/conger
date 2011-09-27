/*
    IMGLIB:  An example image loading library for use with SDL
    Copyright (C) 1999  Sam Lantinga

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@devolution.com
*/

/* A simple library to load images of various formats as SDL surfaces */

#ifndef _IMG_h
#define _IMG_h

#include "SDL.h"
#include "begin_code.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

/* Load an image from an SDL data source.
   The 'type' may be one of: "BMP", "GIF", "PNG", etc.

   If the image format supports a transparent pixel, SDL will set the
   colorkey for the surface.  You can enable RLE acceleration on the
   surface afterwards by calling:
    SDL_SetColorKey(image, SDL_RLEACCEL, image->format->colorkey);
 */
extern DECLSPEC sdl_surface *img_load_typed_rw(sdl_rwops *src, int freesrc,
                          char *type);
/* Convenience functions */
extern DECLSPEC sdl_surface *img_load(const char *file);
extern DECLSPEC sdl_surface *img_load_rw(sdl_rwops *src, int freesrc);

/* Invert the alpha of a surface for use with OpenGL
   This function is now a no-op, and only provided for backwards compatibility.
*/
extern DECLSPEC int img_invert_alpha(int on);

/* Functions to detect a file type, given a seekable source */
extern DECLSPEC int img_is_bmp(sdl_rwops *src);
extern DECLSPEC int img_is_pnm(sdl_rwops *src);
extern DECLSPEC int img_is_xpm(sdl_rwops *src);
extern DECLSPEC int img_is_xcf(sdl_rwops *src);
extern DECLSPEC int img_is_pcx(sdl_rwops *src);
extern DECLSPEC int img_is_gif(sdl_rwops *src);
extern DECLSPEC int img_is_jpg(sdl_rwops *src);
extern DECLSPEC int img_is_tif(sdl_rwops *src);
extern DECLSPEC int img_is_png(sdl_rwops *src);

/* Individual loading functions */
extern DECLSPEC sdl_surface *img_load_bmp_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_pnm_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_xpm_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_xcf_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_pcx_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_gif_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_jpg_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_tif_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_png_rw(sdl_rwops *src);
extern DECLSPEC sdl_surface *img_load_tga_rw(sdl_rwops *src);

/* We'll use SDL for reporting errors */
#define IMG_SetError    SDL_SetError
#define IMG_GetError    SDL_GetError

/* used internally, NOT an exported function */
extern DECLSPEC int img_string_equals(const char *str1, const char *str2);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _IMG_h */

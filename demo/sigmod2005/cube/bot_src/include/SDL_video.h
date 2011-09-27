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
 "@(#) $Id: SDL_video.h,v 1.3 2005-07-27 12:26:53 bb Exp $";
#endif

/* Header file for access to the SDL raw framebuffer window */

#ifndef _SDL_video_h
#define _SDL_video_h

#include <stdio.h>

#include "SDL_types.h"
#include "SDL_mutex.h"
#include "SDL_rwops.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

/* Transparency definitions: These define alpha as the opacity of a surface */
#define SDL_ALPHA_OPAQUE 255
#define SDL_ALPHA_TRANSPARENT 0

/* Useful data types */
typedef struct
{
    sint_16 x, y;
    uint_16 w, h;
} sdl_rect;

typedef struct
{
    uint_8 r;
    uint_8 g;
    uint_8 b;
    uint_8 unused;
} sdl_color;
#define SDL_Colour SDL_Color

typedef struct
{
    int       ncolors;
    sdl_color *colors;
} sdl_palette;

/* Everything in the pixel format structure is read-only */
typedef struct sdl_pixel_format
{
    sdl_palette *palette;
    uint_8  bits_per_pixel;
    uint_8  bytes_per_pixel;
    uint_8  rloss;
    uint_8  gloss;
    uint_8  bloss;
    uint_8  aloss;
    uint_8  rshift;
    uint_8  gshift;
    uint_8  bshift;
    uint_8  ashift;
    uint_32 rmask;
    uint_32 gmask;
    uint_32 bmask;
    uint_32 amask;

    /* RGB color key information */
    uint_32 colorkey;
    /* Alpha value information (per-surface alpha) */
    uint_8  alpha;
} sdl_pixel_format;

/* typedef for private surface blitting functions */
struct sdl_surface;
typedef int (*sdl_blit)(struct sdl_surface *src, sdl_rect *srcrect,
            struct sdl_surface *dst, sdl_rect *dstrect);

/* This structure should be treated as read-only, except for 'pixels',
   which, if not NULL, contains the raw pixel data for the surface.
*/
typedef struct sdl_surface
{
    uint_32 flags;               /* Read-only */
    sdl_pixel_format *format;        /* Read-only */
    int w, h;               /* Read-only */
    uint_16 pitch;               /* Read-only */
    void *pixels;               /* Read-write */
    int offset;             /* Private */

    /* Hardware-specific surface info */
    struct private_hwdata *hwdata;

    /* clipping information */
    sdl_rect clip_rect;         /* Read-only */
    uint_32 unused1;             /* for binary compatibility */

    /* Allow recursive locks */
    uint_32 locked;              /* Private */

    /* info for fast blit mapping to other surfaces */
    struct sdl_blit_map *map;        /* Private */

    /* format version, bumped at every change to invalidate blit maps */
    unsigned int format_version;        /* Private */

    /* Reference count -- used when freeing surface */
    int refcount;               /* Read-mostly */
} sdl_surface;

/* These are the currently supported flags for the SDL_surface */
/* Available for SDL_CreateRGBSurface() or SDL_SetVideoMode() */
#define SDL_SWSURFACE   0x00000000  /* Surface is in system memory */
#define SDL_HWSURFACE   0x00000001  /* Surface is in video memory */
#define SDL_ASYNCBLIT   0x00000004  /* Use asynchronous blits if possible */
/* Available for SDL_SetVideoMode() */
#define SDL_ANYFORMAT   0x10000000  /* Allow any video depth/pixel-format */
#define SDL_HWPALETTE   0x20000000  /* Surface has exclusive palette */
#define SDL_DOUBLEBUF   0x40000000  /* Set up double-buffered video mode */
#define SDL_FULLSCREEN  0x80000000  /* Surface is a full screen display */
#define SDL_OPENGL      0x00000002      /* Create an OpenGL rendering context */
#define SDL_OPENGLBLIT  0x0000000A  /* Create an OpenGL rendering context and use it for blitting */
#define SDL_RESIZABLE   0x00000010  /* This video mode may be resized */
#define SDL_NOFRAME 0x00000020  /* No window caption or edge frame */
/* Used internally (read-only) */
#define SDL_HWACCEL 0x00000100  /* Blit uses hardware acceleration */
#define SDL_SRCCOLORKEY 0x00001000  /* Blit uses a source color key */
#define SDL_RLEACCELOK  0x00002000  /* Private flag */
#define SDL_RLEACCEL    0x00004000  /* Surface is RLE encoded */
#define SDL_SRCALPHA    0x00010000  /* Blit uses source alpha blending */
#define SDL_PREALLOC    0x01000000  /* Surface uses preallocated memory */

/* Evaluates to true if the surface needs to be locked before access */
#define SDL_MUSTLOCK(surface)   \
  (surface->offset ||       \
  ((surface->flags & (SDL_HWSURFACE|SDL_ASYNCBLIT|SDL_RLEACCEL)) != 0))


/* Useful for determining the video hardware capabilities */
typedef struct
{
    uint_32 hw_available :1; /* Flag: Can you create hardware surfaces? */
    uint_32 wm_available :1; /* Flag: Can you talk to a window manager? */
    uint_32 unused_bits_1  :6;
    uint_32 unused_bits_2  :1;
    uint_32 blit_hw      :1; /* Flag: Accelerated blits HW --> HW */
    uint_32 blit_hw_cc   :1; /* Flag: Accelerated blits with Colorkey */
    uint_32 blit_hw_a    :1; /* Flag: Accelerated blits with Alpha */
    uint_32 blit_sw      :1; /* Flag: Accelerated blits SW --> HW */
    uint_32 blit_sw_cc   :1; /* Flag: Accelerated blits with Colorkey */
    uint_32 blit_sw_a    :1; /* Flag: Accelerated blits with Alpha */
    uint_32 blit_fill    :1; /* Flag: Accelerated color fill */
    uint_32 unused_bits_3  :16;
    uint_32 video_mem;   /* The total amount of video memory (in K) */
    sdl_pixel_format *vfmt;  /* Value: The format of the video surface */
} sdl_video_info;


/* The most common video overlay formats.
   For an explanation of these pixel formats, see:
    http://www.webartz.com/fourcc/indexyuv.htm

   For information on the relationship between color spaces, see:
   http://www.neuro.sfc.keio.ac.jp/~aly/polygon/info/color-space-faq.html
 */
#define SDL_YV12_OVERLAY  0x32315659    /* Planar mode: Y + V + U  (3 planes) */
#define SDL_IYUV_OVERLAY  0x56555949    /* Planar mode: Y + U + V  (3 planes) */
#define SDL_YUY2_OVERLAY  0x32595559    /* Packed mode: Y0+U0+Y1+V0 (1 plane) */
#define SDL_UYVY_OVERLAY  0x59565955    /* Packed mode: U0+Y0+V0+Y1 (1 plane) */
#define SDL_YVYU_OVERLAY  0x55595659    /* Packed mode: Y0+V0+Y1+U0 (1 plane) */

/* The YUV hardware video overlay */
typedef struct sdl_overlay
{
    uint_32 format;              /* Read-only */
    int w, h;               /* Read-only */
    int planes;             /* Read-only */
    uint_16 *pitches;            /* Read-only */
    uint_8 **pixels;             /* Read-write */

    /* Hardware-specific surface info */
    struct private_yuvhwfuncs *hwfuncs;
    struct private_yuvhwdata *hwdata;

    /* Special flags */
    uint_32 hw_overlay :1;   /* Flag: This overlay hardware accelerated? */
    uint_32 unused_bits :31;
} sdl_overlay;


/* Public enumeration for setting the OpenGL window attributes. */
typedef enum
{
    SDL_GL_RED_SIZE,
    SDL_GL_GREEN_SIZE,
    SDL_GL_BLUE_SIZE,
    SDL_GL_ALPHA_SIZE,
    SDL_GL_BUFFER_SIZE,
    SDL_GL_DOUBLEBUFFER,
    SDL_GL_DEPTH_SIZE,
    SDL_GL_STENCIL_SIZE,
    SDL_GL_ACCUM_RED_SIZE,
    SDL_GL_ACCUM_GREEN_SIZE,
    SDL_GL_ACCUM_BLUE_SIZE,
    SDL_GL_ACCUM_ALPHA_SIZE,
    SDL_GL_STEREO,
    SDL_GL_MULTISAMPLEBUFFERS,
    SDL_GL_MULTISAMPLESAMPLES
} sdl_glattr;

/* flags for SDL_SetPalette() */
#define SDL_LOGPAL 0x01
#define SDL_PHYSPAL 0x02

/* Function prototypes */

/* These functions are used internally, and should not be used unless you
 * have a specific need to specify the video driver you want to use.
 * You should normally use SDL_Init() or SDL_InitSubSystem().
 *
 * SDL_VideoInit() initializes the video subsystem -- sets up a connection
 * to the window manager, etc, and determines the current video mode and
 * pixel format, but does not initialize a window or graphics mode.
 * Note that event handling is activated by this routine.
 *
 * If you use both sound and video in your application, you need to call
 * SDL_Init() before opening the sound device, otherwise under Win32 DirectX,
 * you won't be able to set full-screen display modes.
 */
extern DECLSPEC int SDLCALL sdl_video_init(const char *driver_name, uint_32 flags);
extern DECLSPEC void SDLCALL sdl_video_quit(void);

/* This function fills the given character buffer with the name of the
 * video driver, and returns a pointer to it if the video driver has
 * been initialized.  It returns NULL if no driver has been initialized.
 */
extern DECLSPEC char * SDLCALL sdl_video_driver_name(char *namebuf, int maxlen);

/*
 * This function returns a pointer to the current display surface.
 * If SDL is doing format conversion on the display surface, this
 * function returns the publicly visible surface, not the real video
 * surface.
 */
extern DECLSPEC sdl_surface * SDLCALL sdl_get_video_surface(void);

/*
 * This function returns a read-only pointer to information about the
 * video hardware.  If this is called before SDL_SetVideoMode(), the 'vfmt'
 * member of the returned structure will contain the pixel format of the
 * "best" video mode.
 */
extern DECLSPEC const sdl_video_info * SDLCALL sdl_get_video_info(void);

/*
 * Check to see if a particular video mode is supported.
 * It returns 0 if the requested mode is not supported under any bit depth,
 * or returns the bits-per-pixel of the closest available mode with the
 * given width and height.  If this bits-per-pixel is different from the
 * one used when setting the video mode, SDL_SetVideoMode() will succeed,
 * but will emulate the requested bits-per-pixel with a shadow surface.
 *
 * The arguments to SDL_VideoModeOK() are the same ones you would pass to
 * SDL_SetVideoMode()
 */
extern DECLSPEC int SDLCALL sdl_video_mode_ok(int width, int height, int bpp, uint_32 flags);

/*
 * Return a pointer to an array of available screen dimensions for the
 * given format and video flags, sorted largest to smallest.  Returns
 * NULL if there are no dimensions available for a particular format,
 * or (SDL_Rect **)-1 if any dimension is okay for the given format.
 *
 * If 'format' is NULL, the mode list will be for the format given
 * by SDL_GetVideoInfo()->vfmt
 */
extern DECLSPEC sdl_rect ** SDLCALL sdl_list_modes(sdl_pixel_format *format, uint_32 flags);

/*
 * Set up a video mode with the specified width, height and bits-per-pixel.
 *
 * If 'bpp' is 0, it is treated as the current display bits per pixel.
 *
 * If SDL_ANYFORMAT is set in 'flags', the SDL library will try to set the
 * requested bits-per-pixel, but will return whatever video pixel format is
 * available.  The default is to emulate the requested pixel format if it
 * is not natively available.
 *
 * If SDL_HWSURFACE is set in 'flags', the video surface will be placed in
 * video memory, if possible, and you may have to call SDL_LockSurface()
 * in order to access the raw framebuffer.  Otherwise, the video surface
 * will be created in system memory.
 *
 * If SDL_ASYNCBLIT is set in 'flags', SDL will try to perform rectangle
 * updates asynchronously, but you must always lock before accessing pixels.
 * SDL will wait for updates to complete before returning from the lock.
 *
 * If SDL_HWPALETTE is set in 'flags', the SDL library will guarantee
 * that the colors set by SDL_SetColors() will be the colors you get.
 * Otherwise, in 8-bit mode, SDL_SetColors() may not be able to set all
 * of the colors exactly the way they are requested, and you should look
 * at the video surface structure to determine the actual palette.
 * If SDL cannot guarantee that the colors you request can be set,
 * i.e. if the colormap is shared, then the video surface may be created
 * under emulation in system memory, overriding the SDL_HWSURFACE flag.
 *
 * If SDL_FULLSCREEN is set in 'flags', the SDL library will try to set
 * a fullscreen video mode.  The default is to create a windowed mode
 * if the current graphics system has a window manager.
 * If the SDL library is able to set a fullscreen video mode, this flag
 * will be set in the surface that is returned.
 *
 * If SDL_DOUBLEBUF is set in 'flags', the SDL library will try to set up
 * two surfaces in video memory and swap between them when you call
 * SDL_Flip().  This is usually slower than the normal single-buffering
 * scheme, but prevents "tearing" artifacts caused by modifying video
 * memory while the monitor is refreshing.  It should only be used by
 * applications that redraw the entire screen on every update.
 *
 * If SDL_RESIZABLE is set in 'flags', the SDL library will allow the
 * window manager, if any, to resize the window at runtime.  When this
 * occurs, SDL will send a SDL_VIDEORESIZE event to you application,
 * and you must respond to the event by re-calling SDL_SetVideoMode()
 * with the requested size (or another size that suits the application).
 *
 * If SDL_NOFRAME is set in 'flags', the SDL library will create a window
 * without any title bar or frame decoration.  Fullscreen video modes have
 * this flag set automatically.
 *
 * This function returns the video framebuffer surface, or NULL if it fails.
 *
 * If you rely on functionality provided by certain video flags, check the
 * flags of the returned surface to make sure that functionality is available.
 * SDL will fall back to reduced functionality if the exact flags you wanted
 * are not available.
 */
extern DECLSPEC sdl_surface * SDLCALL sdl_set_video_mode
            (int width, int height, int bpp, uint_32 flags);

/*
 * Makes sure the given list of rectangles is updated on the given screen.
 * If 'x', 'y', 'w' and 'h' are all 0, SDL_UpdateRect will update the entire
 * screen.
 * These functions should not be called while 'screen' is locked.
 */
extern DECLSPEC void SDLCALL sdl_update_rects
        (sdl_surface *screen, int numrects, sdl_rect *rects);
extern DECLSPEC void SDLCALL sdl_update_rect
        (sdl_surface *screen, sint_32 x, sint_32 y, uint_32 w, uint_32 h);

/*
 * On hardware that supports double-buffering, this function sets up a flip
 * and returns.  The hardware will wait for vertical retrace, and then swap
 * video buffers before the next video surface blit or lock will return.
 * On hardware that doesn not support double-buffering, this is equivalent
 * to calling SDL_UpdateRect(screen, 0, 0, 0, 0);
 * The SDL_DOUBLEBUF flag must have been passed to SDL_SetVideoMode() when
 * setting the video mode for this function to perform hardware flipping.
 * This function returns 0 if successful, or -1 if there was an error.
 */
extern DECLSPEC int SDLCALL sdl_flip(sdl_surface *screen);

/*
 * Set the gamma correction for each of the color channels.
 * The gamma values range (approximately) between 0.1 and 10.0
 *
 * If this function isn't supported directly by the hardware, it will
 * be emulated using gamma ramps, if available.  If successful, this
 * function returns 0, otherwise it returns -1.
 */
extern DECLSPEC int SDLCALL sdl_set_gamma(float red, float green, float blue);

/*
 * Set the gamma translation table for the red, green, and blue channels
 * of the video hardware.  Each table is an array of 256 16-bit quantities,
 * representing a mapping between the input and output for that channel.
 * The input is the index into the array, and the output is the 16-bit
 * gamma value at that index, scaled to the output color precision.
 *
 * You may pass NULL for any of the channels to leave it unchanged.
 * If the call succeeds, it will return 0.  If the display driver or
 * hardware does not support gamma translation, or otherwise fails,
 * this function will return -1.
 */
extern DECLSPEC int SDLCALL sdl_set_gamma_ramp(const uint_16 *red, const uint_16 *green, const uint_16 *blue);

/*
 * Retrieve the current values of the gamma translation tables.
 *
 * You must pass in valid pointers to arrays of 256 16-bit quantities.
 * Any of the pointers may be NULL to ignore that channel.
 * If the call succeeds, it will return 0.  If the display driver or
 * hardware does not support gamma translation, or otherwise fails,
 * this function will return -1.
 */
extern DECLSPEC int SDLCALL sdl_get_gamma_ramp(uint_16 *red, uint_16 *green, uint_16 *blue);

/*
 * Sets a portion of the colormap for the given 8-bit surface.  If 'surface'
 * is not a palettized surface, this function does nothing, returning 0.
 * If all of the colors were set as passed to SDL_SetColors(), it will
 * return 1.  If not all the color entries were set exactly as given,
 * it will return 0, and you should look at the surface palette to
 * determine the actual color palette.
 *
 * When 'surface' is the surface associated with the current display, the
 * display colormap will be updated with the requested colors.  If
 * SDL_HWPALETTE was set in SDL_SetVideoMode() flags, SDL_SetColors()
 * will always return 1, and the palette is guaranteed to be set the way
 * you desire, even if the window colormap has to be warped or run under
 * emulation.
 */
extern DECLSPEC int SDLCALL sdl_set_colors(sdl_surface *surface,
            sdl_color *colors, int firstcolor, int ncolors);

/*
 * Sets a portion of the colormap for a given 8-bit surface.
 * 'flags' is one or both of:
 * SDL_LOGPAL  -- set logical palette, which controls how blits are mapped
 *                to/from the surface,
 * SDL_PHYSPAL -- set physical palette, which controls how pixels look on
 *                the screen
 * Only screens have physical palettes. Separate change of physical/logical
 * palettes is only possible if the screen has SDL_HWPALETTE set.
 *
 * The return value is 1 if all colours could be set as requested, and 0
 * otherwise.
 *
 * SDL_SetColors() is equivalent to calling this function with
 *     flags = (SDL_LOGPAL|SDL_PHYSPAL).
 */
extern DECLSPEC int SDLCALL sdl_set_palette(sdl_surface *surface, int flags,
                   sdl_color *colors, int firstcolor,
                   int ncolors);

/*
 * Maps an RGB triple to an opaque pixel value for a given pixel format
 */
extern DECLSPEC uint_32 SDLCALL sdl_map_rgb
            (sdl_pixel_format *format, uint_8 r, uint_8 g, uint_8 b);

/*
 * Maps an RGBA quadruple to a pixel value for a given pixel format
 */
extern DECLSPEC uint_32 SDLCALL sdl_map_rgba(sdl_pixel_format *format,
                   uint_8 r, uint_8 g, uint_8 b, uint_8 a);

/*
 * Maps a pixel value into the RGB components for a given pixel format
 */
extern DECLSPEC void SDLCALL sdl_get_rgb(uint_32 pixel, sdl_pixel_format *fmt,
                uint_8 *r, uint_8 *g, uint_8 *b);

/*
 * Maps a pixel value into the RGBA components for a given pixel format
 */
extern DECLSPEC void SDLCALL sdl_get_rgba(uint_32 pixel, sdl_pixel_format *fmt,
                 uint_8 *r, uint_8 *g, uint_8 *b, uint_8 *a);

/*
 * Allocate and free an RGB surface (must be called after SDL_SetVideoMode)
 * If the depth is 4 or 8 bits, an empty palette is allocated for the surface.
 * If the depth is greater than 8 bits, the pixel format is set using the
 * flags '[RGB]mask'.
 * If the function runs out of memory, it will return NULL.
 *
 * The 'flags' tell what kind of surface to create.
 * SDL_SWSURFACE means that the surface should be created in system memory.
 * SDL_HWSURFACE means that the surface should be created in video memory,
 * with the same format as the display surface.  This is useful for surfaces
 * that will not change much, to take advantage of hardware acceleration
 * when being blitted to the display surface.
 * SDL_ASYNCBLIT means that SDL will try to perform asynchronous blits with
 * this surface, but you must always lock it before accessing the pixels.
 * SDL will wait for current blits to finish before returning from the lock.
 * SDL_SRCCOLORKEY indicates that the surface will be used for colorkey blits.
 * If the hardware supports acceleration of colorkey blits between
 * two surfaces in video memory, SDL will try to place the surface in
 * video memory. If this isn't possible or if there is no hardware
 * acceleration available, the surface will be placed in system memory.
 * SDL_SRCALPHA means that the surface will be used for alpha blits and
 * if the hardware supports hardware acceleration of alpha blits between
 * two surfaces in video memory, to place the surface in video memory
 * if possible, otherwise it will be placed in system memory.
 * If the surface is created in video memory, blits will be _much_ faster,
 * but the surface format must be identical to the video surface format,
 * and the only way to access the pixels member of the surface is to use
 * the SDL_LockSurface() and SDL_UnlockSurface() calls.
 * If the requested surface actually resides in video memory, SDL_HWSURFACE
 * will be set in the flags member of the returned surface.  If for some
 * reason the surface could not be placed in video memory, it will not have
 * the SDL_HWSURFACE flag set, and will be created in system memory instead.
 */
#define SDL_AllocSurface    SDL_CreateRGBSurface
extern DECLSPEC sdl_surface * SDLCALL sdl_create_rgbsurface
            (uint_32 flags, int width, int height, int depth,
            uint_32 rmask, uint_32 gmask, uint_32 bmask, uint_32 amask);
extern DECLSPEC sdl_surface * SDLCALL sdl_create_rgbsurface_from(void *pixels,
            int width, int height, int depth, int pitch,
            uint_32 rmask, uint_32 gmask, uint_32 bmask, uint_32 amask);
extern DECLSPEC void SDLCALL sdl_free_surface(sdl_surface *surface);

/*
 * SDL_LockSurface() sets up a surface for directly accessing the pixels.
 * Between calls to SDL_LockSurface()/SDL_UnlockSurface(), you can write
 * to and read from 'surface->pixels', using the pixel format stored in
 * 'surface->format'.  Once you are done accessing the surface, you should
 * use SDL_UnlockSurface() to release it.
 *
 * Not all surfaces require locking.  If SDL_MUSTLOCK(surface) evaluates
 * to 0, then you can read and write to the surface at any time, and the
 * pixel format of the surface will not change.  In particular, if the
 * SDL_HWSURFACE flag is not given when calling SDL_SetVideoMode(), you
 * will not need to lock the display surface before accessing it.
 *
 * No operating system or library calls should be made between lock/unlock
 * pairs, as critical system locks may be held during this time.
 *
 * SDL_LockSurface() returns 0, or -1 if the surface couldn't be locked.
 */
extern DECLSPEC int SDLCALL sdl_lock_surface(sdl_surface *surface);
extern DECLSPEC void SDLCALL sdl_unlock_surface(sdl_surface *surface);

/*
 * Load a surface from a seekable SDL data source (memory or file.)
 * If 'freesrc' is non-zero, the source will be closed after being read.
 * Returns the new surface, or NULL if there was an error.
 * The new surface should be freed with SDL_FreeSurface().
 */
extern DECLSPEC sdl_surface * SDLCALL sdl_load_bmp_rw(sdl_rwops *src, int freesrc);

/* Convenience macro -- load a surface from a file */
#define SDL_LoadBMP(file)   SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"), 1)

/*
 * Save a surface to a seekable SDL data source (memory or file.)
 * If 'freedst' is non-zero, the source will be closed after being written.
 * Returns 0 if successful or -1 if there was an error.
 */
extern DECLSPEC int SDLCALL sdl_save_bmp_rw
        (sdl_surface *surface, sdl_rwops *dst, int freedst);

/* Convenience macro -- save a surface to a file */
#define SDL_SaveBMP(surface, file) \
        sdl_save_bmp_rw(surface, sdl_rwfrom_file(file, "wb"), 1)

/*
 * Sets the color key (transparent pixel) in a blittable surface.
 * If 'flag' is SDL_SRCCOLORKEY (optionally OR'd with SDL_RLEACCEL),
 * 'key' will be the transparent pixel in the source image of a blit.
 * SDL_RLEACCEL requests RLE acceleration for the surface if present,
 * and removes RLE acceleration if absent.
 * If 'flag' is 0, this function clears any current color key.
 * This function returns 0, or -1 if there was an error.
 */
extern DECLSPEC int SDLCALL sdl_set_color_key
            (sdl_surface *surface, uint_32 flag, uint_32 key);

/*
 * This function sets the alpha value for the entire surface, as opposed to
 * using the alpha component of each pixel. This value measures the range
 * of transparency of the surface, 0 being completely transparent to 255
 * being completely opaque. An 'alpha' value of 255 causes blits to be
 * opaque, the source pixels copied to the destination (the default). Note
 * that per-surface alpha can be combined with colorkey transparency.
 *
 * If 'flag' is 0, alpha blending is disabled for the surface.
 * If 'flag' is SDL_SRCALPHA, alpha blending is enabled for the surface.
 * OR:ing the flag with SDL_RLEACCEL requests RLE acceleration for the
 * surface; if SDL_RLEACCEL is not specified, the RLE accel will be removed.
 *
 * The 'alpha' parameter is ignored for surfaces that have an alpha channel.
 */
extern DECLSPEC int SDLCALL sdl_set_alpha(sdl_surface *surface, uint_32 flag, uint_8 alpha);

/*
 * Sets the clipping rectangle for the destination surface in a blit.
 *
 * If the clip rectangle is NULL, clipping will be disabled.
 * If the clip rectangle doesn't intersect the surface, the function will
 * return SDL_FALSE and blits will be completely clipped.  Otherwise the
 * function returns SDL_TRUE and blits to the surface will be clipped to
 * the intersection of the surface area and the clipping rectangle.
 *
 * Note that blits are automatically clipped to the edges of the source
 * and destination surfaces.
 */
extern DECLSPEC sdl_bool SDLCALL sdl_set_clip_rect(sdl_surface *surface, const sdl_rect *rect);

/*
 * Gets the clipping rectangle for the destination surface in a blit.
 * 'rect' must be a pointer to a valid rectangle which will be filled
 * with the correct values.
 */
extern DECLSPEC void SDLCALL sdl_get_clip_rect(sdl_surface *surface, sdl_rect *rect);

/*
 * Creates a new surface of the specified format, and then copies and maps
 * the given surface to it so the blit of the converted surface will be as
 * fast as possible.  If this function fails, it returns NULL.
 *
 * The 'flags' parameter is passed to SDL_CreateRGBSurface() and has those
 * semantics.  You can also pass SDL_RLEACCEL in the flags parameter and
 * SDL will try to RLE accelerate colorkey and alpha blits in the resulting
 * surface.
 *
 * This function is used internally by SDL_DisplayFormat().
 */
extern DECLSPEC sdl_surface * SDLCALL sdl_convert_surface
            (sdl_surface *src, sdl_pixel_format *fmt, uint_32 flags);

/*
 * This performs a fast blit from the source surface to the destination
 * surface.  It assumes that the source and destination rectangles are
 * the same size.  If either 'srcrect' or 'dstrect' are NULL, the entire
 * surface (src or dst) is copied.  The final blit rectangles are saved
 * in 'srcrect' and 'dstrect' after all clipping is performed.
 * If the blit is successful, it returns 0, otherwise it returns -1.
 *
 * The blit function should not be called on a locked surface.
 *
 * The blit semantics for surfaces with and without alpha and colorkey
 * are defined as follows:
 *
 * RGBA->RGB:
 *     SDL_SRCALPHA set:
 *  alpha-blend (using alpha-channel).
 *  SDL_SRCCOLORKEY ignored.
 *     SDL_SRCALPHA not set:
 *  copy RGB.
 *  if SDL_SRCCOLORKEY set, only copy the pixels matching the
 *  RGB values of the source colour key, ignoring alpha in the
 *  comparison.
 *
 * RGB->RGBA:
 *     SDL_SRCALPHA set:
 *  alpha-blend (using the source per-surface alpha value);
 *  set destination alpha to opaque.
 *     SDL_SRCALPHA not set:
 *  copy RGB, set destination alpha to source per-surface alpha value.
 *     both:
 *  if SDL_SRCCOLORKEY set, only copy the pixels matching the
 *  source colour key.
 *
 * RGBA->RGBA:
 *     SDL_SRCALPHA set:
 *  alpha-blend (using the source alpha channel) the RGB values;
 *  leave destination alpha untouched. [Note: is this correct?]
 *  SDL_SRCCOLORKEY ignored.
 *     SDL_SRCALPHA not set:
 *  copy all of RGBA to the destination.
 *  if SDL_SRCCOLORKEY set, only copy the pixels matching the
 *  RGB values of the source colour key, ignoring alpha in the
 *  comparison.
 *
 * RGB->RGB:
 *     SDL_SRCALPHA set:
 *  alpha-blend (using the source per-surface alpha value).
 *     SDL_SRCALPHA not set:
 *  copy RGB.
 *     both:
 *  if SDL_SRCCOLORKEY set, only copy the pixels matching the
 *  source colour key.
 *
 * If either of the surfaces were in video memory, and the blit returns -2,
 * the video memory was lost, so it should be reloaded with artwork and
 * re-blitted:
    while ( SDL_BlitSurface(image, imgrect, screen, dstrect) == -2 )
    {
        while ( SDL_LockSurface(image) < 0 )
            Sleep(10);
        -- Write image pixels to image->pixels --
        SDL_UnlockSurface(image);
    }
 * This happens under DirectX 5.0 when the system switches away from your
 * fullscreen application.  The lock will also fail until you have access
 * to the video memory again.
 */
/* You should call SDL_BlitSurface() unless you know exactly how SDL
   blitting works internally and how to use the other blit functions.
*/
#define SDL_BlitSurface SDL_UpperBlit

/* This is the public blit function, SDL_BlitSurface(), and it performs
   rectangle validation and clipping before passing it to SDL_LowerBlit()
*/
extern DECLSPEC int SDLCALL sdl_upper_blit
            (sdl_surface *src, sdl_rect *srcrect,
             sdl_surface *dst, sdl_rect *dstrect);
/* This is a semi-private blit function and it performs low-level surface
   blitting only.
*/
extern DECLSPEC int SDLCALL sdl_lower_blit
            (sdl_surface *src, sdl_rect *srcrect,
             sdl_surface *dst, sdl_rect *dstrect);

/*
 * This function performs a fast fill of the given rectangle with 'color'
 * The given rectangle is clipped to the destination surface clip area
 * and the final fill rectangle is saved in the passed in pointer.
 * If 'dstrect' is NULL, the whole surface will be filled with 'color'
 * The color should be a pixel of the format used by the surface, and
 * can be generated by the SDL_MapRGB() function.
 * This function returns 0 on success, or -1 on error.
 */
extern DECLSPEC int SDLCALL sdl_fill_rect
        (sdl_surface *dst, sdl_rect *dstrect, uint_32 color);

/*
 * This function takes a surface and copies it to a new surface of the
 * pixel format and colors of the video framebuffer, suitable for fast
 * blitting onto the display surface.  It calls SDL_ConvertSurface()
 *
 * If you want to take advantage of hardware colorkey or alpha blit
 * acceleration, you should set the colorkey and alpha value before
 * calling this function.
 *
 * If the conversion fails or runs out of memory, it returns NULL
 */
extern DECLSPEC sdl_surface * SDLCALL sdl_display_format(sdl_surface *surface);

/*
 * This function takes a surface and copies it to a new surface of the
 * pixel format and colors of the video framebuffer (if possible),
 * suitable for fast alpha blitting onto the display surface.
 * The new surface will always have an alpha channel.
 *
 * If you want to take advantage of hardware colorkey or alpha blit
 * acceleration, you should set the colorkey and alpha value before
 * calling this function.
 *
 * If the conversion fails or runs out of memory, it returns NULL
 */
extern DECLSPEC sdl_surface * SDLCALL sdl_display_format_alpha(sdl_surface *surface);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* YUV video surface overlay functions                                       */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* This function creates a video output overlay
   Calling the returned surface an overlay is something of a misnomer because
   the contents of the display surface underneath the area where the overlay
   is shown is undefined - it may be overwritten with the converted YUV data.
*/
extern DECLSPEC sdl_overlay * SDLCALL sdl_create_yuvoverlay(int width, int height,
                uint_32 format, sdl_surface *display);

/* Lock an overlay for direct access, and unlock it when you are done */
extern DECLSPEC int SDLCALL sdl_lock_yuvoverlay(sdl_overlay *overlay);
extern DECLSPEC void SDLCALL sdl_unlock_yuvoverlay(sdl_overlay *overlay);

/* Blit a video overlay to the display surface.
   The contents of the video surface underneath the blit destination are
   not defined.
   The width and height of the destination rectangle may be different from
   that of the overlay, but currently only 2x scaling is supported.
*/
extern DECLSPEC int SDLCALL sdl_display_yuvoverlay(sdl_overlay *overlay, sdl_rect *dstrect);

/* Free a video overlay */
extern DECLSPEC void SDLCALL sdl_free_yuvoverlay(sdl_overlay *overlay);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* OpenGL support functions.                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Dynamically load a GL driver, if SDL is built with dynamic GL.
 *
 * SDL links normally with the OpenGL library on your system by default,
 * but you can compile it to dynamically load the GL driver at runtime.
 * If you do this, you need to retrieve all of the GL functions used in
 * your program from the dynamic library using SDL_GL_GetProcAddress().
 *
 * This is disabled in default builds of SDL.
 */
extern DECLSPEC int SDLCALL sdl_gl_load_library(const char *path);

/*
 * Get the address of a GL function (for extension functions)
 */
extern DECLSPEC void * SDLCALL sdl_gl_get_proc_address(const char* proc);

/*
 * Set an attribute of the OpenGL subsystem before intialization.
 */
extern DECLSPEC int SDLCALL sdl_gl_set_attribute(sdl_glattr attr, int value);

/*
 * Get an attribute of the OpenGL subsystem from the windowing
 * interface, such as glX. This is of course different from getting
 * the values from SDL's internal OpenGL subsystem, which only
 * stores the values you request before initialization.
 *
 * Developers should track the values they pass into SDL_GL_SetAttribute
 * themselves if they want to retrieve these values.
 */
extern DECLSPEC int SDLCALL sdl_gl_get_attribute(sdl_glattr attr, int* value);

/*
 * Swap the OpenGL buffers, if double-buffering is supported.
 */
extern DECLSPEC void SDLCALL sdl_gl_swap_buffers(void);

/*
 * Internal functions that should not be called unless you have read
 * and understood the source code for these functions.
 */
extern DECLSPEC void SDLCALL sdl_gl_update_rects(int numrects, sdl_rect* rects);
extern DECLSPEC void SDLCALL sdl_gl_lock(void);
extern DECLSPEC void SDLCALL sdl_gl_unlock(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* These functions allow interaction with the window manager, if any.        */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Sets/Gets the title and icon text of the display window
 */
extern DECLSPEC void SDLCALL sdl_wm_set_caption(const char *title, const char *icon);
extern DECLSPEC void SDLCALL sdl_wm_get_caption(char **title, char **icon);

/*
 * Sets the icon for the display window.
 * This function must be called before the first call to SDL_SetVideoMode().
 * It takes an icon surface, and a mask in MSB format.
 * If 'mask' is NULL, the entire icon surface will be used as the icon.
 */
extern DECLSPEC void SDLCALL sdl_wm_set_icon(sdl_surface *icon, uint_8 *mask);

/*
 * This function iconifies the window, and returns 1 if it succeeded.
 * If the function succeeds, it generates an SDL_APPACTIVE loss event.
 * This function is a noop and returns 0 in non-windowed environments.
 */
extern DECLSPEC int SDLCALL sdl_wm_iconify_window(void);

/*
 * Toggle fullscreen mode without changing the contents of the screen.
 * If the display surface does not require locking before accessing
 * the pixel information, then the memory pointers will not change.
 *
 * If this function was able to toggle fullscreen mode (change from
 * running in a window to fullscreen, or vice-versa), it will return 1.
 * If it is not implemented, or fails, it returns 0.
 *
 * The next call to SDL_SetVideoMode() will set the mode fullscreen
 * attribute based on the flags parameter - if SDL_FULLSCREEN is not
 * set, then the display will be windowed by default where supported.
 *
 * This is currently only implemented in the X11 video driver.
 */
extern DECLSPEC int SDLCALL sdl_wm_toggle_full_screen(sdl_surface *surface);

/*
 * This function allows you to set and query the input grab state of
 * the application.  It returns the new input grab state.
 */
typedef enum
{
    SDL_GRAB_QUERY = -1,
    SDL_GRAB_OFF = 0,
    SDL_GRAB_ON = 1,
    SDL_GRAB_FULLSCREEN /* Used internally */
} sdl_grab_mode;
/*
 * Grabbing means that the mouse is confined to the application window,
 * and nearly all keyboard input is passed directly to the application,
 * and not interpreted by a window manager, if any.
 */
extern DECLSPEC sdl_grab_mode SDLCALL sdl_wm_grab_input(sdl_grab_mode mode);

/* Not in public API at the moment - do not use! */
extern DECLSPEC int SDLCALL sdl_soft_stretch(sdl_surface *src, sdl_rect *srcrect,
                                    sdl_surface *dst, sdl_rect *dstrect);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_video_h */

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
 "@(#) $Id: SDL_events.h,v 1.3 2005-07-27 12:26:53 bb Exp $";
#endif

/* Include file for SDL event handling */

#ifndef _SDL_events_h
#define _SDL_events_h

#include "SDL_types.h"
#include "SDL_active.h"
#include "SDL_keyboard.h"
#include "SDL_mouse.h"
#include "SDL_joystick.h"
#include "SDL_quit.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C"
{
#endif

/* Event enumerations */
enum { SDL_NOEVENT = 0,         /* Unused (do not remove) */
       SDL_ACTIVEEVENT,         /* Application loses/gains visibility */
       SDL_KEYDOWN,         /* Keys pressed */
       SDL_KEYUP,           /* Keys released */
       SDL_MOUSEMOTION,         /* Mouse moved */
       SDL_MOUSEBUTTONDOWN,     /* Mouse button pressed */
       SDL_MOUSEBUTTONUP,       /* Mouse button released */
       SDL_JOYAXISMOTION,       /* Joystick axis motion */
       SDL_JOYBALLMOTION,       /* Joystick trackball motion */
       SDL_JOYHATMOTION,        /* Joystick hat position change */
       SDL_JOYBUTTONDOWN,       /* Joystick button pressed */
       SDL_JOYBUTTONUP,         /* Joystick button released */
       SDL_QUIT,            /* User-requested quit */
       SDL_SYSWMEVENT,          /* System specific event */
       SDL_EVENT_RESERVEDA,     /* Reserved for future use.. */
       SDL_EVENT_RESERVEDB,     /* Reserved for future use.. */
       SDL_VIDEORESIZE,         /* User resized video mode */
       SDL_VIDEOEXPOSE,         /* Screen needs to be redrawn */
       SDL_EVENT_RESERVED2,     /* Reserved for future use.. */
       SDL_EVENT_RESERVED3,     /* Reserved for future use.. */
       SDL_EVENT_RESERVED4,     /* Reserved for future use.. */
       SDL_EVENT_RESERVED5,     /* Reserved for future use.. */
       SDL_EVENT_RESERVED6,     /* Reserved for future use.. */
       SDL_EVENT_RESERVED7,     /* Reserved for future use.. */
       /* Events SDL_USEREVENT through SDL_MAXEVENTS-1 are for your use */
       SDL_USEREVENT = 24,
       /* This last event is only for bounding internal arrays
      It is the number of bits in the event mask datatype -- Uint32
        */
       SDL_NUMEVENTS = 32
};

/* Predefined event masks */
#define SDL_EVENTMASK(X)    (1<<(X))
enum
{
    SDL_ACTIVEEVENTMASK = SDL_EVENTMASK(SDL_ACTIVEEVENT),
    SDL_KEYDOWNMASK     = SDL_EVENTMASK(SDL_KEYDOWN),
    SDL_KEYUPMASK       = SDL_EVENTMASK(SDL_KEYUP),
    SDL_MOUSEMOTIONMASK = SDL_EVENTMASK(SDL_MOUSEMOTION),
    SDL_MOUSEBUTTONDOWNMASK = SDL_EVENTMASK(SDL_MOUSEBUTTONDOWN),
    SDL_MOUSEBUTTONUPMASK   = SDL_EVENTMASK(SDL_MOUSEBUTTONUP),
    SDL_MOUSEEVENTMASK  = SDL_EVENTMASK(SDL_MOUSEMOTION)|
                              SDL_EVENTMASK(SDL_MOUSEBUTTONDOWN)|
                              SDL_EVENTMASK(SDL_MOUSEBUTTONUP),
    SDL_JOYAXISMOTIONMASK   = SDL_EVENTMASK(SDL_JOYAXISMOTION),
    SDL_JOYBALLMOTIONMASK   = SDL_EVENTMASK(SDL_JOYBALLMOTION),
    SDL_JOYHATMOTIONMASK    = SDL_EVENTMASK(SDL_JOYHATMOTION),
    SDL_JOYBUTTONDOWNMASK   = SDL_EVENTMASK(SDL_JOYBUTTONDOWN),
    SDL_JOYBUTTONUPMASK = SDL_EVENTMASK(SDL_JOYBUTTONUP),
    SDL_JOYEVENTMASK    = SDL_EVENTMASK(SDL_JOYAXISMOTION)|
                              SDL_EVENTMASK(SDL_JOYBALLMOTION)|
                              SDL_EVENTMASK(SDL_JOYHATMOTION)|
                              SDL_EVENTMASK(SDL_JOYBUTTONDOWN)|
                              SDL_EVENTMASK(SDL_JOYBUTTONUP),
    SDL_VIDEORESIZEMASK = SDL_EVENTMASK(SDL_VIDEORESIZE),
    SDL_VIDEOEXPOSEMASK = SDL_EVENTMASK(SDL_VIDEOEXPOSE),
    SDL_QUITMASK        = SDL_EVENTMASK(SDL_QUIT),
    SDL_SYSWMEVENTMASK  = SDL_EVENTMASK(SDL_SYSWMEVENT)
};
#define SDL_ALLEVENTS       0xFFFFFFFF

/* Application visibility event structure */
typedef struct
{
    uint_8 type; /* SDL_ACTIVEEVENT */
    uint_8 gain; /* Whether given states were gained or lost (1/0) */
    uint_8 state;    /* A mask of the focus states */
} sdl_active_event;

/* Keyboard event structure */
typedef struct
{
    uint_8 type; /* SDL_KEYDOWN or SDL_KEYUP */
    uint_8 which;    /* The keyboard device index */
    uint_8 state;    /* SDL_PRESSED or SDL_RELEASED */
    sdl_keysym keysym;
} sdl_keyboard_event;

/* Mouse motion event structure */
typedef struct
{
    uint_8 type; /* SDL_MOUSEMOTION */
    uint_8 which;    /* The mouse device index */
    uint_8 state;    /* The current button state */
    uint_16 x, y;    /* The X/Y coordinates of the mouse */
    sint_16 xrel;    /* The relative motion in the X direction */
    sint_16 yrel;    /* The relative motion in the Y direction */
} sdl_mouse_motion_event;

/* Mouse button event structure */
typedef struct
{
    uint_8 type; /* SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP */
    uint_8 which;    /* The mouse device index */
    uint_8 button;   /* The mouse button index */
    uint_8 state;    /* SDL_PRESSED or SDL_RELEASED */
    uint_16 x, y;    /* The X/Y coordinates of the mouse at press time */
} sdl_mouse_button_event;

/* Joystick axis motion event structure */
typedef struct
{
    uint_8 type; /* SDL_JOYAXISMOTION */
    uint_8 which;    /* The joystick device index */
    uint_8 axis; /* The joystick axis index */
    sint_16 value;   /* The axis value (range: -32768 to 32767) */
} sdl_joy_axis_event;

/* Joystick trackball motion event structure */
typedef struct
{
    uint_8 type; /* SDL_JOYBALLMOTION */
    uint_8 which;    /* The joystick device index */
    uint_8 ball; /* The joystick trackball index */
    sint_16 xrel;    /* The relative motion in the X direction */
    sint_16 yrel;    /* The relative motion in the Y direction */
} sdl_joy_ball_event;

/* Joystick hat position change event structure */
typedef struct
{
    uint_8 type; /* SDL_JOYHATMOTION */
    uint_8 which;    /* The joystick device index */
    uint_8 hat;  /* The joystick hat index */
    uint_8 value;    /* The hat position value:
                SDL_HAT_LEFTUP   SDL_HAT_UP       SDL_HAT_RIGHTUP
                SDL_HAT_LEFT     SDL_HAT_CENTERED SDL_HAT_RIGHT
                SDL_HAT_LEFTDOWN SDL_HAT_DOWN     SDL_HAT_RIGHTDOWN
               Note that zero means the POV is centered.
            */
} sdl_joy_hat_event;

/* Joystick button event structure */
typedef struct
{
    uint_8 type; /* SDL_JOYBUTTONDOWN or SDL_JOYBUTTONUP */
    uint_8 which;    /* The joystick device index */
    uint_8 button;   /* The joystick button index */
    uint_8 state;    /* SDL_PRESSED or SDL_RELEASED */
} sdl_joy_button_event;

/* The "window resized" event
   When you get this event, you are responsible for setting a new video
   mode with the new width and height.
 */
typedef struct
{
    uint_8 type; /* SDL_VIDEORESIZE */
    int w;      /* New width */
    int h;      /* New height */
} sdl_resize_event;

/* The "screen redraw" event */
typedef struct
{
    uint_8 type; /* SDL_VIDEOEXPOSE */
} sdl_expose_event;

/* The "quit requested" event */
typedef struct
{
    uint_8 type; /* SDL_QUIT */
} sdl_quit_event;

/* A user-defined event type */
typedef struct
{
    uint_8 type; /* SDL_USEREVENT through SDL_NUMEVENTS-1 */
    int code;   /* User defined event code */
    void *data1;    /* User defined data pointer */
    void *data2;    /* User defined data pointer */
} sdl_user_event;

/* If you want to use this event, you should include SDL_syswm.h */
struct sdl_sys_wmmsg;
typedef struct sdl_sys_wmmsg sdl_sys_wmmsg;
typedef struct
{
    uint_8 type;
    sdl_sys_wmmsg *msg;
} sdl_sys_wmevent;

/* General event structure */
typedef union
{
    uint_8 type;
    sdl_active_event active;
    sdl_keyboard_event key;
    sdl_mouse_motion_event motion;
    sdl_mouse_button_event button;
    sdl_joy_axis_event jaxis;
    sdl_joy_ball_event jball;
    sdl_joy_hat_event jhat;
    sdl_joy_button_event jbutton;
    sdl_resize_event resize;
    sdl_expose_event expose;
    sdl_quit_event quit;
    sdl_user_event user;
    sdl_sys_wmevent syswm;
} sdl_event;


/* Function prototypes */

/* Pumps the event loop, gathering events from the input devices.
   This function updates the event queue and internal input device state.
   This should only be run in the thread that sets the video mode.
*/
extern DECLSPEC void SDLCALL sdl_pump_events(void);

/* Checks the event queue for messages and optionally returns them.
   If 'action' is SDL_ADDEVENT, up to 'numevents' events will be added to
   the back of the event queue.
   If 'action' is SDL_PEEKEVENT, up to 'numevents' events at the front
   of the event queue, matching 'mask', will be returned and will not
   be removed from the queue.
   If 'action' is SDL_GETEVENT, up to 'numevents' events at the front
   of the event queue, matching 'mask', will be returned and will be
   removed from the queue.
   This function returns the number of events actually stored, or -1
   if there was an error.  This function is thread-safe.
*/
typedef enum
{
    SDL_ADDEVENT,
    SDL_PEEKEVENT,
    SDL_GETEVENT
} sdl_eventaction;
/* */
extern DECLSPEC int SDLCALL sdl_peep_events(sdl_event *events, int numevents,
                sdl_eventaction action, uint_32 mask);

/* Polls for currently pending events, and returns 1 if there are any pending
   events, or 0 if there are none available.  If 'event' is not NULL, the next
   event is removed from the queue and stored in that area.
 */
extern DECLSPEC int SDLCALL sdl_poll_event(sdl_event *event);

/* Waits indefinitely for the next available event, returning 1, or 0 if there
   was an error while waiting for events.  If 'event' is not NULL, the next
   event is removed from the queue and stored in that area.
 */
extern DECLSPEC int SDLCALL sdl_wait_event(sdl_event *event);

/* Add an event to the event queue.
   This function returns 0 on success, or -1 if the event queue was full
   or there was some other error.
 */
extern DECLSPEC int SDLCALL sdl_push_event(sdl_event *event);

/*
  This function sets up a filter to process all events before they
  change internal state and are posted to the internal event queue.

  The filter is protypted as:
*/
typedef int (*sdl_event_filter)(const sdl_event *event);
/*
  If the filter returns 1, then the event will be added to the internal queue.
  If it returns 0, then the event will be dropped from the queue, but the
  internal state will still be updated.  This allows selective filtering of
  dynamically arriving events.

  WARNING:  Be very careful of what you do in the event filter function, as
            it may run in a different thread!

  There is one caveat when dealing with the SDL_QUITEVENT event type.  The
  event filter is only called when the window manager desires to close the
  application window.  If the event filter returns 1, then the window will
  be closed, otherwise the window will remain open if possible.
  If the quit event is generated by an interrupt signal, it will bypass the
  internal queue and be delivered to the application at the next event poll.
*/
extern DECLSPEC void SDLCALL sdl_set_event_filter(sdl_event_filter filter);

/*
  Return the current event filter - can be used to "chain" filters.
  If there is no event filter set, this function returns NULL.
*/
extern DECLSPEC sdl_event_filter SDLCALL sdl_get_event_filter(void);

/*
  This function allows you to set the state of processing certain events.
  If 'state' is set to SDL_IGNORE, that event will be automatically dropped
  from the event queue and will not event be filtered.
  If 'state' is set to SDL_ENABLE, that event will be processed normally.
  If 'state' is set to SDL_QUERY, SDL_EventState() will return the
  current processing state of the specified event.
*/
#define SDL_QUERY   -1
#define SDL_IGNORE   0
#define SDL_DISABLE  0
#define SDL_ENABLE   1
extern DECLSPEC uint_8 SDLCALL sdl_event_state(uint_8 type, int state);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_events_h */

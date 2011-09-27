/* ========================================================================================== */
/* FMOD Main header file. Copyright (c), Firelight Technologies Pty, Ltd 1999-2003.           */
/* ========================================================================================== */

#ifndef _FMOD_H_
#define _FMOD_H_

/* ========================================================================================== */
/* DEFINITIONS                                                                                */
/* ========================================================================================== */

#if (!defined(WIN32) && !defined(__WIN32__) && !defined(_WIN32_WCE) && !defined(_XBOX)) || (defined(__GNUC__) && defined(WIN32))
    #ifndef __cdecl
        #define __cdecl
    #endif
    #ifndef __stdcall
        #define __stdcall
    #endif
#endif
#if defined(_WIN32_WCE)
    #define F_API _stdcall
    #define F_CALLBACKAPI _cdecl
#else
    #define F_API __stdcall
    #define F_CALLBACKAPI __cdecl
#endif

#ifdef DLL_EXPORTS
    #define DLL_API __declspec(dllexport)
#else
    #if defined(__LCC__) || defined(__MINGW32__) || defined(__CYGWIN32__)
        #define DLL_API F_API
    #else
        #define DLL_API
    #endif /* __LCC__ ||  __MINGW32__ || __CYGWIN32__ */
#endif /* DLL_EXPORTS */

#define FMOD_VERSION    3.63f

/*
    FMOD defined types
*/
typedef struct FSOUND_SAMPLE    FSOUND_SAMPLE;
typedef struct FSOUND_STREAM    FSOUND_STREAM;
typedef struct FSOUND_DSPUNIT   FSOUND_DSPUNIT;
typedef struct FMUSIC_MODULE    FMUSIC_MODULE;

/*
    Callback types
*/
typedef void *      (F_CALLBACKAPI *FSOUND_DSPCALLBACK)    (void *originalbuffer, void *newbuffer, int length, int param);
typedef signed char (F_CALLBACKAPI *FSOUND_STREAMCALLBACK) (FSOUND_STREAM *stream, void *buff, int len, int param);
typedef void        (F_CALLBACKAPI *FMUSIC_CALLBACK)       (FMUSIC_MODULE *mod, unsigned char param);

typedef unsigned int(F_CALLBACKAPI *FSOUND_OPENCALLBACK)   (const char *name);
typedef void        (F_CALLBACKAPI *FSOUND_CLOSECALLBACK)  (unsigned int handle);
typedef int         (F_CALLBACKAPI *FSOUND_READCALLBACK)   (void *buffer, int size, unsigned int handle);
typedef int         (F_CALLBACKAPI *FSOUND_SEEKCALLBACK)   (unsigned int handle, int pos, signed char mode);
typedef int         (F_CALLBACKAPI *FSOUND_TELLCALLBACK)   (unsigned int handle);

typedef void *      (F_CALLBACKAPI *FSOUND_ALLOCCALLBACK)  (unsigned int size);
typedef void *      (F_CALLBACKAPI *FSOUND_REALLOCCALLBACK)(void *ptr, unsigned int size);
typedef void        (F_CALLBACKAPI *FSOUND_FREECALLBACK)   (void *ptr);


/*
[ENUM]
[
    [DESCRIPTION]
    On failure of commands in FMOD, use FSOUND_GetError to attain what happened.

    [SEE_ALSO]
    FSOUND_GetError
]
*/
enum FMOD_ERRORS
{
    FMOD_ERR_NONE,             /* No errors */
    FMOD_ERR_BUSY,             /* Cannot call this command after FSOUND_Init.  Call FSOUND_Close first. */
    FMOD_ERR_UNINITIALIZED,    /* This command failed because FSOUND_Init or FSOUND_SetOutput was not called */
    FMOD_ERR_INIT,             /* Error initializing output device. */
    FMOD_ERR_ALLOCATED,        /* Error initializing output device, but more specifically, the output device is already in use and cannot be reused. */
    FMOD_ERR_PLAY,             /* Playing the sound failed. */
    FMOD_ERR_OUTPUT_FORMAT,    /* Soundcard does not support the features needed for this soundsystem (16bit stereo output) */
    FMOD_ERR_COOPERATIVELEVEL, /* Error setting cooperative level for hardware. */
    FMOD_ERR_CREATEBUFFER,     /* Error creating hardware sound buffer. */
    FMOD_ERR_FILE_NOTFOUND,    /* File not found */
    FMOD_ERR_FILE_FORMAT,      /* Unknown file format */
    FMOD_ERR_FILE_BAD,         /* Error loading file */
    FMOD_ERR_MEMORY,           /* Not enough memory or resources */
    FMOD_ERR_VERSION,          /* The version number of this file format is not supported */
    FMOD_ERR_INVALID_PARAM,    /* An invalid parameter was passed to this function */
    FMOD_ERR_NO_EAX,           /* Tried to use an EAX command on a non EAX enabled channel or output. */
    FMOD_ERR_CHANNEL_ALLOC,    /* Failed to allocate a new channel */
    FMOD_ERR_RECORD,           /* Recording is not supported on this machine */
    FMOD_ERR_MEDIAPLAYER,      /* Windows Media Player not installed so cannot play wma or use internet streaming. */
    FMOD_ERR_CDDEVICE          /* An error occured trying to open the specified CD device */
};


/*
[ENUM]
[
    [DESCRIPTION]
    These output types are used with FSOUND_SetOutput, to choose which output driver to use.

    FSOUND_OUTPUT_DSOUND will not support hardware 3d acceleration if the sound card driver
    does not support DirectX 6 Voice Manager Extensions.

    FSOUND_OUTPUT_WINMM is recommended for NT and CE.

    [SEE_ALSO]
    FSOUND_SetOutput
    FSOUND_GetOutput
]
*/
enum FSOUND_OUTPUTTYPES
{
    FSOUND_OUTPUT_NOSOUND,    /* NoSound driver, all calls to this succeed but do nothing. */
    FSOUND_OUTPUT_WINMM,      /* Windows Multimedia driver. */
    FSOUND_OUTPUT_DSOUND,     /* DirectSound driver.  You need this to get EAX2 or EAX3 support, or FX api support. */
    FSOUND_OUTPUT_A3D,        /* A3D driver. */

    FSOUND_OUTPUT_OSS,        /* Linux/Unix OSS (Open Sound System) driver, i.e. the kernel sound drivers. */
    FSOUND_OUTPUT_ESD,        /* Linux/Unix ESD (Enlightment Sound Daemon) driver. */
    FSOUND_OUTPUT_ALSA,       /* Linux Alsa driver. */

    FSOUND_OUTPUT_ASIO,       /* Low latency ASIO driver */
    FSOUND_OUTPUT_XBOX,       /* Xbox driver */
    FSOUND_OUTPUT_PS2,        /* PlayStation 2 driver */
    FSOUND_OUTPUT_MAC,        /* Mac SoundManager driver */
    FSOUND_OUTPUT_GC,         /* Gamecube driver */

    FSOUND_OUTPUT_NOSOUND_NONREALTIME   /* This is the same as nosound, but the sound generation is driven by FSOUND_Update */
};


/*
[ENUM]
[
    [DESCRIPTION]
    These mixer types are used with FSOUND_SetMixer, to choose which mixer to use, or to act
    upon for other reasons using FSOUND_GetMixer.
    It is not nescessary to set the mixer.  FMOD will autodetect the best mixer for you.

    [SEE_ALSO]
    FSOUND_SetMixer
    FSOUND_GetMixer
]
*/
enum FSOUND_MIXERTYPES
{
    FSOUND_MIXER_AUTODETECT,        /* CE/PS2/GC Only - Non interpolating/low quality mixer. */
    FSOUND_MIXER_BLENDMODE,         /* Removed / obsolete. */
    FSOUND_MIXER_MMXP5,             /* Removed / obsolete. */
    FSOUND_MIXER_MMXP6,             /* Removed / obsolete. */

    FSOUND_MIXER_QUALITY_AUTODETECT,/* All platforms - Autodetect the fastest quality mixer based on your cpu. */
    FSOUND_MIXER_QUALITY_FPU,       /* Win32/Linux only - Interpolating/volume ramping FPU mixer.  */
    FSOUND_MIXER_QUALITY_MMXP5,     /* Win32/Linux only - Interpolating/volume ramping P5 MMX mixer.  */
    FSOUND_MIXER_QUALITY_MMXP6,     /* Win32/Linux only - Interpolating/volume ramping ppro+ MMX mixer. */

    FSOUND_MIXER_MONO,              /* CE/PS2/GC only - MONO non interpolating/low quality mixer. For speed*/
    FSOUND_MIXER_QUALITY_MONO,      /* CE/PS2/GC only - MONO Interpolating mixer.  For speed */

    FSOUND_MIXER_MAX
};


/*
[ENUM]
[
    [DESCRIPTION]
    These definitions describe the type of song being played.

    [SEE_ALSO]
    FMUSIC_GetType
]
*/
enum FMUSIC_TYPES
{
    FMUSIC_TYPE_NONE,
    FMUSIC_TYPE_MOD,        /* Protracker / Fasttracker */
    FMUSIC_TYPE_S3M,        /* ScreamTracker 3 */
    FMUSIC_TYPE_XM,         /* FastTracker 2 */
    FMUSIC_TYPE_IT,         /* Impulse Tracker. */
    FMUSIC_TYPE_MIDI,       /* MIDI file */
    FMUSIC_TYPE_FSB         /* FMOD Sample Bank file */
};


/*
[DEFINE_START]
[
    [NAME]
    FSOUND_DSP_PRIORITIES

    [DESCRIPTION]
    These default priorities are used by FMOD internal system DSP units.  They describe the
    position of the DSP chain, and the order of how audio processing is executed.
    You can actually through the use of FSOUND_DSP_GetxxxUnit (where xxx is the name of the DSP
    unit), disable or even change the priority of a DSP unit.

    [SEE_ALSO]
    FSOUND_DSP_Create
    FSOUND_DSP_SetPriority
    FSOUND_DSP_GetSpectrum
]
*/
#define FSOUND_DSP_DEFAULTPRIORITY_CLEARUNIT        0       /* DSP CLEAR unit - done first */
#define FSOUND_DSP_DEFAULTPRIORITY_SFXUNIT          100     /* DSP SFX unit - done second */
#define FSOUND_DSP_DEFAULTPRIORITY_MUSICUNIT        200     /* DSP MUSIC unit - done third */
#define FSOUND_DSP_DEFAULTPRIORITY_USER             300     /* User priority, use this as reference for your own DSP units */
#define FSOUND_DSP_DEFAULTPRIORITY_FFTUNIT          900     /* This reads data for FSOUND_DSP_GetSpectrum, so it comes after user units */
#define FSOUND_DSP_DEFAULTPRIORITY_CLIPANDCOPYUNIT  1000    /* DSP CLIP AND COPY unit - last */
/* [DEFINE_END] */


/*
[DEFINE_START]
[
    [NAME]
    FSOUND_CAPS

    [DESCRIPTION]
    Driver description bitfields.  Use FSOUND_Driver_GetCaps to determine if a driver enumerated
    has the settings you are after.  The enumerated driver depends on the output mode, see
    FSOUND_OUTPUTTYPES

    [SEE_ALSO]
    FSOUND_GetDriverCaps
    FSOUND_OUTPUTTYPES
]
*/
#define FSOUND_CAPS_HARDWARE                0x1     /* This driver supports hardware accelerated 3d sound. */
#define FSOUND_CAPS_EAX2                    0x2     /* This driver supports EAX 2 reverb */
#define FSOUND_CAPS_EAX3                    0x10    /* This driver supports EAX 3 reverb */
/* [DEFINE_END] */


/*
[DEFINE_START]
[
    [NAME]
    FSOUND_MODES

    [DESCRIPTION]
    Sample description bitfields, OR them together for loading and describing samples.
    NOTE.  If the file format being loaded already has a defined format, such as WAV or MP3, then
    trying to override the pre-defined format with a new set of format flags will not work.  For
    example, an 8 bit WAV file will not load as 16bit if you specify FSOUND_16BITS.  It will just
    ignore the flag and go ahead loading it as 8bits.  For these type of formats the only flags
    you can specify that will really alter the behaviour of how it is loaded, are the following.

    FSOUND_LOOP_OFF
    FSOUND_LOOP_NORMAL
    FSOUND_LOOP_BIDI
    FSOUND_HW3D
    FSOUND_2D
    FSOUND_STREAMABLE
    FSOUND_LOADMEMORY
    FSOUND_LOADRAW
    FSOUND_MPEGACCURATE

    See flag descriptions for what these do.
]
*/
#define FSOUND_LOOP_OFF      0x00000001  /* For non looping samples. */
#define FSOUND_LOOP_NORMAL   0x00000002  /* For forward looping samples. */
#define FSOUND_LOOP_BIDI     0x00000004  /* For bidirectional looping samples.  (no effect if in hardware). */
#define FSOUND_8BITS         0x00000008  /* For 8 bit samples. */
#define FSOUND_16BITS        0x00000010  /* For 16 bit samples. */
#define FSOUND_MONO          0x00000020  /* For mono samples. */
#define FSOUND_STEREO        0x00000040  /* For stereo samples. */
#define FSOUND_UNSIGNED      0x00000080  /* For user created source data containing unsigned samples. */
#define FSOUND_SIGNED        0x00000100  /* For user created source data containing signed data. */
#define FSOUND_DELTA         0x00000200  /* For user created source data stored as delta values. */
#define FSOUND_IT214         0x00000400  /* For user created source data stored using IT214 compression. */
#define FSOUND_IT215         0x00000800  /* For user created source data stored using IT215 compression. */
#define FSOUND_HW3D          0x00001000  /* Attempts to make samples use 3d hardware acceleration. (if the card supports it) */
#define FSOUND_2D            0x00002000  /* Tells software (not hardware) based sample not to be included in 3d processing. */
#define FSOUND_STREAMABLE    0x00004000  /* For a streamimg sound where you feed the data to it. */
#define FSOUND_LOADMEMORY    0x00008000  /* "name" will be interpreted as a pointer to data for streaming and samples. */
#define FSOUND_LOADRAW       0x00010000  /* Will ignore file format and treat as raw pcm. */
#define FSOUND_MPEGACCURATE  0x00020000  /* For FSOUND_Stream_OpenFile - for accurate FSOUND_Stream_GetLengthMs/FSOUND_Stream_SetTime.  WARNING, see FSOUND_Stream_OpenFile for inital opening time performance issues. */
#define FSOUND_FORCEMONO     0x00040000  /* For forcing stereo streams and samples to be mono - needed if using FSOUND_HW3D and stereo data - incurs a small speed hit for streams */
#define FSOUND_HW2D          0x00080000  /* 2D hardware sounds.  allows hardware specific effects */
#define FSOUND_ENABLEFX      0x00100000  /* Allows DX8 FX to be played back on a sound.  Requires DirectX 8 - Note these sounds cannot be played more than once, be 8 bit, be less than a certain size, or have a changing frequency */
#define FSOUND_MPEGHALFRATE  0x00200000  /* For FMODCE only - decodes mpeg streams using a lower quality decode, but faster execution */
#define FSOUND_XADPCM        0x00400000  /* For XBOX only - Contents are compressed as XADPCM  */
#define FSOUND_VAG           0x00800000  /* For PS2 only - Contents are compressed as Sony VAG format */
#define FSOUND_NONBLOCKING   0x01000000  /* For FSOUND_Stream_OpenFile - Causes stream to open in the background and not block the foreground app - stream functions only work when ready.  Poll any stream function determine when it IS ready. */
#define FSOUND_GCADPCM       0x02000000  /* For Gamecube only - Contents are compressed as Gamecube DSP-ADPCM format */
#define FSOUND_MULTICHANNEL  0x04000000  /* For PS2 and Gamecube only - Contents are interleaved into a multi-channel (more than stereo) format */
#define FSOUND_USECORE0      0x08000000  /* For PS2 only - Sample/Stream is forced to use hardware voices 00-23 */
#define FSOUND_USECORE1      0x10000000  /* For PS2 only - Sample/Stream is forced to use hardware voices 24-47 */
#define FSOUND_LOADMEMORYIOP 0x20000000  /* For PS2 only - "name" will be interpreted as a pointer to data for streaming and samples.  The address provided will be an IOP address */


#define FSOUND_NORMAL       (FSOUND_16BITS | FSOUND_SIGNED | FSOUND_MONO)
/* [DEFINE_END] */



/*
[DEFINE_START]
[
    [NAME]
    FSOUND_CDPLAYMODES

    [DESCRIPTION]
    Playback method for a CD Audio track, with FSOUND_CD_SetPlayMode

    [SEE_ALSO]
    FSOUND_CD_SetPlayMode
    FSOUND_CD_Play
]
*/
#define FSOUND_CD_PLAYCONTINUOUS    0   /* Starts from the current track and plays to end of CD. */
#define FSOUND_CD_PLAYONCE          1   /* Plays the specified track then stops. */
#define FSOUND_CD_PLAYLOOPED        2   /* Plays the specified track looped, forever until stopped manually. */
#define FSOUND_CD_PLAYRANDOM        3   /* Plays tracks in random order */
/* [DEFINE_END] */


/*
[DEFINE_START]
[
    [NAME]
    FSOUND_MISC_VALUES

    [DESCRIPTION]
    Miscellaneous values for FMOD functions.

    [SEE_ALSO]
    FSOUND_PlaySound
    FSOUND_PlaySoundEx
    FSOUND_Sample_Alloc
    FSOUND_Sample_Load
    FSOUND_SetPan
]
*/
#define FSOUND_FREE             -1      /* value to play on any free channel, or to allocate a sample in a free sample slot. */
#define FSOUND_UNMANAGED        -2      /* value to allocate a sample that is NOT managed by FSOUND or placed in a sample slot. */
#define FSOUND_ALL              -3      /* for a channel index , this flag will affect ALL channels available!  Not supported by every function. */
#define FSOUND_STEREOPAN        -1      /* value for FSOUND_SetPan so that stereo sounds are not played at half volume.  See FSOUND_SetPan for more on this. */
#define FSOUND_SYSTEMCHANNEL    -1000   /* special 'channel' ID for all channel based functions that want to alter the global FSOUND software mixing output channel */
#define FSOUND_SYSTEMSAMPLE     -1000   /* special 'sample' ID for all sample based functions that want to alter the global FSOUND software mixing output sample */

/* [DEFINE_END] */


/*
[STRUCTURE]
[
    [DESCRIPTION]
    Structure defining a reverb environment.
    For more indepth descriptions of the reverb properties under win32, please see the EAX2 and EAX3
    documentation at http://developer.creative.com/ under the 'downloads' section.
    If they do not have the EAX3 documentation, then most information can be attained from
    the EAX2 documentation, as EAX3 only adds some more parameters and functionality on top of
    EAX2.
    Note the default reverb properties are the same as the FSOUND_PRESET_GENERIC preset.
    Note that integer values that typically range from -10,000 to 1000 are represented in
    decibels, and are of a logarithmic scale, not linear, wheras float values are typically linear.
    PORTABILITY: Each member has the platform it supports in braces ie (win32/xbox).
    Some reverb parameters are only supported in win32 and some only on xbox. If all parameters are set then
    the reverb should product a similar effect on either platform.
    Only Win32 supports the reverb api.

    The numerical values listed below are the maximum, minimum and default values for each variable respectively.

    [SEE_ALSO]
    FSOUND_Reverb_SetProperties
    FSOUND_Reverb_GetProperties
    FSOUND_REVERB_PRESETS
    FSOUND_REVERB_FLAGS
]
*/
typedef struct _FSOUND_REVERB_PROPERTIES /* MIN     MAX    DEFAULT   DESCRIPTION */
{
    unsigned int environment;            /* 0     , 25    , 0      , sets all listener properties (win32/ps2 only) */
    float        env_size;                /* 1.0   , 100.0 , 7.5    , environment size in meters (win32 only) */
    float        env_diffusion;           /* 0.0   , 1.0   , 1.0    , environment diffusion (win32/xbox) */
    int          room;                   /* -10000, 0     , -1000  , room effect level (at mid frequencies) (win32/xbox/ps2) */
    int          room_hf;                 /* -10000, 0     , -100   , relative room effect level at high frequencies (win32/xbox) */
    int          room_lf;                 /* -10000, 0     , 0      , relative room effect level at low frequencies (win32 only) */
    float        decay_time;              /* 0.1   , 20.0  , 1.49   , reverberation decay time at mid frequencies (win32/xbox) */
    float        decay_hfratio;           /* 0.1   , 2.0   , 0.83   , high-frequency to mid-frequency decay time ratio (win32/xbox) */
    float        decay_lfratio;           /* 0.1   , 2.0   , 1.0    , low-frequency to mid-frequency decay time ratio (win32 only) */
    int          reflections;            /* -10000, 1000  , -2602  , early reflections level relative to room effect (win32/xbox) */
    float        reflections_delay;       /* 0.0   , 0.3   , 0.007  , initial reflection delay time (win32/xbox) */
    float        reflections_pan[3];      /*       ,       , [0,0,0], early reflections panning vector (win32 only) */
    int          reverb;                 /* -10000, 2000  , 200    , late reverberation level relative to room effect (win32/xbox) */
    float        reverb_delay;            /* 0.0   , 0.1   , 0.011  , late reverberation delay time relative to initial reflection (win32/xbox) */
    float        reverb_pan[3];           /*       ,       , [0,0,0], late reverberation panning vector (win32 only) */
    float        echo_time;               /* .075  , 0.25  , 0.25   , echo time (win32 only) */
    float        echo_depth;              /* 0.0   , 1.0   , 0.0    , echo depth (win32 only) */
    float        modulation_time;         /* 0.04  , 4.0   , 0.25   , modulation time (win32 only) */
    float        modulation_depth;        /* 0.0   , 1.0   , 0.0    , modulation depth (win32 only) */
    float        air_absorption_hf;        /* -100  , 0.0   , -5.0   , change in level per meter at high frequencies (win32 only) */
    float        hfreference;            /* 1000.0, 20000 , 5000.0 , reference high frequency (hz) (win32/xbox) */
    float        lfreference;            /* 20.0  , 1000.0, 250.0  , reference low frequency (hz) (win32 only) */
    float        room_rolloff_factor;      /* 0.0   , 10.0  , 0.0    , like FSOUND_3D_SetRolloffFactor but for room effect (win32/xbox) */
    float        diffusion;              /* 0.0   , 100.0 , 100.0  , Value that controls the echo density in the late reverberation decay. (xbox only) */
    float        density;                /* 0.0   , 100.0 , 100.0  , Value that controls the modal density in the late reverberation decay (xbox only) */
    unsigned int flags;                  /* FSOUND_REVERB_FLAGS - modifies the behavior of above properties (win32/ps2 only) */
} FSOUND_REVERB_PROPERTIES;


/*
[DEFINE_START]
[
    [NAME]
    FSOUND_REVERB_FLAGS

    [DESCRIPTION]
    Values for the Flags member of the FSOUND_REVERB_PROPERTIES structure.

    [SEE_ALSO]
    FSOUND_REVERB_PROPERTIES
]
*/
#define FSOUND_REVERB_FLAGS_DECAYTIMESCALE        0x00000001 /* 'EnvSize' affects reverberation decay time */
#define FSOUND_REVERB_FLAGS_REFLECTIONSSCALE      0x00000002 /* 'EnvSize' affects reflection level */
#define FSOUND_REVERB_FLAGS_REFLECTIONSDELAYSCALE 0x00000004 /* 'EnvSize' affects initial reflection delay time */
#define FSOUND_REVERB_FLAGS_REVERBSCALE           0x00000008 /* 'EnvSize' affects reflections level */
#define FSOUND_REVERB_FLAGS_REVERBDELAYSCALE      0x00000010 /* 'EnvSize' affects late reverberation delay time */
#define FSOUND_REVERB_FLAGS_DECAYHFLIMIT          0x00000020 /* AirAbsorptionHF affects DecayHFRatio */
#define FSOUND_REVERB_FLAGS_ECHOTIMESCALE         0x00000040 /* 'EnvSize' affects echo time */
#define FSOUND_REVERB_FLAGS_MODULATIONTIMESCALE   0x00000080 /* 'EnvSize' affects modulation time */
#define FSOUND_REVERB_FLAGS_CORE0                 0x00000100 /* PS2 Only - Reverb is applied to CORE0 (hw voices 0-23) */
#define FSOUND_REVERB_FLAGS_CORE1                 0x00000200 /* PS2 Only - Reverb is applied to CORE1 (hw voices 24-47) */
#define FSOUND_REVERB_FLAGS_DEFAULT              (FSOUND_REVERB_FLAGS_DECAYTIMESCALE |        \
                                                  FSOUND_REVERB_FLAGS_REFLECTIONSSCALE |      \
                                                  FSOUND_REVERB_FLAGS_REFLECTIONSDELAYSCALE | \
                                                  FSOUND_REVERB_FLAGS_REVERBSCALE |           \
                                                  FSOUND_REVERB_FLAGS_REVERBDELAYSCALE |      \
                                                  FSOUND_REVERB_FLAGS_DECAYHFLIMIT |          \
                                                  FSOUND_REVERB_FLAGS_CORE0 |                 \
                                                  FSOUND_REVERB_FLAGS_CORE1 )
/* [DEFINE_END] */




/*
[DEFINE_START]
[
    [NAME]
    FSOUND_REVERB_PRESETS

    [DESCRIPTION]
    A set of predefined environment PARAMETERS, created by Creative Labs
    These are used to initialize an FSOUND_REVERB_PROPERTIES structure statically.
    ie
    FSOUND_REVERB_PROPERTIES prop = FSOUND_PRESET_GENERIC;

    [SEE_ALSO]
    FSOUND_Reverb_SetProperties
]
*/
/*                                     Env  Size    Diffus  Room   RoomHF  RmLF DecTm   DecHF  DecLF   Refl  RefDel  RefPan               Revb  RevDel  ReverbPan           EchoTm  EchDp  ModTm  ModDp  AirAbs  HFRef    LFRef  RRlOff Diffus  Densty  FLAGS */
#define FSOUND_PRESET_OFF              {0,  7.5f,   1.00f, -10000, -10000, 0,   1.00f,  1.00f, 1.0f,  -2602, 0.007f, { 0.0f,0.0f,0.0f },   200, 0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f,   0.0f,   0.0f, 0x33f }
#define FSOUND_PRESET_GENERIC          {0,  7.5f,   1.00f, -1000,  -100,   0,   1.49f,  0.83f, 1.0f,  -2602, 0.007f, { 0.0f,0.0f,0.0f },   200, 0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_PADDEDCELL       {1,  1.4f,   1.00f, -1000,  -6000,  0,   0.17f,  0.10f, 1.0f,  -1204, 0.001f, { 0.0f,0.0f,0.0f },   207, 0.002f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_ROOM             {2,  1.9f,   1.00f, -1000,  -454,   0,   0.40f,  0.83f, 1.0f,  -1646, 0.002f, { 0.0f,0.0f,0.0f },    53, 0.003f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_BATHROOM         {3,  1.4f,   1.00f, -1000,  -1200,  0,   1.49f,  0.54f, 1.0f,   -370, 0.007f, { 0.0f,0.0f,0.0f },  1030, 0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f,  60.0f, 0x3f }
#define FSOUND_PRESET_LIVINGROOM       {4,  2.5f,   1.00f, -1000,  -6000,  0,   0.50f,  0.10f, 1.0f,  -1376, 0.003f, { 0.0f,0.0f,0.0f }, -1104, 0.004f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_STONEROOM        {5,  11.6f,  1.00f, -1000,  -300,   0,   2.31f,  0.64f, 1.0f,   -711, 0.012f, { 0.0f,0.0f,0.0f },    83, 0.017f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_AUDITORIUM       {6,  21.6f,  1.00f, -1000,  -476,   0,   4.32f,  0.59f, 1.0f,   -789, 0.020f, { 0.0f,0.0f,0.0f },  -289, 0.030f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_CONCERTHALL      {7,  19.6f,  1.00f, -1000,  -500,   0,   3.92f,  0.70f, 1.0f,  -1230, 0.020f, { 0.0f,0.0f,0.0f },    -2, 0.029f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_CAVE             {8,  14.6f,  1.00f, -1000,  0,      0,   2.91f,  1.30f, 1.0f,   -602, 0.015f, { 0.0f,0.0f,0.0f },  -302, 0.022f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x1f }
#define FSOUND_PRESET_ARENA            {9,  36.2f,  1.00f, -1000,  -698,   0,   7.24f,  0.33f, 1.0f,  -1166, 0.020f, { 0.0f,0.0f,0.0f },    16, 0.030f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_HANGAR           {10, 50.3f,  1.00f, -1000,  -1000,  0,   10.05f, 0.23f, 1.0f,   -602, 0.020f, { 0.0f,0.0f,0.0f },   198, 0.030f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_CARPETTEDHALLWAY {11, 1.9f,   1.00f, -1000,  -4000,  0,   0.30f,  0.10f, 1.0f,  -1831, 0.002f, { 0.0f,0.0f,0.0f }, -1630, 0.030f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_HALLWAY          {12, 1.8f,   1.00f, -1000,  -300,   0,   1.49f,  0.59f, 1.0f,  -1219, 0.007f, { 0.0f,0.0f,0.0f },   441, 0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_STONECORRIDOR    {13, 13.5f,  1.00f, -1000,  -237,   0,   2.70f,  0.79f, 1.0f,  -1214, 0.013f, { 0.0f,0.0f,0.0f },   395, 0.020f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_ALLEY            {14, 7.5f,   0.30f, -1000,  -270,   0,   1.49f,  0.86f, 1.0f,  -1204, 0.007f, { 0.0f,0.0f,0.0f },    -4, 0.011f, { 0.0f,0.0f,0.0f }, 0.125f, 0.95f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_FOREST           {15, 38.0f,  0.30f, -1000,  -3300,  0,   1.49f,  0.54f, 1.0f,  -2560, 0.162f, { 0.0f,0.0f,0.0f },  -229, 0.088f, { 0.0f,0.0f,0.0f }, 0.125f, 1.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f,  79.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_CITY             {16, 7.5f,   0.50f, -1000,  -800,   0,   1.49f,  0.67f, 1.0f,  -2273, 0.007f, { 0.0f,0.0f,0.0f }, -1691, 0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f,  50.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_MOUNTAINS        {17, 100.0f, 0.27f, -1000,  -2500,  0,   1.49f,  0.21f, 1.0f,  -2780, 0.300f, { 0.0f,0.0f,0.0f }, -1434, 0.100f, { 0.0f,0.0f,0.0f }, 0.250f, 1.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f,  27.0f, 100.0f, 0x1f }
#define FSOUND_PRESET_QUARRY           {18, 17.5f,  1.00f, -1000,  -1000,  0,   1.49f,  0.83f, 1.0f, -10000, 0.061f, { 0.0f,0.0f,0.0f },   500, 0.025f, { 0.0f,0.0f,0.0f }, 0.125f, 0.70f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_PLAIN            {19, 42.5f,  0.21f, -1000,  -2000,  0,   1.49f,  0.50f, 1.0f,  -2466, 0.179f, { 0.0f,0.0f,0.0f }, -1926, 0.100f, { 0.0f,0.0f,0.0f }, 0.250f, 1.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f,  21.0f, 100.0f, 0x3f }
#define FSOUND_PRESET_PARKINGLOT       {20, 8.3f,   1.00f, -1000,  0,      0,   1.65f,  1.50f, 1.0f,  -1363, 0.008f, { 0.0f,0.0f,0.0f }, -1153, 0.012f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x1f }
#define FSOUND_PRESET_SEWERPIPE        {21, 1.7f,   0.80f, -1000,  -1000,  0,   2.81f,  0.14f, 1.0f,    429, 0.014f, { 0.0f,0.0f,0.0f },  1023, 0.021f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f,  80.0f,  60.0f, 0x3f }
#define FSOUND_PRESET_UNDERWATER       {22, 1.8f,   1.00f, -1000,  -4000,  0,   1.49f,  0.10f, 1.0f,   -449, 0.007f, { 0.0f,0.0f,0.0f },  1700, 0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 1.18f, 0.348f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x3f }

/* Non I3DL2 presets */

#define FSOUND_PRESET_DRUGGED          {23, 1.9f,   0.50f, -1000,  0,      0,   8.39f,  1.39f, 1.0f,  -115,  0.002f, { 0.0f,0.0f,0.0f },   985, 0.030f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 0.25f, 1.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x1f }
#define FSOUND_PRESET_DIZZY            {24, 1.8f,   0.60f, -1000,  -400,   0,   17.23f, 0.56f, 1.0f,  -1713, 0.020f, { 0.0f,0.0f,0.0f },  -613, 0.030f, { 0.0f,0.0f,0.0f }, 0.250f, 1.00f, 0.81f, 0.310f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x1f }
#define FSOUND_PRESET_PSYCHOTIC        {25, 1.0f,   0.50f, -1000,  -151,   0,   7.56f,  0.91f, 1.0f,  -626,  0.020f, { 0.0f,0.0f,0.0f },   774, 0.030f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f, 4.00f, 1.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 100.0f, 100.0f, 0x1f }

/* PlayStation 2 Only presets */

#define FSOUND_PRESET_PS2_ROOM         {1,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }
#define FSOUND_PRESET_PS2_STUDIO_A     {2,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }
#define FSOUND_PRESET_PS2_STUDIO_B     {3,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }
#define FSOUND_PRESET_PS2_STUDIO_C     {4,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }
#define FSOUND_PRESET_PS2_HALL         {5,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }
#define FSOUND_PRESET_PS2_SPACE        {6,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }
#define FSOUND_PRESET_PS2_ECHO         {7,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }
#define FSOUND_PRESET_PS2_DELAY        {8,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }
#define FSOUND_PRESET_PS2_PIPE         {9,  0,      0,         0,  0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f, { 0.0f,0.0f,0.0f },     0, 0.000f, { 0.0f,0.0f,0.0f }, 0.000f, 0.00f, 0.00f, 0.000f,  0.0f, 0000.0f,   0.0f, 0.0f,   0.0f,   0.0f, 0x31f }

/* [DEFINE_END] */


/*
[STRUCTURE]
[
    [DESCRIPTION]
    Structure defining the properties for a reverb source, related to a FSOUND channel.
    For more indepth descriptions of the reverb properties under win32, please see the EAX3
    documentation at http://developer.creative.com/ under the 'downloads' section.
    If they do not have the EAX3 documentation, then most information can be attained from
    the EAX2 documentation, as EAX3 only adds some more parameters and functionality on top of
    EAX2.

    Note the default reverb properties are the same as the FSOUND_PRESET_GENERIC preset.
    Note that integer values that typically range from -10,000 to 1000 are represented in
    decibels, and are of a logarithmic scale, not linear, wheras float values are typically linear.
    PORTABILITY: Each member has the platform it supports in braces ie (win32/xbox).
    Some reverb parameters are only supported in win32 and some only on xbox. If all parameters are set then
    the reverb should product a similar effect on either platform.
    Linux and FMODCE do not support the reverb api.

    The numerical values listed below are the maximum, minimum and default values for each variable respectively.

    [SEE_ALSO]
    FSOUND_Reverb_SetChannelProperties
    FSOUND_Reverb_GetChannelProperties
    FSOUND_REVERB_CHANNELFLAGS
]
*/
typedef struct _FSOUND_REVERB_CHANNELPROPERTIES /* MIN     MAX    DEFAULT */
{
    int    direct;                              /* -10000, 1000,  0,    direct path level (at low and mid frequencies) (win32/xbox) */
    int    direct_hf;                            /* -10000, 0,     0,    relative direct path level at high frequencies (win32/xbox) */
    int    room;                                /* -10000, 1000,  0,    room effect level (at low and mid frequencies) (win32/xbox) */
    int    room_hf;                              /* -10000, 0,     0,    relative room effect level at high frequencies (win32/xbox) */
    int    obstruction;                         /* -10000, 0,     0,    main obstruction control (attenuation at high frequencies)  (win32/xbox) */
    float  obstruction_lfratio;                  /* 0.0,    1.0,   0.0,  obstruction low-frequency level re. main control (win32/xbox) */
    int    occlusion;                           /* -10000, 0,     0,    main occlusion control (attenuation at high frequencies) (win32/xbox) */
    float  occlusion_lfratio;                    /* 0.0,    1.0,   0.25, occlusion low-frequency level re. main control (win32/xbox) */
    float  occlusion_room_ratio;                  /* 0.0,    10.0,  1.5,  relative occlusion control for room effect (win32) */
    float  occlusion_direct_ratio;                /* 0.0,    10.0,  1.0,  relative occlusion control for direct path (win32) */
    int    exclusion;                           /* -10000, 0,     0,    main exlusion control (attenuation at high frequencies) (win32) */
    float  exclusion_lfratio;                    /* 0.0,    1.0,   1.0,  exclusion low-frequency level re. main control (win32) */
    int    outside_volume_hf;                     /* -10000, 0,     0,    outside sound cone level at high frequencies (win32) */
    float  doppler_factor;                       /* 0.0,    10.0,  0.0,  like DS3D flDopplerFactor but per source (win32) */
    float  rolloff_factor;                       /* 0.0,    10.0,  0.0,  like DS3D flRolloffFactor but per source (win32) */
    float  room_rolloff_factor;                   /* 0.0,    10.0,  0.0,  like DS3D flRolloffFactor but for room effect (win32/xbox) */
    float  air_absorption_factor;                 /* 0.0,    10.0,  1.0,  multiplies AirAbsorptionHF member of FSOUND_REVERB_PROPERTIES (win32) */
    int    flags;                               /* FSOUND_REVERB_CHANNELFLAGS - modifies the behavior of properties (win32) */
} FSOUND_REVERB_CHANNELPROPERTIES;


/*
[DEFINE_START]
[
    [NAME]
    FSOUND_REVERB_CHANNELFLAGS

    [DESCRIPTION]
    Values for the Flags member of the FSOUND_REVERB_CHANNELPROPERTIES structure.

    [SEE_ALSO]
    FSOUND_REVERB_CHANNELPROPERTIES
]
*/
#define FSOUND_REVERB_CHANNELFLAGS_DIRECTHFAUTO  0x00000001 /* Automatic setting of 'Direct'  due to distance from listener */
#define FSOUND_REVERB_CHANNELFLAGS_ROOMAUTO      0x00000002 /* Automatic setting of 'Room'  due to distance from listener */
#define FSOUND_REVERB_CHANNELFLAGS_ROOMHFAUTO    0x00000004 /* Automatic setting of 'RoomHF' due to distance from listener */
#define FSOUND_REVERB_CHANNELFLAGS_DEFAULT       (FSOUND_REVERB_CHANNELFLAGS_DIRECTHFAUTO |   \
                                                  FSOUND_REVERB_CHANNELFLAGS_ROOMAUTO|        \
                                                  FSOUND_REVERB_CHANNELFLAGS_ROOMHFAUTO)
/* [DEFINE_END] */


/*
[ENUM]
[
    [DESCRIPTION]
    These values are used with FSOUND_FX_Enable to enable DirectX 8 FX for a channel.

    [SEE_ALSO]
    FSOUND_FX_Enable
    FSOUND_FX_Disable
    FSOUND_FX_SetChorus
    FSOUND_FX_SetCompressor
    FSOUND_FX_SetDistortion
    FSOUND_FX_SetEcho
    FSOUND_FX_SetFlanger
    FSOUND_FX_SetGargle
    FSOUND_FX_SetI3DL2Reverb
    FSOUND_FX_SetParamEQ
    FSOUND_FX_SetWavesReverb
]
*/
enum FSOUND_FX_MODES
{
    FSOUND_FX_CHORUS,
    FSOUND_FX_COMPRESSOR,
    FSOUND_FX_DISTORTION,
    FSOUND_FX_ECHO,
    FSOUND_FX_FLANGER,
    FSOUND_FX_GARGLE,
    FSOUND_FX_I3DL2REVERB,
    FSOUND_FX_PARAMEQ,
    FSOUND_FX_WAVES_REVERB,

    FSOUND_FX_MAX
};

/*
[ENUM]
[
    [DESCRIPTION]
    These are speaker types defined for use with the FSOUND_SetSpeakerMode command.
    Note - Only reliably works with FSOUND_OUTPUT_DSOUND or FSOUND_OUTPUT_XBOX output modes.  Other output modes will only
    interpret FSOUND_SPEAKERMODE_MONO and set everything else to be stereo.
    Note - Only reliably works with FSOUND_OUTPUT_DSOUND or FSOUND_OUTPUT_XBOX output modes.  Other output modes will only
    interpret FSOUND_SPEAKERMODE_MONO and set everything else to be stereo.

    Using either DolbyDigital or DTS will use whatever 5.1 digital mode is available if destination hardware is unsure.

    [SEE_ALSO]
    FSOUND_SetSpeakerMode

]
*/
enum FSOUND_SPEAKERMODES
{
    FSOUND_SPEAKERMODE_DOLBYDIGITAL,  /* The audio is played through a speaker arrangement of surround speakers with a subwoofer. */
    FSOUND_SPEAKERMODE_HEADPHONES,    /* The speakers are headphones. */
    FSOUND_SPEAKERMODE_MONO,          /* The speakers are monaural. */
    FSOUND_SPEAKERMODE_QUAD,          /* The speakers are quadraphonic.  */
    FSOUND_SPEAKERMODE_STEREO,        /* The speakers are stereo (default value). */
    FSOUND_SPEAKERMODE_SURROUND,      /* The speakers are surround sound. */
    FSOUND_SPEAKERMODE_DTS,           /* (XBOX Only) The audio is played through a speaker arrangement of surround speakers with a subwoofer. */
    FSOUND_SPEAKERMODE_PROLOGIC2      /* Dolby Prologic 2.  Currently Gamecube only */
};


/*
[DEFINE_START]
[
    [NAME]
    FSOUND_INIT_FLAGS

    [DESCRIPTION]
    Initialization flags.  Use them with FSOUND_Init in the flags parameter to change various behaviour.

    FSOUND_INIT_ENABLEOUTPUTFX Is an init mode which enables the FSOUND mixer buffer to be affected by DirectX 8 effects.
    Note that due to limitations of DirectSound, FSOUND_Init may fail if this is enabled because the buffersize is too small.
    This can be fixed with FSOUND_SetBufferSize.  Increase the BufferSize until it works.
    When it is enabled you can use the FSOUND_FX api, and use FSOUND_SYSTEMCHANNEL as the channel id when setting parameters.

    [SEE_ALSO]
    FSOUND_Init
]
*/
#define FSOUND_INIT_USEDEFAULTMIDISYNTH     0x0001    /* Causes MIDI playback to force software decoding. */
#define FSOUND_INIT_GLOBALFOCUS             0x0002    /* For DirectSound output - sound is not muted when window is out of focus. */
#define FSOUND_INIT_ENABLEOUTPUTFX          0x0004    /* For DirectSound output - Allows FSOUND_FX api to be used on global software mixer output! */
#define FSOUND_INIT_ACCURATEVULEVELS        0x0008    /* This latency adjusts FSOUND_GetCurrentLevels, but incurs a small cpu and memory hit */
#define FSOUND_INIT_PS2_DISABLECORE0REVERB  0x0010    /* PS2 only - Disable reverb on CORE 0 to regain SRAM */
#define FSOUND_INIT_PS2_DISABLECORE1REVERB  0x0020    /* PS2 only - Disable reverb on CORE 1 to regain SRAM */
#define FSOUND_INIT_PS2_SWAPDMACORES        0x0040    /* PS2 only - By default FMOD uses DMA CH0 for mixing, CH1 for uploads, this flag swaps them around */
#define FSOUND_INIT_DONTLATENCYADJUST       0x0080    /* Callbacks are not latency adjusted, and are called at mix time.  Also information functions are immediate */
#define FSOUND_INIT_GC_INITLIBS             0x0100    /* Gamecube only - Initializes GC audio libraries */
#define FSOUND_INIT_STREAM_FROM_MAIN_THREAD 0x0200    /* Turns off fmod streamer thread, and makes streaming update from FSOUND_Update called by the user */
/* [DEFINE_END] */




/* ========================================================================================== */
/* FUNCTION PROTOTYPES                                                                        */
/* ========================================================================================== */

#ifdef __cplusplus
extern "C"
{
#endif

/* ================================== */
/* Initialization / Global functions. */
/* ================================== */

/*
    PRE - FSOUND_Init functions. These can't be called after FSOUND_Init is
    called (they will fail). They set up FMOD system functionality.
*/

DLL_API signed char     F_API fsound_set_output(int outputtype);
DLL_API signed char     F_API fsound_set_driver(int driver);
DLL_API signed char     F_API fsound_set_mixer(int mixer);
DLL_API signed char     F_API fsound_set_buffer_size(int len_ms);
DLL_API signed char     F_API fsound_set_hwnd(void *hwnd);
DLL_API signed char     F_API fsound_set_min_hardware_channels(int min);
DLL_API signed char     F_API fsound_set_max_hardware_channels(int max);
DLL_API signed char     F_API fsound_set_memory_system(void *pool,
                                                     int poollen,
                                                     FSOUND_ALLOCCALLBACK   useralloc,
                                                     FSOUND_REALLOCCALLBACK userrealloc,
                                                     FSOUND_FREECALLBACK    userfree);
/*
    Main initialization / closedown functions.
    Note : Use FSOUND_INIT_USEDEFAULTMIDISYNTH with FSOUND_Init for software override
           with MIDI playback.
         : Use FSOUND_INIT_GLOBALFOCUS with FSOUND_Init to make sound audible no matter
           which window is in focus. (FSOUND_OUTPUT_DSOUND only)
*/

DLL_API signed char     F_API fsound_init(int mixrate, int maxsoftwarechannels, unsigned int flags);
DLL_API void            F_API fsound_close();

/*
    Runtime system level functions
*/

DLL_API void            F_API fsound_update();   /* This is called to update 3d sound / non-realtime output */

DLL_API void            F_API fsound_set_speaker_mode(unsigned int speakermode);
DLL_API void            F_API fsound_set_sfxmaster_volume(int volume);
DLL_API void            F_API fsound_set_pan_seperation(float pansep);
DLL_API void            F_API fsound_file_set_callbacks(FSOUND_OPENCALLBACK  useropen,
                                                       FSOUND_CLOSECALLBACK userclose,
                                                       FSOUND_READCALLBACK  userread,
                                                       FSOUND_SEEKCALLBACK  userseek,
                                                       FSOUND_TELLCALLBACK  usertell);

/*
    System information functions.
*/

DLL_API int             F_API fsound_get_error();
DLL_API float           F_API fsound_get_version();
DLL_API int             F_API fsound_get_output();
DLL_API void *          F_API fsound_get_output_handle();
DLL_API int             F_API fsound_get_driver();
DLL_API int             F_API fsound_get_mixer();
DLL_API int             F_API fsound_get_num_drivers();
DLL_API const char *    F_API fsound_get_driver_name(int id);
DLL_API signed char     F_API fsound_get_driver_caps(int id, unsigned int *caps);
DLL_API int             F_API fsound_get_output_rate();
DLL_API int             F_API fsound_get_max_channels();
DLL_API int             F_API fsound_get_max_samples();
DLL_API int             F_API fsound_get_sfxmaster_volume();
DLL_API int             F_API fsound_get_num_hardware_channels();
DLL_API int             F_API fsound_get_channels_playing();
DLL_API float           F_API fsound_get_cpuusage();
DLL_API void            F_API fsound_get_memory_stats(unsigned int *currentalloced, unsigned int *maxalloced);

/* =================================== */
/* Sample management / load functions. */
/* =================================== */

/*
    Sample creation and management functions
    Note : Use FSOUND_LOADMEMORY   flag with FSOUND_Sample_Load to load from memory.
           Use FSOUND_LOADRAW      flag with FSOUND_Sample_Load to treat as as raw pcm data.
*/

DLL_API FSOUND_SAMPLE * F_API fsound_sample_load(int index, const char *name_or_data, unsigned int mode, int memlength);
DLL_API FSOUND_SAMPLE * F_API fsound_sample_alloc(int index, int length, unsigned int mode, int deffreq, int defvol, int defpan, int defpri);
DLL_API void            F_API fsound_sample_free(FSOUND_SAMPLE *sptr);
DLL_API signed char     F_API fsound_sample_upload(FSOUND_SAMPLE *sptr, void *srcdata, unsigned int mode);
DLL_API signed char     F_API fsound_sample_lock(FSOUND_SAMPLE *sptr, int offset, int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
DLL_API signed char     F_API fsound_sample_unlock(FSOUND_SAMPLE *sptr, void *ptr1, void *ptr2, unsigned int len1, unsigned int len2);

/*
    Sample control functions
*/

DLL_API signed char     F_API fsound_sample_set_mode(FSOUND_SAMPLE *sptr, unsigned int mode);
DLL_API signed char     F_API fsound_sample_set_loop_points(FSOUND_SAMPLE *sptr, int loopstart, int loopend);
DLL_API signed char     F_API fsound_sample_set_defaults(FSOUND_SAMPLE *sptr, int deffreq, int defvol, int defpan, int defpri);
DLL_API signed char     F_API fsound_sample_set_min_max_distance(FSOUND_SAMPLE *sptr, float min, float max);
DLL_API signed char     F_API fsound_sample_set_max_playbacks(FSOUND_SAMPLE *sptr, int max);

/*
    Sample information functions
*/

DLL_API FSOUND_SAMPLE * F_API fsound_sample_get(int sampno);
DLL_API const char *    F_API fsound_sample_get_name(FSOUND_SAMPLE *sptr);
DLL_API unsigned int    F_API fsound_sample_get_length(FSOUND_SAMPLE *sptr);
DLL_API signed char     F_API fsound_sample_get_loop_points(FSOUND_SAMPLE *sptr, int *loopstart, int *loopend);
DLL_API signed char     F_API fsound_sample_get_defaults(FSOUND_SAMPLE *sptr, int *deffreq, int *defvol, int *defpan, int *defpri);
DLL_API unsigned int    F_API fsound_sample_get_mode(FSOUND_SAMPLE *sptr);

/* ============================ */
/* Channel control functions.   */
/* ============================ */

/*
    Playing and stopping sounds.
    Note : Use FSOUND_FREE as the 'channel' variable, to let FMOD pick a free channel for you.
           Use FSOUND_ALL as the 'channel' variable to control ALL channels with one function call!
*/

DLL_API int             F_API fsound_play_sound(int channel, FSOUND_SAMPLE *sptr);
DLL_API int             F_API fsound_play_sound_ex(int channel, FSOUND_SAMPLE *sptr, FSOUND_DSPUNIT *dsp, signed char startpaused);
DLL_API signed char     F_API fsound_stop_sound(int channel);

/*
    Functions to control playback of a channel.
    Note : FSOUND_ALL can be used on most of these functions as a channel value.
*/

DLL_API signed char     F_API fsound_set_frequency(int channel, int freq);
DLL_API signed char     F_API fsound_set_volume(int channel, int vol);
DLL_API signed char     F_API fsound_set_volume_absolute(int channel, int vol);
DLL_API signed char     F_API fsound_set_pan(int channel, int pan);
DLL_API signed char     F_API fsound_set_surround(int channel, signed char surround);
DLL_API signed char     F_API fsound_set_mute(int channel, signed char mute);
DLL_API signed char     F_API fsound_set_priority(int channel, int priority);
DLL_API signed char     F_API fsound_set_reserved(int channel, signed char reserved);
DLL_API signed char     F_API fsound_set_paused(int channel, signed char paused);
DLL_API signed char     F_API fsound_set_loop_mode(int channel, unsigned int loopmode);
DLL_API signed char     F_API fsound_set_current_position(int channel, unsigned int offset);

/*
    Channel information functions.
*/

DLL_API signed char     F_API fsound_is_playing(int channel);
DLL_API int             F_API fsound_get_frequency(int channel);
DLL_API int             F_API fsound_get_volume(int channel);
DLL_API int             F_API fsound_get_pan(int channel);
DLL_API signed char     F_API fsound_get_surround(int channel);
DLL_API signed char     F_API fsound_get_mute(int channel);
DLL_API int             F_API fsound_get_priority(int channel);
DLL_API signed char     F_API fsound_get_reserved(int channel);
DLL_API signed char     F_API fsound_get_paused(int channel);
DLL_API unsigned int    F_API fsound_get_loop_mode(int channel);
DLL_API unsigned int    F_API fsound_get_current_position(int channel);
DLL_API FSOUND_SAMPLE * F_API fsound_get_current_sample(int channel);
DLL_API signed char     F_API fsound_get_current_levels(int channel, float *l, float *r);


/* =================== */
/* FX functions.       */
/* =================== */

/*
    Functions to control DX8 only effects processing.

    - FX enabled samples can only be played once at a time, not multiple times at once.
    - Sounds have to be created with FSOUND_HW2D or FSOUND_HW3D for this to work.
    - FSOUND_INIT_ENABLEOUTPUTFX can be used to apply hardware effect processing to the
      global mixed output of FMOD's software channels.
    - FSOUND_FX_Enable returns an FX handle that you can use to alter fx parameters.
    - FSOUND_FX_Enable can be called multiple times in a row, even on the same FX type,
      it will return a unique handle for each FX.
    - FSOUND_FX_Enable cannot be called if the sound is playing or locked.
    - FSOUND_FX_Disable must be called to reset/clear the FX from a channel.
*/

DLL_API int             F_API fsound_fx_enable(int channel, unsigned int fx);    /* See FSOUND_FX_MODES */
DLL_API signed char     F_API fsound_fx_disable(int channel);

DLL_API signed char     F_API fsound_fx_set_chorus(int fxid, float wet_dry_mix, float depth, float feedback, float frequency, int waveform, float delay, int phase);
DLL_API signed char     F_API fsound_fx_set_compressor(int fxid, float gain, float attack, float release, float threshold, float ratio, float predelay);
DLL_API signed char     F_API fsound_fx_set_distortion(int fxid, float gain, float edge, float post_eqcenter_frequency, float post_eqbandwidth, float pre_lowpass_cutoff);
DLL_API signed char     F_API fsound_fx_set_echo(int fxid, float wet_dry_mix, float feedback, float left_delay, float right_delay, int pan_delay);
DLL_API signed char     F_API fsound_fx_set_flanger(int fxid, float wet_dry_mix, float depth, float feedback, float frequency, int waveform, float delay, int phase);
DLL_API signed char     F_API fsound_fx_set_gargle(int fxid, int rate_hz, int wave_shape);
DLL_API signed char     F_API fsound_fx_set_i3dl2reverb(int fxid, int room, int room_hf, float room_rolloff_factor, float decay_time, float decay_hfratio, int reflections, float reflections_delay, int reverb, float reverb_delay, float diffusion, float density, float hfreference);
DLL_API signed char     F_API fsound_fx_set_param_eq(int fxid, float center, float bandwidth, float gain);
DLL_API signed char     F_API fsound_fx_set_waves_reverb(int fxid, float in_gain, float reverb_mix, float reverb_time, float high_freq_rtratio);

/* =================== */
/* 3D sound functions. */
/* =================== */

/*
    See also FSOUND_Sample_SetMinMaxDistance (above)
    Note! FSOUND_3D_Update is now called FSOUND_Update.
*/

DLL_API void            F_API fsound_3d_set_doppler_factor(float scale);
DLL_API void            F_API fsound_3d_set_distance_factor(float scale);
DLL_API void            F_API fsound_3d_set_rolloff_factor(float scale);
DLL_API signed char     F_API fsound_3d_set_attributes(int channel, float *pos, float *vel);
DLL_API signed char     F_API fsound_3d_get_attributes(int channel, float *pos, float *vel);

DLL_API void            F_API fsound_3d_listener_set_current(int current, int numlisteners);  /* use this if you use multiple listeners / splitscreen */
DLL_API void            F_API fsound_3d_listener_set_attributes(float *pos, float *vel, float fx, float fy, float fz, float tx, float ty, float tz);
DLL_API void            F_API fsound_3d_listener_get_attributes(float *pos, float *vel, float *fx, float *fy, float *fz, float *tx, float *ty, float *tz);

/* ========================= */
/* File Streaming functions. */
/* ========================= */

/*
    Note : Use FSOUND_LOADMEMORY   flag with FSOUND_Stream_OpenFile to stream from memory.
           Use FSOUND_LOADRAW      flag with FSOUND_Stream_OpenFile to treat stream as raw pcm data.
           Use FSOUND_MPEGACCURATE flag with FSOUND_Stream_OpenFile to open mpegs in 'accurate mode' for settime/gettime/getlengthms.
           Use FSOUND_FREE as the 'channel' variable, to let FMOD pick a free channel for you.
*/

DLL_API signed char     F_API fsound_stream_set_buffer_size(int ms);      /* call this before opening streams, not after */

DLL_API FSOUND_STREAM * F_API fsound_stream_open_file(const char *filename, unsigned int mode, int memlength);
DLL_API FSOUND_STREAM * F_API fsound_stream_create(FSOUND_STREAMCALLBACK callback, int length, unsigned int mode, int samplerate, int userdata);
DLL_API signed char     F_API fsound_stream_close(FSOUND_STREAM *stream);

DLL_API int             F_API fsound_stream_play(int channel, FSOUND_STREAM *stream);
DLL_API int             F_API fsound_stream_play_ex(int channel, FSOUND_STREAM *stream, FSOUND_DSPUNIT *dsp, signed char startpaused);
DLL_API signed char     F_API fsound_stream_stop(FSOUND_STREAM *stream);

DLL_API signed char     F_API fsound_stream_set_position(FSOUND_STREAM *stream, unsigned int position);
DLL_API unsigned int    F_API fsound_stream_get_position(FSOUND_STREAM *stream);
DLL_API signed char     F_API fsound_stream_set_time(FSOUND_STREAM *stream, int ms);
DLL_API int             F_API fsound_stream_get_time(FSOUND_STREAM *stream);
DLL_API int             F_API fsound_stream_get_length(FSOUND_STREAM *stream);
DLL_API int             F_API fsound_stream_get_length_ms(FSOUND_STREAM *stream);

DLL_API signed char     F_API fsound_stream_set_mode(FSOUND_STREAM *stream, unsigned int mode);
DLL_API unsigned int    F_API fsound_stream_get_mode(FSOUND_STREAM *stream);
DLL_API signed char     F_API fsound_stream_set_loop_points(FSOUND_STREAM *stream, unsigned int loopstartpcm, unsigned int loopendpcm);
DLL_API signed char     F_API fsound_stream_set_loop_count(FSOUND_STREAM *stream, int count);
DLL_API int             F_API fsound_stream_get_open_state(FSOUND_STREAM *stream);                /* use with FSOUND_NONBLOCKING opened streams */
DLL_API FSOUND_SAMPLE * F_API fsound_stream_get_sample(FSOUND_STREAM *stream);
DLL_API FSOUND_DSPUNIT *F_API fsound_stream_create_dsp(FSOUND_STREAM *stream, FSOUND_DSPCALLBACK callback, int priority, int param);

DLL_API signed char     F_API fsound_stream_set_end_callback(FSOUND_STREAM *stream, FSOUND_STREAMCALLBACK callback, int userdata);
DLL_API signed char     F_API fsound_stream_set_synch_callback(FSOUND_STREAM *stream, FSOUND_STREAMCALLBACK callback, int userdata);
DLL_API int             F_API fsound_stream_add_synch_point(FSOUND_STREAM *stream, unsigned int pcmoffset, const char *name);
DLL_API signed char     F_API fsound_stream_delete_synch_point(FSOUND_STREAM *stream, int index);
DLL_API int             F_API fsound_stream_get_num_synch_points(FSOUND_STREAM *stream);
DLL_API signed char     F_API fsound_stream_set_sub_stream(FSOUND_STREAM *stream, int index);     /* For FMOD .FSB bank files. */
DLL_API int             F_API fsound_stream_get_num_sub_streams(FSOUND_STREAM *stream);            /* For FMOD .FSB bank files. */
DLL_API signed char     F_API fsound_stream_set_sub_stream_sentence(FSOUND_STREAM *stream, int *sentencelist, int numitems);

/* =================== */
/* CD audio functions. */
/* =================== */

/*
    Note : 0 = default cdrom.  Otherwise specify the drive letter, for example. 'D'.
*/

DLL_API signed char     F_API fsound_cd_play(char drive, int track);
DLL_API void            F_API fsound_cd_set_play_mode(char drive, signed char mode);
DLL_API signed char     F_API fsound_cd_stop(char drive);
DLL_API signed char     F_API fsound_cd_set_paused(char drive, signed char paused);
DLL_API signed char     F_API fsound_cd_set_volume(char drive, int volume);
DLL_API signed char     F_API fsound_cd_set_track_time(char drive, unsigned int ms);
DLL_API signed char     F_API fsound_cd_eject(char drive);

DLL_API signed char     F_API fsound_cd_get_paused(char drive);
DLL_API int             F_API fsound_cd_get_track(char drive);
DLL_API int             F_API fsound_cd_get_num_tracks(char drive);
DLL_API int             F_API fsound_cd_get_volume(char drive);
DLL_API int             F_API fsound_cd_get_track_length(char drive, int track);
DLL_API int             F_API fsound_cd_get_track_time(char drive);

/* ============== */
/* DSP functions. */
/* ============== */

/*
    DSP Unit control and information functions.
    These functions allow you access to the mixed stream that FMOD uses to play back sound on.
*/

DLL_API FSOUND_DSPUNIT *F_API fsound_dsp_create(FSOUND_DSPCALLBACK callback, int priority, int param);
DLL_API void            F_API fsound_dsp_free(FSOUND_DSPUNIT *unit);
DLL_API void            F_API fsound_dsp_set_priority(FSOUND_DSPUNIT *unit, int priority);
DLL_API int             F_API fsound_dsp_get_priority(FSOUND_DSPUNIT *unit);
DLL_API void            F_API fsound_dsp_set_active(FSOUND_DSPUNIT *unit, signed char active);
DLL_API signed char     F_API fsound_dsp_get_active(FSOUND_DSPUNIT *unit);

/*
    Functions to get hold of FSOUND 'system DSP unit' handles.
*/

DLL_API FSOUND_DSPUNIT *F_API fsound_dsp_get_clear_unit();
DLL_API FSOUND_DSPUNIT *F_API fsound_dsp_get_sfxunit();
DLL_API FSOUND_DSPUNIT *F_API fsound_dsp_get_music_unit();
DLL_API FSOUND_DSPUNIT *F_API fsound_dsp_get_fftunit();
DLL_API FSOUND_DSPUNIT *F_API fsound_dsp_get_clip_and_copy_unit();

/*
    Miscellaneous DSP functions
    Note for the spectrum analysis function to work, you have to enable the FFT DSP unit with
    the following code FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), TRUE);
    It is off by default to save cpu usage.
*/

DLL_API signed char     F_API fsound_dsp_mix_buffers(void *destbuffer, void *srcbuffer, int len, int freq, int vol, int pan, unsigned int mode);
DLL_API void            F_API fsound_dsp_clear_mix_buffer();
DLL_API int             F_API fsound_dsp_get_buffer_length();      /* Length of each DSP update */
DLL_API int             F_API fsound_dsp_get_buffer_length_total(); /* Total buffer length due to FSOUND_SetBufferSize */
DLL_API float *         F_API fsound_dsp_get_spectrum();          /* Array of 512 floats - call FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), TRUE)) for this to work. */

/* =================================================================================== */
/* Reverb functions. (eax2/eax3 reverb)  (ONLY SUPPORTED ON WIN32 W/ FSOUND_HW3D FLAG) */
/* =================================================================================== */

/*
    See top of file for definitions and information on the reverb parameters.
*/

DLL_API signed char     F_API fsound_reverb_set_properties(FSOUND_REVERB_PROPERTIES *prop);
DLL_API signed char     F_API fsound_reverb_get_properties(FSOUND_REVERB_PROPERTIES *prop);
DLL_API signed char     F_API fsound_reverb_set_channel_properties(int channel, FSOUND_REVERB_CHANNELPROPERTIES *prop);
DLL_API signed char     F_API fsound_reverb_get_channel_properties(int channel, FSOUND_REVERB_CHANNELPROPERTIES *prop);

/* ===================================================== */
/* Recording functions  (ONLY SUPPORTED IN WIN32, WINCE) */
/* ===================================================== */

/*
    Recording initialization functions
*/

DLL_API signed char     F_API fsound_record_set_driver(int outputtype);
DLL_API int             F_API fsound_record_get_num_drivers();
DLL_API const char *    F_API fsound_record_get_driver_name(int id);
DLL_API int             F_API fsound_record_get_driver();

/*
    Recording functionality.  Only one recording session will work at a time.
*/

DLL_API signed char     F_API fsound_record_start_sample(FSOUND_SAMPLE *sptr, signed char loop);
DLL_API signed char     F_API fsound_record_stop();
DLL_API int             F_API fsound_record_get_position();

/* ========================================================================================== */
/* FMUSIC API (MOD,S3M,XM,IT,MIDI PLAYBACK)                                                   */
/* ========================================================================================== */

/*
    Song management / playback functions.
*/

DLL_API FMUSIC_MODULE * F_API fmusic_load_song(const char *name);
DLL_API FMUSIC_MODULE * F_API fmusic_load_song_ex(const char *name_or_data, int memlength, unsigned int mode, int *samplelist, int samplelistnum);
DLL_API int             F_API fmusic_get_open_state(FMUSIC_MODULE *mod);
DLL_API signed char     F_API fmusic_free_song(FMUSIC_MODULE *mod);
DLL_API signed char     F_API fmusic_play_song(FMUSIC_MODULE *mod);
DLL_API signed char     F_API fmusic_stop_song(FMUSIC_MODULE *mod);
DLL_API void            F_API fmusic_stop_all_songs();

DLL_API signed char     F_API fmusic_set_zxx_callback(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback);
DLL_API signed char     F_API fmusic_set_row_callback(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int rowstep);
DLL_API signed char     F_API fmusic_set_order_callback(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int orderstep);
DLL_API signed char     F_API fmusic_set_inst_callback(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int instrument);

DLL_API signed char     F_API fmusic_set_sample(FMUSIC_MODULE *mod, int sampno, FSOUND_SAMPLE *sptr);
DLL_API signed char     F_API fmusic_set_user_data(FMUSIC_MODULE *mod, unsigned int userdata);
DLL_API signed char     F_API fmusic_optimize_channels(FMUSIC_MODULE *mod, int maxchannels, int minvolume);

/*
    Runtime song functions.
*/

DLL_API signed char     F_API fmusic_set_reverb(signed char reverb);             /* MIDI only */
DLL_API signed char     F_API fmusic_set_looping(FMUSIC_MODULE *mod, signed char looping);
DLL_API signed char     F_API fmusic_set_order(FMUSIC_MODULE *mod, int order);
DLL_API signed char     F_API fmusic_set_paused(FMUSIC_MODULE *mod, signed char pause);
DLL_API signed char     F_API fmusic_set_master_volume(FMUSIC_MODULE *mod, int volume);
DLL_API signed char     F_API fmusic_set_master_speed(FMUSIC_MODULE *mode, float speed);
DLL_API signed char     F_API fmusic_set_pan_seperation(FMUSIC_MODULE *mod, float pansep);

/*
    Static song information functions.
*/

DLL_API const char *    F_API fmusic_get_name(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_type(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_num_orders(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_num_patterns(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_num_instruments(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_num_samples(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_num_channels(FMUSIC_MODULE *mod);
DLL_API FSOUND_SAMPLE * F_API fmusic_get_sample(FMUSIC_MODULE *mod, int sampno);
DLL_API int             F_API fmusic_get_pattern_length(FMUSIC_MODULE *mod, int orderno);

/*
    Runtime song information.
*/

DLL_API signed char     F_API fmusic_is_finished(FMUSIC_MODULE *mod);
DLL_API signed char     F_API fmusic_is_playing(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_master_volume(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_global_volume(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_order(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_pattern(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_speed(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_bpm(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_row(FMUSIC_MODULE *mod);
DLL_API signed char     F_API fmusic_get_paused(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_time(FMUSIC_MODULE *mod);
DLL_API int             F_API fmusic_get_real_channel(FMUSIC_MODULE *mod, int modchannel);
DLL_API unsigned int    F_API fmusic_get_user_data(FMUSIC_MODULE *mod);

#ifdef __cplusplus
}
#endif

#endif

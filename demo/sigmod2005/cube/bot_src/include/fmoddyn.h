/* ========================================================================================== */
/* FMOD Dynamic DLL loading header. Copyright (c), Firelight Technologies Pty, Ltd 1999-2003. */
/* ========================================================================================== */

#ifndef _FMODDYN_H_
#define _FMODDYN_H_

#ifdef WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
  #include <string.h>
#endif

#include "fmod.h"

typedef struct
{
    void           *module;

    signed char     (F_API *fsound_set_output)(int outputtype);
    signed char     (F_API *fsound_set_driver)(int driver);
    signed char     (F_API *fsound_set_mixer)(int mixer);
    signed char     (F_API *fsound_set_buffer_size)(int len_ms);
    signed char     (F_API *fsound_set_hwnd)(void *hwnd);
    signed char     (F_API *fsound_set_min_hardware_channels)(int min);
    signed char     (F_API *fsound_set_max_hardware_channels)(int max);
    signed char     (F_API *fsound_set_memory_system)(void *pool, int poollen, FSOUND_ALLOCCALLBACK useralloc, FSOUND_REALLOCCALLBACK userrealloc, FSOUND_FREECALLBACK userfree);
    signed char     (F_API *fsound_init)(int mixrate, int maxsoftwarechannels, unsigned int flags);
    void            (F_API *fsound_close)();
    void            (F_API *fsound_set_speaker_mode)(unsigned int speakermode);
    void            (F_API *fsound_set_sfxmaster_volume)(int volume);
    void            (F_API *fsound_set_pan_seperation)(float pansep);
    void            (F_API *fsound_file_set_callbacks)(FSOUND_OPENCALLBACK  useropen, FSOUND_CLOSECALLBACK userclose, FSOUND_READCALLBACK userread, FSOUND_SEEKCALLBACK  userseek, FSOUND_TELLCALLBACK  usertell);
    int             (F_API *fsound_get_error)();
    float           (F_API *fsound_get_version)();
    int             (F_API *fsound_get_output)();
    void *          (F_API *fsound_get_output_handle)();
    int             (F_API *fsound_get_driver)();
    int             (F_API *fsound_get_mixer)();
    int             (F_API *fsound_get_num_drivers)();
    signed char *   (F_API *fsound_get_driver_name)(int id);
    signed char     (F_API *fsound_get_driver_caps)(int id, unsigned int *caps);
    int             (F_API *fsound_get_output_rate)();
    int             (F_API *fsound_get_max_channels)();
    int             (F_API *fsound_get_max_samples)();
    int             (F_API *fsound_get_sfxmaster_volume)();
    int             (F_API *fsound_get_num_hardware_channels)();
    int             (F_API *fsound_get_channels_playing)();
    float           (F_API *fsound_get_cpuusage)();
    void            (F_API *fsound_get_memory_stats)(unsigned int *currentalloced, unsigned int *maxalloced);
    FSOUND_SAMPLE * (F_API *fsound_sample_load)(int index, const char *name_or_data, unsigned int mode, int memlength);
    FSOUND_SAMPLE * (F_API *fsound_sample_alloc)(int index, int length, unsigned int mode, int deffreq, int defvol, int defpan, int defpri);
    void            (F_API *fsound_sample_free)(FSOUND_SAMPLE *sptr);
    signed char     (F_API *fsound_sample_upload)(FSOUND_SAMPLE *sptr, void *srcdata, unsigned int mode);
    signed char     (F_API *fsound_sample_lock)(FSOUND_SAMPLE *sptr, int offset, int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
    signed char     (F_API *fsound_sample_unlock)(FSOUND_SAMPLE *sptr, void *ptr1, void *ptr2, unsigned int len1, unsigned int len2);
    signed char     (F_API *fsound_sample_set_mode)(FSOUND_SAMPLE *sptr, unsigned int mode);
    signed char     (F_API *fsound_sample_set_loop_points)(FSOUND_SAMPLE *sptr, int loopstart, int loopend);
    signed char     (F_API *fsound_sample_set_defaults)(FSOUND_SAMPLE *sptr, int deffreq, int defvol, int defpan, int defpri);
    signed char     (F_API *fsound_sample_set_min_max_distance)(FSOUND_SAMPLE *sptr, float min, float max);
    signed char     (F_API *fsound_sample_set_max_playbacks)(FSOUND_SAMPLE *sptr, int max);
    FSOUND_SAMPLE * (F_API *fsound_sample_get)(int sampno);
    char *          (F_API *fsound_sample_get_name)(FSOUND_SAMPLE *sptr);
    unsigned int    (F_API *fsound_sample_get_length)(FSOUND_SAMPLE *sptr);
    signed char     (F_API *fsound_sample_get_loop_points)(FSOUND_SAMPLE *sptr, int *loopstart, int *loopend);
    signed char     (F_API *fsound_sample_get_defaults)(FSOUND_SAMPLE *sptr, int *deffreq, int *defvol, int *defpan, int *defpri);
    unsigned int    (F_API *fsound_sample_get_mode)(FSOUND_SAMPLE *sptr);
    int             (F_API *fsound_play_sound)(int channel, FSOUND_SAMPLE *sptr);
    int             (F_API *fsound_play_sound_ex)(int channel, FSOUND_SAMPLE *sptr, FSOUND_DSPUNIT *dsp, signed char startpaused);
    signed char     (F_API *fsound_stop_sound)(int channel);
    signed char     (F_API *fsound_set_frequency)(int channel, int freq);
    signed char     (F_API *fsound_set_volume)(int channel, int vol);
    signed char     (F_API *fsound_set_volume_absolute)(int channel, int vol);
    signed char     (F_API *fsound_set_pan)(int channel, int pan);
    signed char     (F_API *fsound_set_surround)(int channel, signed char surround);
    signed char     (F_API *fsound_set_mute)(int channel, signed char mute);
    signed char     (F_API *fsound_set_priority)(int channel, int priority);
    signed char     (F_API *fsound_set_reserved)(int channel, signed char reserved);
    signed char     (F_API *fsound_set_paused)(int channel, signed char paused);
    signed char     (F_API *fsound_set_loop_mode)(int channel, unsigned int loopmode);
    signed char     (F_API *fsound_set_current_position)(int channel, unsigned int offset);
    signed char     (F_API *fsound_is_playing)(int channel);
    int             (F_API *fsound_get_frequency)(int channel);
    int             (F_API *fsound_get_volume)(int channel);
    int             (F_API *fsound_get_pan)(int channel);
    signed char     (F_API *fsound_get_surround)(int channel);
    signed char     (F_API *fsound_get_mute)(int channel);
    int             (F_API *fsound_get_priority)(int channel);
    signed char     (F_API *fsound_get_reserved)(int channel);
    signed char     (F_API *fsound_get_paused)(int channel);
    unsigned int    (F_API *fsound_get_loop_mode)(int channel);
    unsigned int    (F_API *fsound_get_current_position)(int channel);
    FSOUND_SAMPLE * (F_API *fsound_get_current_sample)(int channel);
    signed char     (F_API *fsound_get_current_levels)(int channel, float *l, float *r);
    int             (F_API *fsound_fx_enable)(int channel, unsigned int fx);    /* See FSOUND_FX_MODES */
    signed char     (F_API *fsound_fx_disable)(int channel);
    signed char     (F_API *fsound_fx_set_chorus)(int fxid, float wet_dry_mix, float depth, float feedback, float frequency, int waveform, float delay, int phase);
    signed char     (F_API *fsound_fx_set_compressor)(int fxid, float gain, float attack, float release, float threshold, float ratio, float predelay);
    signed char     (F_API *fsound_fx_set_distortion)(int fxid, float gain, float edge, float post_eqcenter_frequency, float post_eqbandwidth, float pre_lowpass_cutoff);
    signed char     (F_API *fsound_fx_set_echo)(int fxid, float wet_dry_mix, float feedback, float left_delay, float right_delay, int pan_delay);
    signed char     (F_API *fsound_fx_set_flanger)(int fxid, float wet_dry_mix, float depth, float feedback, float frequency, int waveform, float delay, int phase);
    signed char     (F_API *fsound_fx_set_gargle)(int fxid, int rate_hz, int wave_shape);
    signed char     (F_API *fsound_fx_set_i3dl2reverb)(int fxid, int room, int room_hf, float room_rolloff_factor, float decay_time, float decay_hfratio, int reflections, float reflections_delay, int reverb, float reverb_delay, float diffusion, float density, float hfreference);
    signed char     (F_API *fsound_fx_set_param_eq)(int fxid, float center, float bandwidth, float gain);
    signed char     (F_API *fsound_fx_set_waves_reverb)(int fxid, float in_gain, float reverb_mix, float reverb_time, float high_freq_rtratio);
    void            (F_API *fsound_update)();   /* you must call this once a frame */
    void            (F_API *fsound_3d_set_doppler_factor)(float scale);
    void            (F_API *fsound_3d_set_distance_factor)(float scale);
    void            (F_API *fsound_3d_set_rolloff_factor)(float scale);
    signed char     (F_API *fsound_3d_set_attributes)(int channel, float *pos, float *vel);
    signed char     (F_API *fsound_3d_get_attributes)(int channel, float *pos, float *vel);
    void            (F_API *fsound_3d_listener_set_current)(int current, int numlisteners);  /* use this if you use multiple listeners / splitscreen */
    void            (F_API *fsound_3d_listener_set_attributes)(float *pos, float *vel, float fx, float fy, float fz, float tx, float ty, float tz);
    void            (F_API *fsound_3d_listener_get_attributes)(float *pos, float *vel, float *fx, float *fy, float *fz, float *tx, float *ty, float *tz);
    signed char     (F_API *fsound_stream_set_buffer_size)(int ms);      /* call this before opening streams, not after */
    FSOUND_STREAM * (F_API *fsound_stream_open_file)(const char *filename, unsigned int mode, int memlength);
    FSOUND_STREAM * (F_API *fsound_stream_create)(FSOUND_STREAMCALLBACK callback, int length, unsigned int mode, int samplerate, int userdata);
    int             (F_API *fsound_stream_play)(int channel, FSOUND_STREAM *stream);
    int             (F_API *fsound_stream_play_ex)(int channel, FSOUND_STREAM *stream, FSOUND_DSPUNIT *dsp, signed char startpaused);
    signed char     (F_API *fsound_stream_stop)(FSOUND_STREAM *stream);
    signed char     (F_API *fsound_stream_close)(FSOUND_STREAM *stream);
    signed char     (F_API *fsound_stream_set_end_callback)(FSOUND_STREAM *stream, FSOUND_STREAMCALLBACK callback, int userdata);
    signed char     (F_API *fsound_stream_set_synch_callback)(FSOUND_STREAM *stream, FSOUND_STREAMCALLBACK callback, int userdata);
    FSOUND_SAMPLE * (F_API *fsound_stream_get_sample)(FSOUND_STREAM *stream);   /* every stream contains a sample to playback on */
    FSOUND_DSPUNIT *(F_API *fsound_stream_create_dsp)(FSOUND_STREAM *stream, FSOUND_DSPCALLBACK callback, int priority, int param);
    signed char     (F_API *fsound_stream_set_position)(FSOUND_STREAM *stream, unsigned int position);
    unsigned int    (F_API *fsound_stream_get_position)(FSOUND_STREAM *stream);
    signed char     (F_API *fsound_stream_set_time)(FSOUND_STREAM *stream, int ms);
    int             (F_API *fsound_stream_get_time)(FSOUND_STREAM *stream);
    int             (F_API *fsound_stream_get_length)(FSOUND_STREAM *stream);
    int             (F_API *fsound_stream_get_length_ms)(FSOUND_STREAM *stream);
    signed char     (F_API *fsound_stream_set_sub_stream)(FSOUND_STREAM *stream, int index);
    int             (F_API *fsound_stream_get_num_sub_streams)(FSOUND_STREAM *stream);
    signed char     (F_API *fsound_stream_set_sub_stream_sentence)(FSOUND_STREAM *stream, int *sentencelist, int numitems);
    signed char     (F_API *fsound_stream_set_mode)(FSOUND_STREAM *stream, unsigned int mode);
    unsigned int    (F_API *fsound_stream_get_mode)(FSOUND_STREAM *stream);
    signed char     (F_API *fsound_stream_set_loop_points)(FSOUND_STREAM *stream, unsigned int loopstartpcm, unsigned int loopendpcm);
    signed char     (F_API *fsound_stream_set_loop_count)(FSOUND_STREAM *stream, int count);
    int             (F_API *fsound_stream_add_synch_point)(FSOUND_STREAM *stream, unsigned int pcmoffset, int userdata);
    signed char     (F_API *fsound_stream_delete_synch_point)(FSOUND_STREAM *stream, int index);
    int             (F_API *fsound_stream_get_num_synch_points)(FSOUND_STREAM *stream);
    int             (F_API *fsound_stream_get_open_state)(FSOUND_STREAM *stream);
    signed char     (F_API *fsound_cd_play)(char drive, int track);
    void            (F_API *fsound_cd_set_play_mode)(char drive, signed char mode);
    signed char     (F_API *fsound_cd_stop)(char drive);
    signed char     (F_API *fsound_cd_set_paused)(char drive, signed char paused);
    signed char     (F_API *fsound_cd_set_volume)(char drive, int volume);
    signed char     (F_API *fsound_cd_set_track_time)(char drive, unsigned int ms);
    signed char     (F_API *fsound_cd_eject)(char drive);
    signed char     (F_API *fsound_cd_get_paused)(char drive);
    int             (F_API *fsound_cd_get_track)(char drive);
    int             (F_API *fsound_cd_get_num_tracks)(char drive);
    int             (F_API *fsound_cd_get_volume)(char drive);
    int             (F_API *fsound_cd_get_track_length)(char drive, int track);
    int             (F_API *fsound_cd_get_track_time)(char drive);
    FSOUND_DSPUNIT *(F_API *fsound_dsp_create)(FSOUND_DSPCALLBACK callback, int priority, int param);
    void            (F_API *fsound_dsp_free)(FSOUND_DSPUNIT *unit);
    void            (F_API *fsound_dsp_set_priority)(FSOUND_DSPUNIT *unit, int priority);
    int             (F_API *fsound_dsp_get_priority)(FSOUND_DSPUNIT *unit);
    void            (F_API *fsound_dsp_set_active)(FSOUND_DSPUNIT *unit, signed char active);
    signed char     (F_API *fsound_dsp_get_active)(FSOUND_DSPUNIT *unit);
    FSOUND_DSPUNIT *(F_API *fsound_dsp_get_clear_unit)();
    FSOUND_DSPUNIT *(F_API *fsound_dsp_get_sfxunit)();
    FSOUND_DSPUNIT *(F_API *fsound_dsp_get_music_unit)();
    FSOUND_DSPUNIT *(F_API *fsound_dsp_get_fftunit)();
    FSOUND_DSPUNIT *(F_API *fsound_dsp_get_clip_and_copy_unit)();
    signed char     (F_API *fsound_dsp_mix_buffers)(void *destbuffer, void *srcbuffer, int len, int freq, int vol, int pan, unsigned int mode);
    void            (F_API *fsound_dsp_clear_mix_buffer)();
    int             (F_API *fsound_dsp_get_buffer_length)();      /* Length of each DSP update */
    int             (F_API *fsound_dsp_get_buffer_length_total)(); /* Total buffer length due to FSOUND_SetBufferSize */
    float *         (F_API *fsound_dsp_get_spectrum)();          /* Array of 512 floats - call FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), TRUE)) for this to work. */
    signed char     (F_API *fsound_reverb_set_properties)(FSOUND_REVERB_PROPERTIES *prop);
    signed char     (F_API *fsound_reverb_get_properties)(FSOUND_REVERB_PROPERTIES *prop);
    signed char     (F_API *fsound_reverb_set_channel_properties)(int channel, FSOUND_REVERB_CHANNELPROPERTIES *prop);
    signed char     (F_API *fsound_reverb_get_channel_properties)(int channel, FSOUND_REVERB_CHANNELPROPERTIES *prop);
    signed char     (F_API *fsound_record_set_driver)(int outputtype);
    int             (F_API *fsound_record_get_num_drivers)();
    signed char *   (F_API *fsound_record_get_driver_name)(int id);
    int             (F_API *fsound_record_get_driver)();
    signed char     (F_API *fsound_record_start_sample)(FSOUND_SAMPLE *sptr, signed char loop);
    signed char     (F_API *fsound_record_stop)();
    int             (F_API *fsound_record_get_position)();
    FMUSIC_MODULE * (F_API *fmusic_load_song)(const char *name);
    FMUSIC_MODULE * (F_API *fmusic_load_song_ex)(void *data, int length);
    int             (F_API *fmusic_get_open_state)(FMUSIC_MODULE *mod);
    signed char     (F_API *fmusic_free_song)(FMUSIC_MODULE *mod);
    signed char     (F_API *fmusic_play_song)(FMUSIC_MODULE *mod);
    signed char     (F_API *fmusic_stop_song)(FMUSIC_MODULE *mod);
    void            (F_API *fmusic_stop_all_songs)();
    signed char     (F_API *fmusic_set_zxx_callback)(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback);
    signed char     (F_API *fmusic_set_row_callback)(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int rowstep);
    signed char     (F_API *fmusic_set_order_callback)(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int orderstep);
    signed char     (F_API *fmusic_set_inst_callback)(FMUSIC_MODULE *mod, FMUSIC_CALLBACK callback, int instrument);
    signed char     (F_API *fmusic_set_sample)(FMUSIC_MODULE *mod, int sampno, FSOUND_SAMPLE *sptr);
    signed char     (F_API *fmusic_set_user_data)(FMUSIC_MODULE *mod, unsigned int userdata);
    signed char     (F_API *fmusic_optimize_channels)(FMUSIC_MODULE *mod, int maxchannels, int minvolume);
    signed char     (F_API *fmusic_set_reverb)(signed char reverb);             /* MIDI only */
    signed char     (F_API *fmusic_set_looping)(FMUSIC_MODULE *mod, signed char looping);
    signed char     (F_API *fmusic_set_order)(FMUSIC_MODULE *mod, int order);
    signed char     (F_API *fmusic_set_paused)(FMUSIC_MODULE *mod, signed char pause);
    signed char     (F_API *fmusic_set_master_volume)(FMUSIC_MODULE *mod, int volume);
    signed char     (F_API *fmusic_set_master_speed)(FMUSIC_MODULE *mode, float speed);
    signed char     (F_API *fmusic_set_pan_seperation)(FMUSIC_MODULE *mod, float pansep);
    char *          (F_API *fmusic_get_name)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_type)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_num_orders)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_num_patterns)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_num_instruments)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_num_samples)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_num_channels)(FMUSIC_MODULE *mod);
    FSOUND_SAMPLE * (F_API *fmusic_get_sample)(FMUSIC_MODULE *mod, int sampno);
    int             (F_API *fmusic_get_pattern_length)(FMUSIC_MODULE *mod, int orderno);
    signed char     (F_API *fmusic_is_finished)(FMUSIC_MODULE *mod);
    signed char     (F_API *fmusic_is_playing)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_master_volume)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_global_volume)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_order)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_pattern)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_speed)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_bpm)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_row)(FMUSIC_MODULE *mod);
    signed char     (F_API *fmusic_get_paused)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_time)(FMUSIC_MODULE *mod);
    int             (F_API *fmusic_get_real_channel)(FMUSIC_MODULE *mod, int modchannel);
    unsigned int    (F_API *fmusic_get_user_data)(FMUSIC_MODULE *mod);
} FMOD_INSTANCE;


static FMOD_INSTANCE *fmod_create_instance(char *dll_name)
{
    FMOD_INSTANCE *instance;

    instance = (FMOD_INSTANCE *)calloc(sizeof(FMOD_INSTANCE), 1);
    if (!instance)
    {
        return NULL;
    }

#ifdef WIN32
    instance->module = load_library(dll_name);
#else
    instance->module = dlopen(dll_name, RTLD_GLOBAL | RTLD_LAZY);
#endif
    if (!instance->module)
    {
        free(instance);
        return NULL;
    }

#ifdef WIN32
    #define F_GETPROC(_x, _y)                                                                       \
    {                                                                                             \
        *((unsigned int *)&instance->_x) = (unsigned int)get_proc_address((HMODULE)instance->module, _y);    \
        if (!instance->_x)                                                                        \
        {                                                                                         \
            free_library((HMODULE)instance->module);                                                        \
            free(instance);                                                                       \
            return NULL;                                                                          \
        }                                                                                         \
    }
#else
    #define F_GETPROC(_x, _y)                                                                       \
    {                                                                                             \
        char tmp[] = _y;                                                                          \
        *(strchr(tmp, '@')) = 0;                                                                  \
        *((unsigned int *)&instance->_x) = (unsigned int)dlsym(instance->module, &tmp[1]);        \
        if (!instance->_x)                                                                        \
        {                                                                                         \
            dlclose(instance->module);                                                            \
            free(instance);                                                                       \
            return NULL;                                                                          \
        }                                                                                         \
    }
#endif

    F_GETPROC(fsound_set_output, "_FSOUND_SetOutput@4");
    F_GETPROC(fsound_set_driver, "_FSOUND_SetDriver@4");
    F_GETPROC(fsound_set_mixer, "_FSOUND_SetMixer@4");
    F_GETPROC(fsound_set_buffer_size, "_FSOUND_SetBufferSize@4");
    F_GETPROC(fsound_set_hwnd, "_FSOUND_SetHWND@4");
    F_GETPROC(fsound_set_min_hardware_channels, "_FSOUND_SetMinHardwareChannels@4");
    F_GETPROC(fsound_set_max_hardware_channels, "_FSOUND_SetMaxHardwareChannels@4");
    F_GETPROC(fsound_set_memory_system, "_FSOUND_SetMemorySystem@20");
    F_GETPROC(fsound_init, "_FSOUND_Init@12");
    F_GETPROC(fsound_close, "_FSOUND_Close@0");
    F_GETPROC(fsound_set_sfxmaster_volume, "_FSOUND_SetSFXMasterVolume@4");
    F_GETPROC(fsound_set_pan_seperation, "_FSOUND_SetPanSeperation@4");
    F_GETPROC(fsound_set_speaker_mode, "_FSOUND_SetSpeakerMode@4");
    F_GETPROC(fsound_get_error, "_FSOUND_GetError@0");
    F_GETPROC(fsound_get_version, "_FSOUND_GetVersion@0");
    F_GETPROC(fsound_get_output, "_FSOUND_GetOutput@0");
    F_GETPROC(fsound_get_output_handle, "_FSOUND_GetOutputHandle@0");
    F_GETPROC(fsound_get_driver, "_FSOUND_GetDriver@0");
    F_GETPROC(fsound_get_mixer, "_FSOUND_GetMixer@0");
    F_GETPROC(fsound_get_num_drivers, "_FSOUND_GetNumDrivers@0");
    F_GETPROC(fsound_get_driver_name, "_FSOUND_GetDriverName@4");
    F_GETPROC(fsound_get_driver_caps, "_FSOUND_GetDriverCaps@8");
    F_GETPROC(fsound_get_output_rate, "_FSOUND_GetOutputRate@0");
    F_GETPROC(fsound_get_max_channels, "_FSOUND_GetMaxChannels@0");
    F_GETPROC(fsound_get_max_samples, "_FSOUND_GetMaxSamples@0");
    F_GETPROC(fsound_get_sfxmaster_volume, "_FSOUND_GetSFXMasterVolume@0");
    F_GETPROC(fsound_get_num_hardware_channels, "_FSOUND_GetNumHardwareChannels@0");
    F_GETPROC(fsound_get_channels_playing, "_FSOUND_GetChannelsPlaying@0");
    F_GETPROC(fsound_get_cpuusage, "_FSOUND_GetCPUUsage@0");
    F_GETPROC(fsound_get_memory_stats, "_FSOUND_GetMemoryStats@8");
    F_GETPROC(fsound_sample_load, "_FSOUND_Sample_Load@16");
    F_GETPROC(fsound_sample_alloc, "_FSOUND_Sample_Alloc@28");
    F_GETPROC(fsound_sample_free, "_FSOUND_Sample_Free@4");
    F_GETPROC(fsound_sample_upload, "_FSOUND_Sample_Upload@12");
    F_GETPROC(fsound_sample_lock, "_FSOUND_Sample_Lock@28");
    F_GETPROC(fsound_sample_unlock, "_FSOUND_Sample_Unlock@20");
    F_GETPROC(fsound_sample_set_mode, "_FSOUND_Sample_SetMode@8");
    F_GETPROC(fsound_sample_set_loop_points, "_FSOUND_Sample_SetLoopPoints@12");
    F_GETPROC(fsound_sample_set_defaults, "_FSOUND_Sample_SetDefaults@20");
    F_GETPROC(fsound_sample_set_min_max_distance, "_FSOUND_Sample_SetMinMaxDistance@12");
    F_GETPROC(fsound_sample_set_max_playbacks, "_FSOUND_Sample_SetMaxPlaybacks@8");
    F_GETPROC(fsound_sample_get, "_FSOUND_Sample_Get@4");
    F_GETPROC(fsound_sample_get_name, "_FSOUND_Sample_GetName@4");
    F_GETPROC(fsound_sample_get_length, "_FSOUND_Sample_GetLength@4");
    F_GETPROC(fsound_sample_get_loop_points, "_FSOUND_Sample_GetLoopPoints@12");
    F_GETPROC(fsound_sample_get_defaults, "_FSOUND_Sample_GetDefaults@20");
    F_GETPROC(fsound_sample_get_mode, "_FSOUND_Sample_GetMode@4");
    F_GETPROC(fsound_play_sound, "_FSOUND_PlaySound@8");
    F_GETPROC(fsound_play_sound_ex, "_FSOUND_PlaySoundEx@16");
    F_GETPROC(fsound_stop_sound, "_FSOUND_StopSound@4");
    F_GETPROC(fsound_set_frequency, "_FSOUND_SetFrequency@8");
    F_GETPROC(fsound_set_volume, "_FSOUND_SetVolume@8");
    F_GETPROC(fsound_set_volume_absolute, "_FSOUND_SetVolumeAbsolute@8");
    F_GETPROC(fsound_set_pan, "_FSOUND_SetPan@8");
    F_GETPROC(fsound_set_surround, "_FSOUND_SetSurround@8");
    F_GETPROC(fsound_set_mute, "_FSOUND_SetMute@8");
    F_GETPROC(fsound_set_priority, "_FSOUND_SetPriority@8");
    F_GETPROC(fsound_set_reserved, "_FSOUND_SetReserved@8");
    F_GETPROC(fsound_set_paused, "_FSOUND_SetPaused@8");
    F_GETPROC(fsound_set_loop_mode, "_FSOUND_SetLoopMode@8");
    F_GETPROC(fsound_is_playing, "_FSOUND_IsPlaying@4");
    F_GETPROC(fsound_get_frequency, "_FSOUND_GetFrequency@4");
    F_GETPROC(fsound_get_volume, "_FSOUND_GetVolume@4");
    F_GETPROC(fsound_get_pan, "_FSOUND_GetPan@4");
    F_GETPROC(fsound_get_surround, "_FSOUND_GetSurround@4");
    F_GETPROC(fsound_get_mute, "_FSOUND_GetMute@4");
    F_GETPROC(fsound_get_priority, "_FSOUND_GetPriority@4");
    F_GETPROC(fsound_get_reserved, "_FSOUND_GetReserved@4");
    F_GETPROC(fsound_get_paused, "_FSOUND_GetPaused@4");
    F_GETPROC(fsound_get_loop_mode, "_FSOUND_GetLoopMode@4");
    F_GETPROC(fsound_get_current_position, "_FSOUND_GetCurrentPosition@4");
    F_GETPROC(fsound_set_current_position, "_FSOUND_SetCurrentPosition@8");
    F_GETPROC(fsound_get_current_sample, "_FSOUND_GetCurrentSample@4");
    F_GETPROC(fsound_get_current_levels, "_FSOUND_GetCurrentLevels@12");
    F_GETPROC(fsound_fx_enable, "_FSOUND_FX_Enable@8");
    F_GETPROC(fsound_fx_disable, "_FSOUND_FX_Disable@4");
    F_GETPROC(fsound_fx_set_chorus, "_FSOUND_FX_SetChorus@32");
    F_GETPROC(fsound_fx_set_compressor, "_FSOUND_FX_SetCompressor@28");
    F_GETPROC(fsound_fx_set_distortion, "_FSOUND_FX_SetDistortion@24");
    F_GETPROC(fsound_fx_set_echo, "_FSOUND_FX_SetEcho@24");
    F_GETPROC(fsound_fx_set_flanger, "_FSOUND_FX_SetFlanger@32");
    F_GETPROC(fsound_fx_set_gargle, "_FSOUND_FX_SetGargle@12");
    F_GETPROC(fsound_fx_set_i3dl2reverb, "_FSOUND_FX_SetI3DL2Reverb@52");
    F_GETPROC(fsound_fx_set_param_eq, "_FSOUND_FX_SetParamEQ@16");
    F_GETPROC(fsound_fx_set_waves_reverb, "_FSOUND_FX_SetWavesReverb@20");
    F_GETPROC(fsound_update, "_FSOUND_Update@0");
    F_GETPROC(fsound_3d_set_attributes, "_FSOUND_3D_SetAttributes@12");
    F_GETPROC(fsound_3d_get_attributes, "_FSOUND_3D_GetAttributes@12");
    F_GETPROC(fsound_3d_listener_set_current, "_FSOUND_3D_Listener_SetCurrent@8");
    F_GETPROC(fsound_3d_listener_set_attributes, "_FSOUND_3D_Listener_SetAttributes@32");
    F_GETPROC(fsound_3d_listener_get_attributes, "_FSOUND_3D_Listener_GetAttributes@32");
    F_GETPROC(fsound_3d_set_doppler_factor, "_FSOUND_3D_SetDopplerFactor@4");
    F_GETPROC(fsound_3d_set_distance_factor, "_FSOUND_3D_SetDistanceFactor@4");
    F_GETPROC(fsound_3d_set_rolloff_factor, "_FSOUND_3D_SetRolloffFactor@4");
    F_GETPROC(fsound_stream_open_file, "_FSOUND_Stream_OpenFile@12");
    F_GETPROC(fsound_stream_create, "_FSOUND_Stream_Create@20");
    F_GETPROC(fsound_stream_play, "_FSOUND_Stream_Play@8");
    F_GETPROC(fsound_stream_play_ex, "_FSOUND_Stream_PlayEx@16");
    F_GETPROC(fsound_stream_stop, "_FSOUND_Stream_Stop@4");
    F_GETPROC(fsound_stream_close, "_FSOUND_Stream_Close@4");
    F_GETPROC(fsound_stream_set_end_callback, "_FSOUND_Stream_SetEndCallback@12");
    F_GETPROC(fsound_stream_set_synch_callback, "_FSOUND_Stream_SetSynchCallback@12");
    F_GETPROC(fsound_stream_get_sample, "_FSOUND_Stream_GetSample@4");
    F_GETPROC(fsound_stream_create_dsp, "_FSOUND_Stream_CreateDSP@16");
    F_GETPROC(fsound_stream_set_buffer_size, "_FSOUND_Stream_SetBufferSize@4");
    F_GETPROC(fsound_stream_set_position, "_FSOUND_Stream_SetPosition@8");
    F_GETPROC(fsound_stream_get_position, "_FSOUND_Stream_GetPosition@4");
    F_GETPROC(fsound_stream_set_time, "_FSOUND_Stream_SetTime@8");
    F_GETPROC(fsound_stream_get_time, "_FSOUND_Stream_GetTime@4");
    F_GETPROC(fsound_stream_get_length, "_FSOUND_Stream_GetLength@4");
    F_GETPROC(fsound_stream_get_length_ms, "_FSOUND_Stream_GetLengthMs@4");
    F_GETPROC(fsound_stream_set_mode, "_FSOUND_Stream_SetMode@8");
    F_GETPROC(fsound_stream_get_mode, "_FSOUND_Stream_GetMode@4");
    F_GETPROC(fsound_stream_set_sub_stream, "_FSOUND_Stream_SetSubStream@8");
    F_GETPROC(fsound_stream_get_num_sub_streams, "_FSOUND_Stream_GetNumSubStreams@4");
    F_GETPROC(fsound_stream_set_sub_stream_sentence, "_FSOUND_Stream_SetSubStreamSentence@12");
    F_GETPROC(fsound_stream_set_loop_points, "_FSOUND_Stream_SetLoopPoints@12");
    F_GETPROC(fsound_stream_set_loop_count, "_FSOUND_Stream_SetLoopCount@8");
    F_GETPROC(fsound_stream_add_synch_point, "_FSOUND_Stream_AddSynchPoint@12");
    F_GETPROC(fsound_stream_delete_synch_point, "_FSOUND_Stream_DeleteSynchPoint@8");
    F_GETPROC(fsound_stream_get_num_synch_points, "_FSOUND_Stream_GetNumSynchPoints@4");
    F_GETPROC(fsound_stream_get_open_state, "_FSOUND_Stream_GetOpenState@4");
    F_GETPROC(fsound_cd_play, "_FSOUND_CD_Play@8");
    F_GETPROC(fsound_cd_set_play_mode, "_FSOUND_CD_SetPlayMode@8");
    F_GETPROC(fsound_cd_stop, "_FSOUND_CD_Stop@4");
    F_GETPROC(fsound_cd_set_paused, "_FSOUND_CD_SetPaused@8");
    F_GETPROC(fsound_cd_set_volume, "_FSOUND_CD_SetVolume@8");
    F_GETPROC(fsound_cd_set_track_time, "_FSOUND_CD_SetTrackTime@8");
    F_GETPROC(fsound_cd_eject, "_FSOUND_CD_Eject@4");
    F_GETPROC(fsound_cd_get_paused, "_FSOUND_CD_GetPaused@4");
    F_GETPROC(fsound_cd_get_track, "_FSOUND_CD_GetTrack@4");
    F_GETPROC(fsound_cd_get_num_tracks, "_FSOUND_CD_GetNumTracks@4");
    F_GETPROC(fsound_cd_get_volume, "_FSOUND_CD_GetVolume@4");
    F_GETPROC(fsound_cd_get_track_length, "_FSOUND_CD_GetTrackLength@8");
    F_GETPROC(fsound_cd_get_track_time, "_FSOUND_CD_GetTrackTime@4");
    F_GETPROC(fsound_dsp_create, "_FSOUND_DSP_Create@12");
    F_GETPROC(fsound_dsp_free, "_FSOUND_DSP_Free@4");
    F_GETPROC(fsound_dsp_set_priority, "_FSOUND_DSP_SetPriority@8");
    F_GETPROC(fsound_dsp_get_priority, "_FSOUND_DSP_GetPriority@4");
    F_GETPROC(fsound_dsp_set_active, "_FSOUND_DSP_SetActive@8");
    F_GETPROC(fsound_dsp_get_active, "_FSOUND_DSP_GetActive@4");
    F_GETPROC(fsound_dsp_get_clear_unit, "_FSOUND_DSP_GetClearUnit@0");
    F_GETPROC(fsound_dsp_get_sfxunit, "_FSOUND_DSP_GetSFXUnit@0");
    F_GETPROC(fsound_dsp_get_music_unit, "_FSOUND_DSP_GetMusicUnit@0");
    F_GETPROC(fsound_dsp_get_clip_and_copy_unit, "_FSOUND_DSP_GetClipAndCopyUnit@0");
    F_GETPROC(fsound_dsp_get_fftunit, "_FSOUND_DSP_GetFFTUnit@0");
    F_GETPROC(fsound_dsp_mix_buffers, "_FSOUND_DSP_MixBuffers@28");
    F_GETPROC(fsound_dsp_clear_mix_buffer, "_FSOUND_DSP_ClearMixBuffer@0");
    F_GETPROC(fsound_dsp_get_buffer_length, "_FSOUND_DSP_GetBufferLength@0");
    F_GETPROC(fsound_dsp_get_buffer_length_total, "_FSOUND_DSP_GetBufferLengthTotal@0");
    F_GETPROC(fsound_dsp_get_spectrum, "_FSOUND_DSP_GetSpectrum@0");
    F_GETPROC(fsound_reverb_set_properties, "_FSOUND_Reverb_SetProperties@4");
    F_GETPROC(fsound_reverb_get_properties, "_FSOUND_Reverb_GetProperties@4");
    F_GETPROC(fsound_reverb_set_channel_properties, "_FSOUND_Reverb_SetChannelProperties@8");
    F_GETPROC(fsound_reverb_get_channel_properties, "_FSOUND_Reverb_GetChannelProperties@8");
    F_GETPROC(fsound_record_set_driver, "_FSOUND_Record_SetDriver@4");
    F_GETPROC(fsound_record_get_num_drivers, "_FSOUND_Record_GetNumDrivers@0");
    F_GETPROC(fsound_record_get_driver_name, "_FSOUND_Record_GetDriverName@4");
    F_GETPROC(fsound_record_get_driver, "_FSOUND_Record_GetDriver@0");
    F_GETPROC(fsound_record_start_sample, "_FSOUND_Record_StartSample@8");
    F_GETPROC(fsound_record_stop, "_FSOUND_Record_Stop@0");
    F_GETPROC(fsound_record_get_position, "_FSOUND_Record_GetPosition@0");
    F_GETPROC(fsound_file_set_callbacks, "_FSOUND_File_SetCallbacks@20");
    F_GETPROC(fmusic_load_song, "_FMUSIC_LoadSong@4");
    F_GETPROC(fmusic_load_song_ex, "_FMUSIC_LoadSongEx@20");
    F_GETPROC(fmusic_get_open_state, "_FMUSIC_GetOpenState@4");
    F_GETPROC(fmusic_free_song, "_FMUSIC_FreeSong@4");
    F_GETPROC(fmusic_play_song, "_FMUSIC_PlaySong@4");
    F_GETPROC(fmusic_stop_song, "_FMUSIC_StopSong@4");
    F_GETPROC(fmusic_stop_all_songs, "_FMUSIC_StopAllSongs@0");
    F_GETPROC(fmusic_set_zxx_callback, "_FMUSIC_SetZxxCallback@8");
    F_GETPROC(fmusic_set_row_callback, "_FMUSIC_SetRowCallback@12");
    F_GETPROC(fmusic_set_order_callback, "_FMUSIC_SetOrderCallback@12");
    F_GETPROC(fmusic_set_inst_callback, "_FMUSIC_SetInstCallback@12");
    F_GETPROC(fmusic_set_sample, "_FMUSIC_SetSample@12");
    F_GETPROC(fmusic_set_user_data, "_FMUSIC_SetUserData@8");
    F_GETPROC(fmusic_optimize_channels, "_FMUSIC_OptimizeChannels@12");
    F_GETPROC(fmusic_set_reverb, "_FMUSIC_SetReverb@4");
    F_GETPROC(fmusic_set_looping, "_FMUSIC_SetLooping@8");
    F_GETPROC(fmusic_set_order, "_FMUSIC_SetOrder@8");
    F_GETPROC(fmusic_set_paused, "_FMUSIC_SetPaused@8");
    F_GETPROC(fmusic_set_master_volume, "_FMUSIC_SetMasterVolume@8");
    F_GETPROC(fmusic_set_master_speed, "_FMUSIC_SetMasterSpeed@8");
    F_GETPROC(fmusic_set_pan_seperation, "_FMUSIC_SetPanSeperation@8");
    F_GETPROC(fmusic_get_name, "_FMUSIC_GetName@4");
    F_GETPROC(fmusic_get_type, "_FMUSIC_GetType@4");
    F_GETPROC(fmusic_get_num_orders, "_FMUSIC_GetNumOrders@4");
    F_GETPROC(fmusic_get_num_patterns, "_FMUSIC_GetNumPatterns@4");
    F_GETPROC(fmusic_get_num_instruments, "_FMUSIC_GetNumInstruments@4");
    F_GETPROC(fmusic_get_num_samples, "_FMUSIC_GetNumSamples@4");
    F_GETPROC(fmusic_get_num_channels, "_FMUSIC_GetNumChannels@4");
    F_GETPROC(fmusic_get_sample, "_FMUSIC_GetSample@8");
    F_GETPROC(fmusic_get_pattern_length, "_FMUSIC_GetPatternLength@8");
    F_GETPROC(fmusic_is_finished, "_FMUSIC_IsFinished@4");
    F_GETPROC(fmusic_is_playing, "_FMUSIC_IsPlaying@4");
    F_GETPROC(fmusic_get_master_volume, "_FMUSIC_GetMasterVolume@4");
    F_GETPROC(fmusic_get_global_volume, "_FMUSIC_GetGlobalVolume@4");
    F_GETPROC(fmusic_get_order, "_FMUSIC_GetOrder@4");
    F_GETPROC(fmusic_get_pattern, "_FMUSIC_GetPattern@4");
    F_GETPROC(fmusic_get_speed, "_FMUSIC_GetSpeed@4");
    F_GETPROC(fmusic_get_bpm, "_FMUSIC_GetBPM@4");
    F_GETPROC(fmusic_get_row, "_FMUSIC_GetRow@4");
    F_GETPROC(fmusic_get_paused, "_FMUSIC_GetPaused@4");
    F_GETPROC(fmusic_get_time, "_FMUSIC_GetTime@4");
    F_GETPROC(fmusic_get_real_channel, "_FMUSIC_GetRealChannel@8");
    F_GETPROC(fmusic_get_user_data, "_FMUSIC_GetUserData@4");

    return instance;
}

static void fmod_free_instance(FMOD_INSTANCE *instance)
{
    if (instance)
    {
        if (instance->module)
        {
#ifdef WIN32
            free_library((HMODULE)instance->module);
#else
            dlclose(instance->module);
#endif
        }
        free(instance);
    }
}

#endif


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
 "@(#) $Id: SDL_opengl.h,v 1.3 2005-07-27 12:26:53 bb Exp $";
#endif

/* This is a simple file to encapsulate the OpenGL API headers */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX    /* Don't defined min() and max() */
#endif
#include <windows.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>  /* Header File For The OpenGL Library */
#include <OpenGL/glu.h> /* Header File For The GLU Library */
#else
#ifndef NO_SDL_GLEXT
#define __glext_h_  /* Don't let gl.h include glext.h */
#endif
#include <GL/gl.h>  /* Header File For The OpenGL Library */
#include <GL/glu.h> /* Header File For The GLU Library */
#ifndef NO_SDL_GLEXT
#undef __glext_h_
#endif
#endif

/* This file taken from "GLext.h" from the Jeff Molofee OpenGL tutorials.
   It is included here because glext.h is not available on some systems.
   If you don't want this version included, simply define "NO_SDL_GLEXT"
 */
#ifndef NO_SDL_GLEXT
#if !defined(__glext_h_) && !defined(GL_GLEXT_LEGACY)
#define __glext_h_

#ifdef __cplusplus
extern "C"
{
#endif

/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
**
** http://oss.sgi.com/projects/FreeB
**
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
**
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2002 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
**
** Additional Notice Provisions: This software was created using the
** OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
** not been independently verified as being compliant with the OpenGL(R)
** version 1.2.1 Specification.
*/

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif

/*************************************************************/

/* Header file version number, required by OpenGL ABI for Linux */
/* glext.h last updated 2003/1/12 */
/* Current version at http://oss.sgi.com/projects/ogl-sample/registry/ */
#define GL_GLEXT_VERSION 21

#ifndef GL_VERSION_1_2
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_RESCALE_NORMAL                 0x803A
#define GL_TEXTURE_BINDING_3D             0x806A
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E
#define GL_TEXTURE_3D                     0x806F
#define GL_PROXY_TEXTURE_3D               0x8070
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_MAX_3D_TEXTURE_SIZE            0x8073
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368
#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1
#define GL_MAX_ELEMENTS_VERTICES          0x80E8
#define GL_MAX_ELEMENTS_INDICES           0x80E9
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_LIGHT_MODEL_COLOR_CONTROL      0x81F8
#define GL_SINGLE_COLOR                   0x81F9
#define GL_SEPARATE_SPECULAR_COLOR        0x81FA
#define GL_SMOOTH_POINT_SIZE_RANGE        0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY  0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE        0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY  0x0B23
#define GL_ALIASED_POINT_SIZE_RANGE       0x846D
#define GL_ALIASED_LINE_WIDTH_RANGE       0x846E
#endif

#ifndef GL_ARB_imaging
#define GL_CONSTANT_COLOR                 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
#define GL_CONSTANT_ALPHA                 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
#define GL_BLEND_COLOR                    0x8005
#define GL_FUNC_ADD                       0x8006
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008
#define GL_BLEND_EQUATION                 0x8009
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#define GL_CONVOLUTION_1D                 0x8010
#define GL_CONVOLUTION_2D                 0x8011
#define GL_SEPARABLE_2D                   0x8012
#define GL_CONVOLUTION_BORDER_MODE        0x8013
#define GL_CONVOLUTION_FILTER_SCALE       0x8014
#define GL_CONVOLUTION_FILTER_BIAS        0x8015
#define GL_REDUCE                         0x8016
#define GL_CONVOLUTION_FORMAT             0x8017
#define GL_CONVOLUTION_WIDTH              0x8018
#define GL_CONVOLUTION_HEIGHT             0x8019
#define GL_MAX_CONVOLUTION_WIDTH          0x801A
#define GL_MAX_CONVOLUTION_HEIGHT         0x801B
#define GL_POST_CONVOLUTION_RED_SCALE     0x801C
#define GL_POST_CONVOLUTION_GREEN_SCALE   0x801D
#define GL_POST_CONVOLUTION_BLUE_SCALE    0x801E
#define GL_POST_CONVOLUTION_ALPHA_SCALE   0x801F
#define GL_POST_CONVOLUTION_RED_BIAS      0x8020
#define GL_POST_CONVOLUTION_GREEN_BIAS    0x8021
#define GL_POST_CONVOLUTION_BLUE_BIAS     0x8022
#define GL_POST_CONVOLUTION_ALPHA_BIAS    0x8023
#define GL_HISTOGRAM                      0x8024
#define GL_PROXY_HISTOGRAM                0x8025
#define GL_HISTOGRAM_WIDTH                0x8026
#define GL_HISTOGRAM_FORMAT               0x8027
#define GL_HISTOGRAM_RED_SIZE             0x8028
#define GL_HISTOGRAM_GREEN_SIZE           0x8029
#define GL_HISTOGRAM_BLUE_SIZE            0x802A
#define GL_HISTOGRAM_ALPHA_SIZE           0x802B
#define GL_HISTOGRAM_LUMINANCE_SIZE       0x802C
#define GL_HISTOGRAM_SINK                 0x802D
#define GL_MINMAX                         0x802E
#define GL_MINMAX_FORMAT                  0x802F
#define GL_MINMAX_SINK                    0x8030
#define GL_TABLE_TOO_LARGE                0x8031
#define GL_COLOR_MATRIX                   0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH       0x80B2
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH   0x80B3
#define GL_POST_COLOR_MATRIX_RED_SCALE    0x80B4
#define GL_POST_COLOR_MATRIX_GREEN_SCALE  0x80B5
#define GL_POST_COLOR_MATRIX_BLUE_SCALE   0x80B6
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE  0x80B7
#define GL_POST_COLOR_MATRIX_RED_BIAS     0x80B8
#define GL_POST_COLOR_MATRIX_GREEN_BIAS   0x80B9
#define GL_POST_COLOR_MATRIX_BLUE_BIAS    0x80BA
#define GL_POST_COLOR_MATRIX_ALPHA_BIAS   0x80BB
#define GL_COLOR_TABLE                    0x80D0
#define GL_POST_CONVOLUTION_COLOR_TABLE   0x80D1
#define GL_POST_COLOR_MATRIX_COLOR_TABLE  0x80D2
#define GL_PROXY_COLOR_TABLE              0x80D3
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE 0x80D4
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE 0x80D5
#define GL_COLOR_TABLE_SCALE              0x80D6
#define GL_COLOR_TABLE_BIAS               0x80D7
#define GL_COLOR_TABLE_FORMAT             0x80D8
#define GL_COLOR_TABLE_WIDTH              0x80D9
#define GL_COLOR_TABLE_RED_SIZE           0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE         0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE          0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE         0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE     0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE     0x80DF
#define GL_CONSTANT_BORDER                0x8151
#define GL_REPLICATE_BORDER               0x8153
#define GL_CONVOLUTION_BORDER_COLOR       0x8154
#endif

#ifndef GL_VERSION_1_3
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF
#define GL_ACTIVE_TEXTURE                 0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE          0x84E1
#define GL_MAX_TEXTURE_UNITS              0x84E2
#define GL_TRANSPOSE_MODELVIEW_MATRIX     0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX    0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX       0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX         0x84E6
#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_SAMPLE_BUFFERS                 0x80A8
#define GL_SAMPLES                        0x80A9
#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
#define GL_MULTISAMPLE_BIT                0x20000000
#define GL_NORMAL_MAP                     0x8511
#define GL_REFLECTION_MAP                 0x8512
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP         0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE      0x851C
#define GL_COMPRESSED_ALPHA               0x84E9
#define GL_COMPRESSED_LUMINANCE           0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA     0x84EB
#define GL_COMPRESSED_INTENSITY           0x84EC
#define GL_COMPRESSED_RGB                 0x84ED
#define GL_COMPRESSED_RGBA                0x84EE
#define GL_TEXTURE_COMPRESSION_HINT       0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE  0x86A0
#define GL_TEXTURE_COMPRESSED             0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS     0x86A3
#define GL_CLAMP_TO_BORDER                0x812D
#define GL_CLAMP_TO_BORDER_SGIS           0x812D
#define GL_COMBINE                        0x8570
#define GL_COMBINE_RGB                    0x8571
#define GL_COMBINE_ALPHA                  0x8572
#define GL_SOURCE0_RGB                    0x8580
#define GL_SOURCE1_RGB                    0x8581
#define GL_SOURCE2_RGB                    0x8582
#define GL_SOURCE0_ALPHA                  0x8588
#define GL_SOURCE1_ALPHA                  0x8589
#define GL_SOURCE2_ALPHA                  0x858A
#define GL_OPERAND0_RGB                   0x8590
#define GL_OPERAND1_RGB                   0x8591
#define GL_OPERAND2_RGB                   0x8592
#define GL_OPERAND0_ALPHA                 0x8598
#define GL_OPERAND1_ALPHA                 0x8599
#define GL_OPERAND2_ALPHA                 0x859A
#define GL_RGB_SCALE                      0x8573
#define GL_ADD_SIGNED                     0x8574
#define GL_INTERPOLATE                    0x8575
#define GL_SUBTRACT                       0x84E7
#define GL_CONSTANT                       0x8576
#define GL_PRIMARY_COLOR                  0x8577
#define GL_PREVIOUS                       0x8578
#define GL_DOT3_RGB                       0x86AE
#define GL_DOT3_RGBA                      0x86AF
#endif

#ifndef GL_VERSION_1_4
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_BLEND_SRC_ALPHA                0x80CB
#define GL_POINT_SIZE_MIN                 0x8126
#define GL_POINT_SIZE_MAX                 0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE      0x8128
#define GL_POINT_DISTANCE_ATTENUATION     0x8129
#define GL_GENERATE_MIPMAP                0x8191
#define GL_GENERATE_MIPMAP_HINT           0x8192
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_MIRRORED_REPEAT                0x8370
#define GL_FOG_COORDINATE_SOURCE          0x8450
#define GL_FOG_COORDINATE                 0x8451
#define GL_FRAGMENT_DEPTH                 0x8452
#define GL_CURRENT_FOG_COORDINATE         0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE      0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE    0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER   0x8456
#define GL_FOG_COORDINATE_ARRAY           0x8457
#define GL_COLOR_SUM                      0x8458
#define GL_CURRENT_SECONDARY_COLOR        0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE     0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE     0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE   0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER  0x845D
#define GL_SECONDARY_COLOR_ARRAY          0x845E
#define GL_MAX_TEXTURE_LOD_BIAS           0x84FD
#define GL_TEXTURE_FILTER_CONTROL         0x8500
#define GL_TEXTURE_LOD_BIAS               0x8501
#define GL_INCR_WRAP                      0x8507
#define GL_DECR_WRAP                      0x8508
#define GL_TEXTURE_DEPTH_SIZE             0x884A
#define GL_DEPTH_TEXTURE_MODE             0x884B
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_TEXTURE_COMPARE_FUNC           0x884D
#define GL_COMPARE_R_TO_TEXTURE           0x884E
#endif

#ifndef GL_VERSION_1_5
#define GL_BUFFER_SIZE                    0x8764
#define GL_BUFFER_USAGE                   0x8765
#define GL_QUERY_COUNTER_BITS             0x8864
#define GL_CURRENT_QUERY                  0x8865
#define GL_QUERY_RESULT                   0x8866
#define GL_QUERY_RESULT_AVAILABLE         0x8867
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING    0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING    0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING     0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING     0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING 0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING 0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING 0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING    0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA
#define GL_SAMPLES_PASSED                 0x8914
#define GL_FOG_COORD_SOURCE               GL_FOG_COORDINATE_SOURCE
#define GL_FOG_COORD                      GL_FOG_COORDINATE
#define GL_CURRENT_FOG_COORD              GL_CURRENT_FOG_COORDINATE
#define GL_FOG_COORD_ARRAY_TYPE           GL_FOG_COORDINATE_ARRAY_TYPE
#define GL_FOG_COORD_ARRAY_STRIDE         GL_FOG_COORDINATE_ARRAY_STRIDE
#define GL_FOG_COORD_ARRAY_POINTER        GL_FOG_COORDINATE_ARRAY_POINTER
#define GL_FOG_COORD_ARRAY                GL_FOG_COORDINATE_ARRAY
#define GL_FOG_COORD_ARRAY_BUFFER_BINDING GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING
#define GL_SRC0_RGB                       GL_SOURCE0_RGB
#define GL_SRC1_RGB                       GL_SOURCE1_RGB
#define GL_SRC2_RGB                       GL_SOURCE2_RGB
#define GL_SRC0_ALPHA                     GL_SOURCE0_ALPHA
#define GL_SRC1_ALPHA                     GL_SOURCE1_ALPHA
#define GL_SRC2_ALPHA                     GL_SOURCE2_ALPHA
#endif

#ifndef GL_ARB_multitexture
#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE4_ARB                   0x84C4
#define GL_TEXTURE5_ARB                   0x84C5
#define GL_TEXTURE6_ARB                   0x84C6
#define GL_TEXTURE7_ARB                   0x84C7
#define GL_TEXTURE8_ARB                   0x84C8
#define GL_TEXTURE9_ARB                   0x84C9
#define GL_TEXTURE10_ARB                  0x84CA
#define GL_TEXTURE11_ARB                  0x84CB
#define GL_TEXTURE12_ARB                  0x84CC
#define GL_TEXTURE13_ARB                  0x84CD
#define GL_TEXTURE14_ARB                  0x84CE
#define GL_TEXTURE15_ARB                  0x84CF
#define GL_TEXTURE16_ARB                  0x84D0
#define GL_TEXTURE17_ARB                  0x84D1
#define GL_TEXTURE18_ARB                  0x84D2
#define GL_TEXTURE19_ARB                  0x84D3
#define GL_TEXTURE20_ARB                  0x84D4
#define GL_TEXTURE21_ARB                  0x84D5
#define GL_TEXTURE22_ARB                  0x84D6
#define GL_TEXTURE23_ARB                  0x84D7
#define GL_TEXTURE24_ARB                  0x84D8
#define GL_TEXTURE25_ARB                  0x84D9
#define GL_TEXTURE26_ARB                  0x84DA
#define GL_TEXTURE27_ARB                  0x84DB
#define GL_TEXTURE28_ARB                  0x84DC
#define GL_TEXTURE29_ARB                  0x84DD
#define GL_TEXTURE30_ARB                  0x84DE
#define GL_TEXTURE31_ARB                  0x84DF
#define GL_ACTIVE_TEXTURE_ARB             0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB      0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB          0x84E2
#endif

#ifndef GL_ARB_transpose_matrix
#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB 0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB 0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB   0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX_ARB     0x84E6
#endif

#ifndef GL_ARB_multisample
#define GL_MULTISAMPLE_ARB                0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB   0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_ARB        0x809F
#define GL_SAMPLE_COVERAGE_ARB            0x80A0
#define GL_SAMPLE_BUFFERS_ARB             0x80A8
#define GL_SAMPLES_ARB                    0x80A9
#define GL_SAMPLE_COVERAGE_VALUE_ARB      0x80AA
#define GL_SAMPLE_COVERAGE_INVERT_ARB     0x80AB
#define GL_MULTISAMPLE_BIT_ARB            0x20000000
#endif

#ifndef GL_ARB_texture_env_add
#endif

#ifndef GL_ARB_texture_cube_map
#define GL_NORMAL_MAP_ARB                 0x8511
#define GL_REFLECTION_MAP_ARB             0x8512
#define GL_TEXTURE_CUBE_MAP_ARB           0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB   0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB     0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB  0x851C
#endif

#ifndef GL_ARB_texture_compression
#define GL_COMPRESSED_ALPHA_ARB           0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB       0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB 0x84EB
#define GL_COMPRESSED_INTENSITY_ARB       0x84EC
#define GL_COMPRESSED_RGB_ARB             0x84ED
#define GL_COMPRESSED_RGBA_ARB            0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB   0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB 0x86A0
#define GL_TEXTURE_COMPRESSED_ARB         0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A3
#endif

#ifndef GL_ARB_texture_border_clamp
#define GL_CLAMP_TO_BORDER_ARB            0x812D
#endif

#ifndef GL_ARB_point_parameters
#define GL_POINT_SIZE_MIN_ARB             0x8126
#define GL_POINT_SIZE_MAX_ARB             0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_ARB  0x8128
#define GL_POINT_DISTANCE_ATTENUATION_ARB 0x8129
#endif

#ifndef GL_ARB_vertex_blend
#define GL_MAX_VERTEX_UNITS_ARB           0x86A4
#define GL_ACTIVE_VERTEX_UNITS_ARB        0x86A5
#define GL_WEIGHT_SUM_UNITY_ARB           0x86A6
#define GL_VERTEX_BLEND_ARB               0x86A7
#define GL_CURRENT_WEIGHT_ARB             0x86A8
#define GL_WEIGHT_ARRAY_TYPE_ARB          0x86A9
#define GL_WEIGHT_ARRAY_STRIDE_ARB        0x86AA
#define GL_WEIGHT_ARRAY_SIZE_ARB          0x86AB
#define GL_WEIGHT_ARRAY_POINTER_ARB       0x86AC
#define GL_WEIGHT_ARRAY_ARB               0x86AD
#define GL_MODELVIEW0_ARB                 0x1700
#define GL_MODELVIEW1_ARB                 0x850A
#define GL_MODELVIEW2_ARB                 0x8722
#define GL_MODELVIEW3_ARB                 0x8723
#define GL_MODELVIEW4_ARB                 0x8724
#define GL_MODELVIEW5_ARB                 0x8725
#define GL_MODELVIEW6_ARB                 0x8726
#define GL_MODELVIEW7_ARB                 0x8727
#define GL_MODELVIEW8_ARB                 0x8728
#define GL_MODELVIEW9_ARB                 0x8729
#define GL_MODELVIEW10_ARB                0x872A
#define GL_MODELVIEW11_ARB                0x872B
#define GL_MODELVIEW12_ARB                0x872C
#define GL_MODELVIEW13_ARB                0x872D
#define GL_MODELVIEW14_ARB                0x872E
#define GL_MODELVIEW15_ARB                0x872F
#define GL_MODELVIEW16_ARB                0x8730
#define GL_MODELVIEW17_ARB                0x8731
#define GL_MODELVIEW18_ARB                0x8732
#define GL_MODELVIEW19_ARB                0x8733
#define GL_MODELVIEW20_ARB                0x8734
#define GL_MODELVIEW21_ARB                0x8735
#define GL_MODELVIEW22_ARB                0x8736
#define GL_MODELVIEW23_ARB                0x8737
#define GL_MODELVIEW24_ARB                0x8738
#define GL_MODELVIEW25_ARB                0x8739
#define GL_MODELVIEW26_ARB                0x873A
#define GL_MODELVIEW27_ARB                0x873B
#define GL_MODELVIEW28_ARB                0x873C
#define GL_MODELVIEW29_ARB                0x873D
#define GL_MODELVIEW30_ARB                0x873E
#define GL_MODELVIEW31_ARB                0x873F
#endif

#ifndef GL_ARB_matrix_palette
#define GL_MATRIX_PALETTE_ARB             0x8840
#define GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB 0x8841
#define GL_MAX_PALETTE_MATRICES_ARB       0x8842
#define GL_CURRENT_PALETTE_MATRIX_ARB     0x8843
#define GL_MATRIX_INDEX_ARRAY_ARB         0x8844
#define GL_CURRENT_MATRIX_INDEX_ARB       0x8845
#define GL_MATRIX_INDEX_ARRAY_SIZE_ARB    0x8846
#define GL_MATRIX_INDEX_ARRAY_TYPE_ARB    0x8847
#define GL_MATRIX_INDEX_ARRAY_STRIDE_ARB  0x8848
#define GL_MATRIX_INDEX_ARRAY_POINTER_ARB 0x8849
#endif

#ifndef GL_ARB_texture_env_combine
#define GL_COMBINE_ARB                    0x8570
#define GL_COMBINE_RGB_ARB                0x8571
#define GL_COMBINE_ALPHA_ARB              0x8572
#define GL_SOURCE0_RGB_ARB                0x8580
#define GL_SOURCE1_RGB_ARB                0x8581
#define GL_SOURCE2_RGB_ARB                0x8582
#define GL_SOURCE0_ALPHA_ARB              0x8588
#define GL_SOURCE1_ALPHA_ARB              0x8589
#define GL_SOURCE2_ALPHA_ARB              0x858A
#define GL_OPERAND0_RGB_ARB               0x8590
#define GL_OPERAND1_RGB_ARB               0x8591
#define GL_OPERAND2_RGB_ARB               0x8592
#define GL_OPERAND0_ALPHA_ARB             0x8598
#define GL_OPERAND1_ALPHA_ARB             0x8599
#define GL_OPERAND2_ALPHA_ARB             0x859A
#define GL_RGB_SCALE_ARB                  0x8573
#define GL_ADD_SIGNED_ARB                 0x8574
#define GL_INTERPOLATE_ARB                0x8575
#define GL_SUBTRACT_ARB                   0x84E7
#define GL_CONSTANT_ARB                   0x8576
#define GL_PRIMARY_COLOR_ARB              0x8577
#define GL_PREVIOUS_ARB                   0x8578
#endif

#ifndef GL_ARB_texture_env_crossbar
#endif

#ifndef GL_ARB_texture_env_dot3
#define GL_DOT3_RGB_ARB                   0x86AE
#define GL_DOT3_RGBA_ARB                  0x86AF
#endif

#ifndef GL_ARB_texture_mirrored_repeat
#define GL_MIRRORED_REPEAT_ARB            0x8370
#endif

#ifndef GL_ARB_depth_texture
#define GL_DEPTH_COMPONENT16_ARB          0x81A5
#define GL_DEPTH_COMPONENT24_ARB          0x81A6
#define GL_DEPTH_COMPONENT32_ARB          0x81A7
#define GL_TEXTURE_DEPTH_SIZE_ARB         0x884A
#define GL_DEPTH_TEXTURE_MODE_ARB         0x884B
#endif

#ifndef GL_ARB_shadow
#define GL_TEXTURE_COMPARE_MODE_ARB       0x884C
#define GL_TEXTURE_COMPARE_FUNC_ARB       0x884D
#define GL_COMPARE_R_TO_TEXTURE_ARB       0x884E
#endif

#ifndef GL_ARB_shadow_ambient
#define GL_TEXTURE_COMPARE_FAIL_VALUE_ARB 0x80BF
#endif

#ifndef GL_ARB_window_pos
#endif

#ifndef GL_ARB_vertex_program
#define GL_COLOR_SUM_ARB                  0x8458
#define GL_VERTEX_PROGRAM_ARB             0x8620
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB   0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB   0x8625
#define GL_CURRENT_VERTEX_ATTRIB_ARB      0x8626
#define GL_PROGRAM_LENGTH_ARB             0x8627
#define GL_PROGRAM_STRING_ARB             0x8628
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB 0x862E
#define GL_MAX_PROGRAM_MATRICES_ARB       0x862F
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB 0x8640
#define GL_CURRENT_MATRIX_ARB             0x8641
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB  0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB    0x8643
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB 0x8645
#define GL_PROGRAM_ERROR_POSITION_ARB     0x864B
#define GL_PROGRAM_BINDING_ARB            0x8677
#define GL_MAX_VERTEX_ATTRIBS_ARB         0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB 0x886A
#define GL_PROGRAM_ERROR_STRING_ARB       0x8874
#define GL_PROGRAM_FORMAT_ASCII_ARB       0x8875
#define GL_PROGRAM_FORMAT_ARB             0x8876
#define GL_PROGRAM_INSTRUCTIONS_ARB       0x88A0
#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB   0x88A1
#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A2
#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A3
#define GL_PROGRAM_TEMPORARIES_ARB        0x88A4
#define GL_MAX_PROGRAM_TEMPORARIES_ARB    0x88A5
#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A6
#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A7
#define GL_PROGRAM_PARAMETERS_ARB         0x88A8
#define GL_MAX_PROGRAM_PARAMETERS_ARB     0x88A9
#define GL_PROGRAM_NATIVE_PARAMETERS_ARB  0x88AA
#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AB
#define GL_PROGRAM_ATTRIBS_ARB            0x88AC
#define GL_MAX_PROGRAM_ATTRIBS_ARB        0x88AD
#define GL_PROGRAM_NATIVE_ATTRIBS_ARB     0x88AE
#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB 0x88AF
#define GL_PROGRAM_ADDRESS_REGISTERS_ARB  0x88B0
#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB 0x88B1
#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B2
#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B3
#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB 0x88B4
#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB 0x88B5
#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB 0x88B6
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB   0x88B7
#define GL_MATRIX0_ARB                    0x88C0
#define GL_MATRIX1_ARB                    0x88C1
#define GL_MATRIX2_ARB                    0x88C2
#define GL_MATRIX3_ARB                    0x88C3
#define GL_MATRIX4_ARB                    0x88C4
#define GL_MATRIX5_ARB                    0x88C5
#define GL_MATRIX6_ARB                    0x88C6
#define GL_MATRIX7_ARB                    0x88C7
#define GL_MATRIX8_ARB                    0x88C8
#define GL_MATRIX9_ARB                    0x88C9
#define GL_MATRIX10_ARB                   0x88CA
#define GL_MATRIX11_ARB                   0x88CB
#define GL_MATRIX12_ARB                   0x88CC
#define GL_MATRIX13_ARB                   0x88CD
#define GL_MATRIX14_ARB                   0x88CE
#define GL_MATRIX15_ARB                   0x88CF
#define GL_MATRIX16_ARB                   0x88D0
#define GL_MATRIX17_ARB                   0x88D1
#define GL_MATRIX18_ARB                   0x88D2
#define GL_MATRIX19_ARB                   0x88D3
#define GL_MATRIX20_ARB                   0x88D4
#define GL_MATRIX21_ARB                   0x88D5
#define GL_MATRIX22_ARB                   0x88D6
#define GL_MATRIX23_ARB                   0x88D7
#define GL_MATRIX24_ARB                   0x88D8
#define GL_MATRIX25_ARB                   0x88D9
#define GL_MATRIX26_ARB                   0x88DA
#define GL_MATRIX27_ARB                   0x88DB
#define GL_MATRIX28_ARB                   0x88DC
#define GL_MATRIX29_ARB                   0x88DD
#define GL_MATRIX30_ARB                   0x88DE
#define GL_MATRIX31_ARB                   0x88DF
#endif

#ifndef GL_ARB_fragment_program
#define GL_FRAGMENT_PROGRAM_ARB           0x8804
#define GL_PROGRAM_ALU_INSTRUCTIONS_ARB   0x8805
#define GL_PROGRAM_TEX_INSTRUCTIONS_ARB   0x8806
#define GL_PROGRAM_TEX_INDIRECTIONS_ARB   0x8807
#define GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB 0x8808
#define GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB 0x8809
#define GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB 0x880A
#define GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB 0x880B
#define GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB 0x880C
#define GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB 0x880D
#define GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB 0x880E
#define GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB 0x880F
#define GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB 0x8810
#define GL_MAX_TEXTURE_COORDS_ARB         0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB    0x8872
#endif

#ifndef GL_ARB_vertex_buffer_object
#define GL_BUFFER_SIZE_ARB                0x8764
#define GL_BUFFER_USAGE_ARB               0x8765
#define GL_ARRAY_BUFFER_ARB               0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB       0x8893
#define GL_ARRAY_BUFFER_BINDING_ARB       0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB 0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB 0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB 0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING_ARB 0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING_ARB 0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB 0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB 0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB 0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB 0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB 0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB 0x889F
#define GL_READ_ONLY_ARB                  0x88B8
#define GL_WRITE_ONLY_ARB                 0x88B9
#define GL_READ_WRITE_ARB                 0x88BA
#define GL_BUFFER_ACCESS_ARB              0x88BB
#define GL_BUFFER_MAPPED_ARB              0x88BC
#define GL_BUFFER_MAP_POINTER_ARB         0x88BD
#define GL_STREAM_DRAW_ARB                0x88E0
#define GL_STREAM_READ_ARB                0x88E1
#define GL_STREAM_COPY_ARB                0x88E2
#define GL_STATIC_DRAW_ARB                0x88E4
#define GL_STATIC_READ_ARB                0x88E5
#define GL_STATIC_COPY_ARB                0x88E6
#define GL_DYNAMIC_DRAW_ARB               0x88E8
#define GL_DYNAMIC_READ_ARB               0x88E9
#define GL_DYNAMIC_COPY_ARB               0x88EA
#endif

#ifndef GL_ARB_occlusion_query
#define GL_QUERY_COUNTER_BITS_ARB         0x8864
#define GL_CURRENT_QUERY_ARB              0x8865
#define GL_QUERY_RESULT_ARB               0x8866
#define GL_QUERY_RESULT_AVAILABLE_ARB     0x8867
#define GL_SAMPLES_PASSED_ARB             0x8914
#endif

#ifndef GL_ARB_shader_objects
#define GL_PROGRAM_OBJECT_ARB             0x8B40
#define GL_SHADER_OBJECT_ARB              0x8B48
#define GL_OBJECT_TYPE_ARB                0x8B4E
#define GL_OBJECT_SUBTYPE_ARB             0x8B4F
#define GL_FLOAT_VEC2_ARB                 0x8B50
#define GL_FLOAT_VEC3_ARB                 0x8B51
#define GL_FLOAT_VEC4_ARB                 0x8B52
#define GL_INT_VEC2_ARB                   0x8B53
#define GL_INT_VEC3_ARB                   0x8B54
#define GL_INT_VEC4_ARB                   0x8B55
#define GL_BOOL_ARB                       0x8B56
#define GL_BOOL_VEC2_ARB                  0x8B57
#define GL_BOOL_VEC3_ARB                  0x8B58
#define GL_BOOL_VEC4_ARB                  0x8B59
#define GL_FLOAT_MAT2_ARB                 0x8B5A
#define GL_FLOAT_MAT3_ARB                 0x8B5B
#define GL_FLOAT_MAT4_ARB                 0x8B5C
#define GL_OBJECT_DELETE_STATUS_ARB       0x8B80
#define GL_OBJECT_COMPILE_STATUS_ARB      0x8B81
#define GL_OBJECT_LINK_STATUS_ARB         0x8B82
#define GL_OBJECT_VALIDATE_STATUS_ARB     0x8B83
#define GL_OBJECT_INFO_LOG_LENGTH_ARB     0x8B84
#define GL_OBJECT_ATTACHED_OBJECTS_ARB    0x8B85
#define GL_OBJECT_ACTIVE_UNIFORMS_ARB     0x8B86
#define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB 0x8B87
#define GL_OBJECT_SHADER_SOURCE_LENGTH_ARB 0x8B88
#endif

#ifndef GL_ARB_vertex_shader
#define GL_VERTEX_SHADER_ARB              0x8B31
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB 0x8B4A
#define GL_MAX_VARYING_FLOATS_ARB         0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB 0x8B4D
#define GL_OBJECT_ACTIVE_ATTRIBUTES_ARB   0x8B89
#define GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB 0x8B8A
#endif

#ifndef GL_ARB_fragment_shader
#define GL_FRAGMENT_SHADER_ARB            0x8B30
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB 0x8B49
#endif

#ifndef GL_ARB_shading_language_100
#endif

#ifndef GL_ARB_texture_non_power_of_two
#endif

#ifndef GL_ARB_point_sprite
#define GL_POINT_SPRITE_ARB               0x8861
#define GL_COORD_REPLACE_ARB              0x8862
#endif

#ifndef GL_EXT_abgr
#define GL_ABGR_EXT                       0x8000
#endif

#ifndef GL_EXT_blend_color
#define GL_CONSTANT_COLOR_EXT             0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT   0x8002
#define GL_CONSTANT_ALPHA_EXT             0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT   0x8004
#define GL_BLEND_COLOR_EXT                0x8005
#endif

#ifndef GL_EXT_polygon_offset
#define GL_POLYGON_OFFSET_EXT             0x8037
#define GL_POLYGON_OFFSET_FACTOR_EXT      0x8038
#define GL_POLYGON_OFFSET_BIAS_EXT        0x8039
#endif

#ifndef GL_EXT_texture
#define GL_ALPHA4_EXT                     0x803B
#define GL_ALPHA8_EXT                     0x803C
#define GL_ALPHA12_EXT                    0x803D
#define GL_ALPHA16_EXT                    0x803E
#define GL_LUMINANCE4_EXT                 0x803F
#define GL_LUMINANCE8_EXT                 0x8040
#define GL_LUMINANCE12_EXT                0x8041
#define GL_LUMINANCE16_EXT                0x8042
#define GL_LUMINANCE4_ALPHA4_EXT          0x8043
#define GL_LUMINANCE6_ALPHA2_EXT          0x8044
#define GL_LUMINANCE8_ALPHA8_EXT          0x8045
#define GL_LUMINANCE12_ALPHA4_EXT         0x8046
#define GL_LUMINANCE12_ALPHA12_EXT        0x8047
#define GL_LUMINANCE16_ALPHA16_EXT        0x8048
#define GL_INTENSITY_EXT                  0x8049
#define GL_INTENSITY4_EXT                 0x804A
#define GL_INTENSITY8_EXT                 0x804B
#define GL_INTENSITY12_EXT                0x804C
#define GL_INTENSITY16_EXT                0x804D
#define GL_RGB2_EXT                       0x804E
#define GL_RGB4_EXT                       0x804F
#define GL_RGB5_EXT                       0x8050
#define GL_RGB8_EXT                       0x8051
#define GL_RGB10_EXT                      0x8052
#define GL_RGB12_EXT                      0x8053
#define GL_RGB16_EXT                      0x8054
#define GL_RGBA2_EXT                      0x8055
#define GL_RGBA4_EXT                      0x8056
#define GL_RGB5_A1_EXT                    0x8057
#define GL_RGBA8_EXT                      0x8058
#define GL_RGB10_A2_EXT                   0x8059
#define GL_RGBA12_EXT                     0x805A
#define GL_RGBA16_EXT                     0x805B
#define GL_TEXTURE_RED_SIZE_EXT           0x805C
#define GL_TEXTURE_GREEN_SIZE_EXT         0x805D
#define GL_TEXTURE_BLUE_SIZE_EXT          0x805E
#define GL_TEXTURE_ALPHA_SIZE_EXT         0x805F
#define GL_TEXTURE_LUMINANCE_SIZE_EXT     0x8060
#define GL_TEXTURE_INTENSITY_SIZE_EXT     0x8061
#define GL_REPLACE_EXT                    0x8062
#define GL_PROXY_TEXTURE_1D_EXT           0x8063
#define GL_PROXY_TEXTURE_2D_EXT           0x8064
#define GL_TEXTURE_TOO_LARGE_EXT          0x8065
#endif

#ifndef GL_EXT_texture3D
#define GL_PACK_SKIP_IMAGES_EXT           0x806B
#define GL_PACK_IMAGE_HEIGHT_EXT          0x806C
#define GL_UNPACK_SKIP_IMAGES_EXT         0x806D
#define GL_UNPACK_IMAGE_HEIGHT_EXT        0x806E
#define GL_TEXTURE_3D_EXT                 0x806F
#define GL_PROXY_TEXTURE_3D_EXT           0x8070
#define GL_TEXTURE_DEPTH_EXT              0x8071
#define GL_TEXTURE_WRAP_R_EXT             0x8072
#define GL_MAX_3D_TEXTURE_SIZE_EXT        0x8073
#endif

#ifndef GL_SGIS_texture_filter4
#define GL_FILTER4_SGIS                   0x8146
#define GL_TEXTURE_FILTER4_SIZE_SGIS      0x8147
#endif

#ifndef GL_EXT_subtexture
#endif

#ifndef GL_EXT_copy_texture
#endif

#ifndef GL_EXT_histogram
#define GL_HISTOGRAM_EXT                  0x8024
#define GL_PROXY_HISTOGRAM_EXT            0x8025
#define GL_HISTOGRAM_WIDTH_EXT            0x8026
#define GL_HISTOGRAM_FORMAT_EXT           0x8027
#define GL_HISTOGRAM_RED_SIZE_EXT         0x8028
#define GL_HISTOGRAM_GREEN_SIZE_EXT       0x8029
#define GL_HISTOGRAM_BLUE_SIZE_EXT        0x802A
#define GL_HISTOGRAM_ALPHA_SIZE_EXT       0x802B
#define GL_HISTOGRAM_LUMINANCE_SIZE_EXT   0x802C
#define GL_HISTOGRAM_SINK_EXT             0x802D
#define GL_MINMAX_EXT                     0x802E
#define GL_MINMAX_FORMAT_EXT              0x802F
#define GL_MINMAX_SINK_EXT                0x8030
#define GL_TABLE_TOO_LARGE_EXT            0x8031
#endif

#ifndef GL_EXT_convolution
#define GL_CONVOLUTION_1D_EXT             0x8010
#define GL_CONVOLUTION_2D_EXT             0x8011
#define GL_SEPARABLE_2D_EXT               0x8012
#define GL_CONVOLUTION_BORDER_MODE_EXT    0x8013
#define GL_CONVOLUTION_FILTER_SCALE_EXT   0x8014
#define GL_CONVOLUTION_FILTER_BIAS_EXT    0x8015
#define GL_REDUCE_EXT                     0x8016
#define GL_CONVOLUTION_FORMAT_EXT         0x8017
#define GL_CONVOLUTION_WIDTH_EXT          0x8018
#define GL_CONVOLUTION_HEIGHT_EXT         0x8019
#define GL_MAX_CONVOLUTION_WIDTH_EXT      0x801A
#define GL_MAX_CONVOLUTION_HEIGHT_EXT     0x801B
#define GL_POST_CONVOLUTION_RED_SCALE_EXT 0x801C
#define GL_POST_CONVOLUTION_GREEN_SCALE_EXT 0x801D
#define GL_POST_CONVOLUTION_BLUE_SCALE_EXT 0x801E
#define GL_POST_CONVOLUTION_ALPHA_SCALE_EXT 0x801F
#define GL_POST_CONVOLUTION_RED_BIAS_EXT  0x8020
#define GL_POST_CONVOLUTION_GREEN_BIAS_EXT 0x8021
#define GL_POST_CONVOLUTION_BLUE_BIAS_EXT 0x8022
#define GL_POST_CONVOLUTION_ALPHA_BIAS_EXT 0x8023
#endif

#ifndef GL_SGI_color_matrix
#define GL_COLOR_MATRIX_SGI               0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH_SGI   0x80B2
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI 0x80B3
#define GL_POST_COLOR_MATRIX_RED_SCALE_SGI 0x80B4
#define GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI 0x80B5
#define GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI 0x80B6
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI 0x80B7
#define GL_POST_COLOR_MATRIX_RED_BIAS_SGI 0x80B8
#define GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI 0x80B9
#define GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI 0x80BA
#define GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI 0x80BB
#endif

#ifndef GL_SGI_color_table
#define GL_COLOR_TABLE_SGI                0x80D0
#define GL_POST_CONVOLUTION_COLOR_TABLE_SGI 0x80D1
#define GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI 0x80D2
#define GL_PROXY_COLOR_TABLE_SGI          0x80D3
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE_SGI 0x80D4
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE_SGI 0x80D5
#define GL_COLOR_TABLE_SCALE_SGI          0x80D6
#define GL_COLOR_TABLE_BIAS_SGI           0x80D7
#define GL_COLOR_TABLE_FORMAT_SGI         0x80D8
#define GL_COLOR_TABLE_WIDTH_SGI          0x80D9
#define GL_COLOR_TABLE_RED_SIZE_SGI       0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE_SGI     0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE_SGI      0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE_SGI     0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE_SGI 0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE_SGI 0x80DF
#endif

#ifndef GL_SGIS_pixel_texture
#define GL_PIXEL_TEXTURE_SGIS             0x8353
#define GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS 0x8354
#define GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS 0x8355
#define GL_PIXEL_GROUP_COLOR_SGIS         0x8356
#endif

#ifndef GL_SGIX_pixel_texture
#define GL_PIXEL_TEX_GEN_SGIX             0x8139
#define GL_PIXEL_TEX_GEN_MODE_SGIX        0x832B
#endif

#ifndef GL_SGIS_texture4D
#define GL_PACK_SKIP_VOLUMES_SGIS         0x8130
#define GL_PACK_IMAGE_DEPTH_SGIS          0x8131
#define GL_UNPACK_SKIP_VOLUMES_SGIS       0x8132
#define GL_UNPACK_IMAGE_DEPTH_SGIS        0x8133
#define GL_TEXTURE_4D_SGIS                0x8134
#define GL_PROXY_TEXTURE_4D_SGIS          0x8135
#define GL_TEXTURE_4DSIZE_SGIS            0x8136
#define GL_TEXTURE_WRAP_Q_SGIS            0x8137
#define GL_MAX_4D_TEXTURE_SIZE_SGIS       0x8138
#define GL_TEXTURE_4D_BINDING_SGIS        0x814F
#endif

#ifndef GL_SGI_texture_color_table
#define GL_TEXTURE_COLOR_TABLE_SGI        0x80BC
#define GL_PROXY_TEXTURE_COLOR_TABLE_SGI  0x80BD
#endif

#ifndef GL_EXT_cmyka
#define GL_CMYK_EXT                       0x800C
#define GL_CMYKA_EXT                      0x800D
#define GL_PACK_CMYK_HINT_EXT             0x800E
#define GL_UNPACK_CMYK_HINT_EXT           0x800F
#endif

#ifndef GL_EXT_texture_object
#define GL_TEXTURE_PRIORITY_EXT           0x8066
#define GL_TEXTURE_RESIDENT_EXT           0x8067
#define GL_TEXTURE_1D_BINDING_EXT         0x8068
#define GL_TEXTURE_2D_BINDING_EXT         0x8069
#define GL_TEXTURE_3D_BINDING_EXT         0x806A
#endif

#ifndef GL_SGIS_detail_texture
#define GL_DETAIL_TEXTURE_2D_SGIS         0x8095
#define GL_DETAIL_TEXTURE_2D_BINDING_SGIS 0x8096
#define GL_LINEAR_DETAIL_SGIS             0x8097
#define GL_LINEAR_DETAIL_ALPHA_SGIS       0x8098
#define GL_LINEAR_DETAIL_COLOR_SGIS       0x8099
#define GL_DETAIL_TEXTURE_LEVEL_SGIS      0x809A
#define GL_DETAIL_TEXTURE_MODE_SGIS       0x809B
#define GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS 0x809C
#endif

#ifndef GL_SGIS_sharpen_texture
#define GL_LINEAR_SHARPEN_SGIS            0x80AD
#define GL_LINEAR_SHARPEN_ALPHA_SGIS      0x80AE
#define GL_LINEAR_SHARPEN_COLOR_SGIS      0x80AF
#define GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS 0x80B0
#endif

#ifndef GL_EXT_packed_pixels
#define GL_UNSIGNED_BYTE_3_3_2_EXT        0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT     0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT     0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT       0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT    0x8036
#endif

#ifndef GL_SGIS_texture_lod
#define GL_TEXTURE_MIN_LOD_SGIS           0x813A
#define GL_TEXTURE_MAX_LOD_SGIS           0x813B
#define GL_TEXTURE_BASE_LEVEL_SGIS        0x813C
#define GL_TEXTURE_MAX_LEVEL_SGIS         0x813D
#endif

#ifndef GL_SGIS_multisample
#define GL_MULTISAMPLE_SGIS               0x809D
#define GL_SAMPLE_ALPHA_TO_MASK_SGIS      0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_SGIS       0x809F
#define GL_SAMPLE_MASK_SGIS               0x80A0
#define GL_1PASS_SGIS                     0x80A1
#define GL_2PASS_0_SGIS                   0x80A2
#define GL_2PASS_1_SGIS                   0x80A3
#define GL_4PASS_0_SGIS                   0x80A4
#define GL_4PASS_1_SGIS                   0x80A5
#define GL_4PASS_2_SGIS                   0x80A6
#define GL_4PASS_3_SGIS                   0x80A7
#define GL_SAMPLE_BUFFERS_SGIS            0x80A8
#define GL_SAMPLES_SGIS                   0x80A9
#define GL_SAMPLE_MASK_VALUE_SGIS         0x80AA
#define GL_SAMPLE_MASK_INVERT_SGIS        0x80AB
#define GL_SAMPLE_PATTERN_SGIS            0x80AC
#endif

#ifndef GL_EXT_rescale_normal
#define GL_RESCALE_NORMAL_EXT             0x803A
#endif

#ifndef GL_EXT_vertex_array
#define GL_VERTEX_ARRAY_EXT               0x8074
#define GL_NORMAL_ARRAY_EXT               0x8075
#define GL_COLOR_ARRAY_EXT                0x8076
#define GL_INDEX_ARRAY_EXT                0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT        0x8078
#define GL_EDGE_FLAG_ARRAY_EXT            0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT          0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT          0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT        0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT         0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT          0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT        0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT         0x8080
#define GL_COLOR_ARRAY_SIZE_EXT           0x8081
#define GL_COLOR_ARRAY_TYPE_EXT           0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT         0x8083
#define GL_COLOR_ARRAY_COUNT_EXT          0x8084
#define GL_INDEX_ARRAY_TYPE_EXT           0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT         0x8086
#define GL_INDEX_ARRAY_COUNT_EXT          0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT   0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT   0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT 0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT  0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT     0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT      0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT       0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT       0x808F
#define GL_COLOR_ARRAY_POINTER_EXT        0x8090
#define GL_INDEX_ARRAY_POINTER_EXT        0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT 0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT    0x8093
#endif

#ifndef GL_EXT_misc_attribute
#endif

#ifndef GL_SGIS_generate_mipmap
#define GL_GENERATE_MIPMAP_SGIS           0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS      0x8192
#endif

#ifndef GL_SGIX_clipmap
#define GL_LINEAR_CLIPMAP_LINEAR_SGIX     0x8170
#define GL_TEXTURE_CLIPMAP_CENTER_SGIX    0x8171
#define GL_TEXTURE_CLIPMAP_FRAME_SGIX     0x8172
#define GL_TEXTURE_CLIPMAP_OFFSET_SGIX    0x8173
#define GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX 0x8174
#define GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX 0x8175
#define GL_TEXTURE_CLIPMAP_DEPTH_SGIX     0x8176
#define GL_MAX_CLIPMAP_DEPTH_SGIX         0x8177
#define GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX 0x8178
#define GL_NEAREST_CLIPMAP_NEAREST_SGIX   0x844D
#define GL_NEAREST_CLIPMAP_LINEAR_SGIX    0x844E
#define GL_LINEAR_CLIPMAP_NEAREST_SGIX    0x844F
#endif

#ifndef GL_SGIX_shadow
#define GL_TEXTURE_COMPARE_SGIX           0x819A
#define GL_TEXTURE_COMPARE_OPERATOR_SGIX  0x819B
#define GL_TEXTURE_LEQUAL_R_SGIX          0x819C
#define GL_TEXTURE_GEQUAL_R_SGIX          0x819D
#endif

#ifndef GL_SGIS_texture_edge_clamp
#define GL_CLAMP_TO_EDGE_SGIS             0x812F
#endif

#ifndef GL_EXT_blend_minmax
#define GL_FUNC_ADD_EXT                   0x8006
#define GL_MIN_EXT                        0x8007
#define GL_MAX_EXT                        0x8008
#define GL_BLEND_EQUATION_EXT             0x8009
#endif

#ifndef GL_EXT_blend_subtract
#define GL_FUNC_SUBTRACT_EXT              0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT      0x800B
#endif

#ifndef GL_EXT_blend_logic_op
#endif

#ifndef GL_SGIX_interlace
#define GL_INTERLACE_SGIX                 0x8094
#endif

#ifndef GL_SGIX_pixel_tiles
#define GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX 0x813E
#define GL_PIXEL_TILE_CACHE_INCREMENT_SGIX 0x813F
#define GL_PIXEL_TILE_WIDTH_SGIX          0x8140
#define GL_PIXEL_TILE_HEIGHT_SGIX         0x8141
#define GL_PIXEL_TILE_GRID_WIDTH_SGIX     0x8142
#define GL_PIXEL_TILE_GRID_HEIGHT_SGIX    0x8143
#define GL_PIXEL_TILE_GRID_DEPTH_SGIX     0x8144
#define GL_PIXEL_TILE_CACHE_SIZE_SGIX     0x8145
#endif

#ifndef GL_SGIS_texture_select
#define GL_DUAL_ALPHA4_SGIS               0x8110
#define GL_DUAL_ALPHA8_SGIS               0x8111
#define GL_DUAL_ALPHA12_SGIS              0x8112
#define GL_DUAL_ALPHA16_SGIS              0x8113
#define GL_DUAL_LUMINANCE4_SGIS           0x8114
#define GL_DUAL_LUMINANCE8_SGIS           0x8115
#define GL_DUAL_LUMINANCE12_SGIS          0x8116
#define GL_DUAL_LUMINANCE16_SGIS          0x8117
#define GL_DUAL_INTENSITY4_SGIS           0x8118
#define GL_DUAL_INTENSITY8_SGIS           0x8119
#define GL_DUAL_INTENSITY12_SGIS          0x811A
#define GL_DUAL_INTENSITY16_SGIS          0x811B
#define GL_DUAL_LUMINANCE_ALPHA4_SGIS     0x811C
#define GL_DUAL_LUMINANCE_ALPHA8_SGIS     0x811D
#define GL_QUAD_ALPHA4_SGIS               0x811E
#define GL_QUAD_ALPHA8_SGIS               0x811F
#define GL_QUAD_LUMINANCE4_SGIS           0x8120
#define GL_QUAD_LUMINANCE8_SGIS           0x8121
#define GL_QUAD_INTENSITY4_SGIS           0x8122
#define GL_QUAD_INTENSITY8_SGIS           0x8123
#define GL_DUAL_TEXTURE_SELECT_SGIS       0x8124
#define GL_QUAD_TEXTURE_SELECT_SGIS       0x8125
#endif

#ifndef GL_SGIX_sprite
#define GL_SPRITE_SGIX                    0x8148
#define GL_SPRITE_MODE_SGIX               0x8149
#define GL_SPRITE_AXIS_SGIX               0x814A
#define GL_SPRITE_TRANSLATION_SGIX        0x814B
#define GL_SPRITE_AXIAL_SGIX              0x814C
#define GL_SPRITE_OBJECT_ALIGNED_SGIX     0x814D
#define GL_SPRITE_EYE_ALIGNED_SGIX        0x814E
#endif

#ifndef GL_SGIX_texture_multi_buffer
#define GL_TEXTURE_MULTI_BUFFER_HINT_SGIX 0x812E
#endif

#ifndef GL_EXT_point_parameters
#define GL_POINT_SIZE_MIN_EXT             0x8126
#define GL_POINT_SIZE_MAX_EXT             0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT  0x8128
#define GL_DISTANCE_ATTENUATION_EXT       0x8129
#endif

#ifndef GL_SGIS_point_parameters
#define GL_POINT_SIZE_MIN_SGIS            0x8126
#define GL_POINT_SIZE_MAX_SGIS            0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_SGIS 0x8128
#define GL_DISTANCE_ATTENUATION_SGIS      0x8129
#endif

#ifndef GL_SGIX_instruments
#define GL_INSTRUMENT_BUFFER_POINTER_SGIX 0x8180
#define GL_INSTRUMENT_MEASUREMENTS_SGIX   0x8181
#endif

#ifndef GL_SGIX_texture_scale_bias
#define GL_POST_TEXTURE_FILTER_BIAS_SGIX  0x8179
#define GL_POST_TEXTURE_FILTER_SCALE_SGIX 0x817A
#define GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX 0x817B
#define GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX 0x817C
#endif

#ifndef GL_SGIX_framezoom
#define GL_FRAMEZOOM_SGIX                 0x818B
#define GL_FRAMEZOOM_FACTOR_SGIX          0x818C
#define GL_MAX_FRAMEZOOM_FACTOR_SGIX      0x818D
#endif

#ifndef GL_SGIX_tag_sample_buffer
#endif

#ifndef GL_FfdMaskSGIX
#define GL_TEXTURE_DEFORMATION_BIT_SGIX   0x00000001
#define GL_GEOMETRY_DEFORMATION_BIT_SGIX  0x00000002
#endif

#ifndef GL_SGIX_polynomial_ffd
#define GL_GEOMETRY_DEFORMATION_SGIX      0x8194
#define GL_TEXTURE_DEFORMATION_SGIX       0x8195
#define GL_DEFORMATIONS_MASK_SGIX         0x8196
#define GL_MAX_DEFORMATION_ORDER_SGIX     0x8197
#endif

#ifndef GL_SGIX_reference_plane
#define GL_REFERENCE_PLANE_SGIX           0x817D
#define GL_REFERENCE_PLANE_EQUATION_SGIX  0x817E
#endif

#ifndef GL_SGIX_flush_raster
#endif

#ifndef GL_SGIX_depth_texture
#define GL_DEPTH_COMPONENT16_SGIX         0x81A5
#define GL_DEPTH_COMPONENT24_SGIX         0x81A6
#define GL_DEPTH_COMPONENT32_SGIX         0x81A7
#endif

#ifndef GL_SGIS_fog_function
#define GL_FOG_FUNC_SGIS                  0x812A
#define GL_FOG_FUNC_POINTS_SGIS           0x812B
#define GL_MAX_FOG_FUNC_POINTS_SGIS       0x812C
#endif

#ifndef GL_SGIX_fog_offset
#define GL_FOG_OFFSET_SGIX                0x8198
#define GL_FOG_OFFSET_VALUE_SGIX          0x8199
#endif

#ifndef GL_HP_image_transform
#define GL_IMAGE_SCALE_X_HP               0x8155
#define GL_IMAGE_SCALE_Y_HP               0x8156
#define GL_IMAGE_TRANSLATE_X_HP           0x8157
#define GL_IMAGE_TRANSLATE_Y_HP           0x8158
#define GL_IMAGE_ROTATE_ANGLE_HP          0x8159
#define GL_IMAGE_ROTATE_ORIGIN_X_HP       0x815A
#define GL_IMAGE_ROTATE_ORIGIN_Y_HP       0x815B
#define GL_IMAGE_MAG_FILTER_HP            0x815C
#define GL_IMAGE_MIN_FILTER_HP            0x815D
#define GL_IMAGE_CUBIC_WEIGHT_HP          0x815E
#define GL_CUBIC_HP                       0x815F
#define GL_AVERAGE_HP                     0x8160
#define GL_IMAGE_TRANSFORM_2D_HP          0x8161
#define GL_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP 0x8162
#define GL_PROXY_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP 0x8163
#endif

#ifndef GL_HP_convolution_border_modes
#define GL_IGNORE_BORDER_HP               0x8150
#define GL_CONSTANT_BORDER_HP             0x8151
#define GL_REPLICATE_BORDER_HP            0x8153
#define GL_CONVOLUTION_BORDER_COLOR_HP    0x8154
#endif

#ifndef GL_INGR_palette_buffer
#endif

#ifndef GL_SGIX_texture_add_env
#define GL_TEXTURE_ENV_BIAS_SGIX          0x80BE
#endif

#ifndef GL_EXT_color_subtable
#endif

#ifndef GL_PGI_vertex_hints
#define GL_VERTEX_DATA_HINT_PGI           0x1A22A
#define GL_VERTEX_CONSISTENT_HINT_PGI     0x1A22B
#define GL_MATERIAL_SIDE_HINT_PGI         0x1A22C
#define GL_MAX_VERTEX_HINT_PGI            0x1A22D
#define GL_COLOR3_BIT_PGI                 0x00010000
#define GL_COLOR4_BIT_PGI                 0x00020000
#define GL_EDGEFLAG_BIT_PGI               0x00040000
#define GL_INDEX_BIT_PGI                  0x00080000
#define GL_MAT_AMBIENT_BIT_PGI            0x00100000
#define GL_MAT_AMBIENT_AND_DIFFUSE_BIT_PGI 0x00200000
#define GL_MAT_DIFFUSE_BIT_PGI            0x00400000
#define GL_MAT_EMISSION_BIT_PGI           0x00800000
#define GL_MAT_COLOR_INDEXES_BIT_PGI      0x01000000
#define GL_MAT_SHININESS_BIT_PGI          0x02000000
#define GL_MAT_SPECULAR_BIT_PGI           0x04000000
#define GL_NORMAL_BIT_PGI                 0x08000000
#define GL_TEXCOORD1_BIT_PGI              0x10000000
#define GL_TEXCOORD2_BIT_PGI              0x20000000
#define GL_TEXCOORD3_BIT_PGI              0x40000000
#define GL_TEXCOORD4_BIT_PGI              0x80000000
#define GL_VERTEX23_BIT_PGI               0x00000004
#define GL_VERTEX4_BIT_PGI                0x00000008
#endif

#ifndef GL_PGI_misc_hints
#define GL_PREFER_DOUBLEBUFFER_HINT_PGI   0x1A1F8
#define GL_CONSERVE_MEMORY_HINT_PGI       0x1A1FD
#define GL_RECLAIM_MEMORY_HINT_PGI        0x1A1FE
#define GL_NATIVE_GRAPHICS_HANDLE_PGI     0x1A202
#define GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI 0x1A203
#define GL_NATIVE_GRAPHICS_END_HINT_PGI   0x1A204
#define GL_ALWAYS_FAST_HINT_PGI           0x1A20C
#define GL_ALWAYS_SOFT_HINT_PGI           0x1A20D
#define GL_ALLOW_DRAW_OBJ_HINT_PGI        0x1A20E
#define GL_ALLOW_DRAW_WIN_HINT_PGI        0x1A20F
#define GL_ALLOW_DRAW_FRG_HINT_PGI        0x1A210
#define GL_ALLOW_DRAW_MEM_HINT_PGI        0x1A211
#define GL_STRICT_DEPTHFUNC_HINT_PGI      0x1A216
#define GL_STRICT_LIGHTING_HINT_PGI       0x1A217
#define GL_STRICT_SCISSOR_HINT_PGI        0x1A218
#define GL_FULL_STIPPLE_HINT_PGI          0x1A219
#define GL_CLIP_NEAR_HINT_PGI             0x1A220
#define GL_CLIP_FAR_HINT_PGI              0x1A221
#define GL_WIDE_LINE_HINT_PGI             0x1A222
#define GL_BACK_NORMALS_HINT_PGI          0x1A223
#endif

#ifndef GL_EXT_paletted_texture
#define GL_COLOR_INDEX1_EXT               0x80E2
#define GL_COLOR_INDEX2_EXT               0x80E3
#define GL_COLOR_INDEX4_EXT               0x80E4
#define GL_COLOR_INDEX8_EXT               0x80E5
#define GL_COLOR_INDEX12_EXT              0x80E6
#define GL_COLOR_INDEX16_EXT              0x80E7
#define GL_TEXTURE_INDEX_SIZE_EXT         0x80ED
#endif

#ifndef GL_EXT_clip_volume_hint
#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT  0x80F0
#endif

#ifndef GL_SGIX_list_priority
#define GL_LIST_PRIORITY_SGIX             0x8182
#endif

#ifndef GL_SGIX_ir_instrument1
#define GL_IR_INSTRUMENT1_SGIX            0x817F
#endif

#ifndef GL_SGIX_calligraphic_fragment
#define GL_CALLIGRAPHIC_FRAGMENT_SGIX     0x8183
#endif

#ifndef GL_SGIX_texture_lod_bias
#define GL_TEXTURE_LOD_BIAS_S_SGIX        0x818E
#define GL_TEXTURE_LOD_BIAS_T_SGIX        0x818F
#define GL_TEXTURE_LOD_BIAS_R_SGIX        0x8190
#endif

#ifndef GL_SGIX_shadow_ambient
#define GL_SHADOW_AMBIENT_SGIX            0x80BF
#endif

#ifndef GL_EXT_index_texture
#endif

#ifndef GL_EXT_index_material
#define GL_INDEX_MATERIAL_EXT             0x81B8
#define GL_INDEX_MATERIAL_PARAMETER_EXT   0x81B9
#define GL_INDEX_MATERIAL_FACE_EXT        0x81BA
#endif

#ifndef GL_EXT_index_func
#define GL_INDEX_TEST_EXT                 0x81B5
#define GL_INDEX_TEST_FUNC_EXT            0x81B6
#define GL_INDEX_TEST_REF_EXT             0x81B7
#endif

#ifndef GL_EXT_index_array_formats
#define GL_IUI_V2F_EXT                    0x81AD
#define GL_IUI_V3F_EXT                    0x81AE
#define GL_IUI_N3F_V2F_EXT                0x81AF
#define GL_IUI_N3F_V3F_EXT                0x81B0
#define GL_T2F_IUI_V2F_EXT                0x81B1
#define GL_T2F_IUI_V3F_EXT                0x81B2
#define GL_T2F_IUI_N3F_V2F_EXT            0x81B3
#define GL_T2F_IUI_N3F_V3F_EXT            0x81B4
#endif

#ifndef GL_EXT_compiled_vertex_array
#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT   0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT   0x81A9
#endif

#ifndef GL_EXT_cull_vertex
#define GL_CULL_VERTEX_EXT                0x81AA
#define GL_CULL_VERTEX_EYE_POSITION_EXT   0x81AB
#define GL_CULL_VERTEX_OBJECT_POSITION_EXT 0x81AC
#endif

#ifndef GL_SGIX_ycrcb
#define GL_YCRCB_422_SGIX                 0x81BB
#define GL_YCRCB_444_SGIX                 0x81BC
#endif

#ifndef GL_SGIX_fragment_lighting
#define GL_FRAGMENT_LIGHTING_SGIX         0x8400
#define GL_FRAGMENT_COLOR_MATERIAL_SGIX   0x8401
#define GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX 0x8402
#define GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX 0x8403
#define GL_MAX_FRAGMENT_LIGHTS_SGIX       0x8404
#define GL_MAX_ACTIVE_LIGHTS_SGIX         0x8405
#define GL_CURRENT_RASTER_NORMAL_SGIX     0x8406
#define GL_LIGHT_ENV_MODE_SGIX            0x8407
#define GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX 0x8408
#define GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX 0x8409
#define GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX 0x840A
#define GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX 0x840B
#define GL_FRAGMENT_LIGHT0_SGIX           0x840C
#define GL_FRAGMENT_LIGHT1_SGIX           0x840D
#define GL_FRAGMENT_LIGHT2_SGIX           0x840E
#define GL_FRAGMENT_LIGHT3_SGIX           0x840F
#define GL_FRAGMENT_LIGHT4_SGIX           0x8410
#define GL_FRAGMENT_LIGHT5_SGIX           0x8411
#define GL_FRAGMENT_LIGHT6_SGIX           0x8412
#define GL_FRAGMENT_LIGHT7_SGIX           0x8413
#endif

#ifndef GL_IBM_rasterpos_clip
#define GL_RASTER_POSITION_UNCLIPPED_IBM  0x19262
#endif

#ifndef GL_HP_texture_lighting
#define GL_TEXTURE_LIGHTING_MODE_HP       0x8167
#define GL_TEXTURE_POST_SPECULAR_HP       0x8168
#define GL_TEXTURE_PRE_SPECULAR_HP        0x8169
#endif

#ifndef GL_EXT_draw_range_elements
#define GL_MAX_ELEMENTS_VERTICES_EXT      0x80E8
#define GL_MAX_ELEMENTS_INDICES_EXT       0x80E9
#endif

#ifndef GL_WIN_phong_shading
#define GL_PHONG_WIN                      0x80EA
#define GL_PHONG_HINT_WIN                 0x80EB
#endif

#ifndef GL_WIN_specular_fog
#define GL_FOG_SPECULAR_TEXTURE_WIN       0x80EC
#endif

#ifndef GL_EXT_light_texture
#define GL_FRAGMENT_MATERIAL_EXT          0x8349
#define GL_FRAGMENT_NORMAL_EXT            0x834A
#define GL_FRAGMENT_COLOR_EXT             0x834C
#define GL_ATTENUATION_EXT                0x834D
#define GL_SHADOW_ATTENUATION_EXT         0x834E
#define GL_TEXTURE_APPLICATION_MODE_EXT   0x834F
#define GL_TEXTURE_LIGHT_EXT              0x8350
#define GL_TEXTURE_MATERIAL_FACE_EXT      0x8351
#define GL_TEXTURE_MATERIAL_PARAMETER_EXT 0x8352
/* reuse GL_FRAGMENT_DEPTH_EXT */
#endif

#ifndef GL_SGIX_blend_alpha_minmax
#define GL_ALPHA_MIN_SGIX                 0x8320
#define GL_ALPHA_MAX_SGIX                 0x8321
#endif

#ifndef GL_SGIX_impact_pixel_texture
#define GL_PIXEL_TEX_GEN_Q_CEILING_SGIX   0x8184
#define GL_PIXEL_TEX_GEN_Q_ROUND_SGIX     0x8185
#define GL_PIXEL_TEX_GEN_Q_FLOOR_SGIX     0x8186
#define GL_PIXEL_TEX_GEN_ALPHA_REPLACE_SGIX 0x8187
#define GL_PIXEL_TEX_GEN_ALPHA_NO_REPLACE_SGIX 0x8188
#define GL_PIXEL_TEX_GEN_ALPHA_LS_SGIX    0x8189
#define GL_PIXEL_TEX_GEN_ALPHA_MS_SGIX    0x818A
#endif

#ifndef GL_EXT_bgra
#define GL_BGR_EXT                        0x80E0
#define GL_BGRA_EXT                       0x80E1
#endif

#ifndef GL_SGIX_async
#define GL_ASYNC_MARKER_SGIX              0x8329
#endif

#ifndef GL_SGIX_async_pixel
#define GL_ASYNC_TEX_IMAGE_SGIX           0x835C
#define GL_ASYNC_DRAW_PIXELS_SGIX         0x835D
#define GL_ASYNC_READ_PIXELS_SGIX         0x835E
#define GL_MAX_ASYNC_TEX_IMAGE_SGIX       0x835F
#define GL_MAX_ASYNC_DRAW_PIXELS_SGIX     0x8360
#define GL_MAX_ASYNC_READ_PIXELS_SGIX     0x8361
#endif

#ifndef GL_SGIX_async_histogram
#define GL_ASYNC_HISTOGRAM_SGIX           0x832C
#define GL_MAX_ASYNC_HISTOGRAM_SGIX       0x832D
#endif

#ifndef GL_INTEL_texture_scissor
#endif

#ifndef GL_INTEL_parallel_arrays
#define GL_PARALLEL_ARRAYS_INTEL          0x83F4
#define GL_VERTEX_ARRAY_PARALLEL_POINTERS_INTEL 0x83F5
#define GL_NORMAL_ARRAY_PARALLEL_POINTERS_INTEL 0x83F6
#define GL_COLOR_ARRAY_PARALLEL_POINTERS_INTEL 0x83F7
#define GL_TEXTURE_COORD_ARRAY_PARALLEL_POINTERS_INTEL 0x83F8
#endif

#ifndef GL_HP_occlusion_test
#define GL_OCCLUSION_TEST_HP              0x8165
#define GL_OCCLUSION_TEST_RESULT_HP       0x8166
#endif

#ifndef GL_EXT_pixel_transform
#define GL_PIXEL_TRANSFORM_2D_EXT         0x8330
#define GL_PIXEL_MAG_FILTER_EXT           0x8331
#define GL_PIXEL_MIN_FILTER_EXT           0x8332
#define GL_PIXEL_CUBIC_WEIGHT_EXT         0x8333
#define GL_CUBIC_EXT                      0x8334
#define GL_AVERAGE_EXT                    0x8335
#define GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT 0x8336
#define GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT 0x8337
#define GL_PIXEL_TRANSFORM_2D_MATRIX_EXT  0x8338
#endif

#ifndef GL_EXT_pixel_transform_color_table
#endif

#ifndef GL_EXT_shared_texture_palette
#define GL_SHARED_TEXTURE_PALETTE_EXT     0x81FB
#endif

#ifndef GL_EXT_separate_specular_color
#define GL_LIGHT_MODEL_COLOR_CONTROL_EXT  0x81F8
#define GL_SINGLE_COLOR_EXT               0x81F9
#define GL_SEPARATE_SPECULAR_COLOR_EXT    0x81FA
#endif

#ifndef GL_EXT_secondary_color
#define GL_COLOR_SUM_EXT                  0x8458
#define GL_CURRENT_SECONDARY_COLOR_EXT    0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT 0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT 0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT 0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT 0x845D
#define GL_SECONDARY_COLOR_ARRAY_EXT      0x845E
#endif

#ifndef GL_EXT_texture_perturb_normal
#define GL_PERTURB_EXT                    0x85AE
#define GL_TEXTURE_NORMAL_EXT             0x85AF
#endif

#ifndef GL_EXT_multi_draw_arrays
#endif

#ifndef GL_EXT_fog_coord
#define GL_FOG_COORDINATE_SOURCE_EXT      0x8450
#define GL_FOG_COORDINATE_EXT             0x8451
#define GL_FRAGMENT_DEPTH_EXT             0x8452
#define GL_CURRENT_FOG_COORDINATE_EXT     0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT  0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT 0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT 0x8456
#define GL_FOG_COORDINATE_ARRAY_EXT       0x8457
#endif

#ifndef GL_REND_screen_coordinates
#define GL_SCREEN_COORDINATES_REND        0x8490
#define GL_INVERTED_SCREEN_W_REND         0x8491
#endif

#ifndef GL_EXT_coordinate_frame
#define GL_TANGENT_ARRAY_EXT              0x8439
#define GL_BINORMAL_ARRAY_EXT             0x843A
#define GL_CURRENT_TANGENT_EXT            0x843B
#define GL_CURRENT_BINORMAL_EXT           0x843C
#define GL_TANGENT_ARRAY_TYPE_EXT         0x843E
#define GL_TANGENT_ARRAY_STRIDE_EXT       0x843F
#define GL_BINORMAL_ARRAY_TYPE_EXT        0x8440
#define GL_BINORMAL_ARRAY_STRIDE_EXT      0x8441
#define GL_TANGENT_ARRAY_POINTER_EXT      0x8442
#define GL_BINORMAL_ARRAY_POINTER_EXT     0x8443
#define GL_MAP1_TANGENT_EXT               0x8444
#define GL_MAP2_TANGENT_EXT               0x8445
#define GL_MAP1_BINORMAL_EXT              0x8446
#define GL_MAP2_BINORMAL_EXT              0x8447
#endif

#ifndef GL_EXT_texture_env_combine
#define GL_COMBINE_EXT                    0x8570
#define GL_COMBINE_RGB_EXT                0x8571
#define GL_COMBINE_ALPHA_EXT              0x8572
#define GL_RGB_SCALE_EXT                  0x8573
#define GL_ADD_SIGNED_EXT                 0x8574
#define GL_INTERPOLATE_EXT                0x8575
#define GL_CONSTANT_EXT                   0x8576
#define GL_PRIMARY_COLOR_EXT              0x8577
#define GL_PREVIOUS_EXT                   0x8578
#define GL_SOURCE0_RGB_EXT                0x8580
#define GL_SOURCE1_RGB_EXT                0x8581
#define GL_SOURCE2_RGB_EXT                0x8582
#define GL_SOURCE0_ALPHA_EXT              0x8588
#define GL_SOURCE1_ALPHA_EXT              0x8589
#define GL_SOURCE2_ALPHA_EXT              0x858A
#define GL_OPERAND0_RGB_EXT               0x8590
#define GL_OPERAND1_RGB_EXT               0x8591
#define GL_OPERAND2_RGB_EXT               0x8592
#define GL_OPERAND0_ALPHA_EXT             0x8598
#define GL_OPERAND1_ALPHA_EXT             0x8599
#define GL_OPERAND2_ALPHA_EXT             0x859A
#endif

#ifndef GL_APPLE_specular_vector
#define GL_LIGHT_MODEL_SPECULAR_VECTOR_APPLE 0x85B0
#endif

#ifndef GL_APPLE_transform_hint
#define GL_TRANSFORM_HINT_APPLE           0x85B1
#endif

#ifndef GL_SGIX_fog_scale
#define GL_FOG_SCALE_SGIX                 0x81FC
#define GL_FOG_SCALE_VALUE_SGIX           0x81FD
#endif

#ifndef GL_SUNX_constant_data
#define GL_UNPACK_CONSTANT_DATA_SUNX      0x81D5
#define GL_TEXTURE_CONSTANT_DATA_SUNX     0x81D6
#endif

#ifndef GL_SUN_global_alpha
#define GL_GLOBAL_ALPHA_SUN               0x81D9
#define GL_GLOBAL_ALPHA_FACTOR_SUN        0x81DA
#endif

#ifndef GL_SUN_triangle_list
#define GL_RESTART_SUN                    0x0001
#define GL_REPLACE_MIDDLE_SUN             0x0002
#define GL_REPLACE_OLDEST_SUN             0x0003
#define GL_TRIANGLE_LIST_SUN              0x81D7
#define GL_REPLACEMENT_CODE_SUN           0x81D8
#define GL_REPLACEMENT_CODE_ARRAY_SUN     0x85C0
#define GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN 0x85C1
#define GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN 0x85C2
#define GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN 0x85C3
#define GL_R1UI_V3F_SUN                   0x85C4
#define GL_R1UI_C4UB_V3F_SUN              0x85C5
#define GL_R1UI_C3F_V3F_SUN               0x85C6
#define GL_R1UI_N3F_V3F_SUN               0x85C7
#define GL_R1UI_C4F_N3F_V3F_SUN           0x85C8
#define GL_R1UI_T2F_V3F_SUN               0x85C9
#define GL_R1UI_T2F_N3F_V3F_SUN           0x85CA
#define GL_R1UI_T2F_C4F_N3F_V3F_SUN       0x85CB
#endif

#ifndef GL_SUN_vertex
#endif

#ifndef GL_EXT_blend_func_separate
#define GL_BLEND_DST_RGB_EXT              0x80C8
#define GL_BLEND_SRC_RGB_EXT              0x80C9
#define GL_BLEND_DST_ALPHA_EXT            0x80CA
#define GL_BLEND_SRC_ALPHA_EXT            0x80CB
#endif

#ifndef GL_INGR_color_clamp
#define GL_RED_MIN_CLAMP_INGR             0x8560
#define GL_GREEN_MIN_CLAMP_INGR           0x8561
#define GL_BLUE_MIN_CLAMP_INGR            0x8562
#define GL_ALPHA_MIN_CLAMP_INGR           0x8563
#define GL_RED_MAX_CLAMP_INGR             0x8564
#define GL_GREEN_MAX_CLAMP_INGR           0x8565
#define GL_BLUE_MAX_CLAMP_INGR            0x8566
#define GL_ALPHA_MAX_CLAMP_INGR           0x8567
#endif

#ifndef GL_INGR_interlace_read
#define GL_INTERLACE_READ_INGR            0x8568
#endif

#ifndef GL_EXT_stencil_wrap
#define GL_INCR_WRAP_EXT                  0x8507
#define GL_DECR_WRAP_EXT                  0x8508
#endif

#ifndef GL_EXT_422_pixels
#define GL_422_EXT                        0x80CC
#define GL_422_REV_EXT                    0x80CD
#define GL_422_AVERAGE_EXT                0x80CE
#define GL_422_REV_AVERAGE_EXT            0x80CF
#endif

#ifndef GL_NV_texgen_reflection
#define GL_NORMAL_MAP_NV                  0x8511
#define GL_REFLECTION_MAP_NV              0x8512
#endif

#ifndef GL_EXT_texture_cube_map
#define GL_NORMAL_MAP_EXT                 0x8511
#define GL_REFLECTION_MAP_EXT             0x8512
#define GL_TEXTURE_CUBE_MAP_EXT           0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT   0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT     0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT  0x851C
#endif

#ifndef GL_SUN_convolution_border_modes
#define GL_WRAP_BORDER_SUN                0x81D4
#endif

#ifndef GL_EXT_texture_env_add
#endif

#ifndef GL_EXT_texture_lod_bias
#define GL_MAX_TEXTURE_LOD_BIAS_EXT       0x84FD
#define GL_TEXTURE_FILTER_CONTROL_EXT     0x8500
#define GL_TEXTURE_LOD_BIAS_EXT           0x8501
#endif

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#ifndef GL_EXT_vertex_weighting
#define GL_MODELVIEW0_STACK_DEPTH_EXT     GL_MODELVIEW_STACK_DEPTH
#define GL_MODELVIEW1_STACK_DEPTH_EXT     0x8502
#define GL_MODELVIEW0_MATRIX_EXT          GL_MODELVIEW_MATRIX
#define GL_MODELVIEW1_MATRIX_EXT          0x8506
#define GL_VERTEX_WEIGHTING_EXT           0x8509
#define GL_MODELVIEW0_EXT                 GL_MODELVIEW
#define GL_MODELVIEW1_EXT                 0x850A
#define GL_CURRENT_VERTEX_WEIGHT_EXT      0x850B
#define GL_VERTEX_WEIGHT_ARRAY_EXT        0x850C
#define GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT   0x850D
#define GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT   0x850E
#define GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT 0x850F
#define GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT 0x8510
#endif

#ifndef GL_NV_light_max_exponent
#define GL_MAX_SHININESS_NV               0x8504
#define GL_MAX_SPOT_EXPONENT_NV           0x8505
#endif

#ifndef GL_NV_vertex_array_range
#define GL_VERTEX_ARRAY_RANGE_NV          0x851D
#define GL_VERTEX_ARRAY_RANGE_LENGTH_NV   0x851E
#define GL_VERTEX_ARRAY_RANGE_VALID_NV    0x851F
#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV 0x8520
#define GL_VERTEX_ARRAY_RANGE_POINTER_NV  0x8521
#endif

#ifndef GL_NV_register_combiners
#define GL_REGISTER_COMBINERS_NV          0x8522
#define GL_VARIABLE_A_NV                  0x8523
#define GL_VARIABLE_B_NV                  0x8524
#define GL_VARIABLE_C_NV                  0x8525
#define GL_VARIABLE_D_NV                  0x8526
#define GL_VARIABLE_E_NV                  0x8527
#define GL_VARIABLE_F_NV                  0x8528
#define GL_VARIABLE_G_NV                  0x8529
#define GL_CONSTANT_COLOR0_NV             0x852A
#define GL_CONSTANT_COLOR1_NV             0x852B
#define GL_PRIMARY_COLOR_NV               0x852C
#define GL_SECONDARY_COLOR_NV             0x852D
#define GL_SPARE0_NV                      0x852E
#define GL_SPARE1_NV                      0x852F
#define GL_DISCARD_NV                     0x8530
#define GL_E_TIMES_F_NV                   0x8531
#define GL_SPARE0_PLUS_SECONDARY_COLOR_NV 0x8532
#define GL_UNSIGNED_IDENTITY_NV           0x8536
#define GL_UNSIGNED_INVERT_NV             0x8537
#define GL_EXPAND_NORMAL_NV               0x8538
#define GL_EXPAND_NEGATE_NV               0x8539
#define GL_HALF_BIAS_NORMAL_NV            0x853A
#define GL_HALF_BIAS_NEGATE_NV            0x853B
#define GL_SIGNED_IDENTITY_NV             0x853C
#define GL_SIGNED_NEGATE_NV               0x853D
#define GL_SCALE_BY_TWO_NV                0x853E
#define GL_SCALE_BY_FOUR_NV               0x853F
#define GL_SCALE_BY_ONE_HALF_NV           0x8540
#define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV   0x8541
#define GL_COMBINER_INPUT_NV              0x8542
#define GL_COMBINER_MAPPING_NV            0x8543
#define GL_COMBINER_COMPONENT_USAGE_NV    0x8544
#define GL_COMBINER_AB_DOT_PRODUCT_NV     0x8545
#define GL_COMBINER_CD_DOT_PRODUCT_NV     0x8546
#define GL_COMBINER_MUX_SUM_NV            0x8547
#define GL_COMBINER_SCALE_NV              0x8548
#define GL_COMBINER_BIAS_NV               0x8549
#define GL_COMBINER_AB_OUTPUT_NV          0x854A
#define GL_COMBINER_CD_OUTPUT_NV          0x854B
#define GL_COMBINER_SUM_OUTPUT_NV         0x854C
#define GL_MAX_GENERAL_COMBINERS_NV       0x854D
#define GL_NUM_GENERAL_COMBINERS_NV       0x854E
#define GL_COLOR_SUM_CLAMP_NV             0x854F
#define GL_COMBINER0_NV                   0x8550
#define GL_COMBINER1_NV                   0x8551
#define GL_COMBINER2_NV                   0x8552
#define GL_COMBINER3_NV                   0x8553
#define GL_COMBINER4_NV                   0x8554
#define GL_COMBINER5_NV                   0x8555
#define GL_COMBINER6_NV                   0x8556
#define GL_COMBINER7_NV                   0x8557
/* reuse GL_TEXTURE0_ARB */
/* reuse GL_TEXTURE1_ARB */
/* reuse GL_ZERO */
/* reuse GL_NONE */
/* reuse GL_FOG */
#endif

#ifndef GL_NV_fog_distance
#define GL_FOG_DISTANCE_MODE_NV           0x855A
#define GL_EYE_RADIAL_NV                  0x855B
#define GL_EYE_PLANE_ABSOLUTE_NV          0x855C
/* reuse GL_EYE_PLANE */
#endif

#ifndef GL_NV_texgen_emboss
#define GL_EMBOSS_LIGHT_NV                0x855D
#define GL_EMBOSS_CONSTANT_NV             0x855E
#define GL_EMBOSS_MAP_NV                  0x855F
#endif

#ifndef GL_NV_blend_square
#endif

#ifndef GL_NV_texture_env_combine4
#define GL_COMBINE4_NV                    0x8503
#define GL_SOURCE3_RGB_NV                 0x8583
#define GL_SOURCE3_ALPHA_NV               0x858B
#define GL_OPERAND3_RGB_NV                0x8593
#define GL_OPERAND3_ALPHA_NV              0x859B
#endif

#ifndef GL_MESA_resize_buffers
#endif

#ifndef GL_MESA_window_pos
#endif

#ifndef GL_EXT_texture_compression_s3tc
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

#ifndef GL_IBM_cull_vertex
#define GL_CULL_VERTEX_IBM                103050
#endif

#ifndef GL_IBM_multimode_draw_arrays
#endif

#ifndef GL_IBM_vertex_array_lists
#define GL_VERTEX_ARRAY_LIST_IBM          103070
#define GL_NORMAL_ARRAY_LIST_IBM          103071
#define GL_COLOR_ARRAY_LIST_IBM           103072
#define GL_INDEX_ARRAY_LIST_IBM           103073
#define GL_TEXTURE_COORD_ARRAY_LIST_IBM   103074
#define GL_EDGE_FLAG_ARRAY_LIST_IBM       103075
#define GL_FOG_COORDINATE_ARRAY_LIST_IBM  103076
#define GL_SECONDARY_COLOR_ARRAY_LIST_IBM 103077
#define GL_VERTEX_ARRAY_LIST_STRIDE_IBM   103080
#define GL_NORMAL_ARRAY_LIST_STRIDE_IBM   103081
#define GL_COLOR_ARRAY_LIST_STRIDE_IBM    103082
#define GL_INDEX_ARRAY_LIST_STRIDE_IBM    103083
#define GL_TEXTURE_COORD_ARRAY_LIST_STRIDE_IBM 103084
#define GL_EDGE_FLAG_ARRAY_LIST_STRIDE_IBM 103085
#define GL_FOG_COORDINATE_ARRAY_LIST_STRIDE_IBM 103086
#define GL_SECONDARY_COLOR_ARRAY_LIST_STRIDE_IBM 103087
#endif

#ifndef GL_SGIX_subsample
#define GL_PACK_SUBSAMPLE_RATE_SGIX       0x85A0
#define GL_UNPACK_SUBSAMPLE_RATE_SGIX     0x85A1
#define GL_PIXEL_SUBSAMPLE_4444_SGIX      0x85A2
#define GL_PIXEL_SUBSAMPLE_2424_SGIX      0x85A3
#define GL_PIXEL_SUBSAMPLE_4242_SGIX      0x85A4
#endif

#ifndef GL_SGIX_ycrcb_subsample
#endif

#ifndef GL_SGIX_ycrcba
#define GL_YCRCB_SGIX                     0x8318
#define GL_YCRCBA_SGIX                    0x8319
#endif

#ifndef GL_SGI_depth_pass_instrument
#define GL_DEPTH_PASS_INSTRUMENT_SGIX     0x8310
#define GL_DEPTH_PASS_INSTRUMENT_COUNTERS_SGIX 0x8311
#define GL_DEPTH_PASS_INSTRUMENT_MAX_SGIX 0x8312
#endif

#ifndef GL_3DFX_texture_compression_FXT1
#define GL_COMPRESSED_RGB_FXT1_3DFX       0x86B0
#define GL_COMPRESSED_RGBA_FXT1_3DFX      0x86B1
#endif

#ifndef GL_3DFX_multisample
#define GL_MULTISAMPLE_3DFX               0x86B2
#define GL_SAMPLE_BUFFERS_3DFX            0x86B3
#define GL_SAMPLES_3DFX                   0x86B4
#define GL_MULTISAMPLE_BIT_3DFX           0x20000000
#endif

#ifndef GL_3DFX_tbuffer
#endif

#ifndef GL_EXT_multisample
#define GL_MULTISAMPLE_EXT                0x809D
#define GL_SAMPLE_ALPHA_TO_MASK_EXT       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_EXT        0x809F
#define GL_SAMPLE_MASK_EXT                0x80A0
#define GL_1PASS_EXT                      0x80A1
#define GL_2PASS_0_EXT                    0x80A2
#define GL_2PASS_1_EXT                    0x80A3
#define GL_4PASS_0_EXT                    0x80A4
#define GL_4PASS_1_EXT                    0x80A5
#define GL_4PASS_2_EXT                    0x80A6
#define GL_4PASS_3_EXT                    0x80A7
#define GL_SAMPLE_BUFFERS_EXT             0x80A8
#define GL_SAMPLES_EXT                    0x80A9
#define GL_SAMPLE_MASK_VALUE_EXT          0x80AA
#define GL_SAMPLE_MASK_INVERT_EXT         0x80AB
#define GL_SAMPLE_PATTERN_EXT             0x80AC
#define GL_MULTISAMPLE_BIT_EXT            0x20000000
#endif

#ifndef GL_SGIX_vertex_preclip
#define GL_VERTEX_PRECLIP_SGIX            0x83EE
#define GL_VERTEX_PRECLIP_HINT_SGIX       0x83EF
#endif

#ifndef GL_SGIX_convolution_accuracy
#define GL_CONVOLUTION_HINT_SGIX          0x8316
#endif

#ifndef GL_SGIX_resample
#define GL_PACK_RESAMPLE_SGIX             0x842C
#define GL_UNPACK_RESAMPLE_SGIX           0x842D
#define GL_RESAMPLE_REPLICATE_SGIX        0x842E
#define GL_RESAMPLE_ZERO_FILL_SGIX        0x842F
#define GL_RESAMPLE_DECIMATE_SGIX         0x8430
#endif

#ifndef GL_SGIS_point_line_texgen
#define GL_EYE_DISTANCE_TO_POINT_SGIS     0x81F0
#define GL_OBJECT_DISTANCE_TO_POINT_SGIS  0x81F1
#define GL_EYE_DISTANCE_TO_LINE_SGIS      0x81F2
#define GL_OBJECT_DISTANCE_TO_LINE_SGIS   0x81F3
#define GL_EYE_POINT_SGIS                 0x81F4
#define GL_OBJECT_POINT_SGIS              0x81F5
#define GL_EYE_LINE_SGIS                  0x81F6
#define GL_OBJECT_LINE_SGIS               0x81F7
#endif

#ifndef GL_SGIS_texture_color_mask
#define GL_TEXTURE_COLOR_WRITEMASK_SGIS   0x81EF
#endif

#ifndef GL_EXT_texture_env_dot3
#define GL_DOT3_RGB_EXT                   0x8740
#define GL_DOT3_RGBA_EXT                  0x8741
#endif

#ifndef GL_ATI_texture_mirror_once
#define GL_MIRROR_CLAMP_ATI               0x8742
#define GL_MIRROR_CLAMP_TO_EDGE_ATI       0x8743
#endif

#ifndef GL_NV_fence
#define GL_ALL_COMPLETED_NV               0x84F2
#define GL_FENCE_STATUS_NV                0x84F3
#define GL_FENCE_CONDITION_NV             0x84F4
#endif

#ifndef GL_IBM_texture_mirrored_repeat
#define GL_MIRRORED_REPEAT_IBM            0x8370
#endif

#ifndef GL_NV_evaluators
#define GL_EVAL_2D_NV                     0x86C0
#define GL_EVAL_TRIANGULAR_2D_NV          0x86C1
#define GL_MAP_TESSELLATION_NV            0x86C2
#define GL_MAP_ATTRIB_U_ORDER_NV          0x86C3
#define GL_MAP_ATTRIB_V_ORDER_NV          0x86C4
#define GL_EVAL_FRACTIONAL_TESSELLATION_NV 0x86C5
#define GL_EVAL_VERTEX_ATTRIB0_NV         0x86C6
#define GL_EVAL_VERTEX_ATTRIB1_NV         0x86C7
#define GL_EVAL_VERTEX_ATTRIB2_NV         0x86C8
#define GL_EVAL_VERTEX_ATTRIB3_NV         0x86C9
#define GL_EVAL_VERTEX_ATTRIB4_NV         0x86CA
#define GL_EVAL_VERTEX_ATTRIB5_NV         0x86CB
#define GL_EVAL_VERTEX_ATTRIB6_NV         0x86CC
#define GL_EVAL_VERTEX_ATTRIB7_NV         0x86CD
#define GL_EVAL_VERTEX_ATTRIB8_NV         0x86CE
#define GL_EVAL_VERTEX_ATTRIB9_NV         0x86CF
#define GL_EVAL_VERTEX_ATTRIB10_NV        0x86D0
#define GL_EVAL_VERTEX_ATTRIB11_NV        0x86D1
#define GL_EVAL_VERTEX_ATTRIB12_NV        0x86D2
#define GL_EVAL_VERTEX_ATTRIB13_NV        0x86D3
#define GL_EVAL_VERTEX_ATTRIB14_NV        0x86D4
#define GL_EVAL_VERTEX_ATTRIB15_NV        0x86D5
#define GL_MAX_MAP_TESSELLATION_NV        0x86D6
#define GL_MAX_RATIONAL_EVAL_ORDER_NV     0x86D7
#endif

#ifndef GL_NV_packed_depth_stencil
#define GL_DEPTH_STENCIL_NV               0x84F9
#define GL_UNSIGNED_INT_24_8_NV           0x84FA
#endif

#ifndef GL_NV_register_combiners2
#define GL_PER_STAGE_CONSTANTS_NV         0x8535
#endif

#ifndef GL_NV_texture_compression_vtc
#endif

#ifndef GL_NV_texture_rectangle
#define GL_TEXTURE_RECTANGLE_NV           0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_NV   0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_NV     0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_NV  0x84F8
#endif

#ifndef GL_NV_texture_shader
#define GL_OFFSET_TEXTURE_RECTANGLE_NV    0x864C
#define GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV 0x864D
#define GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV 0x864E
#define GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV 0x86D9
#define GL_UNSIGNED_INT_S8_S8_8_8_NV      0x86DA
#define GL_UNSIGNED_INT_8_8_S8_S8_REV_NV  0x86DB
#define GL_DSDT_MAG_INTENSITY_NV          0x86DC
#define GL_SHADER_CONSISTENT_NV           0x86DD
#define GL_TEXTURE_SHADER_NV              0x86DE
#define GL_SHADER_OPERATION_NV            0x86DF
#define GL_CULL_MODES_NV                  0x86E0
#define GL_OFFSET_TEXTURE_MATRIX_NV       0x86E1
#define GL_OFFSET_TEXTURE_SCALE_NV        0x86E2
#define GL_OFFSET_TEXTURE_BIAS_NV         0x86E3
#define GL_OFFSET_TEXTURE_2D_MATRIX_NV    GL_OFFSET_TEXTURE_MATRIX_NV
#define GL_OFFSET_TEXTURE_2D_SCALE_NV     GL_OFFSET_TEXTURE_SCALE_NV
#define GL_OFFSET_TEXTURE_2D_BIAS_NV      GL_OFFSET_TEXTURE_BIAS_NV
#define GL_PREVIOUS_TEXTURE_INPUT_NV      0x86E4
#define GL_CONST_EYE_NV                   0x86E5
#define GL_PASS_THROUGH_NV                0x86E6
#define GL_CULL_FRAGMENT_NV               0x86E7
#define GL_OFFSET_TEXTURE_2D_NV           0x86E8
#define GL_DEPENDENT_AR_TEXTURE_2D_NV     0x86E9
#define GL_DEPENDENT_GB_TEXTURE_2D_NV     0x86EA
#define GL_DOT_PRODUCT_NV                 0x86EC
#define GL_DOT_PRODUCT_DEPTH_REPLACE_NV   0x86ED
#define GL_DOT_PRODUCT_TEXTURE_2D_NV      0x86EE
#define GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV 0x86F0
#define GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV 0x86F1
#define GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV 0x86F2
#define GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV 0x86F3
#define GL_HILO_NV                        0x86F4
#define GL_DSDT_NV                        0x86F5
#define GL_DSDT_MAG_NV                    0x86F6
#define GL_DSDT_MAG_VIB_NV                0x86F7
#define GL_HILO16_NV                      0x86F8
#define GL_SIGNED_HILO_NV                 0x86F9
#define GL_SIGNED_HILO16_NV               0x86FA
#define GL_SIGNED_RGBA_NV                 0x86FB
#define GL_SIGNED_RGBA8_NV                0x86FC
#define GL_SIGNED_RGB_NV                  0x86FE
#define GL_SIGNED_RGB8_NV                 0x86FF
#define GL_SIGNED_LUMINANCE_NV            0x8701
#define GL_SIGNED_LUMINANCE8_NV           0x8702
#define GL_SIGNED_LUMINANCE_ALPHA_NV      0x8703
#define GL_SIGNED_LUMINANCE8_ALPHA8_NV    0x8704
#define GL_SIGNED_ALPHA_NV                0x8705
#define GL_SIGNED_ALPHA8_NV               0x8706
#define GL_SIGNED_INTENSITY_NV            0x8707
#define GL_SIGNED_INTENSITY8_NV           0x8708
#define GL_DSDT8_NV                       0x8709
#define GL_DSDT8_MAG8_NV                  0x870A
#define GL_DSDT8_MAG8_INTENSITY8_NV       0x870B
#define GL_SIGNED_RGB_UNSIGNED_ALPHA_NV   0x870C
#define GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV 0x870D
#define GL_HI_SCALE_NV                    0x870E
#define GL_LO_SCALE_NV                    0x870F
#define GL_DS_SCALE_NV                    0x8710
#define GL_DT_SCALE_NV                    0x8711
#define GL_MAGNITUDE_SCALE_NV             0x8712
#define GL_VIBRANCE_SCALE_NV              0x8713
#define GL_HI_BIAS_NV                     0x8714
#define GL_LO_BIAS_NV                     0x8715
#define GL_DS_BIAS_NV                     0x8716
#define GL_DT_BIAS_NV                     0x8717
#define GL_MAGNITUDE_BIAS_NV              0x8718
#define GL_VIBRANCE_BIAS_NV               0x8719
#define GL_TEXTURE_BORDER_VALUES_NV       0x871A
#define GL_TEXTURE_HI_SIZE_NV             0x871B
#define GL_TEXTURE_LO_SIZE_NV             0x871C
#define GL_TEXTURE_DS_SIZE_NV             0x871D
#define GL_TEXTURE_DT_SIZE_NV             0x871E
#define GL_TEXTURE_MAG_SIZE_NV            0x871F
#endif

#ifndef GL_NV_texture_shader2
#define GL_DOT_PRODUCT_TEXTURE_3D_NV      0x86EF
#endif

#ifndef GL_NV_vertex_array_range2
#define GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV 0x8533
#endif

#ifndef GL_NV_vertex_program
#define GL_VERTEX_PROGRAM_NV              0x8620
#define GL_VERTEX_STATE_PROGRAM_NV        0x8621
#define GL_ATTRIB_ARRAY_SIZE_NV           0x8623
#define GL_ATTRIB_ARRAY_STRIDE_NV         0x8624
#define GL_ATTRIB_ARRAY_TYPE_NV           0x8625
#define GL_CURRENT_ATTRIB_NV              0x8626
#define GL_PROGRAM_LENGTH_NV              0x8627
#define GL_PROGRAM_STRING_NV              0x8628
#define GL_MODELVIEW_PROJECTION_NV        0x8629
#define GL_IDENTITY_NV                    0x862A
#define GL_INVERSE_NV                     0x862B
#define GL_TRANSPOSE_NV                   0x862C
#define GL_INVERSE_TRANSPOSE_NV           0x862D
#define GL_MAX_TRACK_MATRIX_STACK_DEPTH_NV 0x862E
#define GL_MAX_TRACK_MATRICES_NV          0x862F
#define GL_MATRIX0_NV                     0x8630
#define GL_MATRIX1_NV                     0x8631
#define GL_MATRIX2_NV                     0x8632
#define GL_MATRIX3_NV                     0x8633
#define GL_MATRIX4_NV                     0x8634
#define GL_MATRIX5_NV                     0x8635
#define GL_MATRIX6_NV                     0x8636
#define GL_MATRIX7_NV                     0x8637
#define GL_CURRENT_MATRIX_STACK_DEPTH_NV  0x8640
#define GL_CURRENT_MATRIX_NV              0x8641
#define GL_VERTEX_PROGRAM_POINT_SIZE_NV   0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_NV     0x8643
#define GL_PROGRAM_PARAMETER_NV           0x8644
#define GL_ATTRIB_ARRAY_POINTER_NV        0x8645
#define GL_PROGRAM_TARGET_NV              0x8646
#define GL_PROGRAM_RESIDENT_NV            0x8647
#define GL_TRACK_MATRIX_NV                0x8648
#define GL_TRACK_MATRIX_TRANSFORM_NV      0x8649
#define GL_VERTEX_PROGRAM_BINDING_NV      0x864A
#define GL_PROGRAM_ERROR_POSITION_NV      0x864B
#define GL_VERTEX_ATTRIB_ARRAY0_NV        0x8650
#define GL_VERTEX_ATTRIB_ARRAY1_NV        0x8651
#define GL_VERTEX_ATTRIB_ARRAY2_NV        0x8652
#define GL_VERTEX_ATTRIB_ARRAY3_NV        0x8653
#define GL_VERTEX_ATTRIB_ARRAY4_NV        0x8654
#define GL_VERTEX_ATTRIB_ARRAY5_NV        0x8655
#define GL_VERTEX_ATTRIB_ARRAY6_NV        0x8656
#define GL_VERTEX_ATTRIB_ARRAY7_NV        0x8657
#define GL_VERTEX_ATTRIB_ARRAY8_NV        0x8658
#define GL_VERTEX_ATTRIB_ARRAY9_NV        0x8659
#define GL_VERTEX_ATTRIB_ARRAY10_NV       0x865A
#define GL_VERTEX_ATTRIB_ARRAY11_NV       0x865B
#define GL_VERTEX_ATTRIB_ARRAY12_NV       0x865C
#define GL_VERTEX_ATTRIB_ARRAY13_NV       0x865D
#define GL_VERTEX_ATTRIB_ARRAY14_NV       0x865E
#define GL_VERTEX_ATTRIB_ARRAY15_NV       0x865F
#define GL_MAP1_VERTEX_ATTRIB0_4_NV       0x8660
#define GL_MAP1_VERTEX_ATTRIB1_4_NV       0x8661
#define GL_MAP1_VERTEX_ATTRIB2_4_NV       0x8662
#define GL_MAP1_VERTEX_ATTRIB3_4_NV       0x8663
#define GL_MAP1_VERTEX_ATTRIB4_4_NV       0x8664
#define GL_MAP1_VERTEX_ATTRIB5_4_NV       0x8665
#define GL_MAP1_VERTEX_ATTRIB6_4_NV       0x8666
#define GL_MAP1_VERTEX_ATTRIB7_4_NV       0x8667
#define GL_MAP1_VERTEX_ATTRIB8_4_NV       0x8668
#define GL_MAP1_VERTEX_ATTRIB9_4_NV       0x8669
#define GL_MAP1_VERTEX_ATTRIB10_4_NV      0x866A
#define GL_MAP1_VERTEX_ATTRIB11_4_NV      0x866B
#define GL_MAP1_VERTEX_ATTRIB12_4_NV      0x866C
#define GL_MAP1_VERTEX_ATTRIB13_4_NV      0x866D
#define GL_MAP1_VERTEX_ATTRIB14_4_NV      0x866E
#define GL_MAP1_VERTEX_ATTRIB15_4_NV      0x866F
#define GL_MAP2_VERTEX_ATTRIB0_4_NV       0x8670
#define GL_MAP2_VERTEX_ATTRIB1_4_NV       0x8671
#define GL_MAP2_VERTEX_ATTRIB2_4_NV       0x8672
#define GL_MAP2_VERTEX_ATTRIB3_4_NV       0x8673
#define GL_MAP2_VERTEX_ATTRIB4_4_NV       0x8674
#define GL_MAP2_VERTEX_ATTRIB5_4_NV       0x8675
#define GL_MAP2_VERTEX_ATTRIB6_4_NV       0x8676
#define GL_MAP2_VERTEX_ATTRIB7_4_NV       0x8677
#define GL_MAP2_VERTEX_ATTRIB8_4_NV       0x8678
#define GL_MAP2_VERTEX_ATTRIB9_4_NV       0x8679
#define GL_MAP2_VERTEX_ATTRIB10_4_NV      0x867A
#define GL_MAP2_VERTEX_ATTRIB11_4_NV      0x867B
#define GL_MAP2_VERTEX_ATTRIB12_4_NV      0x867C
#define GL_MAP2_VERTEX_ATTRIB13_4_NV      0x867D
#define GL_MAP2_VERTEX_ATTRIB14_4_NV      0x867E
#define GL_MAP2_VERTEX_ATTRIB15_4_NV      0x867F
#endif

#ifndef GL_SGIX_texture_coordinate_clamp
#define GL_TEXTURE_MAX_CLAMP_S_SGIX       0x8369
#define GL_TEXTURE_MAX_CLAMP_T_SGIX       0x836A
#define GL_TEXTURE_MAX_CLAMP_R_SGIX       0x836B
#endif

#ifndef GL_SGIX_scalebias_hint
#define GL_SCALEBIAS_HINT_SGIX            0x8322
#endif

#ifndef GL_OML_interlace
#define GL_INTERLACE_OML                  0x8980
#define GL_INTERLACE_READ_OML             0x8981
#endif

#ifndef GL_OML_subsample
#define GL_FORMAT_SUBSAMPLE_24_24_OML     0x8982
#define GL_FORMAT_SUBSAMPLE_244_244_OML   0x8983
#endif

#ifndef GL_OML_resample
#define GL_PACK_RESAMPLE_OML              0x8984
#define GL_UNPACK_RESAMPLE_OML            0x8985
#define GL_RESAMPLE_REPLICATE_OML         0x8986
#define GL_RESAMPLE_ZERO_FILL_OML         0x8987
#define GL_RESAMPLE_AVERAGE_OML           0x8988
#define GL_RESAMPLE_DECIMATE_OML          0x8989
#endif

#ifndef GL_NV_copy_depth_to_color
#define GL_DEPTH_STENCIL_TO_RGBA_NV       0x886E
#define GL_DEPTH_STENCIL_TO_BGRA_NV       0x886F
#endif

#ifndef GL_ATI_envmap_bumpmap
#define GL_BUMP_ROT_MATRIX_ATI            0x8775
#define GL_BUMP_ROT_MATRIX_SIZE_ATI       0x8776
#define GL_BUMP_NUM_TEX_UNITS_ATI         0x8777
#define GL_BUMP_TEX_UNITS_ATI             0x8778
#define GL_DUDV_ATI                       0x8779
#define GL_DU8DV8_ATI                     0x877A
#define GL_BUMP_ENVMAP_ATI                0x877B
#define GL_BUMP_TARGET_ATI                0x877C
#endif

#ifndef GL_ATI_fragment_shader
#define GL_FRAGMENT_SHADER_ATI            0x8920
#define GL_REG_0_ATI                      0x8921
#define GL_REG_1_ATI                      0x8922
#define GL_REG_2_ATI                      0x8923
#define GL_REG_3_ATI                      0x8924
#define GL_REG_4_ATI                      0x8925
#define GL_REG_5_ATI                      0x8926
#define GL_REG_6_ATI                      0x8927
#define GL_REG_7_ATI                      0x8928
#define GL_REG_8_ATI                      0x8929
#define GL_REG_9_ATI                      0x892A
#define GL_REG_10_ATI                     0x892B
#define GL_REG_11_ATI                     0x892C
#define GL_REG_12_ATI                     0x892D
#define GL_REG_13_ATI                     0x892E
#define GL_REG_14_ATI                     0x892F
#define GL_REG_15_ATI                     0x8930
#define GL_REG_16_ATI                     0x8931
#define GL_REG_17_ATI                     0x8932
#define GL_REG_18_ATI                     0x8933
#define GL_REG_19_ATI                     0x8934
#define GL_REG_20_ATI                     0x8935
#define GL_REG_21_ATI                     0x8936
#define GL_REG_22_ATI                     0x8937
#define GL_REG_23_ATI                     0x8938
#define GL_REG_24_ATI                     0x8939
#define GL_REG_25_ATI                     0x893A
#define GL_REG_26_ATI                     0x893B
#define GL_REG_27_ATI                     0x893C
#define GL_REG_28_ATI                     0x893D
#define GL_REG_29_ATI                     0x893E
#define GL_REG_30_ATI                     0x893F
#define GL_REG_31_ATI                     0x8940
#define GL_CON_0_ATI                      0x8941
#define GL_CON_1_ATI                      0x8942
#define GL_CON_2_ATI                      0x8943
#define GL_CON_3_ATI                      0x8944
#define GL_CON_4_ATI                      0x8945
#define GL_CON_5_ATI                      0x8946
#define GL_CON_6_ATI                      0x8947
#define GL_CON_7_ATI                      0x8948
#define GL_CON_8_ATI                      0x8949
#define GL_CON_9_ATI                      0x894A
#define GL_CON_10_ATI                     0x894B
#define GL_CON_11_ATI                     0x894C
#define GL_CON_12_ATI                     0x894D
#define GL_CON_13_ATI                     0x894E
#define GL_CON_14_ATI                     0x894F
#define GL_CON_15_ATI                     0x8950
#define GL_CON_16_ATI                     0x8951
#define GL_CON_17_ATI                     0x8952
#define GL_CON_18_ATI                     0x8953
#define GL_CON_19_ATI                     0x8954
#define GL_CON_20_ATI                     0x8955
#define GL_CON_21_ATI                     0x8956
#define GL_CON_22_ATI                     0x8957
#define GL_CON_23_ATI                     0x8958
#define GL_CON_24_ATI                     0x8959
#define GL_CON_25_ATI                     0x895A
#define GL_CON_26_ATI                     0x895B
#define GL_CON_27_ATI                     0x895C
#define GL_CON_28_ATI                     0x895D
#define GL_CON_29_ATI                     0x895E
#define GL_CON_30_ATI                     0x895F
#define GL_CON_31_ATI                     0x8960
#define GL_MOV_ATI                        0x8961
#define GL_ADD_ATI                        0x8963
#define GL_MUL_ATI                        0x8964
#define GL_SUB_ATI                        0x8965
#define GL_DOT3_ATI                       0x8966
#define GL_DOT4_ATI                       0x8967
#define GL_MAD_ATI                        0x8968
#define GL_LERP_ATI                       0x8969
#define GL_CND_ATI                        0x896A
#define GL_CND0_ATI                       0x896B
#define GL_DOT2_ADD_ATI                   0x896C
#define GL_SECONDARY_INTERPOLATOR_ATI     0x896D
#define GL_NUM_FRAGMENT_REGISTERS_ATI     0x896E
#define GL_NUM_FRAGMENT_CONSTANTS_ATI     0x896F
#define GL_NUM_PASSES_ATI                 0x8970
#define GL_NUM_INSTRUCTIONS_PER_PASS_ATI  0x8971
#define GL_NUM_INSTRUCTIONS_TOTAL_ATI     0x8972
#define GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI 0x8973
#define GL_NUM_LOOPBACK_COMPONENTS_ATI    0x8974
#define GL_COLOR_ALPHA_PAIRING_ATI        0x8975
#define GL_SWIZZLE_STR_ATI                0x8976
#define GL_SWIZZLE_STQ_ATI                0x8977
#define GL_SWIZZLE_STR_DR_ATI             0x8978
#define GL_SWIZZLE_STQ_DQ_ATI             0x8979
#define GL_SWIZZLE_STRQ_ATI               0x897A
#define GL_SWIZZLE_STRQ_DQ_ATI            0x897B
#define GL_RED_BIT_ATI                    0x00000001
#define GL_GREEN_BIT_ATI                  0x00000002
#define GL_BLUE_BIT_ATI                   0x00000004
#define GL_2X_BIT_ATI                     0x00000001
#define GL_4X_BIT_ATI                     0x00000002
#define GL_8X_BIT_ATI                     0x00000004
#define GL_HALF_BIT_ATI                   0x00000008
#define GL_QUARTER_BIT_ATI                0x00000010
#define GL_EIGHTH_BIT_ATI                 0x00000020
#define GL_SATURATE_BIT_ATI               0x00000040
#define GL_COMP_BIT_ATI                   0x00000002
#define GL_NEGATE_BIT_ATI                 0x00000004
#define GL_BIAS_BIT_ATI                   0x00000008
#endif

#ifndef GL_ATI_pn_triangles
#define GL_PN_TRIANGLES_ATI               0x87F0
#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI 0x87F1
#define GL_PN_TRIANGLES_POINT_MODE_ATI    0x87F2
#define GL_PN_TRIANGLES_NORMAL_MODE_ATI   0x87F3
#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI 0x87F4
#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI 0x87F5
#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI 0x87F6
#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI 0x87F7
#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI 0x87F8
#endif

#ifndef GL_ATI_vertex_array_object
#define GL_STATIC_ATI                     0x8760
#define GL_DYNAMIC_ATI                    0x8761
#define GL_PRESERVE_ATI                   0x8762
#define GL_DISCARD_ATI                    0x8763
#define GL_OBJECT_BUFFER_SIZE_ATI         0x8764
#define GL_OBJECT_BUFFER_USAGE_ATI        0x8765
#define GL_ARRAY_OBJECT_BUFFER_ATI        0x8766
#define GL_ARRAY_OBJECT_OFFSET_ATI        0x8767
#endif

#ifndef GL_EXT_vertex_shader
#define GL_VERTEX_SHADER_EXT              0x8780
#define GL_VERTEX_SHADER_BINDING_EXT      0x8781
#define GL_OP_INDEX_EXT                   0x8782
#define GL_OP_NEGATE_EXT                  0x8783
#define GL_OP_DOT3_EXT                    0x8784
#define GL_OP_DOT4_EXT                    0x8785
#define GL_OP_MUL_EXT                     0x8786
#define GL_OP_ADD_EXT                     0x8787
#define GL_OP_MADD_EXT                    0x8788
#define GL_OP_FRAC_EXT                    0x8789
#define GL_OP_MAX_EXT                     0x878A
#define GL_OP_MIN_EXT                     0x878B
#define GL_OP_SET_GE_EXT                  0x878C
#define GL_OP_SET_LT_EXT                  0x878D
#define GL_OP_CLAMP_EXT                   0x878E
#define GL_OP_FLOOR_EXT                   0x878F
#define GL_OP_ROUND_EXT                   0x8790
#define GL_OP_EXP_BASE_2_EXT              0x8791
#define GL_OP_LOG_BASE_2_EXT              0x8792
#define GL_OP_POWER_EXT                   0x8793
#define GL_OP_RECIP_EXT                   0x8794
#define GL_OP_RECIP_SQRT_EXT              0x8795
#define GL_OP_SUB_EXT                     0x8796
#define GL_OP_CROSS_PRODUCT_EXT           0x8797
#define GL_OP_MULTIPLY_MATRIX_EXT         0x8798
#define GL_OP_MOV_EXT                     0x8799
#define GL_OUTPUT_VERTEX_EXT              0x879A
#define GL_OUTPUT_COLOR0_EXT              0x879B
#define GL_OUTPUT_COLOR1_EXT              0x879C
#define GL_OUTPUT_TEXTURE_COORD0_EXT      0x879D
#define GL_OUTPUT_TEXTURE_COORD1_EXT      0x879E
#define GL_OUTPUT_TEXTURE_COORD2_EXT      0x879F
#define GL_OUTPUT_TEXTURE_COORD3_EXT      0x87A0
#define GL_OUTPUT_TEXTURE_COORD4_EXT      0x87A1
#define GL_OUTPUT_TEXTURE_COORD5_EXT      0x87A2
#define GL_OUTPUT_TEXTURE_COORD6_EXT      0x87A3
#define GL_OUTPUT_TEXTURE_COORD7_EXT      0x87A4
#define GL_OUTPUT_TEXTURE_COORD8_EXT      0x87A5
#define GL_OUTPUT_TEXTURE_COORD9_EXT      0x87A6
#define GL_OUTPUT_TEXTURE_COORD10_EXT     0x87A7
#define GL_OUTPUT_TEXTURE_COORD11_EXT     0x87A8
#define GL_OUTPUT_TEXTURE_COORD12_EXT     0x87A9
#define GL_OUTPUT_TEXTURE_COORD13_EXT     0x87AA
#define GL_OUTPUT_TEXTURE_COORD14_EXT     0x87AB
#define GL_OUTPUT_TEXTURE_COORD15_EXT     0x87AC
#define GL_OUTPUT_TEXTURE_COORD16_EXT     0x87AD
#define GL_OUTPUT_TEXTURE_COORD17_EXT     0x87AE
#define GL_OUTPUT_TEXTURE_COORD18_EXT     0x87AF
#define GL_OUTPUT_TEXTURE_COORD19_EXT     0x87B0
#define GL_OUTPUT_TEXTURE_COORD20_EXT     0x87B1
#define GL_OUTPUT_TEXTURE_COORD21_EXT     0x87B2
#define GL_OUTPUT_TEXTURE_COORD22_EXT     0x87B3
#define GL_OUTPUT_TEXTURE_COORD23_EXT     0x87B4
#define GL_OUTPUT_TEXTURE_COORD24_EXT     0x87B5
#define GL_OUTPUT_TEXTURE_COORD25_EXT     0x87B6
#define GL_OUTPUT_TEXTURE_COORD26_EXT     0x87B7
#define GL_OUTPUT_TEXTURE_COORD27_EXT     0x87B8
#define GL_OUTPUT_TEXTURE_COORD28_EXT     0x87B9
#define GL_OUTPUT_TEXTURE_COORD29_EXT     0x87BA
#define GL_OUTPUT_TEXTURE_COORD30_EXT     0x87BB
#define GL_OUTPUT_TEXTURE_COORD31_EXT     0x87BC
#define GL_OUTPUT_FOG_EXT                 0x87BD
#define GL_SCALAR_EXT                     0x87BE
#define GL_VECTOR_EXT                     0x87BF
#define GL_MATRIX_EXT                     0x87C0
#define GL_VARIANT_EXT                    0x87C1
#define GL_INVARIANT_EXT                  0x87C2
#define GL_LOCAL_CONSTANT_EXT             0x87C3
#define GL_LOCAL_EXT                      0x87C4
#define GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT 0x87C5
#define GL_MAX_VERTEX_SHADER_VARIANTS_EXT 0x87C6
#define GL_MAX_VERTEX_SHADER_INVARIANTS_EXT 0x87C7
#define GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT 0x87C8
#define GL_MAX_VERTEX_SHADER_LOCALS_EXT   0x87C9
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT 0x87CA
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT 0x87CB
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT 0x87CC
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT 0x87CD
#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT 0x87CE
#define GL_VERTEX_SHADER_INSTRUCTIONS_EXT 0x87CF
#define GL_VERTEX_SHADER_VARIANTS_EXT     0x87D0
#define GL_VERTEX_SHADER_INVARIANTS_EXT   0x87D1
#define GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT 0x87D2
#define GL_VERTEX_SHADER_LOCALS_EXT       0x87D3
#define GL_VERTEX_SHADER_OPTIMIZED_EXT    0x87D4
#define GL_X_EXT                          0x87D5
#define GL_Y_EXT                          0x87D6
#define GL_Z_EXT                          0x87D7
#define GL_W_EXT                          0x87D8
#define GL_NEGATIVE_X_EXT                 0x87D9
#define GL_NEGATIVE_Y_EXT                 0x87DA
#define GL_NEGATIVE_Z_EXT                 0x87DB
#define GL_NEGATIVE_W_EXT                 0x87DC
#define GL_ZERO_EXT                       0x87DD
#define GL_ONE_EXT                        0x87DE
#define GL_NEGATIVE_ONE_EXT               0x87DF
#define GL_NORMALIZED_RANGE_EXT           0x87E0
#define GL_FULL_RANGE_EXT                 0x87E1
#define GL_CURRENT_VERTEX_EXT             0x87E2
#define GL_MVP_MATRIX_EXT                 0x87E3
#define GL_VARIANT_VALUE_EXT              0x87E4
#define GL_VARIANT_DATATYPE_EXT           0x87E5
#define GL_VARIANT_ARRAY_STRIDE_EXT       0x87E6
#define GL_VARIANT_ARRAY_TYPE_EXT         0x87E7
#define GL_VARIANT_ARRAY_EXT              0x87E8
#define GL_VARIANT_ARRAY_POINTER_EXT      0x87E9
#define GL_INVARIANT_VALUE_EXT            0x87EA
#define GL_INVARIANT_DATATYPE_EXT         0x87EB
#define GL_LOCAL_CONSTANT_VALUE_EXT       0x87EC
#define GL_LOCAL_CONSTANT_DATATYPE_EXT    0x87ED
#endif

#ifndef GL_ATI_vertex_streams
#define GL_MAX_VERTEX_STREAMS_ATI         0x876B
#define GL_VERTEX_STREAM0_ATI             0x876C
#define GL_VERTEX_STREAM1_ATI             0x876D
#define GL_VERTEX_STREAM2_ATI             0x876E
#define GL_VERTEX_STREAM3_ATI             0x876F
#define GL_VERTEX_STREAM4_ATI             0x8770
#define GL_VERTEX_STREAM5_ATI             0x8771
#define GL_VERTEX_STREAM6_ATI             0x8772
#define GL_VERTEX_STREAM7_ATI             0x8773
#define GL_VERTEX_SOURCE_ATI              0x8774
#endif

#ifndef GL_ATI_element_array
#define GL_ELEMENT_ARRAY_ATI              0x8768
#define GL_ELEMENT_ARRAY_TYPE_ATI         0x8769
#define GL_ELEMENT_ARRAY_POINTER_ATI      0x876A
#endif

#ifndef GL_SUN_mesh_array
#define GL_QUAD_MESH_SUN                  0x8614
#define GL_TRIANGLE_MESH_SUN              0x8615
#endif

#ifndef GL_SUN_slice_accum
#define GL_SLICE_ACCUM_SUN                0x85CC
#endif

#ifndef GL_NV_multisample_filter_hint
#define GL_MULTISAMPLE_FILTER_HINT_NV     0x8534
#endif

#ifndef GL_NV_depth_clamp
#define GL_DEPTH_CLAMP_NV                 0x864F
#endif

#ifndef GL_NV_occlusion_query
#define GL_PIXEL_COUNTER_BITS_NV          0x8864
#define GL_CURRENT_OCCLUSION_QUERY_ID_NV  0x8865
#define GL_PIXEL_COUNT_NV                 0x8866
#define GL_PIXEL_COUNT_AVAILABLE_NV       0x8867
#endif

#ifndef GL_NV_point_sprite
#define GL_POINT_SPRITE_NV                0x8861
#define GL_COORD_REPLACE_NV               0x8862
#define GL_POINT_SPRITE_R_MODE_NV         0x8863
#endif

#ifndef GL_NV_texture_shader3
#define GL_OFFSET_PROJECTIVE_TEXTURE_2D_NV 0x8850
#define GL_OFFSET_PROJECTIVE_TEXTURE_2D_SCALE_NV 0x8851
#define GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_NV 0x8852
#define GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_SCALE_NV 0x8853
#define GL_OFFSET_HILO_TEXTURE_2D_NV      0x8854
#define GL_OFFSET_HILO_TEXTURE_RECTANGLE_NV 0x8855
#define GL_OFFSET_HILO_PROJECTIVE_TEXTURE_2D_NV 0x8856
#define GL_OFFSET_HILO_PROJECTIVE_TEXTURE_RECTANGLE_NV 0x8857
#define GL_DEPENDENT_HILO_TEXTURE_2D_NV   0x8858
#define GL_DEPENDENT_RGB_TEXTURE_3D_NV    0x8859
#define GL_DEPENDENT_RGB_TEXTURE_CUBE_MAP_NV 0x885A
#define GL_DOT_PRODUCT_PASS_THROUGH_NV    0x885B
#define GL_DOT_PRODUCT_TEXTURE_1D_NV      0x885C
#define GL_DOT_PRODUCT_AFFINE_DEPTH_REPLACE_NV 0x885D
#define GL_HILO8_NV                       0x885E
#define GL_SIGNED_HILO8_NV                0x885F
#define GL_FORCE_BLUE_TO_ONE_NV           0x8860
#endif

#ifndef GL_NV_vertex_program1_1
#endif

#ifndef GL_EXT_shadow_funcs
#endif

#ifndef GL_EXT_stencil_two_side
#define GL_STENCIL_TEST_TWO_SIDE_EXT      0x8910
#define GL_ACTIVE_STENCIL_FACE_EXT        0x8911
#endif

#ifndef GL_ATI_text_fragment_shader
#define GL_TEXT_FRAGMENT_SHADER_ATI       0x8200
#endif

#ifndef GL_APPLE_client_storage
#define GL_UNPACK_CLIENT_STORAGE_APPLE    0x85B2
#endif

#ifndef GL_APPLE_element_array
#define GL_ELEMENT_ARRAY_APPLE            0x8768
#define GL_ELEMENT_ARRAY_TYPE_APPLE       0x8769
#define GL_ELEMENT_ARRAY_POINTER_APPLE    0x876A
#endif

#ifndef GL_APPLE_fence
#define GL_DRAW_PIXELS_APPLE              0x8A0A
#define GL_FENCE_APPLE                    0x8A0B
#endif

#ifndef GL_APPLE_vertex_array_object
#define GL_VERTEX_ARRAY_BINDING_APPLE     0x85B5
#endif

#ifndef GL_APPLE_vertex_array_range
#define GL_VERTEX_ARRAY_RANGE_APPLE       0x851D
#define GL_VERTEX_ARRAY_RANGE_LENGTH_APPLE 0x851E
#define GL_VERTEX_ARRAY_STORAGE_HINT_APPLE 0x851F
#define GL_VERTEX_ARRAY_RANGE_POINTER_APPLE 0x8521
#define GL_STORAGE_CACHED_APPLE           0x85BE
#define GL_STORAGE_SHARED_APPLE           0x85BF
#endif

#ifndef GL_APPLE_ycbcr_422
#define GL_YCBCR_422_APPLE                0x85B9
#define GL_UNSIGNED_SHORT_8_8_APPLE       0x85BA
#define GL_UNSIGNED_SHORT_8_8_REV_APPLE   0x85BB
#endif

#ifndef GL_S3_s3tc
#define GL_RGB_S3TC                       0x83A0
#define GL_RGB4_S3TC                      0x83A1
#define GL_RGBA_S3TC                      0x83A2
#define GL_RGBA4_S3TC                     0x83A3
#endif

#ifndef GL_ATI_draw_buffers
#define GL_MAX_DRAW_BUFFERS_ATI           0x8824
#define GL_DRAW_BUFFER0_ATI               0x8825
#define GL_DRAW_BUFFER1_ATI               0x8826
#define GL_DRAW_BUFFER2_ATI               0x8827
#define GL_DRAW_BUFFER3_ATI               0x8828
#define GL_DRAW_BUFFER4_ATI               0x8829
#define GL_DRAW_BUFFER5_ATI               0x882A
#define GL_DRAW_BUFFER6_ATI               0x882B
#define GL_DRAW_BUFFER7_ATI               0x882C
#define GL_DRAW_BUFFER8_ATI               0x882D
#define GL_DRAW_BUFFER9_ATI               0x882E
#define GL_DRAW_BUFFER10_ATI              0x882F
#define GL_DRAW_BUFFER11_ATI              0x8830
#define GL_DRAW_BUFFER12_ATI              0x8831
#define GL_DRAW_BUFFER13_ATI              0x8832
#define GL_DRAW_BUFFER14_ATI              0x8833
#define GL_DRAW_BUFFER15_ATI              0x8834
#endif

#ifndef GL_ATI_texture_env_combine3
#define GL_MODULATE_ADD_ATI               0x8744
#define GL_MODULATE_SIGNED_ADD_ATI        0x8745
#define GL_MODULATE_SUBTRACT_ATI          0x8746
#endif

#ifndef GL_ATI_texture_float
#define GL_RGBA_FLOAT32_ATI               0x8814
#define GL_RGB_FLOAT32_ATI                0x8815
#define GL_ALPHA_FLOAT32_ATI              0x8816
#define GL_INTENSITY_FLOAT32_ATI          0x8817
#define GL_LUMINANCE_FLOAT32_ATI          0x8818
#define GL_LUMINANCE_ALPHA_FLOAT32_ATI    0x8819
#define GL_RGBA_FLOAT16_ATI               0x881A
#define GL_RGB_FLOAT16_ATI                0x881B
#define GL_ALPHA_FLOAT16_ATI              0x881C
#define GL_INTENSITY_FLOAT16_ATI          0x881D
#define GL_LUMINANCE_FLOAT16_ATI          0x881E
#define GL_LUMINANCE_ALPHA_FLOAT16_ATI    0x881F
#endif

#ifndef GL_NV_float_buffer
#define GL_FLOAT_R_NV                     0x8880
#define GL_FLOAT_RG_NV                    0x8881
#define GL_FLOAT_RGB_NV                   0x8882
#define GL_FLOAT_RGBA_NV                  0x8883
#define GL_FLOAT_R16_NV                   0x8884
#define GL_FLOAT_R32_NV                   0x8885
#define GL_FLOAT_RG16_NV                  0x8886
#define GL_FLOAT_RG32_NV                  0x8887
#define GL_FLOAT_RGB16_NV                 0x8888
#define GL_FLOAT_RGB32_NV                 0x8889
#define GL_FLOAT_RGBA16_NV                0x888A
#define GL_FLOAT_RGBA32_NV                0x888B
#define GL_TEXTURE_FLOAT_COMPONENTS_NV    0x888C
#define GL_FLOAT_CLEAR_COLOR_VALUE_NV     0x888D
#define GL_FLOAT_RGBA_MODE_NV             0x888E
#endif

#ifndef GL_NV_fragment_program
#define GL_MAX_FRAGMENT_PROGRAM_LOCAL_PARAMETERS_NV 0x8868
#define GL_FRAGMENT_PROGRAM_NV            0x8870
#define GL_MAX_TEXTURE_COORDS_NV          0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS_NV     0x8872
#define GL_FRAGMENT_PROGRAM_BINDING_NV    0x8873
#define GL_PROGRAM_ERROR_STRING_NV        0x8874
#endif

#ifndef GL_NV_half_float
#define GL_HALF_FLOAT_NV                  0x140B
#endif

#ifndef GL_NV_pixel_data_range
#define GL_WRITE_PIXEL_DATA_RANGE_NV      0x8878
#define GL_READ_PIXEL_DATA_RANGE_NV       0x8879
#define GL_WRITE_PIXEL_DATA_RANGE_LENGTH_NV 0x887A
#define GL_READ_PIXEL_DATA_RANGE_LENGTH_NV 0x887B
#define GL_WRITE_PIXEL_DATA_RANGE_POINTER_NV 0x887C
#define GL_READ_PIXEL_DATA_RANGE_POINTER_NV 0x887D
#endif

#ifndef GL_NV_primitive_restart
#define GL_PRIMITIVE_RESTART_NV           0x8558
#define GL_PRIMITIVE_RESTART_INDEX_NV     0x8559
#endif

#ifndef GL_NV_texture_expand_normal
#define GL_TEXTURE_UNSIGNED_REMAP_MODE_NV 0x888F
#endif

#ifndef GL_NV_vertex_program2
#endif

#ifndef GL_ATI_map_object_buffer
#endif

#ifndef GL_ATI_separate_stencil
#define GL_STENCIL_BACK_FUNC_ATI          0x8800
#define GL_STENCIL_BACK_FAIL_ATI          0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL_ATI 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS_ATI 0x8803
#endif

#ifndef GL_ATI_vertex_attrib_array_object
#endif

#ifndef GL_EXT_depth_bounds_test
#define GL_DEPTH_BOUNDS_TEST_EXT          0x8890
#define GL_DEPTH_BOUNDS_EXT               0x8891
#endif

#ifndef GL_EXT_texture_mirror_clamp
#define GL_MIRROR_CLAMP_EXT               0x8742
#define GL_MIRROR_CLAMP_TO_EDGE_EXT       0x8743
#define GL_MIRROR_CLAMP_TO_BORDER_EXT     0x8912
#endif

#ifndef GL_EXT_blend_equation_separate
#define GL_BLEND_EQUATION_RGB_EXT         GL_BLEND_EQUATION
#define GL_BLEND_EQUATION_ALPHA_EXT       0x883D
#endif

#ifndef GL_MESA_pack_invert
#define GL_PACK_INVERT_MESA               0x8758
#endif

#ifndef GL_MESA_ycbcr_texture
#define GL_UNSIGNED_SHORT_8_8_MESA        0x85BA
#define GL_UNSIGNED_SHORT_8_8_REV_MESA    0x85BB
#define GL_YCBCR_MESA                     0x8757
#endif


/*************************************************************/

#include <stddef.h>
#ifndef GL_VERSION_1_5
/* GL types for handling large vertex buffer objects */
typedef ptrdiff_t glintptr;
typedef ptrdiff_t glsizeiptr;
#endif

#ifndef GL_ARB_vertex_buffer_object
/* GL types for handling large vertex buffer objects */
typedef ptrdiff_t glintptr_arb;
typedef ptrdiff_t glsizeiptr_arb;
#endif

#ifndef GL_ARB_shader_objects
/* GL types for handling shader object handles and characters */
typedef char glchar_arb;     /* native character */
typedef unsigned int glhandle_arb;   /* shader object handle */
#endif

#ifndef GL_NV_half_float
/* GL type for representing NVIDIA "half" floating point type in host memory */
typedef unsigned short glhalf_nv;
#endif

#ifndef GL_VERSION_1_2
#define GL_VERSION_1_2 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_blend_color (glclampf, glclampf, glclampf, glclampf);
GLAPI void APIENTRY gl_blend_equation (glenum);
GLAPI void APIENTRY gl_draw_range_elements (glenum, gluint, gluint, glsizei, glenum, const glvoid *);
GLAPI void APIENTRY gl_color_table (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_color_table_parameterfv (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_color_table_parameteriv (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_copy_color_table (glenum, glenum, glint, glint, glsizei);
GLAPI void APIENTRY gl_get_color_table (glenum, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_color_table_parameterfv (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_color_table_parameteriv (glenum, glenum, glint *);
GLAPI void APIENTRY gl_color_sub_table (glenum, glsizei, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_copy_color_sub_table (glenum, glsizei, glint, glint, glsizei);
GLAPI void APIENTRY gl_convolution_filter_1d (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_convolution_filter_2d (glenum, glenum, glsizei, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_convolution_parameterf (glenum, glenum, glfloat);
GLAPI void APIENTRY gl_convolution_parameterfv (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_convolution_parameteri (glenum, glenum, glint);
GLAPI void APIENTRY gl_convolution_parameteriv (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_copy_convolution_filter_1d (glenum, glenum, glint, glint, glsizei);
GLAPI void APIENTRY gl_copy_convolution_filter_2d (glenum, glenum, glint, glint, glsizei, glsizei);
GLAPI void APIENTRY gl_get_convolution_filter (glenum, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_convolution_parameterfv (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_convolution_parameteriv (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_separable_filter (glenum, glenum, glenum, glvoid *, glvoid *, glvoid *);
GLAPI void APIENTRY gl_separable_filter_2d (glenum, glenum, glsizei, glsizei, glenum, glenum, const glvoid *, const glvoid *);
GLAPI void APIENTRY gl_get_histogram (glenum, glboolean, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_histogram_parameterfv (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_histogram_parameteriv (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_minmax (glenum, glboolean, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_minmax_parameterfv (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_minmax_parameteriv (glenum, glenum, glint *);
GLAPI void APIENTRY gl_histogram (glenum, glsizei, glenum, glboolean);
GLAPI void APIENTRY gl_minmax (glenum, glenum, glboolean);
GLAPI void APIENTRY gl_reset_histogram (glenum);
GLAPI void APIENTRY gl_reset_minmax (glenum);
GLAPI void APIENTRY gl_tex_image_3d (glenum, glint, glint, glsizei, glsizei, glsizei, glint, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_tex_sub_image_3d (glenum, glint, glint, glint, glint, glsizei, glsizei, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_copy_tex_sub_image_3d (glenum, glint, glint, glint, glint, glint, glint, glsizei, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBLENDCOLORPROC) (glclampf red, glclampf green, glclampf blue, glclampf alpha);
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC) (glenum mode);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC) (glenum mode, gluint start, gluint end, glsizei count, glenum type, const glvoid *indices);
typedef void (APIENTRYP PFNGLCOLORTABLEPROC) (glenum target, glenum internalformat, glsizei width, glenum format, glenum type, const glvoid *table);
typedef void (APIENTRYP PFNGLCOLORTABLEPARAMETERFVPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLCOLORTABLEPARAMETERIVPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLCOPYCOLORTABLEPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width);
typedef void (APIENTRYP PFNGLGETCOLORTABLEPROC) (glenum target, glenum format, glenum type, glvoid *table);
typedef void (APIENTRYP PFNGLGETCOLORTABLEPARAMETERFVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETCOLORTABLEPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLCOLORSUBTABLEPROC) (glenum target, glsizei start, glsizei count, glenum format, glenum type, const glvoid *data);
typedef void (APIENTRYP PFNGLCOPYCOLORSUBTABLEPROC) (glenum target, glsizei start, glint x, glint y, glsizei width);
typedef void (APIENTRYP PFNGLCONVOLUTIONFILTER1DPROC) (glenum target, glenum internalformat, glsizei width, glenum format, glenum type, const glvoid *image);
typedef void (APIENTRYP PFNGLCONVOLUTIONFILTER2DPROC) (glenum target, glenum internalformat, glsizei width, glsizei height, glenum format, glenum type, const glvoid *image);
typedef void (APIENTRYP PFNGLCONVOLUTIONPARAMETERFPROC) (glenum target, glenum pname, glfloat params);
typedef void (APIENTRYP PFNGLCONVOLUTIONPARAMETERFVPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLCONVOLUTIONPARAMETERIPROC) (glenum target, glenum pname, glint params);
typedef void (APIENTRYP PFNGLCONVOLUTIONPARAMETERIVPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLCOPYCONVOLUTIONFILTER1DPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width);
typedef void (APIENTRYP PFNGLCOPYCONVOLUTIONFILTER2DPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width, glsizei height);
typedef void (APIENTRYP PFNGLGETCONVOLUTIONFILTERPROC) (glenum target, glenum format, glenum type, glvoid *image);
typedef void (APIENTRYP PFNGLGETCONVOLUTIONPARAMETERFVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETCONVOLUTIONPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETSEPARABLEFILTERPROC) (glenum target, glenum format, glenum type, glvoid *row, glvoid *column, glvoid *span);
typedef void (APIENTRYP PFNGLSEPARABLEFILTER2DPROC) (glenum target, glenum internalformat, glsizei width, glsizei height, glenum format, glenum type, const glvoid *row, const glvoid *column);
typedef void (APIENTRYP PFNGLGETHISTOGRAMPROC) (glenum target, glboolean reset, glenum format, glenum type, glvoid *values);
typedef void (APIENTRYP PFNGLGETHISTOGRAMPARAMETERFVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETHISTOGRAMPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETMINMAXPROC) (glenum target, glboolean reset, glenum format, glenum type, glvoid *values);
typedef void (APIENTRYP PFNGLGETMINMAXPARAMETERFVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETMINMAXPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLHISTOGRAMPROC) (glenum target, glsizei width, glenum internalformat, glboolean sink);
typedef void (APIENTRYP PFNGLMINMAXPROC) (glenum target, glenum internalformat, glboolean sink);
typedef void (APIENTRYP PFNGLRESETHISTOGRAMPROC) (glenum target);
typedef void (APIENTRYP PFNGLRESETMINMAXPROC) (glenum target);
typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC) (glenum target, glint level, glint internalformat, glsizei width, glsizei height, glsizei depth, glint border, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glsizei width, glsizei height, glsizei depth, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glint x, glint y, glsizei width, glsizei height);
#endif

#ifndef GL_VERSION_1_3
#define GL_VERSION_1_3 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_active_texture (glenum);
GLAPI void APIENTRY gl_client_active_texture (glenum);
GLAPI void APIENTRY gl_multi_tex_coord_1d (glenum, gldouble);
GLAPI void APIENTRY gl_multi_tex_coord_1dv (glenum, const gldouble *);
GLAPI void APIENTRY gl_multi_tex_coord_1f (glenum, glfloat);
GLAPI void APIENTRY gl_multi_tex_coord_1fv (glenum, const glfloat *);
GLAPI void APIENTRY gl_multi_tex_coord_1i (glenum, glint);
GLAPI void APIENTRY gl_multi_tex_coord_1iv (glenum, const glint *);
GLAPI void APIENTRY gl_multi_tex_coord_1s (glenum, glshort);
GLAPI void APIENTRY gl_multi_tex_coord_1sv (glenum, const glshort *);
GLAPI void APIENTRY gl_multi_tex_coord_2d (glenum, gldouble, gldouble);
GLAPI void APIENTRY gl_multi_tex_coord_2dv (glenum, const gldouble *);
GLAPI void APIENTRY gl_multi_tex_coord_2f (glenum, glfloat, glfloat);
GLAPI void APIENTRY gl_multi_tex_coord_2fv (glenum, const glfloat *);
GLAPI void APIENTRY gl_multi_tex_coord_2i (glenum, glint, glint);
GLAPI void APIENTRY gl_multi_tex_coord_2iv (glenum, const glint *);
GLAPI void APIENTRY gl_multi_tex_coord_2s (glenum, glshort, glshort);
GLAPI void APIENTRY gl_multi_tex_coord_2sv (glenum, const glshort *);
GLAPI void APIENTRY gl_multi_tex_coord_3d (glenum, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_multi_tex_coord_3dv (glenum, const gldouble *);
GLAPI void APIENTRY gl_multi_tex_coord_3f (glenum, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_multi_tex_coord_3fv (glenum, const glfloat *);
GLAPI void APIENTRY gl_multi_tex_coord_3i (glenum, glint, glint, glint);
GLAPI void APIENTRY gl_multi_tex_coord_3iv (glenum, const glint *);
GLAPI void APIENTRY gl_multi_tex_coord_3s (glenum, glshort, glshort, glshort);
GLAPI void APIENTRY gl_multi_tex_coord_3sv (glenum, const glshort *);
GLAPI void APIENTRY gl_multi_tex_coord_4d (glenum, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_multi_tex_coord_4dv (glenum, const gldouble *);
GLAPI void APIENTRY gl_multi_tex_coord_4f (glenum, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_multi_tex_coord_4fv (glenum, const glfloat *);
GLAPI void APIENTRY gl_multi_tex_coord_4i (glenum, glint, glint, glint, glint);
GLAPI void APIENTRY gl_multi_tex_coord_4iv (glenum, const glint *);
GLAPI void APIENTRY gl_multi_tex_coord_4s (glenum, glshort, glshort, glshort, glshort);
GLAPI void APIENTRY gl_multi_tex_coord_4sv (glenum, const glshort *);
GLAPI void APIENTRY gl_load_transpose_matrixf (const glfloat *);
GLAPI void APIENTRY gl_load_transpose_matrixd (const gldouble *);
GLAPI void APIENTRY gl_mult_transpose_matrixf (const glfloat *);
GLAPI void APIENTRY gl_mult_transpose_matrixd (const gldouble *);
GLAPI void APIENTRY gl_sample_coverage (glclampf, glboolean);
GLAPI void APIENTRY gl_compressed_tex_image_3d (glenum, glint, glenum, glsizei, glsizei, glsizei, glint, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_image_2d (glenum, glint, glenum, glsizei, glsizei, glint, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_image_1d (glenum, glint, glenum, glsizei, glint, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_sub_image_3d (glenum, glint, glint, glint, glint, glsizei, glsizei, glsizei, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_sub_image_2d (glenum, glint, glint, glint, glsizei, glsizei, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_sub_image_1d (glenum, glint, glint, glsizei, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_get_compressed_tex_image (glenum, glint, glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC) (glenum texture);
typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREPROC) (glenum texture);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DPROC) (glenum target, gldouble s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DVPROC) (glenum target, const gldouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FPROC) (glenum target, glfloat s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FVPROC) (glenum target, const glfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1IPROC) (glenum target, glint s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1IVPROC) (glenum target, const glint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1SPROC) (glenum target, glshort s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1SVPROC) (glenum target, const glshort *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DPROC) (glenum target, gldouble s, gldouble t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DVPROC) (glenum target, const gldouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FPROC) (glenum target, glfloat s, glfloat t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FVPROC) (glenum target, const glfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2IPROC) (glenum target, glint s, glint t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2IVPROC) (glenum target, const glint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2SPROC) (glenum target, glshort s, glshort t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2SVPROC) (glenum target, const glshort *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DPROC) (glenum target, gldouble s, gldouble t, gldouble r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DVPROC) (glenum target, const gldouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FPROC) (glenum target, glfloat s, glfloat t, glfloat r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FVPROC) (glenum target, const glfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3IPROC) (glenum target, glint s, glint t, glint r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3IVPROC) (glenum target, const glint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3SPROC) (glenum target, glshort s, glshort t, glshort r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3SVPROC) (glenum target, const glshort *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DPROC) (glenum target, gldouble s, gldouble t, gldouble r, gldouble q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DVPROC) (glenum target, const gldouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FPROC) (glenum target, glfloat s, glfloat t, glfloat r, glfloat q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FVPROC) (glenum target, const glfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4IPROC) (glenum target, glint s, glint t, glint r, glint q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4IVPROC) (glenum target, const glint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4SPROC) (glenum target, glshort s, glshort t, glshort r, glshort q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4SVPROC) (glenum target, const glshort *v);
typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXFPROC) (const glfloat *m);
typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXDPROC) (const gldouble *m);
typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXFPROC) (const glfloat *m);
typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXDPROC) (const gldouble *m);
typedef void (APIENTRYP PFNGLSAMPLECOVERAGEPROC) (glclampf value, glboolean invert);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glsizei depth, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC) (glenum target, glint level, glenum internalformat, glsizei width, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glsizei width, glsizei height, glsizei depth, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (glenum target, glint level, glint xoffset, glint yoffset, glsizei width, glsizei height, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (glenum target, glint level, glint xoffset, glsizei width, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC) (glenum target, glint level, glvoid *img);
#endif

#ifndef GL_VERSION_1_4
#define GL_VERSION_1_4 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_blend_func_separate (glenum, glenum, glenum, glenum);
GLAPI void APIENTRY gl_fog_coordf (glfloat);
GLAPI void APIENTRY gl_fog_coordfv (const glfloat *);
GLAPI void APIENTRY gl_fog_coordd (gldouble);
GLAPI void APIENTRY gl_fog_coorddv (const gldouble *);
GLAPI void APIENTRY gl_fog_coord_pointer (glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_multi_draw_arrays (glenum, glint *, glsizei *, glsizei);
GLAPI void APIENTRY gl_multi_draw_elements (glenum, const glsizei *, glenum, const glvoid* *, glsizei);
GLAPI void APIENTRY gl_point_parameterf (glenum, glfloat);
GLAPI void APIENTRY gl_point_parameterfv (glenum, const glfloat *);
GLAPI void APIENTRY gl_point_parameteri (glenum, glint);
GLAPI void APIENTRY gl_point_parameteriv (glenum, const glint *);
GLAPI void APIENTRY gl_secondary_color_3b (glbyte, glbyte, glbyte);
GLAPI void APIENTRY gl_secondary_color_3bv (const glbyte *);
GLAPI void APIENTRY gl_secondary_color_3d (gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_secondary_color_3dv (const gldouble *);
GLAPI void APIENTRY gl_secondary_color_3f (glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_secondary_color_3fv (const glfloat *);
GLAPI void APIENTRY gl_secondary_color_3i (glint, glint, glint);
GLAPI void APIENTRY gl_secondary_color_3iv (const glint *);
GLAPI void APIENTRY gl_secondary_color_3s (glshort, glshort, glshort);
GLAPI void APIENTRY gl_secondary_color_3sv (const glshort *);
GLAPI void APIENTRY gl_secondary_color_3ub (glubyte, glubyte, glubyte);
GLAPI void APIENTRY gl_secondary_color_3ubv (const glubyte *);
GLAPI void APIENTRY gl_secondary_color_3ui (gluint, gluint, gluint);
GLAPI void APIENTRY gl_secondary_color_3uiv (const gluint *);
GLAPI void APIENTRY gl_secondary_color_3us (glushort, glushort, glushort);
GLAPI void APIENTRY gl_secondary_color_3usv (const glushort *);
GLAPI void APIENTRY gl_secondary_color_pointer (glint, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_window_pos_2d (gldouble, gldouble);
GLAPI void APIENTRY gl_window_pos_2dv (const gldouble *);
GLAPI void APIENTRY gl_window_pos_2f (glfloat, glfloat);
GLAPI void APIENTRY gl_window_pos_2fv (const glfloat *);
GLAPI void APIENTRY gl_window_pos_2i (glint, glint);
GLAPI void APIENTRY gl_window_pos_2iv (const glint *);
GLAPI void APIENTRY gl_window_pos_2s (glshort, glshort);
GLAPI void APIENTRY gl_window_pos_2sv (const glshort *);
GLAPI void APIENTRY gl_window_pos_3d (gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_window_pos_3dv (const gldouble *);
GLAPI void APIENTRY gl_window_pos_3f (glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_window_pos_3fv (const glfloat *);
GLAPI void APIENTRY gl_window_pos_3i (glint, glint, glint);
GLAPI void APIENTRY gl_window_pos_3iv (const glint *);
GLAPI void APIENTRY gl_window_pos_3s (glshort, glshort, glshort);
GLAPI void APIENTRY gl_window_pos_3sv (const glshort *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC) (glenum sfactor_rgb, glenum dfactor_rgb, glenum sfactor_alpha, glenum dfactor_alpha);
typedef void (APIENTRYP PFNGLFOGCOORDFPROC) (glfloat coord);
typedef void (APIENTRYP PFNGLFOGCOORDFVPROC) (const glfloat *coord);
typedef void (APIENTRYP PFNGLFOGCOORDDPROC) (gldouble coord);
typedef void (APIENTRYP PFNGLFOGCOORDDVPROC) (const gldouble *coord);
typedef void (APIENTRYP PFNGLFOGCOORDPOINTERPROC) (glenum type, glsizei stride, const glvoid *pointer);
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSPROC) (glenum mode, glint *first, glsizei *count, glsizei primcount);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSPROC) (glenum mode, const glsizei *count, glenum type, const glvoid* *indices, glsizei primcount);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFPROC) (glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIPROC) (glenum pname, glint param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIVPROC) (glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BPROC) (glbyte red, glbyte green, glbyte blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BVPROC) (const glbyte *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DPROC) (gldouble red, gldouble green, gldouble blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DVPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FPROC) (glfloat red, glfloat green, glfloat blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FVPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IPROC) (glint red, glint green, glint blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IVPROC) (const glint *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SPROC) (glshort red, glshort green, glshort blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SVPROC) (const glshort *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBPROC) (glubyte red, glubyte green, glubyte blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBVPROC) (const glubyte *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIPROC) (gluint red, gluint green, gluint blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIVPROC) (const gluint *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USPROC) (glushort red, glushort green, glushort blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USVPROC) (const glushort *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLORPOINTERPROC) (glint size, glenum type, glsizei stride, const glvoid *pointer);
typedef void (APIENTRYP PFNGLWINDOWPOS2DPROC) (gldouble x, gldouble y);
typedef void (APIENTRYP PFNGLWINDOWPOS2DVPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2FPROC) (glfloat x, glfloat y);
typedef void (APIENTRYP PFNGLWINDOWPOS2FVPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2IPROC) (glint x, glint y);
typedef void (APIENTRYP PFNGLWINDOWPOS2IVPROC) (const glint *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2SPROC) (glshort x, glshort y);
typedef void (APIENTRYP PFNGLWINDOWPOS2SVPROC) (const glshort *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3DPROC) (gldouble x, gldouble y, gldouble z);
typedef void (APIENTRYP PFNGLWINDOWPOS3DVPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3FPROC) (glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLWINDOWPOS3FVPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3IPROC) (glint x, glint y, glint z);
typedef void (APIENTRYP PFNGLWINDOWPOS3IVPROC) (const glint *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3SPROC) (glshort x, glshort y, glshort z);
typedef void (APIENTRYP PFNGLWINDOWPOS3SVPROC) (const glshort *v);
#endif

#ifndef GL_VERSION_1_5
#define GL_VERSION_1_5 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_gen_queries (glsizei, gluint *);
GLAPI void APIENTRY gl_delete_queries (glsizei, const gluint *);
GLAPI glboolean APIENTRY gl_is_query (gluint);
GLAPI void APIENTRY gl_begin_query (glenum, gluint);
GLAPI void APIENTRY gl_end_query (glenum);
GLAPI void APIENTRY gl_get_queryiv (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_query_objectiv (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_query_objectuiv (gluint, glenum, gluint *);
GLAPI void APIENTRY gl_bind_buffer (glenum, gluint);
GLAPI void APIENTRY gl_delete_buffers (glsizei, const gluint *);
GLAPI void APIENTRY gl_gen_buffers (glsizei, gluint *);
GLAPI glboolean APIENTRY gl_is_buffer (gluint);
GLAPI void APIENTRY gl_buffer_data (glenum, glsizeiptr, const glvoid *, glenum);
GLAPI void APIENTRY gl_buffer_sub_data (glenum, glintptr, glsizeiptr, const glvoid *);
GLAPI void APIENTRY gl_get_buffer_sub_data (glenum, glintptr, glsizeiptr, glvoid *);
GLAPI glvoid* APIENTRY gl_map_buffer (glenum, glenum);
GLAPI glboolean APIENTRY gl_unmap_buffer (glenum);
GLAPI void APIENTRY gl_get_buffer_parameteriv (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_buffer_pointerv (glenum, glenum, glvoid* *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLGENQUERIESPROC) (glsizei n, gluint *ids);
typedef void (APIENTRYP PFNGLDELETEQUERIESPROC) (glsizei n, const gluint *ids);
typedef glboolean (APIENTRYP PFNGLISQUERYPROC) (gluint id);
typedef void (APIENTRYP PFNGLBEGINQUERYPROC) (glenum target, gluint id);
typedef void (APIENTRYP PFNGLENDQUERYPROC) (glenum target);
typedef void (APIENTRYP PFNGLGETQUERYIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVPROC) (gluint id, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVPROC) (gluint id, glenum pname, gluint *params);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (glenum target, gluint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC) (glsizei n, const gluint *buffers);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (glsizei n, gluint *buffers);
typedef glboolean (APIENTRYP PFNGLISBUFFERPROC) (gluint buffer);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (glenum target, glsizeiptr size, const glvoid *data, glenum usage);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC) (glenum target, glintptr offset, glsizeiptr size, const glvoid *data);
typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAPROC) (glenum target, glintptr offset, glsizeiptr size, glvoid *data);
typedef glvoid* (APIENTRYP PFNGLMAPBUFFERPROC) (glenum target, glenum access);
typedef glboolean (APIENTRYP PFNGLUNMAPBUFFERPROC) (glenum target);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVPROC) (glenum target, glenum pname, glvoid* *params);
#endif

#ifndef GL_ARB_multitexture
#define GL_ARB_multitexture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_active_texture_arb (glenum);
GLAPI void APIENTRY gl_client_active_texture_arb (glenum);
GLAPI void APIENTRY gl_multi_tex_coord_1d_arb (glenum, gldouble);
GLAPI void APIENTRY gl_multi_tex_coord_1dv_arb (glenum, const gldouble *);
GLAPI void APIENTRY gl_multi_tex_coord_1f_arb (glenum, glfloat);
GLAPI void APIENTRY gl_multi_tex_coord_1fv_arb (glenum, const glfloat *);
GLAPI void APIENTRY gl_multi_tex_coord_1i_arb (glenum, glint);
GLAPI void APIENTRY gl_multi_tex_coord_1iv_arb (glenum, const glint *);
GLAPI void APIENTRY gl_multi_tex_coord_1s_arb (glenum, glshort);
GLAPI void APIENTRY gl_multi_tex_coord_1sv_arb (glenum, const glshort *);
GLAPI void APIENTRY gl_multi_tex_coord_2d_arb (glenum, gldouble, gldouble);
GLAPI void APIENTRY gl_multi_tex_coord_2dv_arb (glenum, const gldouble *);
GLAPI void APIENTRY gl_multi_tex_coord_2f_arb (glenum, glfloat, glfloat);
GLAPI void APIENTRY gl_multi_tex_coord_2fv_arb (glenum, const glfloat *);
GLAPI void APIENTRY gl_multi_tex_coord_2i_arb (glenum, glint, glint);
GLAPI void APIENTRY gl_multi_tex_coord_2iv_arb (glenum, const glint *);
GLAPI void APIENTRY gl_multi_tex_coord_2s_arb (glenum, glshort, glshort);
GLAPI void APIENTRY gl_multi_tex_coord_2sv_arb (glenum, const glshort *);
GLAPI void APIENTRY gl_multi_tex_coord_3d_arb (glenum, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_multi_tex_coord_3dv_arb (glenum, const gldouble *);
GLAPI void APIENTRY gl_multi_tex_coord_3f_arb (glenum, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_multi_tex_coord_3fv_arb (glenum, const glfloat *);
GLAPI void APIENTRY gl_multi_tex_coord_3i_arb (glenum, glint, glint, glint);
GLAPI void APIENTRY gl_multi_tex_coord_3iv_arb (glenum, const glint *);
GLAPI void APIENTRY gl_multi_tex_coord_3s_arb (glenum, glshort, glshort, glshort);
GLAPI void APIENTRY gl_multi_tex_coord_3sv_arb (glenum, const glshort *);
GLAPI void APIENTRY gl_multi_tex_coord_4d_arb (glenum, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_multi_tex_coord_4dv_arb (glenum, const gldouble *);
GLAPI void APIENTRY gl_multi_tex_coord_4f_arb (glenum, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_multi_tex_coord_4fv_arb (glenum, const glfloat *);
GLAPI void APIENTRY gl_multi_tex_coord_4i_arb (glenum, glint, glint, glint, glint);
GLAPI void APIENTRY gl_multi_tex_coord_4iv_arb (glenum, const glint *);
GLAPI void APIENTRY gl_multi_tex_coord_4s_arb (glenum, glshort, glshort, glshort, glshort);
GLAPI void APIENTRY gl_multi_tex_coord_4sv_arb (glenum, const glshort *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLACTIVETEXTUREARBPROC) (glenum texture);
typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREARBPROC) (glenum texture);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DARBPROC) (glenum target, gldouble s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1DVARBPROC) (glenum target, const gldouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FARBPROC) (glenum target, glfloat s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1FVARBPROC) (glenum target, const glfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1IARBPROC) (glenum target, glint s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1IVARBPROC) (glenum target, const glint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1SARBPROC) (glenum target, glshort s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1SVARBPROC) (glenum target, const glshort *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DARBPROC) (glenum target, gldouble s, gldouble t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2DVARBPROC) (glenum target, const gldouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FARBPROC) (glenum target, glfloat s, glfloat t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FVARBPROC) (glenum target, const glfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2IARBPROC) (glenum target, glint s, glint t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2IVARBPROC) (glenum target, const glint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2SARBPROC) (glenum target, glshort s, glshort t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2SVARBPROC) (glenum target, const glshort *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DARBPROC) (glenum target, gldouble s, gldouble t, gldouble r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3DVARBPROC) (glenum target, const gldouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FARBPROC) (glenum target, glfloat s, glfloat t, glfloat r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3FVARBPROC) (glenum target, const glfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3IARBPROC) (glenum target, glint s, glint t, glint r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3IVARBPROC) (glenum target, const glint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3SARBPROC) (glenum target, glshort s, glshort t, glshort r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3SVARBPROC) (glenum target, const glshort *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DARBPROC) (glenum target, gldouble s, gldouble t, gldouble r, gldouble q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4DVARBPROC) (glenum target, const gldouble *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FARBPROC) (glenum target, glfloat s, glfloat t, glfloat r, glfloat q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FVARBPROC) (glenum target, const glfloat *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4IARBPROC) (glenum target, glint s, glint t, glint r, glint q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4IVARBPROC) (glenum target, const glint *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4SARBPROC) (glenum target, glshort s, glshort t, glshort r, glshort q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4SVARBPROC) (glenum target, const glshort *v);
#endif

#ifndef GL_ARB_transpose_matrix
#define GL_ARB_transpose_matrix 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_load_transpose_matrixf_arb (const glfloat *);
GLAPI void APIENTRY gl_load_transpose_matrixd_arb (const gldouble *);
GLAPI void APIENTRY gl_mult_transpose_matrixf_arb (const glfloat *);
GLAPI void APIENTRY gl_mult_transpose_matrixd_arb (const gldouble *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXFARBPROC) (const glfloat *m);
typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXDARBPROC) (const gldouble *m);
typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXFARBPROC) (const glfloat *m);
typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXDARBPROC) (const gldouble *m);
#endif

#ifndef GL_ARB_multisample
#define GL_ARB_multisample 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_sample_coverage_arb (glclampf, glboolean);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLSAMPLECOVERAGEARBPROC) (glclampf value, glboolean invert);
#endif

#ifndef GL_ARB_texture_env_add
#define GL_ARB_texture_env_add 1
#endif

#ifndef GL_ARB_texture_cube_map
#define GL_ARB_texture_cube_map 1
#endif

#ifndef GL_ARB_texture_compression
#define GL_ARB_texture_compression 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_compressed_tex_image_3darb (glenum, glint, glenum, glsizei, glsizei, glsizei, glint, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_image_2darb (glenum, glint, glenum, glsizei, glsizei, glint, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_image_1darb (glenum, glint, glenum, glsizei, glint, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_sub_image_3darb (glenum, glint, glint, glint, glint, glsizei, glsizei, glsizei, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_sub_image_2darb (glenum, glint, glint, glint, glsizei, glsizei, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_compressed_tex_sub_image_1darb (glenum, glint, glint, glsizei, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_get_compressed_tex_image_arb (glenum, glint, glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DARBPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glsizei depth, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DARBPROC) (glenum target, glint level, glenum internalformat, glsizei width, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glsizei width, glsizei height, glsizei depth, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) (glenum target, glint level, glint xoffset, glint yoffset, glsizei width, glsizei height, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC) (glenum target, glint level, glint xoffset, glsizei width, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) (glenum target, glint level, glvoid *img);
#endif

#ifndef GL_ARB_texture_border_clamp
#define GL_ARB_texture_border_clamp 1
#endif

#ifndef GL_ARB_point_parameters
#define GL_ARB_point_parameters 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_point_parameterf_arb (glenum, glfloat);
GLAPI void APIENTRY gl_point_parameterfv_arb (glenum, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPOINTPARAMETERFARBPROC) (glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVARBPROC) (glenum pname, const glfloat *params);
#endif

#ifndef GL_ARB_vertex_blend
#define GL_ARB_vertex_blend 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_weightbv_arb (glint, const glbyte *);
GLAPI void APIENTRY gl_weightsv_arb (glint, const glshort *);
GLAPI void APIENTRY gl_weightiv_arb (glint, const glint *);
GLAPI void APIENTRY gl_weightfv_arb (glint, const glfloat *);
GLAPI void APIENTRY gl_weightdv_arb (glint, const gldouble *);
GLAPI void APIENTRY gl_weightubv_arb (glint, const glubyte *);
GLAPI void APIENTRY gl_weightusv_arb (glint, const glushort *);
GLAPI void APIENTRY gl_weightuiv_arb (glint, const gluint *);
GLAPI void APIENTRY gl_weight_pointer_arb (glint, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_vertex_blend_arb (glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLWEIGHTBVARBPROC) (glint size, const glbyte *weights);
typedef void (APIENTRYP PFNGLWEIGHTSVARBPROC) (glint size, const glshort *weights);
typedef void (APIENTRYP PFNGLWEIGHTIVARBPROC) (glint size, const glint *weights);
typedef void (APIENTRYP PFNGLWEIGHTFVARBPROC) (glint size, const glfloat *weights);
typedef void (APIENTRYP PFNGLWEIGHTDVARBPROC) (glint size, const gldouble *weights);
typedef void (APIENTRYP PFNGLWEIGHTUBVARBPROC) (glint size, const glubyte *weights);
typedef void (APIENTRYP PFNGLWEIGHTUSVARBPROC) (glint size, const glushort *weights);
typedef void (APIENTRYP PFNGLWEIGHTUIVARBPROC) (glint size, const gluint *weights);
typedef void (APIENTRYP PFNGLWEIGHTPOINTERARBPROC) (glint size, glenum type, glsizei stride, const glvoid *pointer);
typedef void (APIENTRYP PFNGLVERTEXBLENDARBPROC) (glint count);
#endif

#ifndef GL_ARB_matrix_palette
#define GL_ARB_matrix_palette 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_current_palette_matrix_arb (glint);
GLAPI void APIENTRY gl_matrix_indexubv_arb (glint, const glubyte *);
GLAPI void APIENTRY gl_matrix_indexusv_arb (glint, const glushort *);
GLAPI void APIENTRY gl_matrix_indexuiv_arb (glint, const gluint *);
GLAPI void APIENTRY gl_matrix_index_pointer_arb (glint, glenum, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCURRENTPALETTEMATRIXARBPROC) (glint index);
typedef void (APIENTRYP PFNGLMATRIXINDEXUBVARBPROC) (glint size, const glubyte *indices);
typedef void (APIENTRYP PFNGLMATRIXINDEXUSVARBPROC) (glint size, const glushort *indices);
typedef void (APIENTRYP PFNGLMATRIXINDEXUIVARBPROC) (glint size, const gluint *indices);
typedef void (APIENTRYP PFNGLMATRIXINDEXPOINTERARBPROC) (glint size, glenum type, glsizei stride, const glvoid *pointer);
#endif

#ifndef GL_ARB_texture_env_combine
#define GL_ARB_texture_env_combine 1
#endif

#ifndef GL_ARB_texture_env_crossbar
#define GL_ARB_texture_env_crossbar 1
#endif

#ifndef GL_ARB_texture_env_dot3
#define GL_ARB_texture_env_dot3 1
#endif

#ifndef GL_ARB_texture_mirror_repeat
#define GL_ARB_texture_mirror_repeat 1
#endif

#ifndef GL_ARB_depth_texture
#define GL_ARB_depth_texture 1
#endif

#ifndef GL_ARB_shadow
#define GL_ARB_shadow 1
#endif

#ifndef GL_ARB_shadow_ambient
#define GL_ARB_shadow_ambient 1
#endif

#ifndef GL_ARB_window_pos
#define GL_ARB_window_pos 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_window_pos_2d_arb (gldouble, gldouble);
GLAPI void APIENTRY gl_window_pos_2dv_arb (const gldouble *);
GLAPI void APIENTRY gl_window_pos_2f_arb (glfloat, glfloat);
GLAPI void APIENTRY gl_window_pos_2fv_arb (const glfloat *);
GLAPI void APIENTRY gl_window_pos_2i_arb (glint, glint);
GLAPI void APIENTRY gl_window_pos_2iv_arb (const glint *);
GLAPI void APIENTRY gl_window_pos_2s_arb (glshort, glshort);
GLAPI void APIENTRY gl_window_pos_2sv_arb (const glshort *);
GLAPI void APIENTRY gl_window_pos_3d_arb (gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_window_pos_3dv_arb (const gldouble *);
GLAPI void APIENTRY gl_window_pos_3f_arb (glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_window_pos_3fv_arb (const glfloat *);
GLAPI void APIENTRY gl_window_pos_3i_arb (glint, glint, glint);
GLAPI void APIENTRY gl_window_pos_3iv_arb (const glint *);
GLAPI void APIENTRY gl_window_pos_3s_arb (glshort, glshort, glshort);
GLAPI void APIENTRY gl_window_pos_3sv_arb (const glshort *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLWINDOWPOS2DARBPROC) (gldouble x, gldouble y);
typedef void (APIENTRYP PFNGLWINDOWPOS2DVARBPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2FARBPROC) (glfloat x, glfloat y);
typedef void (APIENTRYP PFNGLWINDOWPOS2FVARBPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2IARBPROC) (glint x, glint y);
typedef void (APIENTRYP PFNGLWINDOWPOS2IVARBPROC) (const glint *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2SARBPROC) (glshort x, glshort y);
typedef void (APIENTRYP PFNGLWINDOWPOS2SVARBPROC) (const glshort *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3DARBPROC) (gldouble x, gldouble y, gldouble z);
typedef void (APIENTRYP PFNGLWINDOWPOS3DVARBPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3FARBPROC) (glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLWINDOWPOS3FVARBPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3IARBPROC) (glint x, glint y, glint z);
typedef void (APIENTRYP PFNGLWINDOWPOS3IVARBPROC) (const glint *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3SARBPROC) (glshort x, glshort y, glshort z);
typedef void (APIENTRYP PFNGLWINDOWPOS3SVARBPROC) (const glshort *v);
#endif

#ifndef GL_ARB_vertex_program
#define GL_ARB_vertex_program 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_vertex_attrib_1d_arb (gluint, gldouble);
GLAPI void APIENTRY gl_vertex_attrib_1dv_arb (gluint, const gldouble *);
GLAPI void APIENTRY gl_vertex_attrib_1f_arb (gluint, glfloat);
GLAPI void APIENTRY gl_vertex_attrib_1fv_arb (gluint, const glfloat *);
GLAPI void APIENTRY gl_vertex_attrib_1s_arb (gluint, glshort);
GLAPI void APIENTRY gl_vertex_attrib_1sv_arb (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_2d_arb (gluint, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_attrib_2dv_arb (gluint, const gldouble *);
GLAPI void APIENTRY gl_vertex_attrib_2f_arb (gluint, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_attrib_2fv_arb (gluint, const glfloat *);
GLAPI void APIENTRY gl_vertex_attrib_2s_arb (gluint, glshort, glshort);
GLAPI void APIENTRY gl_vertex_attrib_2sv_arb (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_3d_arb (gluint, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_attrib_3dv_arb (gluint, const gldouble *);
GLAPI void APIENTRY gl_vertex_attrib_3f_arb (gluint, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_attrib_3fv_arb (gluint, const glfloat *);
GLAPI void APIENTRY gl_vertex_attrib_3s_arb (gluint, glshort, glshort, glshort);
GLAPI void APIENTRY gl_vertex_attrib_3sv_arb (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_4nbv_arb (gluint, const glbyte *);
GLAPI void APIENTRY gl_vertex_attrib_4niv_arb (gluint, const glint *);
GLAPI void APIENTRY gl_vertex_attrib_4nsv_arb (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_4nub_arb (gluint, glubyte, glubyte, glubyte, glubyte);
GLAPI void APIENTRY gl_vertex_attrib_4nubv_arb (gluint, const glubyte *);
GLAPI void APIENTRY gl_vertex_attrib_4nuiv_arb (gluint, const gluint *);
GLAPI void APIENTRY gl_vertex_attrib_4nusv_arb (gluint, const glushort *);
GLAPI void APIENTRY gl_vertex_attrib_4bv_arb (gluint, const glbyte *);
GLAPI void APIENTRY gl_vertex_attrib_4d_arb (gluint, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_attrib_4dv_arb (gluint, const gldouble *);
GLAPI void APIENTRY gl_vertex_attrib_4f_arb (gluint, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_attrib_4fv_arb (gluint, const glfloat *);
GLAPI void APIENTRY gl_vertex_attrib_4iv_arb (gluint, const glint *);
GLAPI void APIENTRY gl_vertex_attrib_4s_arb (gluint, glshort, glshort, glshort, glshort);
GLAPI void APIENTRY gl_vertex_attrib_4sv_arb (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_4ubv_arb (gluint, const glubyte *);
GLAPI void APIENTRY gl_vertex_attrib_4uiv_arb (gluint, const gluint *);
GLAPI void APIENTRY gl_vertex_attrib_4usv_arb (gluint, const glushort *);
GLAPI void APIENTRY gl_vertex_attrib_pointer_arb (gluint, glint, glenum, glboolean, glsizei, const glvoid *);
GLAPI void APIENTRY gl_enable_vertex_attrib_array_arb (gluint);
GLAPI void APIENTRY gl_disable_vertex_attrib_array_arb (gluint);
GLAPI void APIENTRY gl_program_string_arb (glenum, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_bind_program_arb (glenum, gluint);
GLAPI void APIENTRY gl_delete_programs_arb (glsizei, const gluint *);
GLAPI void APIENTRY gl_gen_programs_arb (glsizei, gluint *);
GLAPI void APIENTRY gl_program_env_parameter_4d_arb (glenum, gluint, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_program_env_parameter_4dv_arb (glenum, gluint, const gldouble *);
GLAPI void APIENTRY gl_program_env_parameter_4f_arb (glenum, gluint, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_program_env_parameter_4fv_arb (glenum, gluint, const glfloat *);
GLAPI void APIENTRY gl_program_local_parameter_4d_arb (glenum, gluint, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_program_local_parameter_4dv_arb (glenum, gluint, const gldouble *);
GLAPI void APIENTRY gl_program_local_parameter_4f_arb (glenum, gluint, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_program_local_parameter_4fv_arb (glenum, gluint, const glfloat *);
GLAPI void APIENTRY gl_get_program_env_parameterdv_arb (glenum, gluint, gldouble *);
GLAPI void APIENTRY gl_get_program_env_parameterfv_arb (glenum, gluint, glfloat *);
GLAPI void APIENTRY gl_get_program_local_parameterdv_arb (glenum, gluint, gldouble *);
GLAPI void APIENTRY gl_get_program_local_parameterfv_arb (glenum, gluint, glfloat *);
GLAPI void APIENTRY gl_get_programiv_arb (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_program_string_arb (glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_vertex_attribdv_arb (gluint, glenum, gldouble *);
GLAPI void APIENTRY gl_get_vertex_attribfv_arb (gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_vertex_attribiv_arb (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_vertex_attrib_pointerv_arb (gluint, glenum, glvoid* *);
GLAPI glboolean APIENTRY gl_is_program_arb (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DARBPROC) (gluint index, gldouble x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVARBPROC) (gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FARBPROC) (gluint index, glfloat x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVARBPROC) (gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SARBPROC) (gluint index, glshort x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVARBPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DARBPROC) (gluint index, gldouble x, gldouble y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVARBPROC) (gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FARBPROC) (gluint index, glfloat x, glfloat y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVARBPROC) (gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SARBPROC) (gluint index, glshort x, glshort y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVARBPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DARBPROC) (gluint index, gldouble x, gldouble y, gldouble z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVARBPROC) (gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FARBPROC) (gluint index, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVARBPROC) (gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SARBPROC) (gluint index, glshort x, glshort y, glshort z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVARBPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NBVARBPROC) (gluint index, const glbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NIVARBPROC) (gluint index, const glint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NSVARBPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBARBPROC) (gluint index, glubyte x, glubyte y, glubyte z, glubyte w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBVARBPROC) (gluint index, const glubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUIVARBPROC) (gluint index, const gluint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUSVARBPROC) (gluint index, const glushort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4BVARBPROC) (gluint index, const glbyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DARBPROC) (gluint index, gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVARBPROC) (gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FARBPROC) (gluint index, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVARBPROC) (gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4IVARBPROC) (gluint index, const glint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SARBPROC) (gluint index, glshort x, glshort y, glshort z, glshort w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVARBPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVARBPROC) (gluint index, const glubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UIVARBPROC) (gluint index, const gluint *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4USVARBPROC) (gluint index, const glushort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERARBPROC) (gluint index, glint size, glenum type, glboolean normalized, glsizei stride, const glvoid *pointer);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYARBPROC) (gluint index);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) (gluint index);
typedef void (APIENTRYP PFNGLPROGRAMSTRINGARBPROC) (glenum target, glenum format, glsizei len, const glvoid *string);
typedef void (APIENTRYP PFNGLBINDPROGRAMARBPROC) (glenum target, gluint program);
typedef void (APIENTRYP PFNGLDELETEPROGRAMSARBPROC) (glsizei n, const gluint *programs);
typedef void (APIENTRYP PFNGLGENPROGRAMSARBPROC) (glsizei n, gluint *programs);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4DARBPROC) (glenum target, gluint index, gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4DVARBPROC) (glenum target, gluint index, const gldouble *params);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4FARBPROC) (glenum target, gluint index, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4FVARBPROC) (glenum target, gluint index, const glfloat *params);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4DARBPROC) (glenum target, gluint index, gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4DVARBPROC) (glenum target, gluint index, const gldouble *params);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FARBPROC) (glenum target, gluint index, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) (glenum target, gluint index, const glfloat *params);
typedef void (APIENTRYP PFNGLGETPROGRAMENVPARAMETERDVARBPROC) (glenum target, gluint index, gldouble *params);
typedef void (APIENTRYP PFNGLGETPROGRAMENVPARAMETERFVARBPROC) (glenum target, gluint index, glfloat *params);
typedef void (APIENTRYP PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) (glenum target, gluint index, gldouble *params);
typedef void (APIENTRYP PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) (glenum target, gluint index, glfloat *params);
typedef void (APIENTRYP PFNGLGETPROGRAMIVARBPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMSTRINGARBPROC) (glenum target, glenum pname, glvoid *string);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVARBPROC) (gluint index, glenum pname, gldouble *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVARBPROC) (gluint index, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVARBPROC) (gluint index, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVARBPROC) (gluint index, glenum pname, glvoid* *pointer);
typedef glboolean (APIENTRYP PFNGLISPROGRAMARBPROC) (gluint program);
#endif

#ifndef GL_ARB_fragment_program
#define GL_ARB_fragment_program 1
/* All ARB_fragment_program entry points are shared with ARB_vertex_program. */
#endif

#ifndef GL_ARB_vertex_buffer_object
#define GL_ARB_vertex_buffer_object 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_bind_buffer_arb (glenum, gluint);
GLAPI void APIENTRY gl_delete_buffers_arb (glsizei, const gluint *);
GLAPI void APIENTRY gl_gen_buffers_arb (glsizei, gluint *);
GLAPI glboolean APIENTRY gl_is_buffer_arb (gluint);
GLAPI void APIENTRY gl_buffer_data_arb (glenum, glsizeiptr_arb, const glvoid *, glenum);
GLAPI void APIENTRY gl_buffer_sub_data_arb (glenum, glintptr_arb, glsizeiptr_arb, const glvoid *);
GLAPI void APIENTRY gl_get_buffer_sub_data_arb (glenum, glintptr_arb, glsizeiptr_arb, glvoid *);
GLAPI glvoid* APIENTRY gl_map_buffer_arb (glenum, glenum);
GLAPI glboolean APIENTRY gl_unmap_buffer_arb (glenum);
GLAPI void APIENTRY gl_get_buffer_parameteriv_arb (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_buffer_pointerv_arb (glenum, glenum, glvoid* *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBINDBUFFERARBPROC) (glenum target, gluint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSARBPROC) (glsizei n, const gluint *buffers);
typedef void (APIENTRYP PFNGLGENBUFFERSARBPROC) (glsizei n, gluint *buffers);
typedef glboolean (APIENTRYP PFNGLISBUFFERARBPROC) (gluint buffer);
typedef void (APIENTRYP PFNGLBUFFERDATAARBPROC) (glenum target, glsizeiptr_arb size, const glvoid *data, glenum usage);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAARBPROC) (glenum target, glintptr_arb offset, glsizeiptr_arb size, const glvoid *data);
typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAARBPROC) (glenum target, glintptr_arb offset, glsizeiptr_arb size, glvoid *data);
typedef glvoid* (APIENTRYP PFNGLMAPBUFFERARBPROC) (glenum target, glenum access);
typedef glboolean (APIENTRYP PFNGLUNMAPBUFFERARBPROC) (glenum target);
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVARBPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVARBPROC) (glenum target, glenum pname, glvoid* *params);
#endif

#ifndef GL_ARB_occlusion_query
#define GL_ARB_occlusion_query 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_gen_queries_arb (glsizei, gluint *);
GLAPI void APIENTRY gl_delete_queries_arb (glsizei, const gluint *);
GLAPI glboolean APIENTRY gl_is_query_arb (gluint);
GLAPI void APIENTRY gl_begin_query_arb (glenum, gluint);
GLAPI void APIENTRY gl_end_query_arb (glenum);
GLAPI void APIENTRY gl_get_queryiv_arb (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_query_objectiv_arb (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_query_objectuiv_arb (gluint, glenum, gluint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLGENQUERIESARBPROC) (glsizei n, gluint *ids);
typedef void (APIENTRYP PFNGLDELETEQUERIESARBPROC) (glsizei n, const gluint *ids);
typedef glboolean (APIENTRYP PFNGLISQUERYARBPROC) (gluint id);
typedef void (APIENTRYP PFNGLBEGINQUERYARBPROC) (glenum target, gluint id);
typedef void (APIENTRYP PFNGLENDQUERYARBPROC) (glenum target);
typedef void (APIENTRYP PFNGLGETQUERYIVARBPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVARBPROC) (gluint id, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVARBPROC) (gluint id, glenum pname, gluint *params);
#endif

#ifndef GL_ARB_shader_objects
#define GL_ARB_shader_objects 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_delete_object_arb (glhandle_arb);
GLAPI glhandle_arb APIENTRY gl_get_handle_arb (glenum);
GLAPI void APIENTRY gl_detach_object_arb (glhandle_arb, glhandle_arb);
GLAPI glhandle_arb APIENTRY gl_create_shader_object_arb (glenum);
GLAPI void APIENTRY gl_shader_source_arb (glhandle_arb, glsizei, const glchar_arb* *, const glint *);
GLAPI void APIENTRY gl_compile_shader_arb (glhandle_arb);
GLAPI glhandle_arb APIENTRY gl_create_program_object_arb (void);
GLAPI void APIENTRY gl_attach_object_arb (glhandle_arb, glhandle_arb);
GLAPI void APIENTRY gl_link_program_arb (glhandle_arb);
GLAPI void APIENTRY gl_use_program_object_arb (glhandle_arb);
GLAPI void APIENTRY gl_validate_program_arb (glhandle_arb);
GLAPI void APIENTRY gl_uniform_1f_arb (glint, glfloat);
GLAPI void APIENTRY gl_uniform_2f_arb (glint, glfloat, glfloat);
GLAPI void APIENTRY gl_uniform_3f_arb (glint, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_uniform_4f_arb (glint, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_uniform_1i_arb (glint, glint);
GLAPI void APIENTRY gl_uniform_2i_arb (glint, glint, glint);
GLAPI void APIENTRY gl_uniform_3i_arb (glint, glint, glint, glint);
GLAPI void APIENTRY gl_uniform_4i_arb (glint, glint, glint, glint, glint);
GLAPI void APIENTRY gl_uniform_1fv_arb (glint, glsizei, const glfloat *);
GLAPI void APIENTRY gl_uniform_2fv_arb (glint, glsizei, const glfloat *);
GLAPI void APIENTRY gl_uniform_3fv_arb (glint, glsizei, const glfloat *);
GLAPI void APIENTRY gl_uniform_4fv_arb (glint, glsizei, const glfloat *);
GLAPI void APIENTRY gl_uniform_1iv_arb (glint, glsizei, const glint *);
GLAPI void APIENTRY gl_uniform_2iv_arb (glint, glsizei, const glint *);
GLAPI void APIENTRY gl_uniform_3iv_arb (glint, glsizei, const glint *);
GLAPI void APIENTRY gl_uniform_4iv_arb (glint, glsizei, const glint *);
GLAPI void APIENTRY gl_uniform_matrix_2fv_arb (glint, glsizei, glboolean, const glfloat *);
GLAPI void APIENTRY gl_uniform_matrix_3fv_arb (glint, glsizei, glboolean, const glfloat *);
GLAPI void APIENTRY gl_uniform_matrix_4fv_arb (glint, glsizei, glboolean, const glfloat *);
GLAPI void APIENTRY gl_get_object_parameterfv_arb (glhandle_arb, glenum, glfloat *);
GLAPI void APIENTRY gl_get_object_parameteriv_arb (glhandle_arb, glenum, glint *);
GLAPI void APIENTRY gl_get_info_log_arb (glhandle_arb, glsizei, glsizei *, glchar_arb *);
GLAPI void APIENTRY gl_get_attached_objects_arb (glhandle_arb, glsizei, glsizei *, glhandle_arb *);
GLAPI glint APIENTRY gl_get_uniform_location_arb (glhandle_arb, const glchar_arb *);
GLAPI void APIENTRY gl_get_active_uniform_arb (glhandle_arb, gluint, glsizei, glsizei *, glint *, glenum *, glchar_arb *);
GLAPI void APIENTRY gl_get_uniformfv_arb (glhandle_arb, glint, glfloat *);
GLAPI void APIENTRY gl_get_uniformiv_arb (glhandle_arb, glint, glint *);
GLAPI void APIENTRY gl_get_shader_source_arb (glhandle_arb, glsizei, glsizei *, glchar_arb *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLDELETEOBJECTARBPROC) (glhandle_arb obj);
typedef glhandle_arb (APIENTRYP PFNGLGETHANDLEARBPROC) (glenum pname);
typedef void (APIENTRYP PFNGLDETACHOBJECTARBPROC) (glhandle_arb container_obj, glhandle_arb attached_obj);
typedef glhandle_arb (APIENTRYP PFNGLCREATESHADEROBJECTARBPROC) (glenum shader_type);
typedef void (APIENTRYP PFNGLSHADERSOURCEARBPROC) (glhandle_arb shader_obj, glsizei count, const glchar_arb* *string, const glint *length);
typedef void (APIENTRYP PFNGLCOMPILESHADERARBPROC) (glhandle_arb shader_obj);
typedef glhandle_arb (APIENTRYP PFNGLCREATEPROGRAMOBJECTARBPROC) (void);
typedef void (APIENTRYP PFNGLATTACHOBJECTARBPROC) (glhandle_arb container_obj, glhandle_arb obj);
typedef void (APIENTRYP PFNGLLINKPROGRAMARBPROC) (glhandle_arb program_obj);
typedef void (APIENTRYP PFNGLUSEPROGRAMOBJECTARBPROC) (glhandle_arb program_obj);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMARBPROC) (glhandle_arb program_obj);
typedef void (APIENTRYP PFNGLUNIFORM1FARBPROC) (glint location, glfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM2FARBPROC) (glint location, glfloat v0, glfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM3FARBPROC) (glint location, glfloat v0, glfloat v1, glfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM4FARBPROC) (glint location, glfloat v0, glfloat v1, glfloat v2, glfloat v3);
typedef void (APIENTRYP PFNGLUNIFORM1IARBPROC) (glint location, glint v0);
typedef void (APIENTRYP PFNGLUNIFORM2IARBPROC) (glint location, glint v0, glint v1);
typedef void (APIENTRYP PFNGLUNIFORM3IARBPROC) (glint location, glint v0, glint v1, glint v2);
typedef void (APIENTRYP PFNGLUNIFORM4IARBPROC) (glint location, glint v0, glint v1, glint v2, glint v3);
typedef void (APIENTRYP PFNGLUNIFORM1FVARBPROC) (glint location, glsizei count, const glfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM2FVARBPROC) (glint location, glsizei count, const glfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM3FVARBPROC) (glint location, glsizei count, const glfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM4FVARBPROC) (glint location, glsizei count, const glfloat *value);
typedef void (APIENTRYP PFNGLUNIFORM1IVARBPROC) (glint location, glsizei count, const glint *value);
typedef void (APIENTRYP PFNGLUNIFORM2IVARBPROC) (glint location, glsizei count, const glint *value);
typedef void (APIENTRYP PFNGLUNIFORM3IVARBPROC) (glint location, glsizei count, const glint *value);
typedef void (APIENTRYP PFNGLUNIFORM4IVARBPROC) (glint location, glsizei count, const glint *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVARBPROC) (glint location, glsizei count, glboolean transpose, const glfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVARBPROC) (glint location, glsizei count, glboolean transpose, const glfloat *value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVARBPROC) (glint location, glsizei count, glboolean transpose, const glfloat *value);
typedef void (APIENTRYP PFNGLGETOBJECTPARAMETERFVARBPROC) (glhandle_arb obj, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETOBJECTPARAMETERIVARBPROC) (glhandle_arb obj, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETINFOLOGARBPROC) (glhandle_arb obj, glsizei max_length, glsizei *length, glchar_arb *info_log);
typedef void (APIENTRYP PFNGLGETATTACHEDOBJECTSARBPROC) (glhandle_arb container_obj, glsizei max_count, glsizei *count, glhandle_arb *obj);
typedef glint (APIENTRYP PFNGLGETUNIFORMLOCATIONARBPROC) (glhandle_arb program_obj, const glchar_arb *name);
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMARBPROC) (glhandle_arb program_obj, gluint index, glsizei max_length, glsizei *length, glint *size, glenum *type, glchar_arb *name);
typedef void (APIENTRYP PFNGLGETUNIFORMFVARBPROC) (glhandle_arb program_obj, glint location, glfloat *params);
typedef void (APIENTRYP PFNGLGETUNIFORMIVARBPROC) (glhandle_arb program_obj, glint location, glint *params);
typedef void (APIENTRYP PFNGLGETSHADERSOURCEARBPROC) (glhandle_arb obj, glsizei max_length, glsizei *length, glchar_arb *source);
#endif

#ifndef GL_ARB_vertex_shader
#define GL_ARB_vertex_shader 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_bind_attrib_location_arb (glhandle_arb, gluint, const glchar_arb *);
GLAPI void APIENTRY gl_get_active_attrib_arb (glhandle_arb, gluint, glsizei, glsizei *, glint *, glenum *, glchar_arb *);
GLAPI glint APIENTRY gl_get_attrib_location_arb (glhandle_arb, const glchar_arb *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONARBPROC) (glhandle_arb program_obj, gluint index, const glchar_arb *name);
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBARBPROC) (glhandle_arb program_obj, gluint index, glsizei max_length, glsizei *length, glint *size, glenum *type, glchar_arb *name);
typedef glint (APIENTRYP PFNGLGETATTRIBLOCATIONARBPROC) (glhandle_arb program_obj, const glchar_arb *name);
#endif

#ifndef GL_ARB_fragment_shader
#define GL_ARB_fragment_shader 1
#endif

#ifndef GL_ARB_shading_language_100
#define GL_ARB_shading_language_100 1
#endif

#ifndef GL_ARB_texture_non_power_of_two
#define GL_ARB_texture_non_power_of_two 1
#endif

#ifndef GL_ARB_point_sprite
#define GL_ARB_point_sprite 1
#endif

#ifndef GL_EXT_abgr
#define GL_EXT_abgr 1
#endif

#ifndef GL_EXT_blend_color
#define GL_EXT_blend_color 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_blend_color_ext (glclampf, glclampf, glclampf, glclampf);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBLENDCOLOREXTPROC) (glclampf red, glclampf green, glclampf blue, glclampf alpha);
#endif

#ifndef GL_EXT_polygon_offset
#define GL_EXT_polygon_offset 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_polygon_offset_ext (glfloat, glfloat);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPOLYGONOFFSETEXTPROC) (glfloat factor, glfloat bias);
#endif

#ifndef GL_EXT_texture
#define GL_EXT_texture 1
#endif

#ifndef GL_EXT_texture3D
#define GL_EXT_texture3D 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_tex_image_3dext (glenum, glint, glenum, glsizei, glsizei, glsizei, glint, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_tex_sub_image_3dext (glenum, glint, glint, glint, glint, glsizei, glsizei, glsizei, glenum, glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTEXIMAGE3DEXTPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glsizei depth, glint border, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DEXTPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glsizei width, glsizei height, glsizei depth, glenum format, glenum type, const glvoid *pixels);
#endif

#ifndef GL_SGIS_texture_filter4
#define GL_SGIS_texture_filter4 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_get_tex_filter_func_sgis (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_tex_filter_func_sgis (glenum, glenum, glsizei, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLGETTEXFILTERFUNCSGISPROC) (glenum target, glenum filter, glfloat *weights);
typedef void (APIENTRYP PFNGLTEXFILTERFUNCSGISPROC) (glenum target, glenum filter, glsizei n, const glfloat *weights);
#endif

#ifndef GL_EXT_subtexture
#define GL_EXT_subtexture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_tex_sub_image_1dext (glenum, glint, glint, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_tex_sub_image_2dext (glenum, glint, glint, glint, glsizei, glsizei, glenum, glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DEXTPROC) (glenum target, glint level, glint xoffset, glsizei width, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DEXTPROC) (glenum target, glint level, glint xoffset, glint yoffset, glsizei width, glsizei height, glenum format, glenum type, const glvoid *pixels);
#endif

#ifndef GL_EXT_copy_texture
#define GL_EXT_copy_texture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_copy_tex_image_1dext (glenum, glint, glenum, glint, glint, glsizei, glint);
GLAPI void APIENTRY gl_copy_tex_image_2dext (glenum, glint, glenum, glint, glint, glsizei, glsizei, glint);
GLAPI void APIENTRY gl_copy_tex_sub_image_1dext (glenum, glint, glint, glint, glint, glsizei);
GLAPI void APIENTRY gl_copy_tex_sub_image_2dext (glenum, glint, glint, glint, glint, glint, glsizei, glsizei);
GLAPI void APIENTRY gl_copy_tex_sub_image_3dext (glenum, glint, glint, glint, glint, glint, glint, glsizei, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DEXTPROC) (glenum target, glint level, glenum internalformat, glint x, glint y, glsizei width, glint border);
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DEXTPROC) (glenum target, glint level, glenum internalformat, glint x, glint y, glsizei width, glsizei height, glint border);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DEXTPROC) (glenum target, glint level, glint xoffset, glint x, glint y, glsizei width);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DEXTPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint x, glint y, glsizei width, glsizei height);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DEXTPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glint x, glint y, glsizei width, glsizei height);
#endif

#ifndef GL_EXT_histogram
#define GL_EXT_histogram 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_get_histogram_ext (glenum, glboolean, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_histogram_parameterfv_ext (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_histogram_parameteriv_ext (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_minmax_ext (glenum, glboolean, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_minmax_parameterfv_ext (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_minmax_parameteriv_ext (glenum, glenum, glint *);
GLAPI void APIENTRY gl_histogram_ext (glenum, glsizei, glenum, glboolean);
GLAPI void APIENTRY gl_minmax_ext (glenum, glenum, glboolean);
GLAPI void APIENTRY gl_reset_histogram_ext (glenum);
GLAPI void APIENTRY gl_reset_minmax_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLGETHISTOGRAMEXTPROC) (glenum target, glboolean reset, glenum format, glenum type, glvoid *values);
typedef void (APIENTRYP PFNGLGETHISTOGRAMPARAMETERFVEXTPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETHISTOGRAMPARAMETERIVEXTPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETMINMAXEXTPROC) (glenum target, glboolean reset, glenum format, glenum type, glvoid *values);
typedef void (APIENTRYP PFNGLGETMINMAXPARAMETERFVEXTPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETMINMAXPARAMETERIVEXTPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLHISTOGRAMEXTPROC) (glenum target, glsizei width, glenum internalformat, glboolean sink);
typedef void (APIENTRYP PFNGLMINMAXEXTPROC) (glenum target, glenum internalformat, glboolean sink);
typedef void (APIENTRYP PFNGLRESETHISTOGRAMEXTPROC) (glenum target);
typedef void (APIENTRYP PFNGLRESETMINMAXEXTPROC) (glenum target);
#endif

#ifndef GL_EXT_convolution
#define GL_EXT_convolution 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_convolution_filter_1dext (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_convolution_filter_2dext (glenum, glenum, glsizei, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_convolution_parameterf_ext (glenum, glenum, glfloat);
GLAPI void APIENTRY gl_convolution_parameterfv_ext (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_convolution_parameteri_ext (glenum, glenum, glint);
GLAPI void APIENTRY gl_convolution_parameteriv_ext (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_copy_convolution_filter_1dext (glenum, glenum, glint, glint, glsizei);
GLAPI void APIENTRY gl_copy_convolution_filter_2dext (glenum, glenum, glint, glint, glsizei, glsizei);
GLAPI void APIENTRY gl_get_convolution_filter_ext (glenum, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_convolution_parameterfv_ext (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_convolution_parameteriv_ext (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_separable_filter_ext (glenum, glenum, glenum, glvoid *, glvoid *, glvoid *);
GLAPI void APIENTRY gl_separable_filter_2dext (glenum, glenum, glsizei, glsizei, glenum, glenum, const glvoid *, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCONVOLUTIONFILTER1DEXTPROC) (glenum target, glenum internalformat, glsizei width, glenum format, glenum type, const glvoid *image);
typedef void (APIENTRYP PFNGLCONVOLUTIONFILTER2DEXTPROC) (glenum target, glenum internalformat, glsizei width, glsizei height, glenum format, glenum type, const glvoid *image);
typedef void (APIENTRYP PFNGLCONVOLUTIONPARAMETERFEXTPROC) (glenum target, glenum pname, glfloat params);
typedef void (APIENTRYP PFNGLCONVOLUTIONPARAMETERFVEXTPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLCONVOLUTIONPARAMETERIEXTPROC) (glenum target, glenum pname, glint params);
typedef void (APIENTRYP PFNGLCONVOLUTIONPARAMETERIVEXTPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width);
typedef void (APIENTRYP PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width, glsizei height);
typedef void (APIENTRYP PFNGLGETCONVOLUTIONFILTEREXTPROC) (glenum target, glenum format, glenum type, glvoid *image);
typedef void (APIENTRYP PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETSEPARABLEFILTEREXTPROC) (glenum target, glenum format, glenum type, glvoid *row, glvoid *column, glvoid *span);
typedef void (APIENTRYP PFNGLSEPARABLEFILTER2DEXTPROC) (glenum target, glenum internalformat, glsizei width, glsizei height, glenum format, glenum type, const glvoid *row, const glvoid *column);
#endif

#ifndef GL_EXT_color_matrix
#define GL_EXT_color_matrix 1
#endif

#ifndef GL_SGI_color_table
#define GL_SGI_color_table 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_color_table_sgi (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_color_table_parameterfv_sgi (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_color_table_parameteriv_sgi (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_copy_color_table_sgi (glenum, glenum, glint, glint, glsizei);
GLAPI void APIENTRY gl_get_color_table_sgi (glenum, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_color_table_parameterfv_sgi (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_color_table_parameteriv_sgi (glenum, glenum, glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOLORTABLESGIPROC) (glenum target, glenum internalformat, glsizei width, glenum format, glenum type, const glvoid *table);
typedef void (APIENTRYP PFNGLCOLORTABLEPARAMETERFVSGIPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLCOLORTABLEPARAMETERIVSGIPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLCOPYCOLORTABLESGIPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width);
typedef void (APIENTRYP PFNGLGETCOLORTABLESGIPROC) (glenum target, glenum format, glenum type, glvoid *table);
typedef void (APIENTRYP PFNGLGETCOLORTABLEPARAMETERFVSGIPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETCOLORTABLEPARAMETERIVSGIPROC) (glenum target, glenum pname, glint *params);
#endif

#ifndef GL_SGIX_pixel_texture
#define GL_SGIX_pixel_texture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_pixel_tex_gen_sgix (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPIXELTEXGENSGIXPROC) (glenum mode);
#endif

#ifndef GL_SGIS_pixel_texture
#define GL_SGIS_pixel_texture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_pixel_tex_gen_parameteri_sgis (glenum, glint);
GLAPI void APIENTRY gl_pixel_tex_gen_parameteriv_sgis (glenum, const glint *);
GLAPI void APIENTRY gl_pixel_tex_gen_parameterf_sgis (glenum, glfloat);
GLAPI void APIENTRY gl_pixel_tex_gen_parameterfv_sgis (glenum, const glfloat *);
GLAPI void APIENTRY gl_get_pixel_tex_gen_parameteriv_sgis (glenum, glint *);
GLAPI void APIENTRY gl_get_pixel_tex_gen_parameterfv_sgis (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPIXELTEXGENPARAMETERISGISPROC) (glenum pname, glint param);
typedef void (APIENTRYP PFNGLPIXELTEXGENPARAMETERIVSGISPROC) (glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLPIXELTEXGENPARAMETERFSGISPROC) (glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLPIXELTEXGENPARAMETERFVSGISPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC) (glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC) (glenum pname, glfloat *params);
#endif

#ifndef GL_SGIS_texture4D
#define GL_SGIS_texture4D 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_tex_image_4dsgis (glenum, glint, glenum, glsizei, glsizei, glsizei, glsizei, glint, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_tex_sub_image_4dsgis (glenum, glint, glint, glint, glint, glint, glsizei, glsizei, glsizei, glsizei, glenum, glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTEXIMAGE4DSGISPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glsizei depth, glsizei size4d, glint border, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE4DSGISPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glint woffset, glsizei width, glsizei height, glsizei depth, glsizei size4d, glenum format, glenum type, const glvoid *pixels);
#endif

#ifndef GL_SGI_texture_color_table
#define GL_SGI_texture_color_table 1
#endif

#ifndef GL_EXT_cmyka
#define GL_EXT_cmyka 1
#endif

#ifndef GL_EXT_texture_object
#define GL_EXT_texture_object 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI glboolean APIENTRY gl_are_textures_resident_ext (glsizei, const gluint *, glboolean *);
GLAPI void APIENTRY gl_bind_texture_ext (glenum, gluint);
GLAPI void APIENTRY gl_delete_textures_ext (glsizei, const gluint *);
GLAPI void APIENTRY gl_gen_textures_ext (glsizei, gluint *);
GLAPI glboolean APIENTRY gl_is_texture_ext (gluint);
GLAPI void APIENTRY gl_prioritize_textures_ext (glsizei, const gluint *, const glclampf *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef glboolean (APIENTRYP PFNGLARETEXTURESRESIDENTEXTPROC) (glsizei n, const gluint *textures, glboolean *residences);
typedef void (APIENTRYP PFNGLBINDTEXTUREEXTPROC) (glenum target, gluint texture);
typedef void (APIENTRYP PFNGLDELETETEXTURESEXTPROC) (glsizei n, const gluint *textures);
typedef void (APIENTRYP PFNGLGENTEXTURESEXTPROC) (glsizei n, gluint *textures);
typedef glboolean (APIENTRYP PFNGLISTEXTUREEXTPROC) (gluint texture);
typedef void (APIENTRYP PFNGLPRIORITIZETEXTURESEXTPROC) (glsizei n, const gluint *textures, const glclampf *priorities);
#endif

#ifndef GL_SGIS_detail_texture
#define GL_SGIS_detail_texture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_detail_tex_func_sgis (glenum, glsizei, const glfloat *);
GLAPI void APIENTRY gl_get_detail_tex_func_sgis (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLDETAILTEXFUNCSGISPROC) (glenum target, glsizei n, const glfloat *points);
typedef void (APIENTRYP PFNGLGETDETAILTEXFUNCSGISPROC) (glenum target, glfloat *points);
#endif

#ifndef GL_SGIS_sharpen_texture
#define GL_SGIS_sharpen_texture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_sharpen_tex_func_sgis (glenum, glsizei, const glfloat *);
GLAPI void APIENTRY gl_get_sharpen_tex_func_sgis (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLSHARPENTEXFUNCSGISPROC) (glenum target, glsizei n, const glfloat *points);
typedef void (APIENTRYP PFNGLGETSHARPENTEXFUNCSGISPROC) (glenum target, glfloat *points);
#endif

#ifndef GL_EXT_packed_pixels
#define GL_EXT_packed_pixels 1
#endif

#ifndef GL_SGIS_texture_lod
#define GL_SGIS_texture_lod 1
#endif

#ifndef GL_SGIS_multisample
#define GL_SGIS_multisample 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_sample_mask_sgis (glclampf, glboolean);
GLAPI void APIENTRY gl_sample_pattern_sgis (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLSAMPLEMASKSGISPROC) (glclampf value, glboolean invert);
typedef void (APIENTRYP PFNGLSAMPLEPATTERNSGISPROC) (glenum pattern);
#endif

#ifndef GL_EXT_rescale_normal
#define GL_EXT_rescale_normal 1
#endif

#ifndef GL_EXT_vertex_array
#define GL_EXT_vertex_array 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_array_element_ext (glint);
GLAPI void APIENTRY gl_color_pointer_ext (glint, glenum, glsizei, glsizei, const glvoid *);
GLAPI void APIENTRY gl_draw_arrays_ext (glenum, glint, glsizei);
GLAPI void APIENTRY gl_edge_flag_pointer_ext (glsizei, glsizei, const glboolean *);
GLAPI void APIENTRY gl_get_pointerv_ext (glenum, glvoid* *);
GLAPI void APIENTRY gl_index_pointer_ext (glenum, glsizei, glsizei, const glvoid *);
GLAPI void APIENTRY gl_normal_pointer_ext (glenum, glsizei, glsizei, const glvoid *);
GLAPI void APIENTRY gl_tex_coord_pointer_ext (glint, glenum, glsizei, glsizei, const glvoid *);
GLAPI void APIENTRY gl_vertex_pointer_ext (glint, glenum, glsizei, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLARRAYELEMENTEXTPROC) (glint i);
typedef void (APIENTRYP PFNGLCOLORPOINTEREXTPROC) (glint size, glenum type, glsizei stride, glsizei count, const glvoid *pointer);
typedef void (APIENTRYP PFNGLDRAWARRAYSEXTPROC) (glenum mode, glint first, glsizei count);
typedef void (APIENTRYP PFNGLEDGEFLAGPOINTEREXTPROC) (glsizei stride, glsizei count, const glboolean *pointer);
typedef void (APIENTRYP PFNGLGETPOINTERVEXTPROC) (glenum pname, glvoid* *params);
typedef void (APIENTRYP PFNGLINDEXPOINTEREXTPROC) (glenum type, glsizei stride, glsizei count, const glvoid *pointer);
typedef void (APIENTRYP PFNGLNORMALPOINTEREXTPROC) (glenum type, glsizei stride, glsizei count, const glvoid *pointer);
typedef void (APIENTRYP PFNGLTEXCOORDPOINTEREXTPROC) (glint size, glenum type, glsizei stride, glsizei count, const glvoid *pointer);
typedef void (APIENTRYP PFNGLVERTEXPOINTEREXTPROC) (glint size, glenum type, glsizei stride, glsizei count, const glvoid *pointer);
#endif

#ifndef GL_EXT_misc_attribute
#define GL_EXT_misc_attribute 1
#endif

#ifndef GL_SGIS_generate_mipmap
#define GL_SGIS_generate_mipmap 1
#endif

#ifndef GL_SGIX_clipmap
#define GL_SGIX_clipmap 1
#endif

#ifndef GL_SGIX_shadow
#define GL_SGIX_shadow 1
#endif

#ifndef GL_SGIS_texture_edge_clamp
#define GL_SGIS_texture_edge_clamp 1
#endif

#ifndef GL_SGIS_texture_border_clamp
#define GL_SGIS_texture_border_clamp 1
#endif

#ifndef GL_EXT_blend_minmax
#define GL_EXT_blend_minmax 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_blend_equation_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBLENDEQUATIONEXTPROC) (glenum mode);
#endif

#ifndef GL_EXT_blend_subtract
#define GL_EXT_blend_subtract 1
#endif

#ifndef GL_EXT_blend_logic_op
#define GL_EXT_blend_logic_op 1
#endif

#ifndef GL_SGIX_interlace
#define GL_SGIX_interlace 1
#endif

#ifndef GL_SGIX_pixel_tiles
#define GL_SGIX_pixel_tiles 1
#endif

#ifndef GL_SGIX_texture_select
#define GL_SGIX_texture_select 1
#endif

#ifndef GL_SGIX_sprite
#define GL_SGIX_sprite 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_sprite_parameterf_sgix (glenum, glfloat);
GLAPI void APIENTRY gl_sprite_parameterfv_sgix (glenum, const glfloat *);
GLAPI void APIENTRY gl_sprite_parameteri_sgix (glenum, glint);
GLAPI void APIENTRY gl_sprite_parameteriv_sgix (glenum, const glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLSPRITEPARAMETERFSGIXPROC) (glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLSPRITEPARAMETERFVSGIXPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLSPRITEPARAMETERISGIXPROC) (glenum pname, glint param);
typedef void (APIENTRYP PFNGLSPRITEPARAMETERIVSGIXPROC) (glenum pname, const glint *params);
#endif

#ifndef GL_SGIX_texture_multi_buffer
#define GL_SGIX_texture_multi_buffer 1
#endif

#ifndef GL_EXT_point_parameters
#define GL_EXT_point_parameters 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_point_parameterf_ext (glenum, glfloat);
GLAPI void APIENTRY gl_point_parameterfv_ext (glenum, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPOINTPARAMETERFEXTPROC) (glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVEXTPROC) (glenum pname, const glfloat *params);
#endif

#ifndef GL_SGIS_point_parameters
#define GL_SGIS_point_parameters 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_point_parameterf_sgis (glenum, glfloat);
GLAPI void APIENTRY gl_point_parameterfv_sgis (glenum, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPOINTPARAMETERFSGISPROC) (glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVSGISPROC) (glenum pname, const glfloat *params);
#endif

#ifndef GL_SGIX_instruments
#define GL_SGIX_instruments 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI glint APIENTRY gl_get_instruments_sgix (void);
GLAPI void APIENTRY gl_instruments_buffer_sgix (glsizei, glint *);
GLAPI glint APIENTRY gl_poll_instruments_sgix (glint *);
GLAPI void APIENTRY gl_read_instruments_sgix (glint);
GLAPI void APIENTRY gl_start_instruments_sgix (void);
GLAPI void APIENTRY gl_stop_instruments_sgix (glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef glint (APIENTRYP PFNGLGETINSTRUMENTSSGIXPROC) (void);
typedef void (APIENTRYP PFNGLINSTRUMENTSBUFFERSGIXPROC) (glsizei size, glint *buffer);
typedef glint (APIENTRYP PFNGLPOLLINSTRUMENTSSGIXPROC) (glint *marker_p);
typedef void (APIENTRYP PFNGLREADINSTRUMENTSSGIXPROC) (glint marker);
typedef void (APIENTRYP PFNGLSTARTINSTRUMENTSSGIXPROC) (void);
typedef void (APIENTRYP PFNGLSTOPINSTRUMENTSSGIXPROC) (glint marker);
#endif

#ifndef GL_SGIX_texture_scale_bias
#define GL_SGIX_texture_scale_bias 1
#endif

#ifndef GL_SGIX_framezoom
#define GL_SGIX_framezoom 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_frame_zoom_sgix (glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLFRAMEZOOMSGIXPROC) (glint factor);
#endif

#ifndef GL_SGIX_tag_sample_buffer
#define GL_SGIX_tag_sample_buffer 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_tag_sample_buffer_sgix (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTAGSAMPLEBUFFERSGIXPROC) (void);
#endif

#ifndef GL_SGIX_polynomial_ffd
#define GL_SGIX_polynomial_ffd 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_deformation_map_3d_sgix (glenum, gldouble, gldouble, glint, glint, gldouble, gldouble, glint, glint, gldouble, gldouble, glint, glint, const gldouble *);
GLAPI void APIENTRY gl_deformation_map_3f_sgix (glenum, glfloat, glfloat, glint, glint, glfloat, glfloat, glint, glint, glfloat, glfloat, glint, glint, const glfloat *);
GLAPI void APIENTRY gl_deform_sgix (glbitfield);
GLAPI void APIENTRY gl_load_identity_deformation_map_sgix (glbitfield);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLDEFORMATIONMAP3DSGIXPROC) (glenum target, gldouble u1, gldouble u2, glint ustride, glint uorder, gldouble v1, gldouble v2, glint vstride, glint vorder, gldouble w1, gldouble w2, glint wstride, glint worder, const gldouble *points);
typedef void (APIENTRYP PFNGLDEFORMATIONMAP3FSGIXPROC) (glenum target, glfloat u1, glfloat u2, glint ustride, glint uorder, glfloat v1, glfloat v2, glint vstride, glint vorder, glfloat w1, glfloat w2, glint wstride, glint worder, const glfloat *points);
typedef void (APIENTRYP PFNGLDEFORMSGIXPROC) (glbitfield mask);
typedef void (APIENTRYP PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC) (glbitfield mask);
#endif

#ifndef GL_SGIX_reference_plane
#define GL_SGIX_reference_plane 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_reference_plane_sgix (const gldouble *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLREFERENCEPLANESGIXPROC) (const gldouble *equation);
#endif

#ifndef GL_SGIX_flush_raster
#define GL_SGIX_flush_raster 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_flush_raster_sgix (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLFLUSHRASTERSGIXPROC) (void);
#endif

#ifndef GL_SGIX_depth_texture
#define GL_SGIX_depth_texture 1
#endif

#ifndef GL_SGIS_fog_function
#define GL_SGIS_fog_function 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_fog_func_sgis (glsizei, const glfloat *);
GLAPI void APIENTRY gl_get_fog_func_sgis (glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLFOGFUNCSGISPROC) (glsizei n, const glfloat *points);
typedef void (APIENTRYP PFNGLGETFOGFUNCSGISPROC) (glfloat *points);
#endif

#ifndef GL_SGIX_fog_offset
#define GL_SGIX_fog_offset 1
#endif

#ifndef GL_HP_image_transform
#define GL_HP_image_transform 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_image_transform_parameteri_hp (glenum, glenum, glint);
GLAPI void APIENTRY gl_image_transform_parameterf_hp (glenum, glenum, glfloat);
GLAPI void APIENTRY gl_image_transform_parameteriv_hp (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_image_transform_parameterfv_hp (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_get_image_transform_parameteriv_hp (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_image_transform_parameterfv_hp (glenum, glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLIMAGETRANSFORMPARAMETERIHPPROC) (glenum target, glenum pname, glint param);
typedef void (APIENTRYP PFNGLIMAGETRANSFORMPARAMETERFHPPROC) (glenum target, glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLIMAGETRANSFORMPARAMETERIVHPPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLIMAGETRANSFORMPARAMETERFVHPPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC) (glenum target, glenum pname, glfloat *params);
#endif

#ifndef GL_HP_convolution_border_modes
#define GL_HP_convolution_border_modes 1
#endif

#ifndef GL_SGIX_texture_add_env
#define GL_SGIX_texture_add_env 1
#endif

#ifndef GL_EXT_color_subtable
#define GL_EXT_color_subtable 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_color_sub_table_ext (glenum, glsizei, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_copy_color_sub_table_ext (glenum, glsizei, glint, glint, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOLORSUBTABLEEXTPROC) (glenum target, glsizei start, glsizei count, glenum format, glenum type, const glvoid *data);
typedef void (APIENTRYP PFNGLCOPYCOLORSUBTABLEEXTPROC) (glenum target, glsizei start, glint x, glint y, glsizei width);
#endif

#ifndef GL_PGI_vertex_hints
#define GL_PGI_vertex_hints 1
#endif

#ifndef GL_PGI_misc_hints
#define GL_PGI_misc_hints 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_hint_pgi (glenum, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLHINTPGIPROC) (glenum target, glint mode);
#endif

#ifndef GL_EXT_paletted_texture
#define GL_EXT_paletted_texture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_color_table_ext (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
GLAPI void APIENTRY gl_get_color_table_ext (glenum, glenum, glenum, glvoid *);
GLAPI void APIENTRY gl_get_color_table_parameteriv_ext (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_color_table_parameterfv_ext (glenum, glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOLORTABLEEXTPROC) (glenum target, glenum internal_format, glsizei width, glenum format, glenum type, const glvoid *table);
typedef void (APIENTRYP PFNGLGETCOLORTABLEEXTPROC) (glenum target, glenum format, glenum type, glvoid *data);
typedef void (APIENTRYP PFNGLGETCOLORTABLEPARAMETERIVEXTPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETCOLORTABLEPARAMETERFVEXTPROC) (glenum target, glenum pname, glfloat *params);
#endif

#ifndef GL_EXT_clip_volume_hint
#define GL_EXT_clip_volume_hint 1
#endif

#ifndef GL_SGIX_list_priority
#define GL_SGIX_list_priority 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_get_list_parameterfv_sgix (gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_list_parameteriv_sgix (gluint, glenum, glint *);
GLAPI void APIENTRY gl_list_parameterf_sgix (gluint, glenum, glfloat);
GLAPI void APIENTRY gl_list_parameterfv_sgix (gluint, glenum, const glfloat *);
GLAPI void APIENTRY gl_list_parameteri_sgix (gluint, glenum, glint);
GLAPI void APIENTRY gl_list_parameteriv_sgix (gluint, glenum, const glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLGETLISTPARAMETERFVSGIXPROC) (gluint list, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETLISTPARAMETERIVSGIXPROC) (gluint list, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLLISTPARAMETERFSGIXPROC) (gluint list, glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLLISTPARAMETERFVSGIXPROC) (gluint list, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLLISTPARAMETERISGIXPROC) (gluint list, glenum pname, glint param);
typedef void (APIENTRYP PFNGLLISTPARAMETERIVSGIXPROC) (gluint list, glenum pname, const glint *params);
#endif

#ifndef GL_SGIX_ir_instrument1
#define GL_SGIX_ir_instrument1 1
#endif

#ifndef GL_SGIX_calligraphic_fragment
#define GL_SGIX_calligraphic_fragment 1
#endif

#ifndef GL_SGIX_texture_lod_bias
#define GL_SGIX_texture_lod_bias 1
#endif

#ifndef GL_SGIX_shadow_ambient
#define GL_SGIX_shadow_ambient 1
#endif

#ifndef GL_EXT_index_texture
#define GL_EXT_index_texture 1
#endif

#ifndef GL_EXT_index_material
#define GL_EXT_index_material 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_index_material_ext (glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLINDEXMATERIALEXTPROC) (glenum face, glenum mode);
#endif

#ifndef GL_EXT_index_func
#define GL_EXT_index_func 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_index_func_ext (glenum, glclampf);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLINDEXFUNCEXTPROC) (glenum func, glclampf ref);
#endif

#ifndef GL_EXT_index_array_formats
#define GL_EXT_index_array_formats 1
#endif

#ifndef GL_EXT_compiled_vertex_array
#define GL_EXT_compiled_vertex_array 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_lock_arrays_ext (glint, glsizei);
GLAPI void APIENTRY gl_unlock_arrays_ext (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLLOCKARRAYSEXTPROC) (glint first, glsizei count);
typedef void (APIENTRYP PFNGLUNLOCKARRAYSEXTPROC) (void);
#endif

#ifndef GL_EXT_cull_vertex
#define GL_EXT_cull_vertex 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_cull_parameterdv_ext (glenum, gldouble *);
GLAPI void APIENTRY gl_cull_parameterfv_ext (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCULLPARAMETERDVEXTPROC) (glenum pname, gldouble *params);
typedef void (APIENTRYP PFNGLCULLPARAMETERFVEXTPROC) (glenum pname, glfloat *params);
#endif

#ifndef GL_SGIX_ycrcb
#define GL_SGIX_ycrcb 1
#endif

#ifndef GL_SGIX_fragment_lighting
#define GL_SGIX_fragment_lighting 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_fragment_color_material_sgix (glenum, glenum);
GLAPI void APIENTRY gl_fragment_lightf_sgix (glenum, glenum, glfloat);
GLAPI void APIENTRY gl_fragment_lightfv_sgix (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_fragment_lighti_sgix (glenum, glenum, glint);
GLAPI void APIENTRY gl_fragment_lightiv_sgix (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_fragment_light_modelf_sgix (glenum, glfloat);
GLAPI void APIENTRY gl_fragment_light_modelfv_sgix (glenum, const glfloat *);
GLAPI void APIENTRY gl_fragment_light_modeli_sgix (glenum, glint);
GLAPI void APIENTRY gl_fragment_light_modeliv_sgix (glenum, const glint *);
GLAPI void APIENTRY gl_fragment_materialf_sgix (glenum, glenum, glfloat);
GLAPI void APIENTRY gl_fragment_materialfv_sgix (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_fragment_materiali_sgix (glenum, glenum, glint);
GLAPI void APIENTRY gl_fragment_materialiv_sgix (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_get_fragment_lightfv_sgix (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_fragment_lightiv_sgix (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_fragment_materialfv_sgix (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_fragment_materialiv_sgix (glenum, glenum, glint *);
GLAPI void APIENTRY gl_light_envi_sgix (glenum, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLFRAGMENTCOLORMATERIALSGIXPROC) (glenum face, glenum mode);
typedef void (APIENTRYP PFNGLFRAGMENTLIGHTFSGIXPROC) (glenum light, glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLFRAGMENTLIGHTFVSGIXPROC) (glenum light, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLFRAGMENTLIGHTISGIXPROC) (glenum light, glenum pname, glint param);
typedef void (APIENTRYP PFNGLFRAGMENTLIGHTIVSGIXPROC) (glenum light, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLFRAGMENTLIGHTMODELFSGIXPROC) (glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLFRAGMENTLIGHTMODELFVSGIXPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLFRAGMENTLIGHTMODELISGIXPROC) (glenum pname, glint param);
typedef void (APIENTRYP PFNGLFRAGMENTLIGHTMODELIVSGIXPROC) (glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLFRAGMENTMATERIALFSGIXPROC) (glenum face, glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLFRAGMENTMATERIALFVSGIXPROC) (glenum face, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLFRAGMENTMATERIALISGIXPROC) (glenum face, glenum pname, glint param);
typedef void (APIENTRYP PFNGLFRAGMENTMATERIALIVSGIXPROC) (glenum face, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLGETFRAGMENTLIGHTFVSGIXPROC) (glenum light, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETFRAGMENTLIGHTIVSGIXPROC) (glenum light, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETFRAGMENTMATERIALFVSGIXPROC) (glenum face, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETFRAGMENTMATERIALIVSGIXPROC) (glenum face, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLLIGHTENVISGIXPROC) (glenum pname, glint param);
#endif

#ifndef GL_IBM_rasterpos_clip
#define GL_IBM_rasterpos_clip 1
#endif

#ifndef GL_HP_texture_lighting
#define GL_HP_texture_lighting 1
#endif

#ifndef GL_EXT_draw_range_elements
#define GL_EXT_draw_range_elements 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_draw_range_elements_ext (glenum, gluint, gluint, glsizei, glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSEXTPROC) (glenum mode, gluint start, gluint end, glsizei count, glenum type, const glvoid *indices);
#endif

#ifndef GL_WIN_phong_shading
#define GL_WIN_phong_shading 1
#endif

#ifndef GL_WIN_specular_fog
#define GL_WIN_specular_fog 1
#endif

#ifndef GL_EXT_light_texture
#define GL_EXT_light_texture 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_apply_texture_ext (glenum);
GLAPI void APIENTRY gl_texture_light_ext (glenum);
GLAPI void APIENTRY gl_texture_material_ext (glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLAPPLYTEXTUREEXTPROC) (glenum mode);
typedef void (APIENTRYP PFNGLTEXTURELIGHTEXTPROC) (glenum pname);
typedef void (APIENTRYP PFNGLTEXTUREMATERIALEXTPROC) (glenum face, glenum mode);
#endif

#ifndef GL_SGIX_blend_alpha_minmax
#define GL_SGIX_blend_alpha_minmax 1
#endif

#ifndef GL_EXT_bgra
#define GL_EXT_bgra 1
#endif

#ifndef GL_SGIX_async
#define GL_SGIX_async 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_async_marker_sgix (gluint);
GLAPI glint APIENTRY gl_finish_async_sgix (gluint *);
GLAPI glint APIENTRY gl_poll_async_sgix (gluint *);
GLAPI gluint APIENTRY gl_gen_async_markers_sgix (glsizei);
GLAPI void APIENTRY gl_delete_async_markers_sgix (gluint, glsizei);
GLAPI glboolean APIENTRY gl_is_async_marker_sgix (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLASYNCMARKERSGIXPROC) (gluint marker);
typedef glint (APIENTRYP PFNGLFINISHASYNCSGIXPROC) (gluint *markerp);
typedef glint (APIENTRYP PFNGLPOLLASYNCSGIXPROC) (gluint *markerp);
typedef gluint (APIENTRYP PFNGLGENASYNCMARKERSSGIXPROC) (glsizei range);
typedef void (APIENTRYP PFNGLDELETEASYNCMARKERSSGIXPROC) (gluint marker, glsizei range);
typedef glboolean (APIENTRYP PFNGLISASYNCMARKERSGIXPROC) (gluint marker);
#endif

#ifndef GL_SGIX_async_pixel
#define GL_SGIX_async_pixel 1
#endif

#ifndef GL_SGIX_async_histogram
#define GL_SGIX_async_histogram 1
#endif

#ifndef GL_INTEL_parallel_arrays
#define GL_INTEL_parallel_arrays 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_vertex_pointerv_intel (glint, glenum, const glvoid* *);
GLAPI void APIENTRY gl_normal_pointerv_intel (glenum, const glvoid* *);
GLAPI void APIENTRY gl_color_pointerv_intel (glint, glenum, const glvoid* *);
GLAPI void APIENTRY gl_tex_coord_pointerv_intel (glint, glenum, const glvoid* *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLVERTEXPOINTERVINTELPROC) (glint size, glenum type, const glvoid* *pointer);
typedef void (APIENTRYP PFNGLNORMALPOINTERVINTELPROC) (glenum type, const glvoid* *pointer);
typedef void (APIENTRYP PFNGLCOLORPOINTERVINTELPROC) (glint size, glenum type, const glvoid* *pointer);
typedef void (APIENTRYP PFNGLTEXCOORDPOINTERVINTELPROC) (glint size, glenum type, const glvoid* *pointer);
#endif

#ifndef GL_HP_occlusion_test
#define GL_HP_occlusion_test 1
#endif

#ifndef GL_EXT_pixel_transform
#define GL_EXT_pixel_transform 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_pixel_transform_parameteri_ext (glenum, glenum, glint);
GLAPI void APIENTRY gl_pixel_transform_parameterf_ext (glenum, glenum, glfloat);
GLAPI void APIENTRY gl_pixel_transform_parameteriv_ext (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_pixel_transform_parameterfv_ext (glenum, glenum, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPIXELTRANSFORMPARAMETERIEXTPROC) (glenum target, glenum pname, glint param);
typedef void (APIENTRYP PFNGLPIXELTRANSFORMPARAMETERFEXTPROC) (glenum target, glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC) (glenum target, glenum pname, const glfloat *params);
#endif

#ifndef GL_EXT_pixel_transform_color_table
#define GL_EXT_pixel_transform_color_table 1
#endif

#ifndef GL_EXT_shared_texture_palette
#define GL_EXT_shared_texture_palette 1
#endif

#ifndef GL_EXT_separate_specular_color
#define GL_EXT_separate_specular_color 1
#endif

#ifndef GL_EXT_secondary_color
#define GL_EXT_secondary_color 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_secondary_color_3b_ext (glbyte, glbyte, glbyte);
GLAPI void APIENTRY gl_secondary_color_3bv_ext (const glbyte *);
GLAPI void APIENTRY gl_secondary_color_3d_ext (gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_secondary_color_3dv_ext (const gldouble *);
GLAPI void APIENTRY gl_secondary_color_3f_ext (glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_secondary_color_3fv_ext (const glfloat *);
GLAPI void APIENTRY gl_secondary_color_3i_ext (glint, glint, glint);
GLAPI void APIENTRY gl_secondary_color_3iv_ext (const glint *);
GLAPI void APIENTRY gl_secondary_color_3s_ext (glshort, glshort, glshort);
GLAPI void APIENTRY gl_secondary_color_3sv_ext (const glshort *);
GLAPI void APIENTRY gl_secondary_color_3ub_ext (glubyte, glubyte, glubyte);
GLAPI void APIENTRY gl_secondary_color_3ubv_ext (const glubyte *);
GLAPI void APIENTRY gl_secondary_color_3ui_ext (gluint, gluint, gluint);
GLAPI void APIENTRY gl_secondary_color_3uiv_ext (const gluint *);
GLAPI void APIENTRY gl_secondary_color_3us_ext (glushort, glushort, glushort);
GLAPI void APIENTRY gl_secondary_color_3usv_ext (const glushort *);
GLAPI void APIENTRY gl_secondary_color_pointer_ext (glint, glenum, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BEXTPROC) (glbyte red, glbyte green, glbyte blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BVEXTPROC) (const glbyte *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DEXTPROC) (gldouble red, gldouble green, gldouble blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DVEXTPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FEXTPROC) (glfloat red, glfloat green, glfloat blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FVEXTPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IEXTPROC) (glint red, glint green, glint blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IVEXTPROC) (const glint *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SEXTPROC) (glshort red, glshort green, glshort blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SVEXTPROC) (const glshort *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBEXTPROC) (glubyte red, glubyte green, glubyte blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBVEXTPROC) (const glubyte *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIEXTPROC) (gluint red, gluint green, gluint blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIVEXTPROC) (const gluint *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USEXTPROC) (glushort red, glushort green, glushort blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USVEXTPROC) (const glushort *v);
typedef void (APIENTRYP PFNGLSECONDARYCOLORPOINTEREXTPROC) (glint size, glenum type, glsizei stride, const glvoid *pointer);
#endif

#ifndef GL_EXT_texture_perturb_normal
#define GL_EXT_texture_perturb_normal 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_texture_normal_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTEXTURENORMALEXTPROC) (glenum mode);
#endif

#ifndef GL_EXT_multi_draw_arrays
#define GL_EXT_multi_draw_arrays 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_multi_draw_arrays_ext (glenum, glint *, glsizei *, glsizei);
GLAPI void APIENTRY gl_multi_draw_elements_ext (glenum, const glsizei *, glenum, const glvoid* *, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSEXTPROC) (glenum mode, glint *first, glsizei *count, glsizei primcount);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSEXTPROC) (glenum mode, const glsizei *count, glenum type, const glvoid* *indices, glsizei primcount);
#endif

#ifndef GL_EXT_fog_coord
#define GL_EXT_fog_coord 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_fog_coordf_ext (glfloat);
GLAPI void APIENTRY gl_fog_coordfv_ext (const glfloat *);
GLAPI void APIENTRY gl_fog_coordd_ext (gldouble);
GLAPI void APIENTRY gl_fog_coorddv_ext (const gldouble *);
GLAPI void APIENTRY gl_fog_coord_pointer_ext (glenum, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLFOGCOORDFEXTPROC) (glfloat coord);
typedef void (APIENTRYP PFNGLFOGCOORDFVEXTPROC) (const glfloat *coord);
typedef void (APIENTRYP PFNGLFOGCOORDDEXTPROC) (gldouble coord);
typedef void (APIENTRYP PFNGLFOGCOORDDVEXTPROC) (const gldouble *coord);
typedef void (APIENTRYP PFNGLFOGCOORDPOINTEREXTPROC) (glenum type, glsizei stride, const glvoid *pointer);
#endif

#ifndef GL_REND_screen_coordinates
#define GL_REND_screen_coordinates 1
#endif

#ifndef GL_EXT_coordinate_frame
#define GL_EXT_coordinate_frame 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_tangent_3b_ext (glbyte, glbyte, glbyte);
GLAPI void APIENTRY gl_tangent_3bv_ext (const glbyte *);
GLAPI void APIENTRY gl_tangent_3d_ext (gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_tangent_3dv_ext (const gldouble *);
GLAPI void APIENTRY gl_tangent_3f_ext (glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_tangent_3fv_ext (const glfloat *);
GLAPI void APIENTRY gl_tangent_3i_ext (glint, glint, glint);
GLAPI void APIENTRY gl_tangent_3iv_ext (const glint *);
GLAPI void APIENTRY gl_tangent_3s_ext (glshort, glshort, glshort);
GLAPI void APIENTRY gl_tangent_3sv_ext (const glshort *);
GLAPI void APIENTRY gl_binormal_3b_ext (glbyte, glbyte, glbyte);
GLAPI void APIENTRY gl_binormal_3bv_ext (const glbyte *);
GLAPI void APIENTRY gl_binormal_3d_ext (gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_binormal_3dv_ext (const gldouble *);
GLAPI void APIENTRY gl_binormal_3f_ext (glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_binormal_3fv_ext (const glfloat *);
GLAPI void APIENTRY gl_binormal_3i_ext (glint, glint, glint);
GLAPI void APIENTRY gl_binormal_3iv_ext (const glint *);
GLAPI void APIENTRY gl_binormal_3s_ext (glshort, glshort, glshort);
GLAPI void APIENTRY gl_binormal_3sv_ext (const glshort *);
GLAPI void APIENTRY gl_tangent_pointer_ext (glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_binormal_pointer_ext (glenum, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTANGENT3BEXTPROC) (glbyte tx, glbyte ty, glbyte tz);
typedef void (APIENTRYP PFNGLTANGENT3BVEXTPROC) (const glbyte *v);
typedef void (APIENTRYP PFNGLTANGENT3DEXTPROC) (gldouble tx, gldouble ty, gldouble tz);
typedef void (APIENTRYP PFNGLTANGENT3DVEXTPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLTANGENT3FEXTPROC) (glfloat tx, glfloat ty, glfloat tz);
typedef void (APIENTRYP PFNGLTANGENT3FVEXTPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLTANGENT3IEXTPROC) (glint tx, glint ty, glint tz);
typedef void (APIENTRYP PFNGLTANGENT3IVEXTPROC) (const glint *v);
typedef void (APIENTRYP PFNGLTANGENT3SEXTPROC) (glshort tx, glshort ty, glshort tz);
typedef void (APIENTRYP PFNGLTANGENT3SVEXTPROC) (const glshort *v);
typedef void (APIENTRYP PFNGLBINORMAL3BEXTPROC) (glbyte bx, glbyte by, glbyte bz);
typedef void (APIENTRYP PFNGLBINORMAL3BVEXTPROC) (const glbyte *v);
typedef void (APIENTRYP PFNGLBINORMAL3DEXTPROC) (gldouble bx, gldouble by, gldouble bz);
typedef void (APIENTRYP PFNGLBINORMAL3DVEXTPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLBINORMAL3FEXTPROC) (glfloat bx, glfloat by, glfloat bz);
typedef void (APIENTRYP PFNGLBINORMAL3FVEXTPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLBINORMAL3IEXTPROC) (glint bx, glint by, glint bz);
typedef void (APIENTRYP PFNGLBINORMAL3IVEXTPROC) (const glint *v);
typedef void (APIENTRYP PFNGLBINORMAL3SEXTPROC) (glshort bx, glshort by, glshort bz);
typedef void (APIENTRYP PFNGLBINORMAL3SVEXTPROC) (const glshort *v);
typedef void (APIENTRYP PFNGLTANGENTPOINTEREXTPROC) (glenum type, glsizei stride, const glvoid *pointer);
typedef void (APIENTRYP PFNGLBINORMALPOINTEREXTPROC) (glenum type, glsizei stride, const glvoid *pointer);
#endif

#ifndef GL_EXT_texture_env_combine
#define GL_EXT_texture_env_combine 1
#endif

#ifndef GL_APPLE_specular_vector
#define GL_APPLE_specular_vector 1
#endif

#ifndef GL_APPLE_transform_hint
#define GL_APPLE_transform_hint 1
#endif

#ifndef GL_SGIX_fog_scale
#define GL_SGIX_fog_scale 1
#endif

#ifndef GL_SUNX_constant_data
#define GL_SUNX_constant_data 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_finish_texture_sunx (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLFINISHTEXTURESUNXPROC) (void);
#endif

#ifndef GL_SUN_global_alpha
#define GL_SUN_global_alpha 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_global_alpha_factorb_sun (glbyte);
GLAPI void APIENTRY gl_global_alpha_factors_sun (glshort);
GLAPI void APIENTRY gl_global_alpha_factori_sun (glint);
GLAPI void APIENTRY gl_global_alpha_factorf_sun (glfloat);
GLAPI void APIENTRY gl_global_alpha_factord_sun (gldouble);
GLAPI void APIENTRY gl_global_alpha_factorub_sun (glubyte);
GLAPI void APIENTRY gl_global_alpha_factorus_sun (glushort);
GLAPI void APIENTRY gl_global_alpha_factorui_sun (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLGLOBALALPHAFACTORBSUNPROC) (glbyte factor);
typedef void (APIENTRYP PFNGLGLOBALALPHAFACTORSSUNPROC) (glshort factor);
typedef void (APIENTRYP PFNGLGLOBALALPHAFACTORISUNPROC) (glint factor);
typedef void (APIENTRYP PFNGLGLOBALALPHAFACTORFSUNPROC) (glfloat factor);
typedef void (APIENTRYP PFNGLGLOBALALPHAFACTORDSUNPROC) (gldouble factor);
typedef void (APIENTRYP PFNGLGLOBALALPHAFACTORUBSUNPROC) (glubyte factor);
typedef void (APIENTRYP PFNGLGLOBALALPHAFACTORUSSUNPROC) (glushort factor);
typedef void (APIENTRYP PFNGLGLOBALALPHAFACTORUISUNPROC) (gluint factor);
#endif

#ifndef GL_SUN_triangle_list
#define GL_SUN_triangle_list 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_replacement_codeui_sun (gluint);
GLAPI void APIENTRY gl_replacement_codeus_sun (glushort);
GLAPI void APIENTRY gl_replacement_codeub_sun (glubyte);
GLAPI void APIENTRY gl_replacement_codeuiv_sun (const gluint *);
GLAPI void APIENTRY gl_replacement_codeusv_sun (const glushort *);
GLAPI void APIENTRY gl_replacement_codeubv_sun (const glubyte *);
GLAPI void APIENTRY gl_replacement_code_pointer_sun (glenum, glsizei, const glvoid* *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUISUNPROC) (gluint code);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUSSUNPROC) (glushort code);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUBSUNPROC) (glubyte code);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUIVSUNPROC) (const gluint *code);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUSVSUNPROC) (const glushort *code);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUBVSUNPROC) (const glubyte *code);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEPOINTERSUNPROC) (glenum type, glsizei stride, const glvoid* *pointer);
#endif

#ifndef GL_SUN_vertex
#define GL_SUN_vertex 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_color_4ub_vertex_2f_sun (glubyte, glubyte, glubyte, glubyte, glfloat, glfloat);
GLAPI void APIENTRY gl_color_4ub_vertex_2fv_sun (const glubyte *, const glfloat *);
GLAPI void APIENTRY gl_color_4ub_vertex_3f_sun (glubyte, glubyte, glubyte, glubyte, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_color_4ub_vertex_3fv_sun (const glubyte *, const glfloat *);
GLAPI void APIENTRY gl_color_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_color_3f_vertex_3fv_sun (const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_normal_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_normal_3f_vertex_3fv_sun (const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_color_4f_normal_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_color_4f_normal_3f_vertex_3fv_sun (const glfloat *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_tex_coord_2f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_tex_coord_2f_vertex_3fv_sun (const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_tex_coord_4f_vertex_4f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_tex_coord_4f_vertex_4fv_sun (const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_tex_coord_2f_color_4ub_vertex_3f_sun (glfloat, glfloat, glubyte, glubyte, glubyte, glubyte, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_tex_coord_2f_color_4ub_vertex_3fv_sun (const glfloat *, const glubyte *, const glfloat *);
GLAPI void APIENTRY gl_tex_coord_2f_color_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_tex_coord_2f_color_3f_vertex_3fv_sun (const glfloat *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_tex_coord_2f_normal_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_tex_coord_2f_normal_3f_vertex_3fv_sun (const glfloat *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_tex_coord_2f_color_4f_normal_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_tex_coord_2f_color_4f_normal_3f_vertex_3fv_sun (const glfloat *, const glfloat *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_tex_coord_4f_color_4f_normal_3f_vertex_4f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_tex_coord_4f_color_4f_normal_3f_vertex_4fv_sun (const glfloat *, const glfloat *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_replacement_codeui_vertex_3f_sun (gluint, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_replacement_codeui_vertex_3fv_sun (const gluint *, const glfloat *);
GLAPI void APIENTRY gl_replacement_codeui_color_4ub_vertex_3f_sun (gluint, glubyte, glubyte, glubyte, glubyte, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_replacement_codeui_color_4ub_vertex_3fv_sun (const gluint *, const glubyte *, const glfloat *);
GLAPI void APIENTRY gl_replacement_codeui_color_3f_vertex_3f_sun (gluint, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_replacement_codeui_color_3f_vertex_3fv_sun (const gluint *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_replacement_codeui_normal_3f_vertex_3f_sun (gluint, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_replacement_codeui_normal_3f_vertex_3fv_sun (const gluint *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_replacement_codeui_color_4f_normal_3f_vertex_3f_sun (gluint, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_replacement_codeui_color_4f_normal_3f_vertex_3fv_sun (const gluint *, const glfloat *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_replacement_codeui_tex_coord_2f_vertex_3f_sun (gluint, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_replacement_codeui_tex_coord_2f_vertex_3fv_sun (const gluint *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_replacement_codeui_tex_coord_2f_normal_3f_vertex_3f_sun (gluint, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_replacement_codeui_tex_coord_2f_normal_3f_vertex_3fv_sun (const gluint *, const glfloat *, const glfloat *, const glfloat *);
GLAPI void APIENTRY gl_replacement_codeui_tex_coord_2f_color_4f_normal_3f_vertex_3f_sun (gluint, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_replacement_codeui_tex_coord_2f_color_4f_normal_3f_vertex_3fv_sun (const gluint *, const glfloat *, const glfloat *, const glfloat *, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOLOR4UBVERTEX2FSUNPROC) (glubyte r, glubyte g, glubyte b, glubyte a, glfloat x, glfloat y);
typedef void (APIENTRYP PFNGLCOLOR4UBVERTEX2FVSUNPROC) (const glubyte *c, const glfloat *v);
typedef void (APIENTRYP PFNGLCOLOR4UBVERTEX3FSUNPROC) (glubyte r, glubyte g, glubyte b, glubyte a, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLCOLOR4UBVERTEX3FVSUNPROC) (const glubyte *c, const glfloat *v);
typedef void (APIENTRYP PFNGLCOLOR3FVERTEX3FSUNPROC) (glfloat r, glfloat g, glfloat b, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLCOLOR3FVERTEX3FVSUNPROC) (const glfloat *c, const glfloat *v);
typedef void (APIENTRYP PFNGLNORMAL3FVERTEX3FSUNPROC) (glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLNORMAL3FVERTEX3FVSUNPROC) (const glfloat *n, const glfloat *v);
typedef void (APIENTRYP PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC) (glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC) (const glfloat *c, const glfloat *n, const glfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD2FVERTEX3FSUNPROC) (glfloat s, glfloat t, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLTEXCOORD2FVERTEX3FVSUNPROC) (const glfloat *tc, const glfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD4FVERTEX4FSUNPROC) (glfloat s, glfloat t, glfloat p, glfloat q, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLTEXCOORD4FVERTEX4FVSUNPROC) (const glfloat *tc, const glfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC) (glfloat s, glfloat t, glubyte r, glubyte g, glubyte b, glubyte a, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC) (const glfloat *tc, const glubyte *c, const glfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC) (glfloat s, glfloat t, glfloat r, glfloat g, glfloat b, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC) (const glfloat *tc, const glfloat *c, const glfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC) (glfloat s, glfloat t, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC) (const glfloat *tc, const glfloat *n, const glfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC) (glfloat s, glfloat t, glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC) (const glfloat *tc, const glfloat *c, const glfloat *n, const glfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC) (glfloat s, glfloat t, glfloat p, glfloat q, glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC) (const glfloat *tc, const glfloat *c, const glfloat *n, const glfloat *v);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC) (gluint rc, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC) (const gluint *rc, const glfloat *v);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC) (gluint rc, glubyte r, glubyte g, glubyte b, glubyte a, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC) (const gluint *rc, const glubyte *c, const glfloat *v);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC) (gluint rc, glfloat r, glfloat g, glfloat b, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC) (const gluint *rc, const glfloat *c, const glfloat *v);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC) (gluint rc, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC) (const gluint *rc, const glfloat *n, const glfloat *v);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC) (gluint rc, glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC) (const gluint *rc, const glfloat *c, const glfloat *n, const glfloat *v);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC) (gluint rc, glfloat s, glfloat t, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC) (const gluint *rc, const glfloat *tc, const glfloat *v);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC) (gluint rc, glfloat s, glfloat t, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC) (const gluint *rc, const glfloat *tc, const glfloat *n, const glfloat *v);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC) (gluint rc, glfloat s, glfloat t, glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC) (const gluint *rc, const glfloat *tc, const glfloat *c, const glfloat *n, const glfloat *v);
#endif

#ifndef GL_EXT_blend_func_separate
#define GL_EXT_blend_func_separate 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_blend_func_separate_ext (glenum, glenum, glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEEXTPROC) (glenum sfactor_rgb, glenum dfactor_rgb, glenum sfactor_alpha, glenum dfactor_alpha);
#endif

#ifndef GL_INGR_blend_func_separate
#define GL_INGR_blend_func_separate 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_blend_func_separate_ingr (glenum, glenum, glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEINGRPROC) (glenum sfactor_rgb, glenum dfactor_rgb, glenum sfactor_alpha, glenum dfactor_alpha);
#endif

#ifndef GL_INGR_color_clamp
#define GL_INGR_color_clamp 1
#endif

#ifndef GL_INGR_interlace_read
#define GL_INGR_interlace_read 1
#endif

#ifndef GL_EXT_stencil_wrap
#define GL_EXT_stencil_wrap 1
#endif

#ifndef GL_EXT_422_pixels
#define GL_EXT_422_pixels 1
#endif

#ifndef GL_NV_texgen_reflection
#define GL_NV_texgen_reflection 1
#endif

#ifndef GL_SUN_convolution_border_modes
#define GL_SUN_convolution_border_modes 1
#endif

#ifndef GL_EXT_texture_env_add
#define GL_EXT_texture_env_add 1
#endif

#ifndef GL_EXT_texture_lod_bias
#define GL_EXT_texture_lod_bias 1
#endif

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_EXT_texture_filter_anisotropic 1
#endif

#ifndef GL_EXT_vertex_weighting
#define GL_EXT_vertex_weighting 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_vertex_weightf_ext (glfloat);
GLAPI void APIENTRY gl_vertex_weightfv_ext (const glfloat *);
GLAPI void APIENTRY gl_vertex_weight_pointer_ext (glsizei, glenum, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLVERTEXWEIGHTFEXTPROC) (glfloat weight);
typedef void (APIENTRYP PFNGLVERTEXWEIGHTFVEXTPROC) (const glfloat *weight);
typedef void (APIENTRYP PFNGLVERTEXWEIGHTPOINTEREXTPROC) (glsizei size, glenum type, glsizei stride, const glvoid *pointer);
#endif

#ifndef GL_NV_light_max_exponent
#define GL_NV_light_max_exponent 1
#endif

#ifndef GL_NV_vertex_array_range
#define GL_NV_vertex_array_range 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_flush_vertex_array_range_nv (void);
GLAPI void APIENTRY gl_vertex_array_range_nv (glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);
typedef void (APIENTRYP PFNGLVERTEXARRAYRANGENVPROC) (glsizei length, const glvoid *pointer);
#endif

#ifndef GL_NV_register_combiners
#define GL_NV_register_combiners 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_combiner_parameterfv_nv (glenum, const glfloat *);
GLAPI void APIENTRY gl_combiner_parameterf_nv (glenum, glfloat);
GLAPI void APIENTRY gl_combiner_parameteriv_nv (glenum, const glint *);
GLAPI void APIENTRY gl_combiner_parameteri_nv (glenum, glint);
GLAPI void APIENTRY gl_combiner_input_nv (glenum, glenum, glenum, glenum, glenum, glenum);
GLAPI void APIENTRY gl_combiner_output_nv (glenum, glenum, glenum, glenum, glenum, glenum, glenum, glboolean, glboolean, glboolean);
GLAPI void APIENTRY gl_final_combiner_input_nv (glenum, glenum, glenum, glenum);
GLAPI void APIENTRY gl_get_combiner_input_parameterfv_nv (glenum, glenum, glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_combiner_input_parameteriv_nv (glenum, glenum, glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_combiner_output_parameterfv_nv (glenum, glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_combiner_output_parameteriv_nv (glenum, glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_final_combiner_input_parameterfv_nv (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_final_combiner_input_parameteriv_nv (glenum, glenum, glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOMBINERPARAMETERFVNVPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLCOMBINERPARAMETERFNVPROC) (glenum pname, glfloat param);
typedef void (APIENTRYP PFNGLCOMBINERPARAMETERIVNVPROC) (glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLCOMBINERPARAMETERINVPROC) (glenum pname, glint param);
typedef void (APIENTRYP PFNGLCOMBINERINPUTNVPROC) (glenum stage, glenum portion, glenum variable, glenum input, glenum mapping, glenum component_usage);
typedef void (APIENTRYP PFNGLCOMBINEROUTPUTNVPROC) (glenum stage, glenum portion, glenum ab_output, glenum cd_output, glenum sum_output, glenum scale, glenum bias, glboolean ab_dot_product, glboolean cd_dot_product, glboolean mux_sum);
typedef void (APIENTRYP PFNGLFINALCOMBINERINPUTNVPROC) (glenum variable, glenum input, glenum mapping, glenum component_usage);
typedef void (APIENTRYP PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (glenum stage, glenum portion, glenum variable, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (glenum stage, glenum portion, glenum variable, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (glenum stage, glenum portion, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (glenum stage, glenum portion, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (glenum variable, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (glenum variable, glenum pname, glint *params);
#endif

#ifndef GL_NV_fog_distance
#define GL_NV_fog_distance 1
#endif

#ifndef GL_NV_texgen_emboss
#define GL_NV_texgen_emboss 1
#endif

#ifndef GL_NV_blend_square
#define GL_NV_blend_square 1
#endif

#ifndef GL_NV_texture_env_combine4
#define GL_NV_texture_env_combine4 1
#endif

#ifndef GL_MESA_resize_buffers
#define GL_MESA_resize_buffers 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_resize_buffers_mesa (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLRESIZEBUFFERSMESAPROC) (void);
#endif

#ifndef GL_MESA_window_pos
#define GL_MESA_window_pos 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_window_pos_2d_mesa (gldouble, gldouble);
GLAPI void APIENTRY gl_window_pos_2dv_mesa (const gldouble *);
GLAPI void APIENTRY gl_window_pos_2f_mesa (glfloat, glfloat);
GLAPI void APIENTRY gl_window_pos_2fv_mesa (const glfloat *);
GLAPI void APIENTRY gl_window_pos_2i_mesa (glint, glint);
GLAPI void APIENTRY gl_window_pos_2iv_mesa (const glint *);
GLAPI void APIENTRY gl_window_pos_2s_mesa (glshort, glshort);
GLAPI void APIENTRY gl_window_pos_2sv_mesa (const glshort *);
GLAPI void APIENTRY gl_window_pos_3d_mesa (gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_window_pos_3dv_mesa (const gldouble *);
GLAPI void APIENTRY gl_window_pos_3f_mesa (glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_window_pos_3fv_mesa (const glfloat *);
GLAPI void APIENTRY gl_window_pos_3i_mesa (glint, glint, glint);
GLAPI void APIENTRY gl_window_pos_3iv_mesa (const glint *);
GLAPI void APIENTRY gl_window_pos_3s_mesa (glshort, glshort, glshort);
GLAPI void APIENTRY gl_window_pos_3sv_mesa (const glshort *);
GLAPI void APIENTRY gl_window_pos_4d_mesa (gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_window_pos_4dv_mesa (const gldouble *);
GLAPI void APIENTRY gl_window_pos_4f_mesa (glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_window_pos_4fv_mesa (const glfloat *);
GLAPI void APIENTRY gl_window_pos_4i_mesa (glint, glint, glint, glint);
GLAPI void APIENTRY gl_window_pos_4iv_mesa (const glint *);
GLAPI void APIENTRY gl_window_pos_4s_mesa (glshort, glshort, glshort, glshort);
GLAPI void APIENTRY gl_window_pos_4sv_mesa (const glshort *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLWINDOWPOS2DMESAPROC) (gldouble x, gldouble y);
typedef void (APIENTRYP PFNGLWINDOWPOS2DVMESAPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2FMESAPROC) (glfloat x, glfloat y);
typedef void (APIENTRYP PFNGLWINDOWPOS2FVMESAPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2IMESAPROC) (glint x, glint y);
typedef void (APIENTRYP PFNGLWINDOWPOS2IVMESAPROC) (const glint *v);
typedef void (APIENTRYP PFNGLWINDOWPOS2SMESAPROC) (glshort x, glshort y);
typedef void (APIENTRYP PFNGLWINDOWPOS2SVMESAPROC) (const glshort *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3DMESAPROC) (gldouble x, gldouble y, gldouble z);
typedef void (APIENTRYP PFNGLWINDOWPOS3DVMESAPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3FMESAPROC) (glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLWINDOWPOS3FVMESAPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3IMESAPROC) (glint x, glint y, glint z);
typedef void (APIENTRYP PFNGLWINDOWPOS3IVMESAPROC) (const glint *v);
typedef void (APIENTRYP PFNGLWINDOWPOS3SMESAPROC) (glshort x, glshort y, glshort z);
typedef void (APIENTRYP PFNGLWINDOWPOS3SVMESAPROC) (const glshort *v);
typedef void (APIENTRYP PFNGLWINDOWPOS4DMESAPROC) (gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRYP PFNGLWINDOWPOS4DVMESAPROC) (const gldouble *v);
typedef void (APIENTRYP PFNGLWINDOWPOS4FMESAPROC) (glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLWINDOWPOS4FVMESAPROC) (const glfloat *v);
typedef void (APIENTRYP PFNGLWINDOWPOS4IMESAPROC) (glint x, glint y, glint z, glint w);
typedef void (APIENTRYP PFNGLWINDOWPOS4IVMESAPROC) (const glint *v);
typedef void (APIENTRYP PFNGLWINDOWPOS4SMESAPROC) (glshort x, glshort y, glshort z, glshort w);
typedef void (APIENTRYP PFNGLWINDOWPOS4SVMESAPROC) (const glshort *v);
#endif

#ifndef GL_IBM_cull_vertex
#define GL_IBM_cull_vertex 1
#endif

#ifndef GL_IBM_multimode_draw_arrays
#define GL_IBM_multimode_draw_arrays 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_multi_mode_draw_arrays_ibm (const glenum *, const glint *, const glsizei *, glsizei, glint);
GLAPI void APIENTRY gl_multi_mode_draw_elements_ibm (const glenum *, const glsizei *, glenum, const glvoid* const *, glsizei, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLMULTIMODEDRAWARRAYSIBMPROC) (const glenum *mode, const glint *first, const glsizei *count, glsizei primcount, glint modestride);
typedef void (APIENTRYP PFNGLMULTIMODEDRAWELEMENTSIBMPROC) (const glenum *mode, const glsizei *count, glenum type, const glvoid* const *indices, glsizei primcount, glint modestride);
#endif

#ifndef GL_IBM_vertex_array_lists
#define GL_IBM_vertex_array_lists 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_color_pointer_list_ibm (glint, glenum, glint, const glvoid* *, glint);
GLAPI void APIENTRY gl_secondary_color_pointer_list_ibm (glint, glenum, glint, const glvoid* *, glint);
GLAPI void APIENTRY gl_edge_flag_pointer_list_ibm (glint, const glboolean* *, glint);
GLAPI void APIENTRY gl_fog_coord_pointer_list_ibm (glenum, glint, const glvoid* *, glint);
GLAPI void APIENTRY gl_index_pointer_list_ibm (glenum, glint, const glvoid* *, glint);
GLAPI void APIENTRY gl_normal_pointer_list_ibm (glenum, glint, const glvoid* *, glint);
GLAPI void APIENTRY gl_tex_coord_pointer_list_ibm (glint, glenum, glint, const glvoid* *, glint);
GLAPI void APIENTRY gl_vertex_pointer_list_ibm (glint, glenum, glint, const glvoid* *, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOLORPOINTERLISTIBMPROC) (glint size, glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRYP PFNGLSECONDARYCOLORPOINTERLISTIBMPROC) (glint size, glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRYP PFNGLEDGEFLAGPOINTERLISTIBMPROC) (glint stride, const glboolean* *pointer, glint ptrstride);
typedef void (APIENTRYP PFNGLFOGCOORDPOINTERLISTIBMPROC) (glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRYP PFNGLINDEXPOINTERLISTIBMPROC) (glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRYP PFNGLNORMALPOINTERLISTIBMPROC) (glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRYP PFNGLTEXCOORDPOINTERLISTIBMPROC) (glint size, glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRYP PFNGLVERTEXPOINTERLISTIBMPROC) (glint size, glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
#endif

#ifndef GL_SGIX_subsample
#define GL_SGIX_subsample 1
#endif

#ifndef GL_SGIX_ycrcba
#define GL_SGIX_ycrcba 1
#endif

#ifndef GL_SGIX_ycrcb_subsample
#define GL_SGIX_ycrcb_subsample 1
#endif

#ifndef GL_SGIX_depth_pass_instrument
#define GL_SGIX_depth_pass_instrument 1
#endif

#ifndef GL_3DFX_texture_compression_FXT1
#define GL_3DFX_texture_compression_FXT1 1
#endif

#ifndef GL_3DFX_multisample
#define GL_3DFX_multisample 1
#endif

#ifndef GL_3DFX_tbuffer
#define GL_3DFX_tbuffer 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_tbuffer_mask_3dfx (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTBUFFERMASK3DFXPROC) (gluint mask);
#endif

#ifndef GL_EXT_multisample
#define GL_EXT_multisample 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_sample_mask_ext (glclampf, glboolean);
GLAPI void APIENTRY gl_sample_pattern_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLSAMPLEMASKEXTPROC) (glclampf value, glboolean invert);
typedef void (APIENTRYP PFNGLSAMPLEPATTERNEXTPROC) (glenum pattern);
#endif

#ifndef GL_SGIX_vertex_preclip
#define GL_SGIX_vertex_preclip 1
#endif

#ifndef GL_SGIX_convolution_accuracy
#define GL_SGIX_convolution_accuracy 1
#endif

#ifndef GL_SGIX_resample
#define GL_SGIX_resample 1
#endif

#ifndef GL_SGIS_point_line_texgen
#define GL_SGIS_point_line_texgen 1
#endif

#ifndef GL_SGIS_texture_color_mask
#define GL_SGIS_texture_color_mask 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_texture_color_mask_sgis (glboolean, glboolean, glboolean, glboolean);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTEXTURECOLORMASKSGISPROC) (glboolean red, glboolean green, glboolean blue, glboolean alpha);
#endif

#ifndef GL_SGIX_igloo_interface
#define GL_SGIX_igloo_interface 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_igloo_interface_sgix (glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLIGLOOINTERFACESGIXPROC) (glenum pname, const glvoid *params);
#endif

#ifndef GL_EXT_texture_env_dot3
#define GL_EXT_texture_env_dot3 1
#endif

#ifndef GL_ATI_texture_mirror_once
#define GL_ATI_texture_mirror_once 1
#endif

#ifndef GL_NV_fence
#define GL_NV_fence 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_delete_fences_nv (glsizei, const gluint *);
GLAPI void APIENTRY gl_gen_fences_nv (glsizei, gluint *);
GLAPI glboolean APIENTRY gl_is_fence_nv (gluint);
GLAPI glboolean APIENTRY gl_test_fence_nv (gluint);
GLAPI void APIENTRY gl_get_fenceiv_nv (gluint, glenum, glint *);
GLAPI void APIENTRY gl_finish_fence_nv (gluint);
GLAPI void APIENTRY gl_set_fence_nv (gluint, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLDELETEFENCESNVPROC) (glsizei n, const gluint *fences);
typedef void (APIENTRYP PFNGLGENFENCESNVPROC) (glsizei n, gluint *fences);
typedef glboolean (APIENTRYP PFNGLISFENCENVPROC) (gluint fence);
typedef glboolean (APIENTRYP PFNGLTESTFENCENVPROC) (gluint fence);
typedef void (APIENTRYP PFNGLGETFENCEIVNVPROC) (gluint fence, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLFINISHFENCENVPROC) (gluint fence);
typedef void (APIENTRYP PFNGLSETFENCENVPROC) (gluint fence, glenum condition);
#endif

#ifndef GL_NV_evaluators
#define GL_NV_evaluators 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_map_control_points_nv (glenum, gluint, glenum, glsizei, glsizei, glint, glint, glboolean, const glvoid *);
GLAPI void APIENTRY gl_map_parameteriv_nv (glenum, glenum, const glint *);
GLAPI void APIENTRY gl_map_parameterfv_nv (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_get_map_control_points_nv (glenum, gluint, glenum, glsizei, glsizei, glboolean, glvoid *);
GLAPI void APIENTRY gl_get_map_parameteriv_nv (glenum, glenum, glint *);
GLAPI void APIENTRY gl_get_map_parameterfv_nv (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_map_attrib_parameteriv_nv (glenum, gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_map_attrib_parameterfv_nv (glenum, gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_eval_maps_nv (glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLMAPCONTROLPOINTSNVPROC) (glenum target, gluint index, glenum type, glsizei ustride, glsizei vstride, glint uorder, glint vorder, glboolean packed, const glvoid *points);
typedef void (APIENTRYP PFNGLMAPPARAMETERIVNVPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRYP PFNGLMAPPARAMETERFVNVPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLGETMAPCONTROLPOINTSNVPROC) (glenum target, gluint index, glenum type, glsizei ustride, glsizei vstride, glboolean packed, glvoid *points);
typedef void (APIENTRYP PFNGLGETMAPPARAMETERIVNVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETMAPPARAMETERFVNVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETMAPATTRIBPARAMETERIVNVPROC) (glenum target, gluint index, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETMAPATTRIBPARAMETERFVNVPROC) (glenum target, gluint index, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLEVALMAPSNVPROC) (glenum target, glenum mode);
#endif

#ifndef GL_NV_packed_depth_stencil
#define GL_NV_packed_depth_stencil 1
#endif

#ifndef GL_NV_register_combiners2
#define GL_NV_register_combiners2 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_combiner_stage_parameterfv_nv (glenum, glenum, const glfloat *);
GLAPI void APIENTRY gl_get_combiner_stage_parameterfv_nv (glenum, glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLCOMBINERSTAGEPARAMETERFVNVPROC) (glenum stage, glenum pname, const glfloat *params);
typedef void (APIENTRYP PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC) (glenum stage, glenum pname, glfloat *params);
#endif

#ifndef GL_NV_texture_compression_vtc
#define GL_NV_texture_compression_vtc 1
#endif

#ifndef GL_NV_texture_rectangle
#define GL_NV_texture_rectangle 1
#endif

#ifndef GL_NV_texture_shader
#define GL_NV_texture_shader 1
#endif

#ifndef GL_NV_texture_shader2
#define GL_NV_texture_shader2 1
#endif

#ifndef GL_NV_vertex_array_range2
#define GL_NV_vertex_array_range2 1
#endif

#ifndef GL_NV_vertex_program
#define GL_NV_vertex_program 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI glboolean APIENTRY gl_are_programs_resident_nv (glsizei, const gluint *, glboolean *);
GLAPI void APIENTRY gl_bind_program_nv (glenum, gluint);
GLAPI void APIENTRY gl_delete_programs_nv (glsizei, const gluint *);
GLAPI void APIENTRY gl_execute_program_nv (glenum, gluint, const glfloat *);
GLAPI void APIENTRY gl_gen_programs_nv (glsizei, gluint *);
GLAPI void APIENTRY gl_get_program_parameterdv_nv (glenum, gluint, glenum, gldouble *);
GLAPI void APIENTRY gl_get_program_parameterfv_nv (glenum, gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_programiv_nv (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_program_string_nv (gluint, glenum, glubyte *);
GLAPI void APIENTRY gl_get_track_matrixiv_nv (glenum, gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_vertex_attribdv_nv (gluint, glenum, gldouble *);
GLAPI void APIENTRY gl_get_vertex_attribfv_nv (gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_vertex_attribiv_nv (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_vertex_attrib_pointerv_nv (gluint, glenum, glvoid* *);
GLAPI glboolean APIENTRY gl_is_program_nv (gluint);
GLAPI void APIENTRY gl_load_program_nv (glenum, gluint, glsizei, const glubyte *);
GLAPI void APIENTRY gl_program_parameter_4d_nv (glenum, gluint, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_program_parameter_4dv_nv (glenum, gluint, const gldouble *);
GLAPI void APIENTRY gl_program_parameter_4f_nv (glenum, gluint, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_program_parameter_4fv_nv (glenum, gluint, const glfloat *);
GLAPI void APIENTRY gl_program_parameters_4dv_nv (glenum, gluint, gluint, const gldouble *);
GLAPI void APIENTRY gl_program_parameters_4fv_nv (glenum, gluint, gluint, const glfloat *);
GLAPI void APIENTRY gl_request_resident_programs_nv (glsizei, const gluint *);
GLAPI void APIENTRY gl_track_matrix_nv (glenum, gluint, glenum, glenum);
GLAPI void APIENTRY gl_vertex_attrib_pointer_nv (gluint, glint, glenum, glsizei, const glvoid *);
GLAPI void APIENTRY gl_vertex_attrib_1d_nv (gluint, gldouble);
GLAPI void APIENTRY gl_vertex_attrib_1dv_nv (gluint, const gldouble *);
GLAPI void APIENTRY gl_vertex_attrib_1f_nv (gluint, glfloat);
GLAPI void APIENTRY gl_vertex_attrib_1fv_nv (gluint, const glfloat *);
GLAPI void APIENTRY gl_vertex_attrib_1s_nv (gluint, glshort);
GLAPI void APIENTRY gl_vertex_attrib_1sv_nv (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_2d_nv (gluint, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_attrib_2dv_nv (gluint, const gldouble *);
GLAPI void APIENTRY gl_vertex_attrib_2f_nv (gluint, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_attrib_2fv_nv (gluint, const glfloat *);
GLAPI void APIENTRY gl_vertex_attrib_2s_nv (gluint, glshort, glshort);
GLAPI void APIENTRY gl_vertex_attrib_2sv_nv (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_3d_nv (gluint, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_attrib_3dv_nv (gluint, const gldouble *);
GLAPI void APIENTRY gl_vertex_attrib_3f_nv (gluint, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_attrib_3fv_nv (gluint, const glfloat *);
GLAPI void APIENTRY gl_vertex_attrib_3s_nv (gluint, glshort, glshort, glshort);
GLAPI void APIENTRY gl_vertex_attrib_3sv_nv (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_4d_nv (gluint, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_attrib_4dv_nv (gluint, const gldouble *);
GLAPI void APIENTRY gl_vertex_attrib_4f_nv (gluint, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_attrib_4fv_nv (gluint, const glfloat *);
GLAPI void APIENTRY gl_vertex_attrib_4s_nv (gluint, glshort, glshort, glshort, glshort);
GLAPI void APIENTRY gl_vertex_attrib_4sv_nv (gluint, const glshort *);
GLAPI void APIENTRY gl_vertex_attrib_4ub_nv (gluint, glubyte, glubyte, glubyte, glubyte);
GLAPI void APIENTRY gl_vertex_attrib_4ubv_nv (gluint, const glubyte *);
GLAPI void APIENTRY gl_vertex_attribs_1dv_nv (gluint, glsizei, const gldouble *);
GLAPI void APIENTRY gl_vertex_attribs_1fv_nv (gluint, glsizei, const glfloat *);
GLAPI void APIENTRY gl_vertex_attribs_1sv_nv (gluint, glsizei, const glshort *);
GLAPI void APIENTRY gl_vertex_attribs_2dv_nv (gluint, glsizei, const gldouble *);
GLAPI void APIENTRY gl_vertex_attribs_2fv_nv (gluint, glsizei, const glfloat *);
GLAPI void APIENTRY gl_vertex_attribs_2sv_nv (gluint, glsizei, const glshort *);
GLAPI void APIENTRY gl_vertex_attribs_3dv_nv (gluint, glsizei, const gldouble *);
GLAPI void APIENTRY gl_vertex_attribs_3fv_nv (gluint, glsizei, const glfloat *);
GLAPI void APIENTRY gl_vertex_attribs_3sv_nv (gluint, glsizei, const glshort *);
GLAPI void APIENTRY gl_vertex_attribs_4dv_nv (gluint, glsizei, const gldouble *);
GLAPI void APIENTRY gl_vertex_attribs_4fv_nv (gluint, glsizei, const glfloat *);
GLAPI void APIENTRY gl_vertex_attribs_4sv_nv (gluint, glsizei, const glshort *);
GLAPI void APIENTRY gl_vertex_attribs_4ubv_nv (gluint, glsizei, const glubyte *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef glboolean (APIENTRYP PFNGLAREPROGRAMSRESIDENTNVPROC) (glsizei n, const gluint *programs, glboolean *residences);
typedef void (APIENTRYP PFNGLBINDPROGRAMNVPROC) (glenum target, gluint id);
typedef void (APIENTRYP PFNGLDELETEPROGRAMSNVPROC) (glsizei n, const gluint *programs);
typedef void (APIENTRYP PFNGLEXECUTEPROGRAMNVPROC) (glenum target, gluint id, const glfloat *params);
typedef void (APIENTRYP PFNGLGENPROGRAMSNVPROC) (glsizei n, gluint *programs);
typedef void (APIENTRYP PFNGLGETPROGRAMPARAMETERDVNVPROC) (glenum target, gluint index, glenum pname, gldouble *params);
typedef void (APIENTRYP PFNGLGETPROGRAMPARAMETERFVNVPROC) (glenum target, gluint index, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETPROGRAMIVNVPROC) (gluint id, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMSTRINGNVPROC) (gluint id, glenum pname, glubyte *program);
typedef void (APIENTRYP PFNGLGETTRACKMATRIXIVNVPROC) (glenum target, gluint address, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVNVPROC) (gluint index, glenum pname, gldouble *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVNVPROC) (gluint index, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVNVPROC) (gluint index, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVNVPROC) (gluint index, glenum pname, glvoid* *pointer);
typedef glboolean (APIENTRYP PFNGLISPROGRAMNVPROC) (gluint id);
typedef void (APIENTRYP PFNGLLOADPROGRAMNVPROC) (glenum target, gluint id, glsizei len, const glubyte *program);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETER4DNVPROC) (glenum target, gluint index, gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETER4DVNVPROC) (glenum target, gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETER4FNVPROC) (glenum target, gluint index, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETER4FVNVPROC) (glenum target, gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERS4DVNVPROC) (glenum target, gluint index, gluint count, const gldouble *v);
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERS4FVNVPROC) (glenum target, gluint index, gluint count, const glfloat *v);
typedef void (APIENTRYP PFNGLREQUESTRESIDENTPROGRAMSNVPROC) (glsizei n, const gluint *programs);
typedef void (APIENTRYP PFNGLTRACKMATRIXNVPROC) (glenum target, gluint address, glenum matrix, glenum transform);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERNVPROC) (gluint index, glint fsize, glenum type, glsizei stride, const glvoid *pointer);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DNVPROC) (gluint index, gldouble x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVNVPROC) (gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FNVPROC) (gluint index, glfloat x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVNVPROC) (gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SNVPROC) (gluint index, glshort x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVNVPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DNVPROC) (gluint index, gldouble x, gldouble y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVNVPROC) (gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FNVPROC) (gluint index, glfloat x, glfloat y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVNVPROC) (gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SNVPROC) (gluint index, glshort x, glshort y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVNVPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DNVPROC) (gluint index, gldouble x, gldouble y, gldouble z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVNVPROC) (gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FNVPROC) (gluint index, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVNVPROC) (gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SNVPROC) (gluint index, glshort x, glshort y, glshort z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVNVPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DNVPROC) (gluint index, gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVNVPROC) (gluint index, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FNVPROC) (gluint index, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVNVPROC) (gluint index, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SNVPROC) (gluint index, glshort x, glshort y, glshort z, glshort w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVNVPROC) (gluint index, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBNVPROC) (gluint index, glubyte x, glubyte y, glubyte z, glubyte w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVNVPROC) (gluint index, const glubyte *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS1DVNVPROC) (gluint index, glsizei count, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS1FVNVPROC) (gluint index, glsizei count, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS1SVNVPROC) (gluint index, glsizei count, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS2DVNVPROC) (gluint index, glsizei count, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS2FVNVPROC) (gluint index, glsizei count, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS2SVNVPROC) (gluint index, glsizei count, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS3DVNVPROC) (gluint index, glsizei count, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS3FVNVPROC) (gluint index, glsizei count, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS3SVNVPROC) (gluint index, glsizei count, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4DVNVPROC) (gluint index, glsizei count, const gldouble *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4FVNVPROC) (gluint index, glsizei count, const glfloat *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4SVNVPROC) (gluint index, glsizei count, const glshort *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4UBVNVPROC) (gluint index, glsizei count, const glubyte *v);
#endif

#ifndef GL_SGIX_texture_coordinate_clamp
#define GL_SGIX_texture_coordinate_clamp 1
#endif

#ifndef GL_SGIX_scalebias_hint
#define GL_SGIX_scalebias_hint 1
#endif

#ifndef GL_OML_interlace
#define GL_OML_interlace 1
#endif

#ifndef GL_OML_subsample
#define GL_OML_subsample 1
#endif

#ifndef GL_OML_resample
#define GL_OML_resample 1
#endif

#ifndef GL_NV_copy_depth_to_color
#define GL_NV_copy_depth_to_color 1
#endif

#ifndef GL_ATI_envmap_bumpmap
#define GL_ATI_envmap_bumpmap 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_tex_bump_parameteriv_ati (glenum, const glint *);
GLAPI void APIENTRY gl_tex_bump_parameterfv_ati (glenum, const glfloat *);
GLAPI void APIENTRY gl_get_tex_bump_parameteriv_ati (glenum, glint *);
GLAPI void APIENTRY gl_get_tex_bump_parameterfv_ati (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLTEXBUMPPARAMETERIVATIPROC) (glenum pname, const glint *param);
typedef void (APIENTRYP PFNGLTEXBUMPPARAMETERFVATIPROC) (glenum pname, const glfloat *param);
typedef void (APIENTRYP PFNGLGETTEXBUMPPARAMETERIVATIPROC) (glenum pname, glint *param);
typedef void (APIENTRYP PFNGLGETTEXBUMPPARAMETERFVATIPROC) (glenum pname, glfloat *param);
#endif

#ifndef GL_ATI_fragment_shader
#define GL_ATI_fragment_shader 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI gluint APIENTRY gl_gen_fragment_shaders_ati (gluint);
GLAPI void APIENTRY gl_bind_fragment_shader_ati (gluint);
GLAPI void APIENTRY gl_delete_fragment_shader_ati (gluint);
GLAPI void APIENTRY gl_begin_fragment_shader_ati (void);
GLAPI void APIENTRY gl_end_fragment_shader_ati (void);
GLAPI void APIENTRY gl_pass_tex_coord_ati (gluint, gluint, glenum);
GLAPI void APIENTRY gl_sample_map_ati (gluint, gluint, glenum);
GLAPI void APIENTRY gl_color_fragment_op_1ati (glenum, gluint, gluint, gluint, gluint, gluint, gluint);
GLAPI void APIENTRY gl_color_fragment_op_2ati (glenum, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint);
GLAPI void APIENTRY gl_color_fragment_op_3ati (glenum, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint);
GLAPI void APIENTRY gl_alpha_fragment_op_1ati (glenum, gluint, gluint, gluint, gluint, gluint);
GLAPI void APIENTRY gl_alpha_fragment_op_2ati (glenum, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint);
GLAPI void APIENTRY gl_alpha_fragment_op_3ati (glenum, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint, gluint);
GLAPI void APIENTRY gl_set_fragment_shader_constant_ati (gluint, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef gluint (APIENTRYP PFNGLGENFRAGMENTSHADERSATIPROC) (gluint range);
typedef void (APIENTRYP PFNGLBINDFRAGMENTSHADERATIPROC) (gluint id);
typedef void (APIENTRYP PFNGLDELETEFRAGMENTSHADERATIPROC) (gluint id);
typedef void (APIENTRYP PFNGLBEGINFRAGMENTSHADERATIPROC) (void);
typedef void (APIENTRYP PFNGLENDFRAGMENTSHADERATIPROC) (void);
typedef void (APIENTRYP PFNGLPASSTEXCOORDATIPROC) (gluint dst, gluint coord, glenum swizzle);
typedef void (APIENTRYP PFNGLSAMPLEMAPATIPROC) (gluint dst, gluint interp, glenum swizzle);
typedef void (APIENTRYP PFNGLCOLORFRAGMENTOP1ATIPROC) (glenum op, gluint dst, gluint dst_mask, gluint dst_mod, gluint arg1, gluint arg_1rep, gluint arg_1mod);
typedef void (APIENTRYP PFNGLCOLORFRAGMENTOP2ATIPROC) (glenum op, gluint dst, gluint dst_mask, gluint dst_mod, gluint arg1, gluint arg_1rep, gluint arg_1mod, gluint arg2, gluint arg_2rep, gluint arg_2mod);
typedef void (APIENTRYP PFNGLCOLORFRAGMENTOP3ATIPROC) (glenum op, gluint dst, gluint dst_mask, gluint dst_mod, gluint arg1, gluint arg_1rep, gluint arg_1mod, gluint arg2, gluint arg_2rep, gluint arg_2mod, gluint arg3, gluint arg_3rep, gluint arg_3mod);
typedef void (APIENTRYP PFNGLALPHAFRAGMENTOP1ATIPROC) (glenum op, gluint dst, gluint dst_mod, gluint arg1, gluint arg_1rep, gluint arg_1mod);
typedef void (APIENTRYP PFNGLALPHAFRAGMENTOP2ATIPROC) (glenum op, gluint dst, gluint dst_mod, gluint arg1, gluint arg_1rep, gluint arg_1mod, gluint arg2, gluint arg_2rep, gluint arg_2mod);
typedef void (APIENTRYP PFNGLALPHAFRAGMENTOP3ATIPROC) (glenum op, gluint dst, gluint dst_mod, gluint arg1, gluint arg_1rep, gluint arg_1mod, gluint arg2, gluint arg_2rep, gluint arg_2mod, gluint arg3, gluint arg_3rep, gluint arg_3mod);
typedef void (APIENTRYP PFNGLSETFRAGMENTSHADERCONSTANTATIPROC) (gluint dst, const glfloat *value);
#endif

#ifndef GL_ATI_pn_triangles
#define GL_ATI_pn_triangles 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_pntrianglesi_ati (glenum, glint);
GLAPI void APIENTRY gl_pntrianglesf_ati (glenum, glfloat);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPNTRIANGLESIATIPROC) (glenum pname, glint param);
typedef void (APIENTRYP PFNGLPNTRIANGLESFATIPROC) (glenum pname, glfloat param);
#endif

#ifndef GL_ATI_vertex_array_object
#define GL_ATI_vertex_array_object 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI gluint APIENTRY gl_new_object_buffer_ati (glsizei, const glvoid *, glenum);
GLAPI glboolean APIENTRY gl_is_object_buffer_ati (gluint);
GLAPI void APIENTRY gl_update_object_buffer_ati (gluint, gluint, glsizei, const glvoid *, glenum);
GLAPI void APIENTRY gl_get_object_bufferfv_ati (gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_object_bufferiv_ati (gluint, glenum, glint *);
GLAPI void APIENTRY gl_free_object_buffer_ati (gluint);
GLAPI void APIENTRY gl_array_object_ati (glenum, glint, glenum, glsizei, gluint, gluint);
GLAPI void APIENTRY gl_get_array_objectfv_ati (glenum, glenum, glfloat *);
GLAPI void APIENTRY gl_get_array_objectiv_ati (glenum, glenum, glint *);
GLAPI void APIENTRY gl_variant_array_object_ati (gluint, glenum, glsizei, gluint, gluint);
GLAPI void APIENTRY gl_get_variant_array_objectfv_ati (gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_variant_array_objectiv_ati (gluint, glenum, glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef gluint (APIENTRYP PFNGLNEWOBJECTBUFFERATIPROC) (glsizei size, const glvoid *pointer, glenum usage);
typedef glboolean (APIENTRYP PFNGLISOBJECTBUFFERATIPROC) (gluint buffer);
typedef void (APIENTRYP PFNGLUPDATEOBJECTBUFFERATIPROC) (gluint buffer, gluint offset, glsizei size, const glvoid *pointer, glenum preserve);
typedef void (APIENTRYP PFNGLGETOBJECTBUFFERFVATIPROC) (gluint buffer, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETOBJECTBUFFERIVATIPROC) (gluint buffer, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLFREEOBJECTBUFFERATIPROC) (gluint buffer);
typedef void (APIENTRYP PFNGLARRAYOBJECTATIPROC) (glenum array, glint size, glenum type, glsizei stride, gluint buffer, gluint offset);
typedef void (APIENTRYP PFNGLGETARRAYOBJECTFVATIPROC) (glenum array, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETARRAYOBJECTIVATIPROC) (glenum array, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLVARIANTARRAYOBJECTATIPROC) (gluint id, glenum type, glsizei stride, gluint buffer, gluint offset);
typedef void (APIENTRYP PFNGLGETVARIANTARRAYOBJECTFVATIPROC) (gluint id, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETVARIANTARRAYOBJECTIVATIPROC) (gluint id, glenum pname, glint *params);
#endif

#ifndef GL_EXT_vertex_shader
#define GL_EXT_vertex_shader 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_begin_vertex_shader_ext (void);
GLAPI void APIENTRY gl_end_vertex_shader_ext (void);
GLAPI void APIENTRY gl_bind_vertex_shader_ext (gluint);
GLAPI gluint APIENTRY gl_gen_vertex_shaders_ext (gluint);
GLAPI void APIENTRY gl_delete_vertex_shader_ext (gluint);
GLAPI void APIENTRY gl_shader_op_1ext (glenum, gluint, gluint);
GLAPI void APIENTRY gl_shader_op_2ext (glenum, gluint, gluint, gluint);
GLAPI void APIENTRY gl_shader_op_3ext (glenum, gluint, gluint, gluint, gluint);
GLAPI void APIENTRY gl_swizzle_ext (gluint, gluint, glenum, glenum, glenum, glenum);
GLAPI void APIENTRY gl_write_mask_ext (gluint, gluint, glenum, glenum, glenum, glenum);
GLAPI void APIENTRY gl_insert_component_ext (gluint, gluint, gluint);
GLAPI void APIENTRY gl_extract_component_ext (gluint, gluint, gluint);
GLAPI gluint APIENTRY gl_gen_symbols_ext (glenum, glenum, glenum, gluint);
GLAPI void APIENTRY gl_set_invariant_ext (gluint, glenum, const glvoid *);
GLAPI void APIENTRY gl_set_local_constant_ext (gluint, glenum, const glvoid *);
GLAPI void APIENTRY gl_variantbv_ext (gluint, const glbyte *);
GLAPI void APIENTRY gl_variantsv_ext (gluint, const glshort *);
GLAPI void APIENTRY gl_variantiv_ext (gluint, const glint *);
GLAPI void APIENTRY gl_variantfv_ext (gluint, const glfloat *);
GLAPI void APIENTRY gl_variantdv_ext (gluint, const gldouble *);
GLAPI void APIENTRY gl_variantubv_ext (gluint, const glubyte *);
GLAPI void APIENTRY gl_variantusv_ext (gluint, const glushort *);
GLAPI void APIENTRY gl_variantuiv_ext (gluint, const gluint *);
GLAPI void APIENTRY gl_variant_pointer_ext (gluint, glenum, gluint, const glvoid *);
GLAPI void APIENTRY gl_enable_variant_client_state_ext (gluint);
GLAPI void APIENTRY gl_disable_variant_client_state_ext (gluint);
GLAPI gluint APIENTRY gl_bind_light_parameter_ext (glenum, glenum);
GLAPI gluint APIENTRY gl_bind_material_parameter_ext (glenum, glenum);
GLAPI gluint APIENTRY gl_bind_tex_gen_parameter_ext (glenum, glenum, glenum);
GLAPI gluint APIENTRY gl_bind_texture_unit_parameter_ext (glenum, glenum);
GLAPI gluint APIENTRY gl_bind_parameter_ext (glenum);
GLAPI glboolean APIENTRY gl_is_variant_enabled_ext (gluint, glenum);
GLAPI void APIENTRY gl_get_variant_booleanv_ext (gluint, glenum, glboolean *);
GLAPI void APIENTRY gl_get_variant_integerv_ext (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_variant_floatv_ext (gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_variant_pointerv_ext (gluint, glenum, glvoid* *);
GLAPI void APIENTRY gl_get_invariant_booleanv_ext (gluint, glenum, glboolean *);
GLAPI void APIENTRY gl_get_invariant_integerv_ext (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_invariant_floatv_ext (gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_local_constant_booleanv_ext (gluint, glenum, glboolean *);
GLAPI void APIENTRY gl_get_local_constant_integerv_ext (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_local_constant_floatv_ext (gluint, glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBEGINVERTEXSHADEREXTPROC) (void);
typedef void (APIENTRYP PFNGLENDVERTEXSHADEREXTPROC) (void);
typedef void (APIENTRYP PFNGLBINDVERTEXSHADEREXTPROC) (gluint id);
typedef gluint (APIENTRYP PFNGLGENVERTEXSHADERSEXTPROC) (gluint range);
typedef void (APIENTRYP PFNGLDELETEVERTEXSHADEREXTPROC) (gluint id);
typedef void (APIENTRYP PFNGLSHADEROP1EXTPROC) (glenum op, gluint res, gluint arg1);
typedef void (APIENTRYP PFNGLSHADEROP2EXTPROC) (glenum op, gluint res, gluint arg1, gluint arg2);
typedef void (APIENTRYP PFNGLSHADEROP3EXTPROC) (glenum op, gluint res, gluint arg1, gluint arg2, gluint arg3);
typedef void (APIENTRYP PFNGLSWIZZLEEXTPROC) (gluint res, gluint in, glenum out_x, glenum out_y, glenum out_z, glenum out_w);
typedef void (APIENTRYP PFNGLWRITEMASKEXTPROC) (gluint res, gluint in, glenum out_x, glenum out_y, glenum out_z, glenum out_w);
typedef void (APIENTRYP PFNGLINSERTCOMPONENTEXTPROC) (gluint res, gluint src, gluint num);
typedef void (APIENTRYP PFNGLEXTRACTCOMPONENTEXTPROC) (gluint res, gluint src, gluint num);
typedef gluint (APIENTRYP PFNGLGENSYMBOLSEXTPROC) (glenum datatype, glenum storagetype, glenum range, gluint components);
typedef void (APIENTRYP PFNGLSETINVARIANTEXTPROC) (gluint id, glenum type, const glvoid *addr);
typedef void (APIENTRYP PFNGLSETLOCALCONSTANTEXTPROC) (gluint id, glenum type, const glvoid *addr);
typedef void (APIENTRYP PFNGLVARIANTBVEXTPROC) (gluint id, const glbyte *addr);
typedef void (APIENTRYP PFNGLVARIANTSVEXTPROC) (gluint id, const glshort *addr);
typedef void (APIENTRYP PFNGLVARIANTIVEXTPROC) (gluint id, const glint *addr);
typedef void (APIENTRYP PFNGLVARIANTFVEXTPROC) (gluint id, const glfloat *addr);
typedef void (APIENTRYP PFNGLVARIANTDVEXTPROC) (gluint id, const gldouble *addr);
typedef void (APIENTRYP PFNGLVARIANTUBVEXTPROC) (gluint id, const glubyte *addr);
typedef void (APIENTRYP PFNGLVARIANTUSVEXTPROC) (gluint id, const glushort *addr);
typedef void (APIENTRYP PFNGLVARIANTUIVEXTPROC) (gluint id, const gluint *addr);
typedef void (APIENTRYP PFNGLVARIANTPOINTEREXTPROC) (gluint id, glenum type, gluint stride, const glvoid *addr);
typedef void (APIENTRYP PFNGLENABLEVARIANTCLIENTSTATEEXTPROC) (gluint id);
typedef void (APIENTRYP PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC) (gluint id);
typedef gluint (APIENTRYP PFNGLBINDLIGHTPARAMETEREXTPROC) (glenum light, glenum value);
typedef gluint (APIENTRYP PFNGLBINDMATERIALPARAMETEREXTPROC) (glenum face, glenum value);
typedef gluint (APIENTRYP PFNGLBINDTEXGENPARAMETEREXTPROC) (glenum unit, glenum coord, glenum value);
typedef gluint (APIENTRYP PFNGLBINDTEXTUREUNITPARAMETEREXTPROC) (glenum unit, glenum value);
typedef gluint (APIENTRYP PFNGLBINDPARAMETEREXTPROC) (glenum value);
typedef glboolean (APIENTRYP PFNGLISVARIANTENABLEDEXTPROC) (gluint id, glenum cap);
typedef void (APIENTRYP PFNGLGETVARIANTBOOLEANVEXTPROC) (gluint id, glenum value, glboolean *data);
typedef void (APIENTRYP PFNGLGETVARIANTINTEGERVEXTPROC) (gluint id, glenum value, glint *data);
typedef void (APIENTRYP PFNGLGETVARIANTFLOATVEXTPROC) (gluint id, glenum value, glfloat *data);
typedef void (APIENTRYP PFNGLGETVARIANTPOINTERVEXTPROC) (gluint id, glenum value, glvoid* *data);
typedef void (APIENTRYP PFNGLGETINVARIANTBOOLEANVEXTPROC) (gluint id, glenum value, glboolean *data);
typedef void (APIENTRYP PFNGLGETINVARIANTINTEGERVEXTPROC) (gluint id, glenum value, glint *data);
typedef void (APIENTRYP PFNGLGETINVARIANTFLOATVEXTPROC) (gluint id, glenum value, glfloat *data);
typedef void (APIENTRYP PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC) (gluint id, glenum value, glboolean *data);
typedef void (APIENTRYP PFNGLGETLOCALCONSTANTINTEGERVEXTPROC) (gluint id, glenum value, glint *data);
typedef void (APIENTRYP PFNGLGETLOCALCONSTANTFLOATVEXTPROC) (gluint id, glenum value, glfloat *data);
#endif

#ifndef GL_ATI_vertex_streams
#define GL_ATI_vertex_streams 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_vertex_stream_1s_ati (glenum, glshort);
GLAPI void APIENTRY gl_vertex_stream_1sv_ati (glenum, const glshort *);
GLAPI void APIENTRY gl_vertex_stream_1i_ati (glenum, glint);
GLAPI void APIENTRY gl_vertex_stream_1iv_ati (glenum, const glint *);
GLAPI void APIENTRY gl_vertex_stream_1f_ati (glenum, glfloat);
GLAPI void APIENTRY gl_vertex_stream_1fv_ati (glenum, const glfloat *);
GLAPI void APIENTRY gl_vertex_stream_1d_ati (glenum, gldouble);
GLAPI void APIENTRY gl_vertex_stream_1dv_ati (glenum, const gldouble *);
GLAPI void APIENTRY gl_vertex_stream_2s_ati (glenum, glshort, glshort);
GLAPI void APIENTRY gl_vertex_stream_2sv_ati (glenum, const glshort *);
GLAPI void APIENTRY gl_vertex_stream_2i_ati (glenum, glint, glint);
GLAPI void APIENTRY gl_vertex_stream_2iv_ati (glenum, const glint *);
GLAPI void APIENTRY gl_vertex_stream_2f_ati (glenum, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_stream_2fv_ati (glenum, const glfloat *);
GLAPI void APIENTRY gl_vertex_stream_2d_ati (glenum, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_stream_2dv_ati (glenum, const gldouble *);
GLAPI void APIENTRY gl_vertex_stream_3s_ati (glenum, glshort, glshort, glshort);
GLAPI void APIENTRY gl_vertex_stream_3sv_ati (glenum, const glshort *);
GLAPI void APIENTRY gl_vertex_stream_3i_ati (glenum, glint, glint, glint);
GLAPI void APIENTRY gl_vertex_stream_3iv_ati (glenum, const glint *);
GLAPI void APIENTRY gl_vertex_stream_3f_ati (glenum, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_stream_3fv_ati (glenum, const glfloat *);
GLAPI void APIENTRY gl_vertex_stream_3d_ati (glenum, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_stream_3dv_ati (glenum, const gldouble *);
GLAPI void APIENTRY gl_vertex_stream_4s_ati (glenum, glshort, glshort, glshort, glshort);
GLAPI void APIENTRY gl_vertex_stream_4sv_ati (glenum, const glshort *);
GLAPI void APIENTRY gl_vertex_stream_4i_ati (glenum, glint, glint, glint, glint);
GLAPI void APIENTRY gl_vertex_stream_4iv_ati (glenum, const glint *);
GLAPI void APIENTRY gl_vertex_stream_4f_ati (glenum, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_vertex_stream_4fv_ati (glenum, const glfloat *);
GLAPI void APIENTRY gl_vertex_stream_4d_ati (glenum, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_vertex_stream_4dv_ati (glenum, const gldouble *);
GLAPI void APIENTRY gl_normal_stream_3b_ati (glenum, glbyte, glbyte, glbyte);
GLAPI void APIENTRY gl_normal_stream_3bv_ati (glenum, const glbyte *);
GLAPI void APIENTRY gl_normal_stream_3s_ati (glenum, glshort, glshort, glshort);
GLAPI void APIENTRY gl_normal_stream_3sv_ati (glenum, const glshort *);
GLAPI void APIENTRY gl_normal_stream_3i_ati (glenum, glint, glint, glint);
GLAPI void APIENTRY gl_normal_stream_3iv_ati (glenum, const glint *);
GLAPI void APIENTRY gl_normal_stream_3f_ati (glenum, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_normal_stream_3fv_ati (glenum, const glfloat *);
GLAPI void APIENTRY gl_normal_stream_3d_ati (glenum, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_normal_stream_3dv_ati (glenum, const gldouble *);
GLAPI void APIENTRY gl_client_active_vertex_stream_ati (glenum);
GLAPI void APIENTRY gl_vertex_blend_envi_ati (glenum, glint);
GLAPI void APIENTRY gl_vertex_blend_envf_ati (glenum, glfloat);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLVERTEXSTREAM1SATIPROC) (glenum stream, glshort x);
typedef void (APIENTRYP PFNGLVERTEXSTREAM1SVATIPROC) (glenum stream, const glshort *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM1IATIPROC) (glenum stream, glint x);
typedef void (APIENTRYP PFNGLVERTEXSTREAM1IVATIPROC) (glenum stream, const glint *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM1FATIPROC) (glenum stream, glfloat x);
typedef void (APIENTRYP PFNGLVERTEXSTREAM1FVATIPROC) (glenum stream, const glfloat *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM1DATIPROC) (glenum stream, gldouble x);
typedef void (APIENTRYP PFNGLVERTEXSTREAM1DVATIPROC) (glenum stream, const gldouble *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM2SATIPROC) (glenum stream, glshort x, glshort y);
typedef void (APIENTRYP PFNGLVERTEXSTREAM2SVATIPROC) (glenum stream, const glshort *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM2IATIPROC) (glenum stream, glint x, glint y);
typedef void (APIENTRYP PFNGLVERTEXSTREAM2IVATIPROC) (glenum stream, const glint *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM2FATIPROC) (glenum stream, glfloat x, glfloat y);
typedef void (APIENTRYP PFNGLVERTEXSTREAM2FVATIPROC) (glenum stream, const glfloat *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM2DATIPROC) (glenum stream, gldouble x, gldouble y);
typedef void (APIENTRYP PFNGLVERTEXSTREAM2DVATIPROC) (glenum stream, const gldouble *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM3SATIPROC) (glenum stream, glshort x, glshort y, glshort z);
typedef void (APIENTRYP PFNGLVERTEXSTREAM3SVATIPROC) (glenum stream, const glshort *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM3IATIPROC) (glenum stream, glint x, glint y, glint z);
typedef void (APIENTRYP PFNGLVERTEXSTREAM3IVATIPROC) (glenum stream, const glint *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM3FATIPROC) (glenum stream, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRYP PFNGLVERTEXSTREAM3FVATIPROC) (glenum stream, const glfloat *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM3DATIPROC) (glenum stream, gldouble x, gldouble y, gldouble z);
typedef void (APIENTRYP PFNGLVERTEXSTREAM3DVATIPROC) (glenum stream, const gldouble *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM4SATIPROC) (glenum stream, glshort x, glshort y, glshort z, glshort w);
typedef void (APIENTRYP PFNGLVERTEXSTREAM4SVATIPROC) (glenum stream, const glshort *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM4IATIPROC) (glenum stream, glint x, glint y, glint z, glint w);
typedef void (APIENTRYP PFNGLVERTEXSTREAM4IVATIPROC) (glenum stream, const glint *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM4FATIPROC) (glenum stream, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLVERTEXSTREAM4FVATIPROC) (glenum stream, const glfloat *coords);
typedef void (APIENTRYP PFNGLVERTEXSTREAM4DATIPROC) (glenum stream, gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRYP PFNGLVERTEXSTREAM4DVATIPROC) (glenum stream, const gldouble *coords);
typedef void (APIENTRYP PFNGLNORMALSTREAM3BATIPROC) (glenum stream, glbyte nx, glbyte ny, glbyte nz);
typedef void (APIENTRYP PFNGLNORMALSTREAM3BVATIPROC) (glenum stream, const glbyte *coords);
typedef void (APIENTRYP PFNGLNORMALSTREAM3SATIPROC) (glenum stream, glshort nx, glshort ny, glshort nz);
typedef void (APIENTRYP PFNGLNORMALSTREAM3SVATIPROC) (glenum stream, const glshort *coords);
typedef void (APIENTRYP PFNGLNORMALSTREAM3IATIPROC) (glenum stream, glint nx, glint ny, glint nz);
typedef void (APIENTRYP PFNGLNORMALSTREAM3IVATIPROC) (glenum stream, const glint *coords);
typedef void (APIENTRYP PFNGLNORMALSTREAM3FATIPROC) (glenum stream, glfloat nx, glfloat ny, glfloat nz);
typedef void (APIENTRYP PFNGLNORMALSTREAM3FVATIPROC) (glenum stream, const glfloat *coords);
typedef void (APIENTRYP PFNGLNORMALSTREAM3DATIPROC) (glenum stream, gldouble nx, gldouble ny, gldouble nz);
typedef void (APIENTRYP PFNGLNORMALSTREAM3DVATIPROC) (glenum stream, const gldouble *coords);
typedef void (APIENTRYP PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC) (glenum stream);
typedef void (APIENTRYP PFNGLVERTEXBLENDENVIATIPROC) (glenum pname, glint param);
typedef void (APIENTRYP PFNGLVERTEXBLENDENVFATIPROC) (glenum pname, glfloat param);
#endif

#ifndef GL_ATI_element_array
#define GL_ATI_element_array 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_element_pointer_ati (glenum, const glvoid *);
GLAPI void APIENTRY gl_draw_element_array_ati (glenum, glsizei);
GLAPI void APIENTRY gl_draw_range_element_array_ati (glenum, gluint, gluint, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLELEMENTPOINTERATIPROC) (glenum type, const glvoid *pointer);
typedef void (APIENTRYP PFNGLDRAWELEMENTARRAYATIPROC) (glenum mode, glsizei count);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTARRAYATIPROC) (glenum mode, gluint start, gluint end, glsizei count);
#endif

#ifndef GL_SUN_mesh_array
#define GL_SUN_mesh_array 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_draw_mesh_arrays_sun (glenum, glint, glsizei, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLDRAWMESHARRAYSSUNPROC) (glenum mode, glint first, glsizei count, glsizei width);
#endif

#ifndef GL_SUN_slice_accum
#define GL_SUN_slice_accum 1
#endif

#ifndef GL_NV_multisample_filter_hint
#define GL_NV_multisample_filter_hint 1
#endif

#ifndef GL_NV_depth_clamp
#define GL_NV_depth_clamp 1
#endif

#ifndef GL_NV_occlusion_query
#define GL_NV_occlusion_query 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_gen_occlusion_queries_nv (glsizei, gluint *);
GLAPI void APIENTRY gl_delete_occlusion_queries_nv (glsizei, const gluint *);
GLAPI glboolean APIENTRY gl_is_occlusion_query_nv (gluint);
GLAPI void APIENTRY gl_begin_occlusion_query_nv (gluint);
GLAPI void APIENTRY gl_end_occlusion_query_nv (void);
GLAPI void APIENTRY gl_get_occlusion_queryiv_nv (gluint, glenum, glint *);
GLAPI void APIENTRY gl_get_occlusion_queryuiv_nv (gluint, glenum, gluint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLGENOCCLUSIONQUERIESNVPROC) (glsizei n, gluint *ids);
typedef void (APIENTRYP PFNGLDELETEOCCLUSIONQUERIESNVPROC) (glsizei n, const gluint *ids);
typedef glboolean (APIENTRYP PFNGLISOCCLUSIONQUERYNVPROC) (gluint id);
typedef void (APIENTRYP PFNGLBEGINOCCLUSIONQUERYNVPROC) (gluint id);
typedef void (APIENTRYP PFNGLENDOCCLUSIONQUERYNVPROC) (void);
typedef void (APIENTRYP PFNGLGETOCCLUSIONQUERYIVNVPROC) (gluint id, glenum pname, glint *params);
typedef void (APIENTRYP PFNGLGETOCCLUSIONQUERYUIVNVPROC) (gluint id, glenum pname, gluint *params);
#endif

#ifndef GL_NV_point_sprite
#define GL_NV_point_sprite 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_point_parameteri_nv (glenum, glint);
GLAPI void APIENTRY gl_point_parameteriv_nv (glenum, const glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPOINTPARAMETERINVPROC) (glenum pname, glint param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERIVNVPROC) (glenum pname, const glint *params);
#endif

#ifndef GL_NV_texture_shader3
#define GL_NV_texture_shader3 1
#endif

#ifndef GL_NV_vertex_program1_1
#define GL_NV_vertex_program1_1 1
#endif

#ifndef GL_EXT_shadow_funcs
#define GL_EXT_shadow_funcs 1
#endif

#ifndef GL_EXT_stencil_two_side
#define GL_EXT_stencil_two_side 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_active_stencil_face_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLACTIVESTENCILFACEEXTPROC) (glenum face);
#endif

#ifndef GL_ATI_text_fragment_shader
#define GL_ATI_text_fragment_shader 1
#endif

#ifndef GL_APPLE_client_storage
#define GL_APPLE_client_storage 1
#endif

#ifndef GL_APPLE_element_array
#define GL_APPLE_element_array 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_element_pointer_apple (glenum, const glvoid *);
GLAPI void APIENTRY gl_draw_element_array_apple (glenum, glint, glsizei);
GLAPI void APIENTRY gl_draw_range_element_array_apple (glenum, gluint, gluint, glint, glsizei);
GLAPI void APIENTRY gl_multi_draw_element_array_apple (glenum, const glint *, const glsizei *, glsizei);
GLAPI void APIENTRY gl_multi_draw_range_element_array_apple (glenum, gluint, gluint, const glint *, const glsizei *, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLELEMENTPOINTERAPPLEPROC) (glenum type, const glvoid *pointer);
typedef void (APIENTRYP PFNGLDRAWELEMENTARRAYAPPLEPROC) (glenum mode, glint first, glsizei count);
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC) (glenum mode, gluint start, gluint end, glint first, glsizei count);
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC) (glenum mode, const glint *first, const glsizei *count, glsizei primcount);
typedef void (APIENTRYP PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC) (glenum mode, gluint start, gluint end, const glint *first, const glsizei *count, glsizei primcount);
#endif

#ifndef GL_APPLE_fence
#define GL_APPLE_fence 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_gen_fences_apple (glsizei, gluint *);
GLAPI void APIENTRY gl_delete_fences_apple (glsizei, const gluint *);
GLAPI void APIENTRY gl_set_fence_apple (gluint);
GLAPI glboolean APIENTRY gl_is_fence_apple (gluint);
GLAPI glboolean APIENTRY gl_test_fence_apple (gluint);
GLAPI void APIENTRY gl_finish_fence_apple (gluint);
GLAPI glboolean APIENTRY gl_test_object_apple (glenum, gluint);
GLAPI void APIENTRY gl_finish_object_apple (glenum, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLGENFENCESAPPLEPROC) (glsizei n, gluint *fences);
typedef void (APIENTRYP PFNGLDELETEFENCESAPPLEPROC) (glsizei n, const gluint *fences);
typedef void (APIENTRYP PFNGLSETFENCEAPPLEPROC) (gluint fence);
typedef glboolean (APIENTRYP PFNGLISFENCEAPPLEPROC) (gluint fence);
typedef glboolean (APIENTRYP PFNGLTESTFENCEAPPLEPROC) (gluint fence);
typedef void (APIENTRYP PFNGLFINISHFENCEAPPLEPROC) (gluint fence);
typedef glboolean (APIENTRYP PFNGLTESTOBJECTAPPLEPROC) (glenum object, gluint name);
typedef void (APIENTRYP PFNGLFINISHOBJECTAPPLEPROC) (glenum object, glint name);
#endif

#ifndef GL_APPLE_vertex_array_object
#define GL_APPLE_vertex_array_object 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_bind_vertex_array_apple (gluint);
GLAPI void APIENTRY gl_delete_vertex_arrays_apple (glsizei, const gluint *);
GLAPI void APIENTRY gl_gen_vertex_arrays_apple (glsizei, const gluint *);
GLAPI glboolean APIENTRY gl_is_vertex_array_apple (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYAPPLEPROC) (gluint array);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSAPPLEPROC) (glsizei n, const gluint *arrays);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSAPPLEPROC) (glsizei n, const gluint *arrays);
typedef glboolean (APIENTRYP PFNGLISVERTEXARRAYAPPLEPROC) (gluint array);
#endif

#ifndef GL_APPLE_vertex_array_range
#define GL_APPLE_vertex_array_range 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_vertex_array_range_apple (glsizei, glvoid *);
GLAPI void APIENTRY gl_flush_vertex_array_range_apple (glsizei, glvoid *);
GLAPI void APIENTRY gl_vertex_array_parameteri_apple (glenum, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLVERTEXARRAYRANGEAPPLEPROC) (glsizei length, glvoid *pointer);
typedef void (APIENTRYP PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC) (glsizei length, glvoid *pointer);
typedef void (APIENTRYP PFNGLVERTEXARRAYPARAMETERIAPPLEPROC) (glenum pname, glint param);
#endif

#ifndef GL_APPLE_ycbcr_422
#define GL_APPLE_ycbcr_422 1
#endif

#ifndef GL_S3_s3tc
#define GL_S3_s3tc 1
#endif

#ifndef GL_ATI_draw_buffers
#define GL_ATI_draw_buffers 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_draw_buffers_ati (glsizei, const glenum *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLDRAWBUFFERSATIPROC) (glsizei n, const glenum *bufs);
#endif

#ifndef GL_ATI_texture_env_combine3
#define GL_ATI_texture_env_combine3 1
#endif

#ifndef GL_ATI_texture_float
#define GL_ATI_texture_float 1
#endif

#ifndef GL_NV_float_buffer
#define GL_NV_float_buffer 1
#endif

#ifndef GL_NV_fragment_program
#define GL_NV_fragment_program 1
/* Some NV_fragment_program entry points are shared with ARB_vertex_program. */
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_program_named_parameter_4f_nv (gluint, glsizei, const glubyte *, glfloat, glfloat, glfloat, glfloat);
GLAPI void APIENTRY gl_program_named_parameter_4d_nv (gluint, glsizei, const glubyte *, gldouble, gldouble, gldouble, gldouble);
GLAPI void APIENTRY gl_program_named_parameter_4fv_nv (gluint, glsizei, const glubyte *, const glfloat *);
GLAPI void APIENTRY gl_program_named_parameter_4dv_nv (gluint, glsizei, const glubyte *, const gldouble *);
GLAPI void APIENTRY gl_get_program_named_parameterfv_nv (gluint, glsizei, const glubyte *, glfloat *);
GLAPI void APIENTRY gl_get_program_named_parameterdv_nv (gluint, glsizei, const glubyte *, gldouble *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPROGRAMNAMEDPARAMETER4FNVPROC) (gluint id, glsizei len, const glubyte *name, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRYP PFNGLPROGRAMNAMEDPARAMETER4DNVPROC) (gluint id, glsizei len, const glubyte *name, gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRYP PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC) (gluint id, glsizei len, const glubyte *name, const glfloat *v);
typedef void (APIENTRYP PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC) (gluint id, glsizei len, const glubyte *name, const gldouble *v);
typedef void (APIENTRYP PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC) (gluint id, glsizei len, const glubyte *name, glfloat *params);
typedef void (APIENTRYP PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC) (gluint id, glsizei len, const glubyte *name, gldouble *params);
#endif

#ifndef GL_NV_half_float
#define GL_NV_half_float 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_vertex_2h_nv (glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_vertex_2hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_3h_nv (glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_vertex_3hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_4h_nv (glhalf_nv, glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_vertex_4hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_normal_3h_nv (glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_normal_3hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_color_3h_nv (glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_color_3hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_color_4h_nv (glhalf_nv, glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_color_4hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_tex_coord_1h_nv (glhalf_nv);
GLAPI void APIENTRY gl_tex_coord_1hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_tex_coord_2h_nv (glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_tex_coord_2hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_tex_coord_3h_nv (glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_tex_coord_3hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_tex_coord_4h_nv (glhalf_nv, glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_tex_coord_4hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_multi_tex_coord_1h_nv (glenum, glhalf_nv);
GLAPI void APIENTRY gl_multi_tex_coord_1hv_nv (glenum, const glhalf_nv *);
GLAPI void APIENTRY gl_multi_tex_coord_2h_nv (glenum, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_multi_tex_coord_2hv_nv (glenum, const glhalf_nv *);
GLAPI void APIENTRY gl_multi_tex_coord_3h_nv (glenum, glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_multi_tex_coord_3hv_nv (glenum, const glhalf_nv *);
GLAPI void APIENTRY gl_multi_tex_coord_4h_nv (glenum, glhalf_nv, glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_multi_tex_coord_4hv_nv (glenum, const glhalf_nv *);
GLAPI void APIENTRY gl_fog_coordh_nv (glhalf_nv);
GLAPI void APIENTRY gl_fog_coordhv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_secondary_color_3h_nv (glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_secondary_color_3hv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_weighth_nv (glhalf_nv);
GLAPI void APIENTRY gl_vertex_weighthv_nv (const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_attrib_1h_nv (gluint, glhalf_nv);
GLAPI void APIENTRY gl_vertex_attrib_1hv_nv (gluint, const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_attrib_2h_nv (gluint, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_vertex_attrib_2hv_nv (gluint, const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_attrib_3h_nv (gluint, glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_vertex_attrib_3hv_nv (gluint, const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_attrib_4h_nv (gluint, glhalf_nv, glhalf_nv, glhalf_nv, glhalf_nv);
GLAPI void APIENTRY gl_vertex_attrib_4hv_nv (gluint, const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_attribs_1hv_nv (gluint, glsizei, const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_attribs_2hv_nv (gluint, glsizei, const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_attribs_3hv_nv (gluint, glsizei, const glhalf_nv *);
GLAPI void APIENTRY gl_vertex_attribs_4hv_nv (gluint, glsizei, const glhalf_nv *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLVERTEX2HNVPROC) (glhalf_nv x, glhalf_nv y);
typedef void (APIENTRYP PFNGLVERTEX2HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEX3HNVPROC) (glhalf_nv x, glhalf_nv y, glhalf_nv z);
typedef void (APIENTRYP PFNGLVERTEX3HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEX4HNVPROC) (glhalf_nv x, glhalf_nv y, glhalf_nv z, glhalf_nv w);
typedef void (APIENTRYP PFNGLVERTEX4HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLNORMAL3HNVPROC) (glhalf_nv nx, glhalf_nv ny, glhalf_nv nz);
typedef void (APIENTRYP PFNGLNORMAL3HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLCOLOR3HNVPROC) (glhalf_nv red, glhalf_nv green, glhalf_nv blue);
typedef void (APIENTRYP PFNGLCOLOR3HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLCOLOR4HNVPROC) (glhalf_nv red, glhalf_nv green, glhalf_nv blue, glhalf_nv alpha);
typedef void (APIENTRYP PFNGLCOLOR4HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLTEXCOORD1HNVPROC) (glhalf_nv s);
typedef void (APIENTRYP PFNGLTEXCOORD1HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLTEXCOORD2HNVPROC) (glhalf_nv s, glhalf_nv t);
typedef void (APIENTRYP PFNGLTEXCOORD2HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLTEXCOORD3HNVPROC) (glhalf_nv s, glhalf_nv t, glhalf_nv r);
typedef void (APIENTRYP PFNGLTEXCOORD3HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLTEXCOORD4HNVPROC) (glhalf_nv s, glhalf_nv t, glhalf_nv r, glhalf_nv q);
typedef void (APIENTRYP PFNGLTEXCOORD4HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1HNVPROC) (glenum target, glhalf_nv s);
typedef void (APIENTRYP PFNGLMULTITEXCOORD1HVNVPROC) (glenum target, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2HNVPROC) (glenum target, glhalf_nv s, glhalf_nv t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2HVNVPROC) (glenum target, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3HNVPROC) (glenum target, glhalf_nv s, glhalf_nv t, glhalf_nv r);
typedef void (APIENTRYP PFNGLMULTITEXCOORD3HVNVPROC) (glenum target, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4HNVPROC) (glenum target, glhalf_nv s, glhalf_nv t, glhalf_nv r, glhalf_nv q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4HVNVPROC) (glenum target, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLFOGCOORDHNVPROC) (glhalf_nv fog);
typedef void (APIENTRYP PFNGLFOGCOORDHVNVPROC) (const glhalf_nv *fog);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3HNVPROC) (glhalf_nv red, glhalf_nv green, glhalf_nv blue);
typedef void (APIENTRYP PFNGLSECONDARYCOLOR3HVNVPROC) (const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEXWEIGHTHNVPROC) (glhalf_nv weight);
typedef void (APIENTRYP PFNGLVERTEXWEIGHTHVNVPROC) (const glhalf_nv *weight);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1HNVPROC) (gluint index, glhalf_nv x);
typedef void (APIENTRYP PFNGLVERTEXATTRIB1HVNVPROC) (gluint index, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2HNVPROC) (gluint index, glhalf_nv x, glhalf_nv y);
typedef void (APIENTRYP PFNGLVERTEXATTRIB2HVNVPROC) (gluint index, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3HNVPROC) (gluint index, glhalf_nv x, glhalf_nv y, glhalf_nv z);
typedef void (APIENTRYP PFNGLVERTEXATTRIB3HVNVPROC) (gluint index, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4HNVPROC) (gluint index, glhalf_nv x, glhalf_nv y, glhalf_nv z, glhalf_nv w);
typedef void (APIENTRYP PFNGLVERTEXATTRIB4HVNVPROC) (gluint index, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS1HVNVPROC) (gluint index, glsizei n, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS2HVNVPROC) (gluint index, glsizei n, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS3HVNVPROC) (gluint index, glsizei n, const glhalf_nv *v);
typedef void (APIENTRYP PFNGLVERTEXATTRIBS4HVNVPROC) (gluint index, glsizei n, const glhalf_nv *v);
#endif

#ifndef GL_NV_pixel_data_range
#define GL_NV_pixel_data_range 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_pixel_data_range_nv (glenum, glsizei, glvoid *);
GLAPI void APIENTRY gl_flush_pixel_data_range_nv (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPIXELDATARANGENVPROC) (glenum target, glsizei length, glvoid *pointer);
typedef void (APIENTRYP PFNGLFLUSHPIXELDATARANGENVPROC) (glenum target);
#endif

#ifndef GL_NV_primitive_restart
#define GL_NV_primitive_restart 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_primitive_restart_nv (void);
GLAPI void APIENTRY gl_primitive_restart_index_nv (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPRIMITIVERESTARTNVPROC) (void);
typedef void (APIENTRYP PFNGLPRIMITIVERESTARTINDEXNVPROC) (gluint index);
#endif

#ifndef GL_NV_texture_expand_normal
#define GL_NV_texture_expand_normal 1
#endif

#ifndef GL_NV_vertex_program2
#define GL_NV_vertex_program2 1
#endif

#ifndef GL_ATI_map_object_buffer
#define GL_ATI_map_object_buffer 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI glvoid* APIENTRY gl_map_object_buffer_ati (gluint);
GLAPI void APIENTRY gl_unmap_object_buffer_ati (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef glvoid* (APIENTRYP PFNGLMAPOBJECTBUFFERATIPROC) (gluint buffer);
typedef void (APIENTRYP PFNGLUNMAPOBJECTBUFFERATIPROC) (gluint buffer);
#endif

#ifndef GL_ATI_separate_stencil
#define GL_ATI_separate_stencil 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_stencil_op_separate_ati (glenum, glenum, glenum, glenum);
GLAPI void APIENTRY gl_stencil_func_separate_ati (glenum, glenum, glint, gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEATIPROC) (glenum face, glenum sfail, glenum dpfail, glenum dppass);
typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEATIPROC) (glenum frontfunc, glenum backfunc, glint ref, gluint mask);
#endif

#ifndef GL_ATI_vertex_attrib_array_object
#define GL_ATI_vertex_attrib_array_object 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_vertex_attrib_array_object_ati (gluint, glint, glenum, glboolean, glsizei, gluint, gluint);
GLAPI void APIENTRY gl_get_vertex_attrib_array_objectfv_ati (gluint, glenum, glfloat *);
GLAPI void APIENTRY gl_get_vertex_attrib_array_objectiv_ati (gluint, glenum, glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLVERTEXATTRIBARRAYOBJECTATIPROC) (gluint index, glint size, glenum type, glboolean normalized, glsizei stride, gluint buffer, gluint offset);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC) (gluint index, glenum pname, glfloat *params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC) (gluint index, glenum pname, glint *params);
#endif

#ifndef GL_EXT_depth_bounds_test
#define GL_EXT_depth_bounds_test 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_depth_bounds_ext (glclampd, glclampd);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLDEPTHBOUNDSEXTPROC) (glclampd zmin, glclampd zmax);
#endif

#ifndef GL_EXT_texture_mirror_clamp
#define GL_EXT_texture_mirror_clamp 1
#endif

#ifndef GL_EXT_blend_equation_separate
#define GL_EXT_blend_equation_separate 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY gl_blend_equation_separate_ext (glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEEXTPROC) (glenum mode_rgb, glenum mode_alpha);
#endif

#ifndef GL_MESA_pack_invert
#define GL_MESA_pack_invert 1
#endif

#ifndef GL_MESA_ycbcr_texture
#define GL_MESA_ycbcr_texture 1
#endif


#ifdef __cplusplus
}
#endif

#endif
#endif /* NO_SDL_GLEXT */

#ifndef __glext_h_
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
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
**
** Additional Notice Provisions: This software was created using the
** OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
** not been independently verified as being compliant with the OpenGL(R)
** version 1.2.1 Specification.
*/

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

/*************************************************************/

/* Header file version number, required by OpenGL ABI for Linux */
#define GL_GLEXT_VERSION 7

#ifndef GL_VERSION_1_2
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
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_RESCALE_NORMAL                 0x803A
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368
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
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
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
#define GL_TEXTURE_IMAGE_SIZE_ARB         0x86A0
#define GL_TEXTURE_COMPRESSED_ARB         0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A3
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
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_SKIP_IMAGES_EXT           0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_PACK_IMAGE_HEIGHT_EXT          0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_SKIP_IMAGES_EXT         0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E
#define GL_UNPACK_IMAGE_HEIGHT_EXT        0x806E
#define GL_TEXTURE_3D                     0x806F
#define GL_TEXTURE_3D_EXT                 0x806F
#define GL_PROXY_TEXTURE_3D               0x8070
#define GL_PROXY_TEXTURE_3D_EXT           0x8070
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_DEPTH_EXT              0x8071
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_TEXTURE_WRAP_R_EXT             0x8072
#define GL_MAX_3D_TEXTURE_SIZE            0x8073
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

#ifndef GL_SGIS_texture_border_clamp
#define GL_CLAMP_TO_BORDER_SGIS           0x812D
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

#ifndef GL_SGIS_point_parameters
#define GL_POINT_SIZE_MIN_EXT             0x8126
#define GL_POINT_SIZE_MIN_SGIS            0x8126
#define GL_POINT_SIZE_MAX_EXT             0x8127
#define GL_POINT_SIZE_MAX_SGIS            0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT  0x8128
#define GL_POINT_FADE_THRESHOLD_SIZE_SGIS 0x8128
#define GL_DISTANCE_ATTENUATION_EXT       0x8129
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
#define GL_SOURCE3_RGB_EXT                0x8583
#define GL_SOURCE4_RGB_EXT                0x8584
#define GL_SOURCE5_RGB_EXT                0x8585
#define GL_SOURCE6_RGB_EXT                0x8586
#define GL_SOURCE7_RGB_EXT                0x8587
#define GL_SOURCE0_ALPHA_EXT              0x8588
#define GL_SOURCE1_ALPHA_EXT              0x8589
#define GL_SOURCE2_ALPHA_EXT              0x858A
#define GL_SOURCE3_ALPHA_EXT              0x858B
#define GL_SOURCE4_ALPHA_EXT              0x858C
#define GL_SOURCE5_ALPHA_EXT              0x858D
#define GL_SOURCE6_ALPHA_EXT              0x858E
#define GL_SOURCE7_ALPHA_EXT              0x858F
#define GL_OPERAND0_RGB_EXT               0x8590
#define GL_OPERAND1_RGB_EXT               0x8591
#define GL_OPERAND2_RGB_EXT               0x8592
#define GL_OPERAND3_RGB_EXT               0x8593
#define GL_OPERAND4_RGB_EXT               0x8594
#define GL_OPERAND5_RGB_EXT               0x8595
#define GL_OPERAND6_RGB_EXT               0x8596
#define GL_OPERAND7_RGB_EXT               0x8597
#define GL_OPERAND0_ALPHA_EXT             0x8598
#define GL_OPERAND1_ALPHA_EXT             0x8599
#define GL_OPERAND2_ALPHA_EXT             0x859A
#define GL_OPERAND3_ALPHA_EXT             0x859B
#define GL_OPERAND4_ALPHA_EXT             0x859C
#define GL_OPERAND5_ALPHA_EXT             0x859D
#define GL_OPERAND6_ALPHA_EXT             0x859E
#define GL_OPERAND7_ALPHA_EXT             0x859F
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
#define GL_RESTART_SUN                    0x01
#define GL_REPLACE_MIDDLE_SUN             0x02
#define GL_REPLACE_OLDEST_SUN             0x03
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
#define GL_MODELVIEW_MATRIX1_EXT          0x8506
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


/*************************************************************/

#ifndef GL_VERSION_1_2
#define GL_VERSION_1_2 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_blend_color (glclampf, glclampf, glclampf, glclampf);
extern void APIENTRY gl_blend_equation (glenum);
extern void APIENTRY gl_draw_range_elements (glenum, gluint, gluint, glsizei, glenum, const glvoid *);
extern void APIENTRY gl_color_table (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_color_table_parameterfv (glenum, glenum, const glfloat *);
extern void APIENTRY gl_color_table_parameteriv (glenum, glenum, const glint *);
extern void APIENTRY gl_copy_color_table (glenum, glenum, glint, glint, glsizei);
extern void APIENTRY gl_get_color_table (glenum, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_color_table_parameterfv (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_color_table_parameteriv (glenum, glenum, glint *);
extern void APIENTRY gl_color_sub_table (glenum, glsizei, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_copy_color_sub_table (glenum, glsizei, glint, glint, glsizei);
extern void APIENTRY gl_convolution_filter_1d (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_convolution_filter_2d (glenum, glenum, glsizei, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_convolution_parameterf (glenum, glenum, glfloat);
extern void APIENTRY gl_convolution_parameterfv (glenum, glenum, const glfloat *);
extern void APIENTRY gl_convolution_parameteri (glenum, glenum, glint);
extern void APIENTRY gl_convolution_parameteriv (glenum, glenum, const glint *);
extern void APIENTRY gl_copy_convolution_filter_1d (glenum, glenum, glint, glint, glsizei);
extern void APIENTRY gl_copy_convolution_filter_2d (glenum, glenum, glint, glint, glsizei, glsizei);
extern void APIENTRY gl_get_convolution_filter (glenum, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_convolution_parameterfv (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_convolution_parameteriv (glenum, glenum, glint *);
extern void APIENTRY gl_get_separable_filter (glenum, glenum, glenum, glvoid *, glvoid *, glvoid *);
extern void APIENTRY gl_separable_filter_2d (glenum, glenum, glsizei, glsizei, glenum, glenum, const glvoid *, const glvoid *);
extern void APIENTRY gl_get_histogram (glenum, glboolean, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_histogram_parameterfv (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_histogram_parameteriv (glenum, glenum, glint *);
extern void APIENTRY gl_get_minmax (glenum, glboolean, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_minmax_parameterfv (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_minmax_parameteriv (glenum, glenum, glint *);
extern void APIENTRY gl_histogram (glenum, glsizei, glenum, glboolean);
extern void APIENTRY gl_minmax (glenum, glenum, glboolean);
extern void APIENTRY gl_reset_histogram (glenum);
extern void APIENTRY gl_reset_minmax (glenum);
extern void APIENTRY gl_tex_image_3d (glenum, glint, glint, glsizei, glsizei, glsizei, glint, glenum, glenum, const glvoid *);
extern void APIENTRY gl_tex_sub_image_3d (glenum, glint, glint, glint, glint, glsizei, glsizei, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_copy_tex_sub_image_3d (glenum, glint, glint, glint, glint, glint, glint, glsizei, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLBLENDCOLORPROC) (glclampf red, glclampf green, glclampf blue, glclampf alpha);
typedef void (APIENTRY * PFNGLBLENDEQUATIONPROC) (glenum mode);
typedef void (APIENTRY * PFNGLDRAWRANGEELEMENTSPROC) (glenum mode, gluint start, gluint end, glsizei count, glenum type, const glvoid *indices);
typedef void (APIENTRY * PFNGLCOLORTABLEPROC) (glenum target, glenum internalformat, glsizei width, glenum format, glenum type, const glvoid *table);
typedef void (APIENTRY * PFNGLCOLORTABLEPARAMETERFVPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLCOLORTABLEPARAMETERIVPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLCOPYCOLORTABLEPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPROC) (glenum target, glenum format, glenum type, glvoid *table);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLCOLORSUBTABLEPROC) (glenum target, glsizei start, glsizei count, glenum format, glenum type, const glvoid *data);
typedef void (APIENTRY * PFNGLCOPYCOLORSUBTABLEPROC) (glenum target, glsizei start, glint x, glint y, glsizei width);
typedef void (APIENTRY * PFNGLCONVOLUTIONFILTER1DPROC) (glenum target, glenum internalformat, glsizei width, glenum format, glenum type, const glvoid *image);
typedef void (APIENTRY * PFNGLCONVOLUTIONFILTER2DPROC) (glenum target, glenum internalformat, glsizei width, glsizei height, glenum format, glenum type, const glvoid *image);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERFPROC) (glenum target, glenum pname, glfloat params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERFVPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERIPROC) (glenum target, glenum pname, glint params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERIVPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER1DPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width);
typedef void (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER2DPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width, glsizei height);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONFILTERPROC) (glenum target, glenum format, glenum type, glvoid *image);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERFVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETSEPARABLEFILTERPROC) (glenum target, glenum format, glenum type, glvoid *row, glvoid *column, glvoid *span);
typedef void (APIENTRY * PFNGLSEPARABLEFILTER2DPROC) (glenum target, glenum internalformat, glsizei width, glsizei height, glenum format, glenum type, const glvoid *row, const glvoid *column);
typedef void (APIENTRY * PFNGLGETHISTOGRAMPROC) (glenum target, glboolean reset, glenum format, glenum type, glvoid *values);
typedef void (APIENTRY * PFNGLGETHISTOGRAMPARAMETERFVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETHISTOGRAMPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETMINMAXPROC) (glenum target, glboolean reset, glenum format, glenum type, glvoid *values);
typedef void (APIENTRY * PFNGLGETMINMAXPARAMETERFVPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETMINMAXPARAMETERIVPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLHISTOGRAMPROC) (glenum target, glsizei width, glenum internalformat, glboolean sink);
typedef void (APIENTRY * PFNGLMINMAXPROC) (glenum target, glenum internalformat, glboolean sink);
typedef void (APIENTRY * PFNGLRESETHISTOGRAMPROC) (glenum target);
typedef void (APIENTRY * PFNGLRESETMINMAXPROC) (glenum target);
typedef void (APIENTRY * PFNGLTEXIMAGE3DPROC) (glenum target, glint level, glint internalformat, glsizei width, glsizei height, glsizei depth, glint border, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRY * PFNGLTEXSUBIMAGE3DPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glsizei width, glsizei height, glsizei depth, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE3DPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glint x, glint y, glsizei width, glsizei height);
#endif

#ifndef GL_ARB_multitexture
#define GL_ARB_multitexture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_active_texture_arb (glenum);
extern void APIENTRY gl_client_active_texture_arb (glenum);
extern void APIENTRY gl_multi_tex_coord_1d_arb (glenum, gldouble);
extern void APIENTRY gl_multi_tex_coord_1dv_arb (glenum, const gldouble *);
extern void APIENTRY gl_multi_tex_coord_1f_arb (glenum, glfloat);
extern void APIENTRY gl_multi_tex_coord_1fv_arb (glenum, const glfloat *);
extern void APIENTRY gl_multi_tex_coord_1i_arb (glenum, glint);
extern void APIENTRY gl_multi_tex_coord_1iv_arb (glenum, const glint *);
extern void APIENTRY gl_multi_tex_coord_1s_arb (glenum, glshort);
extern void APIENTRY gl_multi_tex_coord_1sv_arb (glenum, const glshort *);
extern void APIENTRY gl_multi_tex_coord_2d_arb (glenum, gldouble, gldouble);
extern void APIENTRY gl_multi_tex_coord_2dv_arb (glenum, const gldouble *);
extern void APIENTRY gl_multi_tex_coord_2f_arb (glenum, glfloat, glfloat);
extern void APIENTRY gl_multi_tex_coord_2fv_arb (glenum, const glfloat *);
extern void APIENTRY gl_multi_tex_coord_2i_arb (glenum, glint, glint);
extern void APIENTRY gl_multi_tex_coord_2iv_arb (glenum, const glint *);
extern void APIENTRY gl_multi_tex_coord_2s_arb (glenum, glshort, glshort);
extern void APIENTRY gl_multi_tex_coord_2sv_arb (glenum, const glshort *);
extern void APIENTRY gl_multi_tex_coord_3d_arb (glenum, gldouble, gldouble, gldouble);
extern void APIENTRY gl_multi_tex_coord_3dv_arb (glenum, const gldouble *);
extern void APIENTRY gl_multi_tex_coord_3f_arb (glenum, glfloat, glfloat, glfloat);
extern void APIENTRY gl_multi_tex_coord_3fv_arb (glenum, const glfloat *);
extern void APIENTRY gl_multi_tex_coord_3i_arb (glenum, glint, glint, glint);
extern void APIENTRY gl_multi_tex_coord_3iv_arb (glenum, const glint *);
extern void APIENTRY gl_multi_tex_coord_3s_arb (glenum, glshort, glshort, glshort);
extern void APIENTRY gl_multi_tex_coord_3sv_arb (glenum, const glshort *);
extern void APIENTRY gl_multi_tex_coord_4d_arb (glenum, gldouble, gldouble, gldouble, gldouble);
extern void APIENTRY gl_multi_tex_coord_4dv_arb (glenum, const gldouble *);
extern void APIENTRY gl_multi_tex_coord_4f_arb (glenum, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_multi_tex_coord_4fv_arb (glenum, const glfloat *);
extern void APIENTRY gl_multi_tex_coord_4i_arb (glenum, glint, glint, glint, glint);
extern void APIENTRY gl_multi_tex_coord_4iv_arb (glenum, const glint *);
extern void APIENTRY gl_multi_tex_coord_4s_arb (glenum, glshort, glshort, glshort, glshort);
extern void APIENTRY gl_multi_tex_coord_4sv_arb (glenum, const glshort *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (glenum texture);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (glenum texture);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (glenum target, gldouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (glenum target, const gldouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (glenum target, glfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (glenum target, const glfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (glenum target, glint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (glenum target, const glint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (glenum target, glshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (glenum target, const glshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (glenum target, gldouble s, gldouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (glenum target, const gldouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (glenum target, glfloat s, glfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (glenum target, const glfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (glenum target, glint s, glint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (glenum target, const glint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (glenum target, glshort s, glshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (glenum target, const glshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (glenum target, gldouble s, gldouble t, gldouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (glenum target, const gldouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (glenum target, glfloat s, glfloat t, glfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (glenum target, const glfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (glenum target, glint s, glint t, glint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (glenum target, const glint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (glenum target, glshort s, glshort t, glshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (glenum target, const glshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (glenum target, gldouble s, gldouble t, gldouble r, gldouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (glenum target, const gldouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (glenum target, glfloat s, glfloat t, glfloat r, glfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (glenum target, const glfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (glenum target, glint s, glint t, glint r, glint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (glenum target, const glint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (glenum target, glshort s, glshort t, glshort r, glshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (glenum target, const glshort *v);
#endif

#ifndef GL_ARB_transpose_matrix
#define GL_ARB_transpose_matrix 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_load_transpose_matrixf_arb (const glfloat *);
extern void APIENTRY gl_load_transpose_matrixd_arb (const gldouble *);
extern void APIENTRY gl_mult_transpose_matrixf_arb (const glfloat *);
extern void APIENTRY gl_mult_transpose_matrixd_arb (const gldouble *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFARBPROC) (const glfloat *m);
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDARBPROC) (const gldouble *m);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFARBPROC) (const glfloat *m);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDARBPROC) (const gldouble *m);
#endif

#ifndef GL_ARB_multisample
#define GL_ARB_multisample 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_sample_coverage_arb (glclampf, glboolean);
extern void APIENTRY gl_sample_pass_arb (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLSAMPLECOVERAGEARBPROC) (glclampf value, glboolean invert);
typedef void (APIENTRY * PFNGLSAMPLEPASSARBPROC) (glenum pass);
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
extern void APIENTRY gl_compressed_tex_image_3darb (glenum, glint, glenum, glsizei, glsizei, glsizei, glint, glsizei, const glvoid *);
extern void APIENTRY gl_compressed_tex_image_2darb (glenum, glint, glenum, glsizei, glsizei, glint, glsizei, const glvoid *);
extern void APIENTRY gl_compressed_tex_image_1darb (glenum, glint, glenum, glsizei, glint, glsizei, const glvoid *);
extern void APIENTRY gl_compressed_tex_sub_image_3darb (glenum, glint, glint, glint, glint, glsizei, glsizei, glsizei, glenum, glsizei, const glvoid *);
extern void APIENTRY gl_compressed_tex_sub_image_2darb (glenum, glint, glint, glint, glsizei, glsizei, glenum, glsizei, const glvoid *);
extern void APIENTRY gl_compressed_tex_sub_image_1darb (glenum, glint, glint, glsizei, glenum, glsizei, const glvoid *);
extern void APIENTRY gl_get_compressed_tex_image_arb (glenum, glint, void *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DARBPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glsizei depth, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DARBPROC) (glenum target, glint level, glenum internalformat, glsizei width, glint border, glsizei image_size, const glvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glsizei width, glsizei height, glsizei depth, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) (glenum target, glint level, glint xoffset, glint yoffset, glsizei width, glsizei height, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC) (glenum target, glint level, glint xoffset, glsizei width, glenum format, glsizei image_size, const glvoid *data);
typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) (glenum target, glint level, void *img);
#endif

#ifndef GL_EXT_abgr
#define GL_EXT_abgr 1
#endif

#ifndef GL_EXT_blend_color
#define GL_EXT_blend_color 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_blend_color_ext (glclampf, glclampf, glclampf, glclampf);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLBLENDCOLOREXTPROC) (glclampf red, glclampf green, glclampf blue, glclampf alpha);
#endif

#ifndef GL_EXT_polygon_offset
#define GL_EXT_polygon_offset 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_polygon_offset_ext (glfloat, glfloat);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLPOLYGONOFFSETEXTPROC) (glfloat factor, glfloat bias);
#endif

#ifndef GL_EXT_texture
#define GL_EXT_texture 1
#endif

#ifndef GL_EXT_texture3D
#define GL_EXT_texture3D 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_tex_image_3dext (glenum, glint, glenum, glsizei, glsizei, glsizei, glint, glenum, glenum, const glvoid *);
extern void APIENTRY gl_tex_sub_image_3dext (glenum, glint, glint, glint, glint, glsizei, glsizei, glsizei, glenum, glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLTEXIMAGE3DEXTPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glsizei depth, glint border, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRY * PFNGLTEXSUBIMAGE3DEXTPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glsizei width, glsizei height, glsizei depth, glenum format, glenum type, const glvoid *pixels);
#endif

#ifndef GL_SGIS_texture_filter4
#define GL_SGIS_texture_filter4 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_get_tex_filter_func_sgis (glenum, glenum, glfloat *);
extern void APIENTRY gl_tex_filter_func_sgis (glenum, glenum, glsizei, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLGETTEXFILTERFUNCSGISPROC) (glenum target, glenum filter, glfloat *weights);
typedef void (APIENTRY * PFNGLTEXFILTERFUNCSGISPROC) (glenum target, glenum filter, glsizei n, const glfloat *weights);
#endif

#ifndef GL_EXT_subtexture
#define GL_EXT_subtexture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_tex_sub_image_1dext (glenum, glint, glint, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_tex_sub_image_2dext (glenum, glint, glint, glint, glsizei, glsizei, glenum, glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLTEXSUBIMAGE1DEXTPROC) (glenum target, glint level, glint xoffset, glsizei width, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRY * PFNGLTEXSUBIMAGE2DEXTPROC) (glenum target, glint level, glint xoffset, glint yoffset, glsizei width, glsizei height, glenum format, glenum type, const glvoid *pixels);
#endif

#ifndef GL_EXT_copy_texture
#define GL_EXT_copy_texture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_copy_tex_image_1dext (glenum, glint, glenum, glint, glint, glsizei, glint);
extern void APIENTRY gl_copy_tex_image_2dext (glenum, glint, glenum, glint, glint, glsizei, glsizei, glint);
extern void APIENTRY gl_copy_tex_sub_image_1dext (glenum, glint, glint, glint, glint, glsizei);
extern void APIENTRY gl_copy_tex_sub_image_2dext (glenum, glint, glint, glint, glint, glint, glsizei, glsizei);
extern void APIENTRY gl_copy_tex_sub_image_3dext (glenum, glint, glint, glint, glint, glint, glint, glsizei, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCOPYTEXIMAGE1DEXTPROC) (glenum target, glint level, glenum internalformat, glint x, glint y, glsizei width, glint border);
typedef void (APIENTRY * PFNGLCOPYTEXIMAGE2DEXTPROC) (glenum target, glint level, glenum internalformat, glint x, glint y, glsizei width, glsizei height, glint border);
typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE1DEXTPROC) (glenum target, glint level, glint xoffset, glint x, glint y, glsizei width);
typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE2DEXTPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint x, glint y, glsizei width, glsizei height);
typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE3DEXTPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glint x, glint y, glsizei width, glsizei height);
#endif

#ifndef GL_EXT_histogram
#define GL_EXT_histogram 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_get_histogram_ext (glenum, glboolean, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_histogram_parameterfv_ext (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_histogram_parameteriv_ext (glenum, glenum, glint *);
extern void APIENTRY gl_get_minmax_ext (glenum, glboolean, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_minmax_parameterfv_ext (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_minmax_parameteriv_ext (glenum, glenum, glint *);
extern void APIENTRY gl_histogram_ext (glenum, glsizei, glenum, glboolean);
extern void APIENTRY gl_minmax_ext (glenum, glenum, glboolean);
extern void APIENTRY gl_reset_histogram_ext (glenum);
extern void APIENTRY gl_reset_minmax_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLGETHISTOGRAMEXTPROC) (glenum target, glboolean reset, glenum format, glenum type, glvoid *values);
typedef void (APIENTRY * PFNGLGETHISTOGRAMPARAMETERFVEXTPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETHISTOGRAMPARAMETERIVEXTPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETMINMAXEXTPROC) (glenum target, glboolean reset, glenum format, glenum type, glvoid *values);
typedef void (APIENTRY * PFNGLGETMINMAXPARAMETERFVEXTPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETMINMAXPARAMETERIVEXTPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLHISTOGRAMEXTPROC) (glenum target, glsizei width, glenum internalformat, glboolean sink);
typedef void (APIENTRY * PFNGLMINMAXEXTPROC) (glenum target, glenum internalformat, glboolean sink);
typedef void (APIENTRY * PFNGLRESETHISTOGRAMEXTPROC) (glenum target);
typedef void (APIENTRY * PFNGLRESETMINMAXEXTPROC) (glenum target);
#endif

#ifndef GL_EXT_convolution
#define GL_EXT_convolution 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_convolution_filter_1dext (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_convolution_filter_2dext (glenum, glenum, glsizei, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_convolution_parameterf_ext (glenum, glenum, glfloat);
extern void APIENTRY gl_convolution_parameterfv_ext (glenum, glenum, const glfloat *);
extern void APIENTRY gl_convolution_parameteri_ext (glenum, glenum, glint);
extern void APIENTRY gl_convolution_parameteriv_ext (glenum, glenum, const glint *);
extern void APIENTRY gl_copy_convolution_filter_1dext (glenum, glenum, glint, glint, glsizei);
extern void APIENTRY gl_copy_convolution_filter_2dext (glenum, glenum, glint, glint, glsizei, glsizei);
extern void APIENTRY gl_get_convolution_filter_ext (glenum, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_convolution_parameterfv_ext (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_convolution_parameteriv_ext (glenum, glenum, glint *);
extern void APIENTRY gl_get_separable_filter_ext (glenum, glenum, glenum, glvoid *, glvoid *, glvoid *);
extern void APIENTRY gl_separable_filter_2dext (glenum, glenum, glsizei, glsizei, glenum, glenum, const glvoid *, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCONVOLUTIONFILTER1DEXTPROC) (glenum target, glenum internalformat, glsizei width, glenum format, glenum type, const glvoid *image);
typedef void (APIENTRY * PFNGLCONVOLUTIONFILTER2DEXTPROC) (glenum target, glenum internalformat, glsizei width, glsizei height, glenum format, glenum type, const glvoid *image);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERFEXTPROC) (glenum target, glenum pname, glfloat params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERFVEXTPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERIEXTPROC) (glenum target, glenum pname, glint params);
typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERIVEXTPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width);
typedef void (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width, glsizei height);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONFILTEREXTPROC) (glenum target, glenum format, glenum type, glvoid *image);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETSEPARABLEFILTEREXTPROC) (glenum target, glenum format, glenum type, glvoid *row, glvoid *column, glvoid *span);
typedef void (APIENTRY * PFNGLSEPARABLEFILTER2DEXTPROC) (glenum target, glenum internalformat, glsizei width, glsizei height, glenum format, glenum type, const glvoid *row, const glvoid *column);
#endif

#ifndef GL_EXT_color_matrix
#define GL_EXT_color_matrix 1
#endif

#ifndef GL_SGI_color_table
#define GL_SGI_color_table 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_color_table_sgi (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_color_table_parameterfv_sgi (glenum, glenum, const glfloat *);
extern void APIENTRY gl_color_table_parameteriv_sgi (glenum, glenum, const glint *);
extern void APIENTRY gl_copy_color_table_sgi (glenum, glenum, glint, glint, glsizei);
extern void APIENTRY gl_get_color_table_sgi (glenum, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_color_table_parameterfv_sgi (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_color_table_parameteriv_sgi (glenum, glenum, glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCOLORTABLESGIPROC) (glenum target, glenum internalformat, glsizei width, glenum format, glenum type, const glvoid *table);
typedef void (APIENTRY * PFNGLCOLORTABLEPARAMETERFVSGIPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLCOLORTABLEPARAMETERIVSGIPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLCOPYCOLORTABLESGIPROC) (glenum target, glenum internalformat, glint x, glint y, glsizei width);
typedef void (APIENTRY * PFNGLGETCOLORTABLESGIPROC) (glenum target, glenum format, glenum type, glvoid *table);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFVSGIPROC) (glenum target, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIVSGIPROC) (glenum target, glenum pname, glint *params);
#endif

#ifndef GL_SGIX_pixel_texture
#define GL_SGIX_pixel_texture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_pixel_tex_gen_sgix (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLPIXELTEXGENSGIXPROC) (glenum mode);
#endif

#ifndef GL_SGIS_pixel_texture
#define GL_SGIS_pixel_texture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_pixel_tex_gen_parameteri_sgis (glenum, glint);
extern void APIENTRY gl_pixel_tex_gen_parameteriv_sgis (glenum, const glint *);
extern void APIENTRY gl_pixel_tex_gen_parameterf_sgis (glenum, glfloat);
extern void APIENTRY gl_pixel_tex_gen_parameterfv_sgis (glenum, const glfloat *);
extern void APIENTRY gl_get_pixel_tex_gen_parameteriv_sgis (glenum, glint *);
extern void APIENTRY gl_get_pixel_tex_gen_parameterfv_sgis (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLPIXELTEXGENPARAMETERISGISPROC) (glenum pname, glint param);
typedef void (APIENTRY * PFNGLPIXELTEXGENPARAMETERIVSGISPROC) (glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLPIXELTEXGENPARAMETERFSGISPROC) (glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLPIXELTEXGENPARAMETERFVSGISPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC) (glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC) (glenum pname, glfloat *params);
#endif

#ifndef GL_SGIS_texture4D
#define GL_SGIS_texture4D 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_tex_image_4dsgis (glenum, glint, glenum, glsizei, glsizei, glsizei, glsizei, glint, glenum, glenum, const glvoid *);
extern void APIENTRY gl_tex_sub_image_4dsgis (glenum, glint, glint, glint, glint, glint, glsizei, glsizei, glsizei, glsizei, glenum, glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLTEXIMAGE4DSGISPROC) (glenum target, glint level, glenum internalformat, glsizei width, glsizei height, glsizei depth, glsizei size4d, glint border, glenum format, glenum type, const glvoid *pixels);
typedef void (APIENTRY * PFNGLTEXSUBIMAGE4DSGISPROC) (glenum target, glint level, glint xoffset, glint yoffset, glint zoffset, glint woffset, glsizei width, glsizei height, glsizei depth, glsizei size4d, glenum format, glenum type, const glvoid *pixels);
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
extern glboolean APIENTRY gl_are_textures_resident_ext (glsizei, const gluint *, glboolean *);
extern void APIENTRY gl_bind_texture_ext (glenum, gluint);
extern void APIENTRY gl_delete_textures_ext (glsizei, const gluint *);
extern void APIENTRY gl_gen_textures_ext (glsizei, gluint *);
extern glboolean APIENTRY gl_is_texture_ext (gluint);
extern void APIENTRY gl_prioritize_textures_ext (glsizei, const gluint *, const glclampf *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef glboolean (APIENTRY * PFNGLARETEXTURESRESIDENTEXTPROC) (glsizei n, const gluint *textures, glboolean *residences);
typedef void (APIENTRY * PFNGLBINDTEXTUREEXTPROC) (glenum target, gluint texture);
typedef void (APIENTRY * PFNGLDELETETEXTURESEXTPROC) (glsizei n, const gluint *textures);
typedef void (APIENTRY * PFNGLGENTEXTURESEXTPROC) (glsizei n, gluint *textures);
typedef glboolean (APIENTRY * PFNGLISTEXTUREEXTPROC) (gluint texture);
typedef void (APIENTRY * PFNGLPRIORITIZETEXTURESEXTPROC) (glsizei n, const gluint *textures, const glclampf *priorities);
#endif

#ifndef GL_SGIS_detail_texture
#define GL_SGIS_detail_texture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_detail_tex_func_sgis (glenum, glsizei, const glfloat *);
extern void APIENTRY gl_get_detail_tex_func_sgis (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLDETAILTEXFUNCSGISPROC) (glenum target, glsizei n, const glfloat *points);
typedef void (APIENTRY * PFNGLGETDETAILTEXFUNCSGISPROC) (glenum target, glfloat *points);
#endif

#ifndef GL_SGIS_sharpen_texture
#define GL_SGIS_sharpen_texture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_sharpen_tex_func_sgis (glenum, glsizei, const glfloat *);
extern void APIENTRY gl_get_sharpen_tex_func_sgis (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLSHARPENTEXFUNCSGISPROC) (glenum target, glsizei n, const glfloat *points);
typedef void (APIENTRY * PFNGLGETSHARPENTEXFUNCSGISPROC) (glenum target, glfloat *points);
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
extern void APIENTRY gl_sample_mask_sgis (glclampf, glboolean);
extern void APIENTRY gl_sample_pattern_sgis (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLSAMPLEMASKSGISPROC) (glclampf value, glboolean invert);
typedef void (APIENTRY * PFNGLSAMPLEPATTERNSGISPROC) (glenum pattern);
#endif

#ifndef GL_EXT_rescale_normal
#define GL_EXT_rescale_normal 1
#endif

#ifndef GL_EXT_vertex_array
#define GL_EXT_vertex_array 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_array_element_ext (glint);
extern void APIENTRY gl_color_pointer_ext (glint, glenum, glsizei, glsizei, const glvoid *);
extern void APIENTRY gl_draw_arrays_ext (glenum, glint, glsizei);
extern void APIENTRY gl_edge_flag_pointer_ext (glsizei, glsizei, const glboolean *);
extern void APIENTRY gl_get_pointerv_ext (glenum, glvoid* *);
extern void APIENTRY gl_index_pointer_ext (glenum, glsizei, glsizei, const glvoid *);
extern void APIENTRY gl_normal_pointer_ext (glenum, glsizei, glsizei, const glvoid *);
extern void APIENTRY gl_tex_coord_pointer_ext (glint, glenum, glsizei, glsizei, const glvoid *);
extern void APIENTRY gl_vertex_pointer_ext (glint, glenum, glsizei, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (glint i);
typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (glint size, glenum type, glsizei stride, glsizei count, const glvoid *pointer);
typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (glenum mode, glint first, glsizei count);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (glsizei stride, glsizei count, const glboolean *pointer);
typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (glenum pname, glvoid* *params);
typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (glenum type, glsizei stride, glsizei count, const glvoid *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (glenum type, glsizei stride, glsizei count, const glvoid *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (glint size, glenum type, glsizei stride, glsizei count, const glvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (glint size, glenum type, glsizei stride, glsizei count, const glvoid *pointer);
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
extern void APIENTRY gl_blend_equation_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLBLENDEQUATIONEXTPROC) (glenum mode);
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
extern void APIENTRY gl_sprite_parameterf_sgix (glenum, glfloat);
extern void APIENTRY gl_sprite_parameterfv_sgix (glenum, const glfloat *);
extern void APIENTRY gl_sprite_parameteri_sgix (glenum, glint);
extern void APIENTRY gl_sprite_parameteriv_sgix (glenum, const glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLSPRITEPARAMETERFSGIXPROC) (glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLSPRITEPARAMETERFVSGIXPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLSPRITEPARAMETERISGIXPROC) (glenum pname, glint param);
typedef void (APIENTRY * PFNGLSPRITEPARAMETERIVSGIXPROC) (glenum pname, const glint *params);
#endif

#ifndef GL_SGIX_texture_multi_buffer
#define GL_SGIX_texture_multi_buffer 1
#endif

#ifndef GL_EXT_point_parameters
#define GL_EXT_point_parameters 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_point_parameterf_ext (glenum, glfloat);
extern void APIENTRY gl_point_parameterfv_ext (glenum, const glfloat *);
extern void APIENTRY gl_point_parameterf_sgis (glenum, glfloat);
extern void APIENTRY gl_point_parameterfv_sgis (glenum, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC) (glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFSGISPROC) (glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVSGISPROC) (glenum pname, const glfloat *params);
#endif

#ifndef GL_SGIX_instruments
#define GL_SGIX_instruments 1
#ifdef GL_GLEXT_PROTOTYPES
extern glint APIENTRY gl_get_instruments_sgix (void);
extern void APIENTRY gl_instruments_buffer_sgix (glsizei, glint *);
extern glint APIENTRY gl_poll_instruments_sgix (glint *);
extern void APIENTRY gl_read_instruments_sgix (glint);
extern void APIENTRY gl_start_instruments_sgix (void);
extern void APIENTRY gl_stop_instruments_sgix (glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef glint (APIENTRY * PFNGLGETINSTRUMENTSSGIXPROC) (void);
typedef void (APIENTRY * PFNGLINSTRUMENTSBUFFERSGIXPROC) (glsizei size, glint *buffer);
typedef glint (APIENTRY * PFNGLPOLLINSTRUMENTSSGIXPROC) (glint *marker_p);
typedef void (APIENTRY * PFNGLREADINSTRUMENTSSGIXPROC) (glint marker);
typedef void (APIENTRY * PFNGLSTARTINSTRUMENTSSGIXPROC) (void);
typedef void (APIENTRY * PFNGLSTOPINSTRUMENTSSGIXPROC) (glint marker);
#endif

#ifndef GL_SGIX_texture_scale_bias
#define GL_SGIX_texture_scale_bias 1
#endif

#ifndef GL_SGIX_framezoom
#define GL_SGIX_framezoom 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_frame_zoom_sgix (glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLFRAMEZOOMSGIXPROC) (glint factor);
#endif

#ifndef GL_SGIX_tag_sample_buffer
#define GL_SGIX_tag_sample_buffer 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_tag_sample_buffer_sgix (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLTAGSAMPLEBUFFERSGIXPROC) (void);
#endif

#ifndef GL_SGIX_polynomial_ffd
#define GL_SGIX_polynomial_ffd 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_deformation_map_3d_sgix (glenum, gldouble, gldouble, glint, glint, gldouble, gldouble, glint, glint, gldouble, gldouble, glint, glint, const gldouble *);
extern void APIENTRY gl_deformation_map_3f_sgix (glenum, glfloat, glfloat, glint, glint, glfloat, glfloat, glint, glint, glfloat, glfloat, glint, glint, const glfloat *);
extern void APIENTRY gl_deform_sgix (glbitfield);
extern void APIENTRY gl_load_identity_deformation_map_sgix (glbitfield);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLDEFORMATIONMAP3DSGIXPROC) (glenum target, gldouble u1, gldouble u2, glint ustride, glint uorder, gldouble v1, gldouble v2, glint vstride, glint vorder, gldouble w1, gldouble w2, glint wstride, glint worder, const gldouble *points);
typedef void (APIENTRY * PFNGLDEFORMATIONMAP3FSGIXPROC) (glenum target, glfloat u1, glfloat u2, glint ustride, glint uorder, glfloat v1, glfloat v2, glint vstride, glint vorder, glfloat w1, glfloat w2, glint wstride, glint worder, const glfloat *points);
typedef void (APIENTRY * PFNGLDEFORMSGIXPROC) (glbitfield mask);
typedef void (APIENTRY * PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC) (glbitfield mask);
#endif

#ifndef GL_SGIX_reference_plane
#define GL_SGIX_reference_plane 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_reference_plane_sgix (const gldouble *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLREFERENCEPLANESGIXPROC) (const gldouble *equation);
#endif

#ifndef GL_SGIX_flush_raster
#define GL_SGIX_flush_raster 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_flush_raster_sgix (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLFLUSHRASTERSGIXPROC) (void);
#endif

#ifndef GL_SGIX_depth_texture
#define GL_SGIX_depth_texture 1
#endif

#ifndef GL_SGIS_fog_function
#define GL_SGIS_fog_function 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_fog_func_sgis (glsizei, const glfloat *);
extern void APIENTRY gl_get_fog_func_sgis (const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLFOGFUNCSGISPROC) (glsizei n, const glfloat *points);
typedef void (APIENTRY * PFNGLGETFOGFUNCSGISPROC) (const glfloat *points);
#endif

#ifndef GL_SGIX_fog_offset
#define GL_SGIX_fog_offset 1
#endif

#ifndef GL_HP_image_transform
#define GL_HP_image_transform 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_image_transform_parameteri_hp (glenum, glenum, glint);
extern void APIENTRY gl_image_transform_parameterf_hp (glenum, glenum, glfloat);
extern void APIENTRY gl_image_transform_parameteriv_hp (glenum, glenum, const glint *);
extern void APIENTRY gl_image_transform_parameterfv_hp (glenum, glenum, const glfloat *);
extern void APIENTRY gl_get_image_transform_parameteriv_hp (glenum, glenum, glint *);
extern void APIENTRY gl_get_image_transform_parameterfv_hp (glenum, glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLIMAGETRANSFORMPARAMETERIHPPROC) (glenum target, glenum pname, glint param);
typedef void (APIENTRY * PFNGLIMAGETRANSFORMPARAMETERFHPPROC) (glenum target, glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLIMAGETRANSFORMPARAMETERIVHPPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLIMAGETRANSFORMPARAMETERFVHPPROC) (glenum target, glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC) (glenum target, glenum pname, glfloat *params);
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
extern void APIENTRY gl_color_sub_table_ext (glenum, glsizei, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_copy_color_sub_table_ext (glenum, glsizei, glint, glint, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCOLORSUBTABLEEXTPROC) (glenum target, glsizei start, glsizei count, glenum format, glenum type, const glvoid *data);
typedef void (APIENTRY * PFNGLCOPYCOLORSUBTABLEEXTPROC) (glenum target, glsizei start, glint x, glint y, glsizei width);
#endif

#ifndef GL_PGI_vertex_hints
#define GL_PGI_vertex_hints 1
#endif

#ifndef GL_PGI_misc_hints
#define GL_PGI_misc_hints 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_hint_pgi (glenum, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLHINTPGIPROC) (glenum target, glint mode);
#endif

#ifndef GL_EXT_paletted_texture
#define GL_EXT_paletted_texture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_color_table_ext (glenum, glenum, glsizei, glenum, glenum, const glvoid *);
extern void APIENTRY gl_get_color_table_ext (glenum, glenum, glenum, glvoid *);
extern void APIENTRY gl_get_color_table_parameteriv_ext (glenum, glenum, glint *);
extern void APIENTRY gl_get_color_table_parameterfv_ext (glenum, glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCOLORTABLEEXTPROC) (glenum target, glenum internal_format, glsizei width, glenum format, glenum type, const glvoid *table);
typedef void (APIENTRY * PFNGLGETCOLORTABLEEXTPROC) (glenum target, glenum format, glenum type, glvoid *data);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIVEXTPROC) (glenum target, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFVEXTPROC) (glenum target, glenum pname, glfloat *params);
#endif

#ifndef GL_EXT_clip_volume_hint
#define GL_EXT_clip_volume_hint 1
#endif

#ifndef GL_SGIX_list_priority
#define GL_SGIX_list_priority 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_get_list_parameterfv_sgix (gluint, glenum, glfloat *);
extern void APIENTRY gl_get_list_parameteriv_sgix (gluint, glenum, glint *);
extern void APIENTRY gl_list_parameterf_sgix (gluint, glenum, glfloat);
extern void APIENTRY gl_list_parameterfv_sgix (gluint, glenum, const glfloat *);
extern void APIENTRY gl_list_parameteri_sgix (gluint, glenum, glint);
extern void APIENTRY gl_list_parameteriv_sgix (gluint, glenum, const glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLGETLISTPARAMETERFVSGIXPROC) (gluint list, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETLISTPARAMETERIVSGIXPROC) (gluint list, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLLISTPARAMETERFSGIXPROC) (gluint list, glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLLISTPARAMETERFVSGIXPROC) (gluint list, glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLLISTPARAMETERISGIXPROC) (gluint list, glenum pname, glint param);
typedef void (APIENTRY * PFNGLLISTPARAMETERIVSGIXPROC) (gluint list, glenum pname, const glint *params);
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
extern void APIENTRY gl_index_material_ext (glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLINDEXMATERIALEXTPROC) (glenum face, glenum mode);
#endif

#ifndef GL_EXT_index_func
#define GL_EXT_index_func 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_index_func_ext (glenum, glclampf);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLINDEXFUNCEXTPROC) (glenum func, glclampf ref);
#endif

#ifndef GL_EXT_index_array_formats
#define GL_EXT_index_array_formats 1
#endif

#ifndef GL_EXT_compiled_vertex_array
#define GL_EXT_compiled_vertex_array 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_lock_arrays_ext (glint, glsizei);
extern void APIENTRY gl_unlock_arrays_ext (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (glint first, glsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);
#endif

#ifndef GL_EXT_cull_vertex
#define GL_EXT_cull_vertex 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_cull_parameterdv_ext (glenum, gldouble *);
extern void APIENTRY gl_cull_parameterfv_ext (glenum, glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCULLPARAMETERDVEXTPROC) (glenum pname, gldouble *params);
typedef void (APIENTRY * PFNGLCULLPARAMETERFVEXTPROC) (glenum pname, glfloat *params);
#endif

#ifndef GL_SGIX_ycrcb
#define GL_SGIX_ycrcb 1
#endif

#ifndef GL_SGIX_fragment_lighting
#define GL_SGIX_fragment_lighting 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_fragment_color_material_sgix (glenum, glenum);
extern void APIENTRY gl_fragment_lightf_sgix (glenum, glenum, glfloat);
extern void APIENTRY gl_fragment_lightfv_sgix (glenum, glenum, const glfloat *);
extern void APIENTRY gl_fragment_lighti_sgix (glenum, glenum, glint);
extern void APIENTRY gl_fragment_lightiv_sgix (glenum, glenum, const glint *);
extern void APIENTRY gl_fragment_light_modelf_sgix (glenum, glfloat);
extern void APIENTRY gl_fragment_light_modelfv_sgix (glenum, const glfloat *);
extern void APIENTRY gl_fragment_light_modeli_sgix (glenum, glint);
extern void APIENTRY gl_fragment_light_modeliv_sgix (glenum, const glint *);
extern void APIENTRY gl_fragment_materialf_sgix (glenum, glenum, glfloat);
extern void APIENTRY gl_fragment_materialfv_sgix (glenum, glenum, const glfloat *);
extern void APIENTRY gl_fragment_materiali_sgix (glenum, glenum, glint);
extern void APIENTRY gl_fragment_materialiv_sgix (glenum, glenum, const glint *);
extern void APIENTRY gl_get_fragment_lightfv_sgix (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_fragment_lightiv_sgix (glenum, glenum, glint *);
extern void APIENTRY gl_get_fragment_materialfv_sgix (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_fragment_materialiv_sgix (glenum, glenum, glint *);
extern void APIENTRY gl_light_envi_sgix (glenum, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLFRAGMENTCOLORMATERIALSGIXPROC) (glenum face, glenum mode);
typedef void (APIENTRY * PFNGLFRAGMENTLIGHTFSGIXPROC) (glenum light, glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLFRAGMENTLIGHTFVSGIXPROC) (glenum light, glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLFRAGMENTLIGHTISGIXPROC) (glenum light, glenum pname, glint param);
typedef void (APIENTRY * PFNGLFRAGMENTLIGHTIVSGIXPROC) (glenum light, glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLFRAGMENTLIGHTMODELFSGIXPROC) (glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLFRAGMENTLIGHTMODELFVSGIXPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLFRAGMENTLIGHTMODELISGIXPROC) (glenum pname, glint param);
typedef void (APIENTRY * PFNGLFRAGMENTLIGHTMODELIVSGIXPROC) (glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLFRAGMENTMATERIALFSGIXPROC) (glenum face, glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLFRAGMENTMATERIALFVSGIXPROC) (glenum face, glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLFRAGMENTMATERIALISGIXPROC) (glenum face, glenum pname, glint param);
typedef void (APIENTRY * PFNGLFRAGMENTMATERIALIVSGIXPROC) (glenum face, glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLGETFRAGMENTLIGHTFVSGIXPROC) (glenum light, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETFRAGMENTLIGHTIVSGIXPROC) (glenum light, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETFRAGMENTMATERIALFVSGIXPROC) (glenum face, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETFRAGMENTMATERIALIVSGIXPROC) (glenum face, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLLIGHTENVISGIXPROC) (glenum pname, glint param);
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
extern void APIENTRY gl_draw_range_elements_ext (glenum, gluint, gluint, glsizei, glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLDRAWRANGEELEMENTSEXTPROC) (glenum mode, gluint start, gluint end, glsizei count, glenum type, const glvoid *indices);
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
extern void APIENTRY gl_apply_texture_ext (glenum);
extern void APIENTRY gl_texture_light_ext (glenum);
extern void APIENTRY gl_texture_material_ext (glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLAPPLYTEXTUREEXTPROC) (glenum mode);
typedef void (APIENTRY * PFNGLTEXTURELIGHTEXTPROC) (glenum pname);
typedef void (APIENTRY * PFNGLTEXTUREMATERIALEXTPROC) (glenum face, glenum mode);
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
extern void APIENTRY gl_async_marker_sgix (gluint);
extern glint APIENTRY gl_finish_async_sgix (gluint *);
extern glint APIENTRY gl_poll_async_sgix (gluint *);
extern gluint APIENTRY gl_gen_async_markers_sgix (glsizei);
extern void APIENTRY gl_delete_async_markers_sgix (gluint, glsizei);
extern glboolean APIENTRY gl_is_async_marker_sgix (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLASYNCMARKERSGIXPROC) (gluint marker);
typedef glint (APIENTRY * PFNGLFINISHASYNCSGIXPROC) (gluint *markerp);
typedef glint (APIENTRY * PFNGLPOLLASYNCSGIXPROC) (gluint *markerp);
typedef gluint (APIENTRY * PFNGLGENASYNCMARKERSSGIXPROC) (glsizei range);
typedef void (APIENTRY * PFNGLDELETEASYNCMARKERSSGIXPROC) (gluint marker, glsizei range);
typedef glboolean (APIENTRY * PFNGLISASYNCMARKERSGIXPROC) (gluint marker);
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
extern void APIENTRY gl_vertex_pointerv_intel (glint, glenum, const glvoid* *);
extern void APIENTRY gl_normal_pointerv_intel (glenum, const glvoid* *);
extern void APIENTRY gl_color_pointerv_intel (glint, glenum, const glvoid* *);
extern void APIENTRY gl_tex_coord_pointerv_intel (glint, glenum, const glvoid* *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLVERTEXPOINTERVINTELPROC) (glint size, glenum type, const glvoid* *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTERVINTELPROC) (glenum type, const glvoid* *pointer);
typedef void (APIENTRY * PFNGLCOLORPOINTERVINTELPROC) (glint size, glenum type, const glvoid* *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTERVINTELPROC) (glint size, glenum type, const glvoid* *pointer);
#endif

#ifndef GL_HP_occlusion_test
#define GL_HP_occlusion_test 1
#endif

#ifndef GL_EXT_pixel_transform
#define GL_EXT_pixel_transform 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_pixel_transform_parameteri_ext (glenum, glenum, glint);
extern void APIENTRY gl_pixel_transform_parameterf_ext (glenum, glenum, glfloat);
extern void APIENTRY gl_pixel_transform_parameteriv_ext (glenum, glenum, const glint *);
extern void APIENTRY gl_pixel_transform_parameterfv_ext (glenum, glenum, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLPIXELTRANSFORMPARAMETERIEXTPROC) (glenum target, glenum pname, glint param);
typedef void (APIENTRY * PFNGLPIXELTRANSFORMPARAMETERFEXTPROC) (glenum target, glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC) (glenum target, glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC) (glenum target, glenum pname, const glfloat *params);
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
extern void APIENTRY gl_secondary_color_3b_ext (glbyte, glbyte, glbyte);
extern void APIENTRY gl_secondary_color_3bv_ext (const glbyte *);
extern void APIENTRY gl_secondary_color_3d_ext (gldouble, gldouble, gldouble);
extern void APIENTRY gl_secondary_color_3dv_ext (const gldouble *);
extern void APIENTRY gl_secondary_color_3f_ext (glfloat, glfloat, glfloat);
extern void APIENTRY gl_secondary_color_3fv_ext (const glfloat *);
extern void APIENTRY gl_secondary_color_3i_ext (glint, glint, glint);
extern void APIENTRY gl_secondary_color_3iv_ext (const glint *);
extern void APIENTRY gl_secondary_color_3s_ext (glshort, glshort, glshort);
extern void APIENTRY gl_secondary_color_3sv_ext (const glshort *);
extern void APIENTRY gl_secondary_color_3ub_ext (glubyte, glubyte, glubyte);
extern void APIENTRY gl_secondary_color_3ubv_ext (const glubyte *);
extern void APIENTRY gl_secondary_color_3ui_ext (gluint, gluint, gluint);
extern void APIENTRY gl_secondary_color_3uiv_ext (const gluint *);
extern void APIENTRY gl_secondary_color_3us_ext (glushort, glushort, glushort);
extern void APIENTRY gl_secondary_color_3usv_ext (const glushort *);
extern void APIENTRY gl_secondary_color_pointer_ext (glint, glenum, glsizei, glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BEXTPROC) (glbyte red, glbyte green, glbyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BVEXTPROC) (const glbyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DEXTPROC) (gldouble red, gldouble green, gldouble blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DVEXTPROC) (const gldouble *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FEXTPROC) (glfloat red, glfloat green, glfloat blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FVEXTPROC) (const glfloat *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IEXTPROC) (glint red, glint green, glint blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IVEXTPROC) (const glint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SEXTPROC) (glshort red, glshort green, glshort blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SVEXTPROC) (const glshort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBEXTPROC) (glubyte red, glubyte green, glubyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBVEXTPROC) (const glubyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIEXTPROC) (gluint red, gluint green, gluint blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIVEXTPROC) (const gluint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USEXTPROC) (glushort red, glushort green, glushort blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USVEXTPROC) (const glushort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTEREXTPROC) (glint size, glenum type, glsizei stride, glvoid *pointer);
#endif

#ifndef GL_EXT_texture_perturb_normal
#define GL_EXT_texture_perturb_normal 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_texture_normal_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLTEXTURENORMALEXTPROC) (glenum mode);
#endif

#ifndef GL_EXT_multi_draw_arrays
#define GL_EXT_multi_draw_arrays 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_multi_draw_arrays_ext (glenum, glint *, glsizei *, glsizei);
extern void APIENTRY gl_multi_draw_elements_ext (glenum, const glsizei *, glenum, const glvoid* *, glsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLMULTIDRAWARRAYSEXTPROC) (glenum mode, glint *first, glsizei *count, glsizei primcount);
typedef void (APIENTRY * PFNGLMULTIDRAWELEMENTSEXTPROC) (glenum mode, const glsizei *count, glenum type, const glvoid* *indices, glsizei primcount);
#endif

#ifndef GL_EXT_fog_coord
#define GL_EXT_fog_coord 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_fog_coordf_ext (glfloat);
extern void APIENTRY gl_fog_coordfv_ext (const glfloat *);
extern void APIENTRY gl_fog_coordd_ext (gldouble);
extern void APIENTRY gl_fog_coorddv_ext (const gldouble *);
extern void APIENTRY gl_fog_coord_pointer_ext (glenum, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (glfloat coord);
typedef void (APIENTRY * PFNGLFOGCOORDFVEXTPROC) (const glfloat *coord);
typedef void (APIENTRY * PFNGLFOGCOORDDEXTPROC) (gldouble coord);
typedef void (APIENTRY * PFNGLFOGCOORDDVEXTPROC) (const gldouble *coord);
typedef void (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC) (glenum type, glsizei stride, const glvoid *pointer);
#endif

#ifndef GL_REND_screen_coordinates
#define GL_REND_screen_coordinates 1
#endif

#ifndef GL_EXT_coordinate_frame
#define GL_EXT_coordinate_frame 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_tangent_3b_ext (glbyte, glbyte, glbyte);
extern void APIENTRY gl_tangent_3bv_ext (const glbyte *);
extern void APIENTRY gl_tangent_3d_ext (gldouble, gldouble, gldouble);
extern void APIENTRY gl_tangent_3dv_ext (const gldouble *);
extern void APIENTRY gl_tangent_3f_ext (glfloat, glfloat, glfloat);
extern void APIENTRY gl_tangent_3fv_ext (const glfloat *);
extern void APIENTRY gl_tangent_3i_ext (glint, glint, glint);
extern void APIENTRY gl_tangent_3iv_ext (const glint *);
extern void APIENTRY gl_tangent_3s_ext (glshort, glshort, glshort);
extern void APIENTRY gl_tangent_3sv_ext (const glshort *);
extern void APIENTRY gl_binormal_3b_ext (glbyte, glbyte, glbyte);
extern void APIENTRY gl_binormal_3bv_ext (const glbyte *);
extern void APIENTRY gl_binormal_3d_ext (gldouble, gldouble, gldouble);
extern void APIENTRY gl_binormal_3dv_ext (const gldouble *);
extern void APIENTRY gl_binormal_3f_ext (glfloat, glfloat, glfloat);
extern void APIENTRY gl_binormal_3fv_ext (const glfloat *);
extern void APIENTRY gl_binormal_3i_ext (glint, glint, glint);
extern void APIENTRY gl_binormal_3iv_ext (const glint *);
extern void APIENTRY gl_binormal_3s_ext (glshort, glshort, glshort);
extern void APIENTRY gl_binormal_3sv_ext (const glshort *);
extern void APIENTRY gl_tangent_pointer_ext (glenum, glsizei, const glvoid *);
extern void APIENTRY gl_binormal_pointer_ext (glenum, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLTANGENT3BEXTPROC) (glbyte tx, glbyte ty, glbyte tz);
typedef void (APIENTRY * PFNGLTANGENT3BVEXTPROC) (const glbyte *v);
typedef void (APIENTRY * PFNGLTANGENT3DEXTPROC) (gldouble tx, gldouble ty, gldouble tz);
typedef void (APIENTRY * PFNGLTANGENT3DVEXTPROC) (const gldouble *v);
typedef void (APIENTRY * PFNGLTANGENT3FEXTPROC) (glfloat tx, glfloat ty, glfloat tz);
typedef void (APIENTRY * PFNGLTANGENT3FVEXTPROC) (const glfloat *v);
typedef void (APIENTRY * PFNGLTANGENT3IEXTPROC) (glint tx, glint ty, glint tz);
typedef void (APIENTRY * PFNGLTANGENT3IVEXTPROC) (const glint *v);
typedef void (APIENTRY * PFNGLTANGENT3SEXTPROC) (glshort tx, glshort ty, glshort tz);
typedef void (APIENTRY * PFNGLTANGENT3SVEXTPROC) (const glshort *v);
typedef void (APIENTRY * PFNGLBINORMAL3BEXTPROC) (glbyte bx, glbyte by, glbyte bz);
typedef void (APIENTRY * PFNGLBINORMAL3BVEXTPROC) (const glbyte *v);
typedef void (APIENTRY * PFNGLBINORMAL3DEXTPROC) (gldouble bx, gldouble by, gldouble bz);
typedef void (APIENTRY * PFNGLBINORMAL3DVEXTPROC) (const gldouble *v);
typedef void (APIENTRY * PFNGLBINORMAL3FEXTPROC) (glfloat bx, glfloat by, glfloat bz);
typedef void (APIENTRY * PFNGLBINORMAL3FVEXTPROC) (const glfloat *v);
typedef void (APIENTRY * PFNGLBINORMAL3IEXTPROC) (glint bx, glint by, glint bz);
typedef void (APIENTRY * PFNGLBINORMAL3IVEXTPROC) (const glint *v);
typedef void (APIENTRY * PFNGLBINORMAL3SEXTPROC) (glshort bx, glshort by, glshort bz);
typedef void (APIENTRY * PFNGLBINORMAL3SVEXTPROC) (const glshort *v);
typedef void (APIENTRY * PFNGLTANGENTPOINTEREXTPROC) (glenum type, glsizei stride, const glvoid *pointer);
typedef void (APIENTRY * PFNGLBINORMALPOINTEREXTPROC) (glenum type, glsizei stride, const glvoid *pointer);
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
extern void APIENTRY gl_finish_texture_sunx (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLFINISHTEXTURESUNXPROC) (void);
#endif

#ifndef GL_SUN_global_alpha
#define GL_SUN_global_alpha 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_global_alpha_factorb_sun (glbyte);
extern void APIENTRY gl_global_alpha_factors_sun (glshort);
extern void APIENTRY gl_global_alpha_factori_sun (glint);
extern void APIENTRY gl_global_alpha_factorf_sun (glfloat);
extern void APIENTRY gl_global_alpha_factord_sun (gldouble);
extern void APIENTRY gl_global_alpha_factorub_sun (glubyte);
extern void APIENTRY gl_global_alpha_factorus_sun (glushort);
extern void APIENTRY gl_global_alpha_factorui_sun (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLGLOBALALPHAFACTORBSUNPROC) (glbyte factor);
typedef void (APIENTRY * PFNGLGLOBALALPHAFACTORSSUNPROC) (glshort factor);
typedef void (APIENTRY * PFNGLGLOBALALPHAFACTORISUNPROC) (glint factor);
typedef void (APIENTRY * PFNGLGLOBALALPHAFACTORFSUNPROC) (glfloat factor);
typedef void (APIENTRY * PFNGLGLOBALALPHAFACTORDSUNPROC) (gldouble factor);
typedef void (APIENTRY * PFNGLGLOBALALPHAFACTORUBSUNPROC) (glubyte factor);
typedef void (APIENTRY * PFNGLGLOBALALPHAFACTORUSSUNPROC) (glushort factor);
typedef void (APIENTRY * PFNGLGLOBALALPHAFACTORUISUNPROC) (gluint factor);
#endif

#ifndef GL_SUN_triangle_list
#define GL_SUN_triangle_list 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_replacement_codeui_sun (gluint);
extern void APIENTRY gl_replacement_codeus_sun (glushort);
extern void APIENTRY gl_replacement_codeub_sun (glubyte);
extern void APIENTRY gl_replacement_codeuiv_sun (const gluint *);
extern void APIENTRY gl_replacement_codeusv_sun (const glushort *);
extern void APIENTRY gl_replacement_codeubv_sun (const glubyte *);
extern void APIENTRY gl_replacement_code_pointer_sun (glenum, glsizei, const glvoid* *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUISUNPROC) (gluint code);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUSSUNPROC) (glushort code);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUBSUNPROC) (glubyte code);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUIVSUNPROC) (const gluint *code);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUSVSUNPROC) (const glushort *code);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUBVSUNPROC) (const glubyte *code);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEPOINTERSUNPROC) (glenum type, glsizei stride, const glvoid* *pointer);
#endif

#ifndef GL_SUN_vertex
#define GL_SUN_vertex 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_color_4ub_vertex_2f_sun (glubyte, glubyte, glubyte, glubyte, glfloat, glfloat);
extern void APIENTRY gl_color_4ub_vertex_2fv_sun (const glubyte *, const glfloat *);
extern void APIENTRY gl_color_4ub_vertex_3f_sun (glubyte, glubyte, glubyte, glubyte, glfloat, glfloat, glfloat);
extern void APIENTRY gl_color_4ub_vertex_3fv_sun (const glubyte *, const glfloat *);
extern void APIENTRY gl_color_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_color_3f_vertex_3fv_sun (const glfloat *, const glfloat *);
extern void APIENTRY gl_normal_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_normal_3f_vertex_3fv_sun (const glfloat *, const glfloat *);
extern void APIENTRY gl_color_4f_normal_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_color_4f_normal_3f_vertex_3fv_sun (const glfloat *, const glfloat *, const glfloat *);
extern void APIENTRY gl_tex_coord_2f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_tex_coord_2f_vertex_3fv_sun (const glfloat *, const glfloat *);
extern void APIENTRY gl_tex_coord_4f_vertex_4f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_tex_coord_4f_vertex_4fv_sun (const glfloat *, const glfloat *);
extern void APIENTRY gl_tex_coord_2f_color_4ub_vertex_3f_sun (glfloat, glfloat, glubyte, glubyte, glubyte, glubyte, glfloat, glfloat, glfloat);
extern void APIENTRY gl_tex_coord_2f_color_4ub_vertex_3fv_sun (const glfloat *, const glubyte *, const glfloat *);
extern void APIENTRY gl_tex_coord_2f_color_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_tex_coord_2f_color_3f_vertex_3fv_sun (const glfloat *, const glfloat *, const glfloat *);
extern void APIENTRY gl_tex_coord_2f_normal_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_tex_coord_2f_normal_3f_vertex_3fv_sun (const glfloat *, const glfloat *, const glfloat *);
extern void APIENTRY gl_tex_coord_2f_color_4f_normal_3f_vertex_3f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_tex_coord_2f_color_4f_normal_3f_vertex_3fv_sun (const glfloat *, const glfloat *, const glfloat *, const glfloat *);
extern void APIENTRY gl_tex_coord_4f_color_4f_normal_3f_vertex_4f_sun (glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_tex_coord_4f_color_4f_normal_3f_vertex_4fv_sun (const glfloat *, const glfloat *, const glfloat *, const glfloat *);
extern void APIENTRY gl_replacement_codeui_vertex_3f_sun (glenum, glfloat, glfloat, glfloat);
extern void APIENTRY gl_replacement_codeui_vertex_3fv_sun (const glenum *, const glfloat *);
extern void APIENTRY gl_replacement_codeui_color_4ub_vertex_3f_sun (glenum, glubyte, glubyte, glubyte, glubyte, glfloat, glfloat, glfloat);
extern void APIENTRY gl_replacement_codeui_color_4ub_vertex_3fv_sun (const glenum *, const glubyte *, const glfloat *);
extern void APIENTRY gl_replacement_codeui_color_3f_vertex_3f_sun (glenum, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_replacement_codeui_color_3f_vertex_3fv_sun (const glenum *, const glfloat *, const glfloat *);
extern void APIENTRY gl_replacement_codeui_normal_3f_vertex_3f_sun (glenum, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_replacement_codeui_normal_3f_vertex_3fv_sun (const glenum *, const glfloat *, const glfloat *);
extern void APIENTRY gl_replacement_codeui_color_4f_normal_3f_vertex_3f_sun (glenum, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_replacement_codeui_color_4f_normal_3f_vertex_3fv_sun (const glenum *, const glfloat *, const glfloat *, const glfloat *);
extern void APIENTRY gl_replacement_codeui_tex_coord_2f_vertex_3f_sun (glenum, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_replacement_codeui_tex_coord_2f_vertex_3fv_sun (const glenum *, const glfloat *, const glfloat *);
extern void APIENTRY gl_replacement_codeui_tex_coord_2f_normal_3f_vertex_3f_sun (glenum, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_replacement_codeui_tex_coord_2f_normal_3f_vertex_3fv_sun (const glenum *, const glfloat *, const glfloat *, const glfloat *);
extern void APIENTRY gl_replacement_codeui_tex_coord_2f_color_4f_normal_3f_vertex_3f_sun (glenum, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_replacement_codeui_tex_coord_2f_color_4f_normal_3f_vertex_3fv_sun (const glenum *, const glfloat *, const glfloat *, const glfloat *, const glfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCOLOR4UBVERTEX2FSUNPROC) (glubyte r, glubyte g, glubyte b, glubyte a, glfloat x, glfloat y);
typedef void (APIENTRY * PFNGLCOLOR4UBVERTEX2FVSUNPROC) (const glubyte *c, const glfloat *v);
typedef void (APIENTRY * PFNGLCOLOR4UBVERTEX3FSUNPROC) (glubyte r, glubyte g, glubyte b, glubyte a, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLCOLOR4UBVERTEX3FVSUNPROC) (const glubyte *c, const glfloat *v);
typedef void (APIENTRY * PFNGLCOLOR3FVERTEX3FSUNPROC) (glfloat r, glfloat g, glfloat b, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLCOLOR3FVERTEX3FVSUNPROC) (const glfloat *c, const glfloat *v);
typedef void (APIENTRY * PFNGLNORMAL3FVERTEX3FSUNPROC) (glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLNORMAL3FVERTEX3FVSUNPROC) (const glfloat *n, const glfloat *v);
typedef void (APIENTRY * PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC) (glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC) (const glfloat *c, const glfloat *n, const glfloat *v);
typedef void (APIENTRY * PFNGLTEXCOORD2FVERTEX3FSUNPROC) (glfloat s, glfloat t, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLTEXCOORD2FVERTEX3FVSUNPROC) (const glfloat *tc, const glfloat *v);
typedef void (APIENTRY * PFNGLTEXCOORD4FVERTEX4FSUNPROC) (glfloat s, glfloat t, glfloat p, glfloat q, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRY * PFNGLTEXCOORD4FVERTEX4FVSUNPROC) (const glfloat *tc, const glfloat *v);
typedef void (APIENTRY * PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC) (glfloat s, glfloat t, glubyte r, glubyte g, glubyte b, glubyte a, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC) (const glfloat *tc, const glubyte *c, const glfloat *v);
typedef void (APIENTRY * PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC) (glfloat s, glfloat t, glfloat r, glfloat g, glfloat b, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC) (const glfloat *tc, const glfloat *c, const glfloat *v);
typedef void (APIENTRY * PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC) (glfloat s, glfloat t, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC) (const glfloat *tc, const glfloat *n, const glfloat *v);
typedef void (APIENTRY * PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC) (glfloat s, glfloat t, glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC) (const glfloat *tc, const glfloat *c, const glfloat *n, const glfloat *v);
typedef void (APIENTRY * PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC) (glfloat s, glfloat t, glfloat p, glfloat q, glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRY * PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC) (const glfloat *tc, const glfloat *c, const glfloat *n, const glfloat *v);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC) (glenum rc, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC) (const glenum *rc, const glfloat *v);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC) (glenum rc, glubyte r, glubyte g, glubyte b, glubyte a, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC) (const glenum *rc, const glubyte *c, const glfloat *v);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC) (glenum rc, glfloat r, glfloat g, glfloat b, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC) (const glenum *rc, const glfloat *c, const glfloat *v);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC) (glenum rc, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC) (const glenum *rc, const glfloat *n, const glfloat *v);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC) (glenum rc, glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC) (const glenum *rc, const glfloat *c, const glfloat *n, const glfloat *v);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC) (glenum rc, glfloat s, glfloat t, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC) (const glenum *rc, const glfloat *tc, const glfloat *v);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC) (glenum rc, glfloat s, glfloat t, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC) (const glenum *rc, const glfloat *tc, const glfloat *n, const glfloat *v);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC) (glenum rc, glfloat s, glfloat t, glfloat r, glfloat g, glfloat b, glfloat a, glfloat nx, glfloat ny, glfloat nz, glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC) (const glenum *rc, const glfloat *tc, const glfloat *c, const glfloat *n, const glfloat *v);
#endif

#ifndef GL_EXT_blend_func_separate
#define GL_EXT_blend_func_separate 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_blend_func_separate_ext (glenum, glenum, glenum, glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLBLENDFUNCSEPARATEEXTPROC) (glenum sfactor_rgb, glenum dfactor_rgb, glenum sfactor_alpha, glenum dfactor_alpha);
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
extern void APIENTRY gl_vertex_weightf_ext (glfloat);
extern void APIENTRY gl_vertex_weightfv_ext (const glfloat *);
extern void APIENTRY gl_vertex_weight_pointer_ext (glsizei, glenum, glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLVERTEXWEIGHTFEXTPROC) (glfloat weight);
typedef void (APIENTRY * PFNGLVERTEXWEIGHTFVEXTPROC) (const glfloat *weight);
typedef void (APIENTRY * PFNGLVERTEXWEIGHTPOINTEREXTPROC) (glsizei size, glenum type, glsizei stride, const glvoid *pointer);
#endif

#ifndef GL_NV_light_max_exponent
#define GL_NV_light_max_exponent 1
#endif

#ifndef GL_NV_vertex_array_range
#define GL_NV_vertex_array_range 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_flush_vertex_array_range_nv (void);
extern void APIENTRY gl_vertex_array_range_nv (glsizei, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);
typedef void (APIENTRY * PFNGLVERTEXARRAYRANGENVPROC) (glsizei size, const glvoid *pointer);
#endif

#ifndef GL_NV_register_combiners
#define GL_NV_register_combiners 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_combiner_parameterfv_nv (glenum, const glfloat *);
extern void APIENTRY gl_combiner_parameterf_nv (glenum, glfloat);
extern void APIENTRY gl_combiner_parameteriv_nv (glenum, const glint *);
extern void APIENTRY gl_combiner_parameteri_nv (glenum, glint);
extern void APIENTRY gl_combiner_input_nv (glenum, glenum, glenum, glenum, glenum, glenum);
extern void APIENTRY gl_combiner_output_nv (glenum, glenum, glenum, glenum, glenum, glenum, glenum, glboolean, glboolean, glboolean);
extern void APIENTRY gl_final_combiner_input_nv (glenum, glenum, glenum, glenum);
extern void APIENTRY gl_get_combiner_input_parameterfv_nv (glenum, glenum, glenum, glenum, glfloat *);
extern void APIENTRY gl_get_combiner_input_parameteriv_nv (glenum, glenum, glenum, glenum, glint *);
extern void APIENTRY gl_get_combiner_output_parameterfv_nv (glenum, glenum, glenum, glfloat *);
extern void APIENTRY gl_get_combiner_output_parameteriv_nv (glenum, glenum, glenum, glint *);
extern void APIENTRY gl_get_final_combiner_input_parameterfv_nv (glenum, glenum, glfloat *);
extern void APIENTRY gl_get_final_combiner_input_parameteriv_nv (glenum, glenum, glint *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFVNVPROC) (glenum pname, const glfloat *params);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFNVPROC) (glenum pname, glfloat param);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERIVNVPROC) (glenum pname, const glint *params);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERINVPROC) (glenum pname, glint param);
typedef void (APIENTRY * PFNGLCOMBINERINPUTNVPROC) (glenum stage, glenum portion, glenum variable, glenum input, glenum mapping, glenum component_usage);
typedef void (APIENTRY * PFNGLCOMBINEROUTPUTNVPROC) (glenum stage, glenum portion, glenum ab_output, glenum cd_output, glenum sum_output, glenum scale, glenum bias, glboolean ab_dot_product, glboolean cd_dot_product, glboolean mux_sum);
typedef void (APIENTRY * PFNGLFINALCOMBINERINPUTNVPROC) (glenum variable, glenum input, glenum mapping, glenum component_usage);
typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (glenum stage, glenum portion, glenum variable, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (glenum stage, glenum portion, glenum variable, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (glenum stage, glenum portion, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (glenum stage, glenum portion, glenum pname, glint *params);
typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (glenum variable, glenum pname, glfloat *params);
typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (glenum variable, glenum pname, glint *params);
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
extern void APIENTRY gl_resize_buffers_mesa (void);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLRESIZEBUFFERSMESAPROC) (void);
#endif

#ifndef GL_MESA_window_pos
#define GL_MESA_window_pos 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_window_pos_2d_mesa (gldouble, gldouble);
extern void APIENTRY gl_window_pos_2dv_mesa (const gldouble *);
extern void APIENTRY gl_window_pos_2f_mesa (glfloat, glfloat);
extern void APIENTRY gl_window_pos_2fv_mesa (const glfloat *);
extern void APIENTRY gl_window_pos_2i_mesa (glint, glint);
extern void APIENTRY gl_window_pos_2iv_mesa (const glint *);
extern void APIENTRY gl_window_pos_2s_mesa (glshort, glshort);
extern void APIENTRY gl_window_pos_2sv_mesa (const glshort *);
extern void APIENTRY gl_window_pos_3d_mesa (gldouble, gldouble, gldouble);
extern void APIENTRY gl_window_pos_3dv_mesa (const gldouble *);
extern void APIENTRY gl_window_pos_3f_mesa (glfloat, glfloat, glfloat);
extern void APIENTRY gl_window_pos_3fv_mesa (const glfloat *);
extern void APIENTRY gl_window_pos_3i_mesa (glint, glint, glint);
extern void APIENTRY gl_window_pos_3iv_mesa (const glint *);
extern void APIENTRY gl_window_pos_3s_mesa (glshort, glshort, glshort);
extern void APIENTRY gl_window_pos_3sv_mesa (const glshort *);
extern void APIENTRY gl_window_pos_4d_mesa (gldouble, gldouble, gldouble, gldouble);
extern void APIENTRY gl_window_pos_4dv_mesa (const gldouble *);
extern void APIENTRY gl_window_pos_4f_mesa (glfloat, glfloat, glfloat, glfloat);
extern void APIENTRY gl_window_pos_4fv_mesa (const glfloat *);
extern void APIENTRY gl_window_pos_4i_mesa (glint, glint, glint, glint);
extern void APIENTRY gl_window_pos_4iv_mesa (const glint *);
extern void APIENTRY gl_window_pos_4s_mesa (glshort, glshort, glshort, glshort);
extern void APIENTRY gl_window_pos_4sv_mesa (const glshort *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLWINDOWPOS2DMESAPROC) (gldouble x, gldouble y);
typedef void (APIENTRY * PFNGLWINDOWPOS2DVMESAPROC) (const gldouble *v);
typedef void (APIENTRY * PFNGLWINDOWPOS2FMESAPROC) (glfloat x, glfloat y);
typedef void (APIENTRY * PFNGLWINDOWPOS2FVMESAPROC) (const glfloat *v);
typedef void (APIENTRY * PFNGLWINDOWPOS2IMESAPROC) (glint x, glint y);
typedef void (APIENTRY * PFNGLWINDOWPOS2IVMESAPROC) (const glint *v);
typedef void (APIENTRY * PFNGLWINDOWPOS2SMESAPROC) (glshort x, glshort y);
typedef void (APIENTRY * PFNGLWINDOWPOS2SVMESAPROC) (const glshort *v);
typedef void (APIENTRY * PFNGLWINDOWPOS3DMESAPROC) (gldouble x, gldouble y, gldouble z);
typedef void (APIENTRY * PFNGLWINDOWPOS3DVMESAPROC) (const gldouble *v);
typedef void (APIENTRY * PFNGLWINDOWPOS3FMESAPROC) (glfloat x, glfloat y, glfloat z);
typedef void (APIENTRY * PFNGLWINDOWPOS3FVMESAPROC) (const glfloat *v);
typedef void (APIENTRY * PFNGLWINDOWPOS3IMESAPROC) (glint x, glint y, glint z);
typedef void (APIENTRY * PFNGLWINDOWPOS3IVMESAPROC) (const glint *v);
typedef void (APIENTRY * PFNGLWINDOWPOS3SMESAPROC) (glshort x, glshort y, glshort z);
typedef void (APIENTRY * PFNGLWINDOWPOS3SVMESAPROC) (const glshort *v);
typedef void (APIENTRY * PFNGLWINDOWPOS4DMESAPROC) (gldouble x, gldouble y, gldouble z, gldouble w);
typedef void (APIENTRY * PFNGLWINDOWPOS4DVMESAPROC) (const gldouble *v);
typedef void (APIENTRY * PFNGLWINDOWPOS4FMESAPROC) (glfloat x, glfloat y, glfloat z, glfloat w);
typedef void (APIENTRY * PFNGLWINDOWPOS4FVMESAPROC) (const glfloat *v);
typedef void (APIENTRY * PFNGLWINDOWPOS4IMESAPROC) (glint x, glint y, glint z, glint w);
typedef void (APIENTRY * PFNGLWINDOWPOS4IVMESAPROC) (const glint *v);
typedef void (APIENTRY * PFNGLWINDOWPOS4SMESAPROC) (glshort x, glshort y, glshort z, glshort w);
typedef void (APIENTRY * PFNGLWINDOWPOS4SVMESAPROC) (const glshort *v);
#endif

#ifndef GL_IBM_cull_vertex
#define GL_IBM_cull_vertex 1
#endif

#ifndef GL_IBM_multimode_draw_arrays
#define GL_IBM_multimode_draw_arrays 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_multi_mode_draw_arrays_ibm (glenum, const glint *, const glsizei *, glsizei, glint);
extern void APIENTRY gl_multi_mode_draw_elements_ibm (const glenum *, const glsizei *, glenum, const glvoid* *, glsizei, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLMULTIMODEDRAWARRAYSIBMPROC) (glenum mode, const glint *first, const glsizei *count, glsizei primcount, glint modestride);
typedef void (APIENTRY * PFNGLMULTIMODEDRAWELEMENTSIBMPROC) (const glenum *mode, const glsizei *count, glenum type, const glvoid* *indices, glsizei primcount, glint modestride);
#endif

#ifndef GL_IBM_vertex_array_lists
#define GL_IBM_vertex_array_lists 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_color_pointer_list_ibm (glint, glenum, glint, const glvoid* *, glint);
extern void APIENTRY gl_secondary_color_pointer_list_ibm (glint, glenum, glint, const glvoid* *, glint);
extern void APIENTRY gl_edge_flag_pointer_list_ibm (glint, const glboolean* *, glint);
extern void APIENTRY gl_fog_coord_pointer_list_ibm (glenum, glint, const glvoid* *, glint);
extern void APIENTRY gl_index_pointer_list_ibm (glenum, glint, const glvoid* *, glint);
extern void APIENTRY gl_normal_pointer_list_ibm (glenum, glint, const glvoid* *, glint);
extern void APIENTRY gl_tex_coord_pointer_list_ibm (glint, glenum, glint, const glvoid* *, glint);
extern void APIENTRY gl_vertex_pointer_list_ibm (glint, glenum, glint, const glvoid* *, glint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLCOLORPOINTERLISTIBMPROC) (glint size, glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTERLISTIBMPROC) (glint size, glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTERLISTIBMPROC) (glint stride, const glboolean* *pointer, glint ptrstride);
typedef void (APIENTRY * PFNGLFOGCOORDPOINTERLISTIBMPROC) (glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRY * PFNGLINDEXPOINTERLISTIBMPROC) (glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRY * PFNGLNORMALPOINTERLISTIBMPROC) (glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTERLISTIBMPROC) (glint size, glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
typedef void (APIENTRY * PFNGLVERTEXPOINTERLISTIBMPROC) (glint size, glenum type, glint stride, const glvoid* *pointer, glint ptrstride);
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
extern void APIENTRY gl_tbuffer_mask_3dfx (gluint);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLTBUFFERMASK3DFXPROC) (gluint mask);
#endif

#ifndef GL_EXT_multisample
#define GL_EXT_multisample 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_sample_mask_ext (glclampf, glboolean);
extern void APIENTRY gl_sample_pattern_ext (glenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLSAMPLEMASKEXTPROC) (glclampf value, glboolean invert);
typedef void (APIENTRY * PFNGLSAMPLEPATTERNEXTPROC) (glenum pattern);
#endif

#ifndef GL_SGI_vertex_preclip
#define GL_SGI_vertex_preclip 1
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
extern void APIENTRY gl_texture_color_mask_sgis (glboolean, glboolean, glboolean, glboolean);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLTEXTURECOLORMASKSGISPROC) (glboolean red, glboolean green, glboolean blue, glboolean alpha);
#endif

#ifndef GL_SGIX_igloo_interface
#define GL_SGIX_igloo_interface 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY gl_igloo_interface_sgix (glenum, const glvoid *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLIGLOOINTERFACESGIXPROC) (glenum pname, const glvoid *params);
#endif


#ifdef __cplusplus
}
#endif

#endif

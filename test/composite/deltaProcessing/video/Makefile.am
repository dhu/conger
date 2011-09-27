#####################  Makefile.am for delta processing tests  #####################
#

MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = test_adjacent_light  test_adjacent_heavy test_batch_light test_batch_heavy test_chunk_light test_chunk_heavy test_face_detect_light \
	test_face_detect_heavy test_recurse_light test_recurse_heavy test_sample_light test_sample_heavy test_spatial_light test_spatial_heavy 

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../../@BOREALISROOT@

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@  -lSDL -lSDL_image

LDADD = -L$(BOREALIS_SRC)/modules/common             -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util               -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                              \
           -I$(BOREALIS_SRC)/modules/common                               \
           -I$(BOREALIS_SRC)/modules/util                                 \
           -I$(BOREALIS_SRC)/modules/rpc                                  \
           -I$(BOREALIS_SRC)/modules/catalog                              \
           -I$(BOREALIS_SRC)/modules/queryProcessor                       \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr              \
           -I$(BOREALIS_SRC)/external/array

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la        \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la      \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la


            ###########################################
            
test_adjacent_light_SOURCES      = ${test_adjacent_light_DEPENDENCIES}              \
                               		adjacent_light.cc

test_adjacent_light_DEPENDENCIES = AdjacentMarshal.cc  AdjacentMarshal.h

test_adjacent_light_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_adjacent_light_DEPENDENCIES}: Adjacent.xml AdjacentDeploy.xml
	marshal  Adjacent.xml AdjacentDeploy.xml
	

            ###########################################

test_adjacent_heavy_SOURCES      = ${test_adjacent_heavy_DEPENDENCIES}              \
                               		adjacent_heavy.cc

test_adjacent_heavy_DEPENDENCIES = AdjacentMarshal.cc  AdjacentMarshal.h

test_adjacent_heavy_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_adjacent_heavy_DEPENDENCIES}: Adjacent.xml AdjacentDeploy.xml
	marshal  Adjacent.xml AdjacentDeploy.xml
	

            ###########################################

test_batch_light_SOURCES      = ${test_batch_light_DEPENDENCIES}              \
                               		batch_light.cc

test_batch_light_DEPENDENCIES = BatchMarshal.cc  BatchMarshal.h

test_batch_light_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_batch_light_DEPENDENCIES}: Batch.xml BatchDeploy.xml
	marshal  Batch.xml BatchDeploy.xml
	

            ###########################################

test_batch_heavy_SOURCES      = ${test_batch_heavy_DEPENDENCIES}              \
                               		batch_heavy.cc

test_batch_heavy_DEPENDENCIES = BatchMarshal.cc  BatchMarshal.h

test_batch_heavy_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_batch_heavy_DEPENDENCIES}: Batch.xml BatchDeploy.xml
	marshal  Batch.xml BatchDeploy.xml
	

            ###########################################


test_chunk_light_SOURCES      = ${test_chunk_light_DEPENDENCIES}              \
                               		chunk_light.cc

test_chunk_light_DEPENDENCIES = ChunkMarshal.cc  ChunkMarshal.h

test_chunk_light_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_chunk_light_DEPENDENCIES}: Chunk.xml ChunkDeploy.xml
	marshal  Chunk.xml ChunkDeploy.xml
	

            ###########################################

test_chunk_heavy_SOURCES      = ${test_chunk_heavy_DEPENDENCIES}              \
                               		chunk_heavy.cc

test_chunk_heavy_DEPENDENCIES = ChunkMarshal.cc  ChunkMarshal.h

test_chunk_heavy_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_chunk_heavy_DEPENDENCIES}: Chunk.xml ChunkDeploy.xml
	marshal  Chunk.xml ChunkDeploy.xml
	

            ###########################################

test_face_detect_light_SOURCES      = ${test_face_detect_light_DEPENDENCIES}              \
                               		face_detect_light.cc

test_face_detect_light_DEPENDENCIES = FacedetectMarshal.cc  FacedetectMarshal.h

test_face_detect_light_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_face_detect_light_DEPENDENCIES}: FaceDetect.xml FaceDetectDeploy.xml
	marshal  FaceDetect.xml FaceDetectDeploy.xml
	

            ###########################################

test_face_detect_heavy_SOURCES      = ${test_face_detect_heavy_DEPENDENCIES}              \
                               		face_detect_heavy.cc

test_face_detect_heavy_DEPENDENCIES = FacedetectMarshal.cc  FacedetectMarshal.h

test_face_detect_heavy_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_face_detect_heavy_DEPENDENCIES}: FaceDetect.xml FaceDetectDeploy.xml
	marshal  FaceDetect.xml FaceDetectDeploy.xml
	

            ###########################################


test_recurse_light_SOURCES      = ${test_recurse_light_DEPENDENCIES}              \
                               		recurse_light.cc

test_recurse_light_DEPENDENCIES = RecurseMarshal.cc  RecurseMarshal.h

test_recurse_light_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_recurse_light_DEPENDENCIES}: Recurse.xml RecurseDeploy.xml
	marshal  Recurse.xml RecurseDeploy.xml
	

            ###########################################

test_recurse_heavy_SOURCES      = ${test_recurse_heavy_DEPENDENCIES}              \
                               		recurse_heavy.cc

test_recurse_heavy_DEPENDENCIES = RecurseMarshal.cc  RecurseMarshal.h

test_recurse_heavy_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_recurse_heavy_DEPENDENCIES}: Recurse.xml RecurseDeploy.xml
	marshal  Recurse.xml RecurseDeploy.xml
	
            ###########################################


test_sample_light_SOURCES      = ${test_sample_light_DEPENDENCIES}              \
                               		sample_light.cc

test_sample_light_DEPENDENCIES = SampleMarshal.cc  SampleMarshal.h

test_sample_light_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_sample_light_DEPENDENCIES}: Sample.xml SampleDeploy.xml
	marshal  Sample.xml SampleDeploy.xml
	

            ###########################################

test_sample_heavy_SOURCES      = ${test_sample_heavy_DEPENDENCIES}              \
                               		sample_heavy.cc

test_sample_heavy_DEPENDENCIES = SampleMarshal.cc  SampleMarshal.h

test_sample_heavy_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_sample_heavy_DEPENDENCIES}: Sample.xml SampleDeploy.xml
	marshal  Sample.xml SampleDeploy.xml

            ###########################################


test_spatial_light_SOURCES      = ${test_spatial_light_DEPENDENCIES}              \
                               		spatial_light.cc

test_spatial_light_DEPENDENCIES = SpatialMarshal.cc  SpatialMarshal.h

test_spatial_light_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_spatial_light_DEPENDENCIES}: Spatial.xml SpatialDeploy.xml
	marshal  Spatial.xml SpatialDeploy.xml
	

            ###########################################

test_spatial_heavy_SOURCES      = ${test_spatial_heavy_DEPENDENCIES}              \
                               		spatial_heavy.cc

test_spatial_heavy_DEPENDENCIES = SpatialMarshal.cc  SpatialMarshal.h

test_spatial_heavy_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_spatial_heavy_DEPENDENCIES}: Spatial.xml SpatialDeploy.xml
	marshal  Spatial.xml SpatialDeploy.xml
	
            ###########################################
            
            

CLEANFILES = ${test_adjacent_light_DEPENDENCIES}                  \
             ${test_adjacent_heavy_DEPENDENCIES}                 \
             ${test_batch_light_DEPENDENCIES}                  \
             ${test_batch_heavy_DEPENDENCIES}                 \
             ${test_chunk_light_DEPENDENCIES}                  \
             ${test_chunk_heavy_DEPENDENCIES}                 \
             ${test_face_detect_light_DEPENDENCIES}                  \
             ${test_face_detect_heavy_DEPENDENCIES}                 \
             ${test_recurse_light_DEPENDENCIES}                  \
             ${test_recurse_heavy_DEPENDENCIES}                 \
             ${test_sample_light_DEPENDENCIES}                  \
             ${test_sample_heavy_DEPENDENCIES}                 \
             ${test_spatial_light_DEPENDENCIES}                  \
             ${test_spatial_heavy_DEPENDENCIES}                 

#
###################  end Makefile.am for delta processing tests  ###################

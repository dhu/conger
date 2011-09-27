#####################  Makefile.am for delta processing tests  #####################
#

MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = test_uncompressed test_motion_vector test_null_suppress test_downsample test_rle

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../../@BOREALISROOT@

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@  -lSDL -lSDL_image -lMagick++

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
            
test_uncompressed_SOURCES      = ${test_uncompressed_DEPENDENCIES}              \
				uncompressed.cc

test_uncompressed_DEPENDENCIES = UncompressedMarshal.cc  UncompressedMarshal.h

test_uncompressed_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_uncompressed_DEPENDENCIES}: Uncompressed.xml UncompressedDeploy.xml 
	marshal  Uncompressed.xml UncompressedDeploy.xml

            ###########################################
test_motion_vector_SOURCES      = ${test_motion_vector_DEPENDENCIES}              \
				motion_vector.cc

test_motion_vector_DEPENDENCIES = MotionvectorMarshal.cc  MotionvectorMarshal.h

test_motion_vector_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_motion_vector_DEPENDENCIES}: MotionVector.xml MotionVectorDeploy.xml 
	marshal  MotionVector.xml MotionVectorDeploy.xml

            ###########################################
test_null_suppress_SOURCES      = ${test_null_suppress_DEPENDENCIES}              \
				null_suppress.cc

test_null_suppress_DEPENDENCIES = NullsuppressMarshal.cc  NullsuppressMarshal.h

test_null_suppress_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_null_suppress_DEPENDENCIES}: NullSuppress.xml NullSuppressDeploy.xml 
	marshal  NullSuppress.xml NullSuppressDeploy.xml
            ###########################################

test_downsample_SOURCES      = ${test_downsample_DEPENDENCIES}              \
				downsample.cc

test_downsample_DEPENDENCIES = DownsampleMarshal.cc  DownsampleMarshal.h

test_downsample_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_downsample_DEPENDENCIES}: Downsample.xml DownsampleDeploy.xml 
	marshal  Downsample.xml DownsampleDeploy.xml
            ###########################################
test_rle_SOURCES      = ${test_rle_DEPENDENCIES}              \
				run_length.cc

test_rle_DEPENDENCIES = RunlengthMarshal.cc  RunlengthMarshal.h

test_rle_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${test_rle_DEPENDENCIES}: RunLength.xml RunLengthDeploy.xml 
	marshal  RunLength.xml RunLengthDeploy.xml
            ###########################################            

CLEANFILES = ${test_null_suppress_DEPENDENCIES test_downsample_DEPENDENCIES test_rle_DEPENDENCIES test_motion_vector_DEPENDENCIES test_uncompressed_DEPENDENCIES}                 

#
###################  end Makefile.am for delta processing tests  ###################

###################  Makefile.am for a sample system test  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = process_video

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../@BOREALISROOT@

LDADD_EXTRAS = -ldl -ldl   -lSDL -lSDL_image   @XERCESCLIB@   @NMSTLLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common             -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util               -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@ @NMSTLINCLUDE@                                                 \
           -I$(BOREALIS_SRC)/modules/common                               \
           -I$(BOREALIS_SRC)/modules/util                                 \
           -I$(BOREALIS_SRC)/modules/rpc                                  \
           -I$(BOREALIS_SRC)/modules/catalog                              \
           -I$(BOREALIS_SRC)/modules/queryProcessor                       \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la                \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la  \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la

process_video_SOURCES      = ${process_video_DEPENDENCIES}            \
                               process_video.cc

process_video_DEPENDENCIES = VideoMarshal.cc  VideoMarshal.h

process_video_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${process_video_DEPENDENCIES}: Video.xml
	marshal  Video.xml  VideoDeploy.xml


CLEANFILES = ${process_video_DEPENDENCIES}

#
##################  end Makefile.am for a sample system test  ##################

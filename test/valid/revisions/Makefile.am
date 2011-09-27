###################  Makefile.am for a sample system test  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = 	validate_upstream_revisionfilter	\
	validate_upstream_revisionmap		\
	validate_upstream_revisionaggregate	\
	validate_downstream_revisionaggregate	

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common             -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util               -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@ @NMSTLINCLUDE@                                \
           -I$(BOREALIS_SRC)/modules/common                               \
           -I$(BOREALIS_SRC)/modules/util                                 \
           -I$(BOREALIS_SRC)/modules/rpc                                  \
           -I$(BOREALIS_SRC)/modules/catalog                              \
           -I$(BOREALIS_SRC)/modules/queryProcessor                       \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la                \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la  		  \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la


##################################################################################
validate_upstream_revisionfilter_SOURCES      = ${validate_upstream_revisionfilter_DEPENDENCIES}            \
				validate_upstream_revisionfilter.cc
validate_upstream_revisionfilter_DEPENDENCIES = Validate_Upstream_RevisionfilterMarshal.cc  Validate_Upstream_RevisionfilterMarshal.h
validate_upstream_revisionfilter_LDADD        = $(LDADD_EXTRAS) $(LDADD)
${validate_upstream_revisionfilter_DEPENDENCIES}: validate_upstream_revisionfilter.xml
				marshal  validate_upstream_revisionfilter.xml
##################################################################################


##################################################################################
validate_upstream_revisionmap_SOURCES      = ${validate_upstream_revisionmap_DEPENDENCIES}            \
				validate_upstream_revisionmap.cc
validate_upstream_revisionmap_DEPENDENCIES = Validate_Upstream_RevisionmapMarshal.cc  Validate_Upstream_RevisionmapMarshal.h
validate_upstream_revisionmap_LDADD        = $(LDADD_EXTRAS) $(LDADD)
${validate_upstream_revisionmap_DEPENDENCIES}: validate_upstream_revisionmap.xml
				marshal  validate_upstream_revisionmap.xml
##################################################################################


##################################################################################
validate_upstream_revisionaggregate_SOURCES      = ${validate_upstream_revisionaggregate_DEPENDENCIES}            \
				validate_upstream_revisionaggregate.cc
validate_upstream_revisionaggregate_DEPENDENCIES = Validate_Upstream_RevisionaggregateMarshal.cc  Validate_Upstream_RevisionaggregateMarshal.h
validate_upstream_revisionaggregate_LDADD        = $(LDADD_EXTRAS) $(LDADD)
${validate_upstream_revisionaggregate_DEPENDENCIES}: validate_upstream_revisionaggregate.xml
				marshal  validate_upstream_revisionaggregate.xml
##################################################################################


##################################################################################
validate_downstream_revisionaggregate_SOURCES      = ${validate_downstream_revisionaggregate_DEPENDENCIES}            \
				validate_downstream_revisionaggregate.cc
validate_downstream_revisionaggregate_DEPENDENCIES = Validate_Downstream_RevisionaggregateMarshal.cc  Validate_Downstream_RevisionaggregateMarshal.h
validate_downstream_revisionaggregate_LDADD        = $(LDADD_EXTRAS) $(LDADD)
${validate_downstream_revisionaggregate_DEPENDENCIES}: validate_downstream_revisionaggregate.xml
				marshal  validate_downstream_revisionaggregate.xml
##################################################################################



CLEANFILES = 	${validate_upstream_revisionfilter_DEPENDENCIES}	\
		${validate_upstream_revisionmap_DEPENDENCIES}		\
		${validate_upstream_revisionaggregate_DEPENDENCIES}	\
		${validate_downstream_revisionaggregate_DEPENDENCIES}	


#
##################  end Makefile.am for a sample system test  ##################

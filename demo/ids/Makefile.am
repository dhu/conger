MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

if COND_UDB

MAIN = ids

bin_PROGRAMS = $(MAIN)

LDADD_EXTRAS = -ldl  @XERCESCLIB@

LDADD = -L../@BOREALISROOT@/modules/common     -lborealiscommon      \
        -L../@BOREALISROOT@/modules/util       -lborealisutil        \
        @NMSTLLIB@   $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                         \
           -I../@BOREALISROOT@/modules/queryProcessor/statsMgr \
           -I../@BOREALISROOT@/modules/common                        \
           -I../@BOREALISROOT@/modules/util                          \
           -I../@BOREALISROOT@/modules/rpc                           \
           -I../@BOREALISROOT@/modules/catalog


                 ###########################################

TUPLESOURCE = InputTuple.h PacketTuple.h

ids_GENERATED = IdsMarshal.cc IdsMarshal.h

ids_SOURCES = args.h ${ids_DEPENDENCIES} ids.cc
ids_LDADD = $(LDADD)
ids_DEPENDENCIES = ${ids_GENERATED} 

${ids_GENERATED}: ids.xml  ids_deploy.xml
	marshal  ids.xml  ids_deploy.xml


                 ###########################################

CLEANFILES = ${ids_GENERATED}

endif

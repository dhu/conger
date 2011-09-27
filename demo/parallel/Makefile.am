###################  Makefile.am for parallel array tests  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_pass  validate_worker

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../@BOREALISROOT@

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common             -lborealiscommon     \
        -L$(BOREALIS_SRC)/modules/util               -lborealisutil       \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                              \
           -I$(BOREALIS_SRC)/modules/common                               \
           -I$(BOREALIS_SRC)/modules/util                                 \
           -I$(BOREALIS_SRC)/modules/rpc                                  \
           -I$(BOREALIS_SRC)/modules/catalog                              \
           -I$(BOREALIS_SRC)/modules/queryProcessor                       \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la        \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la      \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la

validate_pass_SOURCES      = PassMarshal.cc                 \
                             validate_pass.cc

validate_pass_LDADD        = $(LDADD_EXTRAS) $(LDADD)

            ###########################################

validate_worker_SOURCES      = WorkerMarshalCommon.cc       \
                               validate_worker.cc

validate_worker_LDADD        = $(LDADD_EXTRAS) $(LDADD)


#
#################  end Makefile.am for parallel array tests  ###################

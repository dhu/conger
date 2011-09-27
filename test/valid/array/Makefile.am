#####################  Makefile.am for simple array tests  #####################
#
MYFLAGS =
AM_CFLAGS   = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = validate_dense  validate_sparse   #validate_chunk

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

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
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr

DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la        \
               $(BOREALIS_SRC)/modules/catalog/libborealiscatalog.la      \
               $(BOREALIS_SRC)/modules/queryProcessor/expr/libborealisqpexpr.la

validate_dense_SOURCES      = ${validate_dense_DEPENDENCIES}              \
                               validate_dense.cc

validate_dense_DEPENDENCIES = DenseMarshal.cc  DenseMarshal.h

validate_dense_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_dense_DEPENDENCIES}: Dense.xml
	marshal  Dense.xml

            ###########################################

validate_sparse_SOURCES      = ${validate_sparse_DEPENDENCIES}            \
                               validate_sparse.cc

validate_sparse_DEPENDENCIES = SparseMarshal.cc  SparseMarshal.h

validate_sparse_LDADD        = $(LDADD_EXTRAS) $(LDADD)

${validate_sparse_DEPENDENCIES}: Sparse.xml
	marshal  Sparse.xml

       
            ###########################################

CLEANFILES = ${validate_dense_DEPENDENCIES}                  \
             ${validate_sparse_DEPENDENCIES}                 
       
#
###################  end Makefile.am for simple array tests  ###################


MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = nh_ls nh_ls_equal nh_ls_one  nh_ls_two

bin_PROGRAMS = $(MAIN)

LDADD_MODULES = -L../../@BOREALISROOT@/modules/common  -lborealiscommon    \
                -L../../@BOREALISROOT@/modules/catalog -lborealiscatalog   \
                -L../../@BOREALISROOT@/modules/util    -lborealisutil      \
                -L../../@BOREALISROOT@/modules/ha      -lborealisha        \
                -L../../@BOREALISROOT@/modules/queryProcessor -lborealisqp \
                -L../../@BOREALISROOT@/external        -lborealisexternal

LDADD_EXTRAS = -ldl  @BOREALISLIB@  @EXTERNALLIB@

LDADD = $(LDADD_MODULES) $(LDADD_EXTRAS)

INCLUDES = @BOREALISINCLUDE@                                   \
           -I ../../@BOREALISROOT@/modules/common              \
           -I ../../@BOREALISROOT@/modules/util                \
           -I ../../@BOREALISROOT@/modules/rpc                 \
           -I ../../@BOREALISROOT@/modules/catalog             \
           -I ../../@BOREALISROOT@/modules/queryProcessor      \
           -I ../../@BOREALISROOT@/modules/queryProcessor/statsMgr 

DEPENDENCIES = ../../@BOREALISROOT@/modules/common/libborealiscommon.la

 ##############################################################################

nh_ls_DEPENDENCIES = Nh_LsMarshal.cc Nh_LsMarshal.h

nh_ls_SOURCES = ${nh_ls_DEPENDENCIES} nh_ls.cc

${nh_ls_DEPENDENCIES}: nh_ls.xml nh_ls_deploy.xml
	marshal nh_ls.xml nh_ls_deploy.xml

 ##############################################################################

nh_ls_equal_DEPENDENCIES = Nh_Ls_EqualMarshal.cc Nh_Ls_EqualMarshal.h

nh_ls_equal_SOURCES = ${nh_ls_equal_DEPENDENCIES} nh_ls_equal.cc

${nh_ls_equal_DEPENDENCIES}: nh_ls_equal.xml nh_ls_deploy.xml
	marshal nh_ls_equal.xml nh_ls_deploy.xml

 ##############################################################################

nh_ls_one_DEPENDENCIES = Nh_Ls_OneMarshal.cc Nh_Ls_OneMarshal.h

nh_ls_one_SOURCES = ${nh_ls_one_DEPENDENCIES} nh_ls_one.cc

${nh_ls_one_DEPENDENCIES}: nh_ls_one.xml nh_ls_one_deploy.xml
	marshal nh_ls_one.xml nh_ls_one_deploy.xml

 ##############################################################################

nh_ls_two_DEPENDENCIES = Nh_Ls_TwoMarshal.cc Nh_Ls_TwoMarshal.h

nh_ls_two_SOURCES = ${nh_ls_two_DEPENDENCIES} nh_ls_two.cc

${nh_ls_two_DEPENDENCIES}: nh_ls_two.xml nh_ls_two_deploy.xml
	marshal nh_ls_two.xml nh_ls_two_deploy.xml

 ##############################################################################

CLEANFILES = ${nh_ls_DEPENDENCIES} \
             ${nh_ls_equal_DEPENDENCIES} \
             ${nh_ls_one_DEPENDENCIES} \
             ${nh_ls_two_DEPENDENCIES}

 #############################  end Makefile.am  ##############################

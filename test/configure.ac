dnl Process this file with autoconf to produce a configure script.
AC_INIT(simple/mytest.cc)

# Where are all the config files
AC_CONFIG_AUX_DIR(config)

# What kind of distribution are we supposed to create
AM_INIT_AUTOMAKE(test, 0.1)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX

CFLAGS="-g -Wall -Werror"
CXXFLAGS="-g -Wall -Werror"

AC_PROG_INSTALL
AC_PROG_LN_S
# Declaring that will use libtool
AC_PROG_LIBTOOL

AM_CONFIG_HEADER(config.h)

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Checks for library functions.
AC_CHECK_FUNCS(strstr)



dnl ***************************
dnl     Looking for Borealis
dnl ***************************
AC_DEFUN([AC_BOREALIS],
[AC_ARG_WITH(borealis, [  --with-borealis], ,)
AC_MSG_CHECKING([for Borealis])

# sensible default if nothing specified
if test "x$with_borealis" = x; then
    with_borealis=../src
fi

if test -f "$with_borealis/src/borealis.cc"; then
    BOREALISROOT=$with_borealis
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_ERROR($with_borealis/src/borealis.cc does not exist)
fi
AC_SUBST(BOREALISROOT)
])

AC_BOREALIS



dnl ***************************
dnl     Looking for Xerces-C
dnl ***************************
AC_DEFUN([AC_XERCESC],
[AC_ARG_WITH(xercesc,    [  --with-xercesc], , )
AC_MSG_CHECKING([for Xerces-C])

if test "x$with_xercesc" = x; then
   with_xercesc=/usr
fi

if test -f "$with_xercesc/include/xercesc/util/XercesDefs.hpp"; then
    XERCESCDIR=$with_xercesc
    XERCESCINCLUDE="-I$XERCESCDIR/include"
    XERCESCLIB="-L$XERCESCDIR/lib -lxerces-c"
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_ERROR($with_xercesc/include/xercesc/util/XercesDefs.hpp does not exist.)
fi

AC_MSG_CHECKING([whether $XERCESCINCLUDE causes a compiler warning and should be cleared])
AC_LANG_CPLUSPLUS
cppflags_save="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $XERCESCINCLUDE -Wall -Werror"
## Usage: AC_TRY_COMPILE (includes, function-body, [action-if-found [, action-if-not-found]])
AC_TRY_COMPILE(,,, XERCESCINCLUDE="")
if test -z "$XERCESCINCLUDE"; then
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
fi
CPPFLAGS="$cppflags_save"
AC_LANG_C

AC_MSG_CHECKING([so xercescinclude is now])
AC_MSG_RESULT([$XERCESCINCLUDE])
AC_SUBST(XERCESCINCLUDE)
AC_SUBST(XERCESCLIB)
])

AC_XERCESC



dnl ***************************
dnl     Looking for ANTLR 
dnl ***************************
AC_DEFUN([AC_ANTLR],
[AC_ARG_WITH(antlr, [  --with-antlr], , )
AC_MSG_CHECKING([for ANTLR])

# sensible default if nothing specified
if test "x$with_antlr" = x; then
    with_antlr=/usr
fi

if test -f "$with_antlr/include/antlr/SemanticException.hpp"; then
    ANTLRDIR=$with_antlr
    ANTLRLIB="$ANTLRDIR/lib/libantlr.a "
    ANTLRINCLUDE="-I$ANTLRDIR/include"
    ANTLR_JAR_FILE="$ANTLRDIR/lib/antlr.jar"
    dnl ANTLR_JAR_FILE="$ANTLRDIR/share/antlr-2.7.3/antlr.jar"
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_ERROR($with_antlr/include/antlr/SemanticException.hpp does not exist)
fi
AC_SUBST(ANTLRINCLUDE)
AC_SUBST(ANTLRLIB)
AC_SUBST(ANTLR_JAR_FILE)
])

AC_ANTLR



dnl *************************************
dnl     Looking for Berkeley-DB
dnl *************************************
AC_DEFUN([AC_BDB],
[AC_ARG_WITH(bdb, [  --with-bdb], ,)
AC_MSG_CHECKING([for BDB])
AC_MSG_RESULT(with_bdb = $with_bdb)

# sensible default if nothing specified
if test "x$with_bdb" = x; then
    with_bdb=/usr
fi

BDBDIR=$with_bdb

for headers in "$BDBDIR/include/" "$BDBDIR/include/db4.2/"; do
  if test -f "$headers/db_cxx.h"; then
    BDBINCLUDE="-I$headers -I$PWD/external/bdb"
    bdbinc=$headers
  fi
done

for libs in "$BDBDIR/lib/"; do
  if test -f "$libs/libdb_cxx.so"; then
    BDBLIB1="-L$BDBDIR/lib -ldb_cxx"
    bdblib1=$libs
  fi
done

for libs in "$BDBDIR/lib/"; do
  if test -f "$libs/libdb.so"; then
    BDBLIB2="-ldb"
    bdblib2=$libs
  fi
done

if test "x$bdbinc" = "x" || test "x$bdblib1" = "x" || test "x$bdblib2" = "x"; then
    BDBDIR=
    BDBINCLUDE="-I$PWD/external/bdb/stub"
    BDBLIB=
    bdb="false"
    AC_MSG_RESULT([no])
    AC_MSG_RESULT($with_bdb/include/db_cxx.h or $with_bdb/lib/libdb_cxx.so or $with_bdb/lib/libdb.so does not exist.)
    AC_MSG_RESULT(No Berkeley-Db related stuff will be compiled.)
else
    BDBLIB="$BDBLIB1 $BDBLIB2"
    dbdlibs="$dbdlib1 $dbdlib2"
    bdb="true"
    AC_MSG_RESULT([yes ($bdbinc $bdblibs)])
fi


AC_SUBST(BDBDIR)
AC_SUBST(BDBINCLUDE)
AC_SUBST(BDBLIB)

AM_CONDITIONAL(COND_BDB, test x${bdb} = xtrue)

])

AC_BDB



dnl **************************************************
dnl   Check for OCV (Open Computer Vision)
dnl **************************************************
AC_DEFUN([AC_OCV],
[AC_ARG_WITH(ocv,    [  --with-ocv], , )
AC_MSG_CHECKING([for OCV])

if test "x$with_ocv" = x; then
  with_ocv=/usr
fi

if test -f "$with_ocv/include/opencv/cv.h" &&  test -f "$with_ocv/lib/libcv.so"; then
  OCVDIR=$with_ocv
  OCVINCLUDE="-I$OCVDIR/include/opencv"
  OCVLIB="-L$OCVDIR/lib -lcv -lcxcore -lhighgui -lcvaux -lMagick++"
  AC_MSG_RESULT([yes ($OCVDIR)])
else
  OCVDIR=
  OCVINCLUDE=
  OCVLIB=
  AC_MSG_RESULT([no])
  AC_MSG_RESULT($with_ocv/include/opencv/cv.h does not exist.)
  AC_MSG_RESULT(Array processing will not be compiled.)
fi

AC_SUBST(OCVDIR)
AC_SUBST(OCVINCLUDE)
AC_SUBST(OCVLIB)
])

AC_OCV



dnl **************************************************
dnl   Check for GSL (Gnu Scientific Library)
dnl **************************************************
AC_DEFUN([AC_GSL],
[AC_ARG_WITH(gsl,    [  --with-gsl], , )
AC_MSG_CHECKING([for GSL])

if test "x$with_gsl" = x; then
    with_gsl=/usr
fi

if test -f "$with_gsl/include/gsl/gsl_math.h"; then
    GSLDIR=$with_gsl
    GSLINCLUDE="-I$GSLDIR/include"
    GSLLIB="-L$GSLDIR/lib -lgsl -lgslcblas"
    AC_MSG_RESULT([yes ($GSLDIR)])
else
    GSLDIR=
    GSLINCLUDE=
    GSLLIB=
    AC_MSG_RESULT([no])
    AC_MSG_RESULT($with_gsl/include/gsl/gsl_math.h does not exist.)
fi

AC_SUBST(GSLDIR)
AC_SUBST(GSLINCLUDE)
AC_SUBST(GSLLIB)
])

AC_GSL


                      
dnl ***************************
dnl     Looking for NMSTL
dnl ***************************
AC_DEFUN([AC_NMSTL],
[AC_ARG_WITH(nmstl,    [  --with-nmstl], , )
AC_MSG_CHECKING([for NMSTL])

if test "x$with_nmstl" = x; then
    with_nmstl=/usr/local
fi

if test -f "$with_nmstl/include/NMSTL/ptr"; then
    NMSTLDIR=$with_nmstl
    NMSTLINCLUDE="-I$with_nmstl/include"
    NMSTLLIB="-L$with_nmstl/lib -lNMSTL -lexpat -lpthread -lreadline -lncurses"
    AC_MSG_RESULT($with_nmstl)
else
    AC_MSG_RESULT([not found])
    AC_ERROR(Unable to locate NMSTL in $with_nmstl/include and $with_nmstl/lib.)
fi

AC_MSG_CHECKING([whether $NMSTLINCLUDE causes a compiler warning and should be cleared])
AC_LANG_CPLUSPLUS
cppflags_save="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $NMSTLINCLUDE -Wall -Werror"
AC_TRY_COMPILE(,,, NMSTLINCLUDE="")
if test -z "$NMSTLINCLUDE"; then
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
fi
CPPFLAGS="$cppflags_save"
AC_LANG_C

AC_MSG_CHECKING([so nmstlinclude is now:])
AC_MSG_RESULT([$NMSTLINCLUDE])

AC_SUBST(NMSTLDIR)
AC_SUBST(NMSTLINCLUDE)
AC_SUBST(NMSTLLIB)
AC_SUBST(NMSTLSUBDIR)
])


AC_NMSTL

AC_MSG_CHECKING([whether to use -DNMSTL_PTR_TRACE])
AC_ARG_ENABLE(ptr-trace,    [  --enable-ptr-trace], [
if test "x$enableval" = "xyes"; then
    AC_MSG_RESULT([yes])
    CXXFLAGS="-DNMSTL_PTR_TRACE $CXXFLAGS"
else
    AC_MSG_RESULT([no])
fi
], [AC_MSG_RESULT([no])])



dnl # --------------------------------------------------
dnl # Usage --disable-messages
AC_MSG_CHECKING([whether to use -DNMSTL_NO_MESSAGES])
AC_ARG_ENABLE(messages,    [  --disable-messages], 
[
    AC_MSG_RESULT([yes])
    CXXFLAGS="-DNMSTL_NO_MESSAGES $CXXFLAGS"
], [AC_MSG_RESULT([no])])


dnl **************************************************
dnl Establish composite libraries and includes.
dnl **************************************************

EXTERNALLIB="$GSLLIB $BDBLIB $OCVLIB"
AC_SUBST(EXTERNALLIB)

EXTERNALINCLUDE="$BDBINCLUDE $GSLINCLUDE $OCVINCLUDE $ARRAY_INCLUDE $PREDICT_INCLUDE"
AC_SUBST(EXTERNALINCLUDE)

BOREALISLIB="$NMSTLLIB $XERCESCLIB $ANTLRLIB"
AC_SUBST(BOREALISLIB)

BOREALISINCLUDE="$NMSTLINCLUDE $XERCESCINCLUDE $ANTLRINCLUDE"
AC_SUBST(BOREALISINCLUDE)


dnl AC_OUTPUT(Makefile simple/Makefile unit/Makefile valid/Makefile  valid/catalog/Makefile valid/system/Makefile valid/move/Makefile valid/two_chains/Makefile valid/global/Makefile valid/connect/Makefile valid/revisions/Makefile valid/array/Makefile valid/deploy/Makefile composite/Makefile composite/fault/Makefile composite/sunion/Makefile composite/sunion/src/Makefile composite/loadnetwork/Makefile composite/updates/Makefile composite/NHLoadShedder/Makefile composite/GlobalLoadShedder/Makefile composite/GlobalLoadShedder/chain/Makefile composite/revision/Makefile developer/Makefile composite/deltaProcessing/Makefile composite/deltaProcessing/video/Makefile composite/deltaProcessing/ais/Makefile composite/deltaProcessing/stars/Makefile)
AC_OUTPUT(Makefile cql/Makefile)


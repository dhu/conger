dnl Process this file with autoconf to produce a configure script.
AC_INIT(src/borealis.cc)

# Where are all the config files
AC_CONFIG_AUX_DIR(config)

# What kind of distribution are we supposed to create
AM_INIT_AUTOMAKE(borealis, 0.1)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX

CFLAGS="-g -Wall -Werror"
CXXFLAGS="-g -Wall -Werror"

AC_PROG_INSTALL
AC_PROG_LN_S

# Declaring that will use libtool
AC_PROG_LIBTOOL

#AC_PROG_RANLIB

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
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_ERROR($with_antlr/include/antlr/SemanticException.hpp does not exist)
fi
AC_SUBST(ANTLRINCLUDE)
AC_SUBST(ANTLRLIB)
AC_SUBST(ANTLR_JAR_FILE)
])


dnl ***************************
dnl     Looking for ANTLR3 
dnl ***************************
AC_DEFUN([AC_ANTLR3],
[AC_ARG_WITH(antlr3, [  --with-antlr3], , )
AC_MSG_CHECKING([for ANTLR3])

# sensible default if nothing specified
if test "x$with_antlr3" = x; then
    with_antlr3=/opt/borealis-tools/antlr3c
fi

if test -f "$with_antlr3/include/antlr3.h"; then
    ANTLR3DIR=$with_antlr3
    ANTLR3LIB="$ANTLR3DIR/lib/libantlr3c.a "
    ANTLR3INCLUDE="-I$ANTLR3DIR/include"
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_ERROR($with_antlr3/include/antlr3.h does not exist)
fi
AC_SUBST(ANTLR3INCLUDE)
AC_SUBST(ANTLR3LIB)
])

AC_ANTLR3

dnl *************************************
dnl     Looking for Berkeley-DB
dnl *************************************
AC_DEFUN([AC_BDB],
[AC_ARG_WITH(bdb, [  --with-bdb], ,)
AC_MSG_CHECKING([for BDB])

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
    XERCESCINCLUDE="-I$XERCESCDIR/include/"
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

dnl # --------------------------------------------------
dnl # Usage --disable-messages
AC_MSG_CHECKING([whether to use -DNMSTL_NO_MESSAGES])
AC_ARG_ENABLE(messages,    [  --disable-messages], 
[
    AC_MSG_RESULT([yes])
    CXXFLAGS="-DNMSTL_NO_MESSAGES $CXXFLAGS"
])


dnl # --------------------------------------------------
dnl # Usage --enable-ptr-trace=yes 
AC_MSG_CHECKING([whether to use -DNMSTL_PTR_TRACE])
AC_ARG_ENABLE(ptr-trace,    [  --enable-ptr-trace], [
if test "x$enableval" = "xyes"; then
    AC_MSG_RESULT([yes])
    CXXFLAGS="-DNMSTL_PTR_TRACE $CXXFLAGS"
else
    AC_MSG_RESULT([no])
fi
], [AC_MSG_RESULT([no])]) 


AC_ANTLR
AC_BDB
AC_XERCESC


dnl ***************************
dnl     Looking for Java
dnl ***************************
AC_MSG_CHECKING([for Java compiler])
rm -f conftest.java conftest.class
echo 'public class conftest {}' > conftest.java

with_javac=no
for javac in javac jikes; do
    $javac conftest.java 2>&5
    if test -e conftest.class; then
        with_javac=$javac
        break
    fi
done
rm -f conftest.java conftest.class

AC_MSG_RESULT([$with_javac])
if test "$with_javac" != no; then
    JAVAC="$with_javac"
fi
AC_SUBST(JAVAC)
JAVA="java"
AC_SUBST(JAVA)


dnl **************************************************
dnl    Check if User Defined Boxes should be built.
dnl **************************************************
AC_MSG_CHECKING([whether to build User Defined Boxes])
AC_ARG_ENABLE(udb,    [  --enable-udb], [
if test "x$enableval" = "xyes"; then
    AC_MSG_RESULT([yes])
    udb="true"
else
    AC_MSG_RESULT([no])
    udb="false"
fi
], [AC_MSG_RESULT([no])]) 
AM_CONDITIONAL(COND_UDB, test x${udb} = xtrue)


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
  AC_MSG_RESULT(Predictive QP will not be compiled.)
fi

AC_SUBST(GSLDIR)
AC_SUBST(GSLINCLUDE)
AC_SUBST(GSLLIB)
])

AC_GSL


dnl **************************************************
dnl   Check for OCV (Open Computer Vision)
dnl **************************************************

AC_DEFUN([AC_OCV],
[AC_ARG_WITH(ocv,    [  --with-ocv], , )
AC_MSG_CHECKING([for OCV])

if test "x$with_ocv" = x; then
  with_ocv=/usr
fi

if test -f "$with_ocv/include/opencv/cv.h"; then
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
dnl    Check if array processing should be enabled.
dnl **************************************************

AC_MSG_CHECKING([whether to enable array processing])
AC_ARG_ENABLE(array, [  --enable-array], [
if test "x$enableval" = "xyes"; then
    AC_MSG_RESULT([yes])
    array="true"
    ARRAY_INCLUDE="-I$PWD/external/array"
else
    AC_MSG_RESULT([no])
    array="false"
    ARRAY_INCLUDE="-I$PWD/external/array/stub"
fi

AC_SUBST(ARRAY_INCLUDE)
], [
    AC_MSG_RESULT([no])
    array="false"
    ARRAY_INCLUDE="-I$PWD/external/array/stub"
])

AM_CONDITIONAL(COND_ARRAY, test -f "$OCVDIR/include/opencv/cv.h" -a x${array} = xtrue)



dnl **************************************************
dnl    Check if NHLoadShedder should be enabled.
dnl **************************************************
AC_MSG_CHECKING([whether to enable NHLoadShedder])
AC_ARG_ENABLE(nh_ls, [  --enable-NHLoadShedder], [
if test "x$enableval" = "xyes"; then
    AC_MSG_RESULT([yes])
    nh_ls="true"
    NH_LS_INCLUDE="-I$PWD/modules/optimizer/nhopt"
else
    AC_MSG_RESULT([no])
    nh_ls="false"
    NH_LS_INCLUDE=""
fi

AC_SUBST(NH_LS_INCLUDE)
], [
    AC_MSG_RESULT([no])
    nh_ls="false"
    NH_LS_INCLUDE=""
])


dnl **************************************************
dnl Establish composite libraries and includes.
dnl **************************************************

if test "$array" = "true"; then
    EXTERNALLIB="$GSLLIB $BDBLIB $OCVLIB"
    EXTERNALINCLUDE="$BDBINCLUDE $GSLINCLUDE $OCVINCLUDE $ARRAY_INCLUDE $PREDICT_INCLUDE"
    AC_MSG_RESULT(Build with OCV)
else
    EXTERNALLIB="$GSLLIB $BDBLIB"
    EXTERNALINCLUDE="$BDBINCLUDE $GSLINCLUDE $ARRAY_INCLUDE $PREDICT_INCLUDE"
    AC_MSG_RESULT(Not building with OCV)
fi

AC_SUBST(EXTERNALLIB)
AC_SUBST(EXTERNALINCLUDE)

BOREALISLIB="$NMSTLLIB $XERCESCLIB $ANTLRLIB $ANTLR3LIB"
AC_SUBST(BOREALISLIB)

BOREALISINCLUDE="$NMSTLINCLUDE $XERCESCINCLUDE $ANTLRINCLUDE $ANTLR3INCLUDE"
AC_SUBST(BOREALISINCLUDE)

AC_OUTPUT(Makefile modules/Makefile external/Makefile external/udb/Makefile external/udb/stub/Makefile external/bdb/Makefile external/bdb/stub/Makefile external/array/Makefile external/array/stub/Makefile src/Makefile modules/rpc/Makefile modules/ha/Makefile modules/availMon/Makefile modules/common/Makefile modules/util/Makefile modules/queryProcessor/Makefile modules/queryProcessor/runtime/Makefile modules/queryProcessor/storageMgr/Makefile modules/queryProcessor/boxes/Makefile modules/queryProcessor/expr/Makefile modules/queryProcessor/statsMgr/Makefile modules/optimizer/Makefile modules/optimizer/nhopt/Makefile modules/optimizer/local/Makefile modules/catalog/Makefile)

if test "x$configure_nmstl" = "x1"; then
    cd nmstl
    autoconf
    ./configure
    cd ..
fi

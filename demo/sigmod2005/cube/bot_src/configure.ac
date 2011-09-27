# Process this file with autoconf to produce a configure script.
AC_INIT(src/main.cpp)

# Where are all the config files
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE(test, 0.1)

AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

CXXFLAGS="-DHAS_SOCKLEN_T"

AC_PROG_INSTALL
AC_PROG_LN_S

AC_PROG_LIBTOOL

# Check for X paths
AC_PATH_X
#LIBS="$LIBS -L$x_libraries"

# Checks for libraries.
AC_CHECK_LIB([GL], [main], [], [
              AC_MSG_ERROR([Can't find libGL])])
AC_CHECK_LIB([GLU], [main], LIBGLU="-lGLU", LIBGLU="", -pthread)
#AC_CHECK_LIB([GLU], [main], [], [
#             AC_MSG_ERROR([Can't find libGLU])])
AC_CHECK_LIB([jpeg], [main], [], [
              AC_MSG_ERROR([Can't find libjpeg])])
# FIXME: Replace `main' with a function in `-lz':
AC_CHECK_LIB([z], [main], [], [
              AC_MSG_ERROR([Can't find libz])])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h float.h malloc.h memory.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/socket.h fcntl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit floor gethostbyname gethostname inet_ntoa memset mkdir pow select socket sqrt strchr strdup strrchr strstr stricmp])

AM_PATH_SDL(1.2.0, [], [AC_MSG_ERROR([Can't find SDL on your system])])
dnl Checks for libraries. 
dnl Replace `main' with a function in -lSDL_ttf: 

dnl ***************************
dnl     Looking for SDL_mixer
dnl ***************************
AC_DEFUN(AC_SDL_MIXER,
[AC_ARG_WITH(sdl_mixer,    [  --with-sdl-mixer], , )
AC_MSG_CHECKING([for SDL_MIXER])

if test "x$with_sdl_mixer" = x; then
    with_sdl_mixer=/usr/local
fi

if test -f "$with_sdl_mixer/include/SDL/SDL_mixer.h"; then
    SDL_MIXER_DIR=$with_sdl_mixer
    SDL_MIXER_INCLUDE="-I$with_sdl_mixer/include/SDL"
    SDL_MIXER_LIB="-L$with_sdl_mixer/lib -lSDL_mixer"
    AC_MSG_RESULT($with_sdl_mixer)
else
    AC_MSG_RESULT([not found])
    AC_ERROR(Unable to locate SDL_MIXER in $with_sdl_mixer/include and $with_sdl_mixer/lib.)
fi


AC_SUBST(SDL_MIXER_DIR)
AC_SUBST(SDL_MIXER_INCLUDE)
AC_SUBST(SDL_MIXER_LIB)
])


dnl ***************************
dnl     Looking for SDL_image
dnl ***************************
AC_DEFUN(AC_SDL_IMAGE,
[AC_ARG_WITH(sdl_image,    [  --with-sdl-image], , )
AC_MSG_CHECKING([for SDL_IMAGE])

if test "x$with_sdl_image" = x; then
    with_sdl_image=/usr/local
fi

if test -f "$with_sdl_image/include/SDL/SDL_image.h"; then
    SDL_IMAGE_DIR=$with_sdl_image
    SDL_IMAGE_INCLUDE="-I$with_sdl_image/include/SDL"
    SDL_IMAGE_LIB="-L$with_sdl_image/lib -lSDL_image"
    AC_MSG_RESULT($with_sdl_image)
else
    AC_MSG_RESULT([not found])
    AC_ERROR(Unable to locate SDL_IMAGE in $with_sdl_image/include and $with_sdl_image/lib.)
fi


AC_SUBST(SDL_IMAGE_DIR)
AC_SUBST(SDL_IMAGE_INCLUDE)
AC_SUBST(SDL_IMAGE_LIB)
])

dnl ***************************
dnl     Looking for Borealis
dnl ***************************
AC_DEFUN(AC_BOREALIS,
        [AC_ARG_WITH(borealis, [  --with-borealis], ,)
        AC_MSG_CHECKING([for Borealis])

# sensible default if nothing specified
if test "x$with_borealis" = x; then
    with_borealis=../../
fi

if test -f "$with_borealis/src/src/borealis.cc"; then
    BOREALISROOT=$with_borealis
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_ERROR($with_borealis/src/src/borealis.cc does not exist)
fi
AC_SUBST(BOREALISROOT)
])

dnl ***************************
dnl     Looking for Xerces-C
dnl ***************************
AC_DEFUN(AC_XERCESC,
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
AC_SUBST(XERCESCDIR)
AC_SUBST(XERCESCINCLUDE)
AC_SUBST(XERCESCLIB)
])


dnl ***************************
dnl     Looking for NMSTL
dnl ***************************
AC_DEFUN(AC_NMSTL,
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

#####################
# Looking for Java.
#####################
AC_MSG_CHECKING([for Java])
echo "import java.util.Vector; public class conftest { Vector<Integer> i; }" > conftest.java
with_java=no
for javac in javac jikes; do
  $javac conftest.java 2>&5
  if test -e conftest.class; then
    with_java=java
    break
  fi
done
rm -f conftest.java conftest.class

AC_MSG_RESULT([$with_java])
if test "$with_java" != no; then
  JAVA="$with_java"
  JAVA_HOME=`which java`
fi
AC_SUBST(JAVA_HOME)
AC_SUBST(JAVA)

#####
# Enable NMSTL ptr trace
#####
AC_MSG_CHECKING([whether to use -DNMSTL_PTR_TRACE])
AC_ARG_ENABLE(ptr-trace,    [  --enable-ptr-trace], [
if test "x$enableval" = "xyes"; then
    AC_MSG_RESULT([yes])
    CXXFLAGS="-DNMSTL_PTR_TRACE $CXXFLAGS"
else
    AC_MSG_RESULT([no])
fi
], [AC_MSG_RESULT([no])])


#####
# Disable game rendering
#####
AC_MSG_CHECKING([whether to disable rendering])
AC_ARG_ENABLE(render,       [  --disable-render], [
if test "x$enableval" = "xno"; then
    AC_MSG_RESULT([yes])
    CXXFLAGS="-DDISABLE_RENDER $CXXFLAGS"
else
    AC_MSG_RESULT([no])
fi
], [AC_MSG_RESULT([no])])



AC_SDL_MIXER
AC_SDL_IMAGE
AC_BOREALIS
AC_XERCESC
AC_NMSTL

AC_CONFIG_FILES([../run_cube_demo.sh], [chmod +x ../run_cube_demo.sh])
AC_OUTPUT(Makefile enet/Makefile enet/include/Makefile enet/include/enet/Makefile src/Makefile)

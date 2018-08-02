dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(db, 1.0)
AC_CONFIG_SRCDIR([src/main/main.cpp])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_SUBST(VERSION)

AM_INIT_AUTOMAKE([1.11 foreign -Wgnu subdir-objects])
AM_MAINTAINER_MODE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])

# c++ stuff
AC_PROG_CXX
AC_PROG_CC

AC_LANG([C])
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX(14, noext, mandatory)

LIBRARY_VERSION=1:0:0
AC_SUBST(LIBRARY_VERSION)

# qt
PKG_CHECK_MODULES([QTCORE], [Qt5Core >= 5.1.1], [])
PKG_CHECK_MODULES([QTWIDGETS], [Qt5Widgets >= 5.1.1], [])
PKG_CHECK_MODULES([QTGUI], [Qt5Gui  >= 5.1.1], [])
PKG_CHECK_MODULES([QTWEBKIT], [Qt5WebKit  >= 5.1.1], [])
PKG_CHECK_MODULES([QTWEBKITWIDGETS], [Qt5WebKitWidgets  >= 5.1.1], [])

# python
PKG_CHECK_MODULES([PYTHON], [python >= 2.7], [])

# boost
AX_BOOST_BASE([1.54],, [AC_MSG_ERROR([db needs Boost, but it was not found in your system])])
AX_BOOST_SYSTEM
AX_BOOST_PYTHON
AX_BOOST_FILESYSTEM
AX_BOOST_PROGRAM_OPTIONS

# libclang
AX_LIBCLANG([0], [35])
CLANG_LIBS=$LIBS
AC_SUBST(CLANG_LIBS)

# bfd.h
AC_CHECK_HEADERS(bfd.h,
[AM_CONDITIONAL([HAVE_BFD_H], [true])],
[AC_MSG_WARN([Couldn't find bfd.h, verbose stack dumps disabled.])]
)

# remove autoconf's "-g -O2" defaults
CFLAGS=`echo $CFLAGS | sed 's/-O.//g'`
CFLAGS=`echo $CFLAGS | sed 's/-g.//g'`
CFLAGS=`echo $CFLAGS | sed 's/-O//g'`
CFLAGS=`echo $CFLAGS | sed 's/-g//g'`
CXXFLAGS=`echo $CXXFLAGS | sed 's/-O.//g'`
CXXFLAGS=`echo $CXXFLAGS | sed 's/-g.//g'`
CXXFLAGS=`echo $CXXFLAGS | sed 's/-O//g'`
CXXFLAGS=`echo $CXXFLAGS | sed 's/-g//g'`

CFLAGS="${CFLAGS} -fno-strict-aliasing -Wall -Werror"
CXXFLAGS="${CXXFLAGS} -fno-strict-aliasing -Wall -Werror"

# --enable-debug
AC_ARG_ENABLE([debug],
  [  --enable-debug    turn on compiler/opengl debugging],
  [case "${enableval}" in
    yes) debug=true ;;
    no)  debug=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
  esac],[debug=false])
if test "x$debug" != xfalse; then
      CFLAGS="-O0 -g3 -fno-inline ${CFLAGS}"
      CXXFLAGS="-O0 -g3 -fno-inline ${CXXFLAGS}"
      AC_DEFINE(DEBUG_ENABLED,1,[turn on compiler/opengl debugging])
fi
AM_CONDITIONAL([DO_DEBUG], [test x$debug = xtrue]) 

# --enable-coverage
AC_ARG_ENABLE([coverage],
  [  --enable-coverage    turn on gcov/lcov support],
  [case "${enableval}" in
    yes) coverage=true ;;
    no)  coverage=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-coverage]) ;;
  esac],[coverage=false])
if test "x$coverage" != xfalse; then
      CFLAGS="${CFLAGS} --coverage"
      CXXFLAGS="${CXXFLAGS} --coverage"
fi

# --enable-opt
AC_ARG_ENABLE([opt],
  [  --enable-opt    turn on compiler optging],
  [case "${enableval}" in
    yes) opt=true ;;
    no)  opt=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-opt]) ;;
  esac],[opt=false])
if test "x$opt" != xfalse; then
  CFLAGS="-O3 ${CFLAGS}"
  CXXFLAGS="-O3 ${CXXFLAGS}"
fi
if test "x$opt" != xfalse; then
  AC_DEFINE(NDEBUG,1,[strip out asserts])
fi

# --enable-profile
AC_ARG_ENABLE([profile],
  [  --enable-profile    turn on compiler profiling],
  [case "${enableval}" in
    yes) profile=true ;;
    no)  profile=false ;;
    *) AC_MSG_ERROR([bad value ${enableval} for --enable-profile]) ;;
  esac],[profile=false])
if test "x$profile" != xfalse; then
  CFLAGS="-g3 -fno-inline -fno-omit-frame-pointer ${CFLAGS}"
  CXXFLAGS="-g3 -fno-inline -fno-omit-frame-pointer ${CXXFLAGS}"
fi
AM_CONDITIONAL([DO_PROFILER], [test x$profile = xtrue]) 

AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)

# since we're using boost signals we need to make qt not use it's 
# keywords which would conflict 
AC_DEFINE(QT_NO_KEYWORDS,,[do not use qt keywords])

# processor type
if test "x$host_cpu" == xx86_64; then
  AC_DEFINE(X86_64,,[64 bit arch])
fi

# some "lazy" m4 macros define LIBS at some point during their execution
# which causes problems when using subdirs.  so undefine it first.
unset LIBS

AC_CONFIG_FILES(Makefile)

AX_SUBDIRS_CONFIGURE([src/external/libut], [--disable-shared], [], [], [])

# libut 
LIBUT_CFLAGS="-I${ac_abs_top_srcdir}/src/external/libut/src"
LIBUT_LIBS=-"L${ac_abs_top_builddir}/src/external/libut/.libs -lut"
AC_SUBST(LIBUT_CFLAGS)
AC_SUBST(LIBUT_LIBS)

AC_OUTPUT

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(db, 1.0)

AM_INIT_AUTOMAKE([1.11 foreign -Wgnu subdir-objects])
AM_MAINTAINER_MODE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])

AC_PROG_CXX

# qt
PKG_CHECK_MODULES([QTCORE], [Qt5Core >= 5.2.1], [])
PKG_CHECK_MODULES([QTWIDGETS], [Qt5Widgets >= 5.2.1], [])
PKG_CHECK_MODULES([QTGUI], [Qt5Gui  >= 5.2.1], [])
PKG_CHECK_MODULES([QTWEBKIT], [Qt5WebKit  >= 5.2.1], [])
PKG_CHECK_MODULES([QTWEBKITWIDGETS], [Qt5WebKitWidgets  >= 5.2.1], [])

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

AC_CONFIG_FILES(Makefile)
AC_OUTPUT


dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(DYTSI, 1.0)

AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(subdir-objects)

AC_PROG_CXX
AC_PROG_CC

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GSL], [gsl >= 1.15])
PKG_CHECK_MODULES([XERCES], [xerces-c >= 3.1.1])

AC_CHECK_LIB(pthread, pthread_create, [PTHREAD=-lpthread])

AX_BOOST_BASE([1.46.1])
AX_BOOST_FILESYSTEM
AX_BOOST_SYSTEM

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT


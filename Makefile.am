SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -Wall @GSL_CFLAGS@ @XERCES_CFLAGS@
BOOST_FILESYSTEM_LIB = -lboost_filesystem
BOOST_SYSTEM_LIB = -lboost_system
AM_LDFLAGS = @BOOST_FILESYSTEM_LIB@ @BOOST_SYSTEM_LIB@
LIBS += @GSL_LIBS@ @XERCES_LIBS@ @BOOST_FILESYSTEM_LIB@ @BOOST_SYSTEM_LIB@ -lpthread

EXTRA_DIST = autogen.sh COPYING.LESSER

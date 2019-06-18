AC_PREREQ([2.69])
AC_INIT([egt-thermostat], [0.1])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([Makefile.am])
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_SYSTEM
PKG_PROG_PKG_CONFIG

AM_INIT_AUTOMAKE([1.10 foreign subdir-objects])

# Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

CXXFLAGS="$CXXFLAGS -std=c++11 -pedantic -g"
CFLAGS="$CFLAGS -g"

AC_PROG_CXX
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT32_T

PKG_CHECK_MODULES(LIBEGT, [libegt], [], [
   AC_MSG_ERROR(libegt not found.  This is required.)
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

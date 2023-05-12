AC_PREREQ([2.69])
AC_INIT([egt-thermostat],[1.6])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_TARGET
PKG_PROG_PKG_CONFIG

AM_INIT_AUTOMAKE([1.10 foreign subdir-objects])

# Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([win32-dll])

AC_LANG(C++)
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([17], [noext])
AC_PROG_CC
AC_PROG_CPP
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT32_T

AX_PTHREAD([LIBS="$PTHREAD_LIBS $LIBS"
            CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
            CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"],
            AC_MSG_ERROR(Can not find pthreads.  This is required.))

AC_DEFUN([EGT_CC_TRY_FLAG], [
  AC_MSG_CHECKING([whether $CC supports $1])

  egt_save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $1 -Werror"

  AC_COMPILE_IFELSE([AC_LANG_SOURCE([ ])], [egt_cc_flag=yes], [egt_cc_flag=no])
  CFLAGS="$egt_save_CFLAGS"

  if test "x$egt_cc_flag" = "xyes"; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
  AC_MSG_RESULT([$egt_cc_flag])
])

MAYBE_WARN="-Wall -Wextra \
-pedantic \
-Wfloat-equal \
-Wsign-compare \
-Wpointer-arith \
-Wwrite-strings \
-Wmissing-declarations \
-Wpacked \
-Wstrict-aliasing=2 -Winit-self \
-Wno-attributes -Wno-long-long \
-Wno-missing-field-initializers \
-Wno-unused-parameter \
-Wno-psabi" # no warnings about gcc abi changes

# invalidate cached value if MAYBE_WARN has changed
if test "x$egt_cv_warn_maybe" != "x$MAYBE_WARN"; then
	unset egt_cv_warn_cflags
fi
AC_CACHE_CHECK([for supported warning flags], egt_cv_warn_cflags, [
	echo
	WARN_CFLAGS=""

	# Some warning options are not supported by all versions of
	# gcc, so test all desired options against the current
	# compiler.
	#
	# Note that there are some order dependencies
	# here. Specifically, an option that disables a warning will
	# have no net effect if a later option then enables that
	# warnings, (perhaps implicitly). So we put some grouped
	# options (-Wall and -Wextra) up front and the -Wno options
	# last.

	for W in $MAYBE_WARN; do
		EGT_CC_TRY_FLAG([$W], [WARN_CFLAGS="$WARN_CFLAGS $W"])
	done

	egt_cv_warn_cflags=$WARN_CFLAGS
	egt_cv_warn_maybe=$MAYBE_WARN

	AC_MSG_CHECKING([which warning flags were supported])])
WARN_CFLAGS="$egt_cv_warn_cflags"
AC_SUBST(WARN_CFLAGS)

AC_DEFUN([LIBINTL_SRC], [
AC_LANG_PROGRAM([[
#include <libintl.h>
]],[[
char *msg = gettext("test");
]])])

AC_MSG_CHECKING([if libc contains libintl])
AC_LINK_IFELSE([LIBINTL_SRC],
	[AC_MSG_RESULT([yes])
	LIBC_CONTAINS_LIBINTL=Yes],
	[AC_MSG_RESULT([no])
	LIBC_CONTAINS_LIBINTL=])
AC_SUBST([LIBC_CONTAINS_LIBINTL])

AC_CHECK_HEADER([libintl.h],
[NO_GETTEXT=],
[NO_GETTEXT=Yes])
AC_SUBST([NO_GETTEXT])

if test -z "$NO_GETTEXT"; then
    test -n "$LIBC_CONTAINS_LIBINTL" || LIBS="$LIBS -lintl"
fi

PKG_CHECK_MODULES(LIBEGT, [libegt >= 1.7], [], [
   AC_MSG_ERROR(libegt not found.  This is required.)
])

AC_ARG_WITH([sensors],
    AS_HELP_STRING([--without-sensors], [Ignore presence of sensors and disable it]))
AS_IF([test "x$with_sensors" != "xno"],[
   AC_CHECK_HEADER([sensors/sensors.h], [have_sensors=yes], [have_sensors=no])
   if test "x${have_sensors}" = xyes; then
      CXXFLAGS="-DLUA_USE_READLINE ${CXXFLAGS}"
      LDFLAGS="-lsensors ${LDFLAGS}"
      AC_DEFINE(HAVE_SENSORS, 1, [Have sensors support])
   fi
])
AM_CONDITIONAL([HAVE_SENSORS], [test "x${have_sensors}" = xyes])

AC_ARG_ENABLE([lto],
  [AS_HELP_STRING([--enable-lto], [enable gcc's LTO [default=no]])],
  [enable_lto=$enableval], [enable_lto=no])
if test "x$enable_lto" = "xyes" ; then
  AC_MSG_NOTICE([enabling LTO])
  CFLAGS="$CFLAGS -flto"
  CXXFLAGS="$CXXFLAGS -flto"
fi

AC_CONFIG_FILES([Makefile external/Makefile])
AC_OUTPUT

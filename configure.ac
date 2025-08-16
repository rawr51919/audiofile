dnl Process this file with autoconf to produce a configure script.
AC_INIT([audiofile], [0.3.6])
AC_CONFIG_SRCDIR([libaudiofile/AIFF.cpp])

dnl Set libtool version information.
AUDIOFILE_VERSION_INFO=1:0:0
AUDIOFILE_VERSION=$PACKAGE_VERSION
AC_SUBST(AUDIOFILE_VERSION)
AC_SUBST(AUDIOFILE_VERSION_INFO)

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIRS([m4])        dnl Only include local m4 folder
AC_CONFIG_HEADERS([config.h])

dnl --- Documentation / Internationalization support ---
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_REQUIRE_VERSION([0.20])
IT_PROG_INTLTOOL([0.51])                  dnl Intltool support
GTK_DOC_INIT

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
LT_INIT

dnl Ensure C99 support
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([[
    #if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    #error "C99 support required"
    #endif
  ]], [[return 0;]])],
  [AC_MSG_RESULT([C99 support detected])],
  [AC_MSG_ERROR([C99 support is required])])

dnl Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_BIGENDIAN

dnl Enable large file support by default.
AC_SYS_LARGEFILE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

dnl Platform specific setup
platform=none
AC_MSG_CHECKING([for platform specific tests to compile])
case "$host_os" in
  linux*) TEST_BIN="linuxtest alsaplay"; platform=linux ;;
  irix5*|irix6*) TEST_BIN="irixread irixtestloop"; platform=irix ;;
  darwin*)
    if test -e /System/Library/Frameworks/CoreAudio.framework; then
      TEST_BIN="osxplay"
      platform="Mac OS X"
    fi ;;
esac
AC_MSG_RESULT($platform)
AC_SUBST(TEST_BIN)

dnl Compiler flags and options
AC_ARG_ENABLE(werror, AS_HELP_STRING([--enable-werror], [treat compiler warnings as errors]),
  [enable_werror=$enableval], [enable_werror=no])
AM_CONDITIONAL(ENABLE_WERROR, [test "$enable_werror" = "yes"])
AS_IF([test "$enable_werror" = "yes"], [WERROR_CFLAGS="-Werror"; AC_SUBST(WERROR_CFLAGS)])

AC_ARG_ENABLE(coverage, AS_HELP_STRING([--enable-coverage], [enable code coverage]),
  [enable_coverage=$enableval], [enable_coverage=no])
AM_CONDITIONAL(ENABLE_COVERAGE, [test "$enable_coverage" = "yes"])
AS_IF([test "$enable_coverage" = "yes"],
  [COVERAGE_CFLAGS="-fprofile-arcs -ftest-coverage"
   COVERAGE_LIBS="-lgcov"
   AC_SUBST(COVERAGE_CFLAGS)
   AC_SUBST(COVERAGE_LIBS)
   AC_PATH_PROG(LCOV, lcov, :)
   AC_PATH_PROG(GENHTML, genhtml, :)
   AC_SUBST(LCOV)
   AC_SUBST(GENHTML)
   AS_IF([test "$LCOV" = :], [AC_MSG_ERROR([lcov must be installed for code coverage])])])

AC_ARG_ENABLE(valgrind, AS_HELP_STRING([--enable-valgrind], [enable testing with Valgrind]),
  [enable_valgrind=$enableval], [enable_valgrind=no])
AM_CONDITIONAL(ENABLE_VALGRIND, [test "$enable_valgrind" = "yes"])
AS_IF([test "$enable_valgrind" = "yes"],
  [AC_PATH_PROG(VALGRIND, valgrind, :)
   AC_SUBST(VALGRIND)
   AS_IF([test "$VALGRIND" = :], [AC_MSG_ERROR([Could not find Valgrind.])])])

AC_ARG_ENABLE(docs, AS_HELP_STRING([--disable-docs], [disable documentation]),
  [enable_documentation=$enableval], [enable_documentation=yes])
AM_CONDITIONAL(ENABLE_DOCUMENTATION, [test "$enable_documentation" = "yes"])
AS_IF([test "$enable_documentation" = "yes"],
  [AC_PATH_PROG(A2X, a2x, :)
   AC_PATH_PROG(ASCIIDOC, asciidoc, :)
   AS_IF([test "$A2X" = :], [AC_MSG_WARN([Could not find a2x.])])
   AS_IF([test "$ASCIIDOC" = :], [AC_MSG_WARN([Could not find asciidoc.])])])

AC_ARG_ENABLE(examples, AS_HELP_STRING([--disable-examples], [disable examples]),
  [enable_examples=$enableval], [enable_examples=yes])
AS_IF([test "$enable_examples" != "yes"], [TEST_BIN=""])

AC_ARG_ENABLE(flac, AS_HELP_STRING([--disable-flac], [disable FLAC]),
  [enable_flac=$enableval], [enable_flac=yes])

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

FLAC_CFLAGS=""
FLAC_LIBS=""

if test -n "$PKG_CONFIG"; then
  if test "$enable_flac" = "yes"; then
    PKG_CHECK_MODULES([FLAC], [flac >= 1.2.1], [ac_cv_flac=yes], [ac_cv_flac=no])
    FLAC_CFLAGS=`echo $FLAC_CFLAGS | $SED "s:include/FLAC:include:"`
    if test "$ac_cv_flac" = "no"; then enable_flac=no; fi
  fi
else
  enable_flac=no
fi

AC_SUBST(FLAC_CFLAGS)
AC_SUBST(FLAC_LIBS)
AM_CONDITIONAL(ENABLE_FLAC, [test "$enable_flac" = "yes"])
if test "$enable_flac" = "yes"; then
  AC_DEFINE_UNQUOTED([ENABLE_FLAC], [1], [Whether FLAC is enabled.])
else
  AC_DEFINE_UNQUOTED([ENABLE_FLAC], [0], [Whether FLAC is enabled.])
fi

dnl --- Check for ALSA library (Linux) ---
have_alsa=no
if test "$cross_compiling" != yes; then
  AC_CHECK_LIB([asound], [snd_pcm_open], [have_alsa=yes], [have_alsa=no])
fi

if test "$have_alsa" = yes; then
  AC_DEFINE([HAVE_ALSA], [1], [Define if ALSA is available])
else
  AC_DEFINE([HAVE_ALSA], [0], [Define if ALSA is available])
fi
AM_CONDITIONAL([HAVE_ALSA], [test "$have_alsa" = yes])

dnl --- Check for PortAudio library (non-Linux) ---
have_portaudio=no
have_portaudio_lib=no

AC_CHECK_HEADER([portaudio.h], [have_portaudio=yes], [have_portaudio=no])
if test "$cross_compiling" != yes; then
  AC_CHECK_LIB([portaudio], [Pa_Initialize], [have_portaudio_lib=yes], [have_portaudio_lib=no])
fi

if test "$have_portaudio" = yes && test "$have_portaudio_lib" = yes; then
  AC_DEFINE([HAVE_PORTAUDIO], [1], [Define if PortAudio is available])
else
  AC_DEFINE([HAVE_PORTAUDIO], [0], [Define if PortAudio is available])
fi
AM_CONDITIONAL([HAVE_PORTAUDIO], [test "$have_portaudio" = yes && "$have_portaudio_lib" = yes])

AC_CONFIG_FILES([
  audiofile.spec
  audiofile.pc
  audiofile-uninstalled.pc
  po/Makefile.in
  sfcommands/Makefile
  test/Makefile
  gtest/Makefile
  examples/Makefile
  libaudiofile/Makefile
  libaudiofile/alac/Makefile
  libaudiofile/modules/Makefile
  docs/Makefile
  Makefile
])
AC_OUTPUT

AC_INIT([libfmsynth], [0.3])
AC_CONFIG_AUX_DIR([build-aux])
AC_PREREQ([2.69])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([foreign dist-xz subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIRS([m4])
AC_REQUIRE_AUX_FILE([tap-driver.sh])

AC_PROG_AWK
AC_PROG_SED
AC_PROG_GREP

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([23], [noext], [mandatory])

AX_PTHREAD

LT_INIT

AC_ARG_ENABLE([fmsedit],
  AS_HELP_STRING([--enable-fmsedit], [Build fmsedit.]),
  [ case "${enableval}" in
      yes) fmsedit=true ;;
      no)  fmsedit=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-fmsedit]) ;;
    esac
  ],
  [fmsedit=true]
)
AM_CONDITIONAL([FMSEDIT], [test "x$fmsedit" = "xtrue"])

AC_LANG_PUSH([C++])
AC_CHECK_HEADER([format], [], [
  PKG_CHECK_MODULES([FMT], [fmt], [AC_DEFINE([HAVE_FMT], [1], [Use fmt])])
])
AC_CHECK_HEADER([AudioFile.h], [], [AC_MSG_ERROR([Unable to find header for AudioFile library (https://github.com/adamstark/AudioFile).])])
AC_CHECK_HEADER([cxxopts.hpp], [], [AC_MSG_ERROR([Unable to find header for cxxopts library (https://github.com/jarro2783/cxxopts).])])
AC_LANG_POP

PKG_CHECK_MODULES([JSON],    [json11])
PKG_CHECK_MODULES([RTAUDIO], [rtaudio >= 6.0])

AS_IF([test "x$fmsedit" = "xtrue"], [
  PRE512QT=0
  PKG_CHECK_MODULES([QT], [Qt5Core >= 5.12 Qt5Widgets], [], [
    PKG_CHECK_MODULES([QT], [Qt5Core Qt5Widgets])
    PRE512QT=1
  ])
  AC_SUBST(PRE512QT)

  PKG_CHECK_VAR([QT_CONFIG], [Qt5Core], [qt_config])
  QT_REDUCE_RELOCATIONS=`[echo "x $QT_CONFIG" | $GREP reduce_relocations]`
  AS_IF([test "x$QT_REDUCE_RELOCATIONS" != "x"], [
    AC_MSG_RESULT([Qt is configured with 'reduce_relocations' and requires to compile with PIC, forcing PIC on for fmsedit.])
    AC_SUBST([EXTRA_FMSEDIT_QT_CXXFLAGS], $lt_prog_compiler_pic)
  ])

  AC_CHECK_PROGS([QT_UIC], [uic-qt5 uic])
])

AC_ARG_WITH([valgrind], AS_HELP_STRING([--with-valgrind], [Build with support for Valgrind.]))
AS_IF([test "x$with_valgrind" = "xyes"], [
  PKG_CHECK_MODULES([VALGRIND], [valgrind])
  AC_DEFINE([WITH_VALGRIND], [1], [Define to use Valgrind.])
])

ENABLE_NODETESTING=1
AC_ARG_ENABLE([nodetesting], AS_HELP_STRING([--disable-nodetesting], [Disable code that is used only by unit tests.]))
AS_IF([test "x$enable_nodetesting" = "xno"], [
  ENABLE_NODETESTING=0
])
AM_CONDITIONAL([ENABLE_NODETESTING], [test $ENABLE_NODETESTING = 1])
AC_SUBST(ENABLE_NODETESTING)


DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([libfmsynth])


AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile images/Makefile examples/Makefile doc/Makefile libfmsynth.pc Doxyfile])
AC_OUTPUT

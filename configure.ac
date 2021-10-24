AC_INIT([libfmsynth], [0.1])
AC_PREREQ([2.69])
AM_INIT_AUTOMAKE([foreign dist-xz subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([20], [noext], [mandatory])

AX_PTHREAD

LT_INIT

AC_LANG_PUSH([C++])
AC_CHECK_HEADER([format], [], [
  PKG_CHECK_MODULES([FMT], [fmt], [AC_DEFINE([HAVE_FMT], [1], [Use fmt])])
])
AC_CHECK_HEADER([AudioFile.h], [], [AC_MSG_ERROR([Unable to find header for AudioFile library (https://github.com/adamstark/AudioFile).])])
AC_LANG_POP

PKG_CHECK_MODULES([JSON],   [json11 nlohmann_json >= 3])
PKG_CHECK_MODULES([ALUT],   [freealut])
PKG_CHECK_MODULES([OPENAL], [openal])

PKG_CHECK_MODULES([QT], [Qt5Core Qt5Widgets])
AC_CHECK_PROGS([QT_UIC], [uic-qt5 uic])

AC_ARG_WITH([valgrind], AS_HELP_STRING([--with-valgrind], [Build with support for Valgrind.]))
AS_IF([test "x$with_valgrind" = "xyes"], [
  PKG_CHECK_MODULES([VALGRIND], [valgrind])
  AC_DEFINE([WITH_VALGRIND], [1], [Define to use Valgrind.])
])


DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([libfmsynth])


AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile libfmsynth.pc Doxyfile])
AC_OUTPUT

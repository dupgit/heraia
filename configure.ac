AC_PREREQ(2.59)

AC_INIT([Heraia], [0.1.8],
        [Olivier Delhomme <heraia@delhomme.org>],
        [heraia])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/heraia.c])

AM_INIT_AUTOMAKE([$PACKAGE_TARNAME], [$PACKAGE_VERSION])

AC_PROG_INTLTOOL([0.23])

dnl **************************************************
dnl * Directory requirements                         *
dnl **************************************************
TOP_SRCDIR=`pwd`
SRCDIR="${TOP_SRCDIR}/src"
AC_SUBST(TOP_SRCDIR)
AC_SUBST(SRCDIR)

dnl **************************************************
dnl * Put the preprocessor macros in a header file   *
dnl * instead of command line                        *
dnl **************************************************
AC_CONFIG_HEADER([config.h])

dnl **************************************************
dnl * Libraries requirements                         *
dnl **************************************************
PKG_PROG_PKG_CONFIG([0.10])
GLIB_VERSION=2.2.0
GTK_VERSION=2.16.0
GTKHEX_VERSION=1.0.0
GMODULE_VERSION=2.0.0

AC_SUBST(GTKHEX_VERSION)
AC_SUBST(GLIB_VERSION)
AC_SUBST(GTK_VERSION)
AC_SUBST(GMODULE_VERSION)

dnl **************************************************
dnl * Build time sanity check                        *
dnl **************************************************
AM_SANITY_CHECK

dnl **************************************************
dnl * Initialize maintainer mode                     *
dnl **************************************************
AM_MAINTAINER_MODE

dnl **************************************************
dnl * Checks for headers                             *
dnl **************************************************
AC_HEADER_STDC
AC_HEADER_DIRENT

dnl **************************************************
dnl * Checks for typedefs, structures, and           *
dnl * compiler characteristics                       *
dnl **************************************************
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_CHECK_TYPES(unsigned long long)

dnl **************************************************
dnl * Checks for library functions                   *
dnl **************************************************
dnl AC_TYPE_SIGNAL
dnl AC_FUNC_VPRINTF
dnl AC_CHECK_FUNCS(strftime gmtime)

dnl **************************************************
dnl * Checks dynamic libraries capabilities          *
dnl **************************************************
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl **************************************************
dnl * Checks for paths                               *
dnl **************************************************
if test "x$prefix" = "xNONE"; then
    prefix="/usr/local"
fi

if test "x$localstatedir" = "x\${prefix}/var"; then
    localstatedir="$prefix/var"
fi

if test "x$sysconfdir" = "x\${prefix}/etc"; then
    sysconfdir="$prefix/etc"
fi

AC_SUBST(localstatedir)
AC_SUBST(sysconfdir)
AC_SUBST(prefix)

dnl **************************************************
dnl * directories definitions                        *
dnl **************************************************
plugins_dir=$libdir/heraia
data_dir=$datadir/heraia
LOCALEDIR=$datadir/locale

AC_SUBST(LOCALEDIR)
AC_SUBST(plugins_dir)
AC_SUBST(data_dir)

dnl **************************************************
dnl * languages                                      *
dnl **************************************************
ALL_LINGUAS="fr"

dnl **************************************************
dnl * triggers gettext                               *
dnl **************************************************
GETTEXT_PACKAGE=heraia
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext Package used])
AM_GLIB_GNU_GETTEXT

dnl **************************************************
dnl * checking for glib                              *
dnl **************************************************
PKG_CHECK_MODULES(GLIB, [glib-2.0 >= $GLIB_VERSION])

dnl **************************************************
dnl * checking for gtk+                              *
dnl **************************************************
PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= $GTK_VERSION])

dnl **************************************************
dnl * checking for GtkHex                            *
dnl **************************************************
PKG_CHECK_MODULES(GTKHEX, [gtkhex >= $GTKHEX_VERSION])

dnl **************************************************
dnl * checking for gmodule                           *
dnl **************************************************
PKG_CHECK_MODULES(GMODULE, [gmodule-2.0 >= $GMODULE_VERSION])

AC_PROG_INSTALL

dnl This flag : "-Wcast-qual" implies gcc warnings on gthreads.h
dnl -DGTK_DISABLE_DEPRECATED=1 -DG_DISABLE_DEPRECATED=1 -DGSEAL_ENABLE

CFLAGS="$CFLAGS -Wall -Wstrict-prototypes -Wmissing-declarations \
-Wbad-function-cast -Wcast-align -Wnested-externs -Wunused \
-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE \
-DGSEAL_ENABLE -DG_DISABLE_DEPRECATED=1"


dnl **************************************************
dnl * checking OS                                    *
dnl **************************************************

case $build_os in
    *linux*)
        AC_DEFINE_UNQUOTED(SYS_LINUX, 1, [This is a Linux system])
        LDFLAGS="$LDFLAGS -export-dynamic -no-undefined -module"
    ;;
    *mingw*)
        AC_DEFINE_UNQUOTED(SYS_MINGW, 1, [This is a Windows system using MinGW])
        LDFLAGS="$LDFLAGS -export-dynamic -no-undefined -module -Wl,--export-all-symbols"
        CFLAGS="$CFLAGS -mms-bitfields -mwindows -mno-cygwin"
    ;;
esac



dnl **************************************************
dnl * ./configure options                            *
dnl **************************************************

dnl **************************************************
dnl * debug option                                   *
dnl * At this early stage debugging IS activated     *
dnl * by default                                     *
dnl **************************************************
AC_ARG_ENABLE([debug],
     [  --enable-debug    Turn on debugging],
     [case "${enableval}" in
       yes) debug=true ;;
       no)  debug=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
     esac],[debug=false])
if test x$debug = xtrue
then
 AC_DEFINE_UNQUOTED(ENABLE_DEBUG, TRUE, [Debug mode On])
 CFLAGS="$CFLAGS -ggdb"
else
 AC_DEFINE_UNQUOTED(ENABLE_DEBUG, TRUE, [Debug mode Off])
 CFLAGS="$CFLAGS -ggdb"
fi

dnl **************************************************
dnl * gcov option                                    *
dnl **************************************************
AC_ARG_ENABLE([gcov],
     [  --enable-gcov     Enables gcov coverage tests],
     [case "${enableval}" in
       yes) gcov=true ;;
       no)  gcov=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-gcov]) ;;
     esac],[gcov=false])
if test x$gcov = xtrue
then
 CFLAGS="-g -fprofile-arcs -ftest-coverage $CFLAGS "
else
 CFLAGS="$CFLAGS"
fi

dnl **************************************************
dnl * gprof option                                   *
dnl **************************************************
AC_ARG_ENABLE([gprof],
     [  --enable-gprof    Enables gprof profiling],
     [case "${enableval}" in
       yes) gprof=true ;;
       no)  gprof=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-gprof]) ;;
     esac],[gprof=false])
if test x$gprof = xtrue
then
 CFLAGS="-pg $CFLAGS"
else
 CFLAGS="$CFLAGS"
fi

AC_SUBST(LDFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(GTKHEX_CFLAGS)
AC_SUBST(GTKHEX_LIBS)
AC_SUBST(GMODULE_CFLAGS)
AC_SUBST(GMODULE_LIBS)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AC_CONFIG_FILES([
Makefile
libheraia/Makefile
src/Makefile
po/Makefile.in
plugins/Makefile
plugins/stat/Makefile
pixmaps/Makefile
pixmaps/hicolor/Makefile
pixmaps/hicolor/16x16/Makefile
pixmaps/hicolor/16x16/apps/Makefile
pixmaps/hicolor/32x32/Makefile
pixmaps/hicolor/32x32/apps/Makefile
pixmaps/hicolor/48x48/Makefile
pixmaps/hicolor/48x48/apps/Makefile
pixmaps/hicolor/64x64/Makefile
pixmaps/hicolor/64x64/apps/Makefile
pixmaps/hicolor/128x128/Makefile
pixmaps/hicolor/128x128/apps/Makefile
pixmaps/hicolor/256x256/Makefile
pixmaps/hicolor/256x256/apps/Makefile
])
AC_OUTPUT

echo
echo "*** Dumping configuration ***"
echo
echo "    - Build For OS             : $build_os"
echo "    - Compiler                 : ${CC}"
echo "    - Options                  :"
echo "      . is code profiling on ? : $gprof"
echo "      . Code coverage is on ?  : $gcov"
echo "      . debbuging is on ?      : $debug"
echo "    - Prefix                   : $prefix"
echo "    - Exec prefix              : ${exec_prefix}"
echo "    - Src dir                  : ${srcdir}"
echo "    - Data root dir            : ${datarootdir}"
echo "    - Data dir                 : ${datadir}"
echo "    - Plugins dir              : ${plugins_dir}"
echo "    - Locale dir               : ${LOCALEDIR}"
echo
echo "    You can now run 'make -s' to compile heraia"
echo

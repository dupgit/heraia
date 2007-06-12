AC_PREREQ(2.59)

AC_INIT([Heraia], [0.0.1],
        [Olivier Delhomme],
        [heraia])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([$PACKAGE_TARNAME], [$PACKAGE_VERSION])

dnl directory requirement
TOP_SRCDIR=`pwd`
SRCDIR="${TOP_SRCDIR}/src"
AC_SUBST(TOP_SRCDIR)
AC_SUBST(SRCDIR)

dnl Put the preprocessor macros in a header file instead of command line 
AC_CONFIG_SRCDIR([src/heraia.c])

dnl Libraries requirements
GLIB2_VERSION=2.2.0
GTK2_VERSION=2.2.0
GTKHEX_VERSION=1.0.0
LIBGLADE_VERSION=1.110.0
GMODULE_VERSION=2.0.0

AC_SUBST(GTKHEX_VERSION)
AC_SUBST(GLIB2_VERSION)
AC_SUBST(GTK2_VERSION)
AC_SUBST(LIBGLADE_VERSION)
AC_SUBST(GMODULE_VERSION)

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
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(gettimeofday select socket strdup strerror strstr)

dnl **************************************************
dnl * Checks for programs                            *
dnl **************************************************
AM_PROG_LIBTOOL

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
dnl * plugins directory definition                   *
dnl **************************************************
plugins_dir=$libdir/heraia
data_dir=$datadir/heraia

AC_SUBST(plugins_dir)
AC_SUBST(data_dir)

dnl the line below does not work !!
dnl AC_DEFINE_UNQUOTED(PLUGINS_DIR, "$plugins_dir", [Directory where the plugins are located])

dnl **************************************************
dnl * Check for Gtkdoc.                              *
dnl **************************************************
AC_CHECK_PROG(GTKDOC, gtkdoc-mkdb, true, false)
AM_CONDITIONAL(HAVE_GTK_DOC, test -n $GTKDOC)
AC_SUBST(HAVE_GTK_DOC)

AC_ARG_ENABLE(gtk-doc, [  --enable-gtk-doc  Use gtk-doc to build documentation [default=no]], enable_gtk_doc="$enableval", enable_gtk_doc=no)
AM_CONDITIONAL(ENABLE_GTK_DOC, test x$enable_gtk_doc = xyes)

dnl **************************************************
dnl * languages                                      *
dnl **************************************************
ALL_LINGUAS="fr_FR"

dnl **************************************************
dnl * triggers gettext                               *
dnl **************************************************
AM_GLIB_GNU_GETTEXT
GETTEXT_PACKAGE=heraia
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [fix])

dnl **************************************************
dnl * checking for glib                              *
dnl **************************************************
PKG_CHECK_MODULES(GLIB2, [glib-2.0 >= $GLIB2_VERSION])

dnl **************************************************
dnl * checking for gtk+                              *
dnl **************************************************
PKG_CHECK_MODULES(GKT2, [gtk+-2.0 >= $GTK2_VERSION])

dnl **************************************************
dnl * checking for gtkhex                            *
dnl **************************************************
PKG_CHECK_MODULES(GTKHEX, [gtkhex >= $GTKHEX_VERSION])

dnl **************************************************
dnl * checking for libglade                          *
dnl **************************************************
PKG_CHECK_MODULES(LIBGLADE, [libglade-2.0 >= $LIBGLADE_VERSION])

dnl **************************************************
dnl * checking for gmodule                           *
dnl **************************************************
PKG_CHECK_MODULES(GMODULE, [gmodule-2.0 >= $GMODULE_VERSION])


AC_PROG_INSTALL

CFLAGS="$CFLAGS -Wall -Wstrict-prototypes -Wmissing-declarations \
-Wbad-function-cast -Wcast-qual -Wcast-align -Wnested-externs -Wunused \
-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE"


dnl **************************************************
dnl * checking OS                                    *
dnl **************************************************

case $build_os in
    *linux*)
	AC_DEFINE_UNQUOTED(SYS_LINUX, 1, [This is a Linux system])
	;;
    *mingw*)
	AC_DEFINE_UNQUOTED(SYS_MINGW, 1, [This is a Windows system using MinGW])
	LDFLAGS="$LDFLAGS -no-undefined -Wl,--export-all-symbols"
        CFLAGS="$CFLAGS -mms-bitfields -mwindows -mno-cygwin"
        ;;
esac

AC_SUBST(LDFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(GTKHEX_CFLAGS)
AC_SUBST(GTKHEX_LIBS)
AC_SUBST(LIBGLADE_CFLAGS)
AC_SUBST(LIBGLADE_LIBS)
AC_SUBST(GMODULE_CFLAGS)
AC_SUBST(GMODULE_LIBS)
AC_SUBST(GLIB2_CFLAGS)
AC_SUBST(GLIB2_LIBS)
AC_SUBST(GTK2_CFLAGS)
AC_SUBST(GTK2_LIBS)

AC_CONFIG_FILES([Makefile src/Makefile plugins/Makefile plugins/stat/Makefile])
AC_OUTPUT

echo
echo "*** Dumping configuration ***"
echo 
echo "    - Build For OS           : $build_os"
echo "    - Compiler               : ${CC}"
echo "    - Generate documentation : $enable_gtk_doc"
echo "    - Prefix                 : $prefix"
echo "    - Exec prefix            : $exec_prefix"
echo "    - Src dir                : $srcdir"
echo "    - Data dir               : $datadir"
echo "    - Plugins dir            : $plugins_dir"
echo
echo "    You can now run 'make' to compile heraia"
echo

SUBDIRS =

DEFS = -I$(SRCDIR) -I$(SRCDIR)/include			\
	   -I$(top_builddir)/libheraia/include		\
	   -DDATA_DIR=\"$(data_dir)\"				\
	   $(GTKHEX_CFLAGS)							\
	   $(GMODULE_CFLAGS) $(CFLAGS)				\
	   $(GTK_CFLAGS) $(GLIB_CFLAGS)

pkglib_LTLIBRARIES = libstat.la

libstat_la_SOURCES = 	\
	stat.c				\
	stat.h

libstat_la_LDFLAGS = $(LDFLAGS)
libstat_la_LIBADD = $(GTKHEX_LIBS) $(GMODULE_LIBS)	\
					$(GTK_LIBS) $(GLIB_LIBS)

libstatdir = $(plugins_dir)

statdatafiles = stat.gtkbuilder

statdatadir = $(datadir)/@PACKAGE@
statdata_DATA = $(statdatafiles)
EXTRA_DIST = $(statdatafiles)

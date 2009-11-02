SUBDIRS =

DEFS = -I$(SRCDIR) -I$(SRCDIR)/include			\
       -DPLUGINS_DIR=\"$(plugins_dir)\"	     	\
	   -I$(top_builddir)/libheraia/include		\
	   -DDATA_DIR=\"$(data_dir)\"				\
	   $(GTKHEX_CFLAGS)							\
	   $(GMODULE_CFLAGS) $(CFLAGS)				\
	   $(GTK2_CFLAGS) $(GLIB2_CFLAGS)

pkglib_LTLIBRARIES = libstat.la

libstat_la_SOURCES = 	\
	stat.c				\
	stat.h

libstat_la_LDFLAGS = $(LDFLAGS)
libstat_la_LIBADD = $(GTKHEX_LIBS) $(GMODULE_LIBS)	\
					$(GTK2_LIBS) $(GLIB2_LIBS)

libstatdir = $(plugins_dir)

statgtkbuilderfiles = stat.gtkbuilder

statgtkbuilderdir = $(datadir)/@PACKAGE@

statgtkbuilder_DATA = $(statgtkbuilderfiles)

EXTRA_DIST = $(statgtkbuilderfiles)

SUBDIRS =

DEFS = -I$(SRCDIR)  -I$(SRCDIR)/include	 		\
       -I$(top_builddir)/libheraia/include		\
	   -DPLUGINS_DIR=\"$(plugins_dir)\"			\
	   -DDATA_DIR=\"$(data_dir)\"				\
	   $(GTKHEX_CFLAGS) $(LIBGLADE_CFLAGS)		\
	   $(GMODULE_CFLAGS) $(CFLAGS)				\
	   $(GTK2_CFLAGS) $(GLIB2_CFLAGS)

bin_PROGRAMS = heraia
heraia_LDFLAGS = $(LDFLAGS)
heraia_LDADD = $(GTKHEX_LIBS) $(LIBGLADE_LIBS) $(GMODULES_LIBS) \
			   $(GLIB2_LIBS) $(GTK2_LIBS) \
			   -L$(top_builddir)/libheraia/ -lheraia \
			   $(GMODULE_LIBS) $(GKT2_LIBS)

heraia_SOURCES = 					\
	heraia.c						\
	include/heraia.h				\
	include/heraia_types.h			

gladefiles = heraia.glade

gladedir = $(datadir)/@PACKAGE@
glade_DATA = $(gladefiles)
EXTRA_DIST = $(gladefiles)

AM_CPPFLAGS = \
	$(LIBGLADE_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(GTKHEX_CFLAGS) \
	$(GLIB2_CFLAGS) \
	$(GKT2_CFLAGS)


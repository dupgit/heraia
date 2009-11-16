SUBDIRS =

DEFS = -I$(SRCDIR)  -I$(SRCDIR)/include	 		\
       -I$(top_builddir)/libheraia/include		\
	   -DPLUGINS_DIR=\"$(plugins_dir)\"			\
	   -DDATA_DIR=\"$(data_dir)\"				\
	   $(Heraia_Hex_CFLAGS) 						\
	   $(GMODULE_CFLAGS) $(CFLAGS)				\
	   $(GTK2_CFLAGS) $(GLIB2_CFLAGS)

bin_PROGRAMS = heraia
heraia_LDFLAGS = $(LDFLAGS)
heraia_LDADD = $(GTKHEX_LIBS) $(GMODULES_LIBS) 			\
			   $(GLIB2_LIBS) $(GTK2_LIBS) 				\
			   -L$(top_builddir)/libheraia/ -lheraia	\
			   $(GMODULE_LIBS) $(GKT2_LIBS)

heraia_SOURCES = 					\
	heraia.c						\
	include/heraia.h				\
	include/heraia_types.h

gtkbuilderfiles = heraia.gtkbuilder

gtkbuilderdir = $(datadir)/@PACKAGE@
gtkbuilder_DATA = $(gtkbuilderfiles)
EXTRA_DIST = $(gtkbuilderfiles)

AM_CPPFLAGS = 			\
	$(GMODULE_CFLAGS) 	\
	$(GTKHEX_CFLAGS) 	\
	$(GLIB2_CFLAGS) 	\
	$(GKT2_CFLAGS)


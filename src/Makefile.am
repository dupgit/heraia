SUBDIRS =

DEFS = -I$(SRCDIR)  -I$(SRCDIR)/include	 		\
       -I$(top_builddir)/libheraia/include		\
	   -DDATA_DIR=\"$(data_dir)\"				\
	   -DLOCALEDIR=\"$(LOCALEDIR)\"				\
	   $(Heraia_Hex_CFLAGS) 					\
	   $(GMODULE_CFLAGS) $(CFLAGS)				\
	   $(GTK_CFLAGS) $(GLIB_CFLAGS)

bin_PROGRAMS = heraia
heraia_LDFLAGS = $(LDFLAGS)
heraia_LDADD = $(GTKHEX_LIBS) $(GMODULES_LIBS) 			\
			   $(GLIB_LIBS) $(GTK_LIBS) 				\
			   -L$(top_builddir)/libheraia/ -lheraia	\
			   $(GMODULE_LIBS) $(GKT_LIBS)

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
	$(GLIB_CFLAGS) 	\
	$(GKT_CFLAGS)

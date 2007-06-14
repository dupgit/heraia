SUBDIRS = 

DEFS = -I$(SRCDIR) -I$(SRCDIR)/include		\
	-DPLUGINS_DIR=\"$(plugins_dir)\"	\
	-DDATA_DIR=\"$(data_dir)\" 		\
	$(GTKHEX_CFLAGS) $(LIBGLADE_CFLAGS) 	\
	$(GMODULE_CFLAGS) $(CFLAGS) 		\
	$(GTK2_CFLAGS) $(GLIB2_CFLAGS)

bin_PROGRAMS = heraia
heraia_LDFLAGS = -export-dynamic -no-undefined 
heraia_LDADD = $(GTKHEX_LIBS) $(LIBGLADE_LIBS) $(GMODULES_LIBS) $(GLIB2_LIBS) $(GTK2_LIBS)

heraia_SOURCES = 			\
	heraia.c			\
	data_interpretor.c 		\
	decode.c 			\
	ghex_heraia_interface.c		\
	heraia_ui.c 			\
	io.c 				\
	log.c				\
	plugin.c			\
	plugin_list.c			\
	include/heraia.h		\
	include/data_interpretor.h 	\
	include/decode.h		\
	include/ghex_heraia_interface.h	\
	include/heraia_ui.h		\
	include/io.h			\
	include/log.h			\
	include/plugin.h		\
	include/types.h			\
	include/heraia-errors.h		\
	include/plugin_list.h		


gladefiles = heraia.glade
gladedir = $(datadir)/@PACKAGE@
glade_DATA = $(gladefiles)
EXTRA_DIST = $(gladefiles)
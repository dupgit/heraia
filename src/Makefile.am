SUBDIRS = 

DEFS = -I$(top_srcdir)/ -I$(srcdir)/include/	\
	-DPLUGINS_DIR=\"$(plugins_dir)\"	\
	-DDATA_DIR=\"$(data_dir)\" 		\
	$(GTKHEX_CFLAGS) $(LIBGLADE_CFLAGS) 	\
	$(GMODULE_CFLAGS)

bin_PROGRAMS = heraia
heraia_LDFLAGS = -export-dynamic
heraia_LDADD = $(GTKHEX_LIBS) $(LIBGLADE_LIBS)

heraia_SOURCES = 		\
	heraia.c		\
	data_interpretor.c 	\
	decode.c 		\
	ghex_heraia_interface.c	\
	heraia_ui.c 		\
	io.c 			\
	graphic_analysis.c 	\
	log.c			\
	plugin.c		\
	plugin_list.c

gladefiles = heraia.glade
gladedir = $(datadir)/@PACKAGE@
glade_DATA = $(gladefiles)
EXTRA_DIST = $(gladefiles)
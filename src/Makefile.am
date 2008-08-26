SUBDIRS =

DEFS = -I$(SRCDIR) -I$(top_builddir)/libheraia/include -I$(SRCDIR)/include		\
	-DPLUGINS_DIR=\"$(plugins_dir)\"		\
	-DDATA_DIR=\"$(data_dir)\"				\
	$(GTKHEX_CFLAGS) $(LIBGLADE_CFLAGS)		\
	$(GMODULE_CFLAGS) $(CFLAGS)				\
	$(GTK2_CFLAGS) $(GLIB2_CFLAGS)

bin_PROGRAMS = heraia
heraia_LDFLAGS = $(LDFLAGS)
heraia_LDADD = $(GTKHEX_LIBS) $(LIBGLADE_LIBS) $(GMODULES_LIBS) $(GLIB2_LIBS) $(GTK2_LIBS) -L$(top_builddir)/libheraia/libheraia.la

heraia_SOURCES = 					\
	heraia.c						\
	data_interpretor.c 				\
	decode.c 						\
	ghex_heraia_interface.c			\
	heraia_io.c 					\
	heraia_ui.c 					\
	log.c							\
	plugin.c						\
	plugin_list.c					\
	list_data_types.c				\
	data_type.c						\
	treatments.c					\
	include/heraia.h				\
	include/data_interpretor.h   	\
	include/decode.h		        \
	include/ghex_heraia_interface.h	\
	include/heraia_errors.h			\
	include/heraia_io.h		   		\
	include/heraia_types.h			\
	include/heraia_ui.h		    	\
	include/log.h			    	\
	include/plugin.h		    	\
	include/plugin_list.h			\
	include/list_data_types.h		\
	include/data_type.h		    	\
	include/treatments.h

gladefiles = heraia.glade

gladedir = $(datadir)/@PACKAGE@
glade_DATA = $(gladefiles)
EXTRA_DIST = $(gladefiles)

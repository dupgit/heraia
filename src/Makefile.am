SUBDIRS = 

DEFS = -I$(top_srcdir)/ -I$(srcdir)/include/ \
	-DPLUGINS_DIR=\"$(plugins_dir)\" \
	-DDATA_DIR=\"$(datadir)\" \
	$(GTKHEX_CFLAGS) $(LIBGLADE_CFLAGS)

bin_PROGRAMS = heraia
heraia_LDFLAGS = -export-dynamic
heraia_LDADD = $(GTKHEX_LIBS) $(LIBGLADE_LIBS)

heraia_SOURCES = 	\
	heraia.c	\
	data_interpretor.c \
	decode.c \
	ghex_heraia_interface.c \
	heraia_ui.c \
	io.c


SUBDIRS = 

DEFS = -I$(SRCDIR) -I$(SRCDIR)/include			\
	$(GTKHEX_CFLAGS) $(LIBGLADE_CFLAGS) 		\
	$(GMODULE_CFLAGS) $(CFLAGS) $(GTK2_CFLAGS) 	\
	$(GLIB2_CFLAGS)

stat_la_SOURCES = 	\
	stat.c		\
	stat.h

stat_la_LDFLAGS = -no-undefined -module -export-dynamic
statdir = $(plugins_dir)
stat_LTLIBRARIES = stat.la

statgladefiles = stat.glade
statgladedir = $(datadir)/@PACKAGE@
statglade_DATA = $(statgladefiles)
EXTRA_DIST = $(statgladefiles)
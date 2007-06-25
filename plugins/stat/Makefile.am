SUBDIRS = 

DEFS = -I$(SRCDIR) -I$(SRCDIR)/include			\
	$(GTKHEX_CFLAGS) $(LIBGLADE_CFLAGS) 		\
	$(GMODULE_CFLAGS) $(CFLAGS) $(GTK2_CFLAGS) 	\
	$(GLIB2_CFLAGS)

stat_la_SOURCES = 	\
	stat.c		\
	stat.h

stat_la_LDFLAGS = -no-undefined -module -export-dynamic
stat_la_LIBADD = $(GTKHEX_LIBS) $(LIBGLADE_LIBS) $(GMODULE_LIBS) $(GTK2_LIBS) $(GLIB2_LIBS) $(LDFLAGS)
statdir = $(plugins_dir)
stat_LTLIBRARIES = stat.la

statgladefiles = stat.glade
statgladedir = $(datadir)/@PACKAGE@
statglade_DATA = $(statgladefiles)
EXTRA_DIST = $(statgladefiles)
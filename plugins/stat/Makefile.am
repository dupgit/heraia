SUBDIRS = 

DEFS = -I$(top_srcdir)/ -I$(top_srcdir)/src/include/	\
	$(GTKHEX_CFLAGS) $(LIBGLADE_CFLAGS) 		\
	$(GMODULE_CFLAGS)

stat_la_SOURCES = 
	stat.c	\
	stat.h

stat_la_LDFLAGS = -module
statdir = $(plugins_dir)
stat_LTLIBRARIES = stat.la

statgladefiles = stat.glade
statgladedir = $(datadir)/@PACKAGE@
statglade_DATA = $(statgladefiles)
EXTRA_DIST = $(statgladefiles)
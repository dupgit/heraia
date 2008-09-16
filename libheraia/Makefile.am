noinst_LTLIBRARIES = libheraia.la

headerfiles = 				\
		include/libheraia.h \
		include/decode.h

libheraia_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/libheraia/include $(GLIB2_CFLAGS)
AM_LDFLAGS = $(GLIB2_LIBS)
include_HEADERS = $(headerfiles)

libheraia_la_SOURCES =  	\
		decode.c			\
		libheraia.c 		\
		$(headerfiles)


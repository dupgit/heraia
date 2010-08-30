# noinst_LTLIBRARIES = libheraia.la

headerfiles = 							\
		include/libheraia.h 			\
		include/decode.h    			\
		include/user_prefs.h			\
		include/main_pref_window.h		\
		include/heraia_ui.h				\
		include/ghex_heraia_interface.h	\
	    include/heraia_io.h		   		\
		include/plugin.h				\
		include/heraia_errors.h			\
		include/data_interpretor.h		\
		include/plugin_list.h			\
		include/log.h					\
		include/tests.h					\
		include/goto_dialog.h			\
		include/result_window.h

pkglib_LTLIBRARIES = libheraia.la

libheraia_la_CFLAGS = -I$(top_builddir) -DPLUGINS_DIR=\"$(plugins_dir)\"	 \
					  -I$(top_builddir)/libheraia/include $(GTKHEX_CFLAGS)   \
					  $(GMODULE_CFLAGS) $(CFLAGS)	$(GTK2_CFLAGS) 			 \
					  $(GLIB2_CFLAGS) -DLOCALEDIR=\"${LOCALEDIR}\"

AM_LDFLAGS = $(GTKHEX_LIBS) $(GMODULES_LIBS) $(GLIB2_LIBS) $(GTK2_LIBS)

include_HEADERS = $(headerfiles)

libheraiadir =$(libdir)/heraia

libheraia_la_SOURCES =  		\
		decode.c				\
		libheraia.c 			\
		user_prefs.c			\
		main_pref_window.c		\
		heraia_ui.c				\
		log.c					\
		ghex_heraia_interface.c	\
		plugin.c				\
		heraia_io.c 			\
		data_interpretor.c		\
		plugin_list.c			\
		tests.c					\
		goto_dialog.c			\
		result_window.c			\
		$(headerfiles)

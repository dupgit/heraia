noinst_LTLIBRARIES = libheraia.la

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
		include/list_data_types.h		\
		include/data_interpretor.h		\
		include/data_type.h				\
		include/treatments.h			\
		include/plugin_list.h			\
		include/log.h

libheraia_la_CPPFLAGS = -I$(top_builddir) -DPLUGINS_DIR=\"$(plugins_dir)\"	 \ 
						-I$(top_builddir)/libheraia/include $(GTKHEX_CFLAGS) \
						$(LIBGLADE_CFLAGS) $(GMODULE_CFLAGS) $(CFLAGS)		 \
						$(GTK2_CFLAGS) $(GLIB2_CFLAGS)

AM_LDFLAGS = $(GTKHEX_LIBS) $(LIBGLADE_LIBS) $(GMODULES_LIBS) $(GLIB2_LIBS)  \
			 $(GTK2_LIBS)

# include_HEADERS = $(headerfiles)

# libheraiadir =$(libdir)/heraia

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
		list_data_types.c		\
		data_interpretor.c		\
		data_type.c				\
		treatments.c			\
		plugin_list.c			\
		$(headerfiles)


# inspired from glade project

SUBDIRS = hicolor

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache

update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
                $(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
                echo "***   $(gtk_update_icon_cache)"; \
	fi

pixmapsdir = $(datadir)/@PACKAGE@/pixmaps
dist_pixmaps_DATA = 				\
	new_logo.png 					\
	new_logo_petit.png				\
	logo_with_transparency.png		\
	logo_with_more_transparency.png

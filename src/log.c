/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  log.c
  log functions for heraia
 
  (C) Copyright 2006 - 2008 Olivier Delhomme
  e-mail : heraia@delhomme.org
  URL    : http://heraia.tuxfamily.org 
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or  (at your option) 
  any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY;  without even the implied warranty of
  MERCHANTABILITY  or  FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/

#include "heraia_types.h"

/**
 *  A function that allow me to printy things on stdout and in th log window
 */
static void my_log(heraia_window_t *main_window, gchar *log_domain, GLogLevelFlags log_level, const char *format, ...);

static void log_window_connect_signals(heraia_window_t *main_window);
static gboolean delete_log_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data );
static void destroy_log_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void logw_close_clicked(GtkWidget *widget, gpointer data);

/**
 * Prints a	 message to stdout
 */
void print_message(const char *format, ...)
{
	va_list args;
	gchar *str = NULL;
	gchar *str_utf8 = NULL;
	GError *err = NULL;

	g_return_if_fail (format != NULL);
	
	va_start(args, format);
	str = g_strdup_vprintf(format, args);
	va_end(args);
	
	str_utf8 = g_locale_to_utf8(str, -1, NULL, NULL, &err);

	if (str_utf8)
		{
			fputs(str_utf8, stdout);
			g_free(str_utf8);
		}
	else
		{
			fprintf(stderr, "Can't convert output to the locale: %s\n", err->message);
			fputs(str, stderr);
			g_error_free(err);
		}

	g_free(str);
}


/**
 *  The log function 
 */
static void my_log(heraia_window_t *main_window, gchar *log_domain, GLogLevelFlags log_level, const char *format, ...)
{
	va_list args;
	gchar *str = NULL;
	gchar *display = NULL;
	GtkTextView *logw_textview = GTK_TEXT_VIEW(heraia_get_widget(main_window->xmls->main, "logw_textview"));
	GtkTextBuffer *tb = NULL;
	GtkTextIter iStart;

	va_start(args, format);
	str = g_strdup_vprintf(format, args);
	va_end(args);

	switch (log_level)
		{
		case G_LOG_FLAG_RECURSION:
			display = g_strdup_printf("%s - RECURSION: %s\n%c", log_domain, str, '\0');
			g_print("%s\n", display);
			/* exit(log_level); */
			break;
 
		case G_LOG_FLAG_FATAL:
			display = g_strdup_printf("%s - FATAL: %s\n%c", log_domain, str, '\0');
			g_print("%s\n", display);
			/* exit(log_level); */
			break;

		case G_LOG_LEVEL_ERROR:
			display = g_strdup_printf("%s - ERROR: %s\n%c", log_domain, str, '\0');
			g_print("%s\n", display);
			/* exit(log_level); */
			break;

		case G_LOG_LEVEL_CRITICAL:
			display = g_strdup_printf("%s - CRITICAL: %s\n%c", log_domain, str, '\0');
			break;

		case G_LOG_LEVEL_WARNING:
			display = g_strdup_printf("%s - WARNING: %s\n%c", log_domain, str, '\0');
			break;

		case G_LOG_LEVEL_MESSAGE:
			display = g_strdup_printf("%s - MESSAGE: %s\n%c", log_domain, str, '\0');
			break;

		case G_LOG_LEVEL_INFO:
			display = g_strdup_printf("%s - INFO: %s\n%c", log_domain, str, '\0');
			break;

		case G_LOG_LEVEL_DEBUG:
			display = g_strdup_printf("%s - DEBUG: %s\n%c", log_domain, str, '\0');
			break;

		case G_LOG_LEVEL_MASK: /* To avoid a compilation warning */
			break;
		}

	g_print("%s", display);
	tb = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(logw_textview)));
	gtk_text_buffer_get_end_iter(tb, &iStart);
	gtk_text_buffer_insert(tb, &iStart, display, -1);
	
	g_free(str);
	g_free(display);
}


/**
 *  A function that helps logging a message a the specified level 
 */
void log_message(heraia_window_t *main_window, GLogLevelFlags log_level, const char *format, ...)
{
	va_list args;
	gchar *str = NULL;
	gchar *str_time = NULL;
	gchar *str_time_utf8 = NULL;
	gchar *str_utf8 = NULL;
	GTimeVal *time = NULL;
	GError *err = NULL;

    if (!(main_window->debug == FALSE && log_level == G_LOG_LEVEL_DEBUG))
		{
			g_return_if_fail(format != NULL);

			va_start(args, format);
			str = g_strdup_vprintf(format, args);
			va_end(args);
			str_utf8 = g_locale_to_utf8(str, -1, NULL, NULL, &err);

			time = (GTimeVal *) g_malloc0 (sizeof(GTimeVal));
			g_get_current_time(time);
			str_time = g_time_val_to_iso8601(time);
			str_time_utf8 = g_locale_to_utf8(str_time, -1, NULL, NULL, &err);

	
			if (str_utf8)
				{
					if (str_time_utf8)
						{
							my_log(main_window, HERAIA_LOG_DOMAIN, log_level, "%s - %s%c", str_time_utf8, str_utf8, '\0');
						}
					else
						{
							my_log(main_window, HERAIA_LOG_DOMAIN, log_level, "%s - %s%c", str_time, str_utf8, '\0');
						}
				}
			else
				{
					if (str_time_utf8)
						{
							my_log(main_window, HERAIA_LOG_DOMAIN, log_level, "%s - %s%c", str_time_utf8, str, '\0');
						}
					else
						{
							my_log(main_window, HERAIA_LOG_DOMAIN, log_level, "%s - %s%c", str_time, str, '\0');
						}
				}

			g_free(time);
			g_free(str);
			g_free(str_time);
			g_free(str_time_utf8);
			g_free(str_utf8);
		}
}

/**
 *  Shows and hides the log window
 */

void show_hide_log_window(heraia_window_t *main_window, gboolean show, GtkCheckMenuItem *cmi)
{
	GtkWidget *log_dialog = NULL;
	window_prop_t *log_box_prop = main_window->win_prop->log_box;
	
	log_dialog = heraia_get_widget(main_window->xmls->main, "log_window");
	
	if (show == TRUE)
	   {
			main_window->win_prop->log_box = move_and_show_dialog_box(log_dialog, log_box_prop);
	   }
	else
	  {
		  if (log_box_prop->displayed == TRUE)
			{
				gtk_check_menu_item_set_active(cmi, FALSE);
				main_window->win_prop->log_box = record_and_hide_dialog_box(log_dialog, log_box_prop);
			}
	  }
}

/**
 *  The Check menu item for the Log window 
 */
void mw_cmi_affiche_logw_toggle(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkCheckMenuItem *cmi = GTK_CHECK_MENU_ITEM(widget);
	gboolean checked = gtk_check_menu_item_get_active(cmi);

	show_hide_log_window(main_window, checked, cmi);
}


/**** The Signals ****/

/** 
 *  Closing the window 
 */
static gboolean delete_log_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data )
{
	logw_close_clicked(widget, data);

	return FALSE;
}

static void destroy_log_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	logw_close_clicked(widget, data);
}

/**
 *  Close button is clicked
 */
static void logw_close_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkCheckMenuItem *cmi = GTK_CHECK_MENU_ITEM(heraia_get_widget(main_window->xmls->main, "mw_cmi_affiche_logw"));

	show_hide_log_window(main_window, FALSE, cmi);
}


/**
 *  Connecting the window signals to the right functions  
 */
static void log_window_connect_signals(heraia_window_t *main_window)
{

	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "log_window")), "delete_event", 
					 G_CALLBACK(delete_log_window_event), main_window);

	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "log_window")), "destroy", 
					 G_CALLBACK(destroy_log_window), main_window);
	
	/* Close Button */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "logw_close_b")), "clicked", 
					 G_CALLBACK(logw_close_clicked), main_window);

	/* the toogle button */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "mw_cmi_affiche_logw")), "toggled",
							 G_CALLBACK(mw_cmi_affiche_logw_toggle), main_window);

}

/**** End Signals ****/



/**
 *  Inits the log window interface 
 */
void log_window_init_interface(heraia_window_t *main_window)
{
	log_window_connect_signals(main_window);
}

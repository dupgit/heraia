/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  log.c
  log functions for heraia

  (C) Copyright 2006 - 2011 Olivier Delhomme
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/**
 * @file log.c
 * Includes everything that deals with the logging system
 */
#include <libheraia.h>


static void my_log(heraia_struct_t *main_struct, gchar *log_domain, GLogLevelFlags log_level, const char *format, ...);
static void log_window_connect_signals(heraia_struct_t *main_struct);
static gboolean delete_log_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data );
static void destroy_log_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void logw_close_clicked(GtkWidget *widget, gpointer data);
static void scroll_down_textview(heraia_struct_t *main_struct);

/**
 * @fn print_message(const char *format, ...)
 * Prints a  message to stdout
 * @param format : a printf style format
 * @param ... : va_list to fill the format.
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
            fprintf(stderr, Q_("Can't convert output to the locale: %s\n"), err->message);
            fputs(str, stderr);
            g_error_free(err);
        }

    g_free(str);
}


/**
 * @fn my_log(heraia_struct_t *main_struct, gchar *log_domain, GLogLevelFlags log_level, const char *format, ...);
 *  A function that allow me to printy things on stdout and in th log window
 * @param main_struct : main structure
 * @param log_domain : should be the program's name
 * @param log_level : A string that may be either G_LOG_FLAG_RECURSION,
 *        G_LOG_FLAG_FATAL, G_LOG_LEVEL_ERROR, G_LOG_LEVEL_CRITICAL,
 *        G_LOG_LEVEL_WARNING, G_LOG_LEVEL_MESSAGE, G_LOG_LEVEL_INFO,
 *        G_LOG_LEVEL_DEBUG
 * @param format : a printf style format
 * @param ... : va_list to fill the format.
 */
static void my_log(heraia_struct_t *main_struct, gchar *log_domain, GLogLevelFlags log_level, const char *format, ...)
{
    va_list args;
    gchar *str = NULL;
    gchar *display = NULL;
    GtkTextView *logw_textview = GTK_TEXT_VIEW(heraia_get_widget(main_struct->xmls->main, "logw_textview"));
    GtkTextBuffer *tb = NULL;
    GtkTextIter iStart;

    va_start(args, format);
    str = g_strdup_vprintf(format, args);
    va_end(args);

    switch (log_level)
        {
            case G_LOG_FLAG_RECURSION:
                display = g_strdup_printf(Q_("%s - RECURSION: %s\n%c"), log_domain, str, '\0');
                g_print("%s\n", display);
                /* exit(log_level); */
                break;

            case G_LOG_FLAG_FATAL:
                display = g_strdup_printf(Q_("%s - FATAL: %s\n%c"), log_domain, str, '\0');
                g_print("%s\n", display);
                /* exit(log_level); */
                break;

            case G_LOG_LEVEL_ERROR:
                display = g_strdup_printf(Q_("%s - ERROR: %s\n%c"), log_domain, str, '\0');
                g_print("%s\n", display);
                /* exit(log_level); */
                break;

            case G_LOG_LEVEL_CRITICAL:
                display = g_strdup_printf(Q_("%s - CRITICAL: %s\n%c"), log_domain, str, '\0');
                break;

            case G_LOG_LEVEL_WARNING:
                display = g_strdup_printf(Q_("%s - WARNING: %s\n%c"), log_domain, str, '\0');
                break;

            case G_LOG_LEVEL_MESSAGE:
                display = g_strdup_printf(Q_("%s - MESSAGE: %s\n%c"), log_domain, str, '\0');
                break;

            case G_LOG_LEVEL_INFO:
                display = g_strdup_printf(Q_("%s - INFO: %s\n%c"), log_domain, str, '\0');
                break;

            case G_LOG_LEVEL_DEBUG:
                display = g_strdup_printf(Q_("%s - DEBUG: %s\n%c"), log_domain, str, '\0');
                break;

            case G_LOG_LEVEL_MASK: /* To avoid a compilation warning */
                break;
        }

    g_print("%s", display);

    /* inserting text in the textview */
    tb = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(logw_textview)));

    gtk_text_buffer_get_end_iter(tb, &iStart);
    gtk_text_buffer_insert(tb, &iStart, display, -1);

    /* scrolling down */
    scroll_down_textview(main_struct);

    g_free(str);
    g_free(display);
}


/**
 * Scrolling down to the new line at the end of the textview
 * @param main_struct : main structure
 */
static void scroll_down_textview(heraia_struct_t *main_struct)
{
    GtkTextView *logw_textview = NULL;
    GtkTextBuffer *tb = NULL;
    GtkTextIter end;
    GtkTextMark *mark = NULL;

    logw_textview = GTK_TEXT_VIEW(heraia_get_widget(main_struct->xmls->main, "logw_textview"));
    tb = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(logw_textview)));

    gtk_text_buffer_get_end_iter(tb, &end);
    gtk_text_iter_set_line_offset(&end, 0);
    mark = gtk_text_buffer_get_mark(tb, "scroll");
    gtk_text_buffer_move_mark(tb, mark, &end);
    gtk_text_view_scroll_mark_onscreen(logw_textview, mark);
}


/**
 * @fn log_message(heraia_struct_t *main_struct, GLogLevelFlags log_level, const char *format, ...)
 *  A function that helps logging a message a the specified level. A wrapper to
 *  my_log function log_domain is defined by HERAIA_LOG_DOMAIN
 * @param main_struct : main structure
 * @param log_level : A string that may be either G_LOG_FLAG_RECURSION,
 *        G_LOG_FLAG_FATAL, G_LOG_LEVEL_ERROR, G_LOG_LEVEL_CRITICAL,
 *        G_LOG_LEVEL_WARNING, G_LOG_LEVEL_MESSAGE, G_LOG_LEVEL_INFO,
 *        G_LOG_LEVEL_DEBUG
 * @param format : a printf style format
 * @param ... : va_list to fill the format.
 * @todo may be include the hability to choose a different log domain ?
 */
void log_message(heraia_struct_t *main_struct, GLogLevelFlags log_level, const char *format, ...)
{

#if LOGOUTPUT == 1     /* Logging things only if LOGOUTPUT as a value of 1 (use --disable-logoutput in ./configure to change this) */
    va_list args;
    gchar *str = NULL;
    gchar *str_time = NULL;
    gchar *str_time_utf8 = NULL;
    gchar *str_utf8 = NULL;
    GTimeVal *time = NULL;
    GError *err = NULL;

    if (!(main_struct->debug == FALSE && log_level == G_LOG_LEVEL_DEBUG))
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
                            my_log(main_struct, HERAIA_LOG_DOMAIN, log_level, "%s - %s%c", str_time_utf8, str_utf8, '\0');
                        }
                    else
                        {
                            my_log(main_struct, HERAIA_LOG_DOMAIN, log_level, "%s - %s%c", str_time, str_utf8, '\0');
                        }
                }
            else
                {
                    if (str_time_utf8)
                        {
                            my_log(main_struct, HERAIA_LOG_DOMAIN, log_level, "%s - %s%c", str_time_utf8, str, '\0');
                        }
                    else
                        {
                            my_log(main_struct, HERAIA_LOG_DOMAIN, log_level, "%s - %s%c", str_time, str, '\0');
                        }
                }

            g_free(time);
            g_free(str);
            g_free(str_time);
            g_free(str_time_utf8);
            g_free(str_utf8);
        }

#endif /* LOGOUTPUT */
}


/**
 * @fn mw_cmi_show_logw_toggle(GtkWidget *widget, gpointer data)
 *  The Check menu item for the Log window
 * @param widget : the widget that issued the signal (here the log check menu
 *                 item
 * @param data : user data, MUST be main_struct main structure
 */
void mw_cmi_show_logw_toggle(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *cmi = NULL;
    gboolean checked = FALSE;
    GtkWidget *log_dialog = NULL;

    log_dialog = heraia_get_widget(main_struct->xmls->main, "log_window");

    cmi = heraia_get_widget(main_struct->xmls->main, "mw_cmi_show_logw");
    checked = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(cmi));

    if (checked == TRUE)
        {
            scroll_down_textview(main_struct);
            move_and_show_dialog_box(log_dialog, main_struct->win_prop->log_box);
        }
    else
        {
            record_and_hide_dialog_box(log_dialog, main_struct->win_prop->log_box);
        }
}


/******************************** The Signals *********************************/

/**
 * @fn gboolean delete_log_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data )
 *  Closing the window
 * @param widget : calling widget
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 * @return Always returns TRUE in order to propagate the signal
 */
static gboolean delete_log_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data )
{
    logw_close_clicked(widget, data);

    return TRUE;
}


/**
 * @fn void destroy_log_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
 * When the window is destroyed (Gtk's doc says that we may never get there)
 * @param widget : calling widget
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
*/
static void destroy_log_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    logw_close_clicked(widget, data);
}


/**
 * @fn  void logw_close_clicked(GtkWidget *widget, gpointer data)
 *  Close button is clicked
 * @param widget : calling widget
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void logw_close_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *cmi = NULL;
    GtkWidget *log_dialog = NULL;

    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {
            log_dialog = heraia_get_widget(main_struct->xmls->main, "log_window");
            record_and_hide_dialog_box(log_dialog, main_struct->win_prop->log_box);

            cmi = heraia_get_widget(main_struct->xmls->main, "mw_cmi_show_logw");
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(cmi), FALSE); /* This emits the signals that calls mw_cmi_show_logw_toggle function */
        }
}


/**
 * @fn void log_window_connect_signals(heraia_struct_t *main_struct)
 *  Connecting the window signals to the right functions
 * @param main_struct : main structure
 */
static void log_window_connect_signals(heraia_struct_t *main_struct)
{

    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {

            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "log_window")), "delete_event",
                             G_CALLBACK(delete_log_window_event), main_struct);

            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "log_window")), "destroy",
                             G_CALLBACK(destroy_log_window), main_struct);

            /* Close Button */
            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "logw_close_b")), "clicked",
                             G_CALLBACK(logw_close_clicked), main_struct);

            /* the toogle button */
            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "mw_cmi_show_logw")), "toggled",
                             G_CALLBACK(mw_cmi_show_logw_toggle), main_struct);
        }
}
/******************************** End Signals *********************************/


/**
 * @fn log_window_init_interface(heraia_struct_t *main_struct)
 *  Inits the log window interface
 *  Called once at init time
 * @param main_struct : main structure
 */
void log_window_init_interface(heraia_struct_t *main_struct)
{
    GtkTextView *logw_textview = NULL;
    GtkTextBuffer *tb = NULL;
    GtkTextIter iStart;


    if (main_struct != NULL)
        {
            /* Connecting signals */
            log_window_connect_signals(main_struct);

            /* Creating a "scroll" mark on the textview */
            if (main_struct->xmls != NULL && main_struct->xmls->main != NULL)
                {
                    logw_textview = GTK_TEXT_VIEW(heraia_get_widget(main_struct->xmls->main, "logw_textview"));
                    tb = gtk_text_view_get_buffer(logw_textview);
                    gtk_text_buffer_get_end_iter(tb, &iStart);
                    gtk_text_buffer_create_mark(tb, "scroll", &iStart, TRUE);
                }
        }
}

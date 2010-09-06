/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  result_window.c
  result_window.c - A window that will collect all results of functions such as
                    find, find a data from type, ...

  (C) Copyright 2010 Olivier Delhomme
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
/**
 * @file result_window.c
 *  Manage results coming from functions such as find, find a data from type,
 *  and so on.
 */
#include <libheraia.h>

static gboolean delete_result_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_result_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void result_window_close(GtkWidget *widget, gpointer data);
static void result_window_connect_signal(heraia_struct_t *main_struct);

/**
 * Show result window
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void result_window_show(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *window = NULL;      /**< result window itself */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            window = heraia_get_widget(main_struct->xmls->main, "result_window");
            show_hide_widget(window, TRUE, main_struct->win_prop->result_window);
        }
}


/**
 * Inits all the things in the result window (signal and such)
 * @param main_struct : heraia's main structure
 */
void result_window_init_interface(heraia_struct_t *main_struct)
{
    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {
            result_window_connect_signal(main_struct);
        }
}


/**
 * Call back function for the result window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data :  MUST be heraia_struct_t *main_struct main structure
 */
static gboolean delete_result_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    result_window_close(widget, data);

    return TRUE;
}


/**
 * Call back function for the result window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : user data - MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void destroy_result_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    result_window_close(widget, data);
}


/**
 * Close button has been clicked we want to hide the window
 * @param widget : calling widget
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void result_window_close(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
     GtkWidget *window = NULL;      /**< result window itself */

    if (main_struct != NULL)
        {
             window = heraia_get_widget(main_struct->xmls->main, "result_window");
             show_hide_widget(window, FALSE, main_struct->win_prop->result_window);
        }

}


/**
 * Signal connections for the result window
 * @param main_struct : heraia's main structure
 */
static void result_window_connect_signal(heraia_struct_t *main_struct)
{
    /* Close button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "rw_close_bt")), "clicked",
                     G_CALLBACK(result_window_close), main_struct);

    /* When result window is killed or destroyed */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "result_window")), "delete_event",
                     G_CALLBACK(delete_result_window_event), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "result_window")), "destroy",
                     G_CALLBACK(destroy_result_window_event), main_struct);
}

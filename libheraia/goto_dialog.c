/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  goto_dialog.c
  goto_dialog.c - everything to manage the dialog that lets one go everywhere
                  in the file

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
 * @file goto_dialog.c
 *  File for the goto dialog window that lets one go everywhere in the file.
 */
#include <libheraia.h>

static gboolean delete_goto_dialog_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_goto_dialog_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void goto_dialog_canceled(GtkWidget *widget, gpointer data);
static void goto_dialog_connect_signal(heraia_struct_t *main_struct);


/**
 * Go to..., edit menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_goto_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *dialog = NULL;   /**< dialog window itself */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            dialog = heraia_get_widget(main_struct->xmls->main, "goto_dialog");
            show_hide_widget(dialog, TRUE, main_struct->win_prop->goto_window);
        }
}


/**
 * Inits all the things in the goto dialog window (signal and such)
 * @param main_struct : heraia's main structure
 */
void goto_dialog_init_interface(heraia_struct_t *main_struct)
{
    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {
            goto_dialog_connect_signal(main_struct);
        }
}


/**
 * Call back function for the goto dialog window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data :  MUST be heraia_struct_t *main_struct main structure
 */
static gboolean delete_goto_dialog_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    goto_dialog_canceled(widget, data);

    return TRUE;
}


/**
 * Call back function for the goto dialog window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : user data - not used (may be NULL)
 */
static void destroy_goto_dialog_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    goto_dialog_canceled(widget, data);
}


/**
 * Closing the window effectively
 * @param widget : calling widget
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void goto_dialog_canceled(GtkWidget *widget, gpointer data)
{
     heraia_struct_t *main_struct = (heraia_struct_t *) data;
     GtkWidget *dialog = NULL;   /**< dialog window itself */

     if (main_struct != NULL)
        {
             dialog = heraia_get_widget(main_struct->xmls->main, "goto_dialog");
             show_hide_widget(dialog, FALSE, main_struct->win_prop->goto_window);
        }
}


/**
 * Ok button has been clicked we want to go to the byte number from the entry
 * @param widget : calling widget
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void goto_dialog_ok(GtkWidget *widget, gpointer data)
{
     heraia_struct_t *main_struct = (heraia_struct_t *) data;
     GtkWidget *radio_button = NULL;   /**< a radio button from the group                      */
     GtkWidget *radio_active = NULL;   /**< the radio button which is active within this group */
     const gchar *widget_name = NULL;
     const gchar *entry_text = NULL;
     guint64 offset = 0;


    radio_button = heraia_get_widget(main_struct->xmls->main, "goto_from_beginning");

     if (radio_button != NULL && main_struct->current_doc != NULL)
        {
            entry_text = gtk_entry_get_text(GTK_ENTRY(heraia_get_widget(main_struct->xmls->main, "goto_entry")));

            if (sscanf(entry_text, "%lu", &offset) == 1)
                {

                    radio_active = gtk_radio_button_get_active_from_widget(GTK_RADIO_BUTTON(radio_button));

                     if (radio_active != NULL)
                        {
                            widget_name = gtk_buildable_get_name(GTK_BUILDABLE(radio_active));
                        }

                    /* guessing which radio is active (by name) */
                    if (widget_name != NULL)
                        {
                            if (g_ascii_strcasecmp(widget_name, "goto_from_beginning") == 0)
                                {
                                    ghex_set_cursor_position(main_struct->current_doc->hex_widget, offset - 1);
                                }
                            else if (g_ascii_strcasecmp(widget_name, "goto_from_here_fwd") == 0)
                                {
                                    offset += ghex_get_cursor_position(main_struct->current_doc->hex_widget);
                                    ghex_set_cursor_position(main_struct->current_doc->hex_widget, offset);
                                }
                            else if (g_ascii_strcasecmp(widget_name, "goto_from_here_rwd") == 0)
                                {
                                    offset = ghex_get_cursor_position(main_struct->current_doc->hex_widget) - offset ;
                                    ghex_set_cursor_position(main_struct->current_doc->hex_widget, offset);
                                }
                            else
                                {
                                    offset = ghex_file_size(GTK_HEX(main_struct->current_doc->hex_widget)) - offset;
                                    ghex_set_cursor_position(main_struct->current_doc->hex_widget, offset);
                                }
                        }
                }
        }
}



/**
 * Signal connections for the goto dialog window
 * @param main_struct : heraia's main structure
 */
static void goto_dialog_connect_signal(heraia_struct_t *main_struct)
{
    /* Cancel button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "gtd_cancel_bt")), "clicked",
                     G_CALLBACK(goto_dialog_canceled), main_struct);

    /* Ok button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "gtd_ok_bt")), "clicked",
                     G_CALLBACK(goto_dialog_ok), main_struct);


    /* When goto dialog's window is killed or destroyed */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "goto_dialog")), "delete_event",
                     G_CALLBACK(delete_goto_dialog_event), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "goto_dialog")), "destroy",
                     G_CALLBACK(destroy_goto_dialog_event), main_struct);
}

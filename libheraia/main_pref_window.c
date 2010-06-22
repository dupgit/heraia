/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  main_pref_window.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
 *
 *  (C) Copyright 2008 - 2010 Olivier Delhomme
 *  e-mail : heraia@delhomme.org
 *  URL    : http://heraia.tuxfamily.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or  (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *  MERCHANTABILITY  or  FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/**
 * @file main_pref_window.c
 * Handles main preference window
 */

#include <libheraia.h>

static gboolean pref_window_delete(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void main_pref_window_connect_signals(heraia_struct_t *main_struct);

/* ToolBar buttons */
static void on_mp_tb_fp_bt_toggled(GtkToggleToolButton *tool_button, gpointer data);
static void on_mp_tb_display_bt_toggled(GtkToggleToolButton *tool_button, gpointer data);

/* Toogle Buttons */
static void on_mp_thousand_bt_toggled(GtkToggleButton *toggle_button, gpointer data);
static void on_mp_display_offset_bt_toggled(GtkToggleButton *toggle_button, gpointer data);


/******************************** The Signals *********************************/

/**
 * @fn gboolean pref_window_delete(GtkWidget *widget, GdkEvent *event, gpointer data)
 *  Closing the window
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 * @return Always returns TRUE in order to propagate the signal
 */
static gboolean pref_window_delete(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *pref_window = NULL;

    if (main_struct != NULL && main_struct->win_prop != NULL && main_struct->win_prop->main_pref_window != NULL)
        {
            pref_window = heraia_get_widget(main_struct->xmls->main, "main_preferences_window");
            save_preferences(main_struct);
            record_and_hide_dialog_box(pref_window, main_struct->win_prop->main_pref_window);
        }

    return TRUE;
}


/**
 * @fn void main_pref_window_connect_signals(heraia_struct_t *main_struct)
 *  Connecting the window signals to the right functions
 * @param main_struct : main structure
 */
static void main_pref_window_connect_signals(heraia_struct_t *main_struct)
{
    /* Closing the window */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "main_preferences_window")), "delete-event",
                     G_CALLBACK(pref_window_delete), main_struct);

    /* Clicking on the file preference button of the toolbar */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "mp_tb_fp_bt")), "toggled",
                     G_CALLBACK(on_mp_tb_fp_bt_toggled), main_struct);

    /* Clicking on the display button of the toolbar */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "mp_tb_display_bt")), "toggled",
                     G_CALLBACK(on_mp_tb_display_bt_toggled), main_struct);

    /* Toggling the button to choose to display with separated thousand or not */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "mp_thousand_bt")), "toggled",
                     G_CALLBACK(on_mp_thousand_bt_toggled), main_struct);

    /* Toggling the button to choose to display the offsets or not */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "mp_display_offset_bt")), "toggled",
                     G_CALLBACK(on_mp_display_offset_bt_toggled), main_struct);
}


/**
 * Tool buttons
 */

/**
 * @fn void on_mp_tb_fp_bt_toggled(GtkToolButton *toolbutton, gpointer data)
 *  Main Preferences, ToolBar, File Preference Button
 * @param toolbutton : button that was clicked
 * @param data : user data : MUST be heraia_struct_t *main_struct main structure
 */
static void on_mp_tb_fp_bt_toggled(GtkToggleToolButton *tool_button, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *notebook = NULL;  /* Main Preference Window's Notebook */

    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {
            notebook = heraia_get_widget(main_struct->xmls->main, "mp_first_notebook");
            gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
        }
}


/**
 * @fn void on_mp_tb_display_bt_toggled(GtkToolButton *toolbutton, gpointer data)
 * Main Preferences, ToolBar, Display Button
 * @param toolbutton : button that was clicked
 * @param data : user data : MUST be heraia_struct_t *main_struct main structure
 */
static void on_mp_tb_display_bt_toggled(GtkToggleToolButton *tool_button, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *notebook = NULL;  /* Main Preference Window's Notebook */

    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {

            notebook = heraia_get_widget(main_struct->xmls->main, "mp_first_notebook");
            gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
        }
}


/**
 * @fn void on_mp_thousand_bt_toggled(GtkToggleButton *togglebutton, gpointer data)
 * Refreshes the file labels as an option has been sat
 * @param togglebutton : button that was toggled
 * @param data : user data : MUST be heraia_struct_t *main_struct main structure
 */
static void on_mp_thousand_bt_toggled(GtkToggleButton *toggle_button, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    refresh_file_labels(main_struct);
}


/**
 * Displays or not offsets
 * @param togglebutton : button that was toggled
 * @param data : user data : MUST be heraia_struct_t *main_struct main structure
 */
static void on_mp_display_offset_bt_toggled(GtkToggleButton *toggle_button, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *hex_widget = NULL;


    if (toggle_button != NULL && main_struct != NULL && main_struct->current_doc != NULL)
        {
            hex_widget = main_struct->current_doc->hex_widget;
            gtk_hex_show_offsets(GTK_HEX(hex_widget), gtk_toggle_button_get_active(toggle_button));
        }
}

/******************************** End Signals *********************************/


/**
 * @fn main_pref_window_init_interface(heraia_struct_t *main_struct)
 *  Inits the main preferences window interface
 * @param main_struct : main structure
 */
void main_pref_window_init_interface(heraia_struct_t *main_struct)
{
    main_pref_window_connect_signals(main_struct);
}

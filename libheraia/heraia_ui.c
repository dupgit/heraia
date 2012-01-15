/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_ui.c
  main menus, callback and utility functions

  (C) Copyright 2005 - 2011 Olivier Delhomme
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
/** @file heraia_ui.c
 * This file has all the functions to manage heraia's ui
 * - signals definitions and functions
 * - widgets activations
 * - closing / openning windows
 */

#include <libheraia.h>

static void set_a_propos_properties(GtkWidget *about_dialog);

static gboolean load_heraia_xml(heraia_struct_t *main_struct);

static void heraia_ui_connect_signals(heraia_struct_t *main_struct);

static void record_and_hide_about_box(heraia_struct_t *main_struct);

static gboolean unsaved_documents(heraia_struct_t *main_struct);

static void close_one_document(heraia_struct_t *main_struct, doc_t *closing_doc, gint index);
static gboolean close_a_project(heraia_struct_t *main_struct, const gchar *question);
static gboolean close_heraia(heraia_struct_t *main_struct);

static void on_projects_close_activate(GtkWidget *widget, gpointer data);
static void on_projects_open_activate(GtkWidget *widget, gpointer data);
static void on_projects_save_as_activate(GtkWidget *widget, gpointer data);

/**
 * @fn void on_quit_activate(GtkWidget *widget, gpointer data)
 *  Quit, file menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_quit_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    gboolean quit_heraia = FALSE;

    quit_heraia = close_heraia(main_struct);

    if (quit_heraia == TRUE)
        {
            gtk_main_quit();
        }
}


/**
 * @fn void on_new_activate(GtkWidget *widget, gpointer data)
 *  New, file menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_new_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    Heraia_Document *new_hex_doc = NULL;
    GtkWidget *new_hex_widget = NULL;
    doc_t *doc = NULL;

    new_hex_doc = hex_document_new();

    if (new_hex_doc != NULL)
        {
            new_hex_doc->file_name = g_strdup_printf(Q_("No name"));
            new_hex_doc->changed = TRUE;

            /* creating a new view to this new document */
            new_hex_widget = hex_document_add_view(new_hex_doc);

            /* Sets some option : insert mode */
            gtk_hex_set_insert_mode(GTK_HEX(new_hex_widget), TRUE);
            gtk_hex_show_offsets(GTK_HEX(new_hex_widget), is_toggle_button_activated(main_struct->xmls->main, "mp_display_offset_bt"));

            /* signal connection on cursor moves */
            connect_cursor_moved_signal(main_struct, new_hex_widget);

            /* joining those two new structures in one */
            doc = new_doc_t(new_hex_doc, new_hex_widget);

            /* Adding this new doc to the list of docs (here a GPtrArray) */
            g_ptr_array_add(main_struct->documents, doc);

            add_new_tab_in_main_window(main_struct, doc);
            set_notebook_tab_label_color(main_struct, TRUE);

            grey_main_widgets(main_struct->xmls->main, FALSE);

            log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Hexwidget : %p"), doc->hex_widget);

            /* updating the window name */
            update_main_window_name(main_struct);

            if (main_struct->current_doc != NULL)
                {
                    /* Not thread safe here ? */
                    main_struct->event = HERAIA_REFRESH_NEW_FILE;
                    refresh_event_handler(main_struct->current_doc->hex_widget, main_struct);
                }

        }
}


/**
 * @fn void on_preferences_activate(GtkWidget *widget, gpointer data)
 *  Preferences, file menu :
 *  Displays the preference window (as a modal window)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_preferences_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *pref_window = NULL;

    pref_window = heraia_get_widget(main_struct->xmls->main, "main_preferences_window");

    if (pref_window != NULL)
        {
            move_and_show_dialog_box(pref_window, main_struct->win_prop->main_pref_window);
        }

}


/**
 * @fn void set_a_propos_properties(GtkWidget *about_dialog)
 * Sets name and version in the dialog box
 * @param about_dialog the widget that contain all the about box
 */
static void set_a_propos_properties(GtkWidget *about_dialog)
{

    if (about_dialog != NULL)
        {
           gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), PACKAGE_NAME);
           gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), PACKAGE_VERSION);
        }
}


/**
 * @fn void a_propos_activate(GtkWidget *widget, gpointer data)
 *  Shows apropos's dialog box
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void a_propos_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *about_dialog = NULL;

    about_dialog = heraia_get_widget(main_struct->xmls->main, "about_dialog");

    if (about_dialog != NULL)
        {
            set_a_propos_properties(about_dialog);
            move_and_show_dialog_box(about_dialog, main_struct->win_prop->about_box);
        }
}


/**
 * @fn void move_and_show_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_prop)
 *  Move the dialog box to the wanted position, shows it and says it in the displayed prop
 * @param dialog_box : the dialog box we want to move and show
 * @param dialog_prop : window_prop_t properties structure corresponding to the dialog box
 */
void move_and_show_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_prop)
{
    if (dialog_prop != NULL)
        {

            if (dialog_prop->displayed == FALSE)
                {
                    if (dialog_prop->x > 0 && dialog_prop->y > 0)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), dialog_prop->x, dialog_prop->y);
                        }

                    if (dialog_prop->width > 0 && dialog_prop->height > 0)
                        {
                            gtk_window_resize(GTK_WINDOW(dialog_box), dialog_prop->width, dialog_prop->height);
                        }

                    gtk_widget_show_all(dialog_box);
                    dialog_prop->displayed = TRUE;
                }
        }
}


/**
 * @fn void record_dialog_box_position(GtkWidget *dialog_box, window_prop_t *dialog_prop)
 * Records one dialog position
 * @param dialog_box : a dialog box from which we want to record the position
 * @param[in,out] dialog_prop : window_prop_t properties structure corresponding to the dialog box
 */
void record_dialog_box_position(GtkWidget *dialog_box, window_prop_t *dialog_prop)
{
    gint x = 0;
    gint y = 0;
    gint width = WPT_DEFAULT_WIDTH;
    gint height = WPT_DEFAULT_HEIGHT;

    if (dialog_prop != NULL && dialog_prop->displayed == TRUE)
        {
            if (dialog_box != NULL)
                {
                    gtk_window_get_position(GTK_WINDOW(dialog_box), &x, &y);
                    gtk_window_get_size(GTK_WINDOW(dialog_box), &width, &height);
                    dialog_prop->x = x;
                    dialog_prop->y = y;
                    dialog_prop->width = width;
                    dialog_prop->height = height;
                }
        }
}


/**
 * @fn void record_all_dialog_box_positions(heraia_struct_t *main_struct)
 * Records all the positions of the displayed windows
 * @param[in,out] main_struct : main structure
 */
void record_all_dialog_box_positions(heraia_struct_t *main_struct)
{
    GtkWidget *dialog_box = NULL;

    if (main_struct != NULL &&
        main_struct->xmls != NULL &&
        main_struct->xmls->main != NULL &&
        main_struct->win_prop != NULL &&
        main_struct->current_DW != NULL)
        {
            /* data interpretor */
            dialog_box = main_struct->current_DW->diw;
            record_dialog_box_position(dialog_box, main_struct->win_prop->data_interpretor);

            /* About box */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "about_dialog");
            record_dialog_box_position(dialog_box, main_struct->win_prop->about_box);

            /* Log window */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "log_window");
            record_dialog_box_position(dialog_box, main_struct->win_prop->log_box);

            /* main_dialog */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "main_window");
            record_dialog_box_position(dialog_box, main_struct->win_prop->main_dialog);

            /* plugin_list */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "plugin_list_window");
            record_dialog_box_position(dialog_box, main_struct->win_prop->plugin_list);

            /* list data types */
            /* dialog_box = heraia_get_widget (main_struct->xmls->main, "list_data_types_window");
               record_dialog_box_position(dialog_box, main_struct->win_prop->ldt);
            */

            /* main_preferences */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "main_preferences_window");
            record_dialog_box_position(dialog_box, main_struct->win_prop->main_pref_window);

            /* goto dialog box */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "goto_dialog");
            record_dialog_box_position(dialog_box, main_struct->win_prop->goto_window);

            /* result window */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "result_window");
            record_dialog_box_position(dialog_box, main_struct->win_prop->result_window);

            /* find window */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "find_window");
            record_dialog_box_position(dialog_box, main_struct->win_prop->find_window);

            /* find and replace window */
            dialog_box = heraia_get_widget (main_struct->xmls->main, "fr_window");
            record_dialog_box_position(dialog_box, main_struct->win_prop->fr_window);
        }
}


/**
 * @fn void record_and_hide_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_prop)
 *  Record position and hide a dialog box
 * @param dialog_box : the dialog box we want to record its position and then hide
 * @param dialog_prop : window_prop_t properties structure corresponding to the dialog box
 */
void record_and_hide_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_prop)
{

    if (dialog_prop->displayed == TRUE)
        {
            record_dialog_box_position(dialog_box, dialog_prop);

            gtk_widget_hide(dialog_box);
            dialog_prop->displayed = FALSE;
        }
}


/**
 * @fn static void record_and_hide_about_box(heraia_struct_t *main_struct)
 *  Record position and hide about dialog box
 * @param [in,out] main_struct : main structure
 */
static void record_and_hide_about_box(heraia_struct_t *main_struct)
{
    GtkWidget *about_dialog = NULL;

    about_dialog = heraia_get_widget(main_struct->xmls->main, "about_dialog");

    if (about_dialog != NULL)
        {
            record_and_hide_dialog_box(about_dialog, main_struct->win_prop->about_box);
        }
}


/**
 * @fn void a_propos_response(GtkWidget *widget, gint response, gpointer data)
 *  To close the A propos dialog box (with the "close" button)
 * @param widget : calling widget (may be NULL as we don't use this)
 * @param response : may be whatever you want as we neither use this !
 * @param data : MUST be heraia_struct_t *main_struct main structure
 */
static void a_propos_response(GtkWidget *widget, gint response, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    record_and_hide_about_box(main_struct);
}


/**
 * @fn void a_propos_close(GtkWidget *widget, gpointer data)
 *  To close the A propos dialog box
 * @param widget : calling widget (may be NULL as we don't use this)
 * @param data : MUST be heraia_struct_t *main_struct main structure
 */
static void a_propos_close(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    record_and_hide_about_box(main_struct);
}


/**
 * @fn gboolean a_propos_delete(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  To close the A propos dialog box
 * @param widget : calling widget (may be NULL as we don't use this)
 * @param event : event associated (may be NULL as we don't use this)
 * @param data : MUST be heraia_struct_t *main_struct main structure
 * @return returns TRUE in order to allow other functions to do something with
 *         that event.
 */
static gboolean a_propos_delete(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    record_and_hide_about_box(main_struct);

    return TRUE;
}


/**
 *  Undo, edit menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_undo_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkBuilder *xml = NULL;
    gboolean result = FALSE;

    if (main_struct != NULL && main_struct->current_doc != NULL && main_struct->xmls->main != NULL)
        {
            result = hex_document_undo(main_struct->current_doc->hex_doc);

            xml = main_struct->xmls->main;

            if (result == TRUE)
                {
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_redo"), TRUE);
                }

            if (main_struct->current_doc->hex_doc->undo_top == NULL)
                {   /* No more undos are possible. The document is as the origin ! */
                    set_notebook_tab_label_color(main_struct, FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_undo"), FALSE);
                    main_struct->current_doc->modified = FALSE;
                }
            else
                {
                    set_notebook_tab_label_color(main_struct, TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_undo"), TRUE);
                }

        }

}


/**
 *  Redo, edit menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_redo_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkBuilder *xml = NULL;
    gboolean result = FALSE;

    if (main_struct != NULL && main_struct->current_doc != NULL && main_struct->xmls->main != NULL)
        {
            result = hex_document_redo(main_struct->current_doc->hex_doc);

            xml = main_struct->xmls->main;

            if (result == TRUE)
                {
                    set_notebook_tab_label_color(main_struct, TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_undo"), TRUE);
                    main_struct->current_doc->modified = TRUE;
                }

            if (main_struct->current_doc->hex_doc->undo_stack == NULL || main_struct->current_doc->hex_doc->undo_stack == main_struct->current_doc->hex_doc->undo_top)
                {
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_redo"), FALSE);
                }
            else
                {
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_redo"), TRUE);
                }
        }
}


/**
 *  Delete, edit menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_delete_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            gtk_hex_delete_selection(GTK_HEX(main_struct->current_doc->hex_widget));
            refresh_event_handler(widget, data);
        }
}


/**
 *  Cut, edit menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_cut_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            gtk_hex_cut_to_clipboard(GTK_HEX(main_struct->current_doc->hex_widget));
            refresh_event_handler(widget, data);
        }
}


/**
 *  Copy, edit menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_copy_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            gtk_hex_copy_to_clipboard(GTK_HEX(main_struct->current_doc->hex_widget));
        }
}


/**
 *  Paste, edit menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_paste_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            gtk_hex_paste_from_clipboard(GTK_HEX(main_struct->current_doc->hex_widget));
            refresh_event_handler(widget, data);
        }
}


/**
 * Find, Search menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_find_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            find_window_show(widget, main_struct);
        }
}


/**
 * Find and replace, Search menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_fr_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            fr_window_show(widget, main_struct);
        }
}


/**
 * Find data from type, Search menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_fdft_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            fdft_window_show(widget, main_struct);
        }
}



/**
 *  This function is refreshing the labels on the main
 *  window in order to reflect cursor position, selected
 *  positions and total selected size.
 *  It is also used to refresh the file label on the tab.
 * @param main_struct : main structure
 */
void refresh_file_labels(heraia_struct_t *main_struct)
{
    GtkWidget *position_label = NULL;
    GtkWidget *file_size_label = NULL;
    GtkWidget *file_sel_label = NULL;
    GtkWidget *file_sel_size_label = NULL;
    guint64 position = 0;
    guint64 file_size = 0;
    selection_t *sel = NULL;
    gchar *position_text = NULL;
    gchar *file_size_text = NULL;
    gchar *file_sel_text = NULL;
    gchar *file_sel_size_text = NULL;

    if (main_struct != NULL)
        {
            position_label = heraia_get_widget(main_struct->xmls->main, "file_position_label");
            file_size_label = heraia_get_widget(main_struct->xmls->main, "file_size_label");
            file_sel_label = heraia_get_widget(main_struct->xmls->main, "file_selection_label");
            file_sel_size_label = heraia_get_widget(main_struct->xmls->main, "file_selection_size_label");

            if (main_struct->current_doc != NULL && main_struct->current_doc->hex_widget != NULL)
                {
                    position = ghex_get_cursor_position(main_struct->current_doc->hex_widget);
                    file_size = ghex_file_size((Heraia_Hex *) main_struct->current_doc->hex_widget);
                    sel = ghex_get_selection(main_struct->current_doc->hex_widget);

                    /* position begins at 0 and this is not really human readable */
                    /* it's more confusing than anything so we do + 1             */
                    /* To translators : do not translate <small> and such         */
                    if (is_toggle_button_activated(main_struct->xmls->main, "mp_thousand_bt") == TRUE)
                        {
                            position_text = g_strdup_printf("<small>%'lu</small>", (unsigned long int) (position + 1));
                            file_size_text = g_strdup_printf("<small>%'lu</small>", (unsigned long int) file_size);
                            file_sel_text = g_strdup_printf("<small>%'lu -> %'lu</small>", (unsigned long int) (sel->start + 1), (unsigned long int) (sel->end + 1));
                            file_sel_size_text = g_strdup_printf("<small>%'li</small>", (unsigned long int) ((sel->end - sel->start) + 1));
                        }
                    else
                        {
                            position_text = g_strdup_printf("<small>%lu</small>", (unsigned long int) (position + 1));
                            file_size_text = g_strdup_printf("<small>%lu</small>", (unsigned long int) file_size);
                            file_sel_text = g_strdup_printf("<small>%lu -> %lu</small>", (unsigned long int) (sel->start + 1), (unsigned long int) (sel->end + 1));
                            file_sel_size_text = g_strdup_printf("<small>%li</small>", (unsigned long int) ((sel->end - sel->start) + 1));
                        }

                    gtk_label_set_markup(GTK_LABEL(position_label), position_text);
                    gtk_label_set_markup(GTK_LABEL(file_size_label), file_size_text);
                    gtk_label_set_markup(GTK_LABEL(file_sel_label), file_sel_text);
                    gtk_label_set_markup(GTK_LABEL(file_sel_size_label), file_sel_size_text);

                    g_free(position_text);
                    g_free(file_size_text);
                    g_free(file_sel_text);
                    g_free(file_sel_size_text);
                    g_free(sel);

                    /* refreshing the tab filename itself if necessary only ! */
                    if (main_struct->current_doc->modified != main_struct->current_doc->hex_doc->changed)
                        {
                            main_struct->current_doc->modified = main_struct->current_doc->hex_doc->changed;
                            set_notebook_tab_label_color(main_struct, main_struct->current_doc->hex_doc->changed);

                            /* If the document changes, then when might undo things ... */
                            gtk_widget_set_sensitive(heraia_get_widget(main_struct->xmls->main, "menu_undo"), TRUE);

                        }

                }
            else
                {
                    gtk_label_set_text(GTK_LABEL(position_label), "");
                    gtk_label_set_text(GTK_LABEL(file_size_label), "");
                    gtk_label_set_text(GTK_LABEL(file_sel_label), "");
                    gtk_label_set_text(GTK_LABEL(file_sel_size_label), "");
                }
        }
}


/**
 *  This function is here to ensure that everything will be
 *  refreshed upon a signal event.
 * @warning This function is not thread safe (do not use in a thread)
 * @todo try to put some mutexes on main_struct->event to make this
 *       thread safe some way
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void refresh_event_handler(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL)
        {
            /* Beware, this mechanism is not thread safe ! */
            if (main_struct->event == HERAIA_REFRESH_NOTHING)
                {
                    main_struct->event = HERAIA_REFRESH_CURSOR_MOVE;
                }

            refresh_data_interpretor_window(widget, main_struct);
            refresh_all_plugins(main_struct);
            refresh_file_labels(main_struct);

            main_struct->event = HERAIA_REFRESH_NOTHING;
        }
}


/**
 *  This handles the menuitem "Ouvrir" to open a file
 * @warning This function is not thread safe (do not use in a thread)
 * @todo try to put some mutexes on main_struct->event to make this
 *       thread safe some way
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_open_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GSList *list = NULL;
    GSList *head = NULL;
    gboolean success = FALSE;

    list = select_file_to_load(main_struct, TRUE, N_("Select a file to analyse"));

    if (list != NULL)
        {
            head = list;
            while (list != NULL)
                {
                    success = load_file_to_analyse(main_struct, list->data);
                    g_free(list->data);
                    list = g_slist_next(list);
                }

            g_slist_free(head);

            if (success == TRUE && main_struct->current_doc != NULL)
                 {
                    /* Not thread safe here ? */
                    main_struct->event = HERAIA_REFRESH_NEW_FILE;
                    refresh_event_handler(main_struct->current_doc->hex_widget, main_struct);
                 }
        }
}


/**
 * Searches in a notebook's tabs for a particular widget and returns the number
 * of the corresponding tab if it exists, -1 otherwise
 * @param main_struct : main structure
 * @param notebbok_name : the name of the notebook in the structure
 * @param to_find : a GtkWidget that we want to find in the main notebook tabs
 * @return a gint as tha tab number that contains the widget "to_find" or -1 if
 *         not found
 */
gint find_tab_number_from_widget(heraia_struct_t *main_struct, gchar *notebook_name, GtkWidget *to_find)
{
    GtkWidget *notebook = NULL;  /**< Notenook on the main window         */
    GtkWidget *page = NULL;      /**< pages on the notebook               */
    GtkWidget *tab_label = NULL; /**< tab label GtkWidget (hbox + others) */
    gint nb_pages = 0;           /**< Number of pages in the notebook     */
    gint i = 0;
    gboolean found = FALSE;      /**< True when the widget has been found */
    GList *children = NULL;      /**< Children from the tab label         */

    notebook = heraia_get_widget(main_struct->xmls->main, notebook_name);

    nb_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

    i = 0;
    found = FALSE;
    while (i < nb_pages && found == FALSE)
        {
            page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), i);

            if (page != NULL)
                {
                    tab_label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), page);

                    if (GTK_IS_HBOX(tab_label))
                        {
                            children = gtk_container_get_children(GTK_CONTAINER(tab_label));
                        }

                    while (children != NULL && found == FALSE)
                        {
                            if (children->data == to_find)
                                {
                                    found = TRUE;
                                }
                            else
                                {
                                    children = g_list_next(children);
                                }
                        }
                }

            if (found == FALSE)
                {
                    i++;
                }
        }

    if (found == TRUE)
        {
            return i;
        }
    else
        {
            return -1;
        }
}


/**
 * Closes an entire project
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
static void on_projects_close_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL)
        {
            close_a_project(main_struct, N_("Do you want to close the project"));
        }
}


/**
 * Saves a project in a new file (does not close the project itself)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
static void on_projects_save_as_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    gchar *filename = NULL;  /**< Auto malloc'ed, do not free */

    if (main_struct != NULL)
        {
            filename = select_a_file_to_save(main_struct);

            if (filename != NULL)
                {
                    free_preference_struct(main_struct->prefs);
                    main_struct->prefs = init_preference_struct(g_path_get_dirname(filename),g_path_get_basename(filename));
                    save_preferences(main_struct, main_struct->prefs);
                }
        }
}


/**
 * Opens a project from a file : closes all documents an imports new ones ...
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
static void on_projects_open_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    gchar *filename = NULL;
    GSList *list = NULL;

    if (main_struct != NULL)
        {
            /* We except a list with only one element (no multiple selection allowed) */
            list = select_file_to_load(main_struct, FALSE, (gchar *) Q_("Select a project to load"));

            if (list != NULL && list->data != NULL)
                {
                    /* Closing the projects */
                    on_projects_close_activate(widget, data);

                    /* Opening the new project */
                    filename = list->data;
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Loading project %s"), filename);
                    main_struct->prefs = init_preference_struct(g_path_get_dirname(filename), g_path_get_basename(filename));
                    load_preferences(main_struct, main_struct->prefs);

                    /* . Updating things in the interface */
                    gtk_widget_show_all(heraia_get_widget(main_struct->xmls->main, "file_notebook"));
                    init_window_states(main_struct);
                    g_slist_free(list);
                }
        }
}


/**
 * Closes one document in heraia. Does not do any updates of the interface.
 * @param main_struct : main structure
 * @param closing_doc : the doc_t * document to be closed
 * @param index : the index in the array of documents of the closing_doc
 *                document
 */
static void close_one_document(heraia_struct_t *main_struct, doc_t *closing_doc, gint index)
{
    GtkWidget *notebook = NULL;  /**< Notenook on the main window            */

    if (main_struct != NULL && main_struct->documents != NULL)
        {
            /* Removing the index in the array */
            g_ptr_array_remove_index(main_struct->documents, index);

            /* Removes all results beeing associated with this document */
            rw_remove_all_tabs(main_struct, closing_doc);

            /* And removing it in the notebook */
            notebook = heraia_get_widget(main_struct->xmls->main, "file_notebook");
            gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), index);

            /* kills the widget and the document */
            close_doc_t(closing_doc);
        }
}


/**
 * Closes an opened file
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_close_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    doc_t *closing_doc = NULL;   /**< Current document to close in heraia    */
    doc_t *document = NULL;      /**< To iterate over the array of documents */
    GtkWidget *notebook = NULL;  /**< Notenook on the main window            */
    GtkWidget *dialog = NULL;    /**< The dialog box                         */
    GtkWidget *parent = NULL;    /**< parent widget for the dialog box       */
    gint result = 0;             /**< result from the dialog box             */
    gint index = -1;
    gint i = 0;
    gint tab_number = 0;
    gboolean is_current_doc = FALSE; /**< Says wheter the document that we are
                                          trying to close is the current one or
                                          not                                 */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            /* Guessing where the user asked to close the document */
            if (GTK_IS_BUTTON(widget))
                {   /* From the tab */
                    /* Guessing which document the user has closed */
                    tab_number = find_tab_number_from_widget(main_struct, "file_notebook", widget);
                    closing_doc = g_ptr_array_index(main_struct->documents, tab_number);
                    is_current_doc = (closing_doc == main_struct->current_doc);
                }
            else
                {   /* From the menu */
                    closing_doc = main_struct->current_doc;
                    is_current_doc = TRUE;
                }

            log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Closing document %s"), doc_t_document_get_filename(closing_doc));

            if (closing_doc->modified == TRUE)
                {
                    /* Displays a dialog box that let the user choose what to do */
                    parent = heraia_get_widget(main_struct->xmls->main, "main_window");

                    dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, Q_("This document has been edited and is not saved !"));
                    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), Q_("Do you want to close it without saving it ?"));

                    result = gtk_dialog_run(GTK_DIALOG(dialog));

                    gtk_widget_destroy(dialog);

                    switch (result)
                        {
                            case GTK_RESPONSE_YES: /* Will continue if we say Yes */
                            break;

                            default:  /* Stops closing if we say No */
                                return;
                            break;
                        }
                }

            /* Try to catch the index of the closing document */
            i = 0;
            index = -1;
            while (i < main_struct->documents->len && index == -1)
                {
                    document = g_ptr_array_index(main_struct->documents, i);
                    if (document == closing_doc)
                        {
                            index = i;
                        }
                    i++;
                }

            if (index >= 0)
                {
                    close_one_document(main_struct, closing_doc, index);

                    /* Try to find out the new current document */
                    /* We do not need to update if the current doc is not closed ! */
                    if (is_current_doc == TRUE)
                        {
                            if (main_struct->documents->len > 0)
                                {
                                    /* there is still some documents opened (at least one) */
                                    if (index == 0)
                                        {
                                            main_struct->current_doc = g_ptr_array_index(main_struct->documents, 0);
                                            gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
                                        }
                                    else
                                        {
                                            main_struct->current_doc = g_ptr_array_index(main_struct->documents, index - 1);
                                            gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), index - 1);
                                        }
                                }
                            else
                                {
                                    /* No more documents openned */
                                    main_struct->current_doc = NULL;
                                    grey_main_widgets(main_struct->xmls->main, TRUE);
                                }
                        }
                    /* updating things in conformance to the new situation */
                    refresh_event_handler(notebook, main_struct);
                    update_main_window_name(main_struct);
                }
        }
}


/**
 *  Here we attemp to save the edited file
 *  @todo be more accurate on error (error type, message and filename) returns
 *        we should return something at least ...
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_save_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    HERAIA_ERROR erreur = HERAIA_NOERR;
    gchar *filename = NULL;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            erreur = heraia_hex_document_save(main_struct->current_doc);

            if (erreur != HERAIA_NOERR)
                {
                    filename = doc_t_document_get_filename(main_struct->current_doc);
                    log_message(main_struct, G_LOG_LEVEL_ERROR, Q_("Error while saving file %s !"), filename);
                }
            else
                {
                    set_notebook_tab_label_color(main_struct, FALSE);
                    main_struct->current_doc->modified = FALSE; /* document has just been saved (thus it is not modified !) */
                }
        }
}


/**
 * @fn void on_save_as_activate(GtkWidget *widget, gpointer data)
 *  This handle the save_as menu entry (here the filename changes)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_save_as_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    HERAIA_ERROR erreur = HERAIA_NOERR;
    gchar *filename = NULL;  /**< Auto malloc'ed, do not free */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            filename = select_a_file_to_save(main_struct);

            if (filename != NULL)
                {
                    erreur = heraia_hex_document_save_as(main_struct->current_doc, filename);
                }
            else
                {
                    erreur = HERAIA_CANCELLED;
                }

            if (erreur != HERAIA_NOERR)
                {
                    if (erreur == HERAIA_CANCELLED)
                        {
                            log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Saving file as... : operation cancelled."));
                        }
                    else
                        {
                            log_message(main_struct, G_LOG_LEVEL_ERROR, Q_("Error while saving file as %s"), doc_t_document_get_filename(main_struct->current_doc));
                        }
                }
            else
                {
                    /* updating the window name and tab's name */
                    update_main_window_name(main_struct);
                    set_notebook_tab_name(main_struct);
                    main_struct->current_doc->modified = FALSE; /* document has just been saved (thus it is not modified !) */
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("File %s saved and now edited."), doc_t_document_get_filename(main_struct->current_doc));
                }
        }
}


/**
 * @fn void on_DIMenu_activate(GtkWidget *widget, gpointer data)
 *  This handles the menuitem "Data Interpretor" that
 *  shows or hides the data interpretor window
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_DIMenu_activate(GtkWidget *widget, gpointer data)
{

    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    data_window_t *dw = NULL;      /**< data_window structure for data interpretor */
    GtkNotebook *notebook = NULL;  /**< data interpretor's notebook                */

    if (main_struct != NULL)
        {
            dw = main_struct->current_DW;

            if (dw != NULL)
                {
                    if (dw->diw == NULL)
                        {
                            dw->diw = heraia_get_widget(main_struct->xmls->main, "data_interpretor_window");
                        }

                    if (dw->diw != NULL)
                        {
                            notebook = GTK_NOTEBOOK(heraia_get_widget(main_struct->xmls->main, "diw_notebook"));

                            if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(heraia_get_widget(main_struct->xmls->main, "DIMenu"))) == TRUE)
                                {
                                    /* Setting the first page of the notebook as default (Numbers) */
                                    gtk_notebook_set_current_page(notebook, dw->tab_displayed);

                                    /* moving to the right position */
                                    move_and_show_dialog_box(dw->diw, main_struct->win_prop->data_interpretor);

                                    refresh_data_interpretor_window(widget, data);
                                }
                            else
                                {
                                    /* recording some prefs from the dialog : position + opened tab */
                                    dw->tab_displayed = gtk_notebook_get_current_page(notebook);
                                    record_and_hide_dialog_box(dw->diw, main_struct->win_prop->data_interpretor);
                                }
                        }
                }
        }
}


/**
 * Called when tests submenu is activated
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_tests_menu_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    gboolean result = FALSE;

    if (main_struct != NULL)
        {
            result = tests_ui(main_struct);

            if (result == TRUE)
                {
                    log_message(main_struct, G_LOG_LEVEL_INFO, Q_("All tests went Ok."));
                }
            else
                {
                    log_message(main_struct, G_LOG_LEVEL_WARNING, Q_("Some tests failed."));
                }
        }
}


/**
 * @fn delete_main_struct_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  When the user destroys or delete the main window
 * @param widget : calling widget
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure
 */
gboolean delete_main_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{

    on_quit_activate(widget, data);

    /* If we are leaving heraia, we are not supposed to be here ! */
    return TRUE;
}


/**
 * @fn gboolean delete_dt_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  call back function for the data interpretor window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
gboolean delete_dt_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    g_signal_emit_by_name(heraia_get_widget(main_struct->xmls->main, "DIMenu"), "activate");

    return TRUE;
}


/**
 * @fn void destroy_dt_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  call back function for the data interpretor window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
void destroy_dt_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    g_signal_emit_by_name(heraia_get_widget(main_struct->xmls->main, "DIMenu"), "activate");
}


/**
 * What to do when a change occurs in tabs (user selected a particular
 * tab)
 * @param notebook : the widget that issued this signal
 * @param page : the new current page
 * @param tab_num : index of this page
 * @param data : MUST be heraia_struct_t *main_struct !
 */
gboolean file_notebook_tab_changed(GtkNotebook *notebook, GtkNotebookPage *page, gint tab_num, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL)
        {
            if (tab_num >= 0 && tab_num <= main_struct->documents->len)
                {
                    /* Current document is now from the selected tab */
                    main_struct->current_doc = g_ptr_array_index(main_struct->documents, tab_num);

                    /* Changing main window's name */
                    update_main_window_name(main_struct);

                    /* Refreshing the view */
                    main_struct->event = HERAIA_REFRESH_TAB_CHANGED;
                    refresh_event_handler(GTK_WIDGET(notebook), main_struct);
                    main_struct->event = HERAIA_REFRESH_NOTHING;
                }
        }

    return TRUE;
}

/* End of call back functions that handle the data interpretor window */


/**
 * @fn static gchar *make_absolute_path(gchar *filename)
 *  Returns an absolute path to the filename
 *  the string should be freed when no longer needed
 *  very UGLy !
 * @todo do something without any system calls !!!
 * @param filename : relative notation filename from which to extract an
 *        absolute path
 * @return returns a string with the absolute path which should be freed when
 *         no longer needed
 */
static gchar *make_absolute_path(gchar *filename)
{
    gchar *current_dir = NULL;
    gchar *new_dir = NULL;

    if (g_path_is_absolute(filename) == TRUE)
        {
            /* if the filename is already in an absolute format */
            return  g_path_get_dirname(filename);
        }
    else
        {
            current_dir = g_get_current_dir();
            new_dir = g_path_get_dirname(filename);

            if (g_chdir(new_dir) == 0)
                {
                    g_free(new_dir);
                    new_dir = g_get_current_dir();
                    g_chdir(current_dir);
                    g_free(current_dir);

                    return new_dir;
                }
            else
                {
                    g_free(current_dir);

                    return NULL;
                }
        }
}


/**
 *  Sets the working directory for the file chooser to the directory of the
 *  filename (even if filename is a relative filename such as ../docs/test_file)
 * @param file_chooser : An initialized GtkFileChooser
 * @param filename : a filename (one previously openned)
 */
void set_the_working_directory(GtkFileChooser *file_chooser, gchar *filename)
{
    gchar *dirname = NULL;    /**< directory where we want to be, at first, in the file chooser */

    dirname = make_absolute_path(filename);

    if (dirname != NULL)
        {
            gtk_file_chooser_set_current_folder(file_chooser, dirname);
            g_free(dirname);
        }
}


/**
 *  This function does open a file selector dialog box and returns the selected
 *  filename.
 * @param main_struct : main structure
 * @param multiple : to say wether we want multiple selection be possible or not
 * @param message : the message to print in the title's dialog box
 * @return returns a list of filenames to be loaded (if any)
 */
GSList *select_file_to_load(heraia_struct_t *main_struct, gboolean multiple, gchar *message)
{
    GtkWidget *parent = NULL; /**< A parent window (we use main_struct)            */
    GtkFileChooser *file_chooser = NULL;
    GSList *list = NULL;   /**< list of selected (if any) filenames to be openned  */

    parent = heraia_get_widget(main_struct->xmls->main, "main_window");

    file_chooser = GTK_FILE_CHOOSER(gtk_file_chooser_dialog_new(message,
                                                                GTK_WINDOW(parent),
                                                                GTK_FILE_CHOOSER_ACTION_OPEN,
                                                                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                                GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                                                                NULL));

    gtk_window_set_modal(GTK_WINDOW(file_chooser), TRUE);
    gtk_file_chooser_set_select_multiple(file_chooser, multiple);

    /**
     *  We want the file selection path to be the one of the current
     *  opened file if any !
     */
    if (doc_t_document_get_filename(main_struct->current_doc) != NULL)
       {
            set_the_working_directory(file_chooser, doc_t_document_get_filename(main_struct->current_doc));
       }

    switch (gtk_dialog_run(GTK_DIALOG(file_chooser)))
        {
            case GTK_RESPONSE_OK:

                list = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser));
                gtk_widget_destroy(GTK_WIDGET(file_chooser));

                return list;
             break;

            case GTK_RESPONSE_CANCEL:
            default:
                gtk_widget_destroy(GTK_WIDGET(file_chooser));

                return NULL;
             break;
        }
}


/**
 *  This function opens a dialog box that allow one to choose a
 *  file name to the file which is about to be saved
 * @param main_struct : main structure
 * @return returns complete filename (path and filename)
 */
gchar *select_a_file_to_save(heraia_struct_t *main_struct)
{
    GtkWidget *parent = NULL;     /**< A parent window (we use main_struct) */
    GtkFileChooser *fcd = NULL;
    gchar *filename = NULL;

    parent = heraia_get_widget(main_struct->xmls->main, "main_window");

    /* Selection a name to the file to save */
    fcd = GTK_FILE_CHOOSER(gtk_file_chooser_dialog_new(Q_("Save As..."),
                                                       GTK_WINDOW(parent),
                                                       GTK_FILE_CHOOSER_ACTION_SAVE,
                                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                       GTK_STOCK_SAVE, GTK_RESPONSE_OK,
                                                       NULL));

    /* window properties : modal, without multi-selection and with confirmation */
    gtk_window_set_modal(GTK_WINDOW(fcd), TRUE);
    gtk_file_chooser_set_select_multiple(fcd, FALSE);
    gtk_file_chooser_set_do_overwrite_confirmation(fcd, TRUE);

    /* we do want to have the file's directory where to save the new file */
    if (doc_t_document_get_filename(main_struct->current_doc) != NULL)
       {
            set_the_working_directory(fcd, doc_t_document_get_filename(main_struct->current_doc));
       }

    switch(gtk_dialog_run(GTK_DIALOG(fcd)))
        {
            case GTK_RESPONSE_OK:
                /* retrieving the filename */
                filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fcd));
                break;
            default:
                filename = NULL;
                break;
        }

    gtk_widget_destroy(GTK_WIDGET(fcd));

    return filename;
}


/**
 *  Update main window heraia's name to reflect the current edited file
 * @param main_struct : main structure
 */
void update_main_window_name(heraia_struct_t *main_struct)
{
    GtkWidget *widget = NULL;
    gchar *filename = NULL;
    gchar *whole_filename = NULL;

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            widget = heraia_get_widget(main_struct->xmls->main, "main_window");

            whole_filename = doc_t_document_get_filename(main_struct->current_doc);

            filename = g_filename_display_basename(whole_filename);

            gtk_window_set_title(GTK_WINDOW(widget), filename);
        }
    else if (main_struct != NULL)
        {
            widget = heraia_get_widget(main_struct->xmls->main, "main_window");
            gtk_window_set_title(GTK_WINDOW(widget), "Heraia");
        }
}


/**
 * Tries to find the label contained in the hbox
 * @param hbox : the hbox widget containing one GtkLabel
 * @return the label or NULL if not found
 */
GtkWidget *find_label_from_hbox(GtkWidget *hbox)
{
    GList *children = NULL;     /**< List of children in hbox widget */
    gboolean found = FALSE;

    if (hbox != NULL)
        {
            children = gtk_container_get_children(GTK_CONTAINER(hbox));
            found = FALSE;

            while (children != NULL && found == FALSE)
                {
                    if (GTK_IS_LABEL(children->data))
                        {
                            found = TRUE;
                        }
                    else
                        {
                            children = g_list_next(children);
                        }
                }

            if (found == TRUE)
                {
                    return children->data;
                }
            else
                {
                    return NULL;
                }
        }
    else
        {
            return NULL;
        }
}


/**
 *  Sets notebook's tab's name. This function should only be called
 *  when a new filename was set (open and save as functions)
 * @param main_struct : main structure
 */
void set_notebook_tab_name(heraia_struct_t *main_struct)
{
    GtkWidget *notebook = NULL; /**< file notebook in main window                */
    GtkWidget *page = NULL;     /**< Current page for the file notebook          */
    GtkWidget *hbox = NULL;     /**< container that has the label and the button */
    GtkWidget *label = NULL;    /**< tab's label                                 */
    doc_t *doc = NULL;          /**< corresponding tab's document                */
    gchar *filename = NULL;
    gchar *whole_filename;
    gint current = 0;           /**< index of the current tab displayed          */
    gchar *markup= NULL;        /**< markup text                                 */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            notebook = heraia_get_widget(main_struct->xmls->main, "file_notebook");
            current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
            page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
            hbox = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), page);
            label = find_label_from_hbox(hbox);

            if (label != NULL)
                {
                    doc = g_ptr_array_index(main_struct->documents, current);
                    whole_filename = doc_t_document_get_filename(doc);

                    if (whole_filename != NULL)
                        {
                            filename = g_filename_display_basename(whole_filename);
                            markup =  g_markup_printf_escaped("%s", filename);
                            gtk_label_set_markup(GTK_LABEL(label), markup);
                            gtk_widget_set_tooltip_text(label, g_filename_display_name(whole_filename));
                            g_free(markup);
                        }
                }
        }
}


/**
 * Set the style for the label
 * @param main_struct : main structure
 * @param color : If color is TRUE sets the color for the file tab's label
 *                If not, then sets it to default
 */
void set_notebook_tab_label_color(heraia_struct_t *main_struct, gboolean color)
{
    GtkWidget *notebook = NULL;   /**< file notebook in main window                */
    GtkWidget *page = NULL;       /**< Current page for the file notebook          */
    GtkWidget *label = NULL;      /**< tab's label                                 */
    GtkWidget *menu_label = NULL; /**< menu tab's label                            */
    GtkWidget *hbox = NULL;       /**< container that has the label and the button */
    gint current = 0;             /**< index of the current tab displayed          */
    gchar *markup= NULL;          /**< markup text                                 */
    gchar *text = NULL;           /**< label's text                                */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            notebook = heraia_get_widget(main_struct->xmls->main, "file_notebook");
            current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
            page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
            hbox = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), page);
            label = find_label_from_hbox(hbox);

            if (label != NULL)
                {
                    text = g_strdup(gtk_label_get_text(GTK_LABEL(label)));

                    if (color == TRUE)
                        {
                            markup = g_markup_printf_escaped("<span foreground=\"blue\">%s</span>", text);
                        }
                    else
                        {
                            markup = g_markup_printf_escaped("%s", text);
                        }

                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Changing color for filename %s in tab : %d"), markup, current);
                    gtk_label_set_markup(GTK_LABEL(label), markup);
                    menu_label = gtk_label_new(NULL);
                    gtk_label_set_markup(GTK_LABEL(menu_label), markup);
                    gtk_notebook_set_menu_label(GTK_NOTEBOOK(notebook), page, menu_label);

                    g_free(markup);
                    g_free(text);
                }
        }
}


/**
 * Hides or grey all widgets that needs an open file when boolean greyed is
 * TRUE. Also sets the current page of the notebook to the first one.
 * @param xml : GtkBuilder XML main structure
 * @param greyed : boolean (TRUE to hide an grey widgets)
 */
void grey_main_widgets(GtkBuilder *xml, gboolean greyed)
{
    GtkWidget *notebook = NULL;  /* file notebook in main window */

    if (xml != NULL)
        {
            notebook = heraia_get_widget(xml, "file_notebook");

            if (greyed == TRUE)
                {
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "save"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "save_as"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_cut"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_copy"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_paste"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_delete"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_close"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_goto"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_search"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_projects_close"), FALSE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_projects_save_as"), FALSE);
                    gtk_widget_hide(notebook);
                }
            else
                {
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "save"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "save_as"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_cut"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_copy"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_paste"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_delete"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_close"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_goto"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_search"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_projects_close"), TRUE);
                    gtk_widget_set_sensitive(heraia_get_widget(xml, "menu_projects_save_as"), TRUE);
                    gtk_widget_show_all(notebook);
                }
        }
}


/**
 *  Here we might init some call backs and menu options
 *  and display the interface (main && sub-windows)
 *  This function should be called only once at main program's
 *  init time
 * @param main_struct : main structure
 */
void init_heraia_interface(heraia_struct_t *main_struct)
{

    if (main_struct != NULL)
        {
            /* inits window states (shows or hide windows) */
            init_window_states(main_struct);

            /* menus */
            gtk_widget_set_sensitive(heraia_get_widget(main_struct->xmls->main, "menu_redo"), FALSE);
            gtk_widget_set_sensitive(heraia_get_widget(main_struct->xmls->main, "menu_undo"), FALSE);

            if (main_struct->current_doc != NULL)
                {
                    grey_main_widgets(main_struct->xmls->main, FALSE);
                }
            else
                {
                    grey_main_widgets(main_struct->xmls->main, TRUE);
                }

            refresh_file_labels(main_struct);
        }
}


/**
 *  Loads the GtkBuilder xml files that describes the heraia project
 *  tries the following paths in that order :
 *  - /etc/heraia/heraia.gtkbuilder
 *  - /home/[user]/.heraia/heraia.gtkbuilder
 *  - PWD/heraia.gtkbuilder
 * @param main_struct : main structure
 * @return TRUE if everything went ok, FALSE otherwise
 */
static gboolean load_heraia_xml(heraia_struct_t *main_struct)
{
    gchar *filename = NULL;

    if (main_struct != NULL && main_struct->xmls != NULL)
        {
            filename = g_strdup_printf("heraia.gtkbuilder");
            main_struct->xmls->main = load_xml_file(main_struct->location_list, filename);
            g_free(filename);

            if (main_struct->xmls->main == NULL)
                {
                    return FALSE;
                }
            else
                {
                    /* log_message(main_struct, G_LOG_LEVEL_DEBUG, "%s", gtk_builder_get_translation_domain(main_struct->xmls->main));
                       gtk_builder_set_translation_domain(main_struct->xmls->main, GETTEXT_PACKAGE);
                       log_message(main_struct, G_LOG_LEVEL_DEBUG, "%s", gtk_builder_get_translation_domain(main_struct->xmls->main));
                    */
                    return TRUE;
                }
        }
    else
        {
            return FALSE;
        }
}


/**
 *  Connects the signal that the cursor has moved to
 *  the refreshing function
 * @param main_struct : main structure
 * @param hex_widget : the hex_widget we want to connect the signal to
 */
void connect_cursor_moved_signal(heraia_struct_t *main_struct, GtkWidget *hex_widget)
{
    g_signal_connect(G_OBJECT(hex_widget), "cursor_moved",
                     G_CALLBACK(refresh_event_handler), main_struct);
}


/**
 *  Connect the signals at the interface
 * @param main_struct : main structure
 */
static void heraia_ui_connect_signals(heraia_struct_t *main_struct)
{

    /*** File Menu ***/
    /* Quit, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "quit")), "activate",
                     G_CALLBACK(on_quit_activate), main_struct);

    /* New, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "new")), "activate",
                     G_CALLBACK(on_new_activate), main_struct);

    /* Open, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "open")), "activate",
                     G_CALLBACK(on_open_activate), main_struct);

    /* Close, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_close")), "activate",
                     G_CALLBACK(on_close_activate), main_struct);

    /* Save, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "save")), "activate",
                     G_CALLBACK(on_save_activate), main_struct);

    /* Save As, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "save_as")), "activate",
                     G_CALLBACK(on_save_as_activate), main_struct);

    /**** Projects sub menu ****/
    /* Close, projects sub menu, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_projects_close")), "activate",
                     G_CALLBACK(on_projects_close_activate), main_struct);

    /* Save As, projects sub menu, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_projects_save_as")), "activate",
                     G_CALLBACK(on_projects_save_as_activate), main_struct);

    /* Open, projects sub menu, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_projects_open")), "activate",
                     G_CALLBACK(on_projects_open_activate), main_struct);


    /*** Edit Menu ***/
    /* Goto dialog, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_goto")), "activate",
                     G_CALLBACK(on_goto_activate), main_struct);

    /* Preferences, Edit menu ; See main_pref_window.c for main_pref_window's signals */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "preferences")), "activate",
                     G_CALLBACK(on_preferences_activate), main_struct);

   /* Undo, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_undo")), "activate",
                     G_CALLBACK(on_undo_activate), main_struct);

   /* Redo, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_redo")), "activate",
                     G_CALLBACK(on_redo_activate), main_struct);

    /* Cut, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_cut")), "activate",
                     G_CALLBACK(on_cut_activate), main_struct);

    /* Copy, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_copy")), "activate",
                     G_CALLBACK(on_copy_activate), main_struct);

    /* Paste, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_paste")), "activate",
                     G_CALLBACK(on_paste_activate), main_struct);

    /* Delete, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_delete")), "activate",
                     G_CALLBACK(on_delete_activate), main_struct);


     /*** Search Menu ***/
    /* Find, Search menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_find")), "activate",
                     G_CALLBACK(on_find_activate), main_struct);

    /* Find and replace, Search menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_find_replace")), "activate",
                     G_CALLBACK(on_fr_activate), main_struct);

    /* Find data from type, Search menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_fdft")), "activate",
                     G_CALLBACK(on_fdft_activate), main_struct);


    /*** Display menu ***/
    /* the data interpretor menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "DIMenu")), "activate",
                     G_CALLBACK(on_DIMenu_activate), main_struct);


    /*** Help Menu ***/
    /* Test, Help menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "tests_menu")), "activate",
                     G_CALLBACK(on_tests_menu_activate), main_struct);

    /* about dialog box */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "a_propos")), "activate",
                     G_CALLBACK(a_propos_activate), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "about_dialog")), "close",
                     G_CALLBACK(a_propos_close), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "about_dialog")), "response",
                     G_CALLBACK(a_propos_response), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "about_dialog")), "delete-event",
                     G_CALLBACK(a_propos_delete), main_struct);


    /* main notebook */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "file_notebook")),"switch-page",
                     G_CALLBACK(file_notebook_tab_changed), main_struct);


    /* main window killed or destroyed */
    g_signal_connect(G_OBJECT (heraia_get_widget(main_struct->xmls->main, "main_window")), "delete-event",
                     G_CALLBACK(delete_main_window_event), main_struct);
}


/** @fn int load_heraia_ui(heraia_struct_t *main_struct)
 *  Loads, if possible, the gtkbuilder xml file and then connects the
 *  signals and inits the following windows :
 *  - log window
 *  - data_interpretor window
 *  - list data types
 * @param main_struct : main structure
 * @return TRUE if load_heraia_xml suceeded, FALSE otherwise
 * @todo add more return values to init functions to detect any error while
 *       initializing the ui
 */
int load_heraia_ui(heraia_struct_t *main_struct)
{
    gboolean success = FALSE;

    /* load the XML interfaces (main & treatment) */
    success = load_heraia_xml(main_struct);

    if (success == TRUE)
        {
            /* Heraia UI signals */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("Connecting heraia_ui signals             "));
                }

            heraia_ui_connect_signals(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }

            /* The Log window */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("log window init interface                "));
                }

            log_window_init_interface(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }

            /* Preferences window */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("preferences window init interface        "));
                }

            main_pref_window_init_interface(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }


            /* The data interpretor window */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("data interpretor init interface          "));
                }

            data_interpretor_init_interface(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }


            /* Goto dialog window */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("goto dialog window init interface        "));
                }

            goto_dialog_init_interface(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }


            /* result window */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("result window init interface             "));
                }

            result_window_init_interface(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }

            /* find window */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("find window init interface               "));
                }

            find_window_init_interface(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }

            /* find and replace window */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("find and replace window init interface   "));
                }

            fr_window_init_interface(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }
            /* find and replace window */
            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_("find data from type window init interface"));
                }

            fdft_window_init_interface(main_struct);

            if (main_struct->debug == TRUE)
                {
                    fprintf(stdout, Q_(" [Done]\n"));
                }

            /* preferences file - Setting up preferences */
            fprintf(stdout, Q_("Loading heraia preference file"));
            fprintf(stdout, Q_("Setting up preferences...\n"));
            load_preferences(main_struct, main_struct->prefs);
            fprintf(stdout, Q_("[Done]\n"));
        }

    return success;
}


/**
 * @fn void add_text_to_textview(GtkTextView *textview, const char *format, ...)
 *  adds a text to a textview
 * @param textview : the textview where to add text
 * @param format : printf style format
 * @param ... : a va_list arguments to fit format (as with printf)
 */
void add_text_to_textview(GtkTextView *textview, const char *format, ...)
{
    va_list args;
    GtkTextBuffer *tb = NULL;
    GtkTextIter iEnd;
    gchar *display = NULL;
    GError *err = NULL;

    va_start(args, format);
    display = g_locale_to_utf8(g_strdup_vprintf(format, args), -1, NULL, NULL, &err);
    va_end(args);

    tb = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)));
    gtk_text_buffer_get_end_iter(tb, &iEnd);
    gtk_text_buffer_insert(tb, &iEnd, display, -1);
    g_free(display);
}


/**
 * @fn kill_text_from_textview(GtkTextView *textview)
 *  Kills the text from a textview
 * @param textview : the textview to kill the text from
 */
void kill_text_from_textview(GtkTextView *textview)
{
    GtkTextBuffer *tb = NULL;
    GtkTextIter iStart;
    GtkTextIter iEnd;

    tb = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)));
    gtk_text_buffer_get_start_iter(tb, &iStart);
    gtk_text_buffer_get_end_iter(tb, &iEnd);
    gtk_text_buffer_delete (tb, &iStart, &iEnd);
}


/**
 * @fn GtkWidget *gtk_radio_button_get_active(GSList *group)
 *  Try to find the active radio button widget in a group
 *  This does not take into account inconsistant states
 *  returns the first active radio button otherwise NULL
 * @param group : A group of GtkRadioButtons
 * @return returns the active widget if any (NULL if none)
 */
GtkWidget *gtk_radio_button_get_active(GSList *group)
{
    GSList *tmp_slist = group;
    GtkToggleButton *toggle_button = NULL;

    while (tmp_slist)
        {
            toggle_button = tmp_slist->data;

            if (gtk_toggle_button_get_active(toggle_button))
                {
                    return GTK_WIDGET(toggle_button);
                }

            tmp_slist = g_slist_next(tmp_slist);
        }

    return NULL;
}


/**
 * @fn GtkWidget *gtk_radio_button_get_active_from_widget(GtkRadioButton *radio_group_member)
 * gets the active radio button from a radio group
 * @param radio_button : GtkRadioButton to get radio group from
 * @returns the active GtkRadioButton within the group from
 *          radio_button
 **/
GtkWidget *gtk_radio_button_get_active_from_widget(GtkRadioButton *radio_button)
{
    if (radio_button != NULL)
        {
            return gtk_radio_button_get_active(gtk_radio_button_get_group(radio_button));
        }
    else
        {
            return NULL;
        }
}


/**
 * Sets the radio button active
 * @param radio_button : The GtkRadioButton to be active within it's group
 */
void gtk_radio_button_set_active(GtkRadioButton *radio_button)
{
    GSList *group = NULL;
    GtkToggleButton *toggle_button = NULL;

    group = gtk_radio_button_get_group(radio_button);

    while (group)
        {
            toggle_button = group->data;

            if (toggle_button == GTK_TOGGLE_BUTTON(radio_button))
                {
                   gtk_toggle_button_set_active(toggle_button, TRUE);
                }
            else
                {
                    gtk_toggle_button_set_active(toggle_button, FALSE);
                }

            group = g_slist_next(group);
        }
}


/**
 * @fn gboolean is_cmi_checked(GtkWidget *check_menu_item)
 *  Tells whether a GtkCheckMenuItem is Checked or not
 * @param check_menu_item : a GtkCheckMenuItem to verify
 * @return returns TRUE if the Check Manu Item is checked, FALSE otherwise
 */
gboolean is_cmi_checked(GtkWidget *check_menu_item)
{
    return gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(check_menu_item));
}


/**
 * @fn gboolean is_toggle_button_activated(GtkBuilder *main_xml, gchar *check_button)
 *  returns the state of a named check button contained
 *  in the GtkBuilder XML description
 * @param main_xml : a GtkBuilder XML definition
 * @param check_button : the name of an existing check_button within the GtkBuilder
 *        definition
 * @return TRUE if the button is activated / toggled , FALSE otherwise
 */
gboolean is_toggle_button_activated(GtkBuilder *main_xml, gchar *check_button)
{
    gboolean activated = FALSE;

    if (main_xml != NULL)
        {
            activated = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(heraia_get_widget(main_xml, check_button)));
        }

    return activated;
}


/**
 *  This is a wrapper to the GtkBuilder xml get widget. It is intended
 *  to simplify the developpers lives if they have to choose or
 *  propose other means to do the same thing than libglade (say,
 *  for example, GtkBuilder ;-)
 * @param xml : A GtkBuilder XML definition
 * @param widget_name : an existing widget name in the GtkBuilder definition
 * @return returns the widget itself if it exists in the definition file (NULL
 *         otherwise)
 */
GtkWidget *heraia_get_widget(GtkBuilder *xml, gchar *widget_name)
{
   /**
    * For debug purposes only (very verbose as this function is the main used)
    * fprintf(stdout, "Getting Widget named %s\n", widget_name);
    */

    if (xml != NULL && widget_name != NULL)
        {
            return GTK_WIDGET(gtk_builder_get_object(xml, widget_name));
        }
    else
        {
            return NULL;
        }
}


/**
 * @fn void destroy_a_single_widget(GtkWidget *widget)
 *  Destroys a single widget if it exists
 * @param widget : the widget to destroy
 */
void destroy_a_single_widget(GtkWidget *widget)
{
    if (widget != NULL)
        {
            gtk_widget_destroy(widget);
        }
}


/**
 * Verify if we can safely close everything
 * @param main_struct : main structure
 * @return a boolean which is TRUE if unsaved documents still exists and FALSE
 *         otherwise
 */
static gboolean unsaved_documents(heraia_struct_t *main_struct)
{
    doc_t *current_doc = NULL;
    gboolean result = FALSE;
    guint i = 0;

    if (main_struct != NULL && main_struct->documents != NULL)
        {
            for(i = 0; i < main_struct->documents->len; i++)
                {
                    current_doc =  g_ptr_array_index(main_struct->documents, i);
                    result = result | current_doc->modified;
                }

            return result;
        }

    return result;
}


/**
 * Closes all documents and saves preferences if the users wants to close
 * the documents.
 * @param main_struct : main_struct
 * @param question a const gchar * string to be displayed when an unsaved
 *        document is found. This should ask the user what to do.
 * @return the user anwser if an unsaved document is found and TRUE if not
 */
static gboolean close_a_project(heraia_struct_t *main_struct, const gchar *question)
{
    gboolean unsaved = FALSE;    /**< if there is any unsaved documents */
    gboolean quit_heraia = TRUE; /**< By default we want to quit        */
    GtkWidget *dialog = NULL;    /**< The dialog box                    */
    GtkWidget *parent = NULL;    /**< parent widget for the dialog box  */
    gint result = 0;             /**< result from the dialog box        */
    doc_t *closing_doc = NULL;   /**< The document to be closed         */

    unsaved = unsaved_documents(main_struct);

    if (unsaved == TRUE)
        {
            /* Displays a dialog box that let the user choose what to do */
            parent = heraia_get_widget(main_struct->xmls->main, "main_window");

            dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, Q_("Unsaved document(s) remains."));
            gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), "%s", question);


            result = gtk_dialog_run(GTK_DIALOG(dialog));

            switch (result)
                {
                    case GTK_RESPONSE_YES:
                        quit_heraia = TRUE;
                    break;

                    default:
                        quit_heraia = FALSE;
                    break;
                }

            gtk_widget_destroy(dialog);
        }

    if (quit_heraia == TRUE)
        {
            /* . Recording window's position       */
            record_all_dialog_box_positions(main_struct);

            /* . Saving preferences                */
            save_preferences(main_struct, main_struct->prefs);

            /* . Closing the documents             */
            while (main_struct->documents->len > 0)
                {
                    closing_doc = g_ptr_array_index(main_struct->documents, 0);
                    close_one_document(main_struct, closing_doc, 0);
                }
            main_struct->current_doc = NULL;

             /* . Updating things in the interface */
            refresh_event_handler(parent, main_struct);
            update_main_window_name(main_struct);
            grey_main_widgets(main_struct->xmls->main, TRUE);

            /* . Destroying preferences structure  */
            free_preference_struct(main_struct->prefs);
            main_struct->prefs = NULL;
        }

    return quit_heraia;
}


/**
 * @fn void close_heraia(heraia_struct_t *main_struct)
 * Before closing heraia we need to do few things
 * @param main_struct : main_struct
 * @return TRUE if we can safely quit heraia, FALSE otherwise
 */
static gboolean close_heraia(heraia_struct_t *main_struct)
{
   return close_a_project(main_struct, Q_("Do you want to quit without saving ?"));
}


/**
 * @fn void init_one_cmi_window_state(GtkWidget *dialog_box, GtkWidget *cmi, window_prop_t *dialog_prop)
 * init one cmi window based state
 * @param dialog_box : the window or dialog box we want to init its state
 * @param cmi : corresponding check menu item
 * @param dialog_prop : corresponding window properties (should be initialized and not NULL)
 */
static void init_one_cmi_window_state(GtkWidget *dialog_box, GtkWidget *cmi, window_prop_t *dialog_prop)
{
    gboolean activated = FALSE;

    if (dialog_box != NULL && cmi != NULL && dialog_prop != NULL)
        {
            activated = dialog_prop->displayed;
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(cmi), activated);
            if (activated == TRUE)
                {
                    gtk_window_move(GTK_WINDOW(dialog_box), dialog_prop->x, dialog_prop->y);
                    gtk_window_resize(GTK_WINDOW(dialog_box), dialog_prop->width, dialog_prop->height);
                    gtk_widget_show_all(dialog_box);
                }
        }
}


/**
 * @fn init_window_states(heraia_struct_t *main_struct)
 *  Inits all windows states (positions, displayed, and so on...)
 * @param main_struct : main structure
 */
void init_window_states(heraia_struct_t *main_struct)
{
    GtkWidget *cmi = NULL;
    GtkWidget *dialog_box = NULL;

    if (main_struct != NULL && main_struct->xmls != NULL  && main_struct->xmls->main != NULL)
        {
            if (main_struct->win_prop)
                {
                    /* Main window (always the first one) */
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "main_window");
                    if (main_struct->win_prop->main_dialog->displayed == TRUE)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), main_struct->win_prop->main_dialog->x, main_struct->win_prop->main_dialog->y);
                            gtk_window_resize(GTK_WINDOW(dialog_box), main_struct->win_prop->main_dialog->width, main_struct->win_prop->main_dialog->height);
                            gtk_widget_show(dialog_box);
                        }

                    /* Log Window Interface */
                    cmi = heraia_get_widget(main_struct->xmls->main, "mw_cmi_show_logw");
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "log_window");
                    init_one_cmi_window_state(dialog_box, cmi, main_struct->win_prop->log_box);

                    /* Data Interpretor Interface */
                    cmi = heraia_get_widget(main_struct->xmls->main, "DIMenu");
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "data_interpretor_window");
                    init_one_cmi_window_state(dialog_box, cmi, main_struct->win_prop->data_interpretor);
                    on_DIMenu_activate(cmi, main_struct);

                    /* List Data type Interface */
                    /*
                     * cmi = heraia_get_widget(main_struct->xmls->main, "ldt_menu");
                     * dialog_box = heraia_get_widget(main_struct->xmls->main, "list_data_types_window");
                     * init_one_cmi_window_state(dialog_box, cmi, main_struct->win_prop->ldt);
                     */

                    /* Plugin List Interface */
                    cmi = heraia_get_widget(main_struct->xmls->main, "mw_cmi_plugin_list");
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "plugin_list_window");
                    init_one_cmi_window_state(dialog_box, cmi, main_struct->win_prop->plugin_list);

                    /* Preferences window */
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "main_preferences_window");
                    if (main_struct->win_prop->main_pref_window->displayed == TRUE)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), main_struct->win_prop->main_pref_window->x, main_struct->win_prop->main_pref_window->y);
                            gtk_window_resize(GTK_WINDOW(dialog_box), main_struct->win_prop->main_pref_window->width, main_struct->win_prop->main_pref_window->height);
                            gtk_widget_show_all(dialog_box);
                        }

                    /* Goto dialog window */
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "goto_dialog");
                    if (main_struct->win_prop->goto_window->displayed == TRUE && main_struct->current_doc != NULL)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), main_struct->win_prop->goto_window->x, main_struct->win_prop->goto_window->y);
                            gtk_window_resize(GTK_WINDOW(dialog_box), main_struct->win_prop->goto_window->width, main_struct->win_prop->goto_window->height);
                            gtk_widget_show_all(dialog_box);
                        }
                    else
                        {
                            /* if the window is not displayed modifies it's properties accordingly */
                            main_struct->win_prop->goto_window->displayed = FALSE;
                        }

                    /* result window */
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "result_window");
                    cmi = heraia_get_widget(main_struct->xmls->main, "menu_result");
                    if (main_struct->win_prop->result_window->displayed == TRUE && main_struct->current_doc != NULL)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), main_struct->win_prop->result_window->x, main_struct->win_prop->result_window->y);
                            gtk_window_resize(GTK_WINDOW(dialog_box), main_struct->win_prop->result_window->width, main_struct->win_prop->result_window->height);
                            gtk_widget_show_all(dialog_box);
                        }
                    else
                        {
                            main_struct->win_prop->result_window->displayed = FALSE;
                        }
                    init_one_cmi_window_state(dialog_box, cmi, main_struct->win_prop->result_window);

                    /* find window */
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "find_window");
                    if (main_struct->win_prop->find_window->displayed == TRUE && main_struct->current_doc != NULL)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), main_struct->win_prop->find_window->x, main_struct->win_prop->find_window->y);
                            gtk_window_resize(GTK_WINDOW(dialog_box), main_struct->win_prop->find_window->width, main_struct->win_prop->find_window->height);
                            gtk_widget_show_all(dialog_box);
                        }
                    else
                        {
                            /* if the window is not displayed modifies it's properties accordingly */
                            main_struct->win_prop->find_window->displayed = FALSE;
                        }

                    /* find and replace window */
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "fr_window");
                    if (main_struct->win_prop->fr_window->displayed == TRUE && main_struct->current_doc != NULL)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), main_struct->win_prop->fr_window->x, main_struct->win_prop->fr_window->y);
                            gtk_window_resize(GTK_WINDOW(dialog_box), main_struct->win_prop->fr_window->width, main_struct->win_prop->fr_window->height);
                            gtk_widget_show_all(dialog_box);
                        }
                    else
                        {
                            /* if the window is not displayed modifies it's properties accordingly */
                            main_struct->win_prop->fr_window->displayed = FALSE;
                        }

                    /* find data from type window */
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "fdft_window");
                    if (main_struct->win_prop->fdft_window->displayed == TRUE && main_struct->current_doc != NULL)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), main_struct->win_prop->fdft_window->x, main_struct->win_prop->fdft_window->y);
                            gtk_window_resize(GTK_WINDOW(dialog_box), main_struct->win_prop->fdft_window->width, main_struct->win_prop->fdft_window->height);
                            gtk_widget_show_all(dialog_box);
                        }
                    else
                        {
                            /* if the window is not displayed modifies it's properties accordingly */
                            main_struct->win_prop->fdft_window->displayed = FALSE;
                        }


                    /* About Box */
                    dialog_box = heraia_get_widget(main_struct->xmls->main, "about_dialog");
                    if (main_struct->win_prop->about_box->displayed == TRUE)
                        {
                            gtk_window_move(GTK_WINDOW(dialog_box), main_struct->win_prop->about_box->x, main_struct->win_prop->about_box->y);
                            gtk_window_resize(GTK_WINDOW(dialog_box), main_struct->win_prop->about_box->width, main_struct->win_prop->about_box->height);
                            set_a_propos_properties(dialog_box);
                            gtk_widget_show_all(dialog_box);
                        }
                }
        }
}


/**
 * Creates an hbox containning a cross button (in order to close the tab) and
 * a label (from tab_label).
 * Creates a label an a button to add into a tab from main window's notebook
 * @param main_struct : main structure
 * @param tab_label : a GtkWidget that is the label we want to add to the tab
 * @param signal_handler : the signal to connect to when the close button is
 *                         clicked.
 * @return a newly created GtkWidget which contains the label and a close button
 */
GtkWidget *create_tab_close_button(heraia_struct_t *main_struct, GtkWidget *tab_label, void *signal_handler)
{
    GtkWidget *hbox = NULL;    /**< used for hbox creation in the tabs */
    GtkWidget *button = NULL;  /**< Closing button                     */

    /* Close button in tabs */
    hbox = gtk_hbox_new(FALSE, 0);

    button = gtk_button_new_with_label("x");
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    gtk_widget_set_size_request(button, 18, 17);
    gtk_widget_set_tooltip_text(button, Q_("Close button"));
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(signal_handler), main_struct);

    /* Packing label and button all together in order to display everything in the tab */
    gtk_box_pack_start(GTK_BOX(hbox), tab_label, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 2);
    gtk_widget_show_all(hbox);

    return hbox;
}


/**
 * Adds a new tab to the main window in file's notebook
 * @param main_struct : main structure
 * @param doc : the new document that will be related to the tab
 */
void add_new_tab_in_main_window(heraia_struct_t *main_struct, doc_t *doc)
{
    GtkWidget *vbox = NULL;       /**< used for vbox creation               */
    GtkNotebook *notebook = NULL; /**< file_notebook from heraia.gtkbuilder */
    GtkWidget *tab_label = NULL;  /**< tab's label                          */
    GtkWidget *menu_label = NULL; /**<menu's label                          */
    gint tab_num = -1;            /**< new tab's index                      */
    gchar *filename = NULL;
    gchar *whole_filename = NULL;
    gchar *markup = NULL;         /**< markup text                          */
    gchar *menu_markup = NULL;    /**< menu markup text                     */
    GtkWidget *hbox = NULL;       /**< used for hbox creation in the tabs   */

    notebook = GTK_NOTEBOOK(heraia_get_widget(main_struct->xmls->main, "file_notebook"));
    vbox = gtk_vbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), doc->hex_widget, TRUE, TRUE, 3);

    /* tab's label and menu label */
    tab_label = gtk_label_new(NULL);
    menu_label = gtk_label_new(NULL);
    whole_filename = doc_t_document_get_filename(doc);

    if (whole_filename != NULL)
        {
            filename = g_filename_display_basename(whole_filename);
            markup = g_markup_printf_escaped("%s", filename);
            menu_markup = g_markup_printf_escaped("%s", filename);
            gtk_label_set_markup(GTK_LABEL(tab_label), markup);
            gtk_label_set_markup(GTK_LABEL(menu_label), menu_markup);
            gtk_label_set_justify(GTK_LABEL(menu_label), GTK_JUSTIFY_LEFT);
            gtk_widget_set_tooltip_text(tab_label, g_filename_display_name(whole_filename));
            g_free(markup);
            g_free(menu_markup);
        }

    hbox = create_tab_close_button(main_struct, tab_label, on_close_activate);

    gtk_widget_show_all(vbox);
    tab_num = gtk_notebook_append_page_menu(notebook, vbox, hbox, menu_label);

    main_struct->current_doc = doc;
    gtk_notebook_set_current_page(notebook, tab_num);
}


/**
 *  To help plugins to deal with widgets, shows or hide a specific widget
 * @param widget : the widget to show or hide
 * @param show : what to do : TRUE to show the widget, FALSE to hide it
 * @param win_prop : window properties.
 */
void show_hide_widget(GtkWidget *widget, gboolean show, window_prop_t *win_prop)
{
    if (widget != NULL)
        {
            if (win_prop != NULL)
                    {
                        if (show)
                            {
                                move_and_show_dialog_box(widget, win_prop);
                            }
                        else
                            {
                                record_and_hide_dialog_box(widget, win_prop);
                            }
                    }
                else
                    {
                        if (show)
                            {
                                gtk_widget_show(widget);
                            }
                        else
                            {
                                gtk_widget_hide(widget);
                            }
                    }
        }
}

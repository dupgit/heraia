/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_ui.c
  main menus, callback and utility functions

  (C) Copyright 2005 - 2009 Olivier Delhomme
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
static void close_heraia(heraia_struct_t *main_struct);

/**
 * @fn void on_quit_activate(GtkWidget *widget, gpointer data)
 *  Quit, file menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_quit_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    close_heraia(main_struct);
    gtk_main_quit();
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

    log_message(main_struct, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
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
        if (GTK_MINOR_VERSION >= 12)
        {
            gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), PACKAGE_NAME);
        }
        if (GTK_MINOR_VERSION >= 6)
        {
            gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), PACKAGE_VERSION);
        }
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
        dialog_box = heraia_get_widget (main_struct->xmls->main, "list_data_types_window");
        record_dialog_box_position(dialog_box, main_struct->win_prop->ldt);

        /* main_preferences */
        dialog_box = heraia_get_widget (main_struct->xmls->main, "main_preferences_window");
        record_dialog_box_position(dialog_box, main_struct->win_prop->main_pref_window);
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
 * @fn void on_delete_activate(GtkWidget *widget, gpointer data)
 *  Delete, edit menu
 * @warning Not yet implemented
 * @todo Write a usefull function here :)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_delete_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    log_message(main_struct, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

/**
 * @fn void on_cut_activate(GtkWidget *widget, gpointer data)
 *  Cut, edit menu
 * @warning Not yet implemented
 * @todo Write a usefull function here :)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_cut_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    log_message(main_struct, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

/**
 * @fn void on_copy_activate( GtkWidget *widget, gpointer data )
 *  Copy, edit menu
 * @warning Not yet implemented
 * @todo Write a usefull function here :)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_copy_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    log_message(main_struct, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}


/**
 * @fn void on_paste_activate( GtkWidget *widget, gpointer data )
 *  Paste, edit menu
 * @warning Not yet implemented
 * @todo Write a usefull function here :)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void on_paste_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    log_message(main_struct, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}


/**
 * @fn void refresh_file_labels(heraia_struct_t *main_struct)
 *  This function is refreshing the labels on the main
 *  window in order to reflect cursor position, selected
 *  positions and total selected size
 * @param main_struct : main structure
 */
void refresh_file_labels(heraia_struct_t *main_struct)
{
    GtkWidget *position_label = NULL;
    GtkWidget *file_size_label = NULL;
    guint64 position = 0;
    guint64 file_size = 0;
    gchar *position_text = NULL;
    gchar *file_size_text = NULL;

    if (main_struct != NULL)
        {
            position_label = heraia_get_widget(main_struct->xmls->main, "file_position_label");
            file_size_label = heraia_get_widget(main_struct->xmls->main, "file_size_label");

            if (main_struct->current_doc != NULL && main_struct->current_doc->hex_widget != NULL)
                {
                    position = ghex_get_cursor_position(main_struct->current_doc->hex_widget);
                    file_size = ghex_file_size((Heraia_Hex *) main_struct->current_doc->hex_widget);

                    /* position begins at 0 and this is not really human readable */
                    /* it's more confusing than anything so we do + 1             */
                    /* To translators : do not translate <small> and such         */
                    if (is_toggle_button_activated(main_struct->xmls->main, "mp_thousand_bt") == TRUE)
                    {
                        position_text = g_strdup_printf("<small>%'lld</small>", position + 1);
                        file_size_text = g_strdup_printf("<small>%'lld</small>", file_size);
                    }
                    else
                    {
                        position_text = g_strdup_printf("<small>%lld</small>", position + 1);
                        file_size_text = g_strdup_printf("<small>%lld</small>", file_size);
                    }

                    gtk_label_set_markup(GTK_LABEL(position_label), position_text);
                    gtk_label_set_markup(GTK_LABEL(file_size_label), file_size_text);

                    g_free(position_text);
                    g_free(file_size_text);

                }
            else
                {
                    gtk_label_set_text(GTK_LABEL(position_label), "");
                    gtk_label_set_text(GTK_LABEL(file_size_label), "");
                }
        }
}


/**
 * @fn void refresh_event_handler(GtkWidget *widget, gpointer data)
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
 * @fn void on_open_activate(GtkWidget *widget, gpointer data)
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

    list = select_file_to_load(main_struct);

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
 * @fn void on_save_activate(GtkWidget *widget, gpointer data)
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
            log_message(main_struct, G_LOG_LEVEL_ERROR, "Error while saving file %s !", filename);
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
                log_message(main_struct, G_LOG_LEVEL_DEBUG, "Saving file as... : operation cancelled.");
            }
            else
            {
                log_message(main_struct, G_LOG_LEVEL_ERROR, "Error while saving file as %s", doc_t_document_get_filename(main_struct->current_doc));
            }
        }
        else
        {
            /* updating the window name and tab's name */
            update_main_struct_name(main_struct);
            set_notebook_tab_name(main_struct);
            log_message(main_struct, G_LOG_LEVEL_DEBUG, "File %s saved and now edited.", doc_t_document_get_filename(main_struct->current_doc));
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

                if (main_struct->win_prop->data_interpretor->displayed == FALSE)
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
                    log_message(main_struct, G_LOG_LEVEL_INFO, "All tests went Ok.");
                }
            else
                {
                    log_message(main_struct, G_LOG_LEVEL_WARNING, "Some tests failed.");
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
gboolean delete_main_struct_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{

    on_quit_activate(widget, data);

    return FALSE;
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
 * tab
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
        main_struct->current_doc = g_ptr_array_index(main_struct->documents, tab_num);
        update_main_struct_name(main_struct);
        main_struct->event = HERAIA_REFRESH_TAB_CHANGED;
        refresh_event_handler(GTK_WIDGET(notebook), main_struct);
        main_struct->event = HERAIA_REFRESH_NOTHING;

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
 * @return returns a list of filenames to be loaded (if any)
 */
GSList *select_file_to_load(heraia_struct_t *main_struct)
{
    GtkWidget *parent = NULL; /**< A parent window (we use main_struct)            */
    GtkFileChooser *file_chooser = NULL;
    GSList *list = NULL;   /**< list of selected (if any) filenames to be openned  */

    parent = heraia_get_widget(main_struct->xmls->main, "main_window");

    file_chooser = GTK_FILE_CHOOSER(gtk_file_chooser_dialog_new("Select a file to analyse",
                                                                GTK_WINDOW(parent),
                                                                GTK_FILE_CHOOSER_ACTION_OPEN,
                                                                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                                GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                                                                NULL));

    /**
     *  for the moment we do not want to retrieve multiples selections
     *  but this could be a valuable thing in the future
     */
    gtk_window_set_modal(GTK_WINDOW(file_chooser), TRUE);
    gtk_file_chooser_set_select_multiple(file_chooser, TRUE);

    /**
     *  We want the file selection path to be the one of the previous
     *  openned file if any !
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
 * @fn gchar *select_a_file_to_save(heraia_struct_t *main_struct)
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
    fcd = GTK_FILE_CHOOSER(gtk_file_chooser_dialog_new("Save As...",
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
 * @fn void update_main_struct_name(heraia_struct_t *main_struct)
 *  Update main window heraia's name to reflect the current edited file
 * @param main_struct : main structure
 */
void update_main_struct_name(heraia_struct_t *main_struct)
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
}

/**
 * @fn void set_notebook_tab_name(heraia_struct_t *main_struct)
 *  Sets notebook's tab's name. This function should only be called
 *  when a new filename was set (open and save as functions)
 * @param main_struct : main structure
 */
void set_notebook_tab_name(heraia_struct_t *main_struct)
{
    GtkWidget *notebook = NULL; /* file notebook in main window       */
    GtkWidget *page = NULL;     /* Current page for the file notebook */
    GtkWidget *label = NULL;    /* tab's label                        */
    doc_t *doc = NULL;          /* corresponding tab's document       */
    gchar *filename = NULL;
    gchar *whole_filename;
    gint current = 0;

    if (main_struct != NULL && main_struct->current_doc != NULL)
       {
           notebook = heraia_get_widget(main_struct->xmls->main, "file_notebook");
           current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
           page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
           label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), page);

           doc = g_ptr_array_index(main_struct->documents, current);
           whole_filename = doc_t_document_get_filename(doc);

           if (whole_filename != NULL)
           {
                filename = g_filename_display_basename(whole_filename);
                gtk_label_set_text(GTK_LABEL(label), filename);
                gtk_widget_set_tooltip_text(label, g_filename_display_name(whole_filename));
           }
       }
}


/**
 * Hides or grey all widgets that needs an open file when boolean show is
 * FALSE
 * @param main : main GtkBuilder XML structure
 * @param greyed : boolean (TRUE to hide an grey widgets)
 */
void grey_main_widgets(GtkBuilder *main, gboolean greyed)
{
    GtkWidget *notebook = NULL;  /* file notebook in main window */

    if (main != NULL)
    {
        notebook = heraia_get_widget(main, "file_notebook");
        gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);

        if (greyed == TRUE)
        {
            gtk_widget_set_sensitive(heraia_get_widget(main, "save"), FALSE);
            gtk_widget_set_sensitive(heraia_get_widget(main, "save_as"), FALSE);
            gtk_widget_hide(notebook);
        }
        else
        {
            gtk_widget_set_sensitive(heraia_get_widget(main, "save"), TRUE);
            gtk_widget_set_sensitive(heraia_get_widget(main, "save_as"), TRUE);
            gtk_widget_show_all(notebook);
        }
    }
}


/**
 * @fn init_heraia_interface(heraia_struct_t *main_struct)
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
 * @fn gboolean load_heraia_xml(heraia_struct_t *main_struct)
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
            return TRUE;
        }
    }
    else
    {
        return FALSE;
    }
}

/**
 * @fn connect_cursor_moved_signal(heraia_struct_t *main_struct)
 *  Connects the signal that the cursor has moved to
 *  the refreshing function
 * @param main_struct : main structure
 */
void connect_cursor_moved_signal(heraia_struct_t *main_struct, GtkWidget *hex_widget)
{
    g_signal_connect(G_OBJECT(hex_widget), "cursor_moved",
                     G_CALLBACK(refresh_event_handler), main_struct);
}


/**
 * @fn void heraia_ui_connect_signals(heraia_struct_t *main_struct)
 *  Connect the signals at the interface
 * @param main_struct : main structure
 */
static void heraia_ui_connect_signals(heraia_struct_t *main_struct)
{

    /* the data interpretor menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "DIMenu")), "activate",
                     G_CALLBACK(on_DIMenu_activate), main_struct);

    /* Quit, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "quit")), "activate",
                     G_CALLBACK(on_quit_activate), main_struct);

    /* New, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "new")), "activate",
                     G_CALLBACK(on_new_activate), main_struct);

    /* Open, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "open")), "activate",
                     G_CALLBACK(on_open_activate), main_struct);

    /* Save, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "save")), "activate",
                     G_CALLBACK(on_save_activate), main_struct);

    /* Save As, file menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "save_as")), "activate",
                     G_CALLBACK(on_save_as_activate), main_struct);

    /* Preferences, file menu ; See main_pref_window.c for main_pref_window's signals */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "preferences")), "activate",
                     G_CALLBACK(on_preferences_activate), main_struct);

    /* Cut, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "cut")), "activate",
                     G_CALLBACK(on_cut_activate), main_struct);

    /* Copy, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "copy")), "activate",
                     G_CALLBACK(on_copy_activate), main_struct);

    /* Paste, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "paste")), "activate",
                     G_CALLBACK(on_paste_activate), main_struct);

    /* Delete, edit menu */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "delete")), "activate",
                     G_CALLBACK(on_delete_activate), main_struct);

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
                     G_CALLBACK(delete_main_struct_event), main_struct);
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
            fprintf(stdout, "Connecting heraia_ui signals     ");
        }

        heraia_ui_connect_signals(main_struct);

        if (main_struct->debug == TRUE)
        {
            fprintf(stdout, " [Done]\n");
        }

        /* The Log window */
        if (main_struct->debug == TRUE)
        {
            fprintf(stdout, "log window init interface        ");
        }

        log_window_init_interface(main_struct);

        if (main_struct->debug == TRUE)
        {
            fprintf(stdout, " [Done]\n");
        }

        /* Preferences window */
        if (main_struct->debug == TRUE)
        {
            fprintf(stdout, "preferences window init interface");
        }

        main_pref_window_init_interface(main_struct);

        if (main_struct->debug == TRUE)
        {
            fprintf(stdout, " [Done]\n");
        }


        /* The data interpretor window */
        if (main_struct->debug == TRUE)
        {
            fprintf(stdout, "data interpretor init interface  ");
        }

        data_interpretor_init_interface(main_struct);

        if (main_struct->debug == TRUE)
        {
            fprintf(stdout, " [Done]\n");
        }


        /* preferences file */

        fprintf(stdout, "Loading heraia preference file   ");

        if (load_preference_file(main_struct) != TRUE)
        {
            fprintf(stdout, " [FAILED]\n");
        }
        else /* Setting up preferences */
        {
            fprintf(stdout, " [Done]\n");
            fprintf(stdout, "Setting up preferences           ");
            load_preferences(main_struct);
            fprintf(stdout, " [Done]\n");
        }
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

    while (tmp_slist)
    {
        if (GTK_TOGGLE_BUTTON (tmp_slist->data)->active)
        {
            return GTK_WIDGET (tmp_slist->data);
        }
        tmp_slist = tmp_slist->next;
    }

    return NULL;
}


/**
 * @fn GtkWidget *gtk_radio_button_get_active_from_widget(GtkRadioButton *radio_group_member)
 * gets the active radio button from a radio group
 * @param radio_group_member : widget to get radio group from
 * @returns the active GtkRadioButton within the group from
 *          radio_group_member
 **/
GtkWidget *gtk_radio_button_get_active_from_widget(GtkRadioButton *radio_group_member)
{
    if (radio_group_member)
    {
        return gtk_radio_button_get_active(radio_group_member->group);
    }
    else
    {
        return NULL;
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
 * @fn GtkWidget *heraia_get_widget(GtkBuilder *xml, gchar *widget_name)
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
 * @fn void close_heraia(heraia_struct_t *main_struct)
 * Before closing heraia we need to do few things
 * @param main_struct : main_struct
 */
static void close_heraia(heraia_struct_t *main_struct)
{
    /* recording window's position */
    record_all_dialog_box_positions(main_struct);

    /* . Saving preferences */
    save_preferences(main_struct);
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
            /* Emit the specific signal to activate the check_menu_item */
            if (main_struct->win_prop->data_interpretor->displayed == TRUE)
            {
                main_struct->win_prop->data_interpretor->displayed = FALSE; /* dirty trick */
                g_signal_emit_by_name(heraia_get_widget(main_struct->xmls->main, "DIMenu"), "activate");
            }

            /* List Data type Interface */
            cmi = heraia_get_widget(main_struct->xmls->main, "ldt_menu");
            dialog_box = heraia_get_widget(main_struct->xmls->main, "list_data_types_window");
            init_one_cmi_window_state(dialog_box, cmi, main_struct->win_prop->ldt);

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
 *  Adds a new tab to the main window in file's notebook
 * @param main_struct : main structure
 * @param doc : the new document that will be related to the tab
 */
void add_new_tab_in_main_struct(heraia_struct_t *main_struct, doc_t *doc)
{
    GtkWidget *vbox = NULL;       /**< used for vbox creation               */
    GtkNotebook *notebook = NULL; /**< file_notebook from heraia.gtkbuilder */
    GtkWidget *tab_label = NULL;  /**< tab's label                          */
    gint tab_num = -1;            /**< new tab's index                      */

    notebook = GTK_NOTEBOOK(heraia_get_widget(main_struct->xmls->main, "file_notebook"));
    vbox = gtk_vbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), doc->hex_widget, TRUE, TRUE, 3);

    tab_label = gtk_label_new(NULL);

    gtk_widget_show_all(vbox);
    tab_num = gtk_notebook_append_page(notebook, vbox, tab_label);

    gtk_notebook_set_current_page(notebook, tab_num);
    main_struct->current_doc = doc;

    /* This may not be necessary here has it is done later
        set_notebook_tab_name(main_struct);
    */
}


/**
 *  To help plugins to deal with widgets, shows or hide a specific widget
 * @param widget : the widget to show or hide
 * @param show : what to do : TRUE to show the widget, FALSE to hide it
 * @param win_prop : window properties.
 */
void show_hide_widget(GtkWidget *widget, gboolean show, window_prop_t *win_prop)
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

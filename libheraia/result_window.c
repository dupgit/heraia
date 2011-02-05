/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  result_window.c
  result_window.c - A window that will collect all results of functions such as
                    find, find a data from type, ...

  (C) Copyright 2010 - 2011 Olivier Delhomme
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

static void tree_selection_changed(GtkTreeSelection *selection, gpointer data);
static void rw_on_close_activate(GtkWidget *widget, gpointer data);

static void determine_pos_and_buffer_size(guint64 *pos, guint *buffer_size, guint size, guint64 file_size);
static void add_gtk_tree_view_to_result_notebook(heraia_struct_t *main_struct, GtkListStore *lstore, guchar *label_text, doc_t *doc);

static void menu_result_toggle(GtkWidget *widget, gpointer data);


/**
 * Show result window
 * @param widget : the widget that issued the signal (may be NULL as we do not
 *                 use it).
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void result_window_show(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *window = NULL;      /**< result window itself */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            window = heraia_get_widget(main_struct->xmls->main, "result_window");
            move_and_show_dialog_box(window, main_struct->win_prop->result_window);
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(heraia_get_widget(main_struct->xmls->main, "menu_result")), TRUE);
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
             record_and_hide_dialog_box(window, main_struct->win_prop->result_window);
             gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(heraia_get_widget(main_struct->xmls->main, "menu_result")), FALSE);
        }
}


/**
 * Function called upon selection change. Changes the cursor position to the
 * right place (should also change to the right document).
 * @param selection : the selection that changed
 * @param data : user data MUST be main program's structure
 */
static void tree_selection_changed(GtkTreeSelection *selection, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkTreeIter iter;
    GtkTreeModel *model = NULL;
    GtkWidget *result_notebook = NULL;
    guint64 a_pos = 0;
    gint index = 0;
    doc_t *the_doc = NULL;

    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {
            result_notebook = heraia_get_widget(main_struct->xmls->main, "result_notebook");

            if (gtk_tree_selection_get_selected(selection, &model, &iter))
                {
                    gtk_tree_model_get(model, &iter, R_LS_POS, &a_pos, -1);
                    index = gtk_notebook_get_current_page(GTK_NOTEBOOK(result_notebook));
                    the_doc = g_ptr_array_index(main_struct->results, index);
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Search result index : %d "), index);

                    if (the_doc != NULL)
                        {
                            /* Change file's notebook tab in main window to display the_doc document if it is possible */
                            ghex_set_cursor_position(the_doc->hex_widget, a_pos - 1);
                        }
                }
        }
}


/**
 * Calculates the position and the associated buffer size in order that the text
 * searched for will be centered in the results
 * @param pos : Must be the position of the searched text, is returned as the
 *              position where we will extract the text
 * @param buffer_size : is returned as the total buffer size (the extracted text
 *                      will be buffer_size + 1 in size)
 * @param size : size of the string searched for (in bytes)
 * @param file_size : size of the file (to verify that we are still in the
 *                    limits
 */
static void determine_pos_and_buffer_size(guint64 *pos, guint *buffer_size, guint size, guint64 file_size)
{
    guint64 a_pos = 0;
    guint a_buf_size = 0;
    gint gap = 0;

    a_pos = *pos;
    a_buf_size = *buffer_size;

    if ((a_pos + a_buf_size) > file_size)
        {
            gap = file_size - a_pos - size;
            if (gap < 0)
                {
                    gap = 0;
                }
            else if (gap > 4)
                    {
                        gap = 4;
                    }
            a_pos = a_pos - gap;
            a_buf_size = (2 * gap) + size;
        }
    else
        {
            if (a_pos >= 4)
                {
                    a_pos = a_pos - 4;
                    a_buf_size = 8 + size; /* we want to have 4 bytes before and 4 bytes after the result to be displayed */
                }
            else
                {
                    a_buf_size = (2 * a_pos) + size;
                    a_pos = 0;
                }
        }

    *pos = a_pos;
    *buffer_size = a_buf_size;
}


/**
 * Closes a result tab
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
static void rw_on_close_activate(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *notebook = NULL;  /**< result_notebook from heraia.gtkbuilder     */
    gint tab_number = 0;         /**< index of the tab (and thus from the array) */

    tab_number = find_tab_number_from_widget(main_struct, "result_notebook", widget);

    /* Removing the index in the array */
    g_ptr_array_remove_index(main_struct->results, tab_number);

    /* And removing it in the notebook */
    notebook = heraia_get_widget(main_struct->xmls->main, "result_notebook");
    gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), tab_number);
}


/**
 * Add one tab to the result window's notebook with a gtk_tree_view in it
 * @param main_struct : main structure of heraia
 * @param lstore : the populated list store (for the tree view)
 * @param label_text : the text for the label of the tab
 * @param doc : the document from which the search was done
 */
static void add_gtk_tree_view_to_result_notebook(heraia_struct_t *main_struct, GtkListStore *lstore, guchar *label_text, doc_t *doc)
{
    GtkWidget *vbox = NULL;           /**< used for vbox creation                      */
    GtkWidget *scrolledw = NULL;      /**< the scrolled window                         */
    GtkWidget *notebook = NULL;       /**< result_notebook from heraia.gtkbuilder      */
    GtkWidget *tview = NULL;          /**< the tree view                               */
    GtkWidget *tab_label = NULL;      /**< tab's label                                 */
    GtkWidget *hbox = NULL;           /**< the hbox that will receive the close button */
    GtkCellRenderer *renderer = NULL; /**< a rennderer for the cells                   */
    GtkTreeViewColumn *column = NULL; /**< columns to be added to the treeview         */
    GtkTreeSelection *select = NULL;  /**< selection to the treeview                   */
    gint tab_num = -1;                /**< new tab's index                             */
    gchar *markup = NULL;             /**< markup text                                 */
    gchar *menu_markup = NULL;        /**< menu markup text (menu in the notebook)     */
    GtkWidget *menu_label = NULL;     /**<menu's label (notebook's menu)               */
    gchar *whole_filename = NULL;     /**< filename of the document from where the
                                           search took place                           */

    notebook = heraia_get_widget(main_struct->xmls->main, "result_notebook");
    vbox = gtk_vbox_new(FALSE, 2);
    scrolledw = gtk_scrolled_window_new(NULL, NULL);
    tview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(lstore));
    gtk_box_pack_start(GTK_BOX(vbox), scrolledw, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(scrolledw), tview);

    g_object_unref(lstore);

    /* Column Number (1....21321) the number of columns */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(Q_("Number"), renderer, "text", R_LS_N, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tview), column);

    /* Column Position (23, 545, 879, 1324, ...) the positions of the text found */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(Q_("Position"), renderer, "text", R_LS_POS, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tview), column);

    /* Column Hex : Hexadecimal text as found in the file */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(Q_("Hex"), renderer, "text", R_LS_HEX, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tview), column);

    /* Column Ascii : Normal text found in the file (if printable) */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(Q_("Ascii"), renderer, "text", R_LS_ASCII, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tview), column);

    /* Signal handling */
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(tree_selection_changed), main_struct);

    tree_selection_changed(gtk_tree_view_get_selection(GTK_TREE_VIEW(tview)), main_struct);

    /* tab's label, menu label and tooltips */
    if (label_text != NULL)
        {
            tab_label = gtk_label_new(NULL);
            markup = g_markup_printf_escaped("%s", label_text);
            gtk_label_set_markup(GTK_LABEL(tab_label), markup);

            menu_label = gtk_label_new(NULL);
            menu_markup = g_markup_printf_escaped("%s", label_text);
            gtk_label_set_markup(GTK_LABEL(menu_label), menu_markup);
            gtk_label_set_justify(GTK_LABEL(menu_label), GTK_JUSTIFY_LEFT);

            whole_filename = doc_t_document_get_filename(doc);
            gtk_widget_set_tooltip_text(tab_label, g_filename_display_name(whole_filename));

            g_free(markup);
            g_free(menu_markup);
        }

    /* Creating the close button */
    hbox = create_tab_close_button(main_struct, tab_label, rw_on_close_activate);

    gtk_widget_show_all(vbox);
    tab_num = gtk_notebook_append_page_menu(GTK_NOTEBOOK(notebook), vbox, hbox, menu_label);

    gtk_widget_show_all(notebook);
}


/**
 * Add one tab for the results from the find all button
 * @param main_struct : main structure of heraia
 * @param all_pos : A GArray of all found positions
 * @param size : size of the string searched for (in bytes)
 * @param text : the text for the label of the newly created tab. If NULL, a text
 *               is guessed from the last found position and the text found
 */
void rw_add_one_tab_from_find_all_bt(heraia_struct_t *main_struct, GArray *all_pos, guint size, guchar *text)
{
    GtkListStore *lstore =  NULL;         /**< List store that will contain results                    */
    guint i = 0;
    guint64 pos = 0;                      /**< a calculated position to center the search string       */
    guint64 real_pos = 0;                 /**< real position                                           */
    guint64 file_size = 0 ;               /**< size of the file  during the search (should not change) */
    doc_t *current_doc = NULL;            /**< Current document on which we want to do the search      */
    gint endianness = H_DI_LITTLE_ENDIAN; /**< endianness as selected in the data interpretor window   */
    guint buffer_size = 0;                /**< buffer size (bigger than size in order to display      */
                                          /**  some byte before and after the results)                 */
    guchar *ascii_buffer = NULL;          /**< the ascii buffer                                        */
    guchar *hex_buffer = NULL;            /**< the hex buffer                                          */
    GtkTreeIter iter;
    guchar *label_text = NULL;            /**< text label                                              */


    current_doc = main_struct->current_doc;
    g_ptr_array_add(main_struct->results, current_doc);

    endianness = H_DI_LITTLE_ENDIAN; /** Endianness by default (we want the search to be flat) -> However this may
                                    be modified if someone asks for */
    file_size = ghex_file_size(GTK_HEX(current_doc->hex_widget));

    /* The columns :  R_LS_N (G_TYPE_UINT), R_LS_POS (G_TYPE_UINT64), R_LS_HEX (G_TYPE_STRING), R_LS_ASCII (G_TYPE_STRING) */
    lstore = gtk_list_store_new(R_LS_N_COLUMNS, G_TYPE_UINT, G_TYPE_UINT64, G_TYPE_STRING, G_TYPE_STRING);

    for (i = 0; i < all_pos->len; i++)
        {
            pos = g_array_index(all_pos, guint64, i);
            real_pos = pos;

            determine_pos_and_buffer_size(&pos, &buffer_size, size, file_size);

            ascii_buffer = ghex_get_data_to_ascii(current_doc->hex_widget, pos, buffer_size, endianness);
            hex_buffer = ghex_get_data_to_hex(current_doc->hex_widget, pos, buffer_size, endianness);

            if (ascii_buffer != NULL && hex_buffer != NULL)
                {
                    gtk_list_store_append(lstore, &iter);
                    gtk_list_store_set(lstore, &iter, R_LS_N, i + 1, R_LS_POS, real_pos + 1, R_LS_HEX, hex_buffer, R_LS_ASCII, ascii_buffer, -1);

                    log_message(main_struct, G_LOG_LEVEL_DEBUG, "%lld : %d, %s - %s", pos, buffer_size, ascii_buffer, hex_buffer);

                    g_free(ascii_buffer);
                    g_free(hex_buffer);
                }
        }

    if (text == NULL)
        {
            /* Using last pos to retrieve the text for the label */
            label_text = ghex_get_data_to_ascii(current_doc->hex_widget, real_pos, size, endianness);
        }
    else
        {
            label_text = (guchar *) g_strdup((gchar *) text);
        }

    add_gtk_tree_view_to_result_notebook(main_struct, lstore, label_text, current_doc);

    result_window_show(NULL, main_struct);
}


/**
 *  The Check menu item for the result window
 * @param widget : the widget that issued the signal (here the menu_result check
 *                 menu item
 * @param data : user data, MUST be main_struct main structure
 */
static void menu_result_toggle(GtkWidget *widget, gpointer data)
{
    GtkCheckMenuItem *cmi = GTK_CHECK_MENU_ITEM(widget);
    gboolean checked = gtk_check_menu_item_get_active(cmi);

    if (checked == TRUE)
        {
            result_window_show(widget, data);
        }
    else
        {
             result_window_close(widget, data);
        }
}


/**
 * Function called when the user switches the pages from the notebook. Here we
 * unselect the selection if any. This is to let the user be able to select
 * again the first row if there is only one row in the treeview.
 * @param notebook : result window's notebook object which received the signal.
 * @param page : the new current page
 * @param page_num : the index of the page (the new one)
 * @param data :user data, MUST be main_struct main structure
 */
static void rw_on_notebook_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GList *widget_list =  NULL;         /** The list of the widgets in the page     */
    GtkWidget *widget = NULL;
    gboolean ok = FALSE;
    GtkTreeSelection *selection = NULL; /** The selection (if any) to be unselected */

    if (page != NULL)
        {
            /* Page indicates the selected notebook's page */
            log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Page : %p, %d"), page, page_num);

            widget_list = gtk_container_get_children(GTK_CONTAINER(gtk_notebook_get_nth_page(notebook, page_num)));

            while (widget_list != NULL && ok != TRUE)
                {
                    widget = widget_list->data;

                    /* Widget is the GtkWidget being processed */
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, "Widget : %p, %s",widget, gtk_widget_get_name(widget));

                    if GTK_IS_SCROLLED_WINDOW(widget) /* If this is the scrolled window, get the tree view that is in it */
                        {
                            widget = gtk_bin_get_child(GTK_BIN(widget));
                            ok = TRUE;

                            selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
                            if (selection != NULL)
                                {
                                    /* Unselect the selection -> this allows one to reselect the same
                                     * raw (in case that there is only one raw in the results
                                     */
                                    gtk_tree_selection_unselect_all(selection);
                                }
                        }

                    widget_list = g_list_next(widget_list);
                }
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

    /* The toogle button in the main menu*/
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "menu_result")), "toggled",
                     G_CALLBACK(menu_result_toggle), main_struct);

  /* Switch pages on the notebook */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "result_notebook")), "switch-page",
                     G_CALLBACK(rw_on_notebook_switch_page), main_struct);
}


/**
 * Remove all tabs from the result window that correspond to the specified
 * document.
 * @param main_struct : heraia's main structure
 * @param doc : the document beeing close in main window's notebook
 */
void rw_remove_all_tabs(heraia_struct_t *main_struct, doc_t *doc)
{
    gint i = 0;
    gint len = 0;               /**< total len of result array                   */
    GtkWidget *notebook = NULL; /**< result_notebook from heraia.gtkbuilder      */
    doc_t *value = NULL;        /**< Value to compare with doc                   */
    GArray *array = NULL;       /**< array that stores the indexes where results
                                     value is equal to doc.                      */

    if (main_struct != NULL && main_struct->results != NULL)
        {
            notebook = heraia_get_widget(main_struct->xmls->main, "result_notebook");

            array = g_array_new(TRUE, TRUE, sizeof(gint));

            len = main_struct->results->len;

            /* looking for the results */
            i = 0;
            while (i < len)
                {
                    value = g_ptr_array_index(main_struct->results, i);

                    if (value == doc)
                        {
                            log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Removing index %d."), i);
                            g_ptr_array_remove_index(main_struct->results, i);
                            gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), i);
                            len = main_struct->results->len;
                        }
                    else
                        {
                            i = i + 1;
                        }
                }
        }
}

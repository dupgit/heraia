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

static void determine_pos_and_buffer_size(guint64 *pos, guint *buffer_size, guint size, guint64 file_size);
static void add_gtk_tree_view_to_result_notebook(heraia_struct_t *main_struct, GtkListStore *lstore, guchar *label_text);

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
 * Add one tab to the result window's notebook with a gtk_tree_view in it
 * @param main_struct : main structure of heraia
 * @param lstore : the populated list store (for the tree view)
 * @param label_text : the text for the label of the tab
 */
static void add_gtk_tree_view_to_result_notebook(heraia_struct_t *main_struct, GtkListStore *lstore, guchar *label_text)
{
    GtkWidget *vbox = NULL;           /**< used for vbox creation                 */
    GtkWidget *scrolledw = NULL;      /**< the scrolled window                    */
    GtkWidget *notebook = NULL;       /**< result_notebook from heraia.gtkbuilder */
    GtkWidget *tview = NULL;          /**< the tree view                          */
    GtkWidget *tab_label = NULL;      /**< tab's label                            */
    GtkCellRenderer *renderer = NULL; /**< a rennderer for the cells              */
    GtkTreeViewColumn *column = NULL; /**< columns to be added to the treeview    */
    gint tab_num = -1;                /**< new tab's index                        */
    gchar *markup = NULL;             /**< markup text                            */


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

    /* tab's label and menu label */
    if (label_text != NULL)
        {
            tab_label = gtk_label_new(NULL);
            markup = g_markup_printf_escaped("%s", label_text);
            gtk_label_set_markup(GTK_LABEL(tab_label), markup);
            g_free(markup);
        }

    tab_num = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, tab_label);

    gtk_widget_show_all(notebook);
}


/**
 * Add one tab for the results from the find all button
 * @param main_struct : main structure of heraia
 * @param all_pos : A GArray of all found positions
 * @param size : size of the string searched for (in bytes)
 */
void rw_add_one_tab_from_find_all_bt(heraia_struct_t *main_struct, GArray *all_pos, guint size)
{
    GtkListStore *lstore =  NULL;     /**< List store that will contain results                    */
    guint i = 0;
    guint64 pos = 0;                  /**< a calculated position to center the search string       */
    guint64 real_pos = 0;             /**< real position                                           */
    guint64 file_size = 0 ;           /**< size of the file  during the search (should not change) */
    doc_t *current_doc = NULL;        /**< Current document on which we want to do the search      */
    gint endianness = LITTLE_ENDIAN;  /**< endianness as selected in the data interpretor window   */
    guint buffer_size = 0;             /**< buffer size (bigger than size in order to display      */
                                      /**  some byte before and after the results)                 */
    guchar *ascii_buffer = NULL;      /**< the ascii buffer                                        */
    guchar *hex_buffer = NULL;        /**< the hex buffer                                          */
    GtkTreeIter iter;
    guchar *label_text = NULL;        /**< text label                                              */


    current_doc = main_struct->current_doc;
    endianness = LITTLE_ENDIAN; /** Endianness by default (we want the search to be flat) -> However this may
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

    /* Using last pos to retrieve the text for the label */
    label_text = ghex_get_data_to_ascii(current_doc->hex_widget, real_pos, size, endianness);

    add_gtk_tree_view_to_result_notebook(main_struct, lstore, label_text);

    result_window_show(NULL, main_struct);
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

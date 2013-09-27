/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  find_replace_window.c
  find_replace_window.c - Windows used to find text and find and replace text in
                          the opened hex documents.


  (C) Copyright 2010 - 2013 Olivier Delhomme
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
 * @file find_replace_window.c
 * Windows used to find text and find and replace text in the opened hex
 * documents.
 */
#include <libheraia.h>

/*** common stuff ***/
static guchar *fr_get_search_string(heraia_struct_t * main_struct, doc_t *doc, guint *buffer_size);
static doc_t *create_find_or_replace_doc_t(void);
static void find_replace_add_ghex_widget(xml_t *xmls, gchar *widget_name, doc_t *entry);
static void fr_search_forward(heraia_struct_t *main_struct, doc_t *search_doc, goffset offset);

/*** find window ***/
static gboolean delete_find_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_find_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void find_window_close(GtkWidget *widget, gpointer data);
static void find_window_connect_signal(heraia_struct_t *main_struct);
static void find_all_bt_clicked(GtkWidget *widget, gpointer data);
static void find_next_bt_clicked(GtkWidget *widget, gpointer data);
static void find_prev_bt_clicked(GtkWidget *widget, gpointer data);

/*** find and replace window ***/
static gboolean delete_fr_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_fr_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void fr_window_close(GtkWidget *widget, gpointer data);
static void fr_window_connect_signal(heraia_struct_t *main_struct);
static void fr_search_bt_clicked(GtkWidget *widget, gpointer data);
static void fr_replace_bt_clicked(GtkWidget *widget, gpointer data);
static void fr_replace_search_bt_clicked(GtkWidget *widget, gpointer data);
static goffset fr_replace_data(heraia_struct_t *main_struct);


/*** fdft window ***/
static gboolean delete_fdft_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_fdft_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static fdft_t *fdft_window_init_widgets(heraia_struct_t * main_struct);
static void fdft_window_close(GtkWidget *widget, gpointer data);
static void fdft_window_connect_signal(heraia_struct_t *main_struct);

/**
 * Show find window
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void find_window_show(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *window = NULL;      /**< find window */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            window = heraia_get_widget(main_struct->xmls->main, "find_window");
            show_hide_widget(window, TRUE, main_struct->win_prop->find_window);
        }
}


/**
 * Call back function for the find window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data :  MUST be heraia_struct_t *main_struct main structure
 */
static gboolean delete_find_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    find_window_close(widget, data);

    return TRUE;
}


/**
 * Call back function for the find window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : user data - MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void destroy_find_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    find_window_close(widget, data);
}


/**
 * Close button has been clicked we want to hide the window
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void find_window_close(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *window = NULL;     /**< find window */

    if (main_struct != NULL)
        {
            window = heraia_get_widget(main_struct->xmls->main, "find_window");
            show_hide_widget(window, FALSE, main_struct->win_prop->find_window);
        }

}


/**
 * Tries to find, in the document, what the user entered in the GtkHex entry in
 * the find window (forward from the current position)
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void find_next_bt_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->find_doc != NULL)
        {
            fr_search_forward(main_struct, main_struct->find_doc, 0);
        }
}


/**
 * Tries to find, in the document, what the user entered in the GtkHex entry in
 * the find window (backward from the current position)
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void find_prev_bt_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    guchar *buffer = NULL;     /**< Buffer that contains the search string             */
    doc_t *current_doc = NULL; /**< Current doc where we want to search for the string */
    gboolean result = FALSE;
    guint64 position = 0;
    guint buffer_size = 0;

    if (main_struct != NULL)
        {
            buffer = fr_get_search_string(main_struct, main_struct->find_doc, &buffer_size);

            if (buffer != NULL)
                {
                    current_doc = main_struct->current_doc;
                    position = ghex_get_cursor_position(current_doc->hex_widget) + 1 ;
                    result = ghex_find_backward(current_doc, buffer, buffer_size, &position);

                    if (result == TRUE)
                        {
                            ghex_set_cursor_position(current_doc->hex_widget, position);
                        }
                }
        }
}


/**
 * Tries to find, in the document, what the user entered in the GtkHex entry in
 * the find window (all positions from 0)
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void find_all_bt_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    guchar *buffer = NULL;     /**< Buffer that contains the search string             */
    doc_t *current_doc = NULL; /**< Current doc where we want to search for the string */
    gboolean result = FALSE;
    guint64 position = 0;
    guint buffer_size = 0;
    GArray *all_pos = NULL;    /**< All positions of the searched string               */

    if (main_struct != NULL)
        {
            buffer = fr_get_search_string(main_struct, main_struct->find_doc, &buffer_size);

            if (buffer != NULL)
                {
                    all_pos = g_array_new(TRUE, TRUE, sizeof(guint64));
                    current_doc = main_struct->current_doc;
                    position = 0;
                    result = ghex_find_forward(current_doc, buffer, buffer_size, &position);

                    while (result == TRUE)
                        {
                            all_pos = g_array_append_val(all_pos, position);
                            result = ghex_find_forward(current_doc, buffer, buffer_size, &position);
                        }
                }
        }

    if (all_pos != NULL)
        {
            rw_add_one_tab_from_find_all_bt(main_struct, all_pos, buffer_size, NULL);
            g_array_free(all_pos, TRUE);
        }
}


/**
 * Signal connections for the find window
 * @param main_struct : heraia's main structure
 */
static void find_window_connect_signal(heraia_struct_t *main_struct)
{
    /* Close button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "find_close_bt")), "clicked",
                     G_CALLBACK(find_window_close), main_struct);

    /* Next button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "find_next_bt")), "clicked",
                     G_CALLBACK(find_next_bt_clicked), main_struct);

    /* Prev button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "find_prev_bt")), "clicked",
                     G_CALLBACK(find_prev_bt_clicked), main_struct);

    /* Find all button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "find_all_bt")), "clicked",
                     G_CALLBACK(find_all_bt_clicked), main_struct);

    /* When find window is killed or destroyed */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "find_window")), "delete_event",
                     G_CALLBACK(delete_find_window_event), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "find_window")), "destroy",
                     G_CALLBACK(destroy_find_window_event), main_struct);
}


/**
 * Inits all the things in the find window (signal and such)
 * @param main_struct : heraia's main structure
 */
void find_window_init_interface(heraia_struct_t * main_struct)
{

    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {
            find_window_connect_signal(main_struct);

            main_struct->find_doc = create_find_or_replace_doc_t();

            if (main_struct->find_doc != NULL)
                {
                   find_replace_add_ghex_widget(main_struct->xmls, "find_al", main_struct->find_doc);
                }
        }
}


/******************************************************************************/
/******************************* common stuff *********************************/
/******************************************************************************/

/**
 * Gets the string from the document doc
 * @param main_struct : main structure, needed here to compute endianness
 * @param doc : the document (HexDocument and HexWidget) used to defined the
 *              search string
 * @return a newly allocated guchar string that may be g_free'ed when no longer
 *         needed
 */
static guchar *fr_get_search_string(heraia_struct_t * main_struct, doc_t *doc, guint *buffer_size)
{
    guint size = 0;          /**< size of the search string (we hope that this value is small) */
    guchar *buffer = NULL;   /**< buffer for the search string                                 */
    guint endianness = 0;    /**< endianness as selected in data interpretor's window          */
    gboolean result = FALSE;


    size = ghex_file_size(GTK_HEX(doc->hex_widget));

    if (size > 0 && size < 4096)  /* Here fixes some limits ! */
        {
            buffer = (guchar *) g_malloc0(sizeof(guchar) * size);
            endianness = which_endianness(main_struct);
            result = ghex_get_data_position(doc->hex_widget, 0, size, endianness, buffer);

            if (result == TRUE)
                {
                    *buffer_size = size;
                    return buffer;
                }
            else
                {
                    *buffer_size = 0;
                    return NULL;
                }
        }
    else
        {
            *buffer_size = 0;
            return NULL;
        }
}


/**
 * Creates the HexDocument and the GtkHex widget with the right properties and
 * Fills a doc_t structure with them.
 * @return a newly allocated doc_t structure with HexDocument and GtkHex widget
 *         correctly configured to fit in the find and find and replace windows
 */
static doc_t *create_find_or_replace_doc_t(void)
{
    Heraia_Document *find_hex_doc = NULL;
    GtkWidget *find_hex_widget = NULL;

    find_hex_doc = hex_document_new();

    if (find_hex_doc != NULL)
        {
            /* creating a new view to this new document */
            find_hex_widget = hex_document_add_view(find_hex_doc);

            /* Sets some options : no offests, insert mode and 4 lines of 16 chars */
            gtk_hex_show_offsets(GTK_HEX(find_hex_widget), FALSE);
            gtk_hex_set_insert_mode(GTK_HEX(find_hex_widget), TRUE);
            gtk_hex_set_geometry(GTK_HEX(find_hex_widget), 16, 4);

            /* joining those two new structures in one */
            return new_doc_t(find_hex_doc, find_hex_widget);
        }
    else
        {
            return NULL;
        }
}


/**
 * Adds the GtkHex widget to the right frame
 * @param xmls : xmls structure
 * @param widget_name : the widget that will receive the GtkHex widget (a frame)
 * @param entry : the doc_t structure that contains document and gtkhex widget
 *                used as an entry field
 */
static void find_replace_add_ghex_widget(xml_t *xmls, gchar *widget_name, doc_t *entry)
{
    GtkWidget *al = NULL;

    al = heraia_get_widget(xmls->main, widget_name);
    gtk_container_add(GTK_CONTAINER(al), entry->hex_widget);
    gtk_container_set_border_width(GTK_CONTAINER(al), 3);
}


/**
 * Searches the string entered in the search document in the current one (from
 * the currenty position + offset) in the main window.
 * @param main_struct : heraia's main structure
 * @param search_doc : the document used to enter the searched string
 * @param offset : the offset from the current position to begin the search.
 */
static void fr_search_forward(heraia_struct_t *main_struct, doc_t *search_doc, goffset offset)
{
    guchar *buffer = NULL;     /**< Buffer that contains the search string             */
    doc_t *current_doc = NULL; /**< Current doc where we want to search for the string */
    gboolean result = FALSE;
    guint64 position = 0;
    guint buffer_size = 0;

    buffer = fr_get_search_string(main_struct, search_doc, &buffer_size);

    if (buffer != NULL)
        {
            current_doc = main_struct->current_doc;
            position = ghex_get_cursor_position(current_doc->hex_widget);
            position = position + offset;
            result = ghex_find_forward(current_doc, buffer, buffer_size, &position);

            if (result == TRUE)
                {
                    ghex_set_cursor_position(current_doc->hex_widget, position);
                }
        }
}


/******************************************************************************/
/************************** find and replace window ***************************/
/******************************************************************************/


/**
 * Show find and replace window
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void fr_window_show(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *window = NULL;      /**< find window */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            window = heraia_get_widget(main_struct->xmls->main, "fr_window");
            show_hide_widget(window, TRUE, main_struct->win_prop->fr_window);
        }
}


/**
 * Call back function for the find and replace window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data :  MUST be heraia_struct_t *main_struct main structure
 */
static gboolean delete_fr_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    fr_window_close(widget, data);

    return TRUE;
}


/**
 * Call back function for the find and replace window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : user data - MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void destroy_fr_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    fr_window_close(widget, data);
}


/**
 * Close button has been clicked we want to hide the window
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void fr_window_close(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
     GtkWidget *window = NULL;     /**< find window */

    if (main_struct != NULL)
        {
             window = heraia_get_widget(main_struct->xmls->main, "fr_window");
             show_hide_widget(window, FALSE, main_struct->win_prop->fr_window);
        }
}


/**
 * Signal connections for the find and replace window
 * @param main_struct : heraia's main structure
 */
static void fr_window_connect_signal(heraia_struct_t *main_struct)
{
    /* Close button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fr_close_bt")), "clicked",
                     G_CALLBACK(fr_window_close), main_struct);

    /* When find and replace  window is killed or destroyed */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fr_window")), "delete_event",
                     G_CALLBACK(delete_fr_window_event), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fr_window")), "destroy",
                     G_CALLBACK(destroy_fr_window_event), main_struct);

    /* Search button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fr_find_bt")), "clicked",
                     G_CALLBACK(fr_search_bt_clicked), main_struct);

    /* Replace button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fr_replace_bt")), "clicked",
                     G_CALLBACK(fr_replace_bt_clicked), main_struct);

    /* Replace and search button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fr_replace_search_bt")), "clicked",
                     G_CALLBACK(fr_replace_search_bt_clicked), main_struct);
}


/**
 * Tries to find, in the document, what the user entered in the GtkHex entry in
 * the fr window in the find hexwidget (forward from the current position)
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void fr_search_bt_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;

    if (main_struct != NULL && main_struct->fr_find_doc != NULL)
        {
           fr_search_forward(main_struct, main_struct->fr_find_doc, 0);
        }
}


/**
 * Tries to replace, in the document, what the user entered in the GtkHex entry
 * in the fr window in the find hexwidget by what the user entered in the
 * replace entry in that same window and then goes to the next position (if any)
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void fr_replace_search_bt_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    goffset offset = 0;

    if (main_struct != NULL && main_struct->fr_find_doc != NULL)
        {
            offset = fr_replace_data(main_struct);
            /* fprintf(stdout, "offset : %ld\n", offset); */
            fr_search_forward(main_struct, main_struct->fr_find_doc, offset);
        }
}


/**
 * Tries to replace, in the document, what the user entered in the GtkHex entry
 * in the fr window in the find hexwidget by what the user entered in the
 * replace entry in that same window
 * @param main_struct : main structure
 * @return a goffset that indicates the length difference between the length of
 *         the replaced data and the length of the inserted data
 */
static goffset fr_replace_data(heraia_struct_t *main_struct)
{
    guchar *buffer = NULL;     /**< Buffer that contains the search string             */
    guchar *rep_buffer = NULL; /**< Buffer that contains the replace string            */
    doc_t *current_doc = NULL; /**< Current doc where we want to search for the string */
    guint buffer_size = 0;     /**< Size of the searched string                        */
    guint rep_buf_size = 0;    /**< Size of the replace string                         */
    guint64 position = 0;      /**< Current position in the current document !         */
    goffset length = 0;        /**< length of the result of that replace               */

    if (main_struct != NULL && main_struct->current_doc != NULL && main_struct->fr_find_doc != NULL && main_struct->fr_replace_doc != NULL)
        {
            current_doc = main_struct->current_doc;
            buffer = fr_get_search_string(main_struct, main_struct->fr_find_doc, &buffer_size);
            rep_buffer = fr_get_search_string(main_struct, main_struct->fr_replace_doc, &rep_buf_size);
            position = ghex_get_cursor_position(current_doc->hex_widget);
            if (ghex_compare_data(current_doc, buffer, buffer_size, position) == 0)
                {
                    /* The strings are equal and can be replaced in the current document */
                    ghex_set_data(current_doc, position, buffer_size, rep_buf_size, rep_buffer);
                    length = (goffset) rep_buf_size - buffer_size;
                    return length;
                }
            else
                {
                    return length;
                }
        }
    else
        {
            return length;
        }
}


/**
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void fr_replace_bt_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    /* goffset length = 0; */ /* Unused here but returned by fr_replace_data() function */

    fr_replace_data(main_struct);
}


/**
 * Inits all the things in the find and replace window (signal and such)
 * @param main_struct : heraia's main structure
 */
void fr_window_init_interface(heraia_struct_t * main_struct)
{

      if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {
            fr_window_connect_signal(main_struct);

            /* Creating a new hex document */
            main_struct->fr_find_doc = create_find_or_replace_doc_t();
            main_struct->fr_replace_doc = create_find_or_replace_doc_t();

            if (main_struct->fr_replace_doc != NULL && main_struct->fr_find_doc != NULL)
                {
                    find_replace_add_ghex_widget(main_struct->xmls, "fr_find_al", main_struct->fr_find_doc);
                    find_replace_add_ghex_widget(main_struct->xmls, "fr_replace_al", main_struct->fr_replace_doc);
                }

        }
}


/******************************************************************************/
/**************************** Find data from type *****************************/
/******************************************************************************/


/***
 * Populates the category combobox from the structures of the data interpretor
 * window
 * @param main_struct : main structure of the program
 */
static void fdft_window_populate_category_cb(heraia_struct_t *main_struct)
{
    GtkWidget *cb = NULL;  /**< category's combobox */
    GtkWidget *label = NULL;
    tab_t *tab = NULL;
    gint i = 0;
    const gchar *text = NULL;

    if (main_struct != NULL)
        {
            cb = main_struct->fdft->category_cb;

            for (i = 0; i < main_struct->current_DW->nb_tabs; i++)
                {
                    tab = g_ptr_array_index(main_struct->current_DW->tabs, i);
                    label = tab->label;
                    text = gtk_label_get_text(GTK_LABEL(label));

                    #if GTK_MAJOR_VERSION == 2
                        #if GTK_MINOR_VERSION >= 24
                            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb), text);
                        #endif
                        #if GTK_MINOR_VERSION <= 23
                            gtk_combo_box_append_text(GTK_COMBO_BOX(cb), text);
                        #endif
                    #endif
                    #if GTK_MAJOR_VERSION > 2
                        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb), text);
                    #endif
                }
        }
}


/**
 * Show find data from type window
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_struct_t *main_struct main structure
 */
void fdft_window_show(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *window = NULL;      /**< find data from type window */

    if (main_struct != NULL && main_struct->current_doc != NULL)
        {
            window = heraia_get_widget(main_struct->xmls->main, "fdft_window");
            show_hide_widget(window, TRUE, main_struct->win_prop->fdft_window);
        }
}


/**
 * Call back function for the find data from type window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data :  MUST be heraia_struct_t *main_struct main structure
 */
static gboolean delete_fdft_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    fdft_window_close(widget, data);

    return TRUE;
}


/**
 * Call back function for the find data from type window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : user data - MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void destroy_fdft_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    fdft_window_close(widget, data);
}


/**
 * Close button has been clicked we want to hide the window
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void fdft_window_close(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkWidget *window = NULL;     /**< find window */

    if (main_struct != NULL)
        {
            window = heraia_get_widget(main_struct->xmls->main, "fdft_window");
            show_hide_widget(window, FALSE, main_struct->win_prop->fdft_window);
        }
}

/**
 * return the decode structure that corresponds to the indexes from category,
 * type and feature as stated in the parameters
 * @param main_struct : heraia's main structure
 * @param cat_index : category index (tab's number in the data interpertor's window
 * @param typ_index : type index (the row number in the category's tab)
 * @param fea_index : feature index (column number in the row of the tab of the data
 *                    interpretor's window
 * @param[out] data_size : size of the data to be filled to the decoding function
 * @return the correspondinf decode structure that contains, the function, the
 *         entry (gtkwidget) and an error message to be displayed in case of an error
 */
static decode_t *get_decode_struct(heraia_struct_t *main_struct, gint cat_index, gint typ_index, gint fea_index, guint *data_size)
{
    decode_generic_t *decod = NULL; /**< stores row structure (boxes, labels, entries and functions)                 */
    tab_t *tab = NULL;              /**< stores description for one tab                                              */

    /* Everything is selected and we know what data type the user is looking for */
    if (cat_index >= 0 && typ_index >= 0 && fea_index >= 0)
        {
            tab = g_ptr_array_index(main_struct->current_DW->tabs, cat_index);
            if (tab != NULL)
                {
                    decod = g_ptr_array_index(tab->rows, typ_index);
                    if (decod  != NULL)
                        {
                            *data_size = decod->data_size;
                            return g_ptr_array_index(decod->decode_array, fea_index);
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
    else
        {
            return NULL;
        }
}

/**
 * Searches the string entered in the search document in the current one (from
 * the currenty position + offset) in the main window.
 * @param main_struct : heraia's main structure
 * @param dircetion : says wether we should go forward or backward
 * @param decode_struct : the decoding structure
 * @param data_size : the size of the data to be send to the decode function
 * @param buffer : Buffer that contains the string to look for
 */
static void fdft_search_direction(heraia_struct_t *main_struct, gint direction, decode_t *decode_struct, gint data_size, gchar *buffer)
{
    decode_parameters_t *decode_parameters = NULL;
    doc_t *current_doc = NULL; /**< Current doc where we want to search for the string */
    gboolean result = FALSE;
    guint64 position = 0;
    guint endianness = 0;
    guint stream_size = 0;
    GArray *all_pos = NULL;    /**< All positions of the searched string               */


    endianness = which_endianness(main_struct);    /** Endianness is computed only once here  */
    stream_size =  which_stream_size(main_struct); /** stream size is computed only once here */

    decode_parameters = new_decode_parameters_t(endianness, stream_size);


    if (buffer != NULL)
        {
            current_doc = main_struct->current_doc;

            if (direction == HERAIA_FIND_FORWARD || direction == HERAIA_FIND_BACKWARD)
                {
                    position = ghex_get_cursor_position(current_doc->hex_widget);
                    result = ghex_find_decode(direction, current_doc, decode_struct->func, decode_parameters, data_size, buffer, &position);

                    log_message(main_struct, G_LOG_LEVEL_DEBUG, "endianness : %d ; stream_size : %d - result : %d", endianness, stream_size, result);

                    if (result == TRUE)
                        {
                            ghex_set_cursor_position(current_doc->hex_widget, position);
                        }
                }
            else if (direction == HERAIA_FIND_ALL)
                {
                    all_pos = g_array_new(TRUE, TRUE, sizeof(guint64));

                    position = 0;
                    result = ghex_find_decode(HERAIA_FIND_ALL, current_doc, decode_struct->func, decode_parameters, data_size, buffer, &position);

                    log_message(main_struct, G_LOG_LEVEL_DEBUG, "endianness : %d ; stream_size : %d - result : %d", endianness, stream_size, result);

                    while (result == TRUE)
                        {
                            all_pos = g_array_append_val(all_pos, position);
                            result = ghex_find_decode(HERAIA_FIND_FORWARD, current_doc, decode_struct->func, decode_parameters, data_size, buffer, &position);
                        }

                    if (all_pos != NULL)
                        {
                            rw_add_one_tab_from_find_all_bt(main_struct, all_pos, data_size, (guchar *) buffer);
                            g_array_free(all_pos, TRUE);
                        }
                }
        }

    g_free(decode_parameters);
}


/**
 * Searches data from the selected type (if any) in the current document (if any)
 * and returns the results in the result window
 * @param widget : Calling widget (Is used to determine the direction of the search)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void fdft_prev_next_bt_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    gint cat_index = 0;   /**< index for the selected category in the combo box */
    gint typ_index = 0;   /**< index for the selected type in the combo box     */
    gint fea_index = 0;   /**< index for the selected feature in the combo box  */
    GtkWidget *cb = NULL; /**< represents the combo boxes (category, type and feature combo boxes) */
    decode_t *decode_struct = NULL;  /**< The structure that contains the function we need */
    const gchar *buffer = NULL; /**< contains what the user enterer in the search window */
    guint data_size = 0;
    GtkWidget *button = NULL;

    if (main_struct != NULL && main_struct->current_doc != NULL && main_struct->current_DW != NULL && main_struct->fdft != NULL)
        {
            cb = main_struct->fdft->category_cb;
            cat_index = gtk_combo_box_get_active(GTK_COMBO_BOX(cb));

            cb = main_struct->fdft->type_cb;
            typ_index = gtk_combo_box_get_active(GTK_COMBO_BOX(cb));

            cb = main_struct->fdft->feature_cb;
            fea_index = gtk_combo_box_get_active(GTK_COMBO_BOX(cb));

            decode_struct = get_decode_struct(main_struct, cat_index, typ_index, fea_index, &data_size);

            buffer = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(heraia_get_widget(main_struct->xmls->main, "fdft_value_entry"))));

            log_message(main_struct, G_LOG_LEVEL_DEBUG, "cat : %d, typ : %d, fea : %d - decode_struct : %p , data_size : %d ; buffer : %s", cat_index, typ_index, fea_index, decode_struct, data_size, buffer);

            if (decode_struct != NULL && buffer != NULL && data_size > 0)
                {
                    button = heraia_get_widget(main_struct->xmls->main, "fdft_next_bt");

                    if (widget == button)
                        {
                            fdft_search_direction(main_struct, HERAIA_FIND_FORWARD, decode_struct, data_size, (gchar *) buffer);
                        }
                    else
                        {
                            button = heraia_get_widget(main_struct->xmls->main, "fdft_prev_bt");
                            if (widget == button)
                                {
                                    fdft_search_direction(main_struct, HERAIA_FIND_BACKWARD, decode_struct, data_size, (gchar *) buffer);
                                }
                            else
                                {
                                    fdft_search_direction(main_struct, HERAIA_FIND_ALL, decode_struct, data_size, (gchar *) buffer);
                                }
                        }
                }
        }
}


/**
 * Signal connections for the find data from type window
 * @param main_struct : heraia's main structure
 */
static void fdft_window_connect_signal(heraia_struct_t *main_struct)
{
    /* Close button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fdft_close_bt")), "clicked",
                     G_CALLBACK(fdft_window_close), main_struct);

    /* When fdft window is killed or destroyed */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fdft_window")), "delete_event",
                     G_CALLBACK(delete_fdft_window_event), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fdft_window")), "destroy",
                     G_CALLBACK(destroy_fdft_window_event), main_struct);

    /* next button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fdft_next_bt")), "clicked",
                     G_CALLBACK(fdft_prev_next_bt_clicked), main_struct);

    /* prev button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fdft_prev_bt")), "clicked",
                     G_CALLBACK(fdft_prev_next_bt_clicked), main_struct);

    /* find all button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fdft_all_bt")), "clicked",
                     G_CALLBACK(fdft_prev_next_bt_clicked), main_struct);
}


/**
 * Fills the type ComboBox with the right values
 * @param widget : the combobox that issued the changed signal
 * @param data : must be heraia_struct_t *main_struct
 */
static void fdft_category_cb_changed(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    decode_generic_t *decod = NULL; /**< stores row structure (boxes, labels, entries and functions)                 */
    tab_t *tab = NULL;              /**< stores description for one tab                                              */
    GtkWidget *cb = NULL;           /**< represents the combo boxes (category, type and feature combo boxes)         */
    gint index = 0;                 /**< active index of the category combo box                                      */
    gint i = 0;
    GtkWidget *label = NULL;        /**< used to retrieve labels from the GtkLabel widgets                           */
    const gchar *text = NULL;       /**< contains text from the labels                                               */
    GtkTreeModel *model = NULL;     /**< the models from the combo box (used to delete everything in the combo boxes */

    if (main_struct != NULL && main_struct->fdft != NULL)
        {
            /* retrieve the selected category  */
            cb = main_struct->fdft->category_cb;
            index = gtk_combo_box_get_active(GTK_COMBO_BOX(cb));
            tab = g_ptr_array_index(main_struct->current_DW->tabs, index);

            /* Type combobox                   */
            /* First delete all entries if any */
            cb = main_struct->fdft->type_cb;
            model = gtk_combo_box_get_model(GTK_COMBO_BOX(cb));
            gtk_list_store_clear(GTK_LIST_STORE(model));

            /* Second fill the combobox with the values */
            for (i = 0; i < tab->nb_rows; i++)
                {
                    decod = g_ptr_array_index(tab->rows, i);
                    label = decod->label;
                    text = gtk_label_get_text(GTK_LABEL(label));

                    #if GTK_MAJOR_VERSION == 2
                        #if GTK_MINOR_VERSION >= 24
                            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb), text);
                        #endif
                        #if GTK_MINOR_VERSION <= 23
                            gtk_combo_box_append_text(GTK_COMBO_BOX(cb), text);
                        #endif
                    #endif
                    #if  GTK_MAJOR_VERSION > 2
                        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb), text);
                    #endif
                }

            /* Feature combobox                */
            /* First delete all entries if any */
            cb = main_struct->fdft->feature_cb;
            model = gtk_combo_box_get_model(GTK_COMBO_BOX(cb));
            gtk_list_store_clear(GTK_LIST_STORE(model));

            /* Second fill the combobox with the values */
            /* Here we start from 1, because column 0 is the row's title column */
            for (i = 1; i < tab->nb_cols; i++)
                {
                    label = g_ptr_array_index(tab->col_labels, i);
                    text = gtk_label_get_text(GTK_LABEL(label));

                    #if GTK_MAJOR_VERSION == 2
                        #if GTK_MINOR_VERSION >= 24
                            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb), text);
                        #endif
                        #if GTK_MINOR_VERSION <= 23
                            gtk_combo_box_append_text(GTK_COMBO_BOX(cb), text);
                        #endif
                    #endif
                    #if  GTK_MAJOR_VERSION > 2
                        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb), text);
                    #endif
                }
        }
}


/**
 * Inits the fdft structure and adds the widgets to the window
 * @param main_struct : heraia's main structure
 */
static fdft_t *fdft_window_init_widgets(heraia_struct_t * main_struct)
{
    fdft_t *fdft = NULL;
    GtkWidget *vbox = NULL;

    if (main_struct != NULL && main_struct->fdft == NULL)
        {
            fdft = (fdft_t *) g_malloc0 (sizeof(fdft_t));

            #if GTK_MAJOR_VERSION == 2
                #if GTK_MINOR_VERSION >= 24
                    fdft->category_cb = gtk_combo_box_text_new();
                    fdft->type_cb = gtk_combo_box_text_new();
                    fdft->feature_cb = gtk_combo_box_text_new();
                #endif
                #if GTK_MINOR_VERSION <= 23
                    fdft->category_cb = gtk_combo_box_new_text();
                    fdft->type_cb = gtk_combo_box_new_text();
                    fdft->feature_cb = gtk_combo_box_new_text();
                #endif
            #endif

            #if GTK_MAJOR_VERSION > 2
                fdft->category_cb = gtk_combo_box_text_new();
                fdft->type_cb = gtk_combo_box_text_new();
                fdft->feature_cb = gtk_combo_box_text_new();
            #endif

            main_struct->fdft = fdft;

            vbox = heraia_get_widget(main_struct->xmls->main, "fdft_category_vbox");
            gtk_box_pack_end(GTK_BOX(vbox), fdft->category_cb, FALSE, FALSE, 0);

            vbox = heraia_get_widget(main_struct->xmls->main, "fdft_type_vbox");
            gtk_box_pack_end(GTK_BOX(vbox), fdft->type_cb, FALSE, FALSE, 0);

            vbox = heraia_get_widget(main_struct->xmls->main, "fdft_feature_vbox");
            gtk_box_pack_end(GTK_BOX(vbox), fdft->feature_cb, FALSE, FALSE, 0);

            /* Combo box button */
            g_signal_connect(G_OBJECT(fdft->category_cb), "changed",
                             G_CALLBACK(fdft_category_cb_changed), main_struct);

            return fdft;
        }
    else
        {
            return NULL;
        }
}


/**
 * Inits all the things in the find data from type window (signal and such)
 * @param main_struct : heraia's main structure
 */
void fdft_window_init_interface(heraia_struct_t * main_struct)
{

    if (main_struct != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
        {
            fdft_window_connect_signal(main_struct);
            main_struct->fdft = fdft_window_init_widgets(main_struct);
            fdft_window_populate_category_cb(main_struct);
        }
}

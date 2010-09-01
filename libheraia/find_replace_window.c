/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  find_replace_window.c
  find_replace_window.c - Windows used to find text and find and replace text in
                          the opened hex documents.


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
 * @file find_replace_window.c
 * Windows used to find text and find and replace text in the opened hex
 * documents.
 */
#include <libheraia.h>

/*** common stuff ***/
static doc_t *create_find_or_replace_doc_t(void);
static void find_replace_add_ghex_widget(xml_t *xmls, gchar *widget_name, doc_t *entry);

/*** find window ***/
static gboolean delete_find_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_find_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void find_window_close(GtkWidget *widget, gpointer data);
static void find_window_connect_signal(heraia_struct_t *main_struct);

/*** find and replace window ***/
static gboolean delete_fr_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_fr_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void fr_window_close(GtkWidget *widget, gpointer data);
static void fr_window_connect_signal(heraia_struct_t *main_struct);

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
 * Signal connections for the find window
 * @param main_struct : heraia's main structure
 */
static void find_window_connect_signal(heraia_struct_t *main_struct)
{
    /* Close button */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "find_close_bt")), "clicked",
                     G_CALLBACK(find_window_close), main_struct);

    /* When result window is killed or destroyed */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "result_window")), "delete_event",
                     G_CALLBACK(delete_find_window_event), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "result_window")), "destroy",
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


static void find_replace_add_ghex_widget(xml_t *xmls, gchar *widget_name, doc_t *entry)
{
    GtkWidget *al = NULL;

    al = heraia_get_widget(xmls->main, widget_name);
    gtk_container_add(GTK_CONTAINER(al), entry->hex_widget);
    gtk_container_set_border_width(GTK_CONTAINER(al), 3);


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

    /* When result window is killed or destroyed */
    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fr_window")), "delete_event",
                     G_CALLBACK(delete_fr_window_event), main_struct);

    g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "fr_window")), "destroy",
                     G_CALLBACK(destroy_fr_window_event), main_struct);
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

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  list_data_types.c
  Window allowing the user to manage his data types (add, remove, edit,
  save and load them)
  
  (C) Copyright 2005 - 2007 Olivier Delhomme
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

#include "heraia_types.h"

/**
 *  Shows or hide the list data type window
 */
void on_ldt_menu_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *check_menu_item = NULL;

	if (main_window != NULL)
		{
			check_menu_item = glade_xml_get_widget(main_window->xml, "ldt_menu");

			if (is_cmi_checked(check_menu_item) == TRUE)
				{  /* if the menu is checked, shows the window */
					gtk_widget_show_all(glade_xml_get_widget(main_window->xml, "list_data_types_window")); 
				}
			else
				{
					gtk_widget_hide(glade_xml_get_widget(main_window->xml, "list_data_types_window")); 
				}
		}
}

/**
 *  Adds the data type name to the treeview
 */
void add_data_type_name_to_treeview(heraia_window_t *main_window, gchar *name)
{
	GtkListStore *list_store = NULL;  /* Treeview Stuff for rendering */
	GtkTreeIter iter;                 /* the text in it.              */
	GtkCellRenderer *renderer = NULL; 
	GtkTreeViewColumn *column = NULL;  
	GtkTreeView *treeview = NULL;     /* Treeview where data type name will be displayed */

	treeview = GTK_TREE_VIEW(glade_xml_get_widget(main_window->xml, "ldt_treeview"));

	list_store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));

	if (list_store == NULL)
		{
			list_store = gtk_list_store_new(LDT_TV_N_COLUMNS, G_TYPE_STRING);
			renderer = gtk_cell_renderer_text_new();
			column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", LDT_TV_COLUMN_NAME, NULL);
			gtk_tree_view_append_column(treeview, column);
		}

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, LDT_TV_COLUMN_NAME, name, -1);
	gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(list_store));
}


/**
 *  Called when the list data types is killed or closed
 *  !! Not to be confused with delete_dt_window_event !!
 */
static gboolean delete_ldt_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{	
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name(glade_xml_get_widget(main_window->xml, "ldt_menu"), "activate");

	return TRUE;
}


/**
 *  Called when the list data types is killed or closed
 *  !! Not to be confused with destroy_dt_window !!
 */
static void destroy_ldt_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name(glade_xml_get_widget(main_window->xml, "ldt_menu"), "activate");
}



/**
 *  When the add button (+) is clicked
 */
static void ldt_add_button_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	clear_data_type_widgets(main_window);
	gtk_widget_show_all(glade_xml_get_widget(main_window->xml, "data_type_window"));
}


/**
 *  Connects list_data_types's window signals.
 */
static void connect_list_data_types_signals(heraia_window_t *main_window)
{

	if (main_window != NULL && main_window->xml != NULL)
		{
			/* list data types menu */
			g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "ldt_menu")), "activate", 
							  G_CALLBACK(on_ldt_menu_activate), main_window);
      
			/* list data types window killed or destroyed */
			g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "list_data_types_window")), "delete_event", 
							  G_CALLBACK(delete_ldt_window_event), main_window);

			g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "list_data_types_window")), "destroy", 
							  G_CALLBACK(destroy_ldt_window), main_window);

			/* Add button */
			g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "ldt_add_button")), "clicked", 
							  G_CALLBACK(ldt_add_button_clicked), main_window);
		}
}


/**
 *  Inits the list data type window
 *  with default values
 *  Should be called only once
 */
void list_data_types_init_interface(heraia_window_t *main_window)
{

	if (main_window != NULL)
		{
			connect_list_data_types_signals(main_window);
		}
}

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  list_data_types.c
  Window allowing the user to manage his data types (add, remove, edit,
  save and load them)
  
  (C) Copyright 2007 - 2007 Olivier Delhomme
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

static gboolean delete_ldt_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_ldt_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void ldt_add_button_clicked(GtkWidget *widget, gpointer data);
static void ldt_remove_button_clicked(GtkWidget *widget, gpointer data);
static void ldt_edit_button_clicked(GtkWidget *widget, gpointer data);
static void ldt_save_button_clicked(GtkWidget *widget, gpointer data);
static void connect_list_data_types_signals(heraia_window_t *main_window);

/**
 *  Shows or hide the list data type window
 */
void on_ldt_menu_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *check_menu_item = NULL;

	if (main_window != NULL)
		{
			check_menu_item = heraia_get_widget(main_window->xmls->main, "ldt_menu");

			if (is_cmi_checked(check_menu_item) == TRUE)
				{  /* if the menu is checked, shows the window */
					move_and_show_dialog_box(heraia_get_widget(main_window->xmls->main, "list_data_types_window"), main_window->win_prop->ldt);
				}
			else
				{
					record_and_hide_dialog_box(heraia_get_widget(main_window->xmls->main, "list_data_types_window"), main_window->win_prop->ldt);
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

	treeview = GTK_TREE_VIEW(heraia_get_widget(main_window->xmls->main, "ldt_treeview"));

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

	g_signal_emit_by_name(heraia_get_widget(main_window->xmls->main, "ldt_menu"), "activate");

	return TRUE;
}


/**
 *  Called when the list data types is killed or closed
 *  !! Not to be confused with destroy_dt_window !!
 */
static void destroy_ldt_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name(heraia_get_widget(main_window->xmls->main, "ldt_menu"), "activate");
}


/**
 *  When the add button (+) is clicked
 */
static void ldt_add_button_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	main_window->current_data_type = new_data_type("", DT_SPIN_MIN);

	/* data interpretor widget creation */
	create_ud_data_interpretor_widgets(main_window, main_window->current_data_type);

	show_data_type_window(main_window, main_window->current_data_type);
}


/**
 *  When the remove (-) button is clicked
 */
static void ldt_remove_button_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GList *data_type_list = main_window->data_type_list;
	GtkTreeView *treeview = NULL; 
	GtkTreeSelection *selection = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = NULL;
	GtkListStore *list_store = NULL;
	gchar *name = NULL;
	data_type_t *a_data_type = NULL;

	treeview = GTK_TREE_VIEW(heraia_get_widget(main_window->xmls->main, "ldt_treeview"));

	/* gets the selection of the treeview (if any) */
	selection = gtk_tree_view_get_selection(treeview);

	if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{    
			gtk_tree_model_get(model, &iter, LDT_TV_COLUMN_NAME, &name, -1);
			data_type_list = is_data_type_name_already_used(data_type_list, name);
			
			if (data_type_list != NULL)
				{
					/* frees internal structure */
					a_data_type = (data_type_t *) data_type_list->data;
					
					/* the data interpretor part */
					destroy_a_single_widget(a_data_type->di_label);
					destroy_a_single_widget(a_data_type->di_entry);
					free_data_type(a_data_type);

					main_window->data_type_list = g_list_delete_link(main_window->data_type_list, data_type_list);

					/* frees the treeview accordingly */
					list_store = GTK_LIST_STORE(model);
					if (list_store != NULL)
						{
							gtk_list_store_remove(list_store, &iter);
							gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(list_store));
						}
				}
		}
}


/**
 *  When the edit button is clicked
 */
static void ldt_edit_button_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	GList *data_type_list = main_window->data_type_list;
	GtkTreeSelection *selection = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = NULL;
	gchar *name = NULL;

	/* gets the selection of the treeview (if any) */
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(heraia_get_widget(main_window->xmls->main, "ldt_treeview")));

	if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{    
			gtk_tree_model_get(model, &iter, LDT_TV_COLUMN_NAME, &name, -1);
			data_type_list = is_data_type_name_already_used(data_type_list, name);
			
			if (data_type_list != NULL)
				{		
					/** 
					 *  We use a copy because we want to be able to be in edit mode even if
					 *  the user clicked add(+) button.
					 */
					main_window->current_data_type = copy_data_type_struct(main_window, data_type_list->data);
					
					show_data_type_window(main_window, main_window->current_data_type);
				}
		}
}


/**
 *  When the save button is clicked
 */
static void ldt_save_button_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	/* GList *data_type_list = main_window->data_type_list; */

	/**
	 *  I'll do this later ...
	 *  I first have to understand XML !
	 */
	log_message(main_window, G_LOG_LEVEL_WARNING, "This function is not yet implemented. Please Contribute :)");

}


/**
 *  Connects list_data_types's window signals.
 */
static void connect_list_data_types_signals(heraia_window_t *main_window)
{

	if (main_window != NULL && main_window->xmls != NULL && main_window->xmls->main != NULL)
		{
			/* list data types menu */
			g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "ldt_menu")), "activate", 
							 G_CALLBACK(on_ldt_menu_activate), main_window);
      
			/* list data types window killed or destroyed */
			g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "list_data_types_window")), "delete_event", 
							 G_CALLBACK(delete_ldt_window_event), main_window);

			g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "list_data_types_window")), "destroy", 
							 G_CALLBACK(destroy_ldt_window), main_window);

			/* Add button    */
			g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "ldt_add_button")), "clicked", 
							 G_CALLBACK(ldt_add_button_clicked), main_window);

			/* Remove button */
			g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "ldt_remove_button")), "clicked", 
							 G_CALLBACK(ldt_remove_button_clicked), main_window);

			/* Edit button   */
			g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "ldt_edit_button")), "clicked", 
							 G_CALLBACK(ldt_edit_button_clicked), main_window);

			/* Save button   */
			g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "ldt_save_button")), "clicked", 
							 G_CALLBACK(ldt_save_button_clicked), main_window);
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

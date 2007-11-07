/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  plugin_list.c
 *  manage the plugin_list_window window
 * 
 *  (C) Copyright 2007 Olivier Delhomme
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

#include "heraia_types.h"

static gboolean delete_plw_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_plw_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void plw_close_clicked(GtkWidget *widget, gpointer data);
static void plw_refresh_clicked(GtkWidget *widget, gpointer data);
static void mw_cmi_plw_toggle(GtkWidget *widget, gpointer data);
static void pn_treeview_selection_changed_cb(GtkTreeSelection *selection, gpointer data);

static void plugin_list_window_connect_signals(heraia_window_t *main_window);

static void init_plugin_name_tv(heraia_window_t *main_window);
static void print_plugin_basics(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_interface(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_filter_structure(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_functions(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_info_structure(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_extra_structure(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_state(GtkTextView *textview, heraia_plugin_t *plugin);

/*** call back function for the plugins_window ***/
/**
 *  Signal handler called when the user closes the window */
static gboolean delete_plw_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	plw_close_clicked(widget, data);

	return TRUE;
}

static void destroy_plw_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	plw_close_clicked(widget, data);
}

/**
 *  Closing the window effectively
 */
static void plw_close_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkCheckMenuItem *cmi = GTK_CHECK_MENU_ITEM(glade_xml_get_widget(main_window->xml, "mw_cmi_plugin_list"));

	show_hide_widget(GTK_WIDGET(glade_xml_get_widget(main_window->xml, "plugin_list_window")), FALSE);
	gtk_check_menu_item_set_active(cmi, FALSE);
}


/**
 *  Refreshing the window effectively
 */
static void plw_refresh_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkTextView *textview = GTK_TEXT_VIEW(glade_xml_get_widget(main_window->xml, "plugin_info_textview"));

	init_plugin_name_tv(main_window);
	kill_text_from_textview(textview);	
}



/**
 *  When the toggle button 'Liste des plugins' is toggled !
 */
static void mw_cmi_plw_toggle(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkCheckMenuItem *cmi = GTK_CHECK_MENU_ITEM(glade_xml_get_widget(main_window->xml, "mw_cmi_plugin_list"));
	GtkPaned *paned = GTK_PANED(glade_xml_get_widget(main_window->xml, "plw_hpaned"));
	gint pos = 0;

	if (gtk_check_menu_item_get_active(cmi) == TRUE)
		{
			pos = gtk_paned_get_position(paned);
			if (pos < 15)
				{
					pos = 100;
					gtk_paned_set_position(paned, pos);
				}
		}

	show_hide_widget(GTK_WIDGET(glade_xml_get_widget(main_window->xml, "plugin_list_window")), 
					 gtk_check_menu_item_get_active(cmi));
}


/**
 *  adds to the textview the relevant informations about the plugin info structure !
 */
static void print_plugin_info_structure(GtkTextView *textview, heraia_plugin_t *plugin)
{
	if (plugin->info != NULL)
		{
			switch (plugin->info->type)
				{
				case HERAIA_PLUGIN_UNKNOWN:
					add_text_to_textview(textview, "Type        : Unknown\n");
					break;

				case HERAIA_PLUGIN_FILTER:
					add_text_to_textview(textview, "Type        : Filter\n");
					break;

				case HERAIA_PLUGIN_ACTION:
					add_text_to_textview(textview, "Type        : Action\n");
					break;

				default:
					add_text_to_textview(textview, "Type        : Unknown\n");
				}

			add_text_to_textview(textview, "Priority    : %d\n", plugin->info->priority);
			add_text_to_textview(textview, "Id          : %d\n", plugin->info->id);

			if (plugin->info->name != NULL)
				{
					add_text_to_textview(textview, "Name        : %s\n", plugin->info->name);
				}

			if (plugin->info->version != NULL)
				{
					add_text_to_textview(textview, "Version     : %s\n", plugin->info->version);
				}

			if (plugin->info->summary != NULL)
				{
					add_text_to_textview(textview, "Summary     : %s\n", plugin->info->summary);
				}

			if (plugin->info->description != NULL)
				{
					add_text_to_textview(textview, "Description : %s\n", plugin->info->description);
				}

			if (plugin->info->author != NULL)
				{
					add_text_to_textview(textview, "Author      : %s\n", plugin->info->author);
				}

			if (plugin->info->homepage != NULL)
				{
					add_text_to_textview(textview, "Web site    : %s\n", plugin->info->homepage);
				}
		}
	else
		{
			add_text_to_textview(textview, "The 'info' structure is not initialized !\n");
		}
}

/**
 *  adds to the textview the relevant informations about the plugin functions !
 */
static void print_plugin_functions(GtkTextView *textview, heraia_plugin_t *plugin)
{

	if (plugin->init_proc != NULL || 
		plugin->run_proc != NULL  ||
		plugin->quit_proc != NULL ||
		plugin->refresh_proc != NULL)
		{
			add_text_to_textview(textview, "\nPlugin's defined functions :\n");

			if (plugin->init_proc != NULL)
				{
					add_text_to_textview(textview, "    - Initialization function : %p\n", plugin->init_proc);
				}
			
			if (plugin->run_proc != NULL)
				{
					add_text_to_textview(textview, "    - Main function           : %p\n", plugin->run_proc);
				}

			if (plugin->quit_proc != NULL)
				{
					add_text_to_textview(textview, "    - Exit function           : %p\n", plugin->quit_proc);
				}

			if (plugin->refresh_proc != NULL)
				{
					add_text_to_textview(textview, "    - Refresh function        : %p\n", plugin->refresh_proc);
				}
		}
	else
		{
			add_text_to_textview(textview, "\nThis plugin does not provide any function !!\n");
		}
}

/**
 *  adds to the textview the relevant informations about the plugin filter structure !
 */
static void print_plugin_filter_structure(GtkTextView *textview, heraia_plugin_t *plugin)
{
	if (plugin->filter != NULL)
		{
			if (plugin->filter->import != NULL || 
				plugin->filter->export != NULL)
				{
					add_text_to_textview(textview, "\nFilter functions :\n");

					if (plugin->filter->import != NULL)
						{
							add_text_to_textview(textview, "    - Import function : %p\n", plugin->filter->import);
						}

					if (plugin->filter->export != NULL)
						{
							add_text_to_textview(textview, "    - Export function : %p\n", plugin->filter->export);
						}
				}
			else
				{
					add_text_to_textview(textview, "\nThis plugin does not provide any filter function\n");
				}
		}
	else
		{
			add_text_to_textview(textview, "\nThe structure 'filter' is not initialized !");
		}
}

/**
 *  adds to the textview the relevant informations about the plugin interface (xml) !
 */
static void print_plugin_interface(GtkTextView *textview, heraia_plugin_t *plugin)
{
	add_text_to_textview(textview, "\nThis plugin provides :\n");

	if (plugin->cmi_entry != NULL)
		{
			add_text_to_textview(textview, "  - a menu entry in the plugins menu.\n");
		}
	else
		{
			add_text_to_textview(textview, "  - no menu entry.\n");
		}
	  
	if (plugin->xml != NULL)
		{
			add_text_to_textview(textview, "  - an xml interface.\n");
		}
	else
		{
			add_text_to_textview(textview, "  - no xml interface.\n");
		}
}

/**
 *  adds to the textview the relevant informations about the plugin basics !
 */
static void print_plugin_basics(GtkTextView *textview, heraia_plugin_t *plugin)
{
	if (plugin->info != NULL)
		{
			add_text_to_textview(textview, "API version : %d\n", plugin->info->api_version);
		}

	if (plugin->filename != NULL)
		{
			add_text_to_textview(textview, "File        : %s\n", plugin->filename);
		}

	if (plugin->path != NULL)
		{
			add_text_to_textview(textview, "Directory  : %s\n", plugin->path);
		}

	if (plugin->handle != NULL)
		{
			add_text_to_textview(textview, "Handle      : %p\n", plugin->handle);
		}
	else
		{
			add_text_to_textview(textview, "Handle      : NONE <-- Is there anything normal ?\n");
		}
}

/**
 *  adds to the textview the relevant informations about the plugin extra structure !
 */
static void print_plugin_extra_structure(GtkTextView *textview, heraia_plugin_t *plugin)
{

	if (plugin->extra != NULL)
		{
			add_text_to_textview(textview, "\nThis plugin has an additionnal 'extra' structure (%p) sized %d bytes.\n", 
								 plugin->extra, sizeof(*(plugin->extra)));
		}
	else
		{
			add_text_to_textview(textview, "\nThis plugin does not have any additionnal structure.\n");
		}

}

/**
 *  adds to the textview the relevant informations about the plugin state !
 */
static void print_plugin_state(GtkTextView *textview, heraia_plugin_t *plugin)
{

	add_text_to_textview(textview, "Plugin's state : ");
	switch (plugin->state)
		{
		case PLUGIN_STATE_RUNNING:
			add_text_to_textview(textview, "Running\n");
			break;

		case PLUGIN_STATE_INITIALIZING:
			add_text_to_textview(textview, "Initialiazing or initialized\n");
			break;
		case PLUGIN_STATE_LOADED:
			add_text_to_textview(textview, "Loaded\n");
			break;

		case PLUGIN_STATE_NEW:
			add_text_to_textview(textview, "Creating itself\n");
			break;

		case PLUGIN_STATE_EXITING:
			add_text_to_textview(textview, "Exiting\n");
			break;

		case PLUGIN_STATE_NONE:
			add_text_to_textview(textview, "Waiting\n");
			break;

		default:
			add_text_to_textview(textview, "Unknown\n");
		}
}

/**
 *  Function called when the selection changes in the treeview
 *  Displays informations about the selected plugin
 */
static void pn_treeview_selection_changed_cb(GtkTreeSelection *selection, gpointer data)
{
	GtkTreeIter iter;            
	GtkTreeModel *model = NULL;
	heraia_window_t *main_window = (heraia_window_t *) data;
	gchar *name = NULL;
	heraia_plugin_t *plugin = NULL;
	GtkTextView *textview = GTK_TEXT_VIEW(glade_xml_get_widget(main_window->xml, "plugin_info_textview"));

	if (gtk_tree_selection_get_selected(selection, &model, &iter))
		{
			gtk_tree_model_get(model, &iter, PNTV_COLUMN_NAME, &name, -1);
			plugin = find_plugin_by_name(main_window->plugins_list, name);

			if (plugin != NULL)
				{
					kill_text_from_textview(textview);
					
					print_plugin_basics(textview, plugin);
										
					print_plugin_info_structure(textview, plugin);

					print_plugin_functions(textview, plugin);

					print_plugin_filter_structure(textview, plugin);

					print_plugin_interface(textview, plugin);

					print_plugin_extra_structure(textview, plugin);

					print_plugin_state(textview, plugin);
				}
		}
}
/*** End of callback functions that handle the plugins window ***/


static void plugin_list_window_connect_signals(heraia_window_t *main_window)
{
	GtkTreeSelection *select = NULL;
 
	if (main_window != NULL)
		{
			/* When the plugin list window is destroyed or killed */
			g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "plugin_list_window")), "delete_event", 
							 G_CALLBACK(delete_plw_window_event), main_window);

			g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "plugin_list_window")), "destroy", 
							 G_CALLBACK(destroy_plw_window), main_window);
	
			/* Close button */
			g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "plw_close_b")), "clicked", 
							 G_CALLBACK(plw_close_clicked), main_window);

			/* The toogle button */
			g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "mw_cmi_plugin_list")), "toggled",
							 G_CALLBACK(mw_cmi_plw_toggle), main_window);
     
			/* Selection has changed for the pn_treeview */
			select = gtk_tree_view_get_selection(GTK_TREE_VIEW(glade_xml_get_widget(main_window->xml, "pn_treeview")));
			gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
			g_signal_connect (G_OBJECT(select), "changed", G_CALLBACK (pn_treeview_selection_changed_cb), main_window);

			/* Refresh button */
			g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "plw_refresh_b")), "clicked", 
							 G_CALLBACK(plw_refresh_clicked), main_window);
		}
}

/**
 *  the function to init the first treeview (plugin names) 
 */
static void init_plugin_name_tv(heraia_window_t *main_window)
{
	GtkListStore *list_store = NULL;  /* Treeview Stuff for rendering */
	GtkTreeIter iter;                 /* the text in it.              */
	GtkCellRenderer *renderer = NULL; 

	GtkTreeViewColumn *column = NULL;  
	heraia_plugin_t *plugin = NULL;   /* plugin interface structure   */
	GList *p_list = NULL;             /* plugin list                  */
	GtkTreeView *treeview = NULL;     /* Treeview where plugin names are to be displayed */

	if (main_window != NULL)
		{
			treeview = GTK_TREE_VIEW(glade_xml_get_widget(main_window->xml, "pn_treeview"));

			p_list = g_list_first(main_window->plugins_list);

			list_store = gtk_list_store_new(PNTV_N_COLUMNS, G_TYPE_STRING);
      
			while (p_list != NULL)
				{
					plugin = (heraia_plugin_t *) p_list->data;
					log_message(main_window, G_LOG_LEVEL_INFO, "%s", plugin->info->name);

					gtk_list_store_append(list_store, &iter);
					gtk_list_store_set(list_store, &iter, PNTV_COLUMN_NAME, plugin->info->name, -1);
                        
					p_list = p_list->next;
				}
		   
			gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(list_store));

			column = gtk_tree_view_get_column(treeview, PNTV_COLUMN_NAME);

			if (column != NULL)
				{
					gtk_tree_view_remove_column(treeview, column);
				}
		   
			renderer = gtk_cell_renderer_text_new();
			column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", PNTV_COLUMN_NAME, NULL);
			gtk_tree_view_append_column(treeview, column);
		}
}


/**
 *  the function to init the plugin_list_window interface 
 */
void plugin_list_window_init_interface(heraia_window_t *main_window)
{

	plugin_list_window_connect_signals(main_window);

	init_plugin_name_tv(main_window);
}







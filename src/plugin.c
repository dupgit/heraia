/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  plugin.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
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

static heraia_plugin_t *get_plugin_handle(heraia_window_t *main_window, heraia_plugin_t *plugin, 
										  const gchar *full_filename, const gchar *filename);
static heraia_plugin_t *get_plugin_init_symbol(heraia_window_t *main_window, heraia_plugin_t *plugin);
static void init_plugin(heraia_window_t *main_window, heraia_plugin_t *plugin, const gchar *filename, guint plugins_nb);
static void load_one_plugin(heraia_window_t *main_window, const gchar *filename, guint plugins_nb);

gboolean plugin_capable(void)
{
	return g_module_supported();
}

/**  
 *  Creates a new empty plugin 
 *  it may be initialised by
 *  the plugin itself !
 */
heraia_plugin_t *new_plugin(void)
{
	heraia_plugin_t *new = NULL;
	
	new = (heraia_plugin_t *) g_malloc0(sizeof(heraia_plugin_t));

	new->state = PLUGIN_STATE_NEW; /* The plugin state         */
	new->handle = NULL;      /* The module handle        */
	new->path = NULL;        /* The path to the plugin   */
	new->filename = NULL;
	new->info = (plugin_info_t *) g_malloc0(sizeof(plugin_info_t));       /* The plugin information   */
	new->filter = (plugin_filter_t *) g_malloc0(sizeof(plugin_filter_t)); /* The plugin filter        */
	new->error = NULL;
	new->extra = NULL;      /* Plugin-specific data     */

	/* Called when the application initialy starts up */
	new->init_proc = NULL;
	/* Called when the application exits */
	new->quit_proc = NULL;
	/* Called to run a miscellaneous thing everytime the plugin is called */
	new->run_proc = NULL;

	return new;

}

/**
 *  free an unused plugin
 */
void free_plugin(heraia_plugin_t *plugin)
{
	if (plugin != NULL)
		{
			if (plugin->handle != NULL)
				g_module_close(plugin->handle);
			if (plugin->info != NULL)
				{
					g_free(plugin->info->name);
					g_free(plugin->info->version);
					g_free(plugin->info->summary);
					g_free(plugin->info->description);
					g_free(plugin->info->author);
					g_free(plugin->info->homepage);
					g_free(plugin->info);
				}
			if (plugin->filter != NULL)
				{
					g_free(plugin->filter->extensions);
					g_free(plugin->filter);
				}
			g_free(plugin->path);
			g_free(plugin->filename);
			g_free(plugin->error);
			g_free(plugin->extra);
			
			g_free(plugin);
		}
}

/**
 *  Here we try to get a handle for the Gmodule referenced by full_filename
 */
static heraia_plugin_t *get_plugin_handle(heraia_window_t *main_window, heraia_plugin_t *plugin, 
										  const gchar *full_filename, const gchar *filename)
{
	if (plugin != NULL)
		{

			plugin->handle = g_module_open(full_filename, G_MODULE_BIND_MASK);
			
			if (plugin->handle == NULL)
				log_message(main_window, G_LOG_LEVEL_WARNING, "Could not open plugin %s - %s", filename, g_module_error());
		}

	return plugin;
}

/**
 *  If the handle is ok, we want to have the heraia_plugin_init function (to call it)
 *  in order to init the plugin (by itself)
 */ 
static heraia_plugin_t *get_plugin_init_symbol(heraia_window_t *main_window, heraia_plugin_t *plugin)
{
	heraia_plugin_t *(*heraia_plugin_init)(heraia_plugin_t *);
	gboolean get_symbol = FALSE;

	if (plugin != NULL && plugin->handle != NULL)
		{
			get_symbol = g_module_symbol(plugin->handle, "heraia_plugin_init", (gpointer *)(&heraia_plugin_init));

			if (get_symbol == FALSE)
				{
					log_message(main_window, G_LOG_LEVEL_WARNING, "%s", g_module_error());
					free_plugin(plugin);
					return NULL;
				}
			else
				{
					/* the plugins inits itself here */
					plugin = heraia_plugin_init(plugin);
					return plugin;
				}
		}
	else
		{
			free_plugin(plugin);
			return NULL;
		}
}

/**
 *  finalising initialisation : if everything went fine, the plugin is added to the 
 *  plugin list and a menu entry is created in the Plugins menu
 */
static void init_plugin(heraia_window_t *main_window, heraia_plugin_t *plugin, const gchar *filename, guint plugins_nb)
{	
	if (plugin != NULL)
		{
			plugin->info->id = plugins_nb;
			main_window->plugins_list = g_list_append(main_window->plugins_list, plugin);
			log_message(main_window, G_LOG_LEVEL_INFO, "plugin %s loaded.", filename);
			plugin->init_proc(main_window);
			if (plugin->info->type == HERAIA_PLUGIN_ACTION)
				{
					add_entry_to_plugins_menu(main_window, plugin);
				}
				
		}
 }

/**
 *  Here we manage to load on plugin at a time (and this is really enough !)
 */
static void load_one_plugin(heraia_window_t *main_window, const gchar *filename, guint plugins_nb)
{
	const gchar *full_filename = NULL;
	heraia_plugin_t *plugin = NULL;
	gchar *ext = g_strdup_printf(".%s", G_MODULE_SUFFIX); /* system dependent suffix */

	full_filename = g_build_filename(PLUGINS_DIR, filename, NULL);

	/* Make sure we do load the module named .so, .dll or .sl depending on the OS type */
	if ( (g_file_test(full_filename, G_FILE_TEST_IS_DIR) == FALSE) &&
		 (strcmp(strrchr(filename, '.'), ext) == 0)
		 )
		{
			plugin = new_plugin();
			plugin->path = g_strdup_printf("%s", PLUGINS_DIR);
			plugin->filename = g_strdup_printf("%s", filename);
							
			plugin = get_plugin_handle(main_window, plugin, full_filename, filename);
			plugin = get_plugin_init_symbol(main_window, plugin);
		   
			init_plugin(main_window, plugin, filename, plugins_nb);
		}
}


/**
 *  looks at the plugins dir(s) and loads
 *  the needed plugins (all ;-) (one at a time !!)
 */
void load_plugins(heraia_window_t *main_window)
{
	GDir *plugins_dir = NULL;
	GError *error = NULL;
	const gchar *filename = NULL;
	unsigned int plugins_nb = 0;

	/* Register all shared plugins (plugins_dir) (-DPLUGINS_DIR) */
    /* This may be a config file option later ...                */
	plugins_dir = g_dir_open(PLUGINS_DIR, 0, &error);

	if (plugins_dir == NULL) /* error while openning the plugins directory */
		{  
			log_message(main_window, G_LOG_LEVEL_WARNING, "%s", error->message);
			g_error_free(error);
		}
	else
		{
			while ((filename = g_dir_read_name(plugins_dir)) != NULL) 
				{
					load_one_plugin(main_window, filename, ++plugins_nb);
				}
			g_dir_close(plugins_dir);
		}
}

/**
 *  adds a menu entry to the plugin menu
 *  adds a signal handler when the menu is toggled
 */
void add_entry_to_plugins_menu(heraia_window_t *main_window, heraia_plugin_t *plugin)
{
	/* Creates the menu entry (a GtkCheckMenuItem) */
	plugin->cmi_entry = GTK_CHECK_MENU_ITEM(gtk_check_menu_item_new_with_label(plugin->info->name));

	/* Append this menu entry to the menu */
	gtk_menu_shell_append(GTK_MENU_SHELL(glade_xml_get_widget(main_window->xml, "plugins_menu")), GTK_WIDGET(plugin->cmi_entry));

	/* Connect the menu entry toggled signal to the run_proc function of the plugin */
	g_signal_connect(G_OBJECT(plugin->cmi_entry), "toggled", G_CALLBACK(plugin->run_proc), main_window);

	/* Shows the menu entry (so we may toggle it!) */
	gtk_widget_show(GTK_WIDGET(plugin->cmi_entry));
}

/**
 *  Finds the desired plugin by its name and return the plugin structure or NULL
 */
heraia_plugin_t *find_plugin_by_name(GList *plugins_list, gchar *name)
{
	GList *list = g_list_first(plugins_list);
	heraia_plugin_t *plugin = NULL;
	gboolean stop = FALSE;

	while (list != NULL && stop != TRUE)
		{
			plugin = (heraia_plugin_t *) list->data;
			if (plugin != NULL && plugin->info != NULL)
				{
					if (strcmp(plugin->info->name, name) == 0)
						stop = TRUE;
				}
			list = list->next;
 		}
	if (stop == TRUE)
		return plugin;
	else
		return NULL;
}


/** 
 *  Loads the glade xml file that describes the plugin (.glade suffix)
 *  tries the paths found in the location_list
 */
gboolean load_plugin_glade_xml(heraia_window_t *main_window, heraia_plugin_t *plugin)
{	
	gchar *filename = NULL;

	filename = g_strdup_printf("%s.glade", plugin->info->name);	
      
	plugin->xml = load_glade_xml_file(main_window->location_list, filename);

	g_free(filename);
	
	if (plugin->xml == NULL)
		return FALSE;
	else
		return TRUE;
}

/**
 *  To help plugins to deal with widgets, shows or hide a specific widget
 */
void show_hide_widget(GtkWidget *widget, gboolean show)
{
	if (show)
		gtk_widget_show(widget);
	else
		gtk_widget_hide(widget);
}


/**
 *  To help the main program to send events to the plugins
 */
void refresh_all_plugins(heraia_window_t *main_window)
{
	
	GList *list = g_list_first(main_window->plugins_list);
	heraia_plugin_t *plugin = NULL;

	while (list != NULL)
		{
			plugin = (heraia_plugin_t *) list->data;
			if (plugin != NULL && plugin->refresh_proc != NULL)
				plugin->refresh_proc(main_window, list->data);  /* Une petite astuce ! */
				
			list = list->next;
 		}
}
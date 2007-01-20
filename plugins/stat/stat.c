/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  stat.c
  an heraia plugin to calculate some stats on the opened file
 
  (C) Copyright 2007 Olivier Delhomme
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

#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "types.h"
#include "plugin.h"
#include "stat.h"
#include "heraia_ui.h"
#include "ghex_heraia_interface.h"

/* The functions for the plugin's usage */
static void stat_window_connect_signals(heraia_plugin_t *plugin);
static void statw_close_clicked(GtkWidget *widget, gpointer data);
static void destroy_stat_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static gboolean delete_stat_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data );
static void realize_some_numerical_stat(heraia_window_t *main_struct, heraia_plugin_t *plugin);
static void init_stats_histos(heraia_plugin_t *plugin);
static void populate_stats_histos(heraia_window_t *main_struct, heraia_plugin_t *plugin);

/**
 *  Initialisation plugin called when the plugin is loaded (some sort of pre-init)
 */
heraia_plugin_t *heraia_plugin_init(heraia_plugin_t *plugin)
{
	stat_t *extra;

	plugin->state             = PLUGIN_STATE_INITIALIZING;
	plugin->xml = NULL;

  	plugin->info->api_version = API_VERSION;
	plugin->info->type        = PLUGIN_TYPE;
	plugin->info->priority    = HERAIA_PRIORITY_DEFAULT;
	plugin->info->name        = PLUGIN_NAME;
	plugin->info->version     = PLUGIN_VERSION;
	plugin->info->summary     = PLUGIN_SUMMARY;
	plugin->info->description = PLUGIN_DESCRIPTION;
	plugin->info->author      = PLUGIN_AUTHOR;
	plugin->info->homepage    = PLUGIN_HOMEPAGE;

	plugin->init_proc    = init;
	plugin->quit_proc    = quit;
	plugin->run_proc     = run;
	plugin->refresh_proc = refresh;

	plugin->filter->extensions = NULL;
	plugin->filter->import     = NULL;
	plugin->filter->export     = NULL;

	/* add the extra struct to the plugin one */
	extra = (stat_t *) g_malloc0 (sizeof(stat_t));
	plugin->extra = extra;

	return plugin;
}

/* the plugin interface functions */

/**
 *  The real init function of the plugin
 */
void init(heraia_window_t *main_struct)
{
	heraia_plugin_t *plugin = NULL;

	log_message(main_struct, G_LOG_LEVEL_INFO, "Initializing plugin %s", PLUGIN_NAME);
	/* first, know who we are ! */
	plugin = find_plugin_by_name(main_struct->plugins_list, PLUGIN_NAME);
	
	if (plugin != NULL)
		{
			/* load the xml interface */
			log_message(main_struct, G_LOG_LEVEL_INFO, "Plugin from %s found !", plugin->info->author);
			if (load_plugin_glade_xml(main_struct, plugin) == TRUE)
				log_message(main_struct, G_LOG_LEVEL_INFO, "%s xml interface loaded.", plugin->info->name);
			else
				log_message(main_struct, G_LOG_LEVEL_WARNING, "Unable to load %s xml interface.", plugin->info->name);
			
			/* hide the interface */
			gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(plugin->xml, "stat_window")));

			/* connect some signals handlers */
			stat_window_connect_signals(plugin);

		}
}

/**  
 *  Normaly this is called when the plugin is unloaded
 *  One may wait it's entire life for this to be called !! ;)
 */ 
void quit(void)
{
	g_print("Quitting %s\n", PLUGIN_NAME);
}

/**
 *  This function is called via a signal handler when the menu entry is toggled
 */
void run(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_struct = (heraia_window_t *) data; /* the signal send the pointer to this structure */
	heraia_plugin_t *plugin = NULL;

	/* first, know who we are ! */
	plugin = find_plugin_by_name(main_struct->plugins_list, PLUGIN_NAME);
	
	if (plugin != NULL)
		{
			show_hide_widget(GTK_WIDGET(glade_xml_get_widget(plugin->xml, "stat_window")),
							 gtk_check_menu_item_get_active(plugin->cmi_entry));
			if (gtk_check_menu_item_get_active(plugin->cmi_entry) == TRUE)
				{
					plugin->state = PLUGIN_STATE_RUNNING;
					realize_some_numerical_stat(main_struct, plugin);
				}
			else
				plugin->state = PLUGIN_STATE_NONE;
		}

}

/**
 *  The refresh function Called when a new file is loaded or when the cursor is moved
 *  Here we want to refresh the plugin only if a new file is loaded AND if the plugin
 *  is allready displayed (running)
 */
void refresh(heraia_window_t *main_struct, void *data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;

	if (main_struct != NULL && plugin != NULL)
		{
			if (main_struct->event == HERAIA_REFRESH_NEW_FILE && plugin->state == PLUGIN_STATE_RUNNING)
				{
					plugin->run_proc(NULL, (gpointer) main_struct);
				}
		}
}

/* end of the plugin interface functions */

/**
 *  Usefull functions for the stat plugin
 *  those may be included in an another .c source file ?!
 */

/** 
 *  Signals handlers
 */
static gboolean delete_stat_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data )
{
	statw_close_clicked(widget, data);

	return TRUE;
}

static void destroy_stat_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	statw_close_clicked(widget, data);
}

/* What to do when the window is closed */
static void statw_close_clicked(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;

	if (plugin != NULL)
		{
			show_hide_widget(GTK_WIDGET(glade_xml_get_widget(plugin->xml, "stat_window")), FALSE);
			gtk_check_menu_item_set_active(plugin->cmi_entry, FALSE);
		}
}

/**
 *  Connects all the signals to the correct functions
 */
static void stat_window_connect_signals(heraia_plugin_t *plugin)
{
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "stat_window")), "delete_event", 
					 G_CALLBACK(delete_stat_window_event), plugin);

	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "stat_window")), "destroy", 
					 G_CALLBACK(destroy_stat_window), plugin);
	
	/* Close Button */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "statw_close_b")), "clicked", 
					 G_CALLBACK(statw_close_clicked), plugin);

	/* the toogle button is already connected to the run_proc function ! */
}

/**
 *  Do some stats on the selected file (entire file is used)
 */
static void realize_some_numerical_stat(heraia_window_t *main_struct, heraia_plugin_t *plugin)
{
	struct stat *stat_buf;
	gchar buf[42];
	guint i = 0;
	guint j = 0;
	guint nbval1D = 0;
	guint nbval2D = 0;
	stat_t *extra = NULL;
	GtkTextView *textview = GTK_TEXT_VIEW(glade_xml_get_widget(plugin->xml, "statw_textview"));

	if (main_struct->filename != NULL)
		{
			log_message(main_struct, G_LOG_LEVEL_INFO, "Calculating stats on %s", main_struct->filename);

			stat_buf = (struct stat *) g_malloc0 (sizeof(struct stat));
			lstat(main_struct->filename, stat_buf);
			if (S_ISREG(stat_buf->st_mode))
				{
					kill_text_from_textview(textview);
					add_text_to_textview(textview, "Taille du Fichier  : %Ld octets\n", stat_buf->st_size);
					ctime_r(&(stat_buf->st_mtime), buf);
					add_text_to_textview(textview, "Dernière modification interne : %s", buf);
					ctime_r(&(stat_buf->st_atime), buf);
					add_text_to_textview(textview, "Dernier accès au fichier      : %s", buf);
					ctime_r(&(stat_buf->st_ctime), buf);
					add_text_to_textview(textview, "Dernier changement externe    : %s", buf);
					
					populate_stats_histos(main_struct, plugin);

					extra = (stat_t *) plugin->extra;
					for (i=0; i<=255; i++)
						{
							if (extra->histo1D[i] > 0)
								nbval1D++;
							for (j=0; j<=255; j++)
								if (extra->histo2D[i][j] > 0)
									nbval2D++;
						}
					add_text_to_textview(textview, "Nombre d'octets différents : %d\n", nbval1D);
					add_text_to_textview(textview, "Nombre de paires d'octets  : %d\n", nbval2D);

					log_message(main_struct, G_LOG_LEVEL_INFO, "Histos calculated !");
				}		
		}
}

/**
 *  Inits the histograms
 */
static void init_stats_histos(heraia_plugin_t *plugin)
{
	guint i = 0;
	guint j = 0;
	stat_t *extra = NULL;

	extra = (stat_t *) plugin->extra;
	for (i=0; i<=255; i++)
		{
			extra->histo1D[i] = 0 ;
			for (j=0; j<=255; j++)
				extra->histo2D[i][j] = 0 ;
		}

}

/**
 *  Populates the histograms
 */
static void populate_stats_histos(heraia_window_t *main_struct, heraia_plugin_t *plugin)
{
	GtkHex *gh = GTK_HEX(main_struct->current_DW->current_hexwidget);
	guint64 i = 0;
	guint64 taille = ghex_file_size(gh);
	guchar c1, c2;
	stat_t *extra = NULL;

	init_stats_histos(plugin);
	extra = (stat_t *) plugin->extra;

	while (i < taille)
		{
			c1 = gtk_hex_get_byte(gh, i);
			extra->histo1D[c1]++;
			if (i+1 < taille)
				{
					i++;
					c2 = gtk_hex_get_byte(gh, i);
					extra->histo1D[c2]++;
					extra->histo2D[c1][c2]++;
				}
			i++;
		}
}

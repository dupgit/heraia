/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  plugin.h
  heraia - an hexadecimal file editor and analyser based on ghex
 
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

#ifndef _HERAIA_PLUGIN_H_
#define _HERAIA_PLUGIN_H_

/* Based on the gscore plugin interface */

/* API Version to use to verify within the plugin that the interface is what
   expected
*/
#define HERAIA_PLUGIN_API_VERSION 1

/**
 * Plugin types.
 */
typedef enum
{
	HERAIA_PLUGIN_UNKNOWN  = -1,  /* Unknown type   */
	HERAIA_PLUGIN_FILTER = 0,     /* Filter plugin  */
	HERAIA_PLUGIN_ACTION = 1,     /* Action plugin  */

} PluginType;

/**
 *  May indicate the plugin state
 */
typedef enum
{
	PLUGIN_STATE_RUNNING,
	PLUGIN_STATE_INITIALIZING,
	PLUGIN_STATE_LOADED,
	PLUGIN_STATE_NEW,
	PLUGIN_STATE_EXITING,
	PLUGIN_STATE_NONE,
} PluginState;

/* plugins functions */
typedef void (* InitProc)    (heraia_window_t *);   /* Called once when the plugin is loaded              */
typedef void (* QuitProc)    (void);                /* Called once when the plugin is unloaded            */
typedef void (* RunProc)     (GtkWidget *, gpointer);   /* Called via the menu interface                      */
typedef void (* RefreshProc) (heraia_window_t *, void *);   /* Called every time that the cursor position changes */
/* this double structure is here to improve speed avoiding a list search */


/**
 *  Priorities ...
 */
typedef int PluginPriority; 
#define HERAIA_PRIORITY_DEFAULT     0
#define HERAIA_PRIORITY_HIGHEST  9999
#define HERAIA_PRIORITY_LOWEST  -9999


/**
 *  import / export and filters functions this may change quickly
 */

/* returns false on error when loading */
typedef gboolean (* ImportFunction) (const gchar *filename, void *user_data);

/* returns false on error when saving  */
typedef gboolean (* ExportFunction) (const gchar *filename, void *user_data);


/*  this structure may change as the filters may not only be
 *  import / exports functions to load / save files but also
 *  filters (as in signal treatment) to do things on a file.
 */
typedef struct
{
	char *extensions;

	ImportFunction import;
	ExportFunction export;
} plugin_filter_t;


/**
 *  Detailed information about a plugin.
 */
typedef struct
{
	unsigned int api_version;
	PluginType type;
	PluginPriority priority;
	unsigned int id;

	char  *name;
	char  *version;
	char  *summary;
	char  *description;
	char  *author;
	char  *homepage;

} plugin_info_t;

/**
 *  A plugin handle.
 */
typedef struct 
{
	PluginState state;         /* The state of the plugin                  */
	GModule *handle;           /* The module handle                        */
	char *path;                /* The path to the plugin                   */
	char *filename;
	plugin_info_t *info;       /* The plugin information                   */
	plugin_filter_t *filter;   /* The plugin filter                        */
	char *error;               /* last error message                       */
	void *extra;               /* Plugin-specific data                     */

	/* Called when the application initialy starts up */
	InitProc init_proc;
	/* Called when the application exits */
	QuitProc quit_proc;
	/* Called to run an interface everytime the plugin is called */
	RunProc  run_proc;
	/* Called when the cursor changes it's position */
	RefreshProc refresh_proc;

	GtkCheckMenuItem *cmi_entry; /* The CheckMenuItem that may be created in the héraia interface */
	GladeXML *xml;               /* Eventually the plugin Glade XML interface */

} heraia_plugin_t;



/* Usefull plugins related functions provided */
/* to be used within the plugins themselves   */
/* or the main program !                      */
extern gboolean plugin_capable(void);
extern heraia_plugin_t *new_plugin(void);
extern void free_plugin(heraia_plugin_t *plugin);
extern void load_plugins(heraia_window_t *main_window);
extern void add_entry_to_plugins_menu(heraia_window_t *main_window, heraia_plugin_t *plugin);
extern heraia_plugin_t *find_plugin_by_name(GList *plugins_list, gchar *name);
extern gboolean load_plugin_glade_xml(heraia_window_t *main_window, heraia_plugin_t *plugin);
extern void show_hide_widget(GtkWidget *widget, gboolean show);
extern void refresh_all_plugins(heraia_window_t *main_window);

#endif /* _HERAIA_PLUGIN_H_ */

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  heraia.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
 * 
 *  (C) Copyright 2005 - 2007 Olivier Delhomme
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

static void version(void);
static int usage(int status);
static heraia_window_t *heraia_init_main_struct(void);
static HERAIA_ERROR init_heraia_plugin_system(heraia_window_t *main_window);
static void init_heraia_location_list(heraia_window_t *main_window);

static void version(void)
{
	fprintf (stdout, "heraia, %s - %s - Version %s - License %s\n", HERAIA_AUTHORS, HERAIA_DATE, HERAIA_VERSION, HERAIA_LICENSE);
}


static int usage(int status)
{
	if (status == 0)
		{
			fprintf (stderr, 
					 "Try `heraia --help' for more information.\n");
			return FALSE;
		}
	else
		{
			version();
			fprintf(stdout, "\nheraia is a simple hexadecimal file editor and file analyser");
			fprintf(stdout, "\nUsage :\n  heraia [options] filename\n");
			fprintf(stdout, "\nOptions :\n\
  -h, --help\tThis help.\n\
  -v, --version\tProgram version information.\n");
			return TRUE;
		}
}

static heraia_window_t *heraia_init_main_struct(void)
{
	heraia_window_t *herwin;

	herwin = (heraia_window_t *) g_malloc0 (sizeof(heraia_window_t));

	if ( ! herwin )
		return NULL;
	/* First, in this early stage of the development we want to toggle debugging
	   mode ON : 
	*/
	herwin->debug = TRUE;
	herwin->filename = NULL;
	herwin->current_doc = NULL;
	herwin->current_DW = (data_window_t *) g_malloc0 (sizeof(data_window_t));
	herwin->plugins_list = NULL; 
	herwin->location_list = NULL;
	
	init_heraia_location_list(herwin);

	return herwin;
}

/**
 *  Function that initializes the plugin system if any :
 *   - loads any plugin where expected to be found
 *   - inits the plugin window
 */
static HERAIA_ERROR init_heraia_plugin_system(heraia_window_t *main_window)
{

	/* Checking for plugins */
	if (plugin_capable() == TRUE)
		{
			log_message(main_window, G_LOG_LEVEL_INFO, "Enabling plugins");
			load_plugins(main_window);

			/* the plugin_list_window (here the plugins may be loaded !) */
			log_message(main_window, G_LOG_LEVEL_INFO, "Inits the plugin list window");
			plugin_list_window_init_interface(main_window);
			return HERAIA_NOERR;
		}
	else
		{
			log_message(main_window, G_LOG_LEVEL_WARNING, "Plugins will be disabled");
			return HERAIA_NO_PLUGINS;
		}
}

/**
 *  Here we want to init the location list where we might look for
 *  in the future. These can be viewed as default paths
 *  Beware : prepended list in reverse order.
 */
static void init_heraia_location_list(heraia_window_t *main_window)
{
	gchar *path = NULL;
	const gchar* const *system_data_dirs;
	guint i = 0;

	/* heraia's binary path */
	path = g_strdup_printf("%s", g_get_current_dir());
	main_window->location_list = g_list_prepend(main_window->location_list, path);
	
	/* System data dirs */
	system_data_dirs = g_get_system_data_dirs();
	i = 0;
	while(system_data_dirs[i] != NULL)
		{
			path = g_strdup_printf("%s%c%s", system_data_dirs[i], G_DIR_SEPARATOR, "heraia");
			main_window->location_list = g_list_prepend(main_window->location_list, path);
			i++;
		}

	/* System config dirs */
	system_data_dirs = g_get_system_config_dirs();
	i = 0;
	while(system_data_dirs[i] != NULL)
		{
			path = g_strdup_printf("%s%c%s", system_data_dirs[i], G_DIR_SEPARATOR, "heraia");
			main_window->location_list = g_list_prepend(main_window->location_list, path);
			i++;
		}

	/* the user path */
	path =  g_strdup_printf("%s%c.%s", g_get_home_dir(), G_DIR_SEPARATOR, "heraia");
	main_window->location_list = g_list_prepend(main_window->location_list, path);

	/* A global user data path */
	path = g_strdup_printf("%s%c%s", g_get_user_data_dir(), G_DIR_SEPARATOR, "heraia");
	main_window->location_list = g_list_prepend(main_window->location_list, path);

	/* A global config data path */
	path = g_strdup_printf("%s%c%s", g_get_user_config_dir(), G_DIR_SEPARATOR, "heraia");
	main_window->location_list = g_list_prepend(main_window->location_list, path);
}

/**
 *  main program
 */
int main (int argc, char ** argv) 
{  
	Options opt; /* A structure to manage the command line options  */
	int c = 0;
	gboolean exit_value = TRUE;
	heraia_window_t *main_window = NULL;

	opt.filename = NULL;  /* At first we do not have any filename */	
	opt.usage = FALSE;
	
	main_window = heraia_init_main_struct();

	fprintf(stdout, "main_struct initialized !\n");

	while ((c = getopt_long (argc, argv, "vh", long_options, NULL)) != -1)
		{
			switch (c)
				{
				case 0:			/* long options handler */
					break;
	  
				case 'v':
					version();
					break;
	 
				case 'h':
					exit_value = usage(1);
					opt.usage = TRUE;
					break;

				default:
					exit_value = usage(0);
					opt.usage = TRUE;
				}
		}

	if (optind < argc)
		{
			opt.filename = (char*) malloc (sizeof(char) * strlen(argv[optind]) + 1);
			strcpy(opt.filename, argv[optind]);
		}
	else
		{
			if (opt.usage != TRUE)
				{
					exit_value = usage (0);
					opt.usage = TRUE;
				}
		}

	if (opt.usage != TRUE)
		{
			if (main_window->debug == TRUE)
				{
					fprintf(stderr, "Beginning things\n");
				}
		
			/* init of gtk and new window */
			exit_value = gtk_init_check(&argc, &argv);
			
			if (load_heraia_ui(main_window) == TRUE)
				{	
					if (main_window->debug == TRUE)
						{
							log_message(main_window, G_LOG_LEVEL_INFO, "main interface loaded (%s)", main_window->xml->filename);
						}
					
					init_heraia_plugin_system(main_window);

					if (load_file_to_analyse(main_window, opt.filename) == TRUE)
						{								
  						
							log_message(main_window, G_LOG_LEVEL_INFO, "main_window : %p", main_window);

						   	init_heraia_interface(main_window);

							/* gtk main loop */
							gtk_main();
							exit_value = TRUE;
						}
					else
						{
							exit_value = FALSE;
						}
				}
			else
				{
					fprintf(stderr, "File heraia.glade not found !\n");
				}
		}

	return !exit_value; /* Apparently gtk TRUE and FALSE are inverted compared to bash ! */
}

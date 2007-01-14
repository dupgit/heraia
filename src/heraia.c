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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#include "types.h"
#include "heraia.h"
#include "heraia-errors.h"
#include "heraia_ui.h"
#include "io.h"
#include "data_interpretor.h"
/* Remove this in the future (the init function should be called in a 
   manner that it should'nt be necessary to include the header file somewhere */
#include "graphic_analysis.h"
#include "plugin.h"
#include "plugin_list.h"

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
	herwin->current_DW = (data_window_t *) g_malloc0 (sizeof(*herwin->current_DW));
	herwin->ga = NULL; 
	herwin->plugins_list = NULL; 
	herwin->location_list = NULL;
	
	init_heraia_location_list(herwin);

	return herwin;
}

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
 *  possible optimisation : use g_list_prepend and reverse the order
 */
static void init_heraia_location_list(heraia_window_t *main_window)
{
	gchar *path = NULL;

	/* A global path */
	main_window->location_list = g_list_append(main_window->location_list, "/usr/local/share/heraia");

	/* the user path */
	path =  g_strdup_printf("/home/%s/.heraia", getenv("LOGNAME"));
	main_window->location_list = g_list_append(main_window->location_list, path);
	g_free(path);

	/* heraia's binary path */
	path = g_strdup_printf("%s", getcwd(NULL, 0));
	main_window->location_list = g_list_append(main_window->location_list, path);
	g_free(path);

	fprintf(stdout, "init_heraia_location_list Done !\n");
}


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
				fprintf(stderr, "Beginning things\n");
		
			/* init of gtk and new window */
			exit_value = gtk_init_check (&argc, &argv);
			
			if (load_heraia_ui(main_window) == TRUE)
				{	
					if (main_window->debug == TRUE)
						log_message(main_window, G_LOG_LEVEL_INFO, "main interface loaded");

					init_heraia_plugin_system(main_window);

					if (load_file_to_analyse(main_window, opt.filename) == TRUE)
						{	
							/* inits the data interpretor window */
							data_interpret(main_window->current_DW);

  							/* Connection of the signal to the right function
							   in order to interpret things when the cursor is
							   moving                                          */
							g_signal_connect (G_OBJECT (main_window->current_DW->current_hexwidget), "cursor_moved",
											  G_CALLBACK (refresh_event_handler), main_window);

							log_message(main_window, G_LOG_LEVEL_INFO, "main_window : %p", main_window);
						   	init_heraia_interface(main_window);
							
							/* Do some init plugin calls here */
							init_graph_analysis(main_window);

							/* Shows all widgets */
							gtk_widget_show_all(glade_xml_get_widget(main_window->xml, "main_window"));
					
							/* gtk main loop */
							gtk_main();
							exit_value = TRUE;
						}
					else
						exit_value = FALSE;
				}
			else
				fprintf(stderr, "File heraia.glade not found !\n");
		}

	return !exit_value; /* Apparently gtk TRUE and FALSE are inverted compared to bash ! */
}

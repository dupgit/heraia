/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia.c
  heraia - an hexadecimal file editor and analyser based on ghex
 
  (C) Copyright 2005 Olivier Delhomme
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include <gtk/gtk.h>
#include <gtkhex/gtkhex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "data_interpretor.h"
#include "heraia.h"
#include "heraia_ui.h"
#include "io.h"


static void version()
{
	fprintf (stdout, "heraia, %s - %s - Version %s - License %s\n", HERAIA_AUTHORS,
			 HERAIA_DATE, HERAIA_VERSION, HERAIA_LICENSE);
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
			fprintf(stdout, "\nheraia is a simple hexadecimal file editor and \
file analyser");
			fprintf(stdout, "\nUsage :\n\
  heraia [options] filename\n");
			fprintf(stdout, "\nOptions :\n\
  -h, --help\tThis help.\n\
  -v, --version\tProgram version information.\n");
			return TRUE;
		}
}

static gboolean delete_main_window_event( GtkWidget *widget, GdkEvent  *event,
										  gpointer   data )
{
	gtk_widget_destroy (widget);
    return TRUE;
}

/*
static void destroy_main_window( GtkWidget *widget,
								 gpointer   data )
{
    gtk_main_quit ();
}
*/

int main (int argc, char ** argv) 
{  
	Options opt; /* A structure to manage the command line options  */
	int c = 0;
	gboolean exit_value = TRUE;
	heraia_window_t *main_window;

	opt.filename = NULL;  /* At first we do not have any filename */	
	opt.usage = FALSE;
	
	main_window = (heraia_window_t *) g_malloc0 (sizeof(*main_window));
	/* First, in this early stage of the development we want to toggle debugging
	   mode ON : 
	*/
	main_window->debug = TRUE;
	main_window->filename = NULL;
	main_window->current_doc = NULL;
	main_window->current_DW = (data_window_t *) g_malloc0 (sizeof(*main_window->current_DW));

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
				g_print("Beginning things\n");
			/* init of gtk and new window */
			exit_value = gtk_init_check (&argc, &argv);
	   
			if (load_heraia_ui(main_window) == TRUE)
				{
					if (main_window->debug == TRUE)
						g_print("main_interface_loaded!\n");
					/* here we connect the signals (some from heraia_ui.h) */
					g_signal_connect (G_OBJECT (main_window->window), "delete_event", 
									  G_CALLBACK (delete_main_window_event), NULL);
					g_signal_connect (G_OBJECT (main_window->window), "destroy", 
									  G_CALLBACK (on_quitter1_activate), NULL);

					if (load_file_to_analyse(main_window, opt.filename) == TRUE)
						{	
  
							data_interpret (main_window->current_DW);
  
							/* Connection of the signal to the right function
							   in order to interpret things when the cursor is
							   moving                                          */
							g_signal_connect (G_OBJECT (main_window->current_DW->current_hexwidget), "cursor_moved",
											  G_CALLBACK (refresh_data_window), main_window->current_DW);
							
							g_print("main_window : %p\n", main_window);
							init_heraia_interface(main_window);

							gtk_widget_show_all (main_window->window);
					
							gtk_main ();
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

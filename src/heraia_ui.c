/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_ui.c
  main menus, callback and utility functions
  
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "heraia_ui.h"
#include "io.h"


static gboolean load_heraia_glade_xml(heraia_window_t *main_window);
static gboolean load_the_glade_xml_if_it_exists(heraia_window_t *main_window, char *file_to_load);


/* call back functions : for the main program */
void on_quitter1_activate( GtkWidget *widget, gpointer data )
{
	gtk_main_quit();
}

void on_nouveau1_activate( GtkWidget *widget, gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

void on_fonte1_activate( GtkWidget *widget, gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

void on_supprimer1_activate( GtkWidget *widget, gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

void on_a_propos1_activate( GtkWidget *widget, gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

void on_couper1_activate( GtkWidget *widget, gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

void on_copier1_activate( GtkWidget *widget, gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

void on_coller1_activate( GtkWidget *widget, gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

void on_ouvrir1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	if (select_file_to_load(main_window) == TRUE)
		{
			load_file_to_analyse(main_window, main_window->filename);
		}

	refresh_data_window(main_window->current_DW->current_hexwidget, main_window->current_DW);

	/* Connection of the signal to the right function
	   in order to interpret things when the cursor is
	   moving                                           */
	g_signal_connect (G_OBJECT (main_window->current_DW->current_hexwidget), "cursor_moved",
					  G_CALLBACK (refresh_data_window), main_window->current_DW); 
}

void on_enregistrer1_activate( GtkWidget *widget,  gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

void on_enregistrer_sous1_activate( GtkWidget *widget, gpointer data )
{
	g_print("Not implemented Yet (Please contribute !)\n");
}

/* this handles the menuitem "Data Interpretor" that
   show or hide the data interpretor window           */
void on_DIMenu_activate (GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	data_window_t *DW = main_window->current_DW;

	DW->window_displayed = !(DW->window_displayed);
	
	if (DW->window_displayed == TRUE)
		gtk_widget_show_all(DW->window);
	else
		gtk_widget_hide_all(DW->window);

	refresh_data_window(DW->current_hexwidget, DW);
}
/* end for call back functions for the main program */



/* call back functions :  for the data interpretor window */
gboolean delete_dt_window_event( GtkWidget *widget, GdkEvent  *event, gpointer data )
{	
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name (glade_xml_get_widget(main_window->xml, "DIMenu"), "activate");

	return TRUE;
}

void destroy_dt_window( GtkWidget *widget, GdkEvent  *event, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name (glade_xml_get_widget(main_window->xml, "DIMenu"), "activate");
}
/* End of call back functions that handle the data interpretor window */


/* 
   This function does open a file selector dialog box and returns the selected 
   filename. 
   We do fill the main_window->filename parameter here !
*/
gboolean select_file_to_load(heraia_window_t *main_window)
{
	GtkFileSelection *file_selector;
	gboolean success = FALSE;
	gint response_id = 0;
	const gchar *filename = NULL;

	file_selector = GTK_FILE_SELECTION (gtk_file_selection_new ("Please select a file for editing."));

	/* for the moment we do not want to retrieve multiples selections */
	
	gtk_file_selection_set_select_multiple(file_selector, FALSE);
	response_id = gtk_dialog_run(GTK_DIALOG (file_selector));
	
	switch (response_id) 
		{
		case GTK_RESPONSE_OK:
			filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (file_selector));
			if (main_window->debug == TRUE)
				g_print("filename selected : %s\n", filename);
			/* this should be managed with lists */		
			/* g_free(main_window->filename); */

			main_window->filename = (gchar *) g_malloc0 (sizeof(gchar) * (strlen(filename)+2));
			if (main_window->debug == TRUE)
				g_print("main_window->filename = gchar * g_malloc0 (%d) : Ok\n", sizeof(gchar) * (strlen(filename)+2));
			sprintf(main_window->filename, "%s", filename);
			
			success = TRUE;
			break;
		case GTK_RESPONSE_CANCEL:
		default:
			success = FALSE;
			break;
		}

	gtk_widget_destroy (GTK_WIDGET(file_selector));

	return success;
}


/* here we might init some call backs and menu options 
   This function should be called once only            */
void init_heraia_interface(heraia_window_t *main_window)
{
   	data_window_t *DW = main_window->current_DW;
   
	g_signal_connect (G_OBJECT (DW->window), "delete_event", 
					  G_CALLBACK (delete_dt_window_event), main_window);

	g_signal_connect (G_OBJECT (DW->window), "destroy", 
					  G_CALLBACK (destroy_dt_window), main_window);

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(main_window->xml, "DIMenu")), DW->window_displayed); 	
  
}

/* checks if file_to_load exists and is valid and if possible, loads it */
static gboolean load_the_glade_xml_if_it_exists(heraia_window_t *main_window, char *file_to_load)
{
	gboolean success = FALSE;
	struct stat *stat_buf;

	stat_buf = (struct stat *) g_malloc0 (sizeof(struct stat));

	stat(file_to_load, stat_buf);
	if (S_ISREG(stat_buf->st_mode) && stat_buf->st_size>0)
		{
			success = TRUE;
			main_window->xml = glade_xml_new(file_to_load, NULL, NULL);
			if (main_window->xml == NULL)
				success = FALSE;
		}

	g_free(stat_buf);

	return success;
}

/* loads the glade xml file that describes the heraia project
   tries the following paths in that order :                 
   - /etc/heraia/heraia.glade
   - /home/[user]/.heraia/heraia.glade
   - PWD/heraia.glade
 */
static gboolean load_heraia_glade_xml(heraia_window_t *main_window)
{
	
	char *current_path = NULL;
	char *filename = NULL;
	char *file_to_load = NULL;
	char *user = NULL;
	gboolean success = FALSE;
   
	filename = (char *) g_malloc0 (sizeof(char) * 13 );	
	sprintf(filename, "%s", "heraia.glade");
   
	/* first we might see if there is a /etc/heraia/heraia.glade file */
	file_to_load = (char *) g_malloc0 (sizeof(char)*(12 + strlen(filename) + 1));
	sprintf(file_to_load, "/etc/heraia/%s", filename);
	success = load_the_glade_xml_if_it_exists(main_window, file_to_load);
	if (main_window->debug == TRUE && success == TRUE)
		g_print("heraia.glade loaded from : %s\n", file_to_load );
	
	g_free(file_to_load);

	/* if there is not we want to see if /home/[user]/.heraia/heraia.glade exists */
	if (success == FALSE)
		{
			/*user = (char *) g_malloc0 (sizeof(char)*255);*/
			user = getenv("LOGNAME");
			file_to_load = (char *) g_malloc0 (sizeof(char)*(strlen(user) + strlen(filename) + 15));
			sprintf(file_to_load, "/home/%s/.heraia/%s", user, filename);
			success = load_the_glade_xml_if_it_exists(main_window, file_to_load);
			if (main_window->debug == TRUE && success == TRUE)
				g_print("heraia.glade loaded from : %s\n", file_to_load );

			g_free(file_to_load);
		}

	/* if nothing exists we default into the current directory */
	if (success == FALSE)
		{
			current_path = (char *) g_malloc0 (sizeof(char)*255);
			getcwd(current_path, 255);
			/* +2 : the final '\0' and the middle '/' */
			file_to_load = (char *) g_malloc0 (sizeof(char)*(strlen(filename) + strlen(current_path) + 2));
			sprintf(file_to_load, "%s/%s", current_path, filename);
			success = load_the_glade_xml_if_it_exists(main_window, file_to_load);
			if (main_window->debug == TRUE && success == TRUE)
				g_print("heraia.glade loaded from : %s\n", file_to_load );

			g_free(current_path);
			g_free(file_to_load);
		}

	g_free(filename);
	
	return success;
}

void on_analyse_graphique_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(main_window->xml, "analyse_graphique"))))
		gtk_widget_show_all(glade_xml_get_widget(main_window->xml, "graph_analysis"));
	else
		gtk_widget_hide_all(glade_xml_get_widget(main_window->xml, "graph_analysis"));
}


int load_heraia_ui(heraia_window_t *main_window)
{
	gboolean success = FALSE;

	/* load the interface */
	success = load_heraia_glade_xml(main_window);

	if (success == TRUE)
		{
			/* connect the signals in the interface */

			g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "DIMenu")), "activate", 
							  G_CALLBACK (on_DIMenu_activate), main_window);

			g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "quitter1")), "activate", 
							  G_CALLBACK (on_quitter1_activate), main_window);

			g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "nouveau1")), "activate",  
							  G_CALLBACK (on_nouveau1_activate), main_window);

			g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "ouvrir1")), "activate",  
							  G_CALLBACK (on_ouvrir1_activate), main_window);

			g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "enregistrer1")), "activate",  
							  G_CALLBACK (on_enregistrer1_activate), main_window);

			g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "enregistrer_sous1")), "activate",  
							  G_CALLBACK (on_enregistrer_sous1_activate), main_window); 

			g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "analyse_graphique")), "activate",
							  G_CALLBACK (on_analyse_graphique_activate), main_window);

		}

	return success;
}

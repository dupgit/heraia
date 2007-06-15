/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_ui.c
  main menus, callback and utility functions
  
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

#include "types.h"

static gboolean load_heraia_glade_xml(heraia_window_t *main_window);
static void heraia_ui_connect_signals(heraia_window_t *main_window);
/* static gboolean load_the_glade_xml_if_it_exists(heraia_window_t *main_window, char *file_to_load);*/

/* call back functions : for the main program */
void on_quitter1_activate( GtkWidget *widget, gpointer data )
{
	gtk_main_quit();
}

void on_nouveau1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

void on_fonte1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

void on_supprimer1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

void on_a_propos1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	gtk_widget_show(glade_xml_get_widget(main_window->xml, "about_dialog"));
}

/**
 *  To close the A propos dialog box (with the "close" button)
 */
static void a_propos_response(GtkWidget *widget, gint response, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
 	
	gtk_widget_hide(glade_xml_get_widget(main_window->xml, "about_dialog"));
}

static void a_propos_close(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	gtk_widget_hide(glade_xml_get_widget(main_window->xml, "about_dialog"));
}

static gboolean a_propos_delete(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
 	
	gtk_widget_hide(glade_xml_get_widget(main_window->xml, "about_dialog"));
 	
	return TRUE;
}

void on_couper1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

void on_copier1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

void on_coller1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}


/** This function is here to ensure that everything will be
 *  refreshed upon a signal event.
 */
void refresh_event_handler(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	if (main_window != NULL)
		{
			/* Beware, this mechanism is not thread safe ! */
			if (main_window->event == HERAIA_REFRESH_NOTHING)
				main_window->event = HERAIA_REFRESH_CURSOR_MOVE;

			refresh_data_window(widget, main_window->current_DW);
			refresh_all_plugins(main_window);

			main_window->event = HERAIA_REFRESH_NOTHING;
		}
}


/* This handles the menuitem "Ouvrir" to open a file */
void on_ouvrir1_activate(GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	if (select_file_to_load(main_window) == TRUE)
		{
			load_file_to_analyse(main_window, main_window->filename);
		}

	main_window->event = HERAIA_REFRESH_NEW_FILE;
	refresh_event_handler(main_window->current_DW->current_hexwidget, main_window);

	/* Connection of the signal to the right function
	   in order to interpret things when the cursor is
	   moving                                           */
	g_signal_connect (G_OBJECT (main_window->current_DW->current_hexwidget), "cursor_moved",
					  G_CALLBACK (refresh_event_handler), main_window); 
}

void on_enregistrer1_activate( GtkWidget *widget,  gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

void on_enregistrer_sous1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
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

gboolean delete_main_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	gtk_widget_destroy(widget);
    return TRUE;
}


/*
void destroy_main_window(GtkWidget *widget, gpointer   data)
{
    gtk_main_quit ();
}
*/
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
	GtkFileSelection *file_selector = NULL;
	gboolean success = FALSE;
	gint response_id = 0;
	const gchar *filename = NULL;
	gchar *path = NULL;
	GError *err = NULL;

	file_selector = GTK_FILE_SELECTION (gtk_file_selection_new ("Sélectionnez un fichier pour l'éditer"));

	/* for the moment we do not want to retrieve multiples selections */
	gtk_file_selection_set_select_multiple(file_selector, FALSE);
	/*  We want the file selection path to be the on of the previous
	 *	openned file if any !
     */
	if (main_window->filename != NULL)
		{
			path = g_filename_from_utf8(g_locale_to_utf8(g_strdup_printf("%s%c", g_path_get_dirname(main_window->filename), G_DIR_SEPARATOR), -1, NULL, NULL, &err), -1, NULL, NULL, &err);
			gtk_file_selection_set_filename(file_selector, path);
		}

	response_id = gtk_dialog_run(GTK_DIALOG (file_selector));
	
	switch (response_id) 
		{
		case GTK_RESPONSE_OK:
			filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (file_selector));
			if (main_window->debug == TRUE)
				log_message(main_window, G_LOG_LEVEL_INFO, "filename selected : %s", filename);
			/* this should be managed with lists */		
			if (main_window->filename != NULL)
				g_free(main_window->filename);
			main_window->filename = g_strdup_printf("%s", filename);			
			success = TRUE;
			break;
		case GTK_RESPONSE_CANCEL:
		default:
			success = FALSE;
			break;
		}

	gtk_widget_destroy (GTK_WIDGET(file_selector));
	g_free(path);

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


/* checks if file_to_load exists and is valid and if possible, loads it
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
} */


/* loads the glade xml file that describes the heraia project
   tries the following paths in that order :                 
   - /etc/heraia/heraia.glade
   - /home/[user]/.heraia/heraia.glade
   - PWD/heraia.glade
 */
static gboolean load_heraia_glade_xml(heraia_window_t *main_window)
{
	gchar *filename = NULL;

	filename = g_strdup_printf("heraia.glade");

	main_window->xml = load_glade_xml_file(main_window->location_list, filename);

	g_free(filename);

	if (main_window->xml == NULL)
		return FALSE;
	else
		return TRUE;
}


/**
 *  Connect the signals at the interface 
 */
static void heraia_ui_connect_signals(heraia_window_t *main_window)
{

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
		
	/* about dialog box */		
	g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "a_propos1")), "activate",  
					  G_CALLBACK (on_a_propos1_activate), main_window); 
	g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "about_dialog")), "close",
					 G_CALLBACK(a_propos_close), main_window);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "about_dialog")), "response",
					 G_CALLBACK(a_propos_response), main_window);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "about_dialog")), "delete-event",
					 G_CALLBACK(a_propos_delete), main_window);

	g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "main_window")), "delete_event", 
					  G_CALLBACK (delete_main_window_event), NULL);

	g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "main_window")), "destroy", 
					  G_CALLBACK (on_quitter1_activate), NULL);
			
}

/**
 *  Loads, if possible, the glade xml file and then connects the
 *  signals and inits the log window
 */
int load_heraia_ui(heraia_window_t *main_window)
{
	gboolean success = FALSE;

	/* load the interface */
	success = load_heraia_glade_xml(main_window);

	if (success == TRUE)
		{
			heraia_ui_connect_signals(main_window);

			/* The Log window */
			log_window_init_interface(main_window);

		}

	return success;
}


/**
 *  adds a text to a textview
 */
void add_text_to_textview(GtkTextView *textview, const char *format, ...)
{	
	va_list args;
	GtkTextBuffer *tb = NULL;
	GtkTextIter iEnd;
	gchar *display = NULL;
	GError *err = NULL;

	va_start(args, format);
	display = g_locale_to_utf8(g_strdup_vprintf(format, args), -1, NULL, NULL, &err);
	va_end(args);

	tb = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)));
	gtk_text_buffer_get_end_iter(tb, &iEnd);
	gtk_text_buffer_insert(tb, &iEnd, display, -1);
	g_free(display);
}


/**
 *  Kills the text from a textview
 */
void kill_text_from_textview(GtkTextView *textview)
{
	GtkTextBuffer *tb = NULL;
	GtkTextIter iStart;
	GtkTextIter iEnd;

	tb = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)));
	gtk_text_buffer_get_start_iter(tb, &iStart);
	gtk_text_buffer_get_end_iter(tb, &iEnd);
	gtk_text_buffer_delete (tb, &iStart, &iEnd);
}

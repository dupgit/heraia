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

#include "heraia_types.h"

static gboolean load_heraia_glade_xml(heraia_window_t *main_window);
static void heraia_ui_connect_signals(heraia_window_t *main_window);

/**
 *  Quit, file menu
 */
void on_quitter1_activate( GtkWidget *widget, gpointer data )
{
	gtk_main_quit();
}

/**
 *  New, file menu
 */
void on_nouveau1_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

/**
 *  Shows apropos's dialog box
 */
void a_propos_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *about_dialog = NULL;
	
	about_dialog = heraia_get_widget(main_window->xmls->main, "about_dialog");
	
	if (about_dialog != NULL)
		{				
			gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), PACKAGE_NAME);
			gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), PACKAGE_VERSION);
			gtk_widget_show(about_dialog);
		}
}

/**
 *  To close the A propos dialog box (with the "close" button)
 */
static void a_propos_response(GtkWidget *widget, gint response, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
 	
	gtk_widget_hide(heraia_get_widget(main_window->xmls->main, "about_dialog"));
}

static void a_propos_close(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	gtk_widget_hide(heraia_get_widget(main_window->xmls->main, "about_dialog"));
}

static gboolean a_propos_delete(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
 	
	gtk_widget_hide(heraia_get_widget(main_window->xmls->main, "about_dialog"));
 	
	return TRUE;
}


/**
 *  Delete, edit menu
 */
void on_supprimer1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

/**
 *  Cut, edit menu
 */
void on_couper1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

/**
 *  Copy, edit menu
 */
void on_copier1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}


/**
 *  Paste, edit menu
 */
void on_coller1_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}


/** 
 *  This function is here to ensure that everything will be
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

			refresh_data_interpretor_window(widget, main_window);
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

/**
 *  This handles the menuitem "Data Interpretor" that
 *  shows or hides the data interpretor window           
 */
void on_DIMenu_activate(GtkWidget *widget, gpointer data)
{

	heraia_window_t *main_window = (heraia_window_t *) data;
	data_window_t *dw = NULL;      /* program structure           */
	GtkNotebook *notebook = NULL;  /* data interpretor's notebook */

	if (main_window != NULL)
		{
			dw = main_window->current_DW;
     
			if (dw != NULL)
				{
					if (dw->diw == NULL)
						{
							dw->diw = heraia_get_widget(main_window->xmls->main, "data_interpretor_window");
						}
	  
					if (dw->diw != NULL)
						{
							dw->window_displayed = !(dw->window_displayed);
							notebook = GTK_NOTEBOOK(heraia_get_widget(main_window->xmls->main, "diw_notebook"));

							if (dw->window_displayed == TRUE)
								{
									gtk_widget_show_all(dw->diw);

									/* Setting the first page of the notebook as default (Numbers) */					
									gtk_notebook_set_current_page(notebook, dw->tab_displayed);

									refresh_data_interpretor_window(widget, data);
								}
							else
								{
									dw->tab_displayed = gtk_notebook_get_current_page(notebook);
									gtk_widget_hide_all(dw->diw);
								}
						}
				}
		}
}

/**
 *  When the user destroys or delete the main window
 */
gboolean delete_main_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	gtk_widget_destroy(widget);
    return TRUE;
}


/**
 *  call back functions for the data interpretor window destruction
 */
gboolean delete_dt_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{	
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name(heraia_get_widget(main_window->xmls->main, "DIMenu"), "activate");

	return TRUE;
}

void destroy_dt_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name(heraia_get_widget(main_window->xmls->main, "DIMenu"), "activate");
}
/* End of call back functions that handle the data interpretor window */


/**
 *  Returns an absolute path to the filename
 *  the string should be freed when no longer needed
 *  very UGLy !
 */
static gchar *make_absolute_path(gchar *filename)
{
	gchar *current_dir = NULL;
	gchar *new_dir = NULL;

	if (g_path_is_absolute(filename) == TRUE)
		{
			/* if the filename is already in an absolute format */
			return  g_path_get_dirname(filename);
		}
	else
		{
			current_dir = g_get_current_dir();
			new_dir = g_path_get_dirname(filename);

			if (g_chdir(new_dir) == 0)
				{
					g_free(new_dir);
					new_dir = g_get_current_dir();
					g_chdir(current_dir);
					g_free(current_dir);

					return new_dir;
				}
			else
				{
					g_free(current_dir);

					return NULL;
				}
		}
}


/**
 *  Sets the working directory for the file chooser to the directory of the
 *  filename (even if filename is a relative filename such as 
 *  ../docs/test_file)
 */
static void set_the_working_directory(GtkFileChooser *file_chooser, gchar *filename)
{
	gchar *dirname = NULL;    /* directory where we want to be, at first, in the file chooser */

	dirname = make_absolute_path(filename);

	if (dirname != NULL)
		{
			gtk_file_chooser_set_current_folder(file_chooser, dirname);
			g_free(dirname);
		}
}
 

/**
 *  This function does open a file selector dialog box and returns the selected 
 *  filename. 
 *  We do fill the main_window->filename parameter here !
 */
gboolean select_file_to_load(heraia_window_t *main_window)
{
	GtkWidget *parent = NULL; /* A parent window (we use main_window)                         */
	GtkFileChooser *file_chooser = NULL;
	gboolean success = FALSE;
	gchar *filename = NULL;   /* filename selected (if any) to be openned                     */

	parent = heraia_get_widget(main_window->xmls->main, "main_window");

	file_chooser = GTK_FILE_CHOOSER(gtk_file_chooser_dialog_new("Select a file to analyse",
																GTK_WINDOW(parent),
																GTK_FILE_CHOOSER_ACTION_OPEN, 
																GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
																GTK_STOCK_OPEN, GTK_RESPONSE_OK, 
																NULL));

	/**
     *  for the moment we do not want to retrieve multiples selections 
	 *  but this could be a valuable thing in the future
	 */
	gtk_window_set_modal(GTK_WINDOW(file_chooser), TRUE);
	gtk_file_chooser_set_select_multiple(file_chooser, FALSE);

	/**
	 *  We want the file selection path to be the one of the previous
	 *  openned file if any !
	 */
	if (main_window->filename != NULL)
		{
			set_the_working_directory(file_chooser, main_window->filename);
		}
	
	switch (gtk_dialog_run(GTK_DIALOG(file_chooser))) 
		{
		case GTK_RESPONSE_OK:
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
			log_message(main_window, G_LOG_LEVEL_DEBUG, "filename selected : %s", filename);

			/* this should be managed with lists */		
			if (main_window->filename != NULL)
				{
					g_free(main_window->filename);
				}

			main_window->filename = g_strdup_printf("%s", filename);
			success = TRUE;
			break;
		case GTK_RESPONSE_CANCEL:
		default:
			success = FALSE;
			break;
		}
	
	g_free(filename);
	gtk_widget_destroy(GTK_WIDGET(file_chooser));
	
	return success;
}


/**
 *  Here we might init some call backs and menu options
 *  and display the interface (main && sub-windows)
 *  This function should be called only once at main program's 
 *  init time
 */
void init_heraia_interface(heraia_window_t *main_window)
{
	data_window_t *dw = NULL; /* data interpretor structure */
	GtkWidget *diw = NULL;    /* data interpretor window    */
	GtkWidget *menu = NULL;   /* the DIMenu diplay option   */
	GtkWidget *window = NULL; /* the main window widget     */ 
  
	if (main_window != NULL)
		{
			dw = main_window->current_DW;

			menu = heraia_get_widget(main_window->xmls->main, "DIMenu");
			window = heraia_get_widget(main_window->xmls->main, "main_window");

			if (dw != NULL && diw != NULL)
				{
					/*  Connection of the signal to the right function
					 *  in order to interpret things when the cursor is
					 *  moving                                          
					 */
					connect_cursor_moved_signal(main_window);

					gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu), dw->window_displayed);     
	  
					/* Shows all widgets */
					gtk_widget_show_all(window);

					/* Shows or hide the data interpretor window */
					if (dw->window_displayed == TRUE)
						{
							gtk_widget_show_all(diw);
						}
					else
						{
							gtk_widget_hide_all(diw);
						}
				}
		}
}


/**
 *  Loads the glade xml files that describes the heraia project
 *  tries the following paths in that order :                 
 *  - /etc/heraia/heraia.glade
 *  - /home/[user]/.heraia/heraia.glade
 *  - PWD/heraia.glade
 */
static gboolean load_heraia_glade_xml(heraia_window_t *main_window)
{
	gchar *filename = NULL;

	if (main_window != NULL && main_window->xmls != NULL)
		{

			filename = g_strdup_printf("heraia.glade"); 
			main_window->xmls->main = load_glade_xml_file(main_window->location_list, filename);
			g_free(filename);

			/**
			 *  filename = g_strdup_printf("treatment.glade");
			 *  main_window->xmls->treatment = load_glade_xml_file(main_window->location_list, filename);
			 *  g_free(filename);
			 */

			if (main_window->xmls->main == NULL) /* || main_window->xmls->treatment == NULL) */
				return FALSE;
			else
				return TRUE;
		}
	else
		return FALSE;
}

/**
 *  Connects the signal that the cursor has moved to
 *  the refreshing function
 */
void connect_cursor_moved_signal(heraia_window_t *main_window)
{
	g_signal_connect(G_OBJECT(main_window->current_DW->current_hexwidget), "cursor_moved",   
						  G_CALLBACK(refresh_event_handler), main_window);
}


/**
 *  Connect the signals at the interface 
 */
static void heraia_ui_connect_signals(heraia_window_t *main_window)
{

	/* the data interpretor menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "DIMenu")), "activate", 
					  G_CALLBACK (on_DIMenu_activate), main_window);

	/* Quit, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "quitter1")), "activate", 
					  G_CALLBACK (on_quitter1_activate), main_window);

	/* New, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "nouveau1")), "activate",  
					  G_CALLBACK (on_nouveau1_activate), main_window);

	/* Open, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "ouvrir1")), "activate",  
					  G_CALLBACK (on_ouvrir1_activate), main_window);

	/* Save, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "enregistrer1")), "activate",  
					  G_CALLBACK (on_enregistrer1_activate), main_window);

	/* Save As, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "enregistrer_sous1")), "activate",  
					  G_CALLBACK (on_enregistrer_sous1_activate), main_window); 
	
	/* Cut, edit menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "couper1")), "activate",  
					  G_CALLBACK (on_couper1_activate), main_window); 

	/* Copy, edit menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "copier1")), "activate",  
					  G_CALLBACK (on_copier1_activate), main_window); 

	/* Paste, edit menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "coller1")), "activate",  
					  G_CALLBACK (on_coller1_activate), main_window); 


	/* about dialog box */		
	g_signal_connect (G_OBJECT(heraia_get_widget(main_window->xmls->main, "a_propos1")), "activate",  
					  G_CALLBACK(a_propos_activate), main_window); 

	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "about_dialog")), "close",
					 G_CALLBACK(a_propos_close), main_window);

	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "about_dialog")), "response",
					 G_CALLBACK(a_propos_response), main_window);

	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "about_dialog")), "delete-event",
					 G_CALLBACK(a_propos_delete), main_window);


	/* main window killed or destroyed */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "main_window")), "delete_event", 
					  G_CALLBACK (delete_main_window_event), NULL);

	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "main_window")), "destroy", 
					  G_CALLBACK (on_quitter1_activate), NULL);
			
}

/**
 *  Loads, if possible, the glade xml file and then connects the
 *  signals and inits the following windows :
 *  - log window
 *  - data_interpretor window
 *  - list data types
 */
int load_heraia_ui(heraia_window_t *main_window)
{
	gboolean success = FALSE;

	/* load the XML interfaces (main & treatment) */
	success = load_heraia_glade_xml(main_window);

	if (success == TRUE)
		{
			heraia_ui_connect_signals(main_window);

			/* The Log window */
			log_window_init_interface(main_window);

			/* The data interpretor window */
			data_interpretor_init_interface(main_window);

			/* The list data types window */
			list_data_types_init_interface(main_window);

			/* The data type window (create or edit one type) */
			data_type_init_interface(main_window);
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



/**
 *  Try to find the active radio button widget in a group
 *  This does not take into account inconsistant states
 *  returns the first active radio button otherwise NULL
 */
GtkWidget *gtk_radio_button_get_active(GSList *group)
{
	GSList *tmp_slist = group;
  
	while (tmp_slist)
		{
			if (GTK_TOGGLE_BUTTON (tmp_slist->data)->active)
				{
					return GTK_WIDGET (tmp_slist->data);
				}
			tmp_slist = tmp_slist->next;
		}

	return NULL;
}

/**
 * gtk_radio_button_get_active_from_widget:
 * @radio_group_member: widget to get radio group from
 * 
 * @returns: the active #GtkRadioButton within the group from
 *           @radio_group_member
 **/
GtkWidget *gtk_radio_button_get_active_from_widget(GtkRadioButton *radio_group_member)
{ 
	if (radio_group_member)
		{
			return gtk_radio_button_get_active(radio_group_member->group);
		}
	else
		{
			return NULL;
		}
}

/**
 *  Tells whether a GtkCheckMenuItem is Checked or not
 */
gboolean is_cmi_checked(GtkWidget *check_menu_item)
{
	return gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(check_menu_item));
}

/**
 *  This is a wrapper to the glade xml get widget. It is intended
 *  to simplify the developpers lives if they have to choose or
 *  propose other means to do the same thing than libglade (say,
 *  for example, GtkBuilder :)
 */
GtkWidget *heraia_get_widget(GladeXML *xml, gchar *widget_name)
{
	if (xml != NULL && widget_name != NULL)
		{
			return glade_xml_get_widget(xml, widget_name);
		}
	else
		{
			return NULL;
		}
}


/**
 *  Destroys a single widget if it exists 
 */
void destroy_a_single_widget(GtkWidget *widget)
{
	if (widget != NULL)
		{
			gtk_widget_destroy(widget);
		}
}

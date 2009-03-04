/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_ui.c
  main menus, callback and utility functions
 
  (C) Copyright 2005 - 2009 Olivier Delhomme
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
/** @file heraia_ui.c
 * This file has all the functions to manage heraia's ui
 * - signals definitions and functions
 * - widgets activations
 * - closing / openning windows
 */

#include <libheraia.h>

static void set_a_propos_properties(GtkWidget *about_dialog);
static gboolean load_heraia_glade_xml(heraia_window_t *main_window);
static void heraia_ui_connect_signals(heraia_window_t *main_window);
static void record_and_hide_about_box(heraia_window_t *main_window);
static void close_heraia(heraia_window_t *main_window);

/**
 * @fn void on_quit_activate(GtkWidget *widget, gpointer data)
 *  Quit, file menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_quit_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	close_heraia(main_window);
	gtk_main_quit();
}

/**
 * @fn void on_new_activate(GtkWidget *widget, gpointer data)
 *  New, file menu
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_new_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

/**
 * @fn void on_preferences_activate(GtkWidget *widget, gpointer data)
 *  Preferences, file menu :
 *  Displays the preference window (as a modal window)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_preferences_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *pref_window = NULL;
	
	pref_window = heraia_get_widget(main_window->xmls->main, "main_preferences_window");
	
	if (pref_window != NULL)
	{
		move_and_show_dialog_box(pref_window, main_window->win_prop->main_pref_window);
	}
	
}

/**
 * @fn void set_a_propos_properties(GtkWidget *about_dialog)
 * Sets name and version in the dialog box
 * @param about_dialog the widget that contain all the about box
 */
static void set_a_propos_properties(GtkWidget *about_dialog)
{
	
	if (about_dialog != NULL)
	{
		if (GTK_MINOR_VERSION >= 12)
		{
		    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), PACKAGE_NAME);
		}
		if (GTK_MINOR_VERSION >= 6)
		{
			gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), PACKAGE_VERSION);
		}
	}
}


/**
 * @fn void a_propos_activate(GtkWidget *widget, gpointer data)
 *  Shows apropos's dialog box
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void a_propos_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *about_dialog = NULL;
	
	about_dialog = heraia_get_widget(main_window->xmls->main, "about_dialog");
	
	if (about_dialog != NULL)
	{
		set_a_propos_properties(about_dialog);
	
		move_and_show_dialog_box(about_dialog, main_window->win_prop->about_box);
	}
}

/** 
 * @fn void move_and_show_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_prop)
 *  Move the dialog box to the wanted position, shows it and says it in the displayed prop
 * @param dialog_box : the dialog box we want to move and show
 * @param dialog_prop : window_prop_t properties structure corresponding to the dialog box
 */
void move_and_show_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_prop)
{
	if (dialog_prop->displayed == FALSE)
	{
		gtk_window_move(GTK_WINDOW(dialog_box), dialog_prop->x, dialog_prop->y);
		gtk_window_resize(GTK_WINDOW(dialog_box), dialog_prop->width, dialog_prop->height);
		gtk_widget_show_all(dialog_box);
		dialog_prop->displayed = TRUE;
	}
}

/**
 * @fn void record_dialog_box_position(GtkWidget *dialog_box, window_prop_t *dialog_prop)
 * Records one dialog position
 * @param dialog_box : a dialog box from which we want to record the position
 * @param[in,out] dialog_prop : window_prop_t properties structure corresponding to the dialog box
 */
void record_dialog_box_position(GtkWidget *dialog_box, window_prop_t *dialog_prop)
{
	gint x = 0;
	gint y = 0;
	gint width = WPT_DEFAULT_WIDTH;
	gint height = WPT_DEFAULT_HEIGHT;
	
	if (dialog_prop != NULL && dialog_prop->displayed == TRUE)
	{
		if (dialog_box != NULL)
		{
			gtk_window_get_position(GTK_WINDOW(dialog_box), &x, &y);
			gtk_window_get_size(GTK_WINDOW(dialog_box), &width, &height);
			dialog_prop->x = x;
			dialog_prop->y = y;
			dialog_prop->width = width;
			dialog_prop->height = height;
		}
	}
}


/**
 * @fn void record_all_dialog_box_positions(heraia_window_t *main_window)
 * Records all the positions of the displayed windows
 * @param[in,out] main_window : main structure
 */
void record_all_dialog_box_positions(heraia_window_t *main_window)
{
	GtkWidget *dialog_box = NULL;

	if (main_window != NULL && 
		main_window->xmls != NULL && 
		main_window->xmls->main != NULL &&
		main_window->win_prop != NULL &&
		main_window->current_DW != NULL)
	{
		/* data interpretor */
		dialog_box = main_window->current_DW->diw;
		record_dialog_box_position(dialog_box, main_window->win_prop->data_interpretor);
	
		/* About box */
		dialog_box = heraia_get_widget (main_window->xmls->main, "about_dialog");
		record_dialog_box_position(dialog_box, main_window->win_prop->about_box);
	
		/* Log window */
		dialog_box = heraia_get_widget (main_window->xmls->main, "log_window");
		record_dialog_box_position(dialog_box, main_window->win_prop->log_box);
	
		/* main_dialog */
		dialog_box = heraia_get_widget (main_window->xmls->main, "main_window");
		record_dialog_box_position(dialog_box, main_window->win_prop->main_dialog);
	
		/* plugin_list */
		dialog_box = heraia_get_widget (main_window->xmls->main, "plugin_list_window");
		record_dialog_box_position(dialog_box, main_window->win_prop->plugin_list);
	
		/* list data types */
		dialog_box = heraia_get_widget (main_window->xmls->main, "list_data_types_window");
		record_dialog_box_position(dialog_box, main_window->win_prop->ldt);
	
		/* main_preferences */
		dialog_box = heraia_get_widget (main_window->xmls->main, "main_preferences_window");
		record_dialog_box_position(dialog_box, main_window->win_prop->main_pref_window);
	}
}
	
	
/**
 * @fn void record_and_hide_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_prop)
 *  Record position and hide a dialog box
 * @param dialog_box : the dialog box we want to record its position and then hide
 * @param dialog_prop : window_prop_t properties structure corresponding to the dialog box
 */
void record_and_hide_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_prop)
{

	if (dialog_prop->displayed == TRUE)
	{
		record_dialog_box_position(dialog_box, dialog_prop);
		
		gtk_widget_hide(dialog_box);
		dialog_prop->displayed = FALSE;
	}
}


/**
 * @fn static void record_and_hide_about_box(heraia_window_t *main_window)
 *  Record position and hide about dialog box
 * @param [in,out] main_window : main structure
 */
static void record_and_hide_about_box(heraia_window_t *main_window)
{
	GtkWidget *about_dialog = NULL;	
	
	about_dialog = heraia_get_widget(main_window->xmls->main, "about_dialog");
	
	if (about_dialog != NULL)
	{
		record_and_hide_dialog_box(about_dialog, main_window->win_prop->about_box);
	}
}


/**
 * @fn void a_propos_response(GtkWidget *widget, gint response, gpointer data)
 *  To close the A propos dialog box (with the "close" button)
 * @param widget : calling widget (may be NULL as we don't use this)
 * @param response : may be whatever you want as we neither use this !
 * @param data : MUST be heraia_window_t *main_window main structure
 */
static void a_propos_response(GtkWidget *widget, gint response, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	record_and_hide_about_box(main_window);
}

/**
 * @fn void a_propos_close(GtkWidget *widget, gpointer data)
 *  To close the A propos dialog box
 * @param widget : calling widget (may be NULL as we don't use this)
 * @param data : MUST be heraia_window_t *main_window main structure
 */
static void a_propos_close(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	record_and_hide_about_box(main_window);
}


/**
 * @fn gboolean a_propos_delete(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  To close the A propos dialog box
 * @param widget : calling widget (may be NULL as we don't use this)
 * @param event : event associated (may be NULL as we don't use this)
 * @param data : MUST be heraia_window_t *main_window main structure
 * @return returns TRUE in order to allow other functions to do something with
 *         that event.
 */
static gboolean a_propos_delete(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	record_and_hide_about_box(main_window);
	
	return TRUE;
}


/**
 * @fn void on_delete_activate(GtkWidget *widget, gpointer data)
 *  Delete, edit menu
 * @warning Not yet implemented
 * @todo Write a usefull function here :)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_delete_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

/**
 * @fn void on_cut_activate(GtkWidget *widget, gpointer data)
 *  Cut, edit menu
 * @warning Not yet implemented
 * @todo Write a usefull function here :)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_cut_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}

/**
 * @fn void on_copy_activate( GtkWidget *widget, gpointer data )
 *  Copy, edit menu
 * @warning Not yet implemented
 * @todo Write a usefull function here :)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_copy_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}


/**
 * @fn void on_paste_activate( GtkWidget *widget, gpointer data )
 *  Paste, edit menu
 * @warning Not yet implemented
 * @todo Write a usefull function here :)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_paste_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	log_message(main_window, G_LOG_LEVEL_WARNING, "Not implemented Yet (Please contribute !)");
}


/**
 * @fn void refresh_file_labels(heraia_window_t *main_window)
 *  This function is refreshing the labels on the main
 *  window in order to reflect cursor position, selected
 *  positions and total selected size
 * @param main_window : main structure
 */
void refresh_file_labels(heraia_window_t *main_window)
{
	GtkWidget *label = NULL;
	guint64 position = 0;
	gchar *text = NULL;
	
	if (main_window != NULL)
		{
			if (main_window->current_DW != NULL && main_window->current_DW->current_hexwidget != NULL)
				{
					label = heraia_get_widget(main_window->xmls->main, "file_position_label");
					position = ghex_get_cursor_position(main_window->current_DW);
					/* position begins at 0 and this is not really human readable */
					/* it's more confusing than anything so we do + 1             */
					/* To translators : do not translate <small> and such         */
					
					if (is_toggle_button_activated(main_window->xmls->main, "mp_thousand_bt") == TRUE)
					{
						text = g_strdup_printf("<small>%'lld</small>", position + 1);
					}
					else
					{
						text = g_strdup_printf("<small>%lld</small>", position + 1);
					}
					gtk_label_set_markup(GTK_LABEL(label), text);
					g_free(text);
				}
			else
				{
					label = heraia_get_widget(main_window->xmls->main, "file_position_label");
					gtk_label_set_text(GTK_LABEL(label), "");
				}
		}
}


/** 
 * @fn void refresh_event_handler(GtkWidget *widget, gpointer data)
 *  This function is here to ensure that everything will be
 *  refreshed upon a signal event.
 * @warning This function is not thread safe (do not use in a thread)
 * @todo try to put some mutexes on main_window->event to make this
 *       thread safe some way
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void refresh_event_handler(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	if (main_window != NULL)
	{
		/* Beware, this mechanism is not thread safe ! */
		if (main_window->event == HERAIA_REFRESH_NOTHING)
		{
			main_window->event = HERAIA_REFRESH_CURSOR_MOVE;
		}
		
		refresh_data_interpretor_window(widget, main_window);
		refresh_all_plugins(main_window);
		refresh_file_labels(main_window);
		
		main_window->event = HERAIA_REFRESH_NOTHING;
	}
}


/**
 * @fn void on_open_activate(GtkWidget *widget, gpointer data)
 *  This handles the menuitem "Ouvrir" to open a file
 * @warning This function is not thread safe (do not use in a thread)
 * @todo try to put some mutexes on main_window->event to make this
 *       thread safe some way
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_open_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	if (select_file_to_load(main_window) == TRUE)
	{
		load_file_to_analyse(main_window, main_window->filename);
	
	    /* Not thread safe here ? */
	    main_window->event = HERAIA_REFRESH_NEW_FILE;
	    refresh_event_handler(main_window->current_DW->current_hexwidget, main_window);
	}
}


/**
 * @fn void on_save_activate(GtkWidget *widget, gpointer data)
 *  Here we attemp to save the edited file
 *  @todo be more accurate on error (error type, message and filename) returns
 *        we should return something at least ...
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_save_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;	
	HERAIA_ERROR erreur = HERAIA_NOERR;
	gchar *filename = NULL;
	
	if (main_window != NULL && main_window->current_doc != NULL)
	{
		erreur = heraia_hex_document_save(main_window);	

		if (erreur != HERAIA_NOERR)
		{
			filename = heraia_hex_document_get_filename(main_window->current_doc);
			log_message(main_window, G_LOG_LEVEL_ERROR, "Error while saving file %s !", filename);
		}
	}
}

/**
 * @fn void on_save_as_activate(GtkWidget *widget, gpointer data)
 *  This handle the save_as menu entry (here the filename changes)
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
 */
void on_save_as_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	HERAIA_ERROR erreur = HERAIA_NOERR;
	gchar *filename = NULL;  /* Auto malloc'ed, do not free */

	if (main_window != NULL && main_window->current_doc != NULL)
	{
		filename = select_a_file_to_save(main_window);
		
		if (filename != NULL)
		{
			erreur = heraia_hex_document_save_as(main_window, filename);
		}
		else
		{
			erreur = HERAIA_CANCELLED;
		}
		
		if (erreur != HERAIA_NOERR)
		{
			if (erreur == HERAIA_CANCELLED)
			{
				log_message(main_window, G_LOG_LEVEL_DEBUG, "Saving file as... : operation cancelled.");
			}
			else
			{
				log_message(main_window, G_LOG_LEVEL_ERROR, "Error while saving file as %s", main_window->current_doc->file_name);
			}
		}
		else
		{
			/* updating the window name and tab's name */
			update_main_window_name(main_window);
			set_notebook_tab_name(main_window);
			log_message(main_window, G_LOG_LEVEL_DEBUG, "File %s saved and now edited.", main_window->current_doc->file_name);
		}
	}
}


/**
 * @fn void on_DIMenu_activate(GtkWidget *widget, gpointer data)
 *  This handles the menuitem "Data Interpretor" that
 *  shows or hides the data interpretor window     
 * @param widget : the widget that issued the signal
 * @param data : user data MUST be heraia_window_t *main_window main structure
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
				/* dw->window_displayed = !(dw->window_displayed); */
				notebook = GTK_NOTEBOOK(heraia_get_widget(main_window->xmls->main, "diw_notebook"));
				
			if (main_window->win_prop->data_interpretor->displayed == FALSE)
				{
					/* Setting the first page of the notebook as default (Numbers) */					
					gtk_notebook_set_current_page(notebook, dw->tab_displayed);
					
					/* moving to the right position */
					move_and_show_dialog_box(dw->diw, main_window->win_prop->data_interpretor);
					
					refresh_data_interpretor_window(widget, data);
				}
				else
				{
					/* recording some prefs from the dialog : position + opened tab */
					dw->tab_displayed = gtk_notebook_get_current_page(notebook);
					record_and_hide_dialog_box(dw->diw, main_window->win_prop->data_interpretor);
				}
			}
		}
	}
}


/**
 * @fn delete_main_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  When the user destroys or delete the main window
 * @param widget : calling widget 
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_window_t *main_window main structure
 */
gboolean delete_main_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
		
	on_quit_activate(widget, data);
	
	return FALSE;
}


/**
 * @fn gboolean delete_dt_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  call back function for the data interpretor window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_window_t *main_window main structure and not NULL
 */
gboolean delete_dt_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{	
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	g_signal_emit_by_name(heraia_get_widget(main_window->xmls->main, "DIMenu"), "activate");
	
	return TRUE;
}

/**
 * @fn void destroy_dt_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  call back function for the data interpretor window destruction
 * @param widget : calling widget (may be NULL as we don't use this here)
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_window_t *main_window main structure and not NULL
 */
void destroy_dt_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	g_signal_emit_by_name(heraia_get_widget(main_window->xmls->main, "DIMenu"), "activate");
}
/* End of call back functions that handle the data interpretor window */


/**
 * @fn static gchar *make_absolute_path(gchar *filename)
 *  Returns an absolute path to the filename
 *  the string should be freed when no longer needed
 *  very UGLy !
 * @todo do something without any system calls !!! 
 * @param filename : relative notation filename from which to extract an
 *        absolute path
 * @return returns a string with the absolute path which should be freed when
 *         no longer needed
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
 * @fn void set_the_working_directory(GtkFileChooser *file_chooser, gchar *filename)
 *  Sets the working directory for the file chooser to the directory of the
 *  filename (even if filename is a relative filename such as ../docs/test_file)
 * @param file_chooser : An initialized GtkFileChooser
 * @param filename : a filename (one previously openned)
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
 * @fn gboolean select_file_to_load(heraia_window_t *main_window)
 *  This function does open a file selector dialog box and returns the selected 
 *  filename. 
 *  We do fill the main_window->filename parameter here !
 * @param main_window : main structure
 * @return returns TRUE if everything went ok, FALSE otherwise
 */
gboolean select_file_to_load(heraia_window_t *main_window)
{
	GtkWidget *parent = NULL; /* A parent window (we use main_window)      */
	GtkFileChooser *file_chooser = NULL;
	gboolean success = FALSE;
	gchar *filename = NULL;   /* filename selected (if any) to be openned  */
	
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
			
			/* this should be managed with lists and not here !! */		
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
 * @fn gchar *select_a_file_to_save(heraia_window_t *main_window)
 *  This function opens a dialog box that allow one to choose a 
 *  file name to the file which is about to be saved
 * @param main_window : main structure
 * @return returns complete filename (path and filename)
 */
gchar *select_a_file_to_save(heraia_window_t *main_window)
{
	GtkWidget *parent = NULL;     /* A parent window (we use main_window) */
	GtkFileChooser *fcd = NULL;
	gchar *filename = NULL; 

	parent = heraia_get_widget(main_window->xmls->main, "main_window");
	
	/* Selection a name to the file to save */
	fcd = GTK_FILE_CHOOSER(gtk_file_chooser_dialog_new("Save As...",
													   GTK_WINDOW(parent),
													   GTK_FILE_CHOOSER_ACTION_SAVE, 
													   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
													   GTK_STOCK_SAVE, GTK_RESPONSE_OK, 
													   NULL));

	/* window properties : modal, without multi-selection and with confirmation */
	gtk_window_set_modal(GTK_WINDOW(fcd), TRUE);
	gtk_file_chooser_set_select_multiple(fcd, FALSE);
	gtk_file_chooser_set_do_overwrite_confirmation(fcd, TRUE);

	/* we do want to have the file's directory where to save the new file */
	if (main_window->filename != NULL)
	   {
			set_the_working_directory(fcd, main_window->filename);
	   }
	
	switch(gtk_dialog_run(GTK_DIALOG(fcd)))
		{
		case GTK_RESPONSE_OK:
			/* retrieving the filename */
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fcd));
			break;
		default:
			filename = NULL;
			break;
		}

	gtk_widget_destroy(GTK_WIDGET(fcd));

	return filename;
}


/**
 * @fn void update_main_window_name(heraia_window_t *main_window)
 *  Update main window heraia's name to reflect the current edited file
 * @param main_window : main structure
 */
void update_main_window_name(heraia_window_t *main_window)
{
	GtkWidget *widget = NULL;
	gchar *filename = NULL;
	
	if (main_window != NULL && main_window->current_doc != NULL)
	   {
			widget = heraia_get_widget(main_window->xmls->main, "main_window");
		    filename = g_filename_display_basename(main_window->current_doc->file_name);
		   
			gtk_window_set_title(GTK_WINDOW(widget), filename);
	   }
}

/**
 * @fn void set_notebook_tab_name(heraia_window_t *main_window)
 *  Sets notebook's tab's name. This function should only be called
 *  when a new filename was set (open and save as functions)
 * @param main_window : main structure
 */
void set_notebook_tab_name(heraia_window_t *main_window)
{
	GtkWidget *notebook = NULL; /* file notebook in main window       */
	GtkWidget *page = NULL;     /* Current page for the file notebook */
	GtkWidget *label = NULL;    /* tab's label                        */
	gchar *filename = NULL;
	gint current = 0;
	
	if (main_window != NULL && main_window->current_doc != NULL)
	   {
		   notebook = heraia_get_widget(main_window->xmls->main, "file_notebook");
		   current = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
		   page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current);
		   label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), page);
		   filename = g_filename_display_basename(main_window->current_doc->file_name);
		   gtk_label_set_text(GTK_LABEL(label), filename);
		   
		   /* gtk_widget_set_tooltip_text is available since gtk 2.12 */
		   if (GTK_MINOR_VERSION >= 12) 
			{ 
				gtk_widget_set_tooltip_text(label, g_filename_display_name(main_window->current_doc->file_name));
			}
	   }
}


/**
 * @fn init_heraia_interface(heraia_window_t *main_window)
 *  Here we might init some call backs and menu options
 *  and display the interface (main && sub-windows)
 *  This function should be called only once at main program's 
 *  init time
 * @param main_window : main structure
 */
void init_heraia_interface(heraia_window_t *main_window)
{
	GtkWidget *notebook = NULL;  /* file notebook in main window */
	
	if (main_window != NULL)
	{
		/* inits window states (shows or hide windows) */
		init_window_states(main_window);
		
		/* Notebook selection */
		notebook = heraia_get_widget(main_window->xmls->main, "file_notebook");
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
		
		if (main_window->current_doc != NULL)
		{
			gtk_widget_show(notebook);
		}
		else
		{
			/** Hide notebook and menus @todo put these lines in a specific function */
			gtk_widget_set_sensitive(heraia_get_widget(main_window->xmls->main, "save"), FALSE);
			gtk_widget_set_sensitive(heraia_get_widget(main_window->xmls->main, "save_as"), FALSE);				 
			gtk_widget_hide(notebook); 
		}
		
		refresh_file_labels(main_window);
	}
}


/**
 * @fn gboolean load_heraia_glade_xml(heraia_window_t *main_window)
 *  Loads the glade xml files that describes the heraia project
 *  tries the following paths in that order :                 
 *  - /etc/heraia/heraia.glade
 *  - /home/[user]/.heraia/heraia.glade
 *  - PWD/heraia.glade
 * @param main_window : main structure
 * @return TRUE if everything went ok, FALSE otherwise
 */
static gboolean load_heraia_glade_xml(heraia_window_t *main_window)
{
	gchar *filename = NULL;
	
	if (main_window != NULL && main_window->xmls != NULL)
	{
		filename = g_strdup_printf("heraia.glade"); 
		main_window->xmls->main = load_glade_xml_file(main_window->location_list, filename);
		g_free(filename);
		
		if (main_window->xmls->main == NULL)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

/**
 * @fn connect_cursor_moved_signal(heraia_window_t *main_window)
 *  Connects the signal that the cursor has moved to
 *  the refreshing function
 * @param main_window : main structure 
 */
void connect_cursor_moved_signal(heraia_window_t *main_window)
{
	g_signal_connect(G_OBJECT(main_window->current_DW->current_hexwidget), "cursor_moved",   
					 G_CALLBACK(refresh_event_handler), main_window);
}


/**
 * @fn void heraia_ui_connect_signals(heraia_window_t *main_window)
 *  Connect the signals at the interface
 * @param main_window : main structure
 */
static void heraia_ui_connect_signals(heraia_window_t *main_window)
{
	
	/* the data interpretor menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "DIMenu")), "activate", 
					  G_CALLBACK (on_DIMenu_activate), main_window);
	
	/* Quit, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "quit")), "activate", 
					  G_CALLBACK (on_quit_activate), main_window);
	
	/* New, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "new")), "activate",  
					  G_CALLBACK (on_new_activate), main_window);
	
	/* Open, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "open")), "activate",  
					  G_CALLBACK (on_open_activate), main_window);
	
	/* Save, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "save")), "activate",  
					  G_CALLBACK (on_save_activate), main_window);
	
	/* Save As, file menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "save_as")), "activate",  
					  G_CALLBACK (on_save_as_activate), main_window); 
	
	/* Preferences, file menu ; See main_pref_window.c for main_pref_window's signals */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "preferences")), "activate",  
					  G_CALLBACK (on_preferences_activate), main_window);
	
	/* Cut, edit menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "cut")), "activate",  
					  G_CALLBACK (on_cut_activate), main_window); 
	
	/* Copy, edit menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "copy")), "activate",  
					  G_CALLBACK (on_copy_activate), main_window); 
	
	/* Paste, edit menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "paste")), "activate",  
					  G_CALLBACK (on_paste_activate), main_window); 
	
	/* Delete, edit menu */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "delete")), "activate",  
					  G_CALLBACK (on_delete_activate), main_window); 
	
	
	/* about dialog box */		
	g_signal_connect (G_OBJECT(heraia_get_widget(main_window->xmls->main, "a_propos")), "activate",  
					  G_CALLBACK(a_propos_activate), main_window); 
	
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "about_dialog")), "close",
					 G_CALLBACK(a_propos_close), main_window);
	
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "about_dialog")), "response",
					 G_CALLBACK(a_propos_response), main_window);
	
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "about_dialog")), "delete-event",
					 G_CALLBACK(a_propos_delete), main_window);
	
	
	/* main window killed or destroyed */
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "main_window")), "delete-event", 
					  G_CALLBACK (delete_main_window_event), main_window);
	
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "main_window")), "response", 
					  G_CALLBACK (delete_main_window_event), main_window);
	
	g_signal_connect (G_OBJECT (heraia_get_widget(main_window->xmls->main, "main_window")), "close", 
					  G_CALLBACK (on_quit_activate), main_window);
}

/** @fn int load_heraia_ui(heraia_window_t *main_window)
 *  Loads, if possible, the glade xml file and then connects the
 *  signals and inits the following windows :
 *  - log window
 *  - data_interpretor window
 *  - list data types
 * @param main_window : main structure
 * @return TRUE if load_heraia_glade suceeded, FALSE otherwise
 * @todo add more return values to init functions to detect any error while
 *       initializing the ui
 */
int load_heraia_ui(heraia_window_t *main_window)
{
	gboolean success = FALSE;
	
	/* load the XML interfaces (main & treatment) */
	success = load_heraia_glade_xml(main_window);
	
	if (success == TRUE)
	{
		/* Heraia UI signals */
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, "Connecting heraia_ui signals     ");
		}
		
		heraia_ui_connect_signals(main_window);
		
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, " [Done]\n");
		}
		
		/* The Log window */
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, "log window init interface        ");
		}
		
		log_window_init_interface(main_window);
		
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, " [Done]\n");
		}
		
		/* Preferences window */
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, "preferences window init interface");
		}
		
		main_pref_window_init_interface(main_window);
		
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, " [Done]\n");
		}
		
		
		/* The data interpretor window */
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, "data interpretor init interface  ");
		}
		
		data_interpretor_init_interface(main_window);
		
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, " [Done]\n");
		}
		
		
		/* The list data types window */
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, "list data types init interface   ");
		}
		
		list_data_types_init_interface(main_window);
		
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, " [Done]\n");
		}
		
		
		/* The data type window (create or edit one type) */
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, "data type init interface         ");
		}
		
		data_type_init_interface(main_window);
		
		if (main_window->debug == TRUE)
		{
			fprintf(stdout, " [Done]\n");
		}
		
		fprintf(stdout, "Loading heraia preference file   ");
		
		if (load_preference_file(main_window) != TRUE)
		{
			fprintf(stdout, " [FAILED]\n");
		}
		else /* Setting up preferences */
		{
			fprintf(stdout, " [Done]\n");
			fprintf(stdout, "Setting up preferences           ");
			load_preferences(main_window);
			fprintf(stdout, " [Done]\n");
		}
	}
	return success;
}


/**
 * @fn void add_text_to_textview(GtkTextView *textview, const char *format, ...)
 *  adds a text to a textview
 * @param textview : the textview where to add text
 * @param format : printf style format
 * @param ... : a va_list arguments to fit format (as with printf)
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
 * @fn kill_text_from_textview(GtkTextView *textview)
 *  Kills the text from a textview
 * @param textview : the textview to kill the text from
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
 * @fn GtkWidget *gtk_radio_button_get_active(GSList *group)
 *  Try to find the active radio button widget in a group
 *  This does not take into account inconsistant states
 *  returns the first active radio button otherwise NULL
 * @param group : A group of GtkRadioButtons
 * @return returns the active widget if any (NULL if none)
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
 * @fn GtkWidget *gtk_radio_button_get_active_from_widget(GtkRadioButton *radio_group_member)
 * gets the active radio button from a radio group
 * @param radio_group_member : widget to get radio group from
 * @returns the active GtkRadioButton within the group from
 *          radio_group_member
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
 * @fn gboolean is_cmi_checked(GtkWidget *check_menu_item)
 *  Tells whether a GtkCheckMenuItem is Checked or not
 * @param check_menu_item : a GtkCheckMenuItem to verify
 * @return returns TRUE if the Check Manu Item is checked, FALSE otherwise
 */
gboolean is_cmi_checked(GtkWidget *check_menu_item)
{
	return gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(check_menu_item));
}


/**
 * @fn gboolean is_toggle_button_activated(GladeXML *main_xml, gchar *check_button)
 *  returns the state of a named check button contained 
 *  in the Glade XML description
 * @param main_xml : a GladeXML definition
 * @param check_button : the name of an existing check_button within the glade 
 *        definition
 * @return TRUE if the button is activated / toggled , FALSE otherwise
 */
gboolean is_toggle_button_activated(GladeXML *main_xml, gchar *check_button)
{
	gboolean activated = FALSE;
	
	if (main_xml != NULL)
	{
		activated = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(heraia_get_widget(main_xml, check_button)));
	}
	
	return activated;
}


/**
 * @fn GtkWidget *heraia_get_widget(GladeXML *xml, gchar *widget_name)
 *  This is a wrapper to the glade xml get widget. It is intended
 *  to simplify the developpers lives if they have to choose or
 *  propose other means to do the same thing than libglade (say,
 *  for example, GtkBuilder :)
 * @param xml : A glade XML definition 
 * @param widget_name : an existing widget name in the glade definition
 * @return returns the widget itself if it exists in the definition file (NULL
 *         otherwise)
 */
GtkWidget *heraia_get_widget(GladeXML *xml, gchar *widget_name)
{
   /** 
	* For debug purposes only (very verbose as this function is the main used)
	* fprintf(stdout, "Getting Widget named %s\n", widget_name); 
	*/
	
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
 * @fn void destroy_a_single_widget(GtkWidget *widget)
 *  Destroys a single widget if it exists
 * @param widget : the widget to destroy
 */
void destroy_a_single_widget(GtkWidget *widget)
{
	if (widget != NULL)
	{
		gtk_widget_destroy(widget);
	}
}

/**
 * @fn void close_heraia(heraia_window_t *main_window)
 * Before closing heraia we need to do few things
 * @param main_window : main_struct
 */
static void close_heraia(heraia_window_t *main_window)
{

	/* recording window's position */
	record_all_dialog_box_positions(main_window);
	
	/* . Saving preferences */
	save_preferences(main_window);
}

/**
 * @fn void init_one_cmi_window_state(GtkWidget *dialog_box, GtkWidget *cmi, window_prop_t *dialog_prop)
 * init one cmi window based state
 * @param dialog_box : the window or dialog box we want to init its state
 * @param cmi : corresponding check menu item 
 * @param dialog_prop : corresponding window properties (should be initialized and not NULL)
 */
static void init_one_cmi_window_state(GtkWidget *dialog_box, GtkWidget *cmi, window_prop_t *dialog_prop)
{
	gboolean activated = FALSE;
	
	if (dialog_box != NULL && cmi != NULL && dialog_prop != NULL)
	{
		activated = dialog_prop->displayed;
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(cmi), activated);
		if (activated == TRUE)
		{
			gtk_window_move(GTK_WINDOW(dialog_box), dialog_prop->x, dialog_prop->y);
			gtk_window_resize(GTK_WINDOW(dialog_box), dialog_prop->width, dialog_prop->height);
			gtk_widget_show_all(dialog_box);
		}
	}
}


/**
 * @fn init_window_states(heraia_window_t *main_window)
 *  Inits all windows states (positions, displayed, and so on...)
 * @param main_window : main structure
 */
void init_window_states(heraia_window_t *main_window)
{
	GtkWidget *cmi = NULL;
	GtkWidget *dialog_box = NULL;
	
	if (main_window != NULL && main_window->xmls != NULL  && main_window->xmls->main != NULL)
	{
		if (main_window->win_prop)
		{
			/* Main window (always the first one) */
			dialog_box = heraia_get_widget(main_window->xmls->main, "main_window");
			if (main_window->win_prop->main_dialog->displayed == TRUE)
			{
				gtk_window_move(GTK_WINDOW(dialog_box), main_window->win_prop->main_dialog->x, main_window->win_prop->main_dialog->y);
				gtk_window_resize(GTK_WINDOW(dialog_box), main_window->win_prop->main_dialog->width, main_window->win_prop->main_dialog->height);
				gtk_widget_show(dialog_box);
			}
			
			/* Log Window Interface */
			cmi = heraia_get_widget(main_window->xmls->main, "mw_cmi_show_logw");
			dialog_box = heraia_get_widget(main_window->xmls->main, "log_window");
			init_one_cmi_window_state(dialog_box, cmi, main_window->win_prop->log_box);
			
			/* Data Interpretor Interface */
			cmi = heraia_get_widget(main_window->xmls->main, "DIMenu");
			/* Emit the specific signal to activate the check_menu_item */
			if (main_window->win_prop->data_interpretor->displayed == TRUE)
			{
				main_window->win_prop->data_interpretor->displayed = FALSE; /* dirty trick */
				g_signal_emit_by_name(heraia_get_widget(main_window->xmls->main, "DIMenu"), "activate");
			}
		
			/* List Data type Interface */
			cmi = heraia_get_widget(main_window->xmls->main, "ldt_menu");
			dialog_box = heraia_get_widget(main_window->xmls->main, "list_data_types_window");
			init_one_cmi_window_state(dialog_box, cmi, main_window->win_prop->ldt);
			
			/* Plugin List Interface */
			cmi = heraia_get_widget(main_window->xmls->main, "mw_cmi_plugin_list");
			dialog_box = heraia_get_widget(main_window->xmls->main, "plugin_list_window");
			init_one_cmi_window_state(dialog_box, cmi, main_window->win_prop->plugin_list);		
			
			/* Preferences window */
			dialog_box = heraia_get_widget(main_window->xmls->main, "main_preferences_window");
			if (main_window->win_prop->main_pref_window->displayed == TRUE)
			{
				/* main_window->win_prop->main_pref_window->displayed = FALSE; dirty trick */
				gtk_window_move(GTK_WINDOW(dialog_box), main_window->win_prop->main_pref_window->x, main_window->win_prop->main_pref_window->y);
				gtk_window_resize(GTK_WINDOW(dialog_box), main_window->win_prop->main_pref_window->width, main_window->win_prop->main_pref_window->height);
				gtk_widget_show_all(dialog_box);
			}
			
			/* About Box */
			dialog_box = heraia_get_widget(main_window->xmls->main, "about_dialog");
			if (main_window->win_prop->about_box->displayed == TRUE)
			{
				/* main_window->win_prop->main_pref_window->displayed = FALSE; dirty trick */
				gtk_window_move(GTK_WINDOW(dialog_box), main_window->win_prop->about_box->x, main_window->win_prop->about_box->y); 
				gtk_window_resize(GTK_WINDOW(dialog_box), main_window->win_prop->about_box->width, main_window->win_prop->about_box->height);
				set_a_propos_properties(dialog_box);
				gtk_widget_show_all(dialog_box);
			}
		}
	}
}

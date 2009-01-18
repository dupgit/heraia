/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  main_pref_window.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
 *
 *  (C) Copyright 2008 - 2009 Olivier Delhomme
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

#include <libheraia.h>

static void pref_window_close(GtkWidget *widget, gpointer data);
static gboolean pref_window_delete(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void main_pref_window_connect_signals(heraia_window_t *main_window);

/* ToolBar buttons */
static void on_mp_tb_fp_bt_clicked(GtkToolButton *toolbutton, gpointer data);
static void on_mp_tb_display_bt_clicked(GtkToolButton *toolbutton, gpointer data);

/* Toogle Buttons */
static void on_mp_thousand_bt_toggled(GtkToggleButton *togglebutton, gpointer data);



/**** The Signals ****/

/** 
 *  Closing the window 
 */
static gboolean pref_window_delete(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *pref_window = NULL;
	
	if (main_window != NULL && main_window->win_prop != NULL && main_window->win_prop->main_pref_window != NULL)
	{
		pref_window = heraia_get_widget(main_window->xmls->main, "main_preferences_window");
		save_main_preferences(main_window);
		record_and_hide_dialog_box(pref_window, main_window->win_prop->main_pref_window);
	}
	
	return TRUE;
}

/**
 *  Connecting the window signals to the right functions  
 */
static void main_pref_window_connect_signals(heraia_window_t *main_window)
{
	/* Closing the window */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "main_preferences_window")), "delete-event",
					 G_CALLBACK(pref_window_delete), main_window);

	/* Clicking on the file preference button of the toolbar */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "mp_tb_fp_bt")), "clicked",
					 G_CALLBACK(on_mp_tb_fp_bt_clicked), main_window);
	
	/* Clicking on the file preference button of the toolbar */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "mp_tb_display_bt")), "clicked",
					 G_CALLBACK(on_mp_tb_display_bt_clicked), main_window);
	
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "mp_thousand_bt")), "toggled",
					 G_CALLBACK(on_mp_thousand_bt_toggled), main_window);
}


/**
 * Tool buttons
 */

/**
 *  Main Preferences, ToolBar, File Preference Button
 */
static void on_mp_tb_fp_bt_clicked(GtkToolButton *toolbutton, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *notebook = NULL;  /* Main Preference Window's Notebook */
	
	if (main_window != NULL && main_window->xmls != NULL && main_window->xmls->main != NULL)
	{
		notebook = heraia_get_widget(main_window->xmls->main, "mp_first_notebook");
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
	}
}

/**
 * Main Preferences, ToolBar, Display Button
 */
static void on_mp_tb_display_bt_clicked(GtkToolButton *toolbutton, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *notebook = NULL;  /* Main Preference Window's Notebook */
	
	if (main_window != NULL && main_window->xmls != NULL && main_window->xmls->main != NULL)
	{
		notebook = heraia_get_widget(main_window->xmls->main, "mp_first_notebook");
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
	}
}

/**
 * Refreshes the file labels as an option has been sat
 */
static void on_mp_thousand_bt_toggled(GtkToggleButton *togglebutton, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	refresh_file_labels(main_window);
}

/**** End Signals ****/


/**
 *  Inits the main preferences window interface 
 */
void main_pref_window_init_interface(heraia_window_t *main_window)
{
	main_pref_window_connect_signals(main_window);
}

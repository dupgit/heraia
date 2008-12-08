/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  main_pref_window.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
 *
 *  (C) Copyright 2005 - 2008 Olivier Delhomme
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

static gboolean delete_main_pref_window(GtkWidget *widget, GdkEvent  *event, gpointer data );
static void destroy_main_pref_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void main_pref_window_connect_signals(heraia_window_t *main_window);


static void main_pref_window_close(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *pref_window = NULL;
	
	pref_window = heraia_get_widget(main_window->xmls->main, "main_preferences_window");
	log_message(main_window, G_LOG_LEVEL_DEBUG, "Killed main preferences window");
	
	if (pref_window != NULL)
	{
		main_window->win_prop->main_pref_window = record_and_hide_dialog_box(pref_window, main_window->win_prop->main_pref_window);
	}
}


/**** The Signals ****/

/** 
 *  Closing the window 
 */
static gboolean delete_main_pref_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	main_pref_window_close(widget, data);

	return FALSE;
}

 static void destroy_main_pref_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	main_pref_window_close(widget, data);
}

/**
 *  Connecting the window signals to the right functions  
 */
static void main_pref_window_connect_signals(heraia_window_t *main_window)
{

	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "main_preferences_window")), "delete_event", 
					 G_CALLBACK(delete_main_pref_window), main_window);

	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "main_preferences_window")), "destroy", 
					 G_CALLBACK(destroy_main_pref_window), main_window);
}

/**** End Signals ****/

/**
 *  Inits the main preferences window interface 
 */
void main_pref_window_init_interface(heraia_window_t *main_window)
{
	main_pref_window_connect_signals(main_window);
}

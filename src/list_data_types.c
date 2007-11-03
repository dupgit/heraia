/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  list_data_types.c
  window allowing the user to manage his data types (add, remove, edit and
  eventually save them)
  
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

/**
 *  Shows or hide the list data type window
 */

void on_ldt_menu_activate(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkWidget *check_menu_item = NULL;

	if (main_window != NULL)
		{
			check_menu_item = glade_xml_get_widget(main_window->xml, "ldt_menu");

			if (is_cmi_checked(check_menu_item) == TRUE)
				{  /* if the menu is checked, shows the window */
					gtk_widget_show_all(glade_xml_get_widget(main_window->xml, "list_data_types_window")); 
				}
			else
				{
					gtk_widget_hide(glade_xml_get_widget(main_window->xml, "list_data_types_window")); 
				}
		}
}


/**
 *  Called when the list data types is killed or closed
 *  !! Not to be confused with delete_dt_window_event !!
 */
static gboolean delete_ldt_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{	
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name(glade_xml_get_widget(main_window->xml, "ldt_menu"), "activate");

	return TRUE;
}


/**
 *  Called when the list data types is killed or closed
 *  !! Not to be confused with destroy_dt_window !!
 */
static void destroy_ldt_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name(glade_xml_get_widget(main_window->xml, "ldt_menu"), "activate");
}


/**
 *  Connects list_data_types's window signals.
 */
static void connect_list_data_types_signals(heraia_window_t *main_window)
{

	if (main_window != NULL && main_window->xml != NULL)
		{
			/* list data types menu */
			g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "ldt_menu")), "activate", 
							  G_CALLBACK(on_ldt_menu_activate), main_window);
      
			/* list data types window killed or destroyed */
			g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "list_data_types_window")), "delete_event", 
							  G_CALLBACK(delete_ldt_window_event), main_window);

			g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "list_data_types_window")), "destroy", 
							  G_CALLBACK(destroy_ldt_window), main_window);
		}
}


/**
 *  Inits the list data type window
 *  with default values
 *  Should be called only once
 */
void list_data_types_init_interface(heraia_window_t *main_window)
{

	if (main_window != NULL)
		{
			connect_list_data_types_signals(main_window);
		}
}

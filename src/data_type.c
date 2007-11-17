/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  data_types.c
  Window allowing the user to create or edit a new data type
  
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

static gboolean is_data_type_name_already_used(GList *data_type_list, gchar *name);
static gboolean delete_data_type_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_data_type_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void dt_ok_button_clicked(GtkWidget *widget, gpointer data);
static void dt_cancel_button_clicked(GtkWidget *widget, gpointer data);
static void connect_data_type_signals(heraia_window_t *main_window);


/**
 *  Returns a new data_type filled with name and size values
 */
data_type_t *new_data_type(gchar *name, guint size)
{
	data_type_t *a_data_type = NULL;

	a_data_type = (data_type_t *) g_malloc0(sizeof(data_type_t));

	a_data_type->name = g_strdup(name);
	a_data_type->size = size;

	return a_data_type;
}


/**
 *  Clears data_type window's widgets and sets to default
 *  values.
 */
void clear_data_type_widgets(heraia_window_t *main_window)
{
	GtkEntry *entry = NULL;

	/* name */
	entry = GTK_ENTRY(glade_xml_get_widget(main_window->xml, "dt_name_entry"));
	gtk_entry_set_text(entry, "");

	/* size */
	entry = GTK_ENTRY(glade_xml_get_widget(main_window->xml, "dt_size_entry"));
	gtk_entry_set_text(entry, "");
}


/**
 *  Says wether name "name" is already in use in "data_type_list" list
 */
static gboolean is_data_type_name_already_used(GList *data_type_list, gchar *name)
{ 
	gboolean result = FALSE;
	data_type_t *one_data_type;

	while (data_type_list != NULL && result == FALSE)
		{
			one_data_type = (data_type_t *) data_type_list->data;
			
			if (g_ascii_strcasecmp(name, one_data_type->name) == 0)
				{
					result = TRUE;
				}
			else
				{
					data_type_list = data_type_list->next;
				}
		}

	return result;
}


/**
 *  Called when the data type window is killed or closed
 */
static gboolean delete_data_type_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{	
	heraia_window_t *main_window = (heraia_window_t *) data;

	gtk_widget_hide(glade_xml_get_widget(main_window->xml, "data_type_window"));

	return TRUE;
}


/**
 *  Called when the data type window is killed or closed
 */
static void destroy_data_type_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	gtk_widget_hide(glade_xml_get_widget(main_window->xml, "data_type_window"));
}

/**
 *  The user clicks "Ok" button
 */
static void dt_ok_button_clicked(GtkWidget *widget, gpointer data)
{
  heraia_window_t *main_window = (heraia_window_t *) data;
  GtkWidget *data_type_window = NULL;
  GtkEntry *name_entry = NULL;
  gchar *name = NULL;

  name_entry = GTK_ENTRY(glade_xml_get_widget(main_window->xml, "dt_name_entry"));
  name = gtk_entry_get_text(name_entry);

  data_type_window = glade_xml_get_widget(main_window->xml, "data_type_window");

  if (is_data_type_name_already_used(main_window->data_type_list, name) == FALSE)
	  {
		  /* The name does not exists, we want to add it to the treeview */
		  log_message(main_window, G_LOG_LEVEL_DEBUG, "Adding %s data type name to treeview", name);
		  add_data_type_name_to_treeview(main_window, name);
		  /* size ? */
		  main_window->data_type_list = g_list_prepend(main_window->data_type_list, new_data_type(name, 6));
		  gtk_widget_hide(data_type_window);
	  }
  else
	  {
		  /* the name is already in use, we assume that we are in editing mode */
		  /* TODO do the editing mode */
		  log_message(main_window, G_LOG_LEVEL_DEBUG, "Editing mode (%s exists)", name);
		  gtk_widget_hide(data_type_window);
	  }
}

/**
 *  The user clicks "Cancel" button
 */
static void dt_cancel_button_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	/* Here we do nothing as all gets cleared when Add or edit button are clicked */

	gtk_widget_hide(glade_xml_get_widget(main_window->xml, "data_type_window"));
}


/**
 *  Connects data_type's window signals.
 */
static void connect_data_type_signals(heraia_window_t *main_window)
{

  if (main_window != NULL && main_window->xml != NULL)
    {
      /* data type window killed or destroyed */
      g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "data_type_window")), "delete_event", 
		       G_CALLBACK(delete_data_type_window_event), main_window);
		  
      g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "data_type_window")), "destroy", 
		       G_CALLBACK(destroy_data_type_window), main_window);
      
      /* Ok button */
      g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "dt_ok_button")), "clicked", 
			G_CALLBACK(dt_ok_button_clicked), main_window);
      
      /* Cancel button */
      g_signal_connect (G_OBJECT(glade_xml_get_widget(main_window->xml, "dt_cancel_button")), "clicked", 
			G_CALLBACK(dt_cancel_button_clicked), main_window);
    }
}


/**
 *  Inits the data type window with default values
 *  Should be called only once
 */
void data_type_init_interface(heraia_window_t *main_window)
{
  if (main_window != NULL)
    {
      connect_data_type_signals(main_window);
    }
}

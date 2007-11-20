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

static gboolean delete_data_type_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_data_type_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void dt_ok_button_clicked(GtkWidget *widget, gpointer data);
static void dt_cancel_button_clicked(GtkWidget *widget, gpointer data);
static void connect_data_type_signals(heraia_window_t *main_window);
static data_type_t *retrieve_data_type_information(heraia_window_t *main_window);

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
 *  Frees the memory of a particular data_type
 */
void free_data_type(data_type_t *a_data_type)
{
	if (a_data_type != NULL)
		{
			if (a_data_type->name != NULL)
				{
					g_free(a_data_type->name);
				}

			g_free(a_data_type);
		}
}


/**
 *  Prints "length" "bin_datas" in an hexa form to the 
 *  entry widget named "widget_name"
 *  TODO : add some specific representation options such
 *         as space between characters
 */
static void print_bin_to_hex(heraia_window_t *main_window, gchar *widget_name, gchar *bin_datas, guint length)
{
	guint i = 0;
	GtkEntry *entry = GTK_ENTRY(glade_xml_get_widget(main_window->xml, widget_name));
	
	guchar *aux = NULL;
	guchar *hex = NULL;

	aux = (guchar *) g_malloc0(sizeof(gchar)*3);
	hex = (guchar *) g_malloc0(sizeof(guchar)*((length*2) + 1));

   for(i = 0; i < length; i++)  
    {
      sprintf((char *) aux, "%02x", bin_datas[i]); 
      memcpy(hex+(i*2), aux, 2);
    }

   hex[i*2+1] = '\0';
   g_free(aux);
  
   gtk_entry_set_text(entry, hex);

   g_free(hex);

}


/**
 *  Refreshes the "hex_datas_entry" which displays
 *  the hex datas extracted from the hexwidget at the
 *  cursor position. Endianness is Little Endian for now,
 *  we may, at a later time let the user choose its endianness
 */
void refresh_hex_datas_entry(heraia_window_t *main_window)
{
	guchar *bin_datas = NULL;
	data_window_t *data_window = main_window->current_DW;
	GtkSpinButton *dt_size_spinbutton = GTK_SPIN_BUTTON(glade_xml_get_widget(main_window->xml, "dt_size_spinbutton"));
	guint length = 0;
	gboolean result = FALSE;

	if (data_window->current_hexwidget != NULL)
		{
			length = gtk_spin_button_get_value_as_int(dt_size_spinbutton);
			
			bin_datas = (guchar *) g_malloc0(length);
			
			result = ghex_get_data(data_window, length, H_DI_LITTLE_ENDIAN, bin_datas);

			if (result == TRUE)
				{
					print_bin_to_hex(main_window, "hex_datas_entry", bin_datas, length);
				}

			g_free(bin_datas);
		}
}


/**
 *  Clears data_type window's widgets and sets to default
 *  values.
 */
void clear_data_type_widgets(heraia_window_t *main_window)
{
	GtkEntry *entry = NULL;
	GtkSpinButton *size_spin_button = NULL;
	gdouble min = 0.0;
	gdouble max = 0.0;

	/* name */
	entry = GTK_ENTRY(glade_xml_get_widget(main_window->xml, "dt_name_entry"));
	gtk_entry_set_text(entry, "");

	/* size */
	size_spin_button = GTK_SPIN_BUTTON(glade_xml_get_widget(main_window->xml, "dt_size_spinbutton"));
	gtk_spin_button_get_range(size_spin_button, &min, &max);
	gtk_spin_button_set_value(size_spin_button, min);

	refresh_hex_datas_entry(main_window);
}


/**
 *  Fills the data_type window's widgets with values taken from the
 *  data_type_list structure
 */
void fill_data_type_widgets(heraia_window_t *main_window, GList *data_type_list)
{
	GtkEntry *entry = NULL;
	GtkSpinButton *size_spin_button = NULL;
	gdouble min = 0.0;
	gdouble max = 0.0;
	data_type_t *a_data_type = (data_type_t *) data_type_list->data;

	entry = GTK_ENTRY(glade_xml_get_widget(main_window->xml, "dt_name_entry"));
	gtk_entry_set_text(entry, a_data_type->name);

	/* size */
	size_spin_button = GTK_SPIN_BUTTON(glade_xml_get_widget(main_window->xml, "dt_size_spinbutton"));
	gtk_spin_button_get_range(size_spin_button, &min, &max);

	if (a_data_type->size >= min && a_data_type->size <= max)
		{
			gtk_spin_button_set_value(size_spin_button, a_data_type->size);
		}
	else
		{ 
			if (a_data_type->size > max)
				{ /* May be we're close the end of a file */
					gtk_spin_button_set_value(size_spin_button, max);
				}
			else
				{ /* Fallback case */
					gtk_spin_button_set_value(size_spin_button, max);
				}
		}

	refresh_hex_datas_entry(main_window);
}


/**
 *  Says wether name "name" is already in use in "data_type_list" list
 *  and returns NULL if not or the list that contains the data_type_t 
 *  where name was found
 */
GList *is_data_type_name_already_used(GList *data_type_list, gchar *name)
{ 
	gboolean result = FALSE;
	data_type_t *one_data_type = NULL;

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

	if (result == TRUE)
		{
			return data_type_list;
		}
	else
		{
			return NULL;
		}
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
 *  Retrieves informations from the data_type window widgets
 */
static data_type_t *retrieve_data_type_information(heraia_window_t *main_window)
{
	GtkEntry *entry = NULL;  
	gchar *name = NULL;      
	gint size = 0;           
	GtkSpinButton *size_spin_button = NULL;  

	entry = GTK_ENTRY(glade_xml_get_widget(main_window->xml, "dt_name_entry"));
	name = gtk_entry_get_text(entry);  /* name should be const gchar * */

	size_spin_button = GTK_SPIN_BUTTON(glade_xml_get_widget(main_window->xml, "dt_size_spinbutton"));
	size = gtk_spin_button_get_value_as_int(size_spin_button);

	return new_data_type(name, size);
}


/**
 *  The user clicks "Ok" button
 */
static void dt_ok_button_clicked(GtkWidget *widget, gpointer data)
{
  heraia_window_t *main_window = (heraia_window_t *) data;
  GtkWidget *data_type_window = NULL;
  data_type_t *a_data_type = NULL;
  GList *data_type_list = NULL;
  
  a_data_type = retrieve_data_type_information(main_window);
	
  data_type_list = is_data_type_name_already_used(main_window->data_type_list, a_data_type->name);

  if (data_type_list == NULL)
	  {
		  /**
		   *  The name does not exists, we want to add it to the treeview 
		   */
		  log_message(main_window, G_LOG_LEVEL_DEBUG, "Adding %s data type name to treeview", a_data_type->name);
		  add_data_type_name_to_treeview(main_window, a_data_type->name);

		  main_window->data_type_list = g_list_prepend(main_window->data_type_list, a_data_type);
	  }
  else
	  {
		  /**
		   *  The name is already in use, we assume that we are in editing mode
		   *  and we simply replace the old data_type_t structure by the new one !
		   */
		  log_message(main_window, G_LOG_LEVEL_DEBUG, "Editing mode (%s exists)", a_data_type->name);
		  free_data_type((data_type_t *)data_type_list->data);
		  data_type_list->data = a_data_type;
	  }

  data_type_window = glade_xml_get_widget(main_window->xml, "data_type_window");
  gtk_widget_hide(data_type_window);
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
 *  When the user changes the size of the data that will
 *  be used to construct a new type
 */
static void dt_size_spinbutton_value_changed(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	if (main_window != NULL)
		{
			refresh_hex_datas_entry(main_window);
		}
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

	  /* When the size spinbutton changes its value */
	  g_signal_connect(G_OBJECT(glade_xml_get_widget(main_window->xml, "dt_size_spinbutton")), "value-changed", 
					   G_CALLBACK(dt_size_spinbutton_value_changed), main_window);
    }
}


/**
 *  Inits the data type window with default values
 *  Should be called only once
 *  Here we may load a specific file where the user 
 *  data types are saved
 */
void data_type_init_interface(heraia_window_t *main_window)
{
  if (main_window != NULL)
    {
      connect_data_type_signals(main_window);
    }
}

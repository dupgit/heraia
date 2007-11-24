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
/* static data_type_t *retrieve_data_type_information(heraia_window_t *main_window); */
static void set_spinbutton_max_range(heraia_window_t *main_window);

static void close_data_type_window(heraia_window_t *main_window);
static void destroy_container_widget(treatment_container_t *tment_c);
static treatment_container_t *new_treatment_container(heraia_window_t *main_window);
static void create_treatment_container_widget(heraia_window_t *main_window, treatment_container_t *tment_c);

/* treatment container widget signals */
static void add_treatment_container_widget(GtkWidget *widget, gpointer data);


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
 *  Copies the data_type_t structure in an new one
 *  That may be freely freed after use.
 */
data_type_t *copy_data_type_struct(data_type_t *a_data_type)
{
	if (a_data_type != NULL)
		{
			return new_data_type(a_data_type->name, a_data_type->size);
		}
	else
		{
			return NULL;
		}
}


/**
 *  Sets the maximum range of the spinbutton on the Data type Window 
 *  (usefull when we're next to the file's end) 
 *  This is done to ensure that the spinbutton always represent an
 *  exact and valid size 
 */
static void set_spinbutton_max_range(heraia_window_t *main_window)
{
	data_window_t *data_window = main_window->current_DW;
	guint64 till_end = 0;
	GtkSpinButton *size_spin_button = NULL;
	GtkHex *gh = NULL;

	gh = GTK_HEX(data_window->current_hexwidget);

	if (gh != NULL)
		{
			size_spin_button = GTK_SPIN_BUTTON(heraia_get_widget(main_window->xmls->main, "dt_size_spinbutton"));
			till_end = ghex_file_size(gh) - gtk_hex_get_cursor(gh);

			if (till_end >= DT_SPIN_MIN && till_end <= DT_SPIN_MAX)
				{
					gtk_spin_button_set_range(size_spin_button, DT_SPIN_MIN, till_end);
				}
			else
				{
					gtk_spin_button_set_range(size_spin_button, DT_SPIN_MIN, DT_SPIN_MAX);
				}
		}
}


/**
 *  Fills the treatment combobox with the available treatment list names 
 *  Set the active element on nothing (no active element)
 */
static void fill_treatment_combobox(GtkWidget *tment_list, GList *available_list)
{
	treatment_t *tment = NULL;

	while (available_list != NULL)
		{
			tment = (treatment_t *) available_list->data;
			if (tment->name != NULL)
				{
					gtk_combo_box_append_text(GTK_COMBO_BOX(tment_list), tment->name);
				}

			available_list = g_list_next(available_list);			
		}

	/* No active element */
	gtk_combo_box_set_active(GTK_COMBO_BOX(tment_list), -1);
}


/**
 *  Create a new empty treatment container
 */
static treatment_container_t *new_treatment_container(heraia_window_t *main_window)
{
	treatment_container_t *tment_c;
	
	tment_c = (treatment_container_t *) g_malloc0(sizeof(treatment_container_t));
	tment_c->treatment = NULL;

	/* Widget initialization */
	tment_c->container_box = gtk_hbox_new(FALSE, DT_BOX_SPACING);
	tment_c->combo_box = gtk_vbox_new(FALSE, DT_BOX_SPACING);
	tment_c->button_box = gtk_hbox_new(FALSE, DT_BOX_SPACING);
	tment_c->tment_list = gtk_combo_box_new_text();
	fill_treatment_combobox(tment_c->tment_list, main_window->available_treatment_list);

	tment_c->result = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(tment_c->result), FALSE);
	tment_c->moins = gtk_button_new_with_label(" - ");
	tment_c->plus = gtk_button_new_with_label(" + ");
	g_signal_connect(G_OBJECT(tment_c->plus), "clicked", 
					 G_CALLBACK(add_treatment_container_widget), main_window);

	/* Widget packing together */
	gtk_box_pack_start(GTK_BOX(tment_c->button_box), tment_c->moins, FALSE, FALSE, DT_BOX_PADDING);
	gtk_box_pack_start(GTK_BOX(tment_c->button_box), tment_c->plus, FALSE, FALSE, DT_BOX_PADDING);
	gtk_box_pack_start(GTK_BOX(tment_c->button_box), tment_c->result, FALSE, FALSE, DT_BOX_PADDING);
	gtk_box_pack_start(GTK_BOX(tment_c->combo_box), tment_c->tment_list, FALSE, FALSE, DT_BOX_PADDING);
	gtk_box_pack_end(GTK_BOX(tment_c->container_box), tment_c->button_box, FALSE, FALSE, DT_BOX_PADDING);
	gtk_box_pack_start(GTK_BOX(tment_c->container_box), tment_c->combo_box, FALSE, FALSE, DT_BOX_PADDING);

	return tment_c;
}


/**
 *  Packs the container widget to the main window widgets via "dt_treatment_vbox"
 */
static void create_treatment_container_widget(heraia_window_t *main_window, treatment_container_t *tment_c)
{
	GtkWidget *vbox = NULL;

	vbox = heraia_get_widget(main_window->xmls->main, "dt_treatment_vbox");

	gtk_box_pack_start(GTK_BOX(vbox), tment_c->container_box, FALSE, FALSE, DT_BOX_PADDING);

	if (tment_c->treatment != NULL)
		{
			/* TODO : Here we may call the method to init the treatment zone */
		}

}


/**
 *  Adds a treatment container widget when (+) button is clicked
 *  TODO : determine which treatment it is (where the + button was clicked)
 *  and insert the new one just after it (need to do the same in the list)
 */
static void add_treatment_container_widget(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	treatment_container_t *new_container = NULL;
	data_type_t *a_data_type = NULL;

	a_data_type = main_window->current_data_type;

	/* Creates a new treatment container */
	new_container = new_treatment_container(main_window);

	/* Create the last box that will contain one empty treatment container widget */
	create_treatment_container_widget(main_window, new_container);

	gtk_widget_show_all(new_container->container_box);

	/* Append the container to the list */
	a_data_type->treatment_c_list = g_list_append(a_data_type->treatment_c_list, (gpointer) new_container);
}


/**
 *  This constructs, fills and shows the data_type window
 *  First I thought doing things with libglade here, but it seems
 *  very hard. Same thing applies for GtkBuilder. I think I'll get
 *  more flexibility with widgets created on the fly
 */
void show_data_type_window(heraia_window_t *main_window, data_type_t *a_data_type)
{
	treatment_container_t *new_container = NULL;

	/* create all the treatment widgets */
	while (a_data_type->treatment_c_list != NULL)
		{
			/* TODO : Create the widgets with all the treatments */

		}
	
	/* Creates a new treatment container */
	new_container = new_treatment_container(main_window);

	/* Create the last box that will contain one empty treatment container widget */
	create_treatment_container_widget(main_window, new_container);

	/* Append the container to the list */
	a_data_type->treatment_c_list = g_list_append(a_data_type->treatment_c_list, (gpointer) new_container);


	fill_data_type_widgets(main_window, a_data_type); 
	set_spinbutton_max_range(main_window);

	gtk_widget_show_all(heraia_get_widget(main_window->xmls->main, "data_type_window"));
}


/**
 *  Prints "length" "bin_datas" in an hexa form to the 
 *  entry widget named "widget_name"
 *  TODO : add some specific representation options such
 *         as space between characters
 */
static void print_bin_to_hex(heraia_window_t *main_window, gchar *widget_name, guchar *bin_datas, guint length)
{
	guint i = 0;
	GtkEntry *entry = GTK_ENTRY(heraia_get_widget(main_window->xmls->main, widget_name));
	
	guchar *aux = NULL;
	guchar *hex = NULL;

	aux = (guchar *) g_malloc0(sizeof(guchar)*3);
	hex = (guchar *) g_malloc0(sizeof(guchar)*((length*2) + 1));

   for(i = 0; i < length; i++)  
    {
      sprintf((char *) aux, "%02x", bin_datas[i]); 
      memcpy(hex+(i*2), aux, 2);
    }

   hex[i*2+1] = '\0';
   g_free(aux);
  
   gtk_entry_set_text(entry, (gchar *) hex);

   g_free(hex);

}


/**
 *  Refreshes the "hex_datas_entry" which displays
 *  the hex datas extracted from the hexwidget at the
 *  cursor position. Endianness is Little Endian for now,
 *  we may, at a later time let the user choose its endianness
 *  it also places the spin widget value in the current_data_type
 *  structure.
 */
void refresh_hex_datas_entry(heraia_window_t *main_window)
{
	guchar *bin_datas = NULL;
	data_window_t *data_window = main_window->current_DW;
	GtkSpinButton *dt_size_spinbutton = GTK_SPIN_BUTTON(heraia_get_widget(main_window->xmls->main, "dt_size_spinbutton"));
	guint length = 0;
	gboolean result = FALSE;

	length = gtk_spin_button_get_value_as_int(dt_size_spinbutton);
			
	if (length >= DT_SPIN_MIN && length <= DT_SPIN_MAX)
		{
			if (main_window->current_data_type != NULL)
				{
					main_window->current_data_type->size = length;
				}

			if (data_window->current_hexwidget != NULL)
				{
					bin_datas = (guchar *) g_malloc0(length);
					result = ghex_get_data(data_window, length, H_DI_LITTLE_ENDIAN, bin_datas);

					if (result == TRUE)
						{
							print_bin_to_hex(main_window, "hex_datas_entry", bin_datas, length);
						}

					g_free(bin_datas);
				}
		}
}


/**
 *  Clears data_type window's widgets and sets to default
 *  values. Currently UNUSED
 */
/*
void clear_data_type_widgets(heraia_window_t *main_window)
{
	GtkEntry *entry = NULL;
	GtkSpinButton *size_spin_button = NULL;
	gdouble min = 0.0;
	gdouble max = 0.0;

	entry = GTK_ENTRY(heraia_get_widget(main_window->xmls->main, "dt_name_entry"));
	gtk_entry_set_text(entry, "");

	size_spin_button = GTK_SPIN_BUTTON(heraia_get_widget(main_window->xmls->main, "dt_size_spinbutton"));
	gtk_spin_button_get_range(size_spin_button, &min, &max);
	gtk_spin_button_set_value(size_spin_button, min);

	refresh_hex_datas_entry(main_window);
}
*/


/**
 *  Fills the data_type window's widgets with values taken from the
 *  data_type_list structure
 */
void fill_data_type_widgets(heraia_window_t *main_window, data_type_t *a_data_type)
{
	GtkEntry *entry = NULL;
	GtkSpinButton *size_spin_button = NULL;
	gdouble min = 0.0;
	gdouble max = 0.0;

	/* name */
	entry = GTK_ENTRY(heraia_get_widget(main_window->xmls->main, "dt_name_entry"));
	gtk_entry_set_text(entry, a_data_type->name);

	/* size */
	size_spin_button = GTK_SPIN_BUTTON(heraia_get_widget(main_window->xmls->main, "dt_size_spinbutton"));
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

	close_data_type_window(main_window);

	return TRUE;
}


/**
 *  Called when the data type window is killed or closed
 */
static void destroy_data_type_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	close_data_type_window(main_window);
}


/**
 *  Retrieves informations from the data_type window widgets
 *  Currently UNUSED
 */
/*
static data_type_t *retrieve_data_type_information(heraia_window_t *main_window)
{
	GtkEntry *entry = NULL;  
	gchar *name = NULL;      
	gint size = 0;           
	GtkSpinButton *size_spin_button = NULL;  

	entry = GTK_ENTRY(heraia_get_widget(main_window->xmls->main, "dt_name_entry"));
	name = gtk_entry_get_text(entry); 

	size_spin_button = GTK_SPIN_BUTTON(heraia_get_widget(main_window->xmls->main, "dt_size_spinbutton"));
	size = gtk_spin_button_get_value_as_int(size_spin_button);

	return new_data_type(name, size);
}
*/

/**
 *  Destroys a container widget
 */
static void destroy_container_widget(treatment_container_t *tment_c)
{
	gtk_widget_destroy(tment_c->plus);
	gtk_widget_destroy(tment_c->moins);
	gtk_widget_destroy(tment_c->result);
	gtk_widget_destroy(tment_c->tment_list);
	gtk_widget_destroy(tment_c->button_box);
	gtk_widget_destroy(tment_c->combo_box);
	gtk_widget_destroy(tment_c->container_box);

	if (tment_c->treatment != NULL)
		{
			/* Here we must also destroy the treatment */
		}

	g_free(tment_c);
}

/**
 *  Destroys the last container widget (which should always be empty) 
 */
static void close_data_type_window(heraia_window_t *main_window)
{
	data_type_t *a_data_type = NULL; /* data type currently in use */
	GList *last; /* last element of a list */

	/* Removing the last widget from the list and the window   */
	/* TODO : Here we should destroy all the container widgets */
	a_data_type = main_window->current_data_type;
	last = g_list_last(a_data_type->treatment_c_list);
	destroy_container_widget((treatment_container_t *)last->data);
	a_data_type->treatment_c_list  = g_list_delete_link(a_data_type->treatment_c_list, last);

	/* Hiding the window */
	gtk_widget_hide(heraia_get_widget(main_window->xmls->main, "data_type_window"));
}

/**
 *  The user clicks "Ok" button
 */
static void dt_ok_button_clicked(GtkWidget *widget, gpointer data)
{
  heraia_window_t *main_window = (heraia_window_t *) data;
  data_type_t *a_data_type = NULL;
  GList *data_type_list = NULL;
  
  a_data_type = main_window->current_data_type;
	
  if (g_ascii_strcasecmp(a_data_type->name, "") != 0)
	  {
		  data_type_list = is_data_type_name_already_used(main_window->data_type_list, a_data_type->name);

		  if (data_type_list == NULL)
			  {
				  /**
				   *  The name does not exists, we want to add it to the treeview and to the
				   *  list structure (the user clicked add(+) in the list window or edited the name)
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

		  close_data_type_window(main_window);
 
	  }
}


/**
 *  The user clicks "Cancel" button
 */
static void dt_cancel_button_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	close_data_type_window(main_window);
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
 *  When the user leaves the name entry 
 *  Gets the name entry if any and puts it in the current_data_type
 *  structure. 
 */
static void dt_name_entry_leave_notify_event(GtkWidget *widget, GdkEventCrossing *event, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkEntry *entry = NULL;  

	if (main_window != NULL && main_window->current_data_type != NULL)
		{
			entry = GTK_ENTRY(heraia_get_widget(main_window->xmls->main, "dt_name_entry"));
			
			if (main_window->current_data_type->name != NULL)
				{
					g_free(main_window->current_data_type->name);
				}

			main_window->current_data_type->name = g_strdup(gtk_entry_get_text(entry));
		}
}



/**
 *  Connects data_type's window signals.
 */
static void connect_data_type_signals(heraia_window_t *main_window)
{

  if (main_window != NULL && main_window->xmls != NULL && main_window->xmls->main != NULL)
    {
      /* data type window killed or destroyed */
      g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "data_type_window")), "delete_event", 
		       G_CALLBACK(delete_data_type_window_event), main_window);
		  
      g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "data_type_window")), "destroy", 
		       G_CALLBACK(destroy_data_type_window), main_window);
      
      /* Ok button */
      g_signal_connect (G_OBJECT(heraia_get_widget(main_window->xmls->main, "dt_ok_button")), "clicked", 
			G_CALLBACK(dt_ok_button_clicked), main_window);
      
      /* Cancel button */
      g_signal_connect (G_OBJECT(heraia_get_widget(main_window->xmls->main, "dt_cancel_button")), "clicked", 
			G_CALLBACK(dt_cancel_button_clicked), main_window);

	  /* When the size spinbutton changes its value */
	  g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "dt_size_spinbutton")), "value-changed", 
					   G_CALLBACK(dt_size_spinbutton_value_changed), main_window);
	  
	  /* When the user leaves the name entry */
	  g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "dt_name_entry")), "leave-notify-event", 
					   G_CALLBACK(dt_name_entry_leave_notify_event), main_window);

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

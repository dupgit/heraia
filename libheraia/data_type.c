/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  data_types.c
  Window allowing the user to create or edit a new data type
  
  (C) Copyright 2007 - 2009 Olivier Delhomme
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
/**
 * @file data_type.c
 * This file must not be edited unless you know what you are
 * doing : the main maintainer wants to destroy it in a near
 * future !
 */
#include <libheraia.h>

static gboolean delete_data_type_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_data_type_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void dt_ok_button_clicked(GtkWidget *widget, gpointer data);
static void dt_cancel_button_clicked(GtkWidget *widget, gpointer data);
static void connect_data_type_signals(heraia_window_t *main_window);
/* static data_type_t *retrieve_data_type_information(heraia_window_t *main_window); */
static void set_spinbutton_max_range(heraia_window_t *main_window);

static void close_data_type_window(heraia_window_t *main_window);
static void destroy_container_widget(treatment_container_t *tment_c, gboolean all);
static treatment_container_t *new_treatment_container(heraia_window_t *main_window);
static void create_treatment_container_widget(heraia_window_t *main_window, treatment_container_t *tment_c);
static guchar *print_bin_to_hex(GtkWidget *entry, GList *values_list);

/* treatment container widget signals */
static void add_treatment_container_widget(GtkWidget *widget, gpointer data);
static void remove_treatment_container_widget(GtkWidget *widget, gpointer data);
static void cb_changed_in_treatment_container_widget(GtkWidget *widget, gpointer data);

/**
 *  Returns a new data_type filled with name and size values
 */
data_type_t *new_data_type(gchar *name, guint size)
{
	data_type_t *a_data_type = NULL;

	a_data_type = (data_type_t *) g_malloc0(sizeof(data_type_t));

	if (name != NULL)
		{
			a_data_type->name = g_strdup(name);
		}
	else
		{
			a_data_type->name = NULL;
		}

	a_data_type->size = size;

	/* Data interpretor widgets */
	a_data_type->di_label = NULL;
	a_data_type->di_entry = NULL;

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
 *  Copies the data_type_t structure in a new one,
 *  that may be freely freed after use.
 *  main_window needed to fill the combobox widget with available treatment list
 */
data_type_t *copy_data_type_struct(heraia_window_t *main_window, data_type_t *a_data_type)
{
	data_type_t *data_type = NULL;
	treatment_container_t *tment_c = NULL;
	GList *list = NULL;
	GList *list2 = NULL;

	if (a_data_type != NULL)
		{
			data_type = new_data_type(a_data_type->name, a_data_type->size);
			list = a_data_type->treatment_c_list;

			while (list != NULL)
				{
					/**
					 *  Here we do not copy the widgets as they will be created
					 *  on the fly. We copy the list and the treatments
					 */
					tment_c = new_treatment_container(main_window);
					tment_c->treatment = copy_treatment(((treatment_container_t *)list->data)->treatment);
					list2 = g_list_append(list2, tment_c);				   
					list = g_list_next(list);
				}

			data_type->treatment_c_list = list2;

			/* data interpretor widget creation */
			create_ud_data_interpretor_widgets(main_window, data_type);
			gtk_label_set_text(GTK_LABEL(data_type->di_label), gtk_label_get_text(GTK_LABEL(a_data_type->di_label)));
		}
  
	return data_type;
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
 *  Tries to set the named "name" active item of the combobox list.
 */
static void set_active_text_item(GtkWidget *combo_box, gchar *name)
{
	GtkTreeIter iter;
	GtkTreeModel *model = NULL;
	gchar *list_name = NULL;
	gboolean ok = TRUE;
	gboolean stop = FALSE;

	if (name != NULL)
		{

			model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo_box));
	
			ok = gtk_tree_model_get_iter_first(model, &iter);
	
			while (ok == TRUE && stop == FALSE)
				{
					gtk_tree_model_get(model, &iter, 0, &list_name, -1);
					if (g_ascii_strcasecmp(list_name, name) == 0)
						{
							stop = TRUE;
						}
					else
						{
							ok = gtk_tree_model_iter_next(model, &iter);
						}
				}

			if (stop == TRUE)
				{
					gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combo_box), &iter);
				}
		}
}


/**
 *  Create a new empty treatment container
 *  Needs main_window structure to fill the combobox with available
 *  treatments
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
	g_signal_connect(G_OBJECT(tment_c->moins), "clicked", 
					 G_CALLBACK(remove_treatment_container_widget), main_window);
	g_signal_connect(G_OBJECT(tment_c->tment_list), "changed",
					 G_CALLBACK(cb_changed_in_treatment_container_widget), main_window);

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
 *  Packs the container widget to the main window widgets via "dt_treatment_vbox".
 */
static void create_treatment_container_widget(heraia_window_t *main_window, treatment_container_t *tment_c)
{
	GtkWidget *vbox = NULL;

	vbox = heraia_get_widget(main_window->xmls->main, "dt_treatment_vbox");

	gtk_box_pack_start(GTK_BOX(vbox), tment_c->container_box, FALSE, FALSE, DT_BOX_PADDING);

	if (tment_c->treatment != NULL) 
		{
			/* Here we call the method to init the treatment zone */
			tment_c->treatment->init(tment_c->treatment);
			set_active_text_item(tment_c->tment_list, tment_c->treatment->name);
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

	/* Update the whole treatment container list + data interpretor window*/
	refresh_hex_datas_entry(main_window);
}

/**
 *  Finds the container which contains the specified widget
 */
static GList *find_treatment_container_from_widget(GList *container_list, GtkWidget *widget)
{
	gboolean stop = FALSE;
	treatment_container_t *tment_c = NULL;

	while (container_list != NULL && stop == FALSE)
		{
			tment_c = (treatment_container_t *) container_list->data;
			if (tment_c->moins == widget || tment_c->tment_list == widget)
				{
					stop = TRUE;
				}
			else
				{
					container_list = g_list_next(container_list);
				}
		}

	return container_list;
}


/**
 *  Removes a treatment container widget when (-) button is clicked
 */
static void remove_treatment_container_widget(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	data_type_t *a_data_type = NULL;
	treatment_container_t *tment_c = NULL;
	GList *list = NULL;

	a_data_type = main_window->current_data_type;
	list = find_treatment_container_from_widget(a_data_type->treatment_c_list, widget);
	if (list != NULL)
		{
			tment_c = (treatment_container_t *) list->data;

			/* Removes the container completely */
			destroy_container_widget(tment_c, TRUE);

			/* Removes the treatment container from the list */
			a_data_type->treatment_c_list = g_list_delete_link(a_data_type->treatment_c_list, list);

			/* Update the whole treatment container list + data interpretor window*/
			refresh_hex_datas_entry(main_window);
		}
}

/**
 *  Updates the treatment_container list entries
 */
static guchar *update_treatment_container_list_entries(GList *tment_c_list, GList *values_list)
{
	treatment_container_t *tment_c = NULL;
	guchar *final = NULL;

	while (tment_c_list != NULL)
		{
			tment_c = (treatment_container_t *) tment_c_list->data;
			
			if (tment_c->treatment != NULL && tment_c->treatment->do_it != NULL)
				{
					values_list = tment_c->treatment->do_it(values_list);
					final = print_bin_to_hex(tment_c->result, values_list);
				}

			tment_c_list = g_list_next(tment_c_list);
		}

	return final;
}


/**
 *  Called when a change is done in the treatment list combobox.
 *  Affects the new treatment to the treatment_container.
 */
static void cb_changed_in_treatment_container_widget(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	data_type_t *a_data_type = NULL;
	treatment_container_t *tment_c = NULL;
	treatment_t *tment = NULL;
	gchar *tment_name = NULL;
	GList *list = NULL;

	/* Retreiving the treatment container concerned by the changed signal */
	a_data_type = main_window->current_data_type;
	list = find_treatment_container_from_widget(a_data_type->treatment_c_list, widget);
	if (list != NULL)
		{
			tment_c = (treatment_container_t *) list->data;

			/* Retrieving the treatment name selected */
			tment_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(tment_c->tment_list));

			/* Retrieves the treatment (with struct an such) from the treatment name */
			tment = find_treatment(main_window->available_treatment_list, tment_name);
	
			if (tment != NULL)
				{
					/* TODO : kill any previous treatment */

					tment_c->treatment = copy_treatment(tment);
		
					/* Update the whole treatment container list + data interpretor window*/
					refresh_hex_datas_entry(main_window);
				}
		}
}

/**
 *  Creates new user defined widgets for the data interpretor window.
 */
void create_ud_data_interpretor_widgets(heraia_window_t *main_window, data_type_t *a_data_type)
{
	GtkWidget *vbox = NULL;

	/* Creates new widgets for the data interpretor window */
	a_data_type->di_label = gtk_label_new(NULL);
	gtk_misc_set_padding(GTK_MISC(a_data_type->di_label), 4, 4);
	gtk_misc_set_alignment(GTK_MISC(a_data_type->di_label), 0.5, 0.5);

	a_data_type->di_entry = gtk_entry_new();

	vbox = heraia_get_widget(main_window->xmls->main, "ud_type_vbox");
	gtk_box_pack_start(GTK_BOX(vbox), a_data_type->di_label, FALSE, FALSE, DT_BOX_PADDING);
	
	vbox = heraia_get_widget(main_window->xmls->main, "ud_value_vbox");
	gtk_box_pack_start(GTK_BOX(vbox), a_data_type->di_entry, FALSE, FALSE, DT_BOX_PADDING);

	gtk_widget_show(a_data_type->di_label);
	gtk_widget_show(a_data_type->di_entry);
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
	GList *list = NULL;

	/* create all the treatment widgets */
	list = a_data_type->treatment_c_list;
	
	if (list == NULL) /* Creates only the first treatment_container */
		{
			log_message(main_window, G_LOG_LEVEL_DEBUG, "Creating a new treatment container");

			/* Creates a new empty treatment container */
			new_container = new_treatment_container(main_window);

			/* Create the last box that will contain one empty treatment container widget */
			create_treatment_container_widget(main_window, new_container); 

			/* Append the container to the list */
			a_data_type->treatment_c_list = g_list_append(a_data_type->treatment_c_list, (gpointer) new_container);
		}
	else  /* Create the widgets with all the treatments */
		{
			while (list != NULL)
				{
					
					new_container = (treatment_container_t *) list->data;

					log_message(main_window, G_LOG_LEVEL_DEBUG, "Creating a treatment container");

					create_treatment_container_widget(main_window, new_container);
					gtk_widget_show_all(new_container->container_box);

					list = g_list_next(list);
				}
		}

	/* fills widgets that are on top of the window (name, size and hexentry) */
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
static guchar *print_bin_to_hex(GtkWidget *entry, GList *values_list)
{
	guint i = 0;
	guchar *aux = NULL;
	guchar *hex = NULL;
	guchar *final = NULL;
	value_t *a_value = NULL;

	while (values_list != NULL)
		{
			a_value = (value_t *) values_list->data;

			aux = (guchar *) g_malloc0(sizeof(guchar) * 3);
			hex = (guchar *) g_malloc0(sizeof(guchar) * ((a_value->length * 2) + 1) );

			for(i = 0; i < a_value->length; i++)  
				{
					sprintf((char *) aux, "%02x", a_value->bytes[i]); 
					memcpy(hex+(i*2), aux, 2);
				}

			hex[i*2+1] = '\0';
			g_free(aux);

			if (final == NULL)
				{
					final = (guchar *) g_strdup((gchar *) hex);
				}
			else
				{
					aux = final;
					final = (guchar *) g_strconcat((gchar *) aux, " / ", (gchar *) hex, NULL);
					g_free(aux);
				}

			g_free(hex);

			values_list = g_list_next(values_list);
		}
	
	if (entry != NULL)
		{
			gtk_entry_set_text(GTK_ENTRY(entry), (gchar *) final);
		}

	return final;
}


/**
 *  Refreshes one user defined data type when the cursor is moved
 *  in the gtkhex structure.
 */
static void refresh_one_ud_data_interpretor(data_type_t *a_data_type, value_t *a_value)
{
	treatment_container_t *tment_c = NULL; /* one treatment container  */
	GList *tment_c_list = NULL;            /* treatment container list */
	guchar *final = NULL;                  /* final result             */
	GList *values_list = NULL;

	values_list = g_list_append(values_list, (gpointer) a_value);

	if (a_data_type != NULL)
		{
			tment_c_list = a_data_type->treatment_c_list;

			while (tment_c_list != NULL)
				{
					tment_c = (treatment_container_t *) tment_c_list->data;
			
					if (tment_c->treatment != NULL && tment_c->treatment->do_it != NULL)
						{
							values_list = tment_c->treatment->do_it(values_list);	
						}

					tment_c_list = g_list_next(tment_c_list);
				}

			final = print_bin_to_hex(a_data_type->di_entry, values_list);
			g_free(final);
		}
}


/**
 *  Refreshes all the user data defined types (called when the cursor is moved)
 *  Interpretation is done following the endianness 'endianness'
 */
void refresh_all_ud_data_interpretor(heraia_window_t *main_window, guint endianness)
{
	data_window_t *data_window = main_window->current_DW;
	value_t *a_value = NULL;
	guchar *bin_datas = NULL;
	GList *data_type_list = NULL;
	data_type_t *a_data_type = NULL;
	guint length = 0;
	gboolean result = FALSE;
	gchar *text = NULL;
			
	data_type_list = main_window->data_type_list;

	while (data_type_list != NULL)
		{
			a_data_type = (data_type_t *) data_type_list->data;

			if (a_data_type != NULL)
				{
					length = a_data_type->size;
					bin_datas = (guchar *) g_malloc0(sizeof(guchar) * length);
					result = ghex_get_data(data_window, length, endianness, bin_datas);

					if (result == TRUE)
						{
							a_value = new_value_t(length, bin_datas);
							refresh_one_ud_data_interpretor(a_data_type, a_value);
						}
					else
						{
							text = g_strdup_printf("Cannot copy %d bytes in order to interpret them !", length);
							gtk_entry_set_text(GTK_ENTRY(a_data_type->di_entry), text);
							g_free(text);
						}
					g_free(bin_datas);
				}

			data_type_list = g_list_next(data_type_list);
		}
		
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
	guint length = 0;
	gboolean result = FALSE;
	GtkWidget *entry = NULL;
	value_t *a_value = NULL;
	GList *values_list = NULL;
	GList *tment_c_list = NULL;
	guchar *final = NULL;

	length = main_window->current_data_type->size;

	if (data_window->current_hexwidget != NULL)
		{
			bin_datas = (guchar *) g_malloc0(sizeof(guchar) * length);
			result = ghex_get_data(data_window, length, H_DI_LITTLE_ENDIAN, bin_datas);

			if (result == TRUE)
				{
					entry = heraia_get_widget(main_window->xmls->main, "hex_datas_entry");
					a_value = new_value_t(length, bin_datas);
					values_list = g_list_append(values_list, (gpointer) a_value);

					print_bin_to_hex(entry, values_list);

					/* Follows the others for the data_type beiing edited */
					tment_c_list = main_window->current_data_type->treatment_c_list;
					final = update_treatment_container_list_entries(tment_c_list, values_list);
							
					/* data interpretor part */
					if (final != NULL)
						{
							gtk_entry_set_text(GTK_ENTRY(main_window->current_data_type->di_entry), (gchar *) final);
						}
				}

			g_free(bin_datas);
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
 *  Destroys a container Widget.
 *  If all is true it destroys everything even the treatment and such.
 *  If all is false it destroys only the widget part.
 */
static void destroy_container_widget(treatment_container_t *tment_c, gboolean all)
{
	if (tment_c != NULL)
		{
			destroy_a_single_widget(tment_c->plus);	
			tment_c->plus = NULL;

			destroy_a_single_widget(tment_c->moins); 
			tment_c->moins = NULL;
			
			destroy_a_single_widget(tment_c->result); 
			tment_c->result = NULL;

			destroy_a_single_widget(tment_c->tment_list); 
			tment_c->tment_list = NULL;

			destroy_a_single_widget(tment_c->button_box); 
			tment_c->button_box = NULL;

			destroy_a_single_widget(tment_c->combo_box); 
			tment_c->combo_box = NULL;

			destroy_a_single_widget(tment_c->container_box); 
			tment_c->container_box = NULL;

			if (all == TRUE)
				{

					if (tment_c->treatment != NULL && tment_c->treatment->kill != NULL)
						{
							tment_c->treatment->kill(tment_c->treatment); /* erases itself */
						}
					
					g_free(tment_c);
				}
		}
}

/**
 *  Called when the data_type_window is closed
 */
static void close_data_type_window(heraia_window_t *main_window)
{
	data_type_t *a_data_type = NULL; /* data type currently in use */
	GList *list = NULL;

	/* Current data type beeing edited */
	a_data_type = main_window->current_data_type; 

	/* Current treatment container list for this data type */
	list = a_data_type->treatment_c_list;
	
	/* Destroys all widgets associated with the treatment container list */
	/* This does not destroys the list itself here */
	while (list != NULL)
		{
			destroy_container_widget((treatment_container_t *)list->data, FALSE);
			list = g_list_next(list);
		}

	/** Last box stuff : I do not know if this is really usefull !!                                   */
	/* Destroys the last widget (which should never contain any treatment)                            */
	/* list = g_list_last(a_data_type->treatment_c_list);                                             */
	/* if (list != NULL)                                                                              */
	/*	{                                                                                             */
	/*		destroy_container_widget((treatment_container_t *)list->data, TRUE);                      */
	/*		a_data_type->treatment_c_list  = g_list_delete_link(a_data_type->treatment_c_list, list); */
	/*	}                                                                                             */
	
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

				  /* data interpretor part */
				  gtk_label_set_text(GTK_LABEL(a_data_type->di_label), a_data_type->name);

				  main_window->data_type_list = g_list_prepend(main_window->data_type_list, a_data_type);
			  }
		  else
			  {
				  /**
				   *  The name is already in use, we assume that we are in editing mode
				   *  and we simply replace the old data_type_t structure by the new one !
				   */
				  log_message(main_window, G_LOG_LEVEL_DEBUG, "Editing mode (%s exists)", a_data_type->name);
				  
				  /* We kill the old widget's that are unsefull (we replace them) */
				  destroy_a_single_widget(main_window->current_data_type->di_label);
				  destroy_a_single_widget(main_window->current_data_type->di_entry);
				  destroy_a_single_widget(((data_type_t *)data_type_list->data)->di_label);
				  destroy_a_single_widget(((data_type_t *)data_type_list->data)->di_entry);

				  free_data_type((data_type_t *)data_type_list->data);
				  create_ud_data_interpretor_widgets(main_window, a_data_type);

				  /* data interpretor part */
				  gtk_label_set_text(GTK_LABEL(a_data_type->di_label), a_data_type->name);

				  data_type_list->data = a_data_type;
			  }

		  refresh_hex_datas_entry(main_window);

		  close_data_type_window(main_window);
	  }
}


/**
 *  The user clicks "Cancel" button
 */
static void dt_cancel_button_clicked(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	data_type_t *a_data_type = NULL;

	a_data_type = main_window->current_data_type;

	/* the data interpretor part */
	destroy_a_single_widget(a_data_type->di_label);
	destroy_a_single_widget(a_data_type->di_entry);
   
	close_data_type_window(main_window);
}


/**
 *  When the user changes the size of the data that will
 *  be used to construct a new type
 */
static void dt_size_spinbutton_value_changed(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	GtkSpinButton *dt_size_spinbutton = GTK_SPIN_BUTTON(heraia_get_widget(main_window->xmls->main, "dt_size_spinbutton"));
	guint length = 0;

	if (main_window != NULL)
		{

		length = gtk_spin_button_get_value_as_int(dt_size_spinbutton);
			
		if (length >= DT_SPIN_MIN && length <= DT_SPIN_MAX)
			{
				if (main_window->current_data_type != NULL)
					{
						main_window->current_data_type->size = length;
					}	
			}

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

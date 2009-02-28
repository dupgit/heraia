/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  data_interpretor.c
  heraia - an hexadecimal file editor and analyser based on ghex

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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */
/**
 * @file data_interpretor.c
 * Here one may find tools to manage the data_interpretor window
 */
#include <libheraia.h>

static guint which_endianness(heraia_window_t *main_window);
static void interpret(heraia_window_t *main_window, DecodeFunc decode_it, gpointer data_struct, gchar *widget_name, guint length, guint endianness);
static void close_data_interpretor_window(GtkWidget *widget, gpointer data);
static void connect_data_interpretor_signals(heraia_window_t *main_window);

/**
 * @fn guint which_endianness(heraia_window_t *main_window)
 *  Determines which endianness is selected that is to say
 *  which radio button is active in the window
 * @param main_window : main structure
 * @return Something of the following, depending on what selected the user :
 *         - H_DI_LITTLE_ENDIAN for little endian encoding (default answer)
 *         - H_DI_BIG_ENDIAN for big endian encoding
 *         - H_DI_MIDDLE_ENDIAN for middle endian encoding
 */
static guint which_endianness(heraia_window_t *main_window)
{
	GtkRadioButton *rb = GTK_RADIO_BUTTON(heraia_get_widget(main_window->xmls->main, "diw_rb_little_endian"));
	GtkWidget *activated = NULL;
	const gchar *widget_name = NULL;

	activated = gtk_radio_button_get_active_from_widget(rb);
	widget_name = gtk_widget_get_name(activated);

	if (g_ascii_strcasecmp(widget_name, "diw_rb_little_endian") == 0)
		{
			return H_DI_LITTLE_ENDIAN;
		}
	else if (g_ascii_strcasecmp(widget_name, "diw_rb_big_endian") == 0)
		{
			return H_DI_BIG_ENDIAN;
		}
	else if (g_ascii_strcasecmp(widget_name, "diw_rb_middle_endian") == 0)
		{
			return H_DI_MIDDLE_ENDIAN;
		}
	else
		return H_DI_LITTLE_ENDIAN;  /* default interpretation case */
}

/**
 *   Here we do interpret a number according to the decode_it function and we 
 *   write down the result in a widget name named "widget_name".
 *  @warning We are assuming that main_window != NULL and main_window->xml != NULL
 *
 *    @param main_window : main structure
 *    @param decode_it : a DecodeDateFunc which is a function to be called to 
 *				 decode the stream
 *    @param widget_name : a gchar * containing the name of the widget where 
 *				 the result may go
 *    @param length : the length of the data to be decoded (guint)
 *    @param endianness : the endianness to be applied to the datas (as 
 * 			 returned by function which_endianness)
 */
static void interpret(heraia_window_t *main_window, DecodeFunc decode_it, gpointer data_struct, gchar *widget_name, guint length, guint endianness)
{
	gint result = 0;       /**< used to test different results of function calls */
	guchar *c = NULL;      /**< the character under the cursor                   */
	gchar *text = NULL;
	data_window_t *data_window = main_window->current_DW;   /**< We already know that it's not NULL (we hope so) */
	GtkWidget *entry = heraia_get_widget(main_window->xmls->main, widget_name);  /**< @todo we might test the result as this is user input */

	c = (guchar *) g_malloc0(sizeof(guchar) * length);

	result = ghex_get_data(data_window, length, endianness, c);

	if (result == TRUE)
		{
			text = decode_it(c, data_struct);

			if (text != NULL)
				{
					gtk_entry_set_text(GTK_ENTRY(entry), text);
				}
			else
				{
					text = g_strdup_printf("Something's wrong!");
					gtk_entry_set_text(GTK_ENTRY(entry), text);
				}
		}
	else
		{
			text = g_strdup_printf("Cannot interpret as a %d byte(s) number", length);
			gtk_entry_set_text(GTK_ENTRY(entry), text);
		}

	g_free(c);
  	g_free(text);
}


/**
 * @fn void close_data_interpretor_window(GtkWidget *widget, gpointer data)
 *  "Emulates" the user click on the main window menu entry called DIMenu
 *  whose aim is to display or hide the data interpretor window
 * @param widget : the widget caller (may be NULL here)
 * @param data : a gpointer to the main structure : main_window, this must NOT
 *        be NULL !
 */
static void close_data_interpretor_window(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	if (main_window != NULL && main_window->xmls != NULL  && main_window->xmls->main)
		{
			g_signal_emit_by_name(heraia_get_widget(main_window->xmls->main, "DIMenu"), "activate");
		}
}

/**
 * @fn void refresh_data_interpretor_window(GtkWidget *widget, gpointer data)
 *  Refreshes the data interpretor window with the new values
 * @param widget : the widget caller (may be NULL here)
 * @param data : a gpointer to the main structure : main_window, this must NOT
 *        be NULL !
 */
void refresh_data_interpretor_window(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;  /**< data interpretor window structure */
	guint endianness = 0;

	if (main_window != NULL && main_window->current_DW != NULL && main_window->win_prop->main_dialog->displayed == TRUE)
		{
			endianness = which_endianness(main_window);  /**< Endianness is computed only once here */
			interpret(main_window, decode_8bits_unsigned, NULL, "diw_8bits_us", 1, endianness);
			interpret(main_window, decode_8bits_signed, NULL, "diw_8bits_s", 1, endianness);
			interpret(main_window, decode_16bits_unsigned, NULL, "diw_16bits_us", 2, endianness);
			interpret(main_window, decode_16bits_signed, NULL, "diw_16bits_s", 2, endianness);
			interpret(main_window, decode_32bits_unsigned, NULL, "diw_32bits_us", 4, endianness);
			interpret(main_window, decode_32bits_signed, NULL, "diw_32bits_s", 4, endianness);
			interpret(main_window, decode_64bits_unsigned, NULL, "diw_64bits_us", 8, endianness);
			interpret(main_window, decode_64bits_signed, NULL, "diw_64bits_s", 8, endianness);
			
			interpret(main_window, decode_C_date, NULL, "diw_C_date", 4, endianness);
			interpret(main_window, decode_dos_date, NULL, "diw_msdos_date", 4, endianness);
			interpret(main_window, decode_filetime_date, NULL, "diw_filetime_date", 8, endianness);
			interpret(main_window, decode_HFS_date, NULL, "diw_HFS_date", 4, endianness);
			
			interpret(main_window, decode_to_bits, NULL, "diw_base_bits", 1, endianness);
			interpret(main_window, decode_packed_BCD, NULL, "diw_base_bcd", 1, endianness);

			refresh_all_ud_data_interpretor(main_window, endianness);
		}
}


/**
 * @fn void connect_data_interpretor_signals(heraia_window_t *main_window)
 *  Connects data interpretor window's signals to the
 *  right functions
 * @param main_window : main structure
 */
static void connect_data_interpretor_signals(heraia_window_t *main_window)
{
	/* When data interpretor's window is killed or destroyed */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "data_interpretor_window")), "delete_event",
						  G_CALLBACK(delete_dt_window_event), main_window);

	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "data_interpretor_window")), "destroy",
						  G_CALLBACK(destroy_dt_window), main_window);

	/* Menu "close" */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "diw_close_menu")), "activate",
						  G_CALLBACK(close_data_interpretor_window), main_window);

	/* Radio Button "Little Endian" */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "diw_rb_little_endian")), "toggled",
						  G_CALLBACK(refresh_data_interpretor_window), main_window);

	/* Radio Button "Big Endian" */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "diw_rb_big_endian")), "toggled",
						  G_CALLBACK(refresh_data_interpretor_window), main_window);

	/* Radio Button "Middle Endian" */
	g_signal_connect(G_OBJECT(heraia_get_widget(main_window->xmls->main, "diw_rb_middle_endian")), "toggled",
						  G_CALLBACK(refresh_data_interpretor_window), main_window);
}

/**
 * @fn void data_interpretor_init_interface(heraia_window_t *main_window)
 *  Inits the data interpretor structure and window with default values
 *  @warning Should be called only once at program's beginning
 */
void data_interpretor_init_interface(heraia_window_t *main_window)
{
	data_window_t *dw = NULL;

	if (main_window != NULL)
		{
			/* Signals connections */
			connect_data_interpretor_signals(main_window);

			dw = main_window->current_DW;

			if (dw != NULL)
				{
					dw->diw = heraia_get_widget(main_window->xmls->main, "data_interpretor_window");
					
					/* Here we might init all tabs */
					add_default_tabs(main_window);
				}
		}
}


tab_t *add_new_tab_in_data_interpretor(GtkNotebook *notebook, guint index, gchar *label, guint nb_cols, ...)
{
	tab_t *tab = NULL;            /**< tab structure that will remember everything !                     */
	va_list args;                 /**< va_list arguments passed to create a new tab with those columns   */
	guint i = 0;
	gchar *col_label = NULL;      /**< used to fetch atguments                                           */
	GPtrArray *col_labels = NULL; /**< used to remember the columns labels (the arguments in GtkWidgets) */
	GPtrArray *vboxes = NULL;     /**< used to remember vboxes (in order to be able to pack things later */
	GtkWidget *child = NULL;      /**< notebook tab's child container                                    */
	GtkWidget *hpaned = NULL;     /**< used for hpaned creation                                          */
	GtkWidget *hpaned2 = NULL;    /**< in case that we have more than 2 arguments                        */
	GtkWidget *vbox = NULL;       /**< used for vbox creation                                            */
	GtkWidget *vbox_label = NULL; /**< used for label creation in the new vboxes                         */

	col_labels = g_ptr_array_new();
	vboxes = g_ptr_array_new();

	va_start(args, nb_cols);
	for (i = 0 ; i < nb_cols ; i++)
	{
		col_label = (gchar *) va_arg(args, int);
		vbox_label = gtk_label_new(col_label);
		gtk_misc_set_padding(GTK_MISC(vbox_label), 4, 4);
		gtk_misc_set_alignment(GTK_MISC(vbox_label), 0.5, 0.5);
		g_ptr_array_add(col_labels, (gpointer) vbox_label);
	}
	va_end(args);
	
	tab = (tab_t *) g_malloc0(sizeof(tab_t));

    i = 0;
    hpaned = gtk_hpaned_new();
	child = hpaned;
	vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
	g_ptr_array_add(vboxes, vbox);
	gtk_paned_add1(GTK_PANED(hpaned), (gpointer) vbox);
	vbox_label = g_ptr_array_index(col_labels, i);
	gtk_box_pack_start(GTK_BOX(vbox), vbox_label, FALSE, FALSE, 4);
	
	i++;
	while (i < nb_cols-1)
	{
		hpaned2 = gtk_hpaned_new();
		gtk_paned_add2(GTK_PANED(hpaned), hpaned2);
		hpaned = hpaned2;                  /* translation */
		vbox = gtk_vbox_new(FALSE, 2);
		gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
		g_ptr_array_add(vboxes, (gpointer) vbox);
		gtk_paned_add1(GTK_PANED(hpaned), vbox);
		vbox_label = g_ptr_array_index(col_labels, i);
		gtk_box_pack_start(GTK_BOX(vbox), vbox_label, FALSE, FALSE, 4);
		i++;
	}
	
	vbox = gtk_vbox_new(FALSE, 2);
	g_ptr_array_add(vboxes, (gpointer) vbox);
	gtk_paned_add2(GTK_PANED(hpaned), vbox);
	gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
	vbox_label = g_ptr_array_index(col_labels, i);
	gtk_box_pack_start(GTK_BOX(vbox), vbox_label, FALSE, FALSE, 4);

	tab->index = index;
	tab->nb_cols = nb_cols;
	tab->label = gtk_label_new(label);
	tab->col_labels = col_labels;
	tab->vboxes = vboxes;
	tab->rows = NULL;
	
	gtk_notebook_append_page(notebook, child, tab->label);
	
	return tab;
}


void add_new_row_to_tab(tab_t *tab, decode_generic_t *row)
{
	GtkWidget *vbox = NULL;  /**< the vbox to which we want to pack           */
	decode_t *couple = NULL; /**< couple from which we want to pack the entry */
	guint i = 0;
	guint j = 0;
	
	if (tab != NULL && row != NULL)
	{
	
		if (tab->rows == NULL)
		{
			tab->rows = g_ptr_array_new();
		}
	
		g_ptr_array_add(tab->rows, (gpointer) row);
		
		/* label packing */
		i = 0;
		vbox = g_ptr_array_index(tab->vboxes, i);
		gtk_box_pack_start(GTK_BOX(vbox), row->label, FALSE, FALSE, 0);

		j = 0;
		i++;
		while (i <  tab->nb_cols)
		{
			vbox = g_ptr_array_index(tab->vboxes, i);
			couple = g_ptr_array_index(row->decode_array, j);
			gtk_box_pack_start(GTK_BOX(vbox), couple->entry, FALSE, FALSE, 0);
			gtk_widget_show(couple->entry);
			j++;
			i++;
		}
	}
}

void add_default_tabs(heraia_window_t *main_window)
{
	GtkWidget *notebook = NULL;
	tab_t *tab = NULL;
	decode_generic_t *row = NULL;
		
	notebook = heraia_get_widget(main_window->xmls->main, "diw_notebook");	
		
	tab = add_new_tab_in_data_interpretor(GTK_NOTEBOOK(notebook), 4, "This is a test", 3, "Type", "Signed", "Unsigned");

	if (tab != NULL)
	{
		main_window->current_DW->tab = tab;
		row = new_decode_generic_t("8 bits", 1, FALSE, 2, decode_8bits_signed, decode_8bits_unsigned);
		add_new_row_to_tab(tab, row);
		row = new_decode_generic_t("16 bits", 2, FALSE, 2, decode_16bits_signed, decode_16bits_unsigned);
		add_new_row_to_tab(tab, row);
		row = new_decode_generic_t("32 bits", 4, FALSE, 2, decode_32bits_signed, decode_32bits_unsigned);
		add_new_row_to_tab(tab, row);
		row = new_decode_generic_t("64 bits", 8, FALSE, 2, decode_64bits_signed, decode_64bits_unsigned);
		add_new_row_to_tab(tab, row);
	}

}	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	



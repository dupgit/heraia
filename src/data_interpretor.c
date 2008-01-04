/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  data_interpretor.c
  heraia - an hexadecimal file editor and analyser based on ghex
 
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include "heraia_types.h"

static guint which_endianness(heraia_window_t *main_window);
static void interpret_as_date(heraia_window_t *main_window, DecodeDateFunc decode_it, gchar *widget_name, guint length, guint endianness);
static void interpret_as_number(heraia_window_t *main_window, DecodeFunc decode_it, gchar *widget_name, guint length, guint endianness);
static void close_data_interpretor_window(GtkWidget *widget, gpointer data);
static void connect_data_interpretor_signals(heraia_window_t *main_window);

/**
 *  Determines which endianness is selected that is to say
 *  which radio button is active in the window
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
 *   Here we do interpret a date according to the decode_it function
 *   We are assuming that main_window != NULL and main_window->xml != NULL
 *    . heraia_window_t *main_window : the main structure
 *    . DecodeDateFunc decode_it : a function to be called to decode the stream
 *    . gchar *widget_name : the name of the widget where the result may go
 *    . guint length : the length of the data to be decoded
 *    . guint endianness : the endianness to be applied to the datas
 */
static void interpret_as_date(heraia_window_t *main_window, DecodeDateFunc decode_it, gchar *widget_name, guint length, guint endianness)
{	
	gint result = 0;       /* used to test different results of function calls */
	guchar *c = NULL;      /* the character under the cursor                   */
	gchar *text = NULL;
	data_window_t *data_window = main_window->current_DW;
	GtkWidget *entry = heraia_get_widget(main_window->xmls->main, widget_name);
	date_and_time_t *mydate = NULL; /* date resulting of interpretation       */
	
	c = (guchar *) g_malloc0 (sizeof(guchar) * length);
	mydate = (date_and_time_t *) g_malloc0 (sizeof(date_and_time_t));

	result = ghex_get_data(data_window, length, endianness, c);
	
	if (result == TRUE)
		{
			text = decode_it(c, mydate);
 
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
			text = g_strdup_printf("Cannot interpret as a %d byte(s) date", length);
			gtk_entry_set_text(GTK_ENTRY(entry), text);		
		}

	g_free(c);
  	g_free(text);
}


/**
 *   Here we do interpret a number according to the decode_it function
 *   We are assuming that main_window != NULL and main_window->xml != NULL
 *    . heraia_window_t *main_window : the main structure
 *    . DecodeFunc decode_it : a function to be called to decode the stream
 *    . gchar *widget_name : the name of the widget where the result may go
 *    . guint length : the length of the data to be decoded 
 *    . guint endianness : the endianness to be applied to the datas
 */
static void interpret_as_number(heraia_window_t *main_window, DecodeFunc decode_it, gchar *widget_name, guint length, guint endianness)
{	
	gint result = 0;       /* used to test different results of function calls */
	guchar *c = NULL;      /* the character under the cursor                   */
	gchar *text = NULL; 
	data_window_t *data_window = main_window->current_DW;   /* We allready know that it's not NULL */
	GtkWidget *entry = heraia_get_widget(main_window->xmls->main, widget_name);  /* we might test the result as this is user input*/

	c = (guchar *) g_malloc0(sizeof(guchar) * length);

	result = ghex_get_data(data_window, length, endianness, c);

	if (result == TRUE)
		{
			text = decode_it(c);
 
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
 *  "Emulates" the user click on the main window menu entry called DIMenu
 *  whose aim is to display or hide the data interpretor window
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
 *  Refreshes the data interpretor window with the new values
 */
void refresh_data_interpretor_window(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_window = (heraia_window_t *) data;  /* data interpretor window structure */
	guint endianness = 0;

	if (main_window != NULL && main_window->current_DW != NULL && main_window->current_DW->window_displayed == TRUE)
		{
			endianness = which_endianness(main_window);  /* Endianness is computed only once here */
			interpret_as_number(main_window, decode_8bits_unsigned, "diw_8bits_us", 1, endianness);
			interpret_as_number(main_window, decode_8bits_signed, "diw_8bits_s", 1, endianness);
			interpret_as_number(main_window, decode_16bits_unsigned, "diw_16bits_us", 2, endianness);
			interpret_as_number(main_window, decode_16bits_signed, "diw_16bits_s", 2, endianness);
			interpret_as_number(main_window, decode_32bits_unsigned, "diw_32bits_us", 4, endianness);
			interpret_as_number(main_window, decode_32bits_signed, "diw_32bits_s", 4, endianness);
			interpret_as_number(main_window, decode_64bits_unsigned, "diw_64bits_us", 8, endianness);
			interpret_as_number(main_window, decode_64bits_signed, "diw_64bits_s", 8, endianness);
			interpret_as_number(main_window, decode_to_bits, "diw_base_bits", 1, endianness);
			interpret_as_number(main_window, decode_packed_BCD, "diw_base_bcd", 1, endianness);

			interpret_as_date(main_window, decode_C_date, "diw_C_date", 4, endianness);
			interpret_as_date(main_window, decode_dos_date, "diw_msdos_date", 4, endianness);
			interpret_as_date(main_window, decode_filetime_date, "diw_filetime_date", 8, endianness);
			interpret_as_date(main_window, decode_HFS_date, "diw_HFS_date", 4, endianness);

			refresh_all_ud_data_interpretor(main_window, endianness);
		}
}


/**
 *  Connects data interpretor window's signals to the
 *  right functions
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
 *  Inits the data interpretor structure and window
 *  with default values
 *  Should be called only once
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
					/* Says whether the data interpretor window is displayed or not */
					dw->window_displayed = H_DI_DISPLAYED;
					dw->diw = heraia_get_widget(main_window->xmls->main, "data_interpretor_window");
					dw->tab_displayed = 0; /* the first tab */
				}
		}
}

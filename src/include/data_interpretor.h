/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  data_interpretor.h
  heraia - an hexadecimal file editor and analyser based on ghex
 
  (C) Copyright 2005 Olivier Delhomme
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


#ifndef _DATA_INTERPRETOR_H_
#define _DATA_INTERPRETOR_H_

#include <gtk/gtk.h>

/* 
Naming : 
  H stands for Heraia
  DI stands for Data_Interpretor
*/

#define H_DI_MAX_ENTRY 32   /* Max characters in an entry box                      */
#define H_DI_H_SPACE 2      /* Horizontal space between widgets                    */
#define H_DI_V_SPACE 2      /* Vertical space between widgets                      */
#define H_DI_BORDER_WIDTH 2 /* Object's border width in containers                 */
#define H_DI_LINES 6        /* Number of lines in the DI window by default         */
#define H_DI_COLUMNS 3      /* Same but for columns                                */
#define H_DI_DISPLAYED TRUE /* By default the Data Interpretor Window is displayed */


/* defaults values to display the data_interpretor window */
#define H_DI_FONT_FAMILY "FreeMono"
#define H_DI_FONT_STYLE "Bold"
#define H_DI_FONT_WIDTH "9"

typedef struct
{
	/* Current Hexwidget that we want data to be interpreted */
	GtkWidget *current_hexwidget;

	/* window widgets */
	GtkWidget *window;
	GtkWidget *window_vbox;
	GtkWidget *window_table;
	GtkWidget *window_statusbar;
	GtkWidget *window_menu_bar;
	GtkWidget *window_show_menu;
	GtkWidget *window_show_item;
	GtkWidget *window_encoding_menu;
	GtkWidget *window_encoding_item;
	GtkWidget *window_font_menu;
	GtkWidget *window_font_item;

	/* show menu items */
	GtkWidget *window_show_8bits_item;
	GtkWidget *window_show_16bits_item;
	GtkWidget *window_show_32bits_item;
	GtkWidget *window_show_64bits_item;
	GtkWidget *window_show_dos_date_item;
	GtkWidget *window_show_filetime_date_item;
	GtkWidget *window_show_C_date_item;

	GtkWidget *window_enc_b_e_item;   /* encoding_big_endian menu item */

	GtkWidget *window_font_select_item; /* font menu item */

	/* general config and parameters for the DI window*/
	guint table_lines;
	guint table_columns;
	guint statusbar_context;
	gboolean window_displayed;
	gchar *window_font_name;
	

	/* labels */
	GtkWidget *label_unsigned;
	GtkWidget *label_signed;
	GtkWidget *label_type;
	GtkWidget *label_8bits;
	GtkWidget *label_16bits;
	GtkWidget *label_32bits;
	GtkWidget *label_64bits;
	GtkWidget *label_dos_date;
	GtkWidget *label_filetime_date;
	GtkWidget *label_C_date;

	/* entries */
	GtkWidget *entry_signed_byte;
	GtkWidget *entry_unsigned_byte;
	GtkWidget *entry_signed_16bits;
	GtkWidget *entry_unsigned_16bits;
	GtkWidget *entry_signed_32bits;
	GtkWidget *entry_unsigned_32bits;
	GtkWidget *entry_signed_64bits;
	GtkWidget *entry_unsigned_64bits;
	GtkWidget *entry_dos_date;
	GtkWidget *entry_filetime_date;
	GtkWidget *entry_C_date;

} data_window_t;

extern void data_interpret( data_window_t *DW );
extern void refresh_data_window( GtkWidget *hexwidget, gpointer data );

#endif /* _DATA_INTERPRETOR_H_ */

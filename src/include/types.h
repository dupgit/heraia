/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  types.h
  types.h - all data types and pieces of common declarations
 
  (C) Copyright 2006 - 2007 Olivier Delhomme
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

#ifndef _HERAIA_TYPES_H_
#define _HERAIA_TYPES_H_

#ifdef HAVE_CONFIG_H
   #include "config.h"
#endif /* HAVE_CONFIG_H */


/** As there seems to be huge differences between the linux
 *  and the windows developpement platform such as that
 *  the included paths are correcty taken into account
 *  under linux (and thus nobody bothers) but not under
 *  windows, I decide to put all the .h definitions here
 *  even if it's ugly, this is supposed to solve the problem.
 */

#include <getopt.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gmodule.h>
#include <gtk/gtk.h>
#include <gtkhex/gtkhex.h>
#include <glade/glade.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


typedef HexDocument Heraia_Document;
typedef GtkHex Heraia_Hex;
typedef gint HERAIA_ERROR;

/**
 *  Refresh type (may be used to decide what to do
 *  in a particular case
 */
typedef gint RefreshType;
#define HERAIA_REFRESH_NOTHING 0
#define HERAIA_REFRESH_NEW_FILE 1
#define HERAIA_REFRESH_CURSOR_MOVE 2


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


typedef struct
{
	gboolean  debug;          /* Used to tell the program wether we want to display debug messages or not */
	gchar *filename;          /* this could (should) be a list of filenames !!!                           */

	Heraia_Document *current_doc;
	GladeXML *xml;
	
   	data_window_t *current_DW;  /* data_interpretor pointer                            */
	/*void *ga;                  graph analysis pointer                              */
	GList *location_list;       /* this is the location list where we store some paths */
	GList *plugins_list;        /* A list of plugins                                   */
	RefreshType event;          /* Tells what is happening                             */
} heraia_window_t;


/* a human struct to store a date with a time*/
typedef struct
{
	guint32 year;
	guint32 month;
	guint32 day;
	
	guint32 hour;
	guint32 minutes;
	guint32 seconds;

} date_and_time_t;

#include "data_interpretor.h"
#include "decode.h"
#include "ghex_heraia_interface.h"
#include "heraia.h"
#include "heraia_ui.h"
#include "heraia-errors.h"
#include "io.h"
#include "log.h"
#include "plugin.h"
#include "plugin_list.h"

#endif /* _HERAIA_TYPES_H_ */

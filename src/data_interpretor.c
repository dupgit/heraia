/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  data_interpretor.c
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

#include <string.h>

#include "types.h"
#include "heraia-errors.h"
#include "data_interpretor.h"
#include "decode.h"
#include "ghex_heraia_interface.h"

/* Begining of data interpretor things  */
static gboolean is_activate(GtkWidget *check_menu_item);
static void DI_print_offset(data_window_t *DW);
static void interpret_unsigned_byte(data_window_t *DW);
static void interpret_signed_byte(data_window_t *DW);
static void interpret_16bits_number(gboolean is_signed, data_window_t *DW);
static void interpret_32bits_number(gboolean is_signed, data_window_t *DW);
static void interpret_64bits_number(gboolean is_signed, data_window_t *DW);
static void interpret_dos_date(data_window_t *DW);
static void interpret_filetime_date(data_window_t *DW);
static void interpret_C_date(data_window_t *DW);
static void display_8bits(data_window_t *DW, gushort line);
static void display_16bits(data_window_t *DW, gushort line);
static void display_32bits(data_window_t *DW, gushort line);
static void display_64bits(data_window_t *DW, gushort line);
static void display_dos_date(data_window_t *DW, gushort line);
static void display_filetime_date(data_window_t *DW, gushort line);
static void display_C_date(data_window_t *DW, gushort line);
static void display_labels(data_window_t *DW, gushort line);
static void display_statusbar(data_window_t *DW);
static void display_di_window(data_window_t *DW);
static void resize_window_table(data_window_t *DW);

static void show_menu_item_response(GtkWidget *widget, gpointer data);
static void encoding_menu_item_response(GtkWidget *widget, gpointer data);
static void font_select_menu_item_response(GtkWidget *widget, gpointer data);
static void init_di_menu(data_window_t *DW);


/*
  Here we want to know where we are in the file (from the beginning)
*/
static void DI_print_offset(data_window_t *DW)
{	
	guint pos;      /* position of the cursor in the file */
	gchar *text;    /* interpreted text to be printed     */
	GtkHex *gh;

	gh = GTK_HEX (DW->current_hexwidget);

	pos = gtk_hex_get_cursor (gh);
	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));

	/* we do assume that gtk_hex_get_cursor returned something correct ! */ 

	sprintf(text, "offset = %u", pos);
	gtk_statusbar_pop (GTK_STATUSBAR(DW->window_statusbar), 
							 DW->statusbar_context);
	DW->statusbar_context =
		gtk_statusbar_get_context_id (GTK_STATUSBAR(DW->window_statusbar),
												text);
	gtk_statusbar_push (GTK_STATUSBAR(DW->window_statusbar), 
							  DW->statusbar_context, text);
	g_free (text);
}


/*
    Here we do interpret an unsigned byte in decimal format
*/
static void interpret_unsigned_byte(data_window_t *DW)
{	
	gchar *text = NULL;    /* interpreted text to be printed     */
	gint result = 0;       /* used to test different results of function calls */
	guchar *c = NULL;      /* the character under the cursor     */
	GtkHex *gh = NULL;

	gh = GTK_HEX (DW->current_hexwidget);

	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));
	c = (guchar *) g_malloc0 (sizeof(guchar));
	result = ghex_memcpy(gh, gtk_hex_get_cursor (gh), 1, FALSE, c);
	if (result == TRUE)
		{
			result = decode_8bits_unsigned(c, text);
			if (result == TRUE)
				gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_byte), text);
			else
				{
					sprintf(text, "%s", "Something's wrong!");
					gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_byte), text);
				}			
		}
	else
		{
			sprintf(text, "%s", "Not a number");
			gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_byte), text);		
		}

	g_free (c);
  	g_free (text);
}


/*
    Here we do interpret a signed byte in decimal format
*/
static void interpret_signed_byte(data_window_t *DW)
{	
	gchar *text;    /* interpreted text to be printed     */
	gint result;    /* used to test different results of function calls */
	guchar *c;      /* the character under the cursor     */
	GtkHex *gh;

	gh = GTK_HEX (DW->current_hexwidget);

	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));
	c = (guchar *) g_malloc0 (sizeof(guchar));
	result = ghex_memcpy(gh, gtk_hex_get_cursor (gh), 1, FALSE, c);

	if (result == TRUE)
		{
			result = decode_8bits_signed(c, text);
			if (result == TRUE)
				gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_byte), text);
			else
				{
					sprintf(text, "%s", "Something's wrong!");
					gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_byte), text);
				}			
		}
	else
		{
			sprintf(text, "%s", "Not a number");
			gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_byte), text);		
		}

	g_free (c);
  	g_free (text);
}


/*
    Here we do interpret a 16 bits number in decimal format
	 is_signed == TRUE interpets as a signed 16 bits number
	 is_signed == FALSE interprets as an unsigned 16 bits number
*/
static void interpret_16bits_number(gboolean is_signed, data_window_t *DW)
{	
	gchar *text; /* interpreted text to be printed     */
	guchar *c;   /* the characters under the cursor and the next one */
	gint result; /* used to test different results of function calls */
	GtkHex *gh;  /* the GtkHex widget we want to communicate with    */
	
	gh = GTK_HEX (DW->current_hexwidget);

	c = (guchar *) g_malloc0 (2*sizeof(guchar));
	result = ghex_memcpy(gh, gtk_hex_get_cursor (gh), 2, 
								is_activate(DW->window_enc_b_e_item), c);
	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));

	if (result == TRUE)
		{
			if (is_signed == TRUE)
				result = decode_16bits_signed(c, text);
			else
				result = decode_16bits_unsigned(c, text);

			if (result == TRUE)
				{
					if (is_signed == TRUE)
						gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_16bits), text);
					else
						gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_16bits), text);
				}	
		}
	else
		{
			sprintf(text, "%s", "Not a 16 bits number");
			gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_16bits), text);
			gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_16bits), text);
		}	

	g_free (text);
	g_free (c);
}


/*
    Here we do interpret a 32 bits number in decimal format
	 is_signed == TRUE interpets as a signed 32 bits number
	 is_signed == FALSE interprets as an unsigned 32 bits number
*/
static void interpret_32bits_number(gboolean is_signed, data_window_t *DW)
{	
	gchar *text; /* interpreted text to be printed     */
	guchar *c;   /* the characters under the cursor and the next one */
	gint result; /* used to test different results of function calls */
	GtkHex *gh;  /* the GtkHex widget we want to communicate with    */

	gh = GTK_HEX (DW->current_hexwidget);

	c = (guchar *) g_malloc0 (4*sizeof(guchar));
	result = ghex_memcpy(gh, gtk_hex_get_cursor (gh), 4,
								is_activate(DW->window_enc_b_e_item), c);
	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));
	if (result == TRUE)
		{		
			if (is_signed == TRUE)
				result = decode_32bits_signed(c, text);
			else
				result = decode_32bits_unsigned(c, text);

			if (result == TRUE)
				{
					if (is_signed == TRUE)
						gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_32bits), text);
					else
						gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_32bits), text);
				}
		}
	else
		{
			text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));
			sprintf(text, "%s", "Not a 32 bits number");
			gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_32bits), text);
			gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_32bits), text);
		
		}

	g_free (text);
	g_free (c);
}

/*
    Here we do interpret a 64 bits number in decimal format
	 is_signed == TRUE interpets as a signed 64 bits number
	 is_signed == FALSE interprets as an unsigned 64 bits number
*/
static void interpret_64bits_number(gboolean is_signed, data_window_t *DW)
{	
	gchar *text; /* interpreted text to be printed     */
	guchar *c;   /* the characters under the cursor and the next one */
	gint result; /* used to test different results of function calls */
	GtkHex *gh;  /* the GtkHex widget we want to communicate with    */

	gh = GTK_HEX (DW->current_hexwidget);

	c = (guchar *) g_malloc0 (8*sizeof(guchar));
	result = ghex_memcpy(gh, gtk_hex_get_cursor (gh), 8, 
								is_activate(DW->window_enc_b_e_item), c);
	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));

	if (result == TRUE)
		{
			if (is_signed == TRUE)
				result = decode_64bits_signed(c, text);
			else
				result = decode_64bits_unsigned(c, text);

			if (result == TRUE)
				{
					if (is_signed == TRUE)
						gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_64bits), text);
					else
						gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_64bits), text);
				}
		}
	else
		{
			sprintf(text, "%s", "Not a 64 bits number");
			gtk_entry_set_text (GTK_ENTRY(DW->entry_signed_64bits), text);
			gtk_entry_set_text (GTK_ENTRY(DW->entry_unsigned_64bits), text);
			g_free (text);
		}

	g_free (text);
	g_free (c);
}


/*
    Here we do interpret a dos date (a 4 bytes data)
	 We can not have any date struct type because our interprets
    might differ from any real date (when interpreted data does
    not represent a dos date).
*/
static void interpret_dos_date(data_window_t *DW)
{	
	gchar *text; /* interpreted text to be printed                   */
	guchar *c;   /* the characters under the cursor and the next one */
	GtkHex *gh;  /* the GtkHex widget we want to communicate with    */
	gint result; /* used to test different results of function calls */
	date_and_time_t *mydate;

	gh = GTK_HEX (DW->current_hexwidget);
	
	c = (guchar *) g_malloc0 (4*sizeof(guchar));
	mydate = (date_and_time_t *) g_malloc0 (sizeof(*mydate));
	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));
	
	result = ghex_memcpy(gh, gtk_hex_get_cursor (gh), 4,
								is_activate(DW->window_enc_b_e_item), c);

	if (result == TRUE)
		{
			result = decode_dos_date(c, mydate);
			if (result == TRUE)
				{
					sprintf(text, "%02d/%02d/%04d - %02d:%02d:%02d", 
							  mydate->day, mydate->month, mydate->year, 
							  mydate->hour, mydate->minutes, mydate->seconds);
					gtk_entry_set_text (GTK_ENTRY(DW->entry_dos_date), text);
				}
		}
	
	if (result == FALSE)
		{
			sprintf(text, "%s", "Not a dos date");
			gtk_entry_set_text (GTK_ENTRY(DW->entry_dos_date), text);
		}

	g_free (text);
	g_free(mydate);
	g_free (c);
}


/*
    Here we do interpret a filetime date (a 8 bytes data)
	 We can not have any date struct type because our interprets
    might differ from any real date (when interpreted data does
    not represent a filetime date).
*/
static void interpret_filetime_date(data_window_t *DW)
{	
	gchar *text; /* interpreted text to be printed                   */
	guchar *c;   /* the characters under the cursor and the next one */
	GtkHex *gh;  /* the GtkHex widget we want to communicate with    */
	gint result; /* used to test different results of function calls */
	date_and_time_t *mydate;

	gh = GTK_HEX (DW->current_hexwidget);
	
	c = (guchar *) g_malloc0 (8*sizeof(guchar));		
	mydate = (date_and_time_t *) g_malloc0 (sizeof(*mydate));
	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));
	
	result = ghex_memcpy(gh, gtk_hex_get_cursor (gh), 8,
								is_activate(DW->window_enc_b_e_item), c);
	
	if (result == TRUE)
		{
			result = decode_filetime_date(c, mydate);
			if (result == TRUE)
				{
					sprintf(text, "%02u/%02u/%u - %02u:%02u:%02u",
							  mydate->day, mydate->month, mydate->year, 
							  mydate->hour, mydate->minutes, mydate->seconds);
					gtk_entry_set_text (GTK_ENTRY(DW->entry_filetime_date), text);
				}		
		}
	
	if (result == FALSE)
		{
			sprintf(text, "%s", "Not a filetime date");
			gtk_entry_set_text (GTK_ENTRY(DW->entry_filetime_date), text);
		}

	g_free (text);
	g_free(mydate);
	g_free (c);
}


/*
    Here we do interpret a C date (a 4 bytes data)
	 We can not have any date struct type because our interprets
    might differ from any real date (when interpreted data does
    not represent a C date).
*/
static void interpret_C_date(data_window_t *DW)
{	
	gchar *text; /* interpreted text to be printed                   */
	guchar *c;   /* the characters under the cursor and the next one */
	GtkHex *gh;  /* the GtkHex widget we want to communicate with    */
	gint result; /* used to test different results of function calls */
	date_and_time_t *mydate; /* date resulting of interpretation       */


	gh = GTK_HEX (DW->current_hexwidget);
	
	c = (guchar *) g_malloc0 (4*sizeof(guchar));
	mydate = (date_and_time_t *) g_malloc0 (sizeof(*mydate));
	text = (gchar *) g_malloc0 (H_DI_MAX_ENTRY*sizeof(gchar));

	result = ghex_memcpy(gh, gtk_hex_get_cursor (gh), 4,
								is_activate(DW->window_enc_b_e_item), c);

	if (result == TRUE)
		{
			result = decode_C_date(c, mydate);
			if (result == TRUE)
				{
					sprintf(text, "%02u/%02u/%u - %02u:%02u:%02u", 
							  mydate->day, mydate->month, mydate->year, 
							  mydate->hour, mydate->minutes, mydate->seconds);
					gtk_entry_set_text (GTK_ENTRY(DW->entry_C_date), text);
				}
		}
	
	if (result == FALSE)
		{
			sprintf(text, "%s", "Not a C date");
			gtk_entry_set_text (GTK_ENTRY(DW->entry_C_date), text);
		}
	
	g_free (text);
	g_free (mydate);
	g_free (c);
}
/* End of data interpretor things       */


/* Begining of Widget related stuff     */
static gboolean is_activate(GtkWidget *check_menu_item)
{

	return gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(check_menu_item));
}


/*
  function called when the cursor changes its position 
  (CALL_BACK so we need that prototype)
*/
void refresh_data_window(GtkWidget *hexwidget, gpointer data)
{
	data_window_t *DW = (data_window_t *) data;
	PangoFontDescription *pfd;

	if (DW->window_displayed == TRUE)
		{
			DI_print_offset(DW);

			if (DW->window_font_name != NULL)
				pfd = pango_font_description_from_string(DW->window_font_name);
			else
				pfd = NULL;

			gtk_widget_modify_font(GTK_WIDGET(DW->label_unsigned), pfd);
			gtk_widget_modify_font(GTK_WIDGET(DW->label_signed), pfd);
			gtk_widget_modify_font(GTK_WIDGET(DW->label_type), pfd);

			if (is_activate(DW->window_show_8bits_item) == TRUE)
				{
					interpret_unsigned_byte(DW);
					interpret_signed_byte(DW);
					gtk_widget_modify_font(GTK_WIDGET(DW->label_8bits), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_signed_byte), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_unsigned_byte), pfd); 
				}

			if (is_activate(DW->window_show_16bits_item) == TRUE)
				{
					interpret_16bits_number(FALSE, DW);
					interpret_16bits_number(TRUE, DW);
					gtk_widget_modify_font(GTK_WIDGET(DW->label_16bits), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_signed_16bits), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_unsigned_16bits), pfd);
				}

			if (is_activate(DW->window_show_32bits_item) == TRUE)
				{
					interpret_32bits_number(FALSE, DW);
					interpret_32bits_number(TRUE, DW);
					gtk_widget_modify_font(GTK_WIDGET(DW->label_32bits), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_signed_32bits), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_unsigned_32bits), pfd);
				}

			if (is_activate(DW->window_show_64bits_item) == TRUE)
				{
					interpret_64bits_number(FALSE, DW);
					interpret_64bits_number(TRUE, DW);
					gtk_widget_modify_font(GTK_WIDGET(DW->label_64bits), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_signed_64bits), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_unsigned_64bits), pfd);
				}

			if (is_activate(DW->window_show_dos_date_item) == TRUE)
				{
					interpret_dos_date(DW);
					gtk_widget_modify_font(GTK_WIDGET(DW->label_dos_date), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_dos_date), pfd);
				}

			if (is_activate(DW->window_show_filetime_date_item) == TRUE)
				{
					interpret_filetime_date(DW);
					gtk_widget_modify_font(GTK_WIDGET(DW->label_filetime_date), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_filetime_date), pfd);
				}

			if (is_activate(DW->window_show_C_date_item) == TRUE)
				{
					interpret_C_date(DW);
					gtk_widget_modify_font(GTK_WIDGET(DW->label_C_date), pfd);
					gtk_widget_modify_font(GTK_WIDGET(DW->entry_C_date), pfd);
				}

			gtk_widget_show_all (DW->window);
		}
}


/* display functions below */
static void display_8bits(data_window_t *DW, gushort line)
{
	/* First line is number 0 */
	/* label */
	DW->label_8bits = gtk_label_new ("8 bits");
	gtk_table_attach(GTK_TABLE(DW->window_table), GTK_WIDGET(DW->label_8bits),
						  0, 1, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE); 
	
	/* unsigned byte entry */
	DW->entry_unsigned_byte = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_unsigned_byte), 
						  1, 2, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
 
	/* signed byte entry */
	DW->entry_signed_byte = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_signed_byte), 
						  2, 3, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
}


static void display_16bits(data_window_t *DW, gushort line)
{
	/* First line is number 0 */
	/* label */
	DW->label_16bits = gtk_label_new ("16 bits");
	gtk_table_attach(GTK_TABLE(DW->window_table), GTK_WIDGET(DW->label_16bits),
						  0, 1, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);

	/* unsigned 16 bits entry*/
	DW->entry_unsigned_16bits = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_unsigned_16bits), 
						  1, 2, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
	
	/* signed 16 bits entry */
	DW->entry_signed_16bits = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), GTK_WIDGET(DW->entry_signed_16bits), 
						  2, 3, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
					 H_DI_H_SPACE, H_DI_V_SPACE);

}


static void display_32bits(data_window_t *DW, gushort line)
{
	/* First line is number 0 */
	/* label */
	DW->label_32bits = gtk_label_new ("32 bits");
	gtk_table_attach(GTK_TABLE(DW->window_table), GTK_WIDGET(DW->label_32bits), 
						  0, 1, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);

	/* unsigned 32 bits entry*/
	DW->entry_unsigned_32bits = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY); 
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_unsigned_32bits), 
						  1, 2, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
  
	/* signed 32 bits entry */
	DW->entry_signed_32bits = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), GTK_WIDGET(DW->entry_signed_32bits), 
						  2, 3, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
					 H_DI_H_SPACE, H_DI_V_SPACE);

}


static void display_64bits(data_window_t *DW, gushort line)
{
	/* First line is number 0 */
	/* label */
	DW->label_64bits = gtk_label_new ("64 bits");
	gtk_table_attach(GTK_TABLE(DW->window_table), GTK_WIDGET(DW->label_64bits), 
						  0, 1, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);

	/* unsigned 64 bits entry*/
	DW->entry_unsigned_64bits = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY); 
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_unsigned_64bits), 
						  1, 2, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);

	/* signed 64 bits entry */
	DW->entry_signed_64bits = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_signed_64bits), 
						  2, 3, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
}


static void display_dos_date(data_window_t *DW, gushort line)
{
	/* dos date */
	/* First line is number 0 */
	/* label */
	DW->label_dos_date = gtk_label_new ("dos date");
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->label_dos_date),
						  0, 1, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
	/* entry */
	DW->entry_dos_date = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_dos_date), 
						  1, 2, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
}


static void display_filetime_date(data_window_t *DW, gushort line)
{
	/* filetime date */
	/* First line is number 0 */
	/* label */
	DW->label_filetime_date = gtk_label_new ("filetime date");
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->label_filetime_date), 
						  0, 1, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
	/* entry */
	DW->entry_filetime_date = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_filetime_date), 
						  1, 2, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
}


static void display_C_date(data_window_t *DW, gushort line)
{
	/* C date */
	/* First line is number 0 */
	/* label */
	DW->label_C_date = gtk_label_new ("C date");
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->label_C_date), 
						  0, 1, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
	/* entry */
	DW->entry_C_date = gtk_entry_new_with_max_length (H_DI_MAX_ENTRY);
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->entry_C_date), 
						  1, 2, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
}


static void display_labels(data_window_t *DW, gushort line)
{
	/* unsigned byte */
	DW->label_type = gtk_label_new ("type");
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->label_type), 
						  0, 1, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE); 

	/* unsigned byte */
	DW->label_unsigned = gtk_label_new ("unsigned");
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->label_unsigned), 
						  1, 2, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE); 

	/* signed byte */
	DW->label_signed = gtk_label_new ("signed");
	gtk_table_attach(GTK_TABLE(DW->window_table), 
						  GTK_WIDGET(DW->label_signed), 
						  2, 3, line, line+1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 
						  H_DI_H_SPACE, H_DI_V_SPACE);
}


static void display_di_window(data_window_t *DW) 
{
	gushort line = 0;
	if (DW->window_displayed == TRUE)
		{
			if (is_activate(DW->window_show_8bits_item) == TRUE || 
				 is_activate(DW->window_show_16bits_item) == TRUE ||
				 is_activate(DW->window_show_32bits_item) == TRUE || 
				 is_activate(DW->window_show_64bits_item) == TRUE )
				display_labels(DW, line++);

			if (is_activate(DW->window_show_8bits_item) == TRUE)
				display_8bits(DW, line++);
	
			if (is_activate(DW->window_show_16bits_item) == TRUE)
				display_16bits(DW, line++);

			if (is_activate(DW->window_show_32bits_item) == TRUE)
				display_32bits(DW, line++);

			if (is_activate(DW->window_show_64bits_item) == TRUE)
				display_64bits(DW, line++);

			if (is_activate(DW->window_show_dos_date_item) == TRUE)
				display_dos_date(DW, line++);

			if (is_activate(DW->window_show_filetime_date_item) == TRUE)
				display_filetime_date(DW, line++);

			if (is_activate(DW->window_show_C_date_item) == TRUE)
				display_C_date(DW, line++);

			gtk_widget_show_all (DW->window);
		}
}


static void display_statusbar(data_window_t *DW)
{


	DW->statusbar_context = 
		gtk_statusbar_get_context_id (GTK_STATUSBAR(DW->window_statusbar), "");

	DI_print_offset(DW);
}


/* Destroys the widgets in the table and resizes it */
static void resize_window_table(data_window_t *DW)
{
	guint line = 0;

	if (DW->label_type != NULL)
		{
			gtk_widget_destroy (DW->label_unsigned);
			DW->label_unsigned = NULL;
			gtk_widget_destroy (DW->label_signed);
			DW->label_signed = NULL;
			gtk_widget_destroy (DW->label_type);
			DW->label_type = NULL;
		}

	if (DW->label_8bits != NULL)
		{
			gtk_widget_destroy (DW->label_8bits);
			DW->label_8bits = NULL;
			gtk_widget_destroy (DW->entry_signed_byte);
			DW->entry_signed_byte = NULL;
			gtk_widget_destroy (DW->entry_unsigned_byte);
			DW->entry_unsigned_byte = NULL;
		}

	if (DW->label_16bits != NULL)
		{
			gtk_widget_destroy (DW->label_16bits);
			DW->label_16bits = NULL;
			gtk_widget_destroy (DW->entry_signed_16bits);
			DW->entry_signed_16bits = NULL;
			gtk_widget_destroy (DW->entry_unsigned_16bits);
			DW->entry_unsigned_16bits = NULL;
		}

	if (DW->label_32bits != NULL)
		{
			gtk_widget_destroy (DW->label_32bits);
			DW->label_32bits = NULL;
			gtk_widget_destroy (DW->entry_signed_32bits);
			DW->entry_signed_32bits = NULL;
			gtk_widget_destroy (DW->entry_unsigned_32bits);
			DW->entry_unsigned_32bits = NULL;
		}
	
	if (DW->label_64bits != NULL)
		{
			gtk_widget_destroy (DW->label_64bits);
			DW->label_64bits = NULL;
			gtk_widget_destroy (DW->entry_signed_64bits);
			DW->entry_signed_64bits = NULL;
			gtk_widget_destroy (DW->entry_unsigned_64bits);
			DW->entry_unsigned_64bits = NULL;
		}

	if (DW->label_dos_date != NULL)
		{
			gtk_widget_destroy (DW->label_dos_date);
			DW->label_dos_date = NULL;
			gtk_widget_destroy (DW->entry_dos_date);
			DW->entry_dos_date = NULL;
		}

	if (DW->label_filetime_date != NULL)
		{
			gtk_widget_destroy (DW->label_filetime_date);
			DW->label_filetime_date = NULL;
			gtk_widget_destroy (DW->entry_filetime_date);
			DW->entry_filetime_date = NULL;
		}

	if (DW->label_C_date != NULL)
		{
			gtk_widget_destroy (DW->label_C_date);
			DW->label_C_date = NULL;
			gtk_widget_destroy (DW->entry_C_date);
			DW->entry_C_date = NULL;
		}
	
	if (is_activate(DW->window_show_8bits_item) == TRUE || 
		 is_activate(DW->window_show_16bits_item) == TRUE ||
		 is_activate(DW->window_show_32bits_item) == TRUE || 
		 is_activate(DW->window_show_64bits_item) == TRUE)
		line++;

	if (is_activate(DW->window_show_8bits_item) == TRUE)
		line++;
		
	if (is_activate(DW->window_show_16bits_item) == TRUE)
		line++;

	if (is_activate(DW->window_show_32bits_item) == TRUE)
		line++;

	if (is_activate(DW->window_show_64bits_item) == TRUE)
		line++;
	  
	if (is_activate(DW->window_show_dos_date_item) == TRUE)
		line++;

	if (is_activate(DW->window_show_filetime_date_item) == TRUE)
		line++;

	if (is_activate(DW->window_show_C_date_item) == TRUE)
		line++;

	DW->table_lines = line;
	
	if (DW->table_lines > 0 && DW->table_lines < G_MAXUINT &&  
		 DW->table_columns > 0 &&  DW->table_columns < G_MAXUINT)
		gtk_table_resize(GTK_TABLE(DW->window_table), DW->table_lines, 
							  DW->table_columns);
}


/* when a menu has been selected */
static void show_menu_item_response(GtkWidget *widget, gpointer data)
{
	data_window_t *DW = (data_window_t *) data;

	resize_window_table(DW);
	display_di_window(DW);
	refresh_data_window(DW->current_hexwidget, DW);
}


static void encoding_menu_item_response(GtkWidget *widget, gpointer data)
{
	data_window_t *DW = (data_window_t *) data;

	refresh_data_window(DW->current_hexwidget, DW);
}


static void font_select_menu_item_response(GtkWidget *widget, gpointer data)
{
	data_window_t *DW = (data_window_t *) data;
	GtkFontSelectionDialog *font_dialog = NULL;
	gint response_id = 0;
	gchar *selected_font;

	font_dialog = GTK_FONT_SELECTION_DIALOG(gtk_font_selection_dialog_new ("Font Dialog Box"));
	if (DW->window_font_name != NULL)
		gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(font_dialog),
															 (gchar *) DW->window_font_name);
	  
   response_id = gtk_dialog_run(GTK_DIALOG (font_dialog));
	
	switch (response_id) {
	case GTK_RESPONSE_OK:
		selected_font = gtk_font_selection_dialog_get_font_name (font_dialog);
		if (DW->window_font_name != NULL)
			g_free(DW->window_font_name);
		
		DW->window_font_name = (gchar *) g_malloc0 ((strlen(selected_font)+1)*sizeof(gchar));
		memcpy(DW->window_font_name, selected_font, (strlen(selected_font)+1));
		refresh_data_window(DW->current_hexwidget, DW);
		break;
	case GTK_RESPONSE_CANCEL:
	default:
		break;
	}

	gtk_widget_destroy (GTK_WIDGET(font_dialog));
}


/* Initialize the menubar, and the Show menu */
static void init_di_menu(data_window_t *DW)
{
  
	/* Here we could load display settings from a configuration file */
	
	/* global menu definition */
	DW->window_menu_bar = gtk_menu_bar_new();

	gtk_box_pack_start(GTK_BOX(DW->window_vbox), DW->window_menu_bar, FALSE, FALSE, 0);

	/* menu 'show' definition */
	DW->window_show_menu = gtk_menu_new ();
	DW->window_show_item = gtk_menu_item_new_with_label ("Show");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (DW->window_show_item), 
										DW->window_show_menu);
	/* menu 'encoding' definition */
	DW->window_encoding_menu = gtk_menu_new ();
	DW->window_encoding_item = gtk_menu_item_new_with_label ("Encoding");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (DW->window_encoding_item), 
										DW->window_encoding_menu);

	/* menu 'font' definition */
	DW->window_font_menu = gtk_menu_new ();
	DW->window_font_item = gtk_menu_item_new_with_label ("Font");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (DW->window_font_item), 
										DW->window_font_menu);

	/* add the menu previously defined to the menu bar */
	gtk_menu_bar_append (GTK_MENU_BAR (DW->window_menu_bar), 
								DW->window_show_item);
	gtk_menu_bar_append (GTK_MENU_BAR (DW->window_menu_bar), 
								DW->window_encoding_item);
	gtk_menu_bar_append (GTK_MENU_BAR (DW->window_menu_bar),
								DW->window_font_item);

	/* The 'Show' menu definitions */
	DW->window_show_8bits_item = gtk_check_menu_item_new_with_label("8 bits");
	gtk_check_menu_item_set_active(
             GTK_CHECK_MENU_ITEM(DW->window_show_8bits_item),
				 TRUE); /* 8 bits display set to TRUE by default */
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_show_menu),
								  DW->window_show_8bits_item);

	DW->window_show_16bits_item = gtk_check_menu_item_new_with_label("16 bits");
	gtk_check_menu_item_set_active(
				 GTK_CHECK_MENU_ITEM(DW->window_show_16bits_item),
				 TRUE); /* 16 bits display set to TRUE by default */
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_show_menu),
								  DW->window_show_16bits_item);

	DW->window_show_32bits_item = gtk_check_menu_item_new_with_label("32 bits");
	gtk_check_menu_item_set_active(
             GTK_CHECK_MENU_ITEM(DW->window_show_32bits_item),
				 FALSE); /* 32 bits display set to FALSE by default */
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_show_menu),
								  DW->window_show_32bits_item);

	DW->window_show_64bits_item = gtk_check_menu_item_new_with_label("64 bits");
	gtk_check_menu_item_set_active(
				 GTK_CHECK_MENU_ITEM(DW->window_show_64bits_item),
				 FALSE); /* 64 bits display set to FALSE by default */
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_show_menu),
								  DW->window_show_64bits_item);

	DW->window_show_dos_date_item = 
		gtk_check_menu_item_new_with_label("dos date");
	gtk_check_menu_item_set_active(
             GTK_CHECK_MENU_ITEM(DW->window_show_dos_date_item),
				 TRUE); /* dos date display set to TRUE by default */
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_show_menu),
								  DW->window_show_dos_date_item);

	DW->window_show_filetime_date_item = 
		gtk_check_menu_item_new_with_label("filetime date");
	gtk_check_menu_item_set_active(
		       GTK_CHECK_MENU_ITEM(DW->window_show_filetime_date_item),
				 TRUE); /* filetime date display set to TRUE by default */
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_show_menu),
								  DW->window_show_filetime_date_item);

	DW->window_show_C_date_item = 
		gtk_check_menu_item_new_with_label("C date");
	gtk_check_menu_item_set_active(
		       GTK_CHECK_MENU_ITEM(DW->window_show_C_date_item),
				 TRUE);/* C date display set to TRUE by default */
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_show_menu),
								  DW->window_show_C_date_item);
	/* End of the 'Show' menu definitions */

	/* Begining of the 'Encoding' menu definitions */
	DW->window_enc_b_e_item = 
		gtk_check_menu_item_new_with_label("Big endian");
	gtk_check_menu_item_set_active(
		       GTK_CHECK_MENU_ITEM(DW->window_enc_b_e_item),
				 FALSE); /* by default we do decode little endian */
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_encoding_menu),
								  DW->window_enc_b_e_item);
	/* End of the 'Encoding' menu definitions */

	/* Begining of the 'Font' menu definitions */
	DW->window_font_select_item = gtk_menu_item_new_with_label("Select");
	gtk_menu_shell_append (GTK_MENU_SHELL (DW->window_font_menu),
								  DW->window_font_select_item);
	/* End of the 'Font' menu definitions */

	/* signal handling Show menu items */
	g_signal_connect (G_OBJECT (DW->window_show_8bits_item), "activate",
							G_CALLBACK (show_menu_item_response), DW);
	g_signal_connect (G_OBJECT (DW->window_show_16bits_item), "activate",
							G_CALLBACK (show_menu_item_response), DW);
	g_signal_connect (G_OBJECT (DW->window_show_32bits_item), "activate",
							G_CALLBACK (show_menu_item_response), DW);
	g_signal_connect (G_OBJECT (DW->window_show_64bits_item), "activate",
							G_CALLBACK (show_menu_item_response), DW);
	g_signal_connect (G_OBJECT (DW->window_show_dos_date_item), "activate",
							G_CALLBACK (show_menu_item_response), DW);
	g_signal_connect (G_OBJECT (DW->window_show_filetime_date_item), "activate",
							G_CALLBACK (show_menu_item_response), DW);
	g_signal_connect (G_OBJECT (DW->window_show_C_date_item), "activate",
							G_CALLBACK (show_menu_item_response), DW);

	/* signal handling Encoding menu item */
	g_signal_connect (G_OBJECT (DW->window_enc_b_e_item), "activate",
							G_CALLBACK (encoding_menu_item_response), DW);
	
	/* signal handling Select menu item */
	g_signal_connect (G_OBJECT (DW->window_font_select_item), "activate",
							G_CALLBACK (font_select_menu_item_response), DW);
}


/*
  All malloc and such things should have been done !
  Initialise the data_window_t whose aim is to 
  display the data interpreted. Following interpret
  is done automatically :
  8, 16, 32 and 64 bits, dos date and filetime date !
*/
void data_interpret(data_window_t *DW)
{
	/* Default font name used to display things */
	DW->window_font_name = (gchar *) 
		g_malloc0 ((strlen(H_DI_FONT_FAMILY)+ strlen(H_DI_FONT_STYLE)+
						strlen(H_DI_FONT_WIDTH)+3)*sizeof(gchar));
	sprintf(DW->window_font_name, "%s %s %s", H_DI_FONT_FAMILY, 
			  H_DI_FONT_STYLE, H_DI_FONT_WIDTH);

	/* default value */
	DW->window_displayed = H_DI_DISPLAYED;

	DW->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	DW->window_vbox = gtk_vbox_new (FALSE, H_DI_V_SPACE);
	gtk_container_add (GTK_CONTAINER(DW->window), DW->window_vbox);

  	init_di_menu(DW);

	DW->table_lines = H_DI_LINES;
	DW->table_columns = H_DI_COLUMNS;
	DW->window_table = gtk_table_new (DW->table_lines, DW->table_columns, TRUE);
	gtk_container_add (GTK_CONTAINER(DW->window_vbox), DW->window_table);
	
	DW->window_statusbar = gtk_statusbar_new () ;
 
	gtk_box_pack_end(GTK_BOX(DW->window_vbox), DW->window_statusbar, FALSE, FALSE, 0);

	display_statusbar(DW);
	display_di_window(DW);

	refresh_data_window(DW->current_hexwidget, DW);
	
	if (DW->window_displayed == TRUE)
		gtk_widget_show_all (DW->window);
}
/* End of Widget related stuff  */

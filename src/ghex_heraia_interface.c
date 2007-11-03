/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  ghex_heraia_interface.c
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


/**
 *  kills the old document if it exists and add a new one, from the new filename
 */
HERAIA_ERROR heraia_hex_document_new(heraia_window_t *main_window, char *filename) 
{
	if (main_window->current_doc != NULL)
		hex_document_remove_view(main_window->current_doc, main_window->current_DW->current_hexwidget);
	
	if (main_window->current_DW->current_hexwidget != NULL )
		gtk_widget_destroy(main_window->current_DW->current_hexwidget);
	
	main_window->current_doc = hex_document_new_from_file(filename);
	main_window->current_DW->current_hexwidget = hex_document_add_view(main_window->current_doc);
	
	connect_cursor_moved_signal(main_window);

	return HERAIA_NOERR;
}


/**
 *  Returns 'len' number of bytes located at 'pos' in the GtkHex 
 *  document and puts it in the result variable
 *
 *  We assume that a previous g_malloc has been done in order to
 *  use the function. Here we need the "swap_bytes" function
 *  defined in the decode.h header in order to take the endianness
 *  into account
 */
gboolean ghex_memcpy(GtkHex *gh, guint pos, guint len, guint endianness, guchar *result) 
{
	guint i;
	
	if (result == NULL || gh == NULL)
		{
			return FALSE;
		}
	else if ((pos < 0) || ((pos+len) > ghex_file_size(gh)))
		{
			return FALSE;
		}
	else
		{
			for (i=0; i<len ; i++)
				{
					result[i] = gtk_hex_get_byte(gh, pos+i);
				}

			if (endianness == H_DI_BIG_ENDIAN)
				{
					if (len > 1)
						{
							swap_bytes(result, 0, len-1);
						}
					else
						{
							reverse_byte_order(result);  /* Only one byte and big endian requested */
						} 
				}
			else if (endianness == H_DI_MIDDLE_ENDIAN && len >= 4)
				{
					swap_bytes(result, 0, (len/2)-1);
					swap_bytes(result, (len/2), len-1);
				}

			return TRUE;
		}
}

/**
 *  Returns the file size of an opened GtkHex document.
 */
guint64 ghex_file_size(GtkHex *gh)
{
	if ( gh != NULL && gh->document != NULL)
		{
			return gh->document->file_size;
		}
	else
		{
			return 0;
		}
}

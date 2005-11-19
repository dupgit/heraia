/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  ghex_heraia_interface.c
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

#include <gtk/gtk.h>
#include <gtkhex/gtkhex.h>

#include "ghex_heraia_interface.h"
#include "decode.h"

/* 
   kills the old document if it exists and add a new one, from the new filename
*/
HERAIA_ERROR heraia_hex_document_new(heraia_window_t *main_window, char *filename) 
{
	if (main_window->current_doc != NULL)
		hex_document_remove_view(main_window->current_doc, main_window->current_DW->current_hexwidget);
	
	if (main_window->current_DW->current_hexwidget != NULL )
		gtk_widget_destroy (main_window->current_DW->current_hexwidget);
	
	main_window->current_doc = hex_document_new_from_file (filename);
	main_window->current_DW->current_hexwidget = hex_document_add_view(main_window->current_doc);
  
	return HERAIA_NOERR;
}

/* 
	Returns 'len' number of bytes located at 'pos' in th GtkHex 
   document and puts it in the result variable

	We assume that a previous g_malloc has been done in order to
   use the function.
*/
gboolean ghex_memcpy(GtkHex *gh, guint pos, guint len, gboolean big_endian, 
							guchar *result) 
{
  guint i;
	
  if (result == NULL || gh == NULL)
    return FALSE;
  else if ((pos < 0) && ((pos+len) > gh->document->file_size))
    return FALSE;
  else
    {
		 for (i=0; i<len ; i++)
			 result[i] = gtk_hex_get_byte(gh, pos+i);

		 if (big_endian == TRUE)
			 swap_bytes(result, 0, len-1);

      return TRUE;
    }
}

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

static gboolean swap_bytes(guchar *to_swap, guint first, guint last);

static gboolean swap_bytes(guchar *to_swap, guint first, guint last)
{
	guchar aux;

	if (first >= last)
		return TRUE;
	else
		{
			aux = to_swap[first];
			to_swap[first] = to_swap[last];
			to_swap[last] = aux;
			return swap_bytes(to_swap, ++first, --last);
		}
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

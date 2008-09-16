/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  ghex_heraia_interface.c
  heraia - an hexadecimal file editor and analyser based on ghex
 
  (C) Copyright 2005 - 2008 Olivier Delhomme
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
 *  Removes the old document if it exists and adds a new one
 *  from the filename 'filename'
 */
HERAIA_ERROR heraia_hex_document_new(heraia_window_t *main_window, char *filename) 
{
	if (main_window->current_doc != NULL)
		{
			hex_document_remove_view(main_window->current_doc, main_window->current_DW->current_hexwidget);
		}
	
	if (main_window->current_DW->current_hexwidget != NULL )
		{
			gtk_widget_destroy(main_window->current_DW->current_hexwidget);
		}
	
	main_window->current_doc = hex_document_new_from_file(filename);
	main_window->current_DW->current_hexwidget = hex_document_add_view(main_window->current_doc);
	
	connect_cursor_moved_signal(main_window);

	return HERAIA_NOERR;
}


/**
 * Retrieves the filename of a document which ever it is !
 */
gchar *heraia_hex_document_get_filename(Heraia_Document *doc)
{
	return doc->file_name;
}


/**
 * Saves an open and edited document
 */
HERAIA_ERROR heraia_hex_document_save(heraia_window_t *main_window)
{
	gint return_value = FALSE;
	
	if (main_window->current_doc != NULL)
	   {
				return_value = hex_document_write(main_window->current_doc);
		}
	
	if (return_value != FALSE)
	   {
				return HERAIA_NOERR;
		}
	 else
	   {	
				return HERAIA_FILE_ERROR;
	   }
}

/**
 * Saves an opened and edited document to a new file
 */
HERAIA_ERROR heraia_hex_document_save_as(heraia_window_t *main_window, gchar *filename)
{
	gint return_value = FALSE;
	FILE *fp = NULL;
	gint i = 0;
	gchar *path_end = NULL;
	
	if (main_window->current_doc != NULL && filename != NULL)
	   {
			fp = fopen(filename, "w");
			if (fp != NULL)
			{
				return_value = hex_document_write_to_file(main_window->current_doc, fp);
				fclose(fp);
				
				if (main_window->current_doc->file_name)
				 {
					 g_free(main_window->current_doc->file_name);
				 }
				main_window->current_doc->file_name = filename;
				
				/* This may disappear as it duplicates structures */
				if (main_window->filename != NULL)
				{
					 g_free(main_window->filename);
				}
				main_window->filename = g_strdup_printf("%s", main_window->current_doc->file_name);
				
				/* path_end stuff from ghex-window.c from ghex project !!! */
				for(i = strlen(main_window->current_doc->file_name);
                        (i >= 0) && (main_window->current_doc->file_name[i] != '/');
                        i--);
				if (main_window->current_doc->file_name[i] == '/')
                path_end = &main_window->current_doc->file_name[i+1];
				else
               path_end = main_window->current_doc->file_name;
            
				main_window->current_doc->path_end = g_filename_to_utf8(path_end, -1, NULL, NULL, NULL);
			}
		}
	
	if (return_value != FALSE)
	   {
				return HERAIA_NOERR;
		}
	 else
	   {	
				return HERAIA_FILE_ERROR;
	   }
}

/**
 *  Deals with the endianness of 'len' bytes located in 'result'
 *  for BIG_ENDIAN we only swap bytes if we have two or more of them
 *  if we have only one byte, we reverse its order
 *  if endianness is MIDDLE_ENDIAN we swap only four or more bytes
 *  Here we might have funny things with len corresponding to 24 or 56 bits
 *  for example
 *  Assumption is made that the default order is LITTLE_ENDIAN (which may
 *  not be true on some systems !)
 *  We fo assume that 'result' really contains 'len' bytes of data previously
 *  gmalloc'ed
 */
static void change_endianness(guint len, guint endianness, guchar *result)
{
	if (endianness == H_DI_BIG_ENDIAN)
		{
			if (len > 1) /* We swap bytes only if we have two or more */
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
	else if ((pos < 0) || ((pos+len) > ghex_file_size(gh))) /* pos located in the file limits ! */ 
		{
			return FALSE;
		}
	else
		{
			/* Extracts len bytes from the Ghex widget */
			for (i=0; i<len ; i++)
				{
						result[i] = gtk_hex_get_byte(gh, pos+i);
				}
			
			/* Deals with endianness to rearrange datas */
			change_endianness(len, endianness, result);

			return TRUE;
		}
}



/**
 *  Gets the data from the hexwidget, a wrapper to the ghex_memcpy
 *  function. guchar *c MUST have been pre allocated BEFORE the call.
 *  endianness == H_DI_BIG_ENDIAN, H_DI_MIDDLE_ENDIAN or H_DI_LITTLE_ENDIAN
 *  length can be anything but MUST be strictly less than the size allocated 
 *  to *c
 */
gboolean ghex_get_data(data_window_t *data_window, guint length, guint endianness, guchar *c)
{
	GtkHex *gh = NULL;
	gboolean result = FALSE;

	gh = GTK_HEX(data_window->current_hexwidget);

	if (gh != NULL)
		{
			result = ghex_memcpy(gh, gtk_hex_get_cursor(gh), length, endianness, c);
		}
	else
		{
			result = FALSE;
		}
	
	return result;
}


/**
 *  Returns the file size of an opened GtkHex document.
 */
guint64 ghex_file_size(GtkHex *gh)
{
	if (gh != NULL && gh->document != NULL)
		{
			return gh->document->file_size;
		}
	else
		{
			return 0;
		}
}

/**
 *  Retrieves the cursor's position from the hexwidget
 */
guint64 ghex_get_cursor_position(data_window_t *data_window)
{
	GtkHex *gh = NULL;
	
	gh = GTK_HEX(data_window->current_hexwidget);
	
	if (gh != NULL)
	{
		  return gtk_hex_get_cursor(gh);
	}
	else
	{
		  return 0;
	}
}

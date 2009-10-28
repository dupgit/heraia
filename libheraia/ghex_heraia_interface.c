/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  ghex_heraia_interface.c
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
 * @file ghex_heraia_interface.c
 * An interface to the ghex library -> this adds an abstract layer.
 */
#include <libheraia.h>

/**
 * @fn HERAIA_ERROR heraia_hex_document_new(heraia_struct_t *main_struct, char *filename)
 *  Removes the old document if it exists and adds a new one
 *  from the filename 'filename'
 * @param main_struct : main structure
 * @param filename : a char * representing an existing file named "filename"
 * @return Always returns HERAIA_NOERR; @todo : do something to take errors into
 *         account
 */
doc_t *heraia_hex_document_new(heraia_struct_t *main_struct, char *filename)
{
    Heraia_Document *hex_doc = NULL;
    GtkWidget *hex_widget = NULL;
    doc_t *doc = NULL;

    /* Creating a new hex document */
    hex_doc = hex_document_new_from_file(filename);

    if (hex_doc != NULL)
     {
        /* creating a new view to this new document */
        hex_widget = hex_document_add_view(hex_doc);

        /* joining those two new structures in one */
        doc = new_doc_t(hex_doc, hex_widget);

        /* Adding this new doc to the list of docs (here a GPtrArray) */
        g_ptr_array_add(main_struct->documents, doc);

        /* signal connection on cursor moves */
        connect_cursor_moved_signal(main_struct, hex_widget);

        return doc;
     }
     else
     {
         return NULL;
     }
}


/**
 * Retrieves the filename of a document which ever it is !
 * @param doc : an Heraia_Document
 * @return returns the filename of that document.
 */
gchar *heraia_hex_document_get_filename(Heraia_Document *hex_doc)
{
    if (hex_doc != NULL)
    {
        return hex_doc->file_name;
    }
    else
    {
        return NULL;
    }
}

/**
 * Retrieves from a doc_t * document it's filename, which ever it is
 * @param doc : an existing doc_t
 * @return returns the filename of that document.
 */
gchar *doc_t_document_get_filename(doc_t *doc)
{
    if (doc != NULL)
    {
        return heraia_hex_document_get_filename(doc->hex_doc);
    }
    else
    {
        return NULL;
    }
}



/**
 * @fn HERAIA_ERROR heraia_hex_document_save(heraia_struct_t *main_struct)
 * Saves an open and edited document
 * @param current_doc : current edited document (doc_t * structure)
 * @return returns HERAIA_NOERR if everything went ok or HERAIA_FILE_ERROR in case of an error
 */
HERAIA_ERROR heraia_hex_document_save(doc_t *current_doc)
{
    gint return_value = FALSE;

    if (current_doc != NULL)
    {
        if (current_doc->hex_doc != NULL)
        {
            return_value = hex_document_write(current_doc->hex_doc);
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
 * Saves an opened and edited document to a new file
 * @param current_doc : current edited document (doc_t * structure)
 * @param filename : the new filename where to save the file
 * @return returns HERAIA_NOERR if everything went ok or HERAIA_FILE_ERROR in case of an error
 */
HERAIA_ERROR heraia_hex_document_save_as(doc_t *current_doc, gchar *filename)
{
    gint return_value = FALSE;
    FILE *fp = NULL;
    gint i = 0;
    gchar *path_end = NULL; /**< to make libghex happy ! */

    if (current_doc != NULL && current_doc->hex_doc != NULL && filename != NULL)
       {
            fp = fopen(filename, "w");
            if (fp != NULL)
            {
                return_value = hex_document_write_to_file(current_doc->hex_doc, fp);
                fclose(fp);

                /* path_end stuff from ghex-window.c from ghex project !!! */
                for(i = strlen(current_doc->hex_doc->file_name);
                        (i >= 0) && (current_doc->hex_doc->file_name[i] != '/');
                        i--);
                if (current_doc->hex_doc->file_name[i] == '/')
                    path_end = &current_doc->hex_doc->file_name[i+1];
                else
                    path_end = current_doc->hex_doc->file_name;

                current_doc->hex_doc->path_end = g_filename_to_utf8(path_end, -1, NULL, NULL, NULL);
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
 *  @warning Assumption is made that the default order is LITTLE_ENDIAN (which may
 *           not be true on some systems !)
 *  @warning We do assume that 'result' really contains 'len' bytes of data previously
 *           gmalloc'ed
 *  @param len : len bytes to change endianness
 *  @param endianness : H_DI_BIG_ENDIAN or H_DI_MIDDLE_ENDIAN we consider that there
 *         is nothing to do with H_DI_LITTLE_ENDIAN
 *  @param[in,out] result : contains the bytes to be swaped and at the end, contains
 *                 the result.
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
 *  @warning We assume that a previous g_malloc has been done in order to
 *           use the function. Here we need the "swap_bytes" function
 *           defined in the decode.h header in order to take the endianness
 *           into account
 *  @param gh : A GtkHex document.
 *  @param pos : position where we want to begin to copy bytes
 *  @param len : number of bytes we want to copy
 *  @param endianness : endianness we want to apply to the bytes we want to copy
 *  @param[out] result : a previously g_malloc'ed gchar * string that will contain
 *              copied bytes.
 *  @return TRUE if everything went ok, FALSE otherwise
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
 *  function.
 *  @warning guchar *c MUST have been pre allocated BEFORE the call.
 *
 * @param data_window : data interpretor window structure
 * @param length : can be anything but MUST be strictly less than the size allocated
 *        to *c
 * @param endianness : H_DI_BIG_ENDIAN, H_DI_MIDDLE_ENDIAN or H_DI_LITTLE_ENDIAN
 * @param c : a previously g_malloc'ed gchar * string that will contain
 *              copied bytes.
 */
gboolean ghex_get_data(GtkWidget *hex_widget, guint length, guint endianness, guchar *c)
{
    gboolean result = FALSE;
    GtkHex *gh = GTK_HEX(hex_widget);

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
 * @param gh : an opened GtkHex document
 * @return resturns the file size of that document
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
 *  Retrieves the cursor's position from the current hexwidget
 * @param data_window : data interpretor window structure
 * @return returns the cursor's position
 */
guint64 ghex_get_cursor_position(GtkWidget *hex_widget)
{
    GtkHex *gh = GTK_HEX(hex_widget);

    if (gh != NULL)
    {
          return gtk_hex_get_cursor(gh);
    }
    else
    {
          return 0;
    }
}


/**
 * Inits a doc_t structure
 * @param doc : hex_document but encapsulated in Heraia_Document
 *              structure
 * @param hexwidget : Widget to display an hexadecimal view of the file
 * @return returns a newly allocated doc_t structure
 */
doc_t *new_doc_t(Heraia_Document *hex_doc, GtkWidget *hex_widget)
{
    doc_t *new_doc;

    new_doc = (doc_t *) g_malloc0(sizeof(doc_t));

    new_doc->hex_doc = hex_doc;
    new_doc->hex_widget = hex_widget;

    return new_doc;
}

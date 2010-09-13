/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  ghex_heraia_interface.c
  heraia - an hexadecimal file editor and analyser based on ghex

  (C) Copyright 2005 - 2010 Olivier Delhomme
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
 * @return Always returns HERAIA_NOERR;
 * @todo : do something to take errors into account
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

            /* displaying the offsets if requested */
            gtk_hex_show_offsets(GTK_HEX(hex_widget), is_toggle_button_activated(main_struct->xmls->main, "mp_display_offset_bt"));

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
                        {
                            path_end = &current_doc->hex_doc->file_name[i+1];
                        }
                    else
                        {
                            path_end = current_doc->hex_doc->file_name;
                        }

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
 *  Returns 'len' number of bytes located at 'pos' in the Heraia_Hex
 *  document and puts it in the result variable
 *
 *  @warning We assume that a previous g_malloc has been done in order to
 *           use the function. Here we need the "swap_bytes" function
 *           defined in the decode.h header in order to take the endianness
 *           into account
 *  @param gh : A Heraia_Hex document.
 *  @param pos : position where we want to begin to copy bytes
 *  @param len : number of bytes we want to copy
 *  @param endianness : endianness we want to apply to the bytes we want to copy
 *  @param[out] result : a previously g_malloc'ed gchar * string that will contain
 *              copied bytes.
 *  @return TRUE if everything went ok, FALSE otherwise
 */
gboolean ghex_memcpy(Heraia_Hex *gh, guint64 pos, guint len, guint endianness, guchar *result)
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
 *  Gets the data from the hexwidget under the cursor, a wrapper to the
 *  ghex_memcpy function.
 *  @warning guchar *c MUST have been pre allocated BEFORE the call.
 *
 * @param hex_widget : MUST be a Heraia_Hex widget
 * @param length : can be anything but MUST be strictly less than the size allocated
 *        to *c
 * @param endianness : H_DI_BIG_ENDIAN, H_DI_MIDDLE_ENDIAN or H_DI_LITTLE_ENDIAN
 * @param c : a previously g_malloc'ed gchar * string that will contain
 *            copied bytes.
 */
gboolean ghex_get_data(GtkWidget *hex_widget, guint length, guint endianness, guchar *c)
{
    Heraia_Hex *gh = GTK_HEX(hex_widget);

    if (gh != NULL)
        {
            return ghex_get_data_position(hex_widget, gtk_hex_get_cursor(gh), length, endianness, c);
        }
    else
        {
            return FALSE;
        }
}


/**
 *  Gets the data from the hexwidget, a wrapper to the ghex_memcpy
 *  function.
 *  @warning guchar *c MUST have been pre allocated BEFORE the call.
 *
 * @param hex_widget : MUST be a Heraia_Hex widget
 * @param pos : position in the file where we want to get the data
 * @param length : can be anything but MUST be strictly less than the size allocated
 *        to *c
 * @param endianness : H_DI_BIG_ENDIAN, H_DI_MIDDLE_ENDIAN or H_DI_LITTLE_ENDIAN
 * @param c : a previously g_malloc'ed gchar * string that will contain
 *            copied bytes.
 */
gboolean ghex_get_data_position(GtkWidget *hex_widget, guint64 pos, guint length, guint endianness, guchar *c)
{
    gboolean result = FALSE;
    Heraia_Hex *gh = GTK_HEX(hex_widget);

    if (gh != NULL)
        {
            result = ghex_memcpy(gh, pos, length, endianness, c);
        }
    else
        {
            result = FALSE;
        }

    return result;
}


/**
 * Gets data from a defined position and formats it in an ascii displayable form
 * @param hex_widget : MUST be a Heraia_Hex widget
 * @param pos : position in the file where we want to get the data
 * @param length : length of the data to get
 * @param endianness : H_DI_BIG_ENDIAN, H_DI_MIDDLE_ENDIAN or H_DI_LITTLE_ENDIAN
 * @return the newly allocated string that contains the ascii translation from
 *         the data or NULL if something went wrong.
 */
guchar *ghex_get_data_to_ascii(GtkWidget *hex_widget, guint64 pos, guint length, guint endianness)
{
    Heraia_Hex *gh = GTK_HEX(hex_widget);
    guchar *c = NULL;
    guchar *result = NULL;
    gint i = 0;

    c = (guchar *) g_malloc0 (sizeof(guchar)*(length + 1));

    if (ghex_memcpy(gh, pos, length, endianness, c))
        {

            result = (guchar *) g_malloc0 (sizeof(guchar)*(length + 1));

            for(i = 0; i < length; i++)
                {
                    if (is_displayable(c[i]))
                        {
                            result[i] = c[i];
                        }
                    else
                        {
                            result[i] = '.';
                        }
                }

            i++;
            result[i] = (guchar) 0;

            g_free(c);

            return result;
        }
    else
        {
            g_free(c);
            return NULL;
        }
}


/**
 * Gets data from a defined position and formats it in an hex displayable form
 * @param hex_widget : MUST be a Heraia_Hex widget
 * @param pos : position in the file where we want to get the data
 * @param length : length of the data to get
 * @param endianness : H_DI_BIG_ENDIAN, H_DI_MIDDLE_ENDIAN or H_DI_LITTLE_ENDIAN
 * @return the newly allocated string that contains the ascii translation from
 *         the data or NULL if something went wrong.
 */
guchar *ghex_get_data_to_hex(GtkWidget *hex_widget, guint64 pos, guint length, guint endianness)
{
    Heraia_Hex *gh = GTK_HEX(hex_widget);
    static guchar hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    guchar *c = NULL;
    guchar *result = NULL;
    gint i = 0;
    gint j = 0;
    gint high = 0;
    gint low = 0;

    c = (guchar *) g_malloc0 (sizeof(guchar)*(length + 1));

    if (ghex_memcpy(gh, pos, length, endianness, c))
        {

            result = (guchar *) g_malloc0 (sizeof(guchar)*( 3 * length + 1));

            j = 0;
            for(i = 0; i < length; i++)
                {
                    low = c[i] & 0x0F;
                    high = (c[i] & 0xF0) >> 4;

                    result[j] = hex[high];
                    j++;
                    result[j] = hex[low];
                    j++;

                    if (i % gh->group_type == 0)
                        {
                            result[j] = ' ';
                            j++;
                        }
                }

            j++;
            result[j] = (guchar) 0;

            g_free(c);

            return result;
        }
    else
        {
            g_free(c);
            return NULL;
        }
}

/**
 *  Returns the file size of an opened Heraia_Hex document.
 * @param gh : the widget of a an opened Heraia_Hex document
 * @return returns the file size of that document
 */
guint64 ghex_file_size(Heraia_Hex *gh)
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
 * @param hex_widget : the widget that displays the hex document
 * @return returns the cursor's position
 */
guint64 ghex_get_cursor_position(GtkWidget *hex_widget)
{
    Heraia_Hex *gh = GTK_HEX(hex_widget);

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
 * Sets the cursor at the defined position in the hexwidget
 * @param hex_widget : the widget that displays the hex document
 * @param position : the position where we want to go
 * @warning no checks are made here (limits and such ...)
 */
void ghex_set_cursor_position(GtkWidget *hex_widget, guint64 position)
{

    Heraia_Hex *gh = GTK_HEX(hex_widget);

    if (gh != NULL)
        {
              gtk_hex_set_cursor(gh, position);
        }
}

/** @todo a uniq function that will unify ghex_find_forward and ghex_find_backward */
/**
 * Wrapper to the hex_document_find_forward function
 * Tries to find search_buffer in doc
 * @param doc : the document searched
 * @param search_buffer : the string searched for
 * @param buffer_size : size of the buffer
 * @param[out] : position (if any) of the found string
 * @return True if something has been found. False otherwise
 */
gboolean ghex_find_forward(doc_t *doc, guchar *search_buffer, guint buffer_size, guint64 *position)
{
    guint64 current_position = 0;
    guint offset = 0;
    gboolean result = FALSE;

    if (doc != NULL && doc->hex_widget != NULL && doc->hex_doc != NULL)
        {
            current_position = *position;
            result = hex_document_find_forward(doc->hex_doc, current_position + 1, search_buffer, buffer_size, &offset);

            if (result == TRUE)
                {
                    *position = (guint64) offset;
                    return TRUE;
                }
            else
                {
                    *position = 0;
                    return FALSE;
                }
        }
    else
        {
            *position = 0;
            return FALSE;
        }
}


/**
 * Wrapper to the hex_document_find_backward function
 * Tries to find search_buffer in doc
 * @param doc : the document searched
 * @param search_buffer : the string searched for
 * @param buffer_size : size of the buffer
 * @param[out] : position (if any) of the found string
 * @return True if something has been found. False otherwise
 */
gboolean ghex_find_backward(doc_t *doc, guchar *search_buffer, guint buffer_size, guint64 *position)
{
    guint64 current_position = 0;
    guint offset = 0;
    gboolean result = FALSE;

    if (doc != NULL && doc->hex_widget != NULL && doc->hex_doc != NULL)
        {
            current_position = *position;
            result = hex_document_find_backward(doc->hex_doc, current_position - 1, search_buffer, buffer_size, &offset);

            if (result == TRUE)
                {
                    *position = (guint64) offset;
                    return TRUE;
                }
            else
                {
                    *position = 0;
                    return FALSE;
                }
        }
    else
        {
            *position = 0;
            return FALSE;
        }
}


/**
 * Retrieves the selection made (if any) in the hex widget
 * @param hex_widget : the widget that displays the hex document
 * @return returns a filed selection_t structure
 */
selection_t *ghex_get_selection(GtkWidget *hex_widget)
{
    Heraia_Hex *gh = GTK_HEX(hex_widget);
    selection_t *sel = NULL;

    if (gh != NULL)
        {
            sel = (selection_t *) g_malloc0(sizeof(selection_t));

            if (gh->selection.start < gh->selection.end)
                {
                    sel->start = gh->selection.start;
                    sel->end = gh->selection.end;
                }
            else
                {
                    sel->end = gh->selection.start;
                    sel->start = gh->selection.end;
                }

            return sel;
        }
    else
        {
            return NULL;
        }
}


/**
 * Inits a doc_t structure
 * @param hex_doc : hex_document but encapsulated in Heraia_Document
 *                  structure
 * @param hex_widget : Widget to display an hexadecimal view of the file
 * @return returns a newly allocated doc_t structure
 */
doc_t *new_doc_t(Heraia_Document *hex_doc, GtkWidget *hex_widget)
{
    doc_t *new_doc;

    new_doc = (doc_t *) g_malloc0(sizeof(doc_t));

    new_doc->hex_doc = hex_doc;
    new_doc->hex_widget = hex_widget;
    new_doc->modified = hex_doc->changed; /**@todo do a function to access this value **/

    return new_doc;
}


/**
 * Closes a previously malloced doc_t structure
 * @param current_doc
 */
void close_doc_t(doc_t *current_doc)
{

    if (current_doc != NULL)
        {
            gtk_widget_destroy(current_doc->hex_widget);
            g_free(current_doc);
        }
}

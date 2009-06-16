/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_io.c
  heraia_io.c - input and output functions for heraia

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
/**
 * @file heraia_io.c
 * Here I want to see everything that deals with I/O, files, disk and so on.
 */
#include <libheraia.h>

static GladeXML *load_glade_xml_if_it_exists(char *file_to_load);

/**
 *  Loads the file 'filename' to analyse and populates the
 *  corresponfing structure 'main_window' as needed thus
 *  main_window and filename must NOT be NULL pointers
 * @param main_window : main structure (it must not be NULL)
 * @param filename : filename of the file to load (it must not be NULL)
 * @return TRUE if everything went ok, FALSE otherwise
 */
gboolean load_file_to_analyse(heraia_window_t *main_window, gchar *filename)
{
	struct stat *stat_buf = NULL;
	gboolean success = FALSE;
/*	GtkWidget *notebook = NULL; */
	doc_t* doc = NULL;

	g_return_val_if_fail(filename != NULL, FALSE);
	g_return_val_if_fail(main_window != NULL, FALSE);

	stat_buf = (struct stat *) g_malloc0 (sizeof(struct stat));
	stat(filename, stat_buf);

	log_message(main_window, G_LOG_LEVEL_DEBUG, "filename to load : %s", filename);

	if (S_ISREG(stat_buf->st_mode) && stat_buf->st_size>0)
		{

			doc = heraia_hex_document_new(main_window, filename); /* removes the old hexdocument and adds a new one */

			if (doc != NULL)
			{
				add_new_tab_in_main_window(main_window, doc);

				log_message(main_window, G_LOG_LEVEL_DEBUG, "Hexwidget : %p", doc->hex_widget);

				success = TRUE;

				/* updating the window name and tab's name */
				update_main_window_name(main_window);
				set_notebook_tab_name(main_window);

				/* Showing all the widgets */
				grey_main_widgets(main_window->xmls->main, FALSE);

				log_message(main_window, G_LOG_LEVEL_DEBUG, "file %s loaded !", filename);
			}
			else
			{
				log_message(main_window, G_LOG_LEVEL_ERROR, "Error while trying to load file %s", filename);
				success = FALSE;
			}

		}
	else
		{
			if (S_ISREG(stat_buf->st_mode))
				{
					log_message(main_window, G_LOG_LEVEL_WARNING, "The file %s is empty !", filename);
				}
			else
				{
					log_message(main_window, G_LOG_LEVEL_WARNING, "The file %s does not exist !", filename);
				}
			success = FALSE;
		}

	g_free(stat_buf);

	return success;
}


/**
 * @fn GladeXML *load_glade_xml_if_it_exists(gchar *file_to_load)
 *  Checks if file_to_load exists and is valid and if possible, loads it
 *  in the xml structure
 * @param file_to_load : a filename of a possibly existing glade file
 * @return returns the GladeXML structure if any, NULL otherwise
 */
static GladeXML *load_glade_xml_if_it_exists(gchar *file_to_load)
{
	struct stat *stat_buf;
	GladeXML *xml = NULL;

	stat_buf = (struct stat *) g_malloc0 (sizeof(struct stat));

	stat(file_to_load, stat_buf);
	if (S_ISREG(stat_buf->st_mode) && stat_buf->st_size>0)
		{
			xml = glade_xml_new(file_to_load, NULL, NULL);
		}
	else
		{
			xml = NULL;
		}

	g_free(stat_buf);

	return xml;
}

/**
 * @fn GladeXML *load_glade_xml_file(GList *location_list, gchar *filename)
 *  loads the glade xml file ('filename') that describes an interface,
 *  tries all the paths defined in the location_list and put the definition
 *  in the 'xml' variable. A frontend to load_glade_xml_if_it_exists function
 * @param location_list : a Glist containing paths where we might found the file
 * @param filename : glade's file's name that we want to load (possibly)
 * @return returns the GladeXML structure if any, NULL otherwise
 */
GladeXML *load_glade_xml_file(GList *location_list, gchar *filename)
{
	gchar *file_to_load = NULL;
	GList *list = g_list_first(location_list);
	GladeXML *xml = NULL;

	while (list != NULL && xml == NULL)
		{
			file_to_load =  g_build_filename((gchar *) list->data, filename, NULL);

			xml = load_glade_xml_if_it_exists(file_to_load);

			if (xml == NULL)
				{
					list = list->next;
				}
			g_free(file_to_load);
		}

	return xml;
}


/**
 * @fn gboolean load_preference_file(heraia_window_t *main_window)
 *  Load the preference file
 * @param main_window : main structure
 * @return TRUE if everything went ok, FALSE otherwise
 */
gboolean load_preference_file(heraia_window_t *main_window)
{
	if (main_window != NULL && main_window->prefs != NULL)
	{
		return g_key_file_load_from_file(main_window->prefs->file, main_window->prefs->filename,  G_KEY_FILE_KEEP_COMMENTS & G_KEY_FILE_KEEP_TRANSLATIONS, NULL);
	}
	else
	{
		return FALSE;
	}
}


/**
 * @fn gboolean save_preferences_to_file(prefs_t *prefs)
 *  Saves the preferences to the file preferences
 * @param prefs : preferences (from prefs_t structure)
 * @return TRUE if everything went ok, FALSE otherwise
 */
gboolean save_preferences_to_file(prefs_t *prefs)
{
	gsize length = 0;
	gchar *contents = NULL;
	gboolean result = FALSE;

	if (prefs != NULL && prefs->file != NULL && prefs->filename != NULL)
	{
		contents = g_key_file_to_data(prefs->file, &length, NULL);
		result = g_file_set_contents(prefs->filename, contents, length, NULL);
		g_free(contents);
	}

	return result;
}

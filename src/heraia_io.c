/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  io.c
  io.c - input and output functions for heraia
 
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

static GladeXML *load_glade_xml_if_it_exists(char *file_to_load);


gboolean load_file_to_analyse(heraia_window_t *main_window, gchar *filename)
{
	struct stat *stat_buf = NULL;
	gboolean success = FALSE;

	stat_buf = (struct stat *) g_malloc0 (sizeof(struct stat));
	stat(filename, stat_buf);
	if (main_window->debug == TRUE)
		log_message(main_window, G_LOG_LEVEL_INFO, "filename to load : %s", filename);

	if (S_ISREG(stat_buf->st_mode) && stat_buf->st_size>0)
		{

			heraia_hex_document_new(main_window, filename);

			gtk_box_pack_start(GTK_BOX(glade_xml_get_widget(main_window->xml, "vbox1")), 
							   main_window->current_DW->current_hexwidget, TRUE, TRUE, 0);
			
			gtk_widget_show(main_window->current_DW->current_hexwidget);

			if (main_window->debug == TRUE)
				log_message(main_window, G_LOG_LEVEL_INFO, "Hexwidget : %p", main_window->current_DW->current_hexwidget);
			success = TRUE;

			if (main_window->filename != filename)
				{
					if (main_window->filename != NULL)
						g_free(main_window->filename);
					main_window->filename = g_strdup_printf("%s", filename);
				}

			if (main_window->debug == TRUE)
				log_message(main_window, G_LOG_LEVEL_INFO, "file %s loaded !", main_window->filename);
		} 
	else
		{
			if (S_ISREG(stat_buf->st_mode))
				fprintf(stderr, "The file %s is empty !\n", filename);
			else
				fprintf(stderr, "The file %s does not exist !\n", filename);
			success = FALSE;
		}

	g_free(stat_buf);

	return success;
}


/** 
 *  Checks if file_to_load exists and is valid and if possible, loads it
 *  in the xml structure
 */
static GladeXML *load_glade_xml_if_it_exists(gchar *file_to_load)
{
	struct stat *stat_buf;
	GladeXML *xml = NULL;

	stat_buf = (struct stat *) g_malloc0 (sizeof(struct stat));

	stat(file_to_load, stat_buf);
	if (S_ISREG(stat_buf->st_mode) && stat_buf->st_size>0)
			xml = glade_xml_new(file_to_load, NULL, NULL);
	else
		xml = NULL;

	g_free(stat_buf);

	return xml;
}

/* loads the glade xml file ('filename') that describes an interface,
   tries all the paths defined in the location_list and put the definition
   in the 'xml' variable
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
				list = list->next;	
			g_free(file_to_load);
		}
	
	return xml;
}

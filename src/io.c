/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  io.c
  io.c - input and output functions for heraia
 
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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "types.h"
#include "io.h"

gboolean load_file_to_analyse(heraia_window_t *main_window, char *filename)
{
	struct stat *stat_buf = NULL;
	gboolean success = FALSE;

	stat_buf = (struct stat *) g_malloc0 (sizeof(struct stat));
	stat(filename, stat_buf);
	if (main_window->debug == TRUE)
		g_print("filename to load : %s\n", filename);

	if (S_ISREG(stat_buf->st_mode) && stat_buf->st_size>0)
		{

			heraia_hex_document_new(main_window, filename);

			gtk_box_pack_start(GTK_BOX(glade_xml_get_widget(main_window->xml, "vbox1")), 
							   main_window->current_DW->current_hexwidget, TRUE, TRUE, 0);
			
			gtk_widget_show(main_window->current_DW->current_hexwidget);

			if (main_window->debug == TRUE)
				g_print("Hexwidget : %p\n", main_window->current_DW->current_hexwidget);
			success = TRUE;
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


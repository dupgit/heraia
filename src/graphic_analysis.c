/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  graphic_analysis.c
  graphic analysis functions
  
  (C) Copyright 2006 Olivier Delhomme
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "types.h"
#include "graphic_analysis.h"

void do_the_histogram(heraia_window_t *main_window)
{
  guint histo[256];
  guint pos = 0;
  guint size = 0;
  guint c = 0;
  data_window_t *DW = NULL;
  GtkHex *gh = NULL;

  fprintf(stdout, "do_the_histogram !\n");
  if (main_window != NULL)
    {
      fprintf(stdout, "main_window != NULL !\n");
      DW = main_window->current_DW;
      if (DW != NULL)
	{
	  fprintf(stdout, "DW != NULL !\n");
	  gh = GTK_HEX (DW->current_hexwidget);
  
	  if (gh != NULL)
	    {
	      fprintf(stdout, "gh != NULL !\n");
	      pos = 0;
	      size = gh->document->file_size;
	     
	      while (pos < size)
		{
		  c = (guint) gtk_hex_get_byte(gh, pos);
		  pos++;
		  histo[c]++;
		}

	      pos = 0;
	      while (pos < 255)
		{
		  fprintf(stdout, "%d ", histo[pos]);
		  pos++;
		}
	      fprintf(stdout, "\n");
	    }
	}
    }
}


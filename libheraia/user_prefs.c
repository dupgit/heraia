/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  user_prefs.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
 *
 *  (C) Copyright 2005 - 2008 Olivier Delhomme
 *  e-mail : heraia@delhomme.org
 *  URL    : http://heraia.tuxfamily.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or  (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *  MERCHANTABILITY  or  FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <libheraia.h>

static void verify_preference_file_path_presence(gchar *pathname);
static void verify_preference_file_name_presence(gchar *filename);

/**
 *  verify preference file path presence and creates it if it does
 *  not already exists
 */
static void verify_preference_file_path_presence(gchar *pathname)
{
	struct stat *buf = NULL;
	gint result = 0;
	
	buf = (struct stat *) g_malloc0(sizeof(struct stat));
	result = g_stat(pathname, buf);

	if (result != 0)
	{
		g_mkdir_with_parents(pathname, 488);
	}
}

/**
 *  Verify preference file's presence
 */
static void verify_preference_file_name_presence(gchar *filename)
{
	FILE *fp = NULL;
	
	
	
	fp = g_fopen(filename, "r");
	
	if (fp == NULL)
	{
	  	fp = g_fopen(filename, "w");	
		if (fp == NULL)
		{
			fprintf(stderr, "Unable to open and create the main preference file %s\n", filename);
		}
		else
		{
			fprintf(stderr, "Main preference file %s created successfully\n", filename);
			fclose(fp);
		}
	}
	else
	{
		fclose(fp);
	}
}

/**
 *  Verify preference file presence and creates it if it does not
 *  already exists
 */
void verify_preference_file(gchar *pathname, gchar *filename)
{
	
	verify_preference_file_path_presence(pathname);
	verify_preference_file_name_presence(filename);
	
}

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  user_prefs.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
 *
 *  (C) Copyright 2005 - 2009 Olivier Delhomme
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
/**
 * @file user_prefs.c
 * Users preference may be somewhere around here
 */
#include <libheraia.h>

static void verify_preference_file_path_presence(gchar *pathname);
static void verify_preference_file_name_presence(gchar *filename);

static void save_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop);

static void save_mp_file_preferences_options(heraia_window_t *main_window);
static void save_mp_display_preferences_options(heraia_window_t *main_window);

static void load_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop);

static void load_mp_file_preferences_options(heraia_window_t *main_window);
static void load_mp_display_preferences_options(heraia_window_t *main_window);


/**
 * @fn void verify_preference_file_path_presence(gchar *pathname)
 *  verify preference file path presence and creates it if it does
 *  not already exists
 *  @param pathname is a path to look presence for
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
 * @fn void verify_preference_file_name_presence(gchar *filename)
 *  Verify preference file's presence and creates it if it does 
 *  not exists already
 *  @param filename is a name of a file to look presence for
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
 * @fn verify_preference_file(gchar *pathname, gchar *filename)
 *  Verify preference file presence and creates it if it does not
 *  already exists
 * @param pathname is the full pathname
 * @param filename is the filename containing the path itself
 */
void verify_preference_file(gchar *pathname, gchar *filename)
{
	
	verify_preference_file_path_presence(pathname);
	verify_preference_file_name_presence(filename);
	
}

/**
 * @fn void save_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop)
 *  Window preferences
 *  @param file a GKeyFile where values are stored
 *  @param name a keyname (basically a window name)
 *  @param window_prop all window properties to save (structure window_prop_t)
 */
static void save_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop)
{
	gchar *keyname = NULL;
	
	keyname = g_strconcat(name, " Displayed", NULL);
	g_key_file_set_boolean(file, GN_GLOBAL_PREFS, keyname, window_prop->displayed);
	g_free(keyname);
	
	keyname = g_strconcat(name, " X_pos", NULL);
	g_key_file_set_integer(file, GN_GLOBAL_PREFS, keyname, window_prop->x);
	g_free(keyname);
	
	keyname = g_strconcat(name, " Y_pos", NULL);
	g_key_file_set_integer(file, GN_GLOBAL_PREFS, keyname, window_prop->y);
	g_free(keyname);
	
	keyname = g_strconcat(name, " Height", NULL);
	g_key_file_set_integer(file, GN_GLOBAL_PREFS, keyname, window_prop->height);
	g_free(keyname);

	keyname = g_strconcat(name, " Width", NULL);
	g_key_file_set_integer(file, GN_GLOBAL_PREFS, keyname, window_prop->width);
	g_free(keyname);
}

/**
 * @fn void save_mp_file_preferences_options(heraia_window_t *main_window)
 *  Save only file preferences related options
 *  @param main_window the main structure
 */
static void save_mp_file_preferences_options(heraia_window_t *main_window)
{
	prefs_t *prefs = NULL;
	gboolean activated = FALSE;
	
	if (main_window != NULL)
	{
		prefs = main_window->prefs;
		
		if (prefs->file == NULL)
		{
			prefs->file = g_key_file_new();
		}
		
		if (prefs != NULL && prefs->file != NULL)
		{
			/* Saves the position */
			activated = is_toggle_button_activated(main_window->xmls->main, "save_window_position_bt");
			g_key_file_set_boolean(prefs->file, GN_GLOBAL_PREFS, KN_SAVE_WINDOW_PREFS, activated);
			
			/* Saving all window preferences if necessary */
			if (activated == TRUE)
			{
				save_window_preferences(prefs->file, KN_ABOUT_BOX, main_window->win_prop->about_box);
				save_window_preferences(prefs->file, KN_DATA_INTERPRETOR, main_window->win_prop->data_interpretor);
				save_window_preferences(prefs->file, KN_LOG_BOX, main_window->win_prop->log_box);
				save_window_preferences(prefs->file, KN_MAIN_DIALOG, main_window->win_prop->main_dialog);
				save_window_preferences(prefs->file, KN_PLUGIN_LIST, main_window->win_prop->plugin_list);
				save_window_preferences(prefs->file, KN_LDT, main_window->win_prop->ldt);
				save_window_preferences(prefs->file, KN_MAIN_PREFS, main_window->win_prop->main_pref_window);
			}
		}
	}
}

/**
 * @fn void save_mp_display_preferences_options(heraia_window_t *main_window)
 *  Save only display related preferences
 *  @param main_window the main structure
 */
static void save_mp_display_preferences_options(heraia_window_t *main_window)
{
	prefs_t *prefs = NULL;
	gboolean activated = FALSE;
	
	if (main_window != NULL)
	{
		prefs = main_window->prefs;
		
		if (prefs->file == NULL)
		{
			prefs->file = g_key_file_new();
		}
		
		if (prefs != NULL && prefs->file != NULL)
		{
			/* Display Thousand (or not) */
			activated = is_toggle_button_activated(main_window->xmls->main, "mp_thousand_bt");
			g_key_file_set_boolean(prefs->file, GN_DISPLAY_PREFS, KN_DISP_THOUSAND, activated);
		}
	}
}



/**
 * @fn void save_main_preferences(heraia_window_t *main_window)
 * Save all preferences to the user preference file
 * @param main_window the main structure
 */
void save_main_preferences(heraia_window_t *main_window)
{
	if (main_window != NULL)
	{
		/* 1. Saving main Preferences */
		save_mp_file_preferences_options(main_window);
		
		/* 2. Saving Display Preferences */
		save_mp_display_preferences_options(main_window);
		
		if (main_window->prefs != NULL)
		{
			/* Saving to file */
			save_preferences_to_file(main_window->prefs);
		}
	}
}


/**
 * @fn void load_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop)
 *  window preferences
 *  @param file a GKeyFile where values are stored
 *  @param name a keyname (basically a window name)
 *  @param window_prop all window properties to save (structure window_prop_t)
 */
static void load_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop)
{
	gchar *keyname = NULL;
	
	keyname = g_strconcat(name, " Displayed", NULL);
	window_prop->displayed = g_key_file_get_boolean(file, GN_GLOBAL_PREFS, keyname, NULL);
	g_free(keyname);
	
	keyname = g_strconcat(name, " X_pos", NULL);
	window_prop->x = g_key_file_get_integer(file, GN_GLOBAL_PREFS, keyname, NULL);
	g_free(keyname);
	
	keyname = g_strconcat(name, " Y_pos", NULL);
	window_prop->y = g_key_file_get_integer(file, GN_GLOBAL_PREFS, keyname, NULL);
	g_free(keyname);
	
	keyname = g_strconcat(name, " Height", NULL);
	window_prop->height = g_key_file_get_integer(file, GN_GLOBAL_PREFS, keyname, NULL);
	g_free(keyname);

	keyname = g_strconcat(name, " Width", NULL);
	window_prop->width = g_key_file_get_integer(file, GN_GLOBAL_PREFS, keyname, NULL);
	g_free(keyname);
	
}


/**
 * @fn void load_mp_file_preferences_options(heraia_window_t *main_window)
 *  Load only main preferences related options
 * @param main_window the main structure
 */
static void load_mp_file_preferences_options(heraia_window_t *main_window)
{
	prefs_t *prefs = NULL;
	GtkWidget *save_window_position_bt = NULL;
	gboolean activated = FALSE;
	
	if (main_window != NULL)
	{
		prefs = main_window->prefs;
		
		if (prefs != NULL && prefs->file != NULL)
		{
			/* Saving window's positions ? */
			activated = g_key_file_get_boolean(prefs->file, GN_GLOBAL_PREFS, KN_SAVE_WINDOW_PREFS, NULL);
			save_window_position_bt = heraia_get_widget(main_window->xmls->main, "save_window_position_bt");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_window_position_bt), activated);
			
			if (activated == TRUE)
			{
				/* window's positions */
				load_window_preferences(prefs->file, KN_ABOUT_BOX, main_window->win_prop->about_box);
				load_window_preferences(prefs->file, KN_DATA_INTERPRETOR, main_window->win_prop->data_interpretor);
				load_window_preferences(prefs->file, KN_LOG_BOX, main_window->win_prop->log_box);
				load_window_preferences(prefs->file, KN_MAIN_DIALOG, main_window->win_prop->main_dialog);
				load_window_preferences(prefs->file, KN_PLUGIN_LIST, main_window->win_prop->plugin_list);
				load_window_preferences(prefs->file, KN_LDT, main_window->win_prop->ldt);
				load_window_preferences(prefs->file, KN_MAIN_PREFS, main_window->win_prop->main_pref_window);	
			}
		}	
	}
}


/**
 * @fn void load_mp_display_preferences_options(heraia_window_t *main_window)
 *  Load display related preferences
 * @param main_window the main structure
 */
static void load_mp_display_preferences_options(heraia_window_t *main_window)
{
	prefs_t *prefs = NULL;
	GtkWidget *display_thousand_bt = NULL;
	gboolean activated = FALSE;
	
	if (main_window != NULL)
	{
		prefs = main_window->prefs;
		
		if (prefs != NULL && prefs->file != NULL)
		{
			/* Display thousands (or not) */
			activated = g_key_file_get_boolean(prefs->file, GN_DISPLAY_PREFS, KN_DISP_THOUSAND, NULL);
			display_thousand_bt = heraia_get_widget(main_window->xmls->main, "mp_thousand_bt");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(display_thousand_bt), activated);
		}
	}
}


/**
 * @fn setup_preferences(heraia_window_t *main_window)
 *  Sets up the preferences as loaded in the preference file
 * @param main_window the main structure
 */
void setup_preferences(heraia_window_t *main_window)
{
	if (main_window != NULL)
	{
		/* 1. Main Preferences */
		load_mp_file_preferences_options(main_window);
		
		/* 2. Display preferences */
		load_mp_display_preferences_options(main_window);
	}
}

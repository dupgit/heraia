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

static void save_mp_file_preferences_options(heraia_struct_t *main_struct);
static void save_mp_display_preferences_options(heraia_struct_t *main_struct);

static void load_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop);

static void load_mp_file_preferences_options(heraia_struct_t *main_struct);
static void load_mp_display_preferences_options(heraia_struct_t *main_struct);


/**
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
 * look out if the preference structure exists or not. If not
 * it creates it.
 * @param main_struct the main structure
 */
void init_preference_struct(heraia_struct_t *main_struct)
{
    prefs_t *prefs = NULL;

    if (main_struct->prefs == NULL)
    {
       main_struct->prefs = (prefs_t *) g_malloc0(sizeof(prefs_t));
       main_struct->prefs->file = g_key_file_new();
       main_struct->prefs->pathname = g_strdup_printf("%s%c.%s", g_get_home_dir(), G_DIR_SEPARATOR, "heraia");
       main_struct->prefs->filename = g_strdup_printf("%s%c%s", main_struct->prefs->pathname, G_DIR_SEPARATOR, "main_preferences");
    }
    else
    {
        prefs = main_struct->prefs;

        if (prefs->file == NULL)
        {
            prefs->file = g_key_file_new();
        }
    }
}

/**
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
 *  Save only file preferences related options
 *  @param main_struct the main structure
 */
static void save_mp_file_preferences_options(heraia_struct_t *main_struct)
{
    prefs_t *prefs = NULL;
    gboolean activated = FALSE;

    if (main_struct != NULL)
    {
        prefs = main_struct->prefs;

        /* Saves the position */
        activated = is_toggle_button_activated(main_struct->xmls->main, "save_window_position_bt");
        g_key_file_set_boolean(prefs->file, GN_GLOBAL_PREFS, KN_SAVE_WINDOW_PREFS, activated);

        /* Saving all window preferences if necessary */
        if (activated == TRUE)
        {
            save_window_preferences(prefs->file, KN_ABOUT_BOX, main_struct->win_prop->about_box);
            save_window_preferences(prefs->file, KN_DATA_INTERPRETOR, main_struct->win_prop->data_interpretor);
            save_window_preferences(prefs->file, KN_LOG_BOX, main_struct->win_prop->log_box);
            save_window_preferences(prefs->file, KN_MAIN_DIALOG, main_struct->win_prop->main_dialog);
            save_window_preferences(prefs->file, KN_PLUGIN_LIST, main_struct->win_prop->plugin_list);
            save_window_preferences(prefs->file, KN_LDT, main_struct->win_prop->ldt);
            save_window_preferences(prefs->file, KN_MAIN_PREFS, main_struct->win_prop->main_pref_window);
        }
    }
}


/**
 *  Save only display related preferences
 *  @param main_struct : main structure
 */
static void save_mp_display_preferences_options(heraia_struct_t *main_struct)
{
    prefs_t *prefs = NULL;
    gboolean activated = FALSE;

    if (main_struct != NULL)
    {
        prefs = main_struct->prefs;

        /* Display Thousand (or not) */
        activated = is_toggle_button_activated(main_struct->xmls->main, "mp_thousand_bt");
        g_key_file_set_boolean(prefs->file, GN_DISPLAY_PREFS, KN_DISP_THOUSAND, activated);
    }
}


/**
 * Saves data interpretor state and preferences
 * @param main_struct : main structure
 */
static void save_di_preferences(heraia_struct_t *main_struct)
{
    GtkNotebook *notebook = NULL;  /**< data interpretor's notebook               */
    gint selected_tab;             /**< Selected tab in data interpretor's window */
    prefs_t *prefs = NULL;         /**< structure for preferences                 */

    if (main_struct != NULL && main_struct->current_DW != NULL)
    {
        prefs = main_struct->prefs;

        notebook = GTK_NOTEBOOK(heraia_get_widget(main_struct->xmls->main, "diw_notebook"));

        if (notebook != NULL)
        {
            selected_tab = gtk_notebook_get_current_page(notebook);

            if (selected_tab >= 0)
            {
                g_key_file_set_integer(prefs->file, GN_DI_PREFS, KN_DI_SELECTED_TAB, selected_tab);
            }
        }
    }
}


/**
 * Save all preferences to the user preference file
 * @param main_struct the main structure
 */
void save_preferences(heraia_struct_t *main_struct)
{
    if (main_struct != NULL)
    {
        /* 1. Saving main Preferences */
        save_mp_file_preferences_options(main_struct);

        /* 2. Saving Display Preferences */
        save_mp_display_preferences_options(main_struct);

        /* 3. Saving Data Interpretor Preferences */
        save_di_preferences(main_struct);

        if (main_struct->prefs != NULL)
        {
            /* Saving to file */
            save_preferences_to_file(main_struct->prefs);
        }
    }
}


/**
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
 *  Load only main preferences related options
 * @param main_struct the main structure
 */
static void load_mp_file_preferences_options(heraia_struct_t *main_struct)
{
    prefs_t *prefs = NULL;
    GtkWidget *save_window_position_bt = NULL;
    gboolean activated = FALSE;

    if (main_struct != NULL)
    {
        prefs = main_struct->prefs;

        /* Saving window's positions ? */
        activated = g_key_file_get_boolean(prefs->file, GN_GLOBAL_PREFS, KN_SAVE_WINDOW_PREFS, NULL);
        save_window_position_bt = heraia_get_widget(main_struct->xmls->main, "save_window_position_bt");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_window_position_bt), activated);

        if (activated == TRUE)
        {
            /* window's positions */
            load_window_preferences(prefs->file, KN_ABOUT_BOX, main_struct->win_prop->about_box);
            load_window_preferences(prefs->file, KN_DATA_INTERPRETOR, main_struct->win_prop->data_interpretor);
            load_window_preferences(prefs->file, KN_LOG_BOX, main_struct->win_prop->log_box);
            load_window_preferences(prefs->file, KN_MAIN_DIALOG, main_struct->win_prop->main_dialog);
            load_window_preferences(prefs->file, KN_PLUGIN_LIST, main_struct->win_prop->plugin_list);
            load_window_preferences(prefs->file, KN_LDT, main_struct->win_prop->ldt);
            load_window_preferences(prefs->file, KN_MAIN_PREFS, main_struct->win_prop->main_pref_window);
        }
    }
}


/**
 *  Load display related preferences
 * @param main_struct the main structure
 */
static void load_mp_display_preferences_options(heraia_struct_t *main_struct)
{
    prefs_t *prefs = NULL;
    GtkWidget *display_thousand_bt = NULL;
    gboolean activated = FALSE;

    if (main_struct != NULL)
    {
        prefs = main_struct->prefs;

        /* Display thousands (or not) */
        activated = g_key_file_get_boolean(prefs->file, GN_DISPLAY_PREFS, KN_DISP_THOUSAND, NULL);
        display_thousand_bt = heraia_get_widget(main_struct->xmls->main, "mp_thousand_bt");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(display_thousand_bt), activated);
    }
}


/**
 * Load data interpretor state and preferences
 * @param main_struct : main structure
 */
static void load_di_preferences(heraia_struct_t *main_struct)
{
    GtkNotebook *notebook = NULL;  /**< data interpretor's notebook               */
    gint selected_tab;             /**< Selected tab in data interpretor's window */
    prefs_t *prefs = NULL;         /**< structure for preferences                 */

    if (main_struct != NULL && main_struct->current_DW != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL)
    {
        notebook = GTK_NOTEBOOK(heraia_get_widget(main_struct->xmls->main, "diw_notebook"));
        prefs = main_struct->prefs;

        if (notebook != NULL)
        {
            selected_tab = g_key_file_get_integer(prefs->file, GN_DI_PREFS, KN_DI_SELECTED_TAB, NULL);

            if (selected_tab >= 0)
            {
                gtk_notebook_set_current_page(notebook, selected_tab);
                main_struct->current_DW->tab_displayed = selected_tab;
            }
        }
    }
}


/**
 *  Sets up the preferences as loaded in the preference file
 * @param main_struct the main structure
 */
void load_preferences(heraia_struct_t *main_struct)
{
    if (main_struct != NULL)
    {
        /* 1. Main Preferences */
        load_mp_file_preferences_options(main_struct);

        /* 2. Display preferences */
        load_mp_display_preferences_options(main_struct);

        /* 3. Data Interpretor Preferences */
        load_di_preferences(main_struct);
    }
}

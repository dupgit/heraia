/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  user_prefs.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
 *
 *  (C) Copyright 2005 - 2011 Olivier Delhomme
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

/* Verifying */
static void verify_preference_file_path_presence(gchar *pathname);
static void verify_preference_file_name_presence(gchar *filename);

/* Saving */
static void save_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop);

static void save_mp_file_preferences_options(heraia_struct_t *main_struct, prefs_t *prefs);
static void save_mp_display_preferences_options(heraia_struct_t *main_struct, prefs_t *prefs);
static void save_mp_files_filenames(heraia_struct_t *main_struct, prefs_t *prefs);

static void save_di_preferences(heraia_struct_t *main_struct, prefs_t *prefs);

static void save_mpwp_preferences(heraia_struct_t *main_struct, prefs_t *prefs);

/* Loading */
static void load_window_preferences(GKeyFile *file, gchar *name, window_prop_t *window_prop);

static void load_mp_file_preferences_options(heraia_struct_t *main_struct, prefs_t *prefs);
static void load_mp_display_preferences_options(heraia_struct_t *main_struct, prefs_t *prefs);
static void load_mp_files_filenames(heraia_struct_t *main_struct, prefs_t *prefs);

static void load_di_preferences(heraia_struct_t *main_struct, prefs_t *prefs);

static void load_mpwp_preferences(heraia_struct_t *main_struct, prefs_t *prefs);


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
                    fprintf(stderr, Q_("Unable to open and create the main preference file %s\n"), filename);
                }
            else
                {
                    fprintf(stderr, Q_("Main preference file %s created successfully\n"), filename);
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
 *  already exists.
 * @param prefs is a 'prefs_t *' filled preference structure
 */
void verify_preference_file(prefs_t *prefs)
{
    if (prefs != NULL)
        {
            verify_preference_file_path_presence(prefs->pathname);
            verify_preference_file_name_presence(prefs->filename);
        }
}


/**
 * Look out if the preference structure exists or not. If not
 * it creates it.
 * @see http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
 * @param pathname is the pathname where the preference file is
 * @param filename is the filename of the preference file itself
 */
prefs_t *init_preference_struct(gchar *pathname, gchar *filename)
{
    prefs_t *prefs = NULL;

    prefs = (prefs_t *) g_malloc0(sizeof(prefs_t));

    prefs->file = g_key_file_new();
    prefs->pathname = pathname;
    prefs->filename = g_build_filename(prefs->pathname, filename, NULL);

    return prefs;
}


/**
 * Destroys a preference structure
 * @param prefs the preference structure to be freed
 */
void free_preference_struct(prefs_t *prefs)
{
    if (prefs != NULL)
        {
            g_free(prefs->filename);
            g_free(prefs->pathname);
            g_key_file_free(prefs->file);
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
 * Save only file preferences related options
 * @param main_struct the main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
static void save_mp_file_preferences_options(heraia_struct_t *main_struct, prefs_t *prefs)
{
    gboolean activated = FALSE;


    if (main_struct != NULL && prefs != NULL)
        {
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
                    save_window_preferences(prefs->file, KN_GOTO_DIALOG, main_struct->win_prop->goto_window);
                    save_window_preferences(prefs->file, KN_RESULT_WINDOW, main_struct->win_prop->result_window);
                    save_window_preferences(prefs->file, KN_FIND_WINDOW, main_struct->win_prop->find_window);
                    save_window_preferences(prefs->file, KN_FR_WINDOW, main_struct->win_prop->fr_window);
                    save_window_preferences(prefs->file, KN_FDFT_WINDOW, main_struct->win_prop->fdft_window);
                }

            /* Saving opened files filenames ? */
            activated = is_toggle_button_activated(main_struct->xmls->main, "save_filenames_bt");
            g_key_file_set_boolean(prefs->file, GN_GLOBAL_PREFS, KN_SAVE_OPENED_FILES_FILENAMES, activated);

            if (activated == TRUE)
                {
                    save_mp_files_filenames(main_struct, prefs);
                }
        }
}

/**
 * Saves files filenames to the config file
 * @param main_struct the main structure
 * @param prefs preference structure
 */
static void save_mp_files_filenames(heraia_struct_t *main_struct, prefs_t *prefs)
{
    const gchar **list = NULL;  /**< A list that will contain all the files filenames            */
    gchar *filename = NULL;     /**< One filename                                                */
    gint *pos_list = NULL;   /**< list of the current position of the cursor in the documents */
    gsize i = 0;
    gsize len = 0;
    gsize j = 0;
    gint current_tab = 0;       /**< Current selected tab in the main notebook                   */
    doc_t *document = NULL;     /**< One document (from the documents array in the main_struct   */
    GtkWidget *notebook = NULL; /**< Main notebook in the file view in the main window           */

    if (main_struct != NULL && prefs != NULL)
        {
            /* First initializing the list variable with the filenames of the opened documents */
            i = 0;
            j = 0;
            len = main_struct->documents->len;
            list = (const gchar **) g_malloc0 (sizeof(gchar *)*len);
            pos_list = (gint *) g_malloc0 (sizeof(gint)*len);

            while (i < len)
                {
                    document = g_ptr_array_index(main_struct->documents, i);

                    if (document != NULL)
                        {
                            filename = g_strdup(doc_t_document_get_filename(document));
                            /* There will be some limitations here due to the guint64 to gint cast */
                            pos_list[j] = (gint) ghex_get_cursor_position(document->hex_widget);
                            list[j] = filename;
                            j++;
                        }
                    i++;
                }

            /* Saving them to the preference file */
            g_key_file_set_string_list(prefs->file, GN_GLOBAL_PREFS, KN_FILES_FILENAMES, list, j);
            g_key_file_set_integer_list(prefs->file, GN_GLOBAL_PREFS, KN_FILES_CURSOR_POSITIONS, pos_list, j);

            /* saving current tab */
            notebook = heraia_get_widget(main_struct->xmls->main, "file_notebook");
            current_tab = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
            g_key_file_set_integer(prefs->file, GN_GLOBAL_PREFS, KN_CURRENT_TAB, current_tab);
        }
}


/**
 * Save only display related preferences
 * @param main_struct : main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
static void save_mp_display_preferences_options(heraia_struct_t *main_struct, prefs_t *prefs)
{
    gboolean activated = FALSE;

    if (main_struct != NULL && prefs != NULL)
        {
            /* Display Thousand (or not) */
            activated = is_toggle_button_activated(main_struct->xmls->main, "mp_thousand_bt");
            g_key_file_set_boolean(prefs->file, GN_DISPLAY_PREFS, KN_DISP_THOUSAND, activated);

            /* Display offsets (or not) */
            activated = is_toggle_button_activated(main_struct->xmls->main, "mp_display_offset_bt");
            g_key_file_set_boolean(prefs->file, GN_DISPLAY_PREFS, KN_DISP_OFFSETS, activated);
        }
}


/**
 * Saves data interpretor state and preferences
 * @param main_struct : main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
static void save_di_preferences(heraia_struct_t *main_struct, prefs_t *prefs)
{
    gint selected_tab = -1; /**< Selected tab in data interpretor's window */
    gint stream_size = -1;  /**< Stream size in data interpretor's window  */
    gint endianness = -1;   /**< Endianness in data interpretor's window   */

    if (main_struct != NULL && main_struct->current_DW != NULL && prefs != NULL)
        {
            selected_tab = di_get_selected_tab(main_struct);
            if (selected_tab >= 0)
                {
                    g_key_file_set_integer(prefs->file, GN_DI_PREFS, KN_DI_SELECTED_TAB, selected_tab);
                }

            stream_size = di_get_stream_size(main_struct);
            if (stream_size >= 0)
                {
                    g_key_file_set_integer(prefs->file, GN_DI_PREFS, KN_DI_STREAM_SIZE, stream_size);
                }

            endianness = di_get_endianness(main_struct);
            if (endianness >= 0)
                {
                     g_key_file_set_integer(prefs->file, GN_DI_PREFS, KN_DI_ENDIANNESS, endianness);
                }
        }
}


/**
 * Saves main preferences window state and preferences
 * @param main_struct : main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
static void save_mpwp_preferences(heraia_struct_t *main_struct, prefs_t *prefs)
{
    GtkNotebook *notebook = NULL; /**< main preferences's notebook               */
    gint selected_tab = -1;       /**< Selected tab in data interpretor's window */

    if (main_struct != NULL && main_struct->current_DW != NULL && prefs != NULL)
        {
            notebook = GTK_NOTEBOOK(heraia_get_widget(main_struct->xmls->main, "mp_first_notebook"));

            if (notebook != NULL)
                {
                    selected_tab = gtk_notebook_get_current_page(notebook);

                    if (selected_tab >= 0)
                        {
                            g_key_file_set_integer(prefs->file, GN_MPWP_PREFS, KN_MPWP_SELECTED_TAB, selected_tab);
                        }
                }
        }
}


/**
 * Save all preferences to the user preference file
 * @param main_struct the main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
void save_preferences(heraia_struct_t *main_struct, prefs_t *prefs)
{
    if (main_struct != NULL && prefs != NULL)
        {
            /* 1. Saving main Preferences */
            save_mp_file_preferences_options(main_struct, prefs);

            /* 2. Saving Display Preferences */
            save_mp_display_preferences_options(main_struct, prefs);

            /* 3. Saving Data Interpretor Preferences */
            save_di_preferences(main_struct, prefs);

            /* 4. Saving Main Preferences Window Preferences */
            save_mpwp_preferences(main_struct, prefs);

            /* 5. Saving to file */
            save_preferences_to_file(prefs);
        }
}


/**
 *  Window preferences
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
 * Load only main preferences related options
 * @param main_struct the main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
static void load_mp_file_preferences_options(heraia_struct_t *main_struct, prefs_t *prefs)
{
    GtkWidget *save_window_position_bt = NULL;
    GtkWidget *save_filenames_bt = NULL;
    gboolean activated = FALSE;

    if (main_struct != NULL && prefs != NULL)
        {
            log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Loading window's positions"));
            /* Loading window's positions ? */
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
                    load_window_preferences(prefs->file, KN_GOTO_DIALOG, main_struct->win_prop->goto_window);
                    load_window_preferences(prefs->file, KN_RESULT_WINDOW, main_struct->win_prop->result_window);
                    load_window_preferences(prefs->file, KN_FIND_WINDOW, main_struct->win_prop->find_window);
                    load_window_preferences(prefs->file, KN_FR_WINDOW, main_struct->win_prop->fr_window);
                    load_window_preferences(prefs->file, KN_FDFT_WINDOW, main_struct->win_prop->fdft_window);
                }

            /* Loading opened files filenames ? */
            activated = g_key_file_get_boolean(prefs->file, GN_GLOBAL_PREFS, KN_SAVE_OPENED_FILES_FILENAMES, NULL);
            save_filenames_bt = heraia_get_widget(main_struct->xmls->main, "save_filenames_bt");
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_filenames_bt), activated);

            if (activated == TRUE)
                {
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Loading files..."));
                    load_mp_files_filenames(main_struct, prefs);
                }
        }
}


/**
 * Load files filenames from the config file
 * @param main_struct the main structure
 * @param prefs preference structure
 */
static void load_mp_files_filenames(heraia_struct_t *main_struct, prefs_t *prefs)
{
    gchar **list = NULL;      /**< The list that will contain all filenames to be loaded       */
    gint *pos_list = NULL;    /**< list of the current position of the cursor in the documents */
    gsize i = 0;
    gsize len = 0;
    gsize pos_len = 0;
    gint current_tab = 0;
    GtkWidget *notebook = NULL;

    if (main_struct != NULL && prefs != NULL)
        {
            /* get file list */
            list = g_key_file_get_string_list(prefs->file, GN_GLOBAL_PREFS, KN_FILES_FILENAMES, &len, NULL);
            pos_list = (gint *) g_key_file_get_integer_list(prefs->file, GN_GLOBAL_PREFS, KN_FILES_CURSOR_POSITIONS, &pos_len, NULL);

            if (len == pos_len)
                {
                    for (i = 0; i < len; i++)
                        {
                            if (load_file_to_analyse(main_struct, list[i]))
                                {
                                    /* in add_new_tab_in_main_window() function, the newly */
                                    /* opened document becomes the current one             */
                                    /* There is some limitations here due to the cast from */
                                    /* gint to guint64                                     */
                                    ghex_set_cursor_position(main_struct->current_doc->hex_widget, (guint64) pos_list[i]);
                                }
                        }
                }

            /* get current tab */
            current_tab = g_key_file_get_integer(prefs->file, GN_GLOBAL_PREFS, KN_CURRENT_TAB, NULL);
            notebook = heraia_get_widget(main_struct->xmls->main, "file_notebook");
            gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), current_tab);
        }
}


/**
 *  Load display related preferences
 * @param main_struct the main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
static void load_mp_display_preferences_options(heraia_struct_t *main_struct, prefs_t *prefs)
{
    GtkWidget *toggle_button = NULL;
    gboolean activated = FALSE;

    if (main_struct != NULL && prefs != NULL)
        {
            /* Display thousands (or not) */
            activated = g_key_file_get_boolean(prefs->file, GN_DISPLAY_PREFS, KN_DISP_THOUSAND, NULL);
            toggle_button = heraia_get_widget(main_struct->xmls->main, "mp_thousand_bt");
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_button), activated);

            /* Display offsets (or not) */
            activated = g_key_file_get_boolean(prefs->file, GN_DISPLAY_PREFS, KN_DISP_OFFSETS, NULL);
            toggle_button = heraia_get_widget(main_struct->xmls->main, "mp_display_offset_bt");
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_button), activated);
        }
}


/**
 * Load data interpretor state and preferences
 * @param main_struct : main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
static void load_di_preferences(heraia_struct_t *main_struct, prefs_t *prefs)
{
    gint selected_tab = -1;   /**< Selected tab in data interpretor's window */
    gint stream_size = -1;    /**< Stream size in data interpretor's window  */
    gint endianness = -1;     /**< Endianness in data interpretor's window   */

    if (main_struct != NULL && main_struct->current_DW != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL && prefs != NULL)
        {
            selected_tab = g_key_file_get_integer(prefs->file, GN_DI_PREFS, KN_DI_SELECTED_TAB, NULL);
            stream_size = g_key_file_get_integer(prefs->file, GN_DI_PREFS, KN_DI_STREAM_SIZE, NULL);
            endianness = g_key_file_get_integer(prefs->file, GN_DI_PREFS, KN_DI_ENDIANNESS, NULL);

            di_set_selected_tab(main_struct, selected_tab);
            di_set_stream_size(main_struct, stream_size);
            di_set_endianness(main_struct, endianness);
        }
}


/**
 * Load main preferences window state and preferences
 * @param main_struct : main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
static void load_mpwp_preferences(heraia_struct_t *main_struct, prefs_t *prefs)
{
    GtkNotebook *notebook = NULL;  /**< main preferences's notebook               */
    GtkWidget *button = NULL;      /**< tool button from the toolbar              */
    gint selected_tab;             /**< Selected tab in data interpretor's window */

    if (main_struct != NULL && main_struct->current_DW != NULL && main_struct->xmls != NULL && main_struct->xmls->main != NULL && prefs != NULL)
        {
            notebook = GTK_NOTEBOOK(heraia_get_widget(main_struct->xmls->main, "mp_first_notebook"));

            if (notebook != NULL)
                {
                    selected_tab = g_key_file_get_integer(prefs->file, GN_MPWP_PREFS, KN_MPWP_SELECTED_TAB, NULL);

                    switch (selected_tab)
                        {
                            case 0:
                                gtk_notebook_set_current_page(notebook, selected_tab);
                                button = heraia_get_widget(main_struct->xmls->main, "mp_tb_fp_bt");
                                gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(button), TRUE);
                                break;

                            case 1:
                                gtk_notebook_set_current_page(notebook, selected_tab);
                                button = heraia_get_widget(main_struct->xmls->main, "mp_tb_display_bt");
                                gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(button), TRUE);
                                break;

                            default:
                            break;
                        }
                }
        }
}


/**
 * Sets up the preferences as loaded in the preference file
 * @param main_struct the main structure
 * @param prefs is a 'prefs_t *' filled preference structure
 */
void load_preferences(heraia_struct_t *main_struct, prefs_t *prefs)
{
    if (main_struct != NULL && prefs != NULL)
        {
            /* 0. Loading preferences from file */
            if (load_preference_file(prefs) == TRUE)
                {
                    /* 1. Loading Main Preferences */
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Loading main preferences"));
                    load_mp_file_preferences_options(main_struct, prefs);

                    /* 2. Loading Display preferences */
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Loading main display preferences"));
                    load_mp_display_preferences_options(main_struct, prefs);

                    /* 3. Loading Data Interpretor Preferences */
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Loading data interpretor preferences"));
                    load_di_preferences(main_struct, prefs);

                    /* 4. Loading Main Preferences Window Preferences */
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Loading window preference's main preferences"));
                    load_mpwp_preferences(main_struct, prefs);
                }
        }
}

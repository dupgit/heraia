/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  heraia.c
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
/** @file heraia.c
 * This is the main program file.
 * Initialization is done here and then hand is passed to gtk's main thread
 */
/**
 * @author Olivier DELHOMME,
 *         Sébastien TRICAUD,
 *         Grégory AUDET
 * @version 0.1.1
 * @date 2005-2009
 */

#include "heraia_types.h"

static gboolean version(void);
static gboolean usage(int status);
static window_prop_t *init_window_properties(gint x, gint y, guint height, guint width, gboolean displayed);
static heraia_struct_t *init_window_property_struct(heraia_struct_t *main_struct);
static heraia_struct_t *heraia_init_main_struct(void);
static HERAIA_ERROR init_heraia_plugin_system(heraia_struct_t *main_struct);
static GList *init_heraia_location_list(void);
static gboolean manage_command_line_options(Options *opt, int argc, char **argv);

/**
 *  libheraia_main_struct is a global variable that points
 *  to the main structure and is intended for the library use ONLY !
 *  It should not be used anywhere else or for any other purpose
 */
static heraia_struct_t *libheraia_main_struct = NULL;


/**
 *  This is intended to be called by the library or any program that will use
 *  the library in order to get the pointer to the main structure heraia_struct_t.
 *  @return heraia_struct_t *, a pointer to the main structure
 */
heraia_struct_t *get_main_struct(void)
{
    return libheraia_main_struct;
}

/**
 *  prints program name, version, author, date and licence
 *  to the standard output
 */
static gboolean version(void)
{
    fprintf (stdout, "heraia, %s - %s - Version %s - License %s\n", HERAIA_AUTHORS, HERAIA_DATE, HERAIA_VERSION, HERAIA_LICENSE);
    return TRUE;
}


/**
 *  Function that informs the user about the command line options
 *  available with heraia
 *  @param status : integer that indicate wether to display help (!=0) or
 *                  an error message (0)
 *  @return
 *          - TRUE -> help message has been printed
 *          - FALSE -> error message has been printed
 */
static gboolean usage(int status)
{
    if (status == 0)
        {
            fprintf (stderr,
                     "Try `heraia --help' for more information.\n");
            return FALSE;
        }
    else
        {
            version();
            fprintf(stdout, "\nheraia is a simple hexadecimal file editor and file analyser");
            fprintf(stdout, "\nUsage :\n  heraia [options] filename(s)\n");
            fprintf(stdout, "\nOptions :\n\
  -h, --help\tThis help.\n\
  -v, --version\tProgram version information.\n\
  -t, --tests\tRuns some self tests.\n");
            return TRUE;
        }
}


/**
 *  Inits the properties of a window with defined values
 *  @param  x,y are x,y coordinates on the screen
 *  @param  height represents the height of the window
 *  @param  width represents the width of the window. x+height,y+width is
 *          window's right bottom corner
 *  @param  displayed says wether the window is displayed or not
 *  @return a new allocated window_prop_t * structure.
 */
static window_prop_t *init_window_properties(gint x, gint y, guint height, guint width, gboolean displayed)
{
    window_prop_t *window_p;

    /* Malloc the window properties struct */
    window_p = (window_prop_t *) g_malloc0(sizeof(window_prop_t));

    /* Sets the default values */
    window_p->x = x;
    window_p->y = y;
    window_p->height = height;
    window_p->width = width;
    window_p->displayed = displayed;

    return window_p;
}


/**
 *  Inits the window property structure
 *  @param main_struct : main structure
 *  @return main structure with an initiated window property structure
 */
static heraia_struct_t *init_window_property_struct(heraia_struct_t *main_struct)
{
    all_window_prop_t *win_prop = NULL;
    window_prop_t *about_box = NULL;
    window_prop_t *data_interpretor = NULL;
    window_prop_t *log_box = NULL;
    window_prop_t *main_dialog = NULL;
    window_prop_t *plugin_list = NULL;
    window_prop_t *ldt = NULL;
    window_prop_t *main_pref_window = NULL;

    /* Global struct */
    win_prop = (all_window_prop_t *) g_malloc0(sizeof(all_window_prop_t));

    /* Initial states for the dialog boxes (default values) */
    about_box = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);
    data_interpretor = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, H_DI_DISPLAYED);
    log_box = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);
    main_dialog = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, TRUE);
    plugin_list = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);
    ldt = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);
    main_pref_window = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);

    /* Attach to the struct */
    win_prop->about_box = about_box;
    win_prop->data_interpretor = data_interpretor;
    win_prop->log_box = log_box;
    win_prop->main_dialog = main_dialog;
    win_prop->plugin_list = plugin_list;
    win_prop->ldt = ldt;
    win_prop->main_pref_window = main_pref_window;

    /* attach it to the main struct so that it can be read everywhere */
    main_struct->win_prop = win_prop;

    return main_struct;
}


/**
 * Initialize the main structure (main_struct)
 * @return a pointer to a newly initialized main structure
 */
static heraia_struct_t *heraia_init_main_struct(void)
{
    heraia_struct_t *herwin = NULL;
    xml_t *xmls = NULL;

    herwin = (heraia_struct_t *) g_malloc0(sizeof(heraia_struct_t));

    if (!herwin)
        {
            fprintf(stderr, "Main structure could not be initialiazed !");
            fprintf(stderr, "Do you have a memory problem ?\n");
            return NULL;
        }

    /* preference file name initialisation */
    herwin->prefs = NULL;
    init_preference_struct(herwin);
    verify_preference_file(herwin->prefs->pathname, herwin->prefs->filename);

    /**
     * First, in this early stage of the development we want to toggle debugging
     *  mode ON which is enabled by default in the configure.ac file !
     */
    herwin->debug = ENABLE_DEBUG;
    /* herwin->filename = NULL; */

    herwin->current_doc = NULL;
    herwin->plugins_list = NULL;
    herwin->location_list = init_heraia_location_list(); /* location list initilization */

    /* xml_t structure initialisation */
    xmls = (xml_t *) g_malloc0(sizeof(xml_t));
    xmls->main = NULL;
    herwin->xmls = xmls;

    /* data interpretor structure initialization */
    herwin->current_DW = (data_window_t *) g_malloc0 (sizeof(data_window_t));
    herwin->current_DW->diw = NULL;
    herwin->current_DW->tab_displayed = 0;  /* the first tab */

    /* init window property structure */
    herwin = init_window_property_struct(herwin);

    /* documents */
    herwin->documents = g_ptr_array_new();

    /* init global variable for the library */
    libheraia_main_struct = herwin;

    return herwin;
}



/**
 *  Function that initializes the plugin system if any :
 *   - loads any plugin where expected to be found
 *   - inits the plugin window
 * @param main_struct : main structure (heraia_struct_t *)
 * @return HERAIA_NO_PLUGINS if no plugins where found or HERAIA_NOERR in
 *         case of no errors
 */
static HERAIA_ERROR init_heraia_plugin_system(heraia_struct_t *main_struct)
{

    /* Checking for plugins */
    if (plugin_capable() == TRUE)
        {
            log_message(main_struct, G_LOG_LEVEL_INFO, "Enabling plugins");
            load_plugins(main_struct);

            /* the plugin_list_window (here the plugins may be loaded !) */
            log_message(main_struct, G_LOG_LEVEL_DEBUG, "Inits the plugin list window");
            plugin_list_window_init_interface(main_struct);

            return HERAIA_NOERR;
        }
    else
        {
            log_message(main_struct, G_LOG_LEVEL_WARNING, "Plugins will be disabled");
            return HERAIA_NO_PLUGINS;
        }
}

/**
 *  Here we want to init the location list where we might look for
 *  in the future. These can be viewed as default paths
 *  @warning when adding new locations, keep in ming that the list is a
             prepended list in reverse order.
 *  @return a new allocatde GList containing all locations
 */
static GList *init_heraia_location_list(void)
{
    gchar *path = NULL;
    const gchar* const *system_data_dirs;
    guint i = 0;
    GList *location_list = NULL;

    /* heraia's binary path */
    path = g_strdup_printf("%s", g_get_current_dir());
    location_list = g_list_prepend(location_list, path);

    /* System data dirs */
    system_data_dirs = g_get_system_data_dirs();
    i = 0;
    while(system_data_dirs[i] != NULL)
        {
            path = g_strdup_printf("%s%c%s", system_data_dirs[i], G_DIR_SEPARATOR, "heraia");
            location_list = g_list_prepend(location_list, path);
            i++;
        }

    /* System config dirs */
    system_data_dirs = g_get_system_config_dirs();
    i = 0;
    while(system_data_dirs[i] != NULL)
        {
            path = g_strdup_printf("%s%c%s", system_data_dirs[i], G_DIR_SEPARATOR, "heraia");
            location_list = g_list_prepend(location_list, path);
            i++;
        }

    /* the user path */
    path =  g_strdup_printf("%s%c.%s", g_get_home_dir(), G_DIR_SEPARATOR, "heraia");
    location_list = g_list_prepend(location_list, path);

    /* A global user data path */
    path = g_strdup_printf("%s%c%s", g_get_user_data_dir(), G_DIR_SEPARATOR, "heraia");
    location_list = g_list_prepend(location_list, path);

    /* A global config data path */
    path = g_strdup_printf("%s%c%s", g_get_user_config_dir(), G_DIR_SEPARATOR, "heraia");
    location_list = g_list_prepend(location_list, path);

    return location_list;
}


/**
 * Does some self tests in heraia
 * @param main_struct : main structure (heraia_struct_t *)
 * @return gboolean as an exit value for the program
 */
static gboolean do_heraia_self_tests(heraia_struct_t *main_struct)
{

    return TRUE;
}


/**
 *  Manages all the command line options and populates the
 *  Options *opt structure accordingly
 *  @param opt (Options *opt) filled here with the parameters found in **argv
 *  @param argc : number of command line arguments
 *  @param argv : array of string (char *) that contains arguments
 *  @return gboolean that seems to always be TRUE
 */
static gboolean manage_command_line_options(Options *opt, int argc, char **argv)
{
    int exit_value = TRUE;
    int c = 0;
    gchar *filename = NULL;

    while ((c = getopt_long (argc, argv, "vht", long_options, NULL)) != -1)
        {
            switch (c)
                {
                case 0:         /* long options handler */
                    break;

                case 'v':
                    exit_value = version();
                    opt->usage = TRUE;  /* We do not want to continue after */
                    break;

                case 'h':
                    exit_value = usage(1);
                    opt->usage = TRUE;
                    break;

                case 't':
                    exit_value = TRUE;
                    opt->tests = COVERAGE_TESTS;
                    break;

                default:
                    exit_value = usage(0);
                    opt->usage = TRUE;
                }
        }

    while (optind < argc)
        {
            filename = (char *) malloc (sizeof(char) * strlen(argv[optind]) + 1);
            strcpy(filename, argv[optind]);
            opt->filenames = g_list_prepend(opt->filenames, filename);
            optind++;
        }

    return exit_value;
}

/**
 * Inits the Options struct that contains all
 * stuff needed to managed command line options
 * within heraia
 * @return returns a newly allocated Options structure
 *         initialized to default values
 */
static Options *init_options_struct(void)
{
    Options *opt = NULL; /**< Structure to manage prgram's options */

    opt = (Options *) g_malloc0(sizeof(Options));

    opt->filenames = NULL;  /* At first we do not have any filename */
    opt->usage = FALSE;
    opt->tests = NO_TESTS;

    return opt;
}

/**
 *  main program
 *  options :
 *   - --version
 *   - --help
 *   - --tests
 */
int main (int argc, char ** argv)
{
    Options *opt; /**< A structure to manage the command line options  */
    gboolean exit_value = TRUE;
    heraia_struct_t *main_struct = NULL;
    GList *list = NULL;

    opt = init_options_struct();

    main_struct = heraia_init_main_struct();

    libheraia_initialize();

    if (main_struct->debug == TRUE)
        {
            fprintf(stdout, "Main struct initialized !\n");
        }

    /* Command line options evaluation */
    exit_value = manage_command_line_options(opt, argc, argv);

    if (opt->usage != TRUE)
        {
            if (main_struct->debug == TRUE)
                {
                    fprintf(stderr, "Beginning things\n");
                    libheraia_test(); /* testing libheraia */
                }

            /* init of gtk and new window */
            exit_value = gtk_init_check(&argc, &argv);

            if (load_heraia_ui(main_struct) == TRUE)
                {

                    log_message(main_struct, G_LOG_LEVEL_INFO, "Main interface loaded");
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, "Preference file is %s", main_struct->prefs->filename);
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, "data interpretor's tab is %d", main_struct->current_DW->tab_displayed);

                    init_heraia_plugin_system(main_struct);

                    if (opt->filenames != NULL)
                        {
                            list = g_list_first(opt->filenames);
                            while (list != NULL)
                            {
                                load_file_to_analyse(main_struct, list->data);
                                list = g_list_next(list);
                            }
                        }

                    log_message(main_struct, G_LOG_LEVEL_DEBUG, "main_struct : %p", main_struct);

                    init_heraia_interface(main_struct);

                    if (opt->tests != NO_TESTS)
                        {
                            if (opt->tests == COVERAGE_TESTS)
                                {
                                    exit_value = do_heraia_self_tests(main_struct);
                                }
                        }
                    else
                        {
                            /* gtk main loop */
                            gtk_main();

                            exit_value = TRUE;
                        }
                }
            else
                {
                    fprintf(stderr, "File heraia.gtkbuilder not found !\n");
                }
        }

    libheraia_finalize();

    return !exit_value; /* Apparently gtk TRUE and FALSE are inverted compared to bash ! */
}

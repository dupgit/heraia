/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  heraia.c
 *  heraia - an hexadecimal file editor and analyser based on ghex
 *
 *  (C) Copyright 2005 - 2012 Olivier Delhomme
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
/** @file heraia.c
 * This is the main program file.
 * Initialization is done here and then hand is passed to gtk's main thread
 */
/**
 * @author Olivier DELHOMME,
 *         Sébastien TRICAUD,
 *         Grégory AUDET,
 *         Aurélie DERAISME.
 * @version 0.1.8
 * @date 2005-2012
 */


#include "heraia_types.h"


static gboolean version(void);
static gboolean usage(int status);
static window_prop_t *init_window_properties(gint x, gint y, guint height, guint width, gboolean displayed);
static heraia_struct_t *init_window_property_struct(heraia_struct_t *main_struct);
static heraia_struct_t *heraia_init_main_struct(gchar *heraia_path);
static HERAIA_ERROR init_heraia_plugin_system(heraia_struct_t *main_struct);
static GList *init_heraia_location_list(gchar *heraia_path);
static gboolean manage_command_line_options(Options *opt, int argc, char **argv);
static void print_lirairies_versions(void);

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
    fprintf (stdout, Q_("heraia written by %s\n   %s - Version %s\n   License %s\n"), HERAIA_AUTHORS, HERAIA_DATE, HERAIA_VERSION, HERAIA_LICENSE);
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
            fprintf (stderr, Q_("Try `heraia --help' for more information.\n"));

            return FALSE;
        }
    else
        {
            version();
            fprintf(stdout, Q_("\nheraia is a simple hexadecimal file editor and file analyser"));
            fprintf(stdout, Q_("\nUsage :\n  heraia [options] filename(s)\n"));
            fprintf(stdout, Q_("\nOptions :\n"));
            fprintf(stdout, Q_("  -h, --help\tThis help.\n"));
            fprintf(stdout, Q_("  -v, --version\tProgram version information.\n"));
            fprintf(stdout, Q_("  -t, --tests=TESTS\tRuns some tests.\n"));
            fprintf(stdout, Q_("     TESTS might be :\n"));
            fprintf(stdout, Q_("     %d for coverage tests\n"), COVERAGE_TESTS);
            fprintf(stdout, Q_("     %d for loading files tests\n"), LOADING_TESTS);

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
    window_prop_t *goto_window = NULL;
    window_prop_t *result_window = NULL;
    window_prop_t *find_window = NULL;
    window_prop_t *fr_window = NULL;
    window_prop_t *fdft_window = NULL;

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
    goto_window = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);
    result_window = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);
    find_window = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);
    fr_window = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);
    fdft_window = init_window_properties(0, 0, WPT_DEFAULT_HEIGHT, WPT_DEFAULT_WIDTH, FALSE);

    /* Attach to the struct */
    win_prop->about_box = about_box;
    win_prop->data_interpretor = data_interpretor;
    win_prop->log_box = log_box;
    win_prop->main_dialog = main_dialog;
    win_prop->plugin_list = plugin_list;
    win_prop->ldt = ldt;
    win_prop->main_pref_window = main_pref_window;
    win_prop->goto_window = goto_window;
    win_prop->result_window = result_window;
    win_prop->find_window = find_window;
    win_prop->fr_window = fr_window;
    win_prop->fdft_window = fdft_window;

    /* attach it to the main struct so that it can be read everywhere */
    main_struct->win_prop = win_prop;

    return main_struct;
}


/**
 * Initialize the main structure (main_struct)
 * @param heraia_path is the path used to invoke heraia : '/usr/bin/heraia'
 *        invocation would lead to '/usr/bin'
 * @return a pointer to a newly initialized main structure
 */
static heraia_struct_t *heraia_init_main_struct(gchar *heraia_path)
{
    heraia_struct_t *main_struct = NULL;
    xml_t *xmls = NULL;

    main_struct = (heraia_struct_t *) g_malloc0(sizeof(heraia_struct_t));

    if (main_struct == NULL)
        {
            fprintf(stderr, Q_("Main structure could not be initialiazed !"));
            fprintf(stderr, Q_("Do you have a memory problem ?\n"));
            return NULL;
        }

    /* preference file name initialisation */
    main_struct->prefs = init_preference_struct(g_build_path(G_DIR_SEPARATOR_S, g_get_user_config_dir(), "heraia", NULL), "main_preferences");
    verify_preference_file(main_struct->prefs);

    /**
     * First, in this early stage of the development we want to toggle debugging
     *  mode ON which is enabled by default in the configure.ac file !
     */
    main_struct->debug = ENABLE_DEBUG;

    main_struct->current_doc = NULL;
    main_struct->plugins_list = NULL;
    main_struct->location_list = init_heraia_location_list(heraia_path); /* location list initilization */

    /* xml_t structure initialisation */
    xmls = (xml_t *) g_malloc0(sizeof(xml_t));
    xmls->main = NULL;
    main_struct->xmls = xmls;

    /* data interpretor structure initialization */
    main_struct->current_DW = (data_window_t *) g_malloc0 (sizeof(data_window_t));
    main_struct->current_DW->diw = NULL;
    main_struct->current_DW->tab_displayed = 0;  /* the first tab */

    /* init window property structure */
    main_struct = init_window_property_struct(main_struct);

    /* documents */
    main_struct->documents = g_ptr_array_new();

    /* find and replace stuff */
    main_struct->find_doc = NULL;
    main_struct->fr_find_doc = NULL;
    main_struct->fr_replace_doc = NULL;
    main_struct->results = g_ptr_array_new();
    main_struct->fdft = NULL;

    /* init global variable for the library */
    libheraia_main_struct = main_struct;

    return main_struct;
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
            log_message(main_struct, G_LOG_LEVEL_INFO, Q_("Enabling plugins"));
            load_plugins(main_struct);

            /* the plugin_list_window (here the plugins may be loaded !) */
            log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Inits the plugin list window"));
            plugin_list_window_init_interface(main_struct);

            return HERAIA_NOERR;
        }
    else
        {
            log_message(main_struct, G_LOG_LEVEL_WARNING, Q_("Plugins will be disabled"));
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
static GList *init_heraia_location_list(gchar *heraia_path)
{
    gchar *path = NULL;
    const gchar* const *system_dirs;
    guint i = 0;
    GList *location_list = NULL;

    /* Heraia's binary path */
    path = g_get_current_dir();
    location_list = g_list_prepend(location_list, path);

    /* System data dirs */
    system_dirs = g_get_system_data_dirs();
    i = 0;
    while(system_dirs[i] != NULL)
        {
            path = g_build_path(G_DIR_SEPARATOR_S, system_dirs[i], "heraia", NULL);
            location_list = g_list_prepend(location_list, path);
            i++;
        }

    /* System config dirs */
    system_dirs = g_get_system_config_dirs();
    i = 0;
    while(system_dirs[i] != NULL)
        {
            path = g_build_path(G_DIR_SEPARATOR_S, system_dirs[i], "heraia", NULL);
            location_list = g_list_prepend(location_list, path);
            i++;
        }

    /* The user path */
    path =  g_build_path(G_DIR_SEPARATOR_S, g_get_home_dir(), "heraia", NULL);
    location_list = g_list_prepend(location_list, path);

    /* A global user data path */
    path = g_build_path(G_DIR_SEPARATOR_S, g_get_user_data_dir(), "heraia", NULL);
    location_list = g_list_prepend(location_list, path);

    /* A global config data path */
    path = g_build_path(G_DIR_SEPARATOR_S, g_get_user_config_dir(), "heraia", NULL);
    location_list = g_list_prepend(location_list, path);

    /* An absolute path (from where is executed heraia itself) */
    path = g_build_path(G_DIR_SEPARATOR_S, heraia_path, "..", "share", "heraia", NULL);
    location_list = g_list_prepend(location_list, path);

    return location_list;
}


/**
 *  Inits internationalisation (don't know wether it works or not!)
 */
static void init_international_languages(void)
{
    gchar *result = NULL;
    gchar *codeset = NULL;
    gchar *text_domain = NULL;

    /* gtk_set_locale(); */
    result = bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    codeset = bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    text_domain = textdomain(GETTEXT_PACKAGE);

    if (ENABLE_DEBUG == TRUE)
        {
            fprintf(stdout, Q_("Gettext package : %s\n"), GETTEXT_PACKAGE);
            fprintf(stdout, Q_("Locale dir : %s\n"), LOCALEDIR);
            fprintf(stdout, Q_("Bindtextdomain : %s\n"), result);
            fprintf(stdout, Q_("Code set : %s\n"), codeset);
            fprintf(stdout, Q_("Text domain : %s\n"), text_domain);
        }
}


/**
 * Does some self tests for code coverage in heraia
 *
 * This function does some calls to functions that are not called
 * on a normal test procedure
 *
 * @param main_struct : main structure (heraia_struct_t *)
 * @return gboolean as an exit value for the program
 */
static gboolean do_heraia_coverage_tests(heraia_struct_t *main_struct)
{
    heraia_struct_t *tmp_struct;
    gboolean exit_value = FALSE;

    exit_value = version();
    exit_value = usage(0);
    exit_value = usage(1);

    tmp_struct = get_main_struct();
    if (tmp_struct != main_struct)
        {
            fprintf(stderr, Q_("WARNING : tmp_struct is not equal to main_struct!\n"));
        }

    exit_value = test_decode_functions();
    if (exit_value != TRUE)
        {
            fprintf(stderr, Q_("WARNING : Error while testing decode functions\n"));
        }

    return TRUE;
}


/**
 * Does some loading tests in heraia
 * @param main_struct : main structure (heraia_struct_t *)
 * @return gboolean as an exit value for the program
 */
static gboolean do_heraia_loading_tests(heraia_struct_t *main_struct)
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
    int tests = 0;

    while ((c = getopt_long(argc, argv, "vht", long_options, NULL)) != -1)
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
                        if (optarg)
                            {
                                if (sscanf(optarg, "%d", &tests) < 1)
                                    {
                                        /* Tests done by default */
                                        opt->tests = COVERAGE_TESTS;
                                    }
                                else
                                    {
                                        opt->tests = tests;
                                    }
                            }
                        else
                            {
                                opt->tests = COVERAGE_TESTS;
                            }
                         exit_value = TRUE;
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
 * Prints on stdout the librairies versions. To be used for debugging.
 * Takes no parameters and returns nothing.
 */
static void print_lirairies_versions(void)
{
    fprintf(stdout, Q_("GTK version : %d.%d.%d\n"),GTK_MAJOR_VERSION, GTK_MINOR_VERSION,GTK_MICRO_VERSION);
    fprintf(stdout, Q_("GLIB version : %d.%d.%d\n"), glib_major_version, glib_minor_version,  glib_micro_version);
}


/**
 *  main program
 *  options :
 *   - --version
 *   - --help
 *   - --tests
 */
int main(int argc, char **argv)
{
    Options *opt; /**< A structure to manage the command line options  */
    gboolean exit_value = TRUE;
    heraia_struct_t *main_struct = NULL;
    GList *list = NULL;
    gchar *heraia_path = NULL;

    if (argv != NULL && argv[0] != NULL)
        {
            heraia_path = g_path_get_dirname(argv[0]);
        }
    else
        {
            return -1;
        }

    init_international_languages();

    /* init of gtk */
    exit_value = gtk_init_check(&argc, &argv);

    opt = init_options_struct();

    main_struct = heraia_init_main_struct(heraia_path);

    libheraia_initialize();

    if (main_struct->debug == TRUE)
        {
            print_lirairies_versions();
            fprintf(stdout, Q_("Main struct initialized !\n"));
        }

    /* Command line options evaluation */
    exit_value = manage_command_line_options(opt, argc, argv);

    if (opt->usage != TRUE)
        {
            if (main_struct->debug == TRUE)
                {
                    fprintf(stderr, Q_("Beginning things\n"));
                    libheraia_test(); /* testing libheraia */
                }



            if (load_heraia_ui(main_struct) == TRUE)
                {

                    log_message(main_struct, G_LOG_LEVEL_INFO, Q_("Main interface loaded"));
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("Preference file is %s"), main_struct->prefs->filename);
                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("data interpretor's tab is %d"), main_struct->current_DW->tab_displayed);

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

                    log_message(main_struct, G_LOG_LEVEL_DEBUG, Q_("main_struct : %p"), main_struct);

                    init_heraia_interface(main_struct);

                    /* tests or main loop */
                    switch (opt->tests)
                            {
                                case COVERAGE_TESTS:
                                    exit_value = do_heraia_coverage_tests(main_struct);
                                break;

                                case LOADING_TESTS:
                                    exit_value = do_heraia_loading_tests(main_struct);
                                break;

                                default:
                                    /* gtk main loop */
                                    gtk_main();
                                    exit_value = TRUE;
                            }
                }
            else
                {
                    fprintf(stderr, Q_("File heraia.gtkbuilder not found !\n"));
                    if (main_struct->debug == TRUE)
                        {
                            list = main_struct->location_list;
                            while (list)
                                {
                                    /* here heraia_path is used only as a normal gchar*  */
                                    heraia_path = (gchar *) list->data;
                                    fprintf(stdout, "\t%s\n", heraia_path);
                                    list = g_list_next(list);
                                }
                        }
                }
        }

    libheraia_finalize();

    return !exit_value; /* Apparently gtk TRUE and FALSE are inverted compared to bash ! */
}

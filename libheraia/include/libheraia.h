/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  libheraia.h
  Heraia's library header

  (C) Copyright 2008 - 2011 Sébastien Tricaud, Olivier Delhomme
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */
/**
 * @file libheraia.h
 *
 *  This file contains all the definitions and includes all other .h
 *  files. This is not very friendly, but ease compilation on exotic
 *  systems.
 */
#ifndef _LIBHERAIA_H_
#define _LIBHERAIA_H_

/* I have some problems under windows if #ifdef statement is activated */

/* #ifdef HAVE_CONFIG_H */
#include "config.h"
/* #endif */  /* HAVE_CONFIG_H */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>
#include <gmodule.h>


#ifdef GTKHEX3
    #include <gtkhex-3/gtkhex.h>
#endif
#ifndef GTKHEX3
    #include <gtkhex/gtkhex.h>
#endif


/**
 * @typedef HexDocument Heraia_Document
 *  Abstract layer this may be usefull if we decide to leave Heraia_Hex
 *  and use something else !
 *
 * @typedef GtkHex Heraia_Hex
 *  Abstract layer this may be usefull if we decide to leave Heraia_Hex
 *  and use something else !
 */
/**
 * @typedef  gint HERAIA_ERROR
 *  Defines heraia error type (this should be used !)
 */
typedef HexDocument Heraia_Document;
typedef GtkHex Heraia_Hex;
typedef gint HERAIA_ERROR;

/**
 *  @typedef gint RefreshType
 *   Refresh type (may be used to decide what to do
 *   in a particular case)
 *  @warning This is not thread safe !!
 */
/**
 *  @def HERAIA_REFRESH_NOTHING
 *   When nothing is refreshed
 *
 *  @def HERAIA_REFRESH_NEW_FILE
 *   When a new file has been loaded
 *
 *  @def HERAIA_REFRESH_CURSOR_MOVE
 *   When the cursor is moving
 *
 *  @def HERAIA_REFRESH_TAB_CHANGED
 *   When user selects another tab in main notebook
 */
typedef gint RefreshType;
#define HERAIA_REFRESH_NOTHING 0
#define HERAIA_REFRESH_NEW_FILE 1
#define HERAIA_REFRESH_CURSOR_MOVE 2
#define HERAIA_REFRESH_TAB_CHANGED 3


/**
 * @def HERAIA_FIND_FORWARD
 *  When one wants to do a search in the forward direction
 *
 * @def HERAIA_FIND_BACKWARD
 *  When one wants to do a search in the backward direction
 *
 * @def HERAIA_FIND_ALL
 *  When one wants to do a global search in th whole document
 */
#define HERAIA_FIND_FORWARD 32
#define HERAIA_FIND_BACKWARD 64
#define HERAIA_FIND_ALL 128


/**
 * @struct date_and_time_t
 *  A human struct to store a date with a time.
 * @todo add an UTC info field
 */
typedef struct
{
    guint32 year;
    guint32 month;
    guint32 day;

    guint32 hour;
    guint32 minutes;
    guint32 seconds;
} date_and_time_t;


/** Templates for the decoding functions */
typedef gchar *(* DecodeFunc) (guchar *, gpointer);     /**< Decode function template */


/**
 * @struct decode_parameters_t
 * Used to pass decoding options to the functions. Those options are evaluated
 * from data_interpretor's window
 */
typedef struct
{
        guint endianness;  /**< endianness  */
        guint stream_size; /**< stream_size */
} decode_parameters_t;


/**
 * @struct decode_t
 * Basic way to associate a decode function and an entry that will receive the
 * result
 * @warning this structure is subject to changes
 */
 typedef struct
 {
    DecodeFunc func;  /**< a function to decode into something                  */
    GtkWidget *entry; /**< the widget that will receive the result              */
    gchar *err_msg;   /**< error message if something went wrong when decoding
                            expects a %d somewhere in the message to represents
                            the stream lenght to be decoded                     */
 } decode_t;


/**
 * @struct decode_generic_t
 * Basic way to have as many as we want decoding functions corresponding to one
 * label. This structure is basicaly one row in the data intrepretor window
 * @warning this structure is subject to changes
 */
 typedef struct
 {
    GPtrArray *decode_array; /**< Pointer Array of decode_t functions and corresponding entries */
    GtkWidget *label;        /**< label for these decoding functions                            */
    guint data_size;         /**< size of what we may decode                                    */
    gboolean fixed_size;     /**< says whether we can modify data_size or not                   */
 } decode_generic_t;


/**
 * @struct tab_t
 * Tabulation structure to be used in the GtkNoteBook of
 * data_interpretor's window
 */
typedef struct
{
    guint index;           /**< number for this tab                                             */
    guint nb_cols;         /**< number of columns in this tab - this MUST NOT change in any way */
    guint nb_rows;         /**< number of rows in this tab - this is automatically updated      */
    GtkWidget *label;      /**< label for this tab                                              */
    GPtrArray *col_labels; /**< array of GtkWidgets of columns labels                           */
    GPtrArray *vboxes;     /**< array of vboxes where we will pack label and entry widgets      */
    GPtrArray *rows;       /**< array of pointers to decode_generic_t variables.                */
} tab_t;


/**
 * @struct data_window_t
 *  Data interpretor window structure
 */
typedef struct
{
    GtkWidget *diw;                /**< data interpretor window                                           */
    gint tab_displayed;            /**< keeps the last displayed tab's number before closing              */
    guint nb_tabs;                 /**< keeps Number of tabs in the GPtrArray                             */
    GPtrArray *tabs;               /**< an array of tabs displayed in data interpretor's notebook (tab_t) */
} data_window_t;


/**
 * @struct xml_t
 *  Structure that contains all the xml definitions loaded at
 *  running time using GtkBuilder
 */
typedef struct
{
    GtkBuilder *main;      /**< the main interface xml description */
} xml_t;


/**
 * @def WPT_DEFAULT_HEIGHT
 *  Defines the default height for a window (set in window_prop_t)
 *
 * @def WPT_DEFAULT_WIDTH
 *  Defines the default width for a window (set in window_prop_t)
 */
#define WPT_DEFAULT_HEIGHT 200
#define WPT_DEFAULT_WIDTH 200


/**
 * @struct window_prop_t
 * Window properties
 *  - position (x,y) record window's position
 *  - displayed (boolean) say whether the window is displayed or not
 */
typedef struct
{
    gint x;              /**< x position (upper left corner)     */
    gint y;              /**< y position (upper left corner)     */
    guint height;        /**< y+height (bottom right corner)     */
    guint width;         /**< x+width (bottom right corner)      */
    gboolean displayed;  /**< TRUE if displayed, FALSE otherwise */
} window_prop_t;


/**
 * @struct all_window_prop_t
 *  Structure to keep window properties for each window
 *  @todo Transform this to a list of properties and add values to distinguish
 *        between windows ?
 */
typedef struct
{
    window_prop_t *about_box;
    window_prop_t *data_interpretor;  /**< data interpretor window    */
    window_prop_t *log_box;           /**< log window                 */
    window_prop_t *main_dialog;       /**< heraia's main window       */
    window_prop_t *plugin_list;       /**< plugin description window  */
    window_prop_t *ldt;               /**< list data types window     */
    window_prop_t *main_pref_window;  /**< main preference window     */
    window_prop_t *goto_window;       /**< goto dialog window         */
    window_prop_t *result_window;     /**< result window properties   */
    window_prop_t *find_window;       /**< find window                */
    window_prop_t *fr_window;         /**< find and replace window    */
    window_prop_t *fdft_window;       /**< find data from type window */
} all_window_prop_t;


/**
 * @struct prefs_t
 *  Data type related to preferences
 */
typedef struct
{
   gchar *filename;    /**< user preference file file name   */
   gchar *pathname;    /**< user preference file path name   */
   GKeyFile *file;     /**< preference file contents         */
} prefs_t;


/**
 * @struct doc_t
 * Proposal for a structure that will group all informations about
 * a single document. This structure is managed in heraia_io.c
 */
typedef struct
{
    Heraia_Document *hex_doc;  /**< Document definition related to Heraia_Hex (GtkHex) */
    GtkWidget *hex_widget;     /**< hexwidget corresponding to the document            */
    gboolean modified;         /**< If hex_doc->changed <> modified then the
                                    document has something changed that may need
                                    an upate                                           */
} doc_t;


/**
 * @struct selection_t
 * A structure to manage a single selection
 */
typedef struct
{
    guint64 start;  /**< Starting position of the selection */
    guint64 end;    /**< Ending position of the selection   */
} selection_t;


/**
 * @struct
 * A structure to manage the find data type window
 */
typedef struct
{
    GtkWidget *category_cb; /** ComboBox Widget for the category of the search                  */
    GtkWidget *type_cb;     /** ComboBox Widget for the type of the data in the category        */
    GtkWidget *feature_cb;  /** ComboBox Widget for the feature related to the type of the data */
} fdft_t;


/**
 * @struct heraia_struct_t
 *  This is the main structure. It contains all things that the program needs
 *  results GPtrArray stores the pointer of the corresponding document from
 *  which the search took place.
 */
typedef struct
{
    gboolean  debug;                /**< Used to tell the program wether we want to display debug messages or not  */
    doc_t *current_doc;             /**< This is a pointer to the current edited document                          */
    GPtrArray *documents;           /**< An array of doc_t in order to be able to open more than one doc           */
    xml_t *xmls;                    /**< All the xmls used in the program, loaded at running time                  */
    data_window_t *current_DW;      /**< data_interpretor pointer                                                  */
    GList *location_list;           /**< this is the location list where we store some paths                       */
    GList *plugins_list;            /**< A list of plugins                                                         */
    RefreshType event;              /**< Tells what is happening                                                   */
    all_window_prop_t *win_prop;    /**< Keeps window properties                                                   */
    prefs_t *prefs;                 /**< All datas related to main preferences                                     */
    doc_t *find_doc;                /**< find document and hexwidget for find window                               */
    doc_t *fr_find_doc;             /**< find and replace window, find document and hexwidget                      */
    doc_t *fr_replace_doc;          /**< find and replace window, replace document and hexwidget                   */
    fdft_t *fdft;                   /**< Keeps comboboxes created for the fdft window                              */
    GPtrArray *results;             /**< An array of pointers (doc_t *) for each tab in the result window.         */
} heraia_struct_t;


#include "data_interpretor.h"
#include "decode.h"
#include "ghex_heraia_interface.h"
#include "heraia_errors.h"
#include "heraia_io.h"
#include "heraia_ui.h"
#include "log.h"
#include "main_pref_window.h"
#include "plugin.h"
#include "plugin_list.h"
#include "user_prefs.h"
#include "tests.h"
#include "goto_dialog.h"
#include "result_window.h"
#include "find_replace_window.h"


extern int libheraia_test(void);

/**
 * Python specific
 */
extern void libheraia_initialize(void);
extern void libheraia_finalize(void);

#endif /* _LIBHERAIA_H_ */

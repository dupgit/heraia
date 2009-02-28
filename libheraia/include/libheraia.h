/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  libheraia.h
  Heraia's library header

  (C) Copyright 2008 - 2009 Sébastien Tricaud, Olivier Delhomme
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
/** 
 * @file libheraia.h 
 *
 *  This file contains all the definitions and includes all other .h
 *  file. This is not very friendly, but ease compilation on exotic
 *  systems.
 */
#ifndef _LIBHERAIA_H_
#define _LIBHERAIA_H_

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
#include <glade/glade.h>
#include <gmodule.h>

#include <gtkhex/gtkhex.h>

/**
 * @typedef HexDocument Heraia_Document
 *  Abstract layer this may be usefull if we decide to leave GtkHex 
 *  and use something else !
 *
 * @typedef GtkHex Heraia_Hex
 *  Abstract layer this may be usefull if we decide to leave GtkHex 
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
 */
typedef gint RefreshType;
#define HERAIA_REFRESH_NOTHING 0
#define HERAIA_REFRESH_NEW_FILE 1
#define HERAIA_REFRESH_CURSOR_MOVE 2

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
 * @struct decode_t
 * Basic way to associate a decode function and an entry that will receive the
 * result
 * @warning this structure is subject to changes
 */
 typedef struct
 {
    DecodeFunc func;  /**< a function to decode into something     */
    GtkWidget *entry; /**< the widget that will receive the result */
 } decode_t;


/**
 * @struct decode_generic_t
 * Basic way to have as many as we want decoding functions corresponding to one
 * label. This Structure is basicaly one row in the data intrepretor window
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
	guint index;           /**< number for this tab                                            */
	guint nb_cols;         /**< number of columns in this tab (this MUST NOT change in any way */
	GtkWidget *label;      /**< label for this tab                                             */
	GPtrArray *col_labels; /**< array of GtkWidgets of columns labels                          */
	GPtrArray *vboxes;     /**< array of vboxes where we will pack label and entry widgets     */
	GPtrArray *rows;       /**< array of pointers to decode_generic_t variables.               */
} tab_t;

/**
 * @struct data_window_t
 *  Data interpretor window structure
 */
typedef struct
{
    /** Current Hexwidget that we want data to be interpreted */
    GtkWidget *current_hexwidget;  /** @todo we may want to move this from here to heraia_window_t structure */
    GtkWidget *diw;                /**< data interpretor window                                              */
    gint tab_displayed;            /**< keeps the last displayed tab's number before closing                 */
    tab_t *tab;                    /**< only here for testing puposes @todo make this a GPtrArray            */
} data_window_t;

/* Treatment Stuff (if one wants to add new data types) */
typedef GList *(* TreatmentDoFunc) (GList *);     /**< Treatment function called while operating the treatment */
typedef void (* TreatmentInitFunc) (gpointer);    /**< Treatment init function                                 */
typedef void (* TreatmentDelFunc) (gpointer);     /**< Treatment delete function                               */
typedef gpointer (*TreatmentCopyFunc) (gpointer); /**< Treatment copy function that have to copy internal
													   structures (widgets and all stuff in it)                */
/**
 * @struct treatment_t
 *  Treatment structure
 * @warning I'm not happy with this struct and all data_type.c file. I plan
 *          to replace thoses ugly things with an embedded scripting language
 *          such as python.
 */
typedef struct
{
	gchar *name;               /**< Treatment name  */
	TreatmentDoFunc do_it;     /**< Treatment function that manages the whole treatment (interface + treatment itself) */
	TreatmentInitFunc init;    /**< inits the interface */
	TreatmentDelFunc kill;     /**< kills the treatment itself */
	TreatmentCopyFunc copy;    /**< Copy the gpointer data sub structure of the treatment itself */
	gpointer data;             /**< Generic treatment data. Each instantiated treatment may have it's own              */
} treatment_t;

/**
 * @struct treatment_container_t
 *  Structure in order to contain one treatment
 * @warning I'm not happy with this struct and all data_type.c file. I plan
 *          to replace thoses ugly things with an embedded scripting language
 *          such as python.
 */
typedef struct
{
	GtkWidget *container_box;  /**< Upper box containing the whole stuff                */
	GtkWidget *button_box;     /**< Right part of the hbox. Contains "-", GtkEntry, "+" */
	GtkWidget *combo_box;      /**< Left box where we have the combobox                 */
	GtkWidget *tment_list;     /**< Combobox containning the treatment list             */
	GtkWidget *result;         /**< The GtkEntry in the vbox                            */
	GtkWidget *moins;          /**< "-" button                                          */
	GtkWidget *plus;           /**< "+" button                                          */
	treatment_t *treatment;    /**< Selected treatment                                  */
} treatment_container_t;

/**
 * @struct data_type_t
 *  Data type structure entry that contains user defined data types
 *  This is integrated within a GList.
 *  !! Do not forget to update the functions related to this such as
 *   - new_data_type
 *   - free_data_type
 *   - copy_data_type
 *  See data_type.c for thoses functions
 * @warning I'm not happy with this struct and all data_type.c file. I plan
 *          to replace thoses ugly things with an embedded scripting language
 *          such as python.
 */
typedef struct
{
	gchar *name;             /**< Name of the data type                                                       */
	guint size;              /**< size of the data type  (here we may limit size entry, eg <= 16 for example) */
	GList *treatment_c_list; /**< Treatments containers to be applied (in the list order) to the data
								  (treatment_container_t *)                                                   */
	GtkWidget *di_label;     /**< label for the data_interpretor window                                       */
	GtkWidget *di_entry;     /**< entry for the data interpretor window                                       */
} data_type_t;


/**
 * @struct xml_t
 *  Structure that contains all the xml definitions loaded at
 *  running time using libglade
 */
typedef struct
{
	GladeXML *main;      /**< the main interface xml description */
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
	guint width;         /**< x+width (bottom right corner)     */
	gboolean displayed;  /**< TRUE if displayed, FALSE otherwise */
} window_prop_t;

/** 
 * @struct all_window_prop_t
 *  Structure to keep window properties for each window
 */
typedef struct
{
	window_prop_t *about_box;
	window_prop_t *data_interpretor;  /**< data interpretor window   */
	window_prop_t *log_box;           /**< log window                */
	window_prop_t *main_dialog;       /**< heraia's main window      */
	window_prop_t *plugin_list;       /**< plugin description window */
	window_prop_t *ldt;               /**< list data types window    */
	window_prop_t *main_pref_window;  /**< main preference window    */
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
 * @struct heraia_window_t
 *  This is the main structure (mainly named main_window due to historycal reasons)
 *  It contains all things that the program needs
 */
typedef struct
{
	gboolean  debug;                /**< Used to tell the program wether we want to display debug messages or not  */
	gchar *filename;                /**< this could (should) be a list of filenames !!!                            */
	Heraia_Document *current_doc;   /**< We may want to group this with current_hexwidget in a specific struct     */
	xml_t *xmls;                    /**< All the xmls used in the program, loaded at running time                  */
   	data_window_t *current_DW;      /**< data_interpretor pointer                                                  */
	GList *location_list;           /**< this is the location list where we store some paths                       */
	GList *plugins_list;            /**< A list of plugins                                                         */
	GList *data_type_list;          /**< A list of data types                                                      */
	data_type_t *current_data_type; /**< data type that is being edited                                            */
	GList *available_treatment_list;/**< Available treatments that can be used by the user in the data type window */
	RefreshType event;              /**< Tells what is happening                                                   */
	all_window_prop_t *win_prop;    /**< Keeps window properties                                                   */
	prefs_t *prefs;                 /**< All datas related to main preferences                                     */
} heraia_window_t;

#include "config.h"
#include "data_interpretor.h"
#include "data_type.h"
#include "decode.h"
#include "ghex_heraia_interface.h"
#include "heraia_errors.h"
#include "heraia_io.h"
#include "heraia_ui.h"
#include "list_data_types.h"
#include "log.h"
#include "main_pref_window.h"
#include "plugin.h"
#include "plugin_list.h"
#include "treatments.h"
#include "user_prefs.h"

extern int libheraia_test(void);

/** 
 * Python specific 
 */
extern void libheraia_initialize(void);
extern void libheraia_finalize(void);

#endif /* _LIBHERAIA_H_ */

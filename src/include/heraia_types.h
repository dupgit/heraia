/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  types.h
  types.h - all data types and pieces of common declarations
 
  (C) Copyright 2006 - 2008 Olivier Delhomme
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/

#ifndef _HERAIA_TYPES_H_
#define _HERAIA_TYPES_H_

/* I have some problems under windows if #ifdef statement is activated */

/* #ifdef HAVE_CONFIG_H */ 
#include "config.h"
/* #endif */  /* HAVE_CONFIG_H */


/** 
 *  As there seems to be huge differences between the linux
 *  and the windows developpement platform such as that
 *  the included paths are correctly taken into account
 *  under linux (and thus nobody bothers) but not under
 *  windows, I decided to put all the .h definitions here
 *  even if it's ugly, this is supposed to solve the problem.
 */

#include <getopt.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <gmodule.h>
#include <gtk/gtk.h>
#include <gtkhex/gtkhex.h>
#include <glade/glade.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


typedef HexDocument Heraia_Document;
typedef GtkHex Heraia_Hex;
typedef gint HERAIA_ERROR;

/**
 *  Refresh type (may be used to decide what to do
 *  in a particular case)
 */
typedef gint RefreshType;
#define HERAIA_REFRESH_NOTHING 0
#define HERAIA_REFRESH_NEW_FILE 1
#define HERAIA_REFRESH_CURSOR_MOVE 2

/**
 *  Data interpretor window structure
 */
typedef struct
{
	/* Current Hexwidget that we want data to be interpreted */
	GtkWidget *current_hexwidget;  /* we may want to move this from here to heraia_window_t structure */
	GtkWidget *diw;                /* data interpretor window                                         */

	gboolean window_displayed;     /* says whether the window is displayed or not                     */
	gint tab_displayed;            /* keeps the last displayed tab's number before closing            */

} data_window_t;

/* Treatment Stuff (if one wants to add new data types) */
typedef GList *(* TreatmentDoFunc) (GList *);     /* Treatment function called while operating the treatment */
typedef void (* TreatmentInitFunc) (gpointer);    /* Treatment init function                                 */
typedef void (* TreatmentDelFunc) (gpointer);     /* Treatment delete function                               */
typedef gpointer (*TreatmentCopyFunc) (gpointer); /* Treatment copy function that have to copy internal 
													 structures (widgets and all stuff in it)                */
/**
 *  Treatment structure
 */
typedef struct
{
	gchar *name;               /* Treatment name  */
	TreatmentDoFunc do_it;     /* Treatment function that manages the whole treatment (interface + treatment itself) */
	TreatmentInitFunc init;    /* inits the interface */
	TreatmentDelFunc kill;     /* kills the treatment itself */
	TreatmentCopyFunc copy;    /* Copy the gpointer data sub structure of the treatment itself */
	gpointer data;             /* Generic treatment data. Each instantiated treatment may have it's own              */
} treatment_t;

/**
 *  Structure in order to contain one treatment
 */
typedef struct
{
	GtkWidget *container_box;  /* Upper box containing the whole stuff                */
	GtkWidget *button_box;     /* Right part of the hbox. Contains "-", GtkEntry, "+" */
	GtkWidget *combo_box;      /* Left box where we have the combobox                 */
	GtkWidget *tment_list;     /* Combobox containning the treatment list             */
	GtkWidget *result;         /* The GtkEntry in the vbox                            */
	GtkWidget *moins;          /* "-" button                                          */
	GtkWidget *plus;           /* "+" button                                          */
	treatment_t *treatment;    /* Selected treatment                                  */
} treatment_container_t;

/**
 *  Data type structure entry that contains user defined data types
 *  This is integrated within a GList.
 *  !! Do not forget to update the functions related to this such as
 *   . new_data_type
 *   . free_data_type
 *   . copy_data_type
 *  See data_type.c for thoses functions
 */
typedef struct
{
	gchar *name;             /* Name of the data type                                                       */
	guint size;              /* size of the data type  (here we may limit size entry, eg <= 16 for example) */
	GList *treatment_c_list; /* Treatments containers to be applied (in the list order) to the data 
								(treatment_container_t *)                                                   */
	GtkWidget *di_label;     /* label for the data_interpretor window                                       */
	GtkWidget *di_entry;     /* entry for the data interpretor window                                       */
} data_type_t;


/**
 *  Structure that contains all the xml definitions loaded at 
 *  running time using libglade
 */
typedef struct
{
	GladeXML *main;       /* the main interface xml description       */
} xml_t;



/**
 * Window positions
 *
 */
typedef struct
{
	gint x;
	gint y;
} window_position;


typedef struct
{
	window_position *about_box;
	window_position *data_interpretor;
	window_position *log;
} all_window_pos;

/**
 *  This is the main structure (mainly named main_window due to historycal reasons)
 *  It contains all things that the program needs
 */
typedef struct
{
	gboolean  debug;                /* Used to tell the program wether we want to display debug messages or not  */
	gchar *filename;                /* this could (should) be a list of filenames !!!                            */
	Heraia_Document *current_doc;   /* We may want to group this with current_hexwidget in a specific struct     */
	xml_t *xmls;                    /* All the xmls used in the program, loaded at running time                  */
   	data_window_t *current_DW;      /* data_interpretor pointer                                                  */
	GList *location_list;           /* this is the location list where we store some paths                       */
	GList *plugins_list;            /* A list of plugins                                                         */
	GList *data_type_list;          /* A list of data types                                                      */
	data_type_t *current_data_type; /* data type that is being edited                                            */
	GList *available_treatment_list;/* Available treatments that can be used by the user in the data type window */
	RefreshType event;              /* Tells what is happening                                                   */
	all_window_pos *win_pos;        /* Keeps window positions                                                    */
} heraia_window_t;


/* a human struct to store a date with a time. TODO : add an UTC info */
typedef struct
{
	guint32 year;
	guint32 month;
	guint32 day;
	
	guint32 hour;
	guint32 minutes;
	guint32 seconds;

} date_and_time_t;

#include "data_interpretor.h"
#include "decode.h"
#include "ghex_heraia_interface.h"
#include "heraia.h"
#include "heraia_errors.h"
#include "heraia_io.h"
#include "heraia_ui.h"
#include "log.h"
#include "plugin.h"
#include "plugin_list.h"
#include "list_data_types.h"
#include "data_type.h"
#include "treatments.h"

#endif /* _HERAIA_TYPES_H_ */

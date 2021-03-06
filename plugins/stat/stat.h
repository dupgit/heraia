/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  stat.h
  an heraia plugin to calculate some stats on the opened file
  done as an example

  (C) Copyright 2007 - 2010 Olivier Delhomme
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/**
 * @file stat.h
 * Contains everything needed from stat.c : structures and defines
 */
#ifndef _PLUGIN_STAT_H_
#define _PLUGIN_STAT_H_

#include <libheraia.h>

#define API_VERSION 1
#define PLUGIN_TYPE HERAIA_PLUGIN_ACTION

#define PLUGIN_NAME "stat"
#define PLUGIN_VERSION "1.1"
#define PLUGIN_SUMMARY "stat"
#define PLUGIN_DESCRIPTION N_("Does some stats on the opened file")
#define PLUGIN_AUTHOR "Olivier Delhomme <heraia@delhomme.org>"
#define PLUGIN_HOMEPAGE "http://heraia.tuxfamily.org/"


typedef struct
{
    guint64 max;   /**< maximum value              */
    guint64 min;   /**< minimum value              */
    guint64 mean;  /**< mean value                 */
    guint nb_val;  /**< number of different values */
} histo_infos_t;


/**
 *  This is the stat structure
 */
typedef struct
{
    guint64 histo1D[256];        /**< The values for the 1D histogram                 */
    guint64 histo2D[256][256];   /**< The values for the 2D histogram                 */
    histo_infos_t *infos_1D;     /**< stores mathematical datas on the histogram (1D) */
    histo_infos_t *infos_2D;     /**< stores mathematical datas on the histogram (2D) */
    GdkPixbuf *pixbuf_1D;        /**< Pixbuf to be displayed (1D)                     */
    GdkPixbuf *pixbuf_2D;        /**< Pixbuf to be displayed (2D)                     */
    gchar *dirname;              /**< last openned directory where files were saved   */
} stat_t;


/**
 *  The plugin interface functions
 */
extern void init(heraia_struct_t *);            /**< When the application initialy starts up */
extern void quit(void);                         /**< When the application exits */
extern void run (GtkWidget *, gpointer);        /**< To run anything everytime the plugin is called */
extern void refresh(heraia_struct_t *, void *); /**< Called every refresh time (New file & cursor move) */
extern heraia_plugin_t *heraia_plugin_init(heraia_plugin_t *);

#endif /* _PLUGIN_STAT_H_ */

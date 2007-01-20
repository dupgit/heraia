/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  stat.h
  an heraia plugin to calculate some stats on the opened file
  done as an example
 
  (C) Copyright 2007 Olivier Delhomme
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

#ifndef _PLUGIN_STAT_H_
#define _PLUGIN_STAT_H_

#define API_VERSION 1
#define PLUGIN_TYPE HERAIA_PLUGIN_ACTION

#define PLUGIN_NAME "stat"
#define PLUGIN_VERSION "1"
#define PLUGIN_SUMMARY "stat"
#define PLUGIN_DESCRIPTION "Does some stats on the opened file"
#define PLUGIN_AUTHOR "Olivier Delhomme <heraia@delhomme.org>"
#define PLUGIN_HOMEPAGE "http://heraia.tuxfamily.org/"


typedef struct 
{
  guint64 histo1D[256];
  guint64 histo2D[256][256];
} stat_t;

/* the plugin interface functions */

extern void init(heraia_window_t *);      /* When the application initialy starts up */
extern void quit(void);                   /* When the application exits */
extern void run (GtkWidget *, gpointer);  /* To run anything everytime the plugin is called */
extern void refresh(heraia_window_t *, void *); /* Called every refresh time (New file & cursor move) */
extern heraia_plugin_t *heraia_plugin_init(heraia_plugin_t *);

#endif /* _PLUGIN_STAT_H_ */

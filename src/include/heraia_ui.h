/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_ui.h
  main menus, callback and utility functions
 
  (C) Copyright 2005 Olivier Delhomme
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

#ifndef _HERAIA_UI_H_
#define _HERAIA_UI_H_

#define H_DEBUG 1

extern void on_quitter1_activate( GtkWidget *widget,  gpointer data );
extern void on_nouveau1_activate( GtkWidget *widget, gpointer data );
extern void on_ouvrir1_activate( GtkWidget *widget,  gpointer data );
extern void on_enregistrer1_activate( GtkWidget *widget,  gpointer data );
extern void on_enregistrer_sous1_activate( GtkWidget *widget, gpointer data );
extern void on_fonte1_activate( GtkWidget *widget, gpointer data );
extern void on_supprimer1_activate( GtkWidget *widget, gpointer data );
extern void on_a_propos1_activate( GtkWidget *widget, gpointer data );
extern void on_couper1_activate( GtkWidget *widget, gpointer data );
extern void on_copier1_activate( GtkWidget *widget, gpointer data );
extern void on_coller1_activate( GtkWidget *widget, gpointer data );
extern void on_DIMenu_activate (GtkWidget *widget, gpointer data);

extern gboolean delete_dt_window_event( GtkWidget *widget, GdkEvent  *event, gpointer data );
extern void destroy_dt_window( GtkWidget *widget, GdkEvent  *event, gpointer data );

extern gboolean select_file_to_load(heraia_window_t *main_window);
extern void init_heraia_interface(heraia_window_t *main_window);
extern int load_heraia_ui(heraia_window_t *main_window);


#endif /* _HERAIA_UI_H_ */
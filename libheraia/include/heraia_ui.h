/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_ui.h
  main menus, callback and utility functions
 
  (C) Copyright 2005 - 2008 Olivier Delhomme
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

#ifndef _LIBHERAIA_HERAIA_UI_H_
#define _LIBHERAIA_HERAIA_UI_H_

#define H_DEBUG 1

extern void on_quit_activate(GtkWidget *widget, gpointer data);
extern void on_new_activate(GtkWidget *widget, gpointer data);
extern void on_open_activate(GtkWidget *widget, gpointer data);
extern void on_save_activate(GtkWidget *widget, gpointer data);
extern void on_save_as_activate(GtkWidget *widget, gpointer data);
extern void on_preferences_activate(GtkWidget *widget, gpointer data);
extern void on_delete_activate(GtkWidget *widget, gpointer data);
extern void a_propos_activate(GtkWidget *widget, gpointer data);
extern void on_cut_activate(GtkWidget *widget, gpointer data);
extern void on_copy_activate(GtkWidget *widget, gpointer data);
extern void on_paste_activate(GtkWidget *widget, gpointer data);
extern void on_DIMenu_activate(GtkWidget *widget, gpointer data);
extern gboolean delete_main_window_event(GtkWidget *widget, GdkEvent *event, gpointer data);

extern gboolean delete_dt_window_event(GtkWidget *widget, GdkEvent *event, gpointer data);
extern void destroy_dt_window(GtkWidget *widget, GdkEvent  *event, gpointer data);

/* file selection */
extern gboolean select_file_to_load(heraia_window_t *main_window);
extern gchar *select_a_file_to_save(heraia_window_t *main_window);

/* initialisation process */
extern void init_heraia_interface(heraia_window_t *main_window);
extern int load_heraia_ui(heraia_window_t *main_window);
extern void refresh_event_handler(GtkWidget *widget, gpointer data);
extern void connect_cursor_moved_signal(heraia_window_t *main_window);
extern void init_window_states(heraia_window_t *main_window);

/* window positionning system */
extern window_prop_t *move_and_show_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_pos);
extern window_prop_t *record_and_hide_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_pos);

/* libglade wrapper */
GtkWidget *heraia_get_widget(GladeXML *xml, gchar *widget_name);

/* Text view */
extern void add_text_to_textview(GtkTextView *textview, const char *format, ...);
extern void kill_text_from_textview(GtkTextView *textview);

/* RadioButton */
extern GtkWidget *gtk_radio_button_get_active(GSList *group);
extern GtkWidget *gtk_radio_button_get_active_from_widget(GtkRadioButton *radio_group_member);

/* CheckMenuItem */
extern gboolean is_cmi_checked(GtkWidget *check_menu_item);

/* Widget */
extern void destroy_a_single_widget(GtkWidget *widget);
extern void update_main_window_name(heraia_window_t *main_window);

/* NoteBook */
extern void set_notebook_tab_name(heraia_window_t *main_window);


#endif /* _LIBHERAIA_HERAIA_UI_H_ */

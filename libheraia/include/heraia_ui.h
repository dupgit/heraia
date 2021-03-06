/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_ui.h
  main menus, callback and utility functions

  (C) Copyright 2005 - 2011 Olivier Delhomme
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
 * @file heraia_ui.h
 * Header file for menu, callbacks, utility functions and ui management
 */
#ifndef _LIBHERAIA_HERAIA_UI_H_
#define _LIBHERAIA_HERAIA_UI_H_

#define H_DEBUG 1

/* Deprecated function rewritten */
extern void my_gtk_misc_set_padding(GtkWidget *widget, gint xpad, gint ypad);
extern void my_gtk_misc_set_alignment(GtkWidget *widget);


/* Menu functions */
extern void on_quit_activate(GtkWidget *widget, gpointer data);
extern void on_new_activate(GtkWidget *widget, gpointer data);
extern void on_open_activate(GtkWidget *widget, gpointer data);
extern void on_close_activate(GtkWidget *widget, gpointer data);
extern void on_save_activate(GtkWidget *widget, gpointer data);
extern void on_save_as_activate(GtkWidget *widget, gpointer data);
extern void on_preferences_activate(GtkWidget *widget, gpointer data);
extern void on_delete_activate(GtkWidget *widget, gpointer data);
extern void a_propos_activate(GtkWidget *widget, gpointer data);
extern void on_undo_activate(GtkWidget *widget, gpointer data);
extern void on_redo_activate(GtkWidget *widget, gpointer data);
extern void on_cut_activate(GtkWidget *widget, gpointer data);
extern void on_copy_activate(GtkWidget *widget, gpointer data);
extern void on_paste_activate(GtkWidget *widget, gpointer data);
extern void on_fr_activate(GtkWidget *widget, gpointer data);
extern void on_find_activate(GtkWidget *widget, gpointer data);
extern void on_fdft_activate(GtkWidget *widget, gpointer data);
extern void on_DIMenu_activate(GtkWidget *widget, gpointer data);
extern void on_tests_menu_activate(GtkWidget *widget, gpointer data);
extern gboolean delete_main_window_event(GtkWidget *widget, GdkEvent *event, gpointer data);

extern gboolean delete_dt_window_event(GtkWidget *widget, GdkEvent *event, gpointer data);
extern void destroy_dt_window(GtkWidget *widget, GdkEvent  *event, gpointer data);

/* file selection */
extern GSList *select_file_to_load(heraia_struct_t *main_struct, gboolean multiple, gchar *message);
extern gchar *select_a_file_to_save(heraia_struct_t *main_struct);
extern void set_the_working_directory(GtkFileChooser *file_chooser, gchar *filename);

/* initialisation process */
extern void init_heraia_interface(heraia_struct_t *main_struct);
extern int load_heraia_ui(heraia_struct_t *main_struct);
extern void init_window_states(heraia_struct_t *main_struct);
extern void grey_main_widgets(GtkBuilder *main, gboolean greyed);

/* refreshing things */
extern void refresh_event_handler(GtkWidget *widget, gpointer data);
extern void refresh_file_labels(heraia_struct_t *main_struct);
extern void connect_cursor_moved_signal(heraia_struct_t *main_struct, GtkWidget *hex_widget);
extern void connect_data_changed_signal(heraia_struct_t *main_struct, GtkWidget *hex_widget);
extern void data_has_changed(GtkWidget *widget, gpointer data);

/* window positionning system */
extern void record_dialog_box_position(GtkWidget *dialog_box, window_prop_t *dialog_prop);
extern void record_all_dialog_box_positions(heraia_struct_t *main_struct);
extern void move_and_show_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_pos);
extern void record_and_hide_dialog_box(GtkWidget *dialog_box, window_prop_t *dialog_pos);
extern void show_hide_widget(GtkWidget *widget, gboolean show, window_prop_t *win_prop);

/* libglade wrapper (now GtkBuilder's one) */
GtkWidget *heraia_get_widget(GtkBuilder *xml, gchar *widget_name);

/* Text view */
extern void add_text_to_textview(GtkTextView *textview, const char *format, ...);
extern void kill_text_from_textview(GtkTextView *textview);

/* RadioButton */
extern GtkWidget *gtk_radio_button_get_active(GSList *group);
extern GtkWidget *gtk_radio_button_get_active_from_widget(GtkRadioButton *radio_group_member);
extern void gtk_radio_button_set_active(GtkRadioButton *radio_button);

/* CheckMenuItem */
extern gboolean is_cmi_checked(GtkWidget *check_menu_item);

/* Toogle Button */
extern gboolean is_toggle_button_activated(GtkBuilder *main_xml, gchar *check_button);

/* Widget */
extern void destroy_a_single_widget(GtkWidget *widget);
extern void update_main_window_name(heraia_struct_t *main_struct);

/* NoteBook */
extern void set_notebook_tab_name(heraia_struct_t *main_struct);
extern void add_new_tab_in_main_window(heraia_struct_t *main_struct, doc_t *doc);
extern void set_notebook_tab_label_color(heraia_struct_t *main_struct, gboolean color);
extern gint find_tab_number_from_widget(heraia_struct_t *main_struct, gchar *notebook_name, GtkWidget *to_find);
extern GtkWidget *find_label_from_hbox(GtkWidget *hbox);
extern GtkWidget *create_tab_close_button(heraia_struct_t *main_struct, GtkWidget *tab_label, void *signal_handler);
extern gboolean file_notebook_tab_changed(GtkNotebook *notebook, GtkWidget *page, gint tab_num, gpointer data);

#endif /* _LIBHERAIA_HERAIA_UI_H_ */

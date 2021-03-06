/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  data_interpretor.h
  heraia - an hexadecimal file editor and analyser based on ghex

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
 * @file data_interpretor.h
 * Header file for the data interpretor
 */

#ifndef _DATA_INTERPRETOR_H_
#define _DATA_INTERPRETOR_H_

/**
 * @note
 *  Naming :
 *   H stands for Heraia
 *   DI stands for Data_Interpretor
 *
 * @def H_DI_DISPLAYED
 *  Says whether data_interpretor is displayed or not
 *  @todo this is used at initialisation time. We can add a new option to the
 *        program in order to say wether we want the data interpretor to be
 *        visible or not (regardless of options saved ?)
 *
 * @def H_DI_BIG_ENDIAN
 *  Stands for big endian representation
 *
 * @def H_DI_MIDDLE_ENDIAN
 *  Stands for middle endian representation (http://en.wikipedia.org/wiki/Endianness#Middle-endian)
 *
 * @def H_DI_LITTLE_ENDIAN
 *  Stands for little endian representation (this is the default)
 */
#define H_DI_DISPLAYED FALSE  /* By default the Data Interpretor Window is not displayed */

/* Little Endian, Middle Endian and Big Endian in a reverse order ;) */
#define H_DI_BIG_ENDIAN 2
#define H_DI_MIDDLE_ENDIAN 4
#define H_DI_LITTLE_ENDIAN 8

extern guint which_endianness(heraia_struct_t *main_struct);
extern guint which_stream_size(heraia_struct_t *main_struct);

extern void data_interpretor_init_interface(heraia_struct_t *main_struct);
extern void refresh_data_interpretor_window(GtkWidget *hexwidget, gpointer data);

extern void add_tab_in_data_interpretor(GtkNotebook *notebook, tab_t *tab);
extern tab_t *add_new_tab_in_data_interpretor(GtkNotebook *notebook, guint index, const gchar *label, guint num_cols, ...);
extern void add_new_row_to_tab(tab_t *tab, decode_generic_t *row);

extern gint di_get_selected_tab(heraia_struct_t *main_struct);
extern void di_set_selected_tab(heraia_struct_t *main_struct, gint selected_tab);

extern gint di_get_stream_size(heraia_struct_t *main_struct);
extern void di_set_stream_size(heraia_struct_t *main_struct, gint stream_size);

extern gint di_get_endianness(heraia_struct_t *main_struct);
extern void di_set_endianness(heraia_struct_t *main_struct, gint endianness);

#endif /* _DATA_INTERPRETOR_H_ */

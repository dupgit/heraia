/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  data_type.h
  Window allowing the user to create or edit a new data type
 
  (C) Copyright 2005 - 2007 Olivier Delhomme
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

#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

/**
 *  If any changes occurs in the .glade file description
 *  one might also do it here
 */
#define DT_SPIN_MIN 1
#define DT_SPIN_MAX 16
#define DT_BOX_SPACING 4
#define DT_BOX_PADDING 0

/*** Functions ***/
extern void refresh_hex_datas_entry(heraia_window_t *main_window);

extern void clear_data_type_widgets(heraia_window_t *main_window);
extern void fill_data_type_widgets(heraia_window_t *main_window, data_type_t *a_data_type);

extern GList *is_data_type_name_already_used(GList *data_type_list, gchar *name);

/* Structure operations */
extern data_type_t *new_data_type(gchar *name, guint size);
extern void free_data_type(data_type_t *a_data_type);
extern data_type_t *copy_data_type_struct(heraia_window_t *main_window, data_type_t *a_data_type);

/* interface initialization and construction */
extern void data_type_init_interface(heraia_window_t *main_window);
extern void show_data_type_window(heraia_window_t *main_window, data_type_t *a_data_type);
extern void create_ud_data_interpretor_widgets(heraia_window_t *main_window, data_type_t *a_data_type);
extern void refresh_all_ud_data_interpretor(heraia_window_t *main_window, guint endianness);

#endif /* _DATA_TYPE_H_ */

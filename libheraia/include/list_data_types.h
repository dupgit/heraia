/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  list_data_types.h
  window allowing the user to manage his data types (add, remove, edit and
  eventually save them)

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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifndef _LIST_DATA_TYPES_H_
#define _LIST_DATA_TYPES_H_

enum {
  LDT_TV_COLUMN_NAME,
  LDT_TV_N_COLUMNS
};

extern void on_ldt_menu_activate(GtkWidget *widget, gpointer data);
extern void list_data_types_init_interface(heraia_struct_t *main_struct);
extern void add_data_type_name_to_treeview(heraia_struct_t *main_struct, gchar *name);

#endif /* _LIST_DATA_TYPES_H_ */

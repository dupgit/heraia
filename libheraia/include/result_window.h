/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  result_window.h
  result_window.h - A window that will collect all results of functions such as
                    find, find a data from type, ...

  (C) Copyright 2010 Olivier Delhomme
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
/**
 * @file result_window.h
 * Header file for the result window
 */
#ifndef _HERAIA_RESULT_WINDOW_H_
#define _HERAIA_RESULT_WINDOW_H_

extern void result_window_show(GtkWidget *widget, gpointer data);
extern void result_window_init_interface(heraia_struct_t *main_struct);

#endif /* _HERAIA_RESULT_WINDOW_H_ */
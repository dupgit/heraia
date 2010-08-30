/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  goto_dialog.h
  goto_dialog.h - everything to manage the dialog

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
 * @file goto_dialog.h
 *  Header file for the goto dialog window
 */
#ifndef _HERAIA_GOTO_DIALOG_H_
#define _HERAIA_GOTO_DIALOG_H_

extern void on_goto_activate(GtkWidget *widget, gpointer data);
extern void goto_dialog_init_interface(heraia_struct_t *main_struct);

#endif /* _HERAIA_GOTO_DIALOG_H_ */

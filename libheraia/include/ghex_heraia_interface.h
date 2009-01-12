/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  ghex_heraia_interface.h
  heraia - an hexadecimal file editor and analyser based on ghex
 
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

#ifndef _GHEX_HERAIA_INTERFACE_H_
#define _GHEX_HERAIA_INTERFACE_H_

extern gboolean ghex_memcpy(GtkHex *gh, guint pos, guint len, guint endianness, guchar *result);
extern gboolean ghex_get_data(data_window_t *data_window, guint length, guint endianness, guchar *c);

extern HERAIA_ERROR heraia_hex_document_new(heraia_window_t *main_window, char *filename);
extern HERAIA_ERROR heraia_hex_document_save(heraia_window_t *main_window);
extern HERAIA_ERROR heraia_hex_document_save_as(heraia_window_t *main_window, gchar *filename);
extern gchar *heraia_hex_document_get_filename(Heraia_Document *doc);
extern guint64 ghex_file_size(GtkHex *gh);
extern guint64 ghex_get_cursor_position(data_window_t *data_window);

#endif /* _GHEX_HERAIA_INTERFACE_H_ */
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

/* 
	Returns 'len' number of bytes located at 'pos' in th GtkHex 
   document and puts it in the result variable

	We assume that a previous g_malloc has been done in order to
   use the function.
*/
extern gboolean ghex_memcpy(GtkHex *gh, guint pos, guint len, gboolean big_endian, guchar *result);

/* 
   kills the old document if it exists and add a new one, from the new filename
*/
extern HERAIA_ERROR heraia_hex_document_new(heraia_window_t *main_window, char *filename);

#endif /* _GHEX_HERAIA_INTERFACE_H_ */

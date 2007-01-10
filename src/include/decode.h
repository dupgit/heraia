/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  decode.h
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


#ifndef _DECODE_H_
#define _DECODE_H_

/* dates */
extern gint decode_dos_date( guchar *data, date_and_time_t *mydate );
extern gint decode_filetime_date( guchar *data, date_and_time_t *mydate );
extern gint decode_C_date( guchar *data, date_and_time_t *mydate );

/* maths */
extern gint decode_8bits_signed( guchar *data, gchar *result );
extern gint decode_8bits_unsigned( guchar *data, gchar *result );
extern gint decode_16bits_signed( guchar *data, gchar *result );
extern gint decode_16bits_unsigned( guchar *data, gchar *result );
extern gint decode_32bits_signed( guchar *data, gchar *result );
extern gint decode_32bits_unsigned( guchar *data, gchar *result );
extern gint decode_64bits_signed( guchar *data, gchar *result );
extern gint decode_64bits_unsigned( guchar *data, gchar *result );

extern gboolean swap_bytes(guchar *to_swap, guint first, guint last);

#endif /* _DECODE_H_ */

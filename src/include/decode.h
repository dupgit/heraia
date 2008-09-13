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
/* templates */
typedef gchar *(* DecodeFunc) (guchar *); /* Decode function template for numbers*/
typedef gchar *(* DecodeDateFunc) (guchar *, date_and_time_t *mydate); /* Decode function template for dates */

/* bits */
extern gchar *decode_to_bits(guchar *data);

/* dates */
extern gchar *decode_dos_date(guchar *data, date_and_time_t *mydate);
extern gchar *decode_filetime_date(guchar *data, date_and_time_t *mydate);
extern gchar *decode_C_date(guchar *data, date_and_time_t *mydate);
extern gchar *decode_HFS_date(guchar *data, date_and_time_t *mydate);

/* bcd */
extern gchar *decode_packed_BCD(guchar *data);

/* numbers ->  See libheraia
 extern gchar *decode_8bits_signed(guchar *data);
 extern gchar *decode_8bits_unsigned(guchar *data); 
 extern gchar *decode_16bits_signed(guchar *data);
 extern gchar *decode_16bits_unsigned(guchar *data);
 extern gchar *decode_32bits_signed(guchar *data);
 extern gchar *decode_32bits_unsigned(guchar *data);
 extern gchar *decode_64bits_signed(guchar *data);
 extern gchar *decode_64bits_unsigned(guchar *data);
*/
 
/* Utils */
extern gboolean swap_bytes(guchar *to_swap, guint first, guint last);
extern void reverse_byte_order(guchar *to_reverse);
#endif /* _DECODE_H_ */

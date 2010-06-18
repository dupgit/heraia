/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  decode.h
  Heraia's library decode.c

  (C) Copyright 2008 Sébastien Tricaud        e-mail : toady@gscore.org
  (C) Copyright 2008 - 2010 Olivier Delhomme  e-mail : heraia@delhomme.org
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
/**
 * @file decode.h
 * Header for decoding things (numbers, dates, binary, ...)
 */
#ifndef _LIBHERAIA_DECODE_H_
#define _LIBHERAIA_DECODE_H_

/*-- Decoding function --*/
/* Numbers */
extern gchar *decode_8bits_unsigned(guchar *data, gpointer data_struct);
extern gchar *decode_8bits_signed(guchar *data, gpointer data_struct);
extern gchar *decode_16bits_signed(guchar *data, gpointer data_struct);
extern gchar *decode_16bits_unsigned(guchar *data, gpointer data_struct);
extern gchar *decode_32bits_signed(guchar *data, gpointer data_struct);
extern gchar *decode_32bits_unsigned(guchar *data, gpointer data_struct);
extern gchar *decode_64bits_signed(guchar *data, gpointer data_struct);
extern gchar *decode_64bits_unsigned(guchar *data, gpointer data_struct);

/* Floats */
extern gchar *decode_float_normal(guchar *data, gpointer data_struct);
extern gchar *decode_float_scientific(guchar *data, gpointer data_struct);
extern gchar *decode_double_normal(guchar *data, gpointer data_struct);
extern gchar *decode_double_scientific(guchar *data, gpointer data_struct);

/* bits */
extern gchar *decode_to_bits(guchar *data, gpointer data_struct);

/* dates */
extern gchar *decode_dos_date(guchar *data, gpointer data_struct);
extern gchar *decode_filetime_date(guchar *data, gpointer data_struct);
extern gchar *decode_C_date(guchar *data, gpointer data_struct);
extern gchar *decode_HFS_date(guchar *data, gpointer data_struct);

/* bcd */
extern gchar *decode_packed_BCD(guchar *data, gpointer data_struct);

/*-- Non decoding functions --*/
/* Utils */
extern gboolean swap_bytes(guchar *to_swap, guint first, guint last);
extern void reverse_byte_order(guchar *to_reverse);
extern decode_parameters_t *new_decode_parameters_t(guint endianness, guint stream_size);
extern decode_t * new_decode_t(DecodeFunc decode_func, GtkWidget *entry, const gchar *err_msg);
extern decode_generic_t *new_decode_generic_t(const gchar *label, guint data_size, gboolean fixed_size, const gchar *err_msg, guint nb_cols, ...);

#endif /* _LIBHERAIA_DECODE_H_ */


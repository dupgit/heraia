/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  decode.h
  Heraia's library decode.c

  (C) Copyright 2008 Sébastien Tricaud        e-mail : toady@gscore.org
  (C) Copyright 2008 - 2009 Olivier Delhomme  e-mail : heraia@delhomme.org
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

/* templates */
typedef gchar *(* DecodeFunc) (guchar *);                              /**< Decode function template for numbers*/
typedef gchar *(* DecodeDateFunc) (guchar *, date_and_time_t *mydate); /**< Decode function template for dates */


/**
 * @struct decode_t
 * Basic way to associate a decode function and an entry that will receive the
 * result
 * @warning this structure is subject to changes
 */
 typedef struct
 {
	DecodeFunc func;  /**< a function to decode into something     */
    GtkWidget *entry; /**< the widget that will receive the result */
 } decode_t;


/**
 * @struct decode_generic_t
 * Basic way to have one decoding function for date
 * @warning this structure is subject to changes
 */
 typedef struct
 {
    GList decode_list; /**< List of decode_t functions and corresponding entries */
    guint how_much;    /**< says how much decoding function there is in the list */
    GtkWidget *label;  /**< label for these decoding functions                   */
    guint data_size;   /**< size of what we may decode                           */
 } decode_generic_t;


/**
 * @struct decode_date_t
 * Basic way to have one decoding function for date
 * @warning this structure is subject to changes
 */
 typedef struct
 {
	DecodeDateFunc date_func; /**< a function to decode into a date        */
	GtkWidget *entry;         /**< the widget that will receive the result */
	
    guint data_size;          /**< size of what we may decode              */
 } decode_date_t;



/** 
 * @struct date_and_time_t
 *  A human struct to store a date with a time. 
 * @todo add an UTC info field
 */
typedef struct
{
	guint32 year;
	guint32 month;
	guint32 day;

	guint32 hour;
	guint32 minutes;
	guint32 seconds;
} date_and_time_t;


/* Numbers */
extern gchar *decode_8bits_unsigned(guchar *data);
extern gchar *decode_8bits_signed(guchar *data);
extern gchar *decode_16bits_signed(guchar *data);
extern gchar *decode_16bits_unsigned(guchar *data);
extern gchar *decode_32bits_signed(guchar *data);
extern gchar *decode_32bits_unsigned(guchar *data);
extern gchar *decode_64bits_signed(guchar *data);
extern gchar *decode_64bits_unsigned(guchar *data);

/* bits */
extern gchar *decode_to_bits(guchar *data);

/* dates */
extern gchar *decode_dos_date(guchar *data, date_and_time_t *mydate);
extern gchar *decode_filetime_date(guchar *data, date_and_time_t *mydate);
extern gchar *decode_C_date(guchar *data, date_and_time_t *mydate);
extern gchar *decode_HFS_date(guchar *data, date_and_time_t *mydate);

/* bcd */
extern gchar *decode_packed_BCD(guchar *data);
 
/* Utils */
extern gboolean swap_bytes(guchar *to_swap, guint first, guint last);
extern void reverse_byte_order(guchar *to_reverse);

#endif /* _LIBHERAIA_DECODE_H_ */


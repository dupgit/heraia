/* -*- Mode: C; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*
  decode.c
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/

#include <string.h>

#include "types.h"
#include "decode.h"

static gboolean bissextile_year (guint32 year);
static void calc_which_month_day ( date_and_time_t *mydate, guint32 day, guint tab_ns_months[12]);
static void which_month_day ( date_and_time_t *mydate, guint32 day, gboolean bi);
static void which_year_month_day (date_and_time_t *mydate, guint32 days, guint32 base);

/*
   Says whether a year is a bissextile one or not
*/
static gboolean bissextile_year (guint32 year)
{

	if ((year % 4) == 0)
		{
			if ((year % 100) == 0)
				{
					if ((year % 400) == 0)
						return TRUE;
					else
						return FALSE;
				}
			else
				return TRUE;
		}
	else
		return FALSE;
}


static void calc_which_month_day (date_and_time_t *mydate, guint32 day, guint tab_ns_months[12])
{
	gushort i = 0;

	while (i<12 && day > tab_ns_months[i])
		i++;
	mydate->month = i + 1;
	if (i == 0)
		mydate->day = 1 + day;
	else
		mydate->day = (1 + day) - tab_ns_months[i-1];
}


static void which_month_day (date_and_time_t *mydate, guint32 day, gboolean bi)
{

	if (bi == TRUE)
		{
			if (day <= 366)
				{		 
					guint tab_ns_months[12] = { 31, 60, 91, 121, 152, 182, 
														 213, 244, 274, 305, 335, 366 } ;
					calc_which_month_day(mydate, day, tab_ns_months);
				}
			else
				{
					mydate->day = 0;
					mydate->month = 0;
				}
		}
	else
		{
			if (day <= 365)
				{
					guint tab_ns_months[12] = { 31, 59, 90, 120, 151, 181, 
														 212, 243, 273, 304, 334, 365 };
					calc_which_month_day(mydate, day, tab_ns_months);
				}
			else
				{
					mydate->day = 0;
					mydate->month = 0;
				}
		}
}

/*
  About date calculation : bissextile years are periods of 4 years except
  the years that we can divide by 100 and not 400.
  So we can distinguish 2 periods : one of 400 years and one of 4 years.
  - we have 100 bissextiles years in a period of 400 years this means that
    every 400 years we have exactly 146100 days.
  - we have 1 bissextile year every 4 years : this means that we have exactly
    1461 days every 4 years.
  As we can calculate _exactly_ the number of days in a filetime or C_date
  format, we could calculate _exactly_ the number of periods of 400 years and
  then the number of periods of 4 years.
*/
static void which_year_month_day (date_and_time_t *mydate, guint32 days, guint32 base)
{
	guint32 modulus = 0;
	guint32 reste = 0;
	guint32 nbdays = 0;
	
	if (days > 146100)
		{
			modulus = days / 146100;	
			mydate->year = modulus * 400;
			reste = modulus*3;     /* To add centuries in the 400 years modulus */
			days = days % 146100;
		}

	modulus = days / 1461;
	mydate->year += modulus * 4;
	reste += (modulus*4) / 100;    /* To add centuries */
	reste += days % 1461;

	mydate->year += base;
	if ( bissextile_year(mydate->year) )
		nbdays = 366;
	else
		nbdays = 365;

	while (reste > nbdays)
		{
			reste -= nbdays; 
			mydate->year +=1;
			if ( bissextile_year(mydate->year) )
				nbdays = 366;
			else
				nbdays = 365;
		}
 
	which_month_day (mydate, reste, bissextile_year(mydate->year));
}

/*
   general purpose of this function is to take a 4 byte data stream
   and convert it as if it is a dos date. If it is not, the result
   may be funny !
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 4 guchars
   result : at least 21 gchars
*/
gint decode_dos_date(guchar *data, date_and_time_t *mydate)
{

	if ((data == NULL) || (mydate == NULL))
		return FALSE;
	else
		{
			mydate->year = (data[3] >> 1) + 1980;
			mydate->month = ((data[3] & 0x01) << 3) + (data[2] >> 5);
			mydate->day = data[2] & 0x1F;
			mydate->hour = (data[1] & 0xF8) >> 3;
			mydate->minutes = ((data[1] & 0x07) << 3) + ((data[0] & 0xE0) >> 5);
			mydate->seconds = (data[0] & 0x1F) << 1;
			return TRUE;
		}
}


/*
   general purpose of this function is to take a 8 byte data stream
   and convert it as if it is a filetime date. If it is not, the result
   may be funny !
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 8 guchars
   result : at least 21 gchars
*/
gint decode_filetime_date(guchar *data, date_and_time_t *mydate)
{
	guint64 total = 0;
	guint64 calc = 0;
	guint32 days = 0;
 
	if ((data == NULL) || (mydate == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, 8 * sizeof (guchar));
			
			total = total / 10000000;
			calc = total  /  86400 ;
			days = (guint32) calc;
			
			which_year_month_day (mydate, days, 1601);
			
			mydate->hour = ((total % 86400) / 3600);
			mydate->minutes = ((total % 3600) / 60);
			mydate->seconds = (total % 60);
			
			return TRUE;
		}
}


/*
   general purpose of this function is to take a 4 byte data stream
   and convert it as if it is a C date. If it is not, the result may 
	be funny !
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 4 guchars
   result : at least 21 gchars
*/
gint decode_C_date(guchar *data, date_and_time_t *mydate)
{
	guint32 total = 0;
	guint32 days = 0;
  
	if ((data == NULL) || (mydate == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, 4 * sizeof (guchar));
		 		
			days = total /  86400 ;

			which_year_month_day (mydate, days, 1970);

			mydate->hour = ((total) % 86400) / 3600;
			mydate->minutes = ((total) % 3600) / 60;
			mydate->seconds = (total % 60);

			return TRUE;
		}
}

/*
   general purpose of this function is to take a 1 byte data stream
   and convert it as if it is an 8 bits signed number
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 1 guchars
   result : max 3 gchar
*/
gint decode_8bits_signed(guchar *data, gchar *result)
{
	gint8 total;

	if ((data == NULL) || (result == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, sizeof (guchar));
			sprintf(result, "%d", total);
			return TRUE;
		}
}


/*
   general purpose of this function is to take a 1 byte data stream
   and convert it as if it is an 8 bits signed number
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 1 guchars
   result : max 3 gchar
*/
gint decode_8bits_unsigned(guchar *data, gchar *result)
{
	guint8 total;

	if ((data == NULL) || (result == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, sizeof (guchar));
			sprintf(result, "%u", total);
			return TRUE;
		}
}


/*
   general purpose of this function is to take a 2 byte data stream
   and convert it as if it is a 16 bits signed number
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 2 guchars
   result : max 6 gchar
*/
gint decode_16bits_signed(guchar *data, gchar *result)
{
	gint16 total;

	if ((data == NULL) || (result == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, 2 * sizeof (guchar));
			sprintf(result, "%d", total);
			return TRUE;
		}
}


/*
   general purpose of this function is to take a 2 byte data stream
   and convert it as if it is a 16 bits unsigned number
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 2 guchars
   result : max 5 gchar
*/
gint decode_16bits_unsigned(guchar *data, gchar *result)
{
	guint16 total;

	if ((data == NULL) || (result == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, 2 * sizeof (guchar));
			sprintf(result, "%u", total);
			return TRUE;
		}
}


/*
   general purpose of this function is to take a 4 byte data stream
   and convert it as if it is a 32 bits signed number
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 4 guchars
   result : max 11 gchar
*/
gint decode_32bits_signed(guchar *data, gchar *result)
{
	gint32 total;

	if ((data == NULL) || (result == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, 4 * sizeof (guchar));
			sprintf(result, "%d", total);
			return TRUE;
		}
}


/*
   general purpose of this function is to take a 4 byte data stream
   and convert it as if it is a 32 bits unsigned number
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 4 guchars
   result : max 11 gchar
*/
gint decode_32bits_unsigned(guchar *data, gchar *result)
{
	guint32 total;

	if ((data == NULL) || (result == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, 4 * sizeof (guchar));
			sprintf(result, "%u", total);
			return TRUE;
		}
}

/*
   general purpose of this function is to take a 8 byte data stream
   and convert it as if it is a 64 bits signed number
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 8 guchars
   result : max 21 gchar
*/
gint decode_64bits_signed(guchar *data, gchar *result)
{
	gint64 total;

	if ((data == NULL) || (result == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, 8 * sizeof (guchar));
			sprintf(result, "%lld", total);
			return TRUE;
		}
}


/*
   general purpose of this function is to take a 8 byte data stream
   and convert it as if it is a 64 bits unsigned number
   We do asume that both *data and *result contains correct values
   and has enough space to store any result.
   data : 8 guchars
   result : max 21 gchar
*/
gint decode_64bits_unsigned(guchar *data, gchar *result)
{
	guint64 total;

	if ((data == NULL) || (result == NULL))
		return FALSE;
	else
		{
			bcopy (data, &total, 8 * sizeof (guchar));
			sprintf(result, "%llu", total);
			return TRUE;
		}
}


/*
  Swap bytes from the buffer to_swap
  recursive function
  call with first = 0 and last = last byte of buffer to swap
*/
gboolean swap_bytes(guchar *to_swap, guint first, guint last)
{
	guchar aux;

	if (first >= last)
		return TRUE;
	else
		{
			aux = to_swap[first];
			to_swap[first] = to_swap[last];
			to_swap[last] = aux;
			return swap_bytes(to_swap, ++first, --last);
		}
}

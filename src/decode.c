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

#include "heraia_types.h"

static gboolean bissextile_year(guint32 year);
static void calc_which_month_day(date_and_time_t *mydate, guint32 day, guint tab_ns_months[12]);
static void which_month_day(date_and_time_t *mydate, guint32 day, gboolean bi);
static guint32 remove_days_from_first_january(guint32 base_year, guint8 base_month, guint8 base_day);
static void which_year_month_day(date_and_time_t *mydate, guint32 days, guint32 base_year, guint base_month, guint8 base_day);
static void make_date_and_time(date_and_time_t *mydate, guchar *data, guint8 len, guint64 nbticks, guint32 base_year, guint base_month, guint8 base_day);
static void transform_bcd_to_human(gchar *bcd, guint8 part, guint8 part_number);

/**
 *  Says whether a year is a leap one or not
 */
static gboolean bissextile_year(guint32 year)
{

	if ((year % 4) == 0)
		{
			if ((year % 100) == 0)
				{
					if ((year % 400) == 0)
						{
							return TRUE;
						}
					else
						{
							return FALSE;
						}
				}
			else
				{
					return TRUE;
				}
		}
	else
		{
			return FALSE;
		}
}

/**
 *  Says, from a number of days (eg 154), which month it is (eg  may)
 *  and which day in the corresponding month (eg 2 (leap year) or 3)
 */
static void calc_which_month_day(date_and_time_t *mydate, guint32 day, guint tab_ns_months[12])
{
	gushort i = 0;

	while (i<12 && day > tab_ns_months[i])
		{
			i++;
		}

	mydate->month = i + 1;

	if (i == 0)
		{
			mydate->day = 1 + day;
		}
	else
		{
			mydate->day = (1 + day) - tab_ns_months[i-1];
		}
}

/**
 *  Front end function for the calc_which_month_day function !
 */
static void which_month_day(date_and_time_t *mydate, guint32 day, gboolean bi)
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

/**
 *  Returns the number of days since 01/01/base_year
 *  eg 15/02/base_year --> 31 + 15 = 46
 */
static guint32 remove_days_from_first_january(guint32 base_year, guint8 base_month, guint8 base_day)
{ 
	guint tab_ns_months[11];
  
	if (base_day > 0 && base_day < 32)
		{
			base_day -= 1;
		}
	else
		{
			return 0;  /* Obviously something might be wrong if we fall here */
		}

	tab_ns_months[0] = 31;
	if (bissextile_year(base_year))
		{
			tab_ns_months[1] = 60;
			tab_ns_months[2] = 91;
			tab_ns_months[3] = 121;
			tab_ns_months[4] = 152;
			tab_ns_months[5] = 182;
			tab_ns_months[6] = 213;
			tab_ns_months[7] = 244;
			tab_ns_months[8] = 274;
			tab_ns_months[9] = 305;
			tab_ns_months[10] = 335;
		}
	else
		{
			tab_ns_months[1] = 59;
			tab_ns_months[2] = 90;
			tab_ns_months[3] = 120;
			tab_ns_months[4] = 151;
			tab_ns_months[5] = 181;
			tab_ns_months[6] = 212;
			tab_ns_months[7] = 243;
			tab_ns_months[8] = 273;
			tab_ns_months[9] = 304;
			tab_ns_months[10] = 334;
		}

	if (base_month > 1 && base_month < 13)
		{
			return (tab_ns_months[base_month-2] + base_day);
		}
	else if (base_month == 1)
		{
			return base_day;
		}
	else
		{
			return 0;
		}
}



/**
 * About date calculation : Leap years are periods of 4 years except
 * the years that we can divide by 100 and not 400.
 * So we can distinguish 2 periods : one of 400 years and one of 4 years.
 *  - we have 100 bissextiles years in a period of 400 years this means that
 *    every 400 years we have exactly 146100 days.
 *  - we have 1 bissextile year every 4 years : this means that we have exactly
 *    1461 days every 4 years.
 *  As we can calculate _exactly_ the number of days in a filetime or C_date
 *  format, we could calculate _exactly_ the number of periods of 400 years and
 *  then the number of periods of 4 years.
*/
static void which_year_month_day (date_and_time_t *mydate, guint32 days, guint32 base_year, guint base_month, guint8 base_day)
{
	guint32 modulus = 0;
	guint32 reste = 0;
	guint32 nbdays = 0;
	
	days -= remove_days_from_first_january(base_year, base_month, base_day);

	if (days > 146100)
		{
			modulus = days / 146100;	
			mydate->year = modulus * 400;
			reste = modulus * 3;     /* To add centuries in the 400 years modulus */
			days = days % 146100;
		}

	modulus = days / 1461;
	mydate->year += modulus * 4;
	reste += (modulus*4) / 100;    /* To add centuries */
	reste += days % 1461;

	mydate->year += base_year;
	if (bissextile_year(mydate->year))
		nbdays = 366;
	else
		nbdays = 365;

	while (reste > nbdays)
		{
			reste -= nbdays; 
			mydate->year += 1;
			if (bissextile_year(mydate->year))
				nbdays = 366;
			else
				nbdays = 365;
		}
 
	which_month_day(mydate, reste, bissextile_year(mydate->year));
}


/**
 *  Return a gchar* that contains the date and time
 *  encoded from the values contained in the date_and_time_t structure
 *  it may be freed when no longer needed
 *  We do not use any of the g_date_strftime or strftime function
 *  because interpreted dates are not always valid !
 */
static gchar *date_printf(date_and_time_t *mydate)
{
	return g_strdup_printf("%02u/%02u/%04u - %02u:%02u:%02u", mydate->day, mydate->month, mydate->year, mydate->hour, mydate->minutes, mydate->seconds);
}


/**
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a dos date. If it is not, the result
 *  may be funny !
 *  data : 4 guchars
 *  returns a gchar* that may be freed when no longer needed 
*/
gchar *decode_dos_date(guchar *data, date_and_time_t *mydate)
{

	if (data == NULL) 
		{
			return NULL;
		}
	else
		{
			mydate->year = (data[3] >> 1) + 1980;
			mydate->month = ((data[3] & 0x01) << 3) + (data[2] >> 5);
			mydate->day = data[2] & 0x1F;
			mydate->hour = (data[1] & 0xF8) >> 3;
			mydate->minutes = ((data[1] & 0x07) << 3) + ((data[0] & 0xE0) >> 5);
			mydate->seconds = (data[0] & 0x1F) << 1;

			return date_printf(mydate);
		}
}


/**
 *  Reads the data from the stream (specified length !! <= 64 bits )
 *   . date_and_time_t *mydate : the resulting date
 *   . guchar *data : the stream
 *   . guint8 len : length of the stream in bytes ( must be <= 64 bits)
 *   . guint64 nbticks : number of ticks per seconds (1, 1000, ...)
 *   . guint32 base_year : Epoch year (1970, 1904, ...)
 *   . guint8 base_month : Epoch month (january, ...)
 *   . guint8 base_day : Epoch day (01, 15, ...)
 *  populates the date_and_time_t structure
*/
static void make_date_and_time(date_and_time_t *mydate, guchar *data, guint8 len, guint64 nbticks, guint32 base_year, guint base_month, guint8 base_day)
{
	guint64 total = 0;
	guint32 days = 0;
  
	memcpy(&total, data, len * sizeof (guchar));

	total = (total / nbticks);         /* 1 seconds represents nbticks */
	days = (guint32) (total / 86400);  /* 86400 seconds a day          */;

	which_year_month_day(mydate, days, base_year, base_month, base_day);

	mydate->hour = ((total % 86400) / 3600);  /* 3600 seconds is one hour out of one day */
	mydate->minutes = ((total % 3600) / 60);  /* 60 seconds is one minute out of one hour */
	mydate->seconds = (total % 60);
}


/**
 *  general purpose of this function is to take a 8 byte data stream
 *  and convert it as if it is a filetime date. If it is not, the result
 *  may be funny ! Counting 100th of nanoseconds from 01/01/1601
 *  data : 8 guchars
 *  returns a gchar* that may be freed when no longer needed 
*/
gchar *decode_filetime_date(guchar *data, date_and_time_t *mydate)
{
	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			make_date_and_time(mydate, data, 8, 10000000, 1601, 1, 1);
			return date_printf(mydate);
		}
}


/**
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a C date. If it is not, the result may 
 *  be funny ! Counting seconds from 01/01/1970
 *  data : 4 guchars
 *  returns a gchar* that may be freed when no longer needed 
*/
gchar *decode_C_date(guchar *data, date_and_time_t *mydate)
{ 
	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			make_date_and_time(mydate, data, 4, 1, 1970, 1, 1);
			return date_printf(mydate);
		}
}

/**
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a HFS date. If it is not, the result may 
 *  be funny ! Counting seconds 01/01/1904
 *  data : 4 guchars
 *  returns a gchar* that may be freed when no longer needed 
*/
gchar *decode_HFS_date(guchar *data, date_and_time_t *mydate)
{
	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			make_date_and_time(mydate, data, 4, 1, 1904, 1, 1);
			return date_printf(mydate);
		}
}



/**
 *  decodes the stream represented by *data (one byte) to a
 *  string containing eight 0 or 1 (Little Endian style)
 */
gchar *decode_to_bits(guchar *data)
{

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			return g_strdup_printf("%1u%1u%1u%1u%1u%1u%1u%1u",
										  (data[0] & 0x80) > 0 ? 1 : 0, 
										  (data[0] & 0x40) > 0 ? 1 : 0,
										  (data[0] & 0x20) > 0 ? 1 : 0, 
										  (data[0] & 0x10) > 0 ? 1 : 0,
										  (data[0] & 0x08) > 0 ? 1 : 0, 
										  (data[0] & 0x04) > 0 ? 1 : 0,
										  (data[0] & 0x02) > 0 ? 1 : 0, 
										  (data[0] & 0x01));
		}
}


/**
 *  transcribes the bcd number "part" into a 
 *  gchar * human readable string
 *  Coding style is from ETSI GSM 04.08 ETS 300557 p387
 *  TODO : give choice of coding style (eg for numbers >=10)
 */
static void transform_bcd_to_human(gchar *bcd, guint8 part, guint8 part_number)
{
	switch (part)
		{
		case 0 :
			bcd[part_number] = '0';
			break;
		case 1 :
			bcd[part_number] = '1';
			break;
		case 2 :
			bcd[part_number] = '2';
			break;
		case 3 :
			bcd[part_number] = '3';
			break;
		case 4 :
			bcd[part_number] = '4';
			break;
		case 5 :
			bcd[part_number] = '5';
			break;
		case 6 :
			bcd[part_number] = '6';
			break;
		case 7 :
			bcd[part_number] = '7';
			break;
		case 8 :
			bcd[part_number] = '8';
			break;
		case 9 :
			bcd[part_number] = '9';
			break;
		case 10 :
			bcd[part_number] = '*';
			break;
		case 11 :
			bcd[part_number] = '#';
			break;
		case 12 :
			bcd[part_number] = 'a';
			break;
		case 13 :
			bcd[part_number] = 'b';
			break;
		case 14 :
			bcd[part_number] = 'c';
			break;
		case 15 :
			bcd[part_number] = ' ';  /* Endmark */
			break;
		default :
			bcd[part_number] = '?';  /* This default case should never happen */
			break;
		}
}


/**
 *  Decode one byte as a Packed BCD (Binary Coded Decimal)
 *  and return a gchar* that me be freed when no longer 
 *  needed
 */
gchar *decode_packed_BCD(guchar *data)
{
	guint8 total = 0;
	gchar *bcd = NULL;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, sizeof(guchar));
			bcd = (gchar *) g_malloc0(3 * sizeof(gchar));
			transform_bcd_to_human(bcd, (total & 0x0F), 0);
			transform_bcd_to_human(bcd, ((total & 0xF0)>>4), 1);
			bcd[2] = '\0';

			return bcd;
		}
}


/**
 *  general purpose of this function is to take a 1 byte data stream
 *  and convert it as if it is an 8 bits signed number
 *  data : 1 guchar
 *  returns a gchar* that may be freed when no longer needed 
 */
gchar *decode_8bits_signed(guchar *data)
{
	gint8 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, sizeof (guchar));
			return g_strdup_printf("%d", total);
		}
}


/**
 *  general purpose of this function is to take a 1 byte data stream
 *  and convert it as if it is an 8 bits unsigned number
 *  data : 1 guchar
 *  returns a gchar* that may be freed when no longer needed
*/
gchar *decode_8bits_unsigned(guchar *data)
{
	guint8 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, sizeof (guchar));
			return g_strdup_printf("%u", total);
		}
}


/**
 *  general purpose of this function is to take a 2 byte data stream
 *  and convert it as if it is a 16 bits signed number
 *  data : 2 guchars
 *  returns a gchar* that may be freed when no longer needed
 */
gchar *decode_16bits_signed(guchar *data)
{
	gint16 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, 2 * sizeof (guchar));
			return g_strdup_printf("%d", total);
		}
}


/**
 *  general purpose of this function is to take a 2 byte data stream
 *  and convert it as if it is a 16 bits unsigned number
 *  data : 2 guchars
 *  returns a gchar* that may be freed when no longer needed
 */
gchar *decode_16bits_unsigned(guchar *data)
{
	guint16 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, 2 * sizeof (guchar));
			return g_strdup_printf("%u", total);
		}
}


/**
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a 32 bits signed number
 *  data : 4 guchars
 *  returns a gchar* that may be freed when no longer needed
*/
gchar *decode_32bits_signed(guchar *data)
{
	gint32 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, 4 * sizeof (guchar));
			return g_strdup_printf("%d", total);
		}
}


/**
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a 32 bits unsigned number
 *  data : 4 guchars
 *  returns a gchar* that may be freed when no longer needed
*/
gchar *decode_32bits_unsigned(guchar *data)
{
	guint32 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, 4 * sizeof (guchar));
			return g_strdup_printf("%u", total);
		}
}

/**
 *  general purpose of this function is to take a 8 byte data stream
 *  and convert it as if it is a 64 bits signed number
 *  data : 8 guchars
 *  returns a gchar* that may be freed when no longer needed
*/
gchar *decode_64bits_signed(guchar *data)
{
	gint64 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, 8 * sizeof (guchar));
			return g_strdup_printf("%lld", total);
		}
}


/**
 *  general purpose of this function is to take a 8 byte data stream
 *  and convert it as if it is a 64 bits unsigned number
 *  data : 8 guchars
 *  returns a gchar* that may be freed when no longer needed
*/
gchar *decode_64bits_unsigned(guchar *data)
{
	guint64 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, 8 * sizeof (guchar));
			return g_strdup_printf("%llu", total);
		}
}


/**
 *  Swap bytes from the buffer to_swap
 *  recursive function !!
 *  call with first = 0 and last = last byte of buffer to swap
 */
gboolean swap_bytes(guchar *to_swap, guint first, guint last)
{
	guchar aux;

	if (first >= last)
		{
			return TRUE;
		}
	else
		{
			aux = to_swap[first];
			to_swap[first] = to_swap[last];
			to_swap[last] = aux;
			return swap_bytes(to_swap, ++first, --last);
		}
}

/**
 *  Reverse the byte order LSB -> MSB in MSB -> LSB
 *  12345678 in 87654321
 */
void reverse_byte_order(guchar *to_reverse)
{
	guint8 car = (guint8) to_reverse[0];
	guint8 aux = 0;

	aux = ((car & 0x80) >> 7);
	aux += ((car & 0x40) >> 5);
	aux += ((car & 0x20) >> 3);
	aux += ((car & 0x10) >> 1);
	aux += ((car & 0x08) << 1);
	aux += ((car & 0x04) << 3);
	aux += ((car & 0x02) << 5);
	aux += ((car & 0x01) << 7);

	to_reverse[0] = (guchar) aux;
}

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  decode.c
  Heraia's library decode.c

  (C) Copyright 2008 - 2009 Sébastien Tricaud, Olivier Delhomme
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
/**
 * @file decode.c
 * This file include all functions that may help in decoding a binary stream
 * to something else such as numbers, dates, other binary stream and so on
 * @todo I see, while putting some functions documentation that there may be
 *       things to do to improve speed with date calculations -> should we fill
 *       tab_ns_months at every function calls ? This may not be necessary
 */
#include <libheraia.h>

static gboolean bissextile_year(guint32 year);
static void calc_which_month_day(date_and_time_t *mydate, guint32 day, guint tab_ns_months[12]);
static void which_month_day(date_and_time_t *mydate, guint32 day, gboolean bi);
static guint32 remove_days_from_first_january(guint32 base_year, guint8 base_month, guint8 base_day);
static void which_year_month_day(date_and_time_t *mydate, guint32 days, guint32 base_year, guint base_month, guint8 base_day);
static void make_date_and_time(date_and_time_t *mydate, guchar *data, guint8 len, guint64 nbticks, guint32 base_year, guint base_month, guint8 base_day);
static void transform_bcd_to_human(gchar *bcd, guint8 part, guint8 part_number);

/**
 * @fn gchar *decode_8bits_signed(guchar *data)
 *  General purpose of this function is to take a 1 byte data stream
 *  and convert it as if it is an 8 bits signed number
 *  @param data : 1 guchar
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_8bits_unsigned(guchar *data)
 *  general purpose of this function is to take a 1 byte data stream
 *  and convert it as if it is an 8 bits unsigned number
 *  @param data : 1 guchar
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_16bits_signed(guchar *data)
 *  general purpose of this function is to take a 2 byte data stream
 *  and convert it as if it is a 16 bits signed number
 *  @param data : 2 guchars
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_16bits_unsigned(guchar *data)
 *  general purpose of this function is to take a 2 byte data stream
 *  and convert it as if it is a 16 bits unsigned number
 *  @param data : 2 guchars
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_32bits_signed(guchar *data)
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a 32 bits signed number
 *  @param data : 4 guchars
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_32bits_unsigned(guchar *data)
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a 32 bits unsigned number
 *  @param data : 4 guchars
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_64bits_signed(guchar *data)
 *  general purpose of this function is to take a 8 byte data stream
 *  and convert it as if it is a 64 bits signed number
 *  @param data : 8 guchars
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_64bits_unsigned(guchar *data)
 *  general purpose of this function is to take a 8 byte data stream
 *  and convert it as if it is a 64 bits unsigned number
 *  @param data : 8 guchars
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn static gboolean bissextile_year(guint32 year)
 *  Says whether a year is a leap one or not
 * @param year : a guint32 representing a year such as 2009
 * @return TRUE if it's a leap year FALSE instead.
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
 * @fn void calc_which_month_day(date_and_time_t *mydate, guint32 day, guint tab_ns_months[12])
 *  Says, from a number of days (eg 154), which month it is (eg  may)
 *  and which day in the corresponding month (eg 2 (leap year) or 3)
 *  @param[out] mydate : Filled date structure
 *	@param day : guint32 representing the number of day in the year (1..365/366)
 *  @param tab_ns_months : an array filled with the cumulative number of days for each month (31 to 365/366)
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
 * @fn void which_month_day(date_and_time_t *mydate, guint32 day, gboolean bi)
 *  Front end function for the calc_which_month_day function !
 * @param[out] mydate : Filled date structure
 * @param day : guint32 representing the number of day in the year (1..365/366)
 * @param bi : TRUE if it's a leap year, FALSE instead
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
 * @fn guint32 remove_days_from_first_january(guint32 base_year, guint8 base_month, guint8 base_day)
 *  Returns the number of days since 01/01/base_year
 *  eg 15/02/base_year --> 31 + 15 = 46
 * @param base_year : year where we want to begin the calculation
 * @param base_month : month (in the example 02)
 * @param base_day : day in the month (from the example 15)
 * @return returns a number of days since begining of the year (eg 1..365/366)
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
 * @fn void which_year_month_day(date_and_time_t *mydate, guint32 days, guint32 base_year, guint base_month, guint8 base_day)
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
 * @param[out] mydate : Filled date structure
 * @param days : number of days calculated
 * @param base_year : base year used for calculation (eg 1601 for filetime)
 * @param base_month : base month used for calculation (eg 01, january for filetime)
 * @param base_day : base day used for calculation (eg 01 for filetime)
*/
static void which_year_month_day(date_and_time_t *mydate, guint32 days, guint32 base_year, guint base_month, guint8 base_day)
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
 * @fn gchar *date_printf(date_and_time_t *mydate)
 *  Return a gchar * that contains the date and time
 *  encoded from the values contained in the date_and_time_t structure
 *  it may be freed when no longer needed
 *  We do not use any of the g_date_strftime or strftime function
 *  because interpreted dates are not always valid !
 * @param[in] mydate : structure that contains a date
 * @return returns a gchar * printed out in a french like format "dd/mm/aaaa - hh:mm:ss"
 * @todo Add a way to use a user defined format and/or predefined formats
 */
static gchar *date_printf(date_and_time_t *mydate)
{
	return g_strdup_printf("%02u/%02u/%04u - %02u:%02u:%02u", mydate->day, mydate->month, mydate->year, mydate->hour, mydate->minutes, mydate->seconds);
}


/**
 * @fn gchar *decode_dos_date(guchar *data, date_and_time_t *mydate)
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a dos date. If it is not, the result
 *  may be funny !
 *  @param data : 4 guchars
 *  @param[out] mydate : date_and_time_t * structure that contain the resulting date
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn void make_date_and_time(date_and_time_t *mydate, guchar *data, guint8 len, guint64 nbticks, guint32 base_year, guint base_month, guint8 base_day)
 *  Reads the data from the stream (specified length !! <= 8 or 64 bits to decode)
 *  @param[out] mydate : date_and_time_t * structure that contain the resulting date
 *  @param data : a guchar * as a stream to decode as a date
 *  @param len : guint8 as length of the stream in bytes (must be <= 8 or 64 bits to decode)
 *  @param nbticks : guint64 that tells the number of ticks per seconds (1, 1000, ...)
 *  @param base_year : guint32, Epoch year (1970, 1904, ...)
 *  @param base_month : guint32, Epoch month (january, ...)
 *  @param base_day : guint32 Epoch day (01, 15, ...)
 *  @return populates the date_and_time_t structure (my_date)
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
 * @fn gchar *decode_filetime_date(guchar *data, date_and_time_t *mydate)
 *  general purpose of this function is to take a 8 byte data stream
 *  and convert it as if it is a filetime date. If it is not, the result
 *  may be funny ! Counting 100th of nanoseconds from 01/01/1601
 *  @param data : 8 guchars
 *  @param[out] mydate : date_and_time_t * structure that contain the resulting date
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_C_date(guchar *data, date_and_time_t *mydate)
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a C date. If it is not, the result may
 *  be funny ! Counting seconds from 01/01/1970
 *  @param data : 4 guchars
 *  @param[out] mydate : date_and_time_t * structure that contain the resulting date
 *  @return returns a gchar * that may be freed when no longer needed
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
 * @fn gchar *decode_HFS_date(guchar *data, date_and_time_t *mydate)
 *  general purpose of this function is to take a 4 byte data stream
 *  and convert it as if it is a HFS date. If it is not, the result may
 *  be funny ! Counting seconds 01/01/1904
 *  @param data : 4 guchars
 *  @param[out] mydate : date_and_time_t * structure that contain the resulting date
 *  @return returns a gchar* that may be freed when no longer needed
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
 * @fn gchar *decode_to_bits(guchar *data)
 *  decodes the stream represented by *data (one byte) to a
 *  string containing eight 0 or 1 (Little Endian style)
 *  @param data : 1 guchar
 *  @return 8 gchars that are either "1" or "0", the string may be freed when
 *          no longer needed
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
 * @fn static void transform_bcd_to_human(gchar *bcd, guint8 part, guint8 part_number)
 *  transcribes the bcd number "part" into a
 *  @param[out] bcd : gchar * human readable string
 *  @param part : guint8 as an half byte to decode
 *  @param part_number : 0 or 1 as MSB and LSB
 *  Coding style is from ETSI GSM 04.08 ETS 300557 p387
 *  @todo give choice of coding style (eg for numbers >=10)
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
 * @fn gchar *decode_packed_BCD(guchar *data)
 *  Decode one byte as a Packed BCD (Binary Coded Decimal)
 *  and return a gchar* that may be freed when no longer
 *  needed
 *  @param data : stream to decode as 1 guchar
 *  @return returns a gchar * that contain the packed BCD interpretation
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
 * @fn gboolean swap_bytes(guchar *to_swap, guint first, guint last)
 *  Swap bytes from the buffer to_swap
 *  @warning recursive function !! Call with first = 0 and last = last byte 
 *  of buffer to swap
 *  @param[in,out] to_swap : buffer to swap
 *  @param first : first byte in the buffer to swap
 *  @param last : last byte in the buffer to swap
 *  @return returns TRUE when first is >= to last and this end recursivity
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
 * @fn reverse_byte_order(guchar *to_reverse)
 *  Reverse the byte order LSB -> MSB in MSB -> LSB
 *  12345678 in 87654321
 * @param[in,out] to_reverse : one guchar to be reversed
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

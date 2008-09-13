/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  decode.c
  Heraia's library decode.c

  (C) Copyright 2008 Sébastien Tricaud, Olivier Delhomme
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

#include <libheraia.h>

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

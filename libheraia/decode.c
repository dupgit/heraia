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


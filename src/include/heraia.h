/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia.h
  heraia - an hexadecimal file editor and analyser based on ghex
 
  (C) Copyright 2005 Olivier Delhomme
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

#ifndef _HERAIA_H_
#define _HERAIA_H_

#define HERAIA_AUTHORS "Olivier Delhomme"
#define HERAIA_DATE "20 02 2005"
#define HERAIA_LICENSE "GPL"
#define HERAIA_VERSION "0.0.1 (20.02.2005)"

typedef struct
{
	char *filename;    /* The filename to open */
	char usage;        /* To know if we displayed the usage message */
} Options;

static struct option const long_options[] =
{
	{"version", no_argument, NULL, 'v'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0} 
};


#endif /* _HERAIA_H_ */

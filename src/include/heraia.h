/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia.h
  heraia - an hexadecimal file editor and analyser based on ghex
 
  (C) Copyright 2005 - 2009 Olivier Delhomme
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
 * @file heraia.h
 * heraia's header file.
 */
#ifndef _HERAIA_H_
#define _HERAIA_H_
 
/**
 * @def HERAIA_DATE 
 * defines heraia's creation date
 *
 * @def HERAIA_AUTHORS
 * defines heraia's main authors
 *
 * @def HERAIA_LICENSE
 * defines heraia's license (a least GPL)
 *
 * @def HERAIA_VERSION
 * defines heraia's current version and release date of this version 
 * (00.00.20XX means a development version)
 */
#define HERAIA_AUTHORS "Olivier Delhomme, Sébastien TRICAUD, Grégory AUDET"
#define HERAIA_DATE "20 02 2005"
#define HERAIA_LICENSE "GPL"
#define HERAIA_VERSION "0.0.7 (00.00.2009)"

/**
 * @struct Options
 * Structure Options gives a way to store program options passed from the
 * command line.
 * - char *filename is the filename to open (should be a list of filenames)
 * - char usage is there to know which kind of usage message we want to display 
 */
typedef struct
{
	char *filename;    /**< the filename to open                      */
	char usage;        /**< to know if we displayed the usage message */
} Options;

static struct option const long_options[] =
{
	{"version", no_argument, NULL, 'v'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0} 
};

extern heraia_window_t *get_main_struct(void);

#endif /* _HERAIA_H_ */

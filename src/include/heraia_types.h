/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  types.h
  types.h - all data types and pieces of common declarations

  (C) Copyright 2006 - 2008 Olivier Delhomme
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

#ifndef _HERAIA_TYPES_H_
#define _HERAIA_TYPES_H_

/* I have some problems under windows if #ifdef statement is activated */

/* #ifdef HAVE_CONFIG_H */
#include "config.h"
/* #endif */  /* HAVE_CONFIG_H */


/**
 *  As there seems to be huge differences between the linux
 *  and the windows developpement platform such as that
 *  the included paths are correctly taken into account
 *  under linux (and thus nobody bothers) but not under
 *  windows, I decided to put all the .h definitions here
 *  even if it's ugly, this is supposed to solve the problem.
 */

#include <libheraia.h>

#include "heraia.h"

#endif /* _HERAIA_TYPES_H_ */

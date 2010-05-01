/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  heraia_errors.h
  heraia - an hexadecimal file editor and analyser based on ghex

  (C) Copyright 2005 - 2007 Sebastien Tricaud  e-mail : toady@gscore.org
  (C) Copyright 2005 - 2010 Olivier Delhomme   e-mail : heraia@delhomme.org

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
 * @file heraia_errors.h
 * Header file for handling errors
 */
#ifndef _HERAIA_ERRORS_H_
#define _HERAIA_ERRORS_H_

/* this is empty for now, until there should be some errors functions */
/**
 * @def HERAIA_NOERR
 *   No error occured, everything is fine
 *
 * @def HERAIA_MEMORY_ERROR
 *   Malloc error, you may get into trouble
 *
 * @def HERAIA_NO_PLUGINS
 *   When no plugin are found
 *
 * @def HERAIA_FILE_ERROR
 *   When an error on a file occurs
 *  @todo We may use errno to have more accurate file errors
 *
 * @def HERAIA_CANCELLED
 *   an operation has been cancelled
 */
#define HERAIA_NOERR 0
#define HERAIA_MEMORY_ERROR 1
#define HERAIA_NO_PLUGINS 2
#define HERAIA_FILE_ERROR 3
#define HERAIA_CANCELLED 4

#endif /* _HERAIA_ERRORS_H_ */

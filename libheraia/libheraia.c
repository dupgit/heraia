/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  libheraia.c
  Heraia's library

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
 * @file libheraia.c
 *
 * Contains function that may be usefull if we want all this beeing
 * integrated in a python script or something similar
 *
 * @todo do the functions that may be called by python when initializing
 *       the library.
 */
#include <stdio.h>
#include <libheraia.h>

#include "config.h"
#include "data_interpretor.h"
#include "decode.h"
#include "ghex_heraia_interface.h"
#include "heraia_errors.h"
#include "heraia_io.h"
#include "heraia_ui.h"
#include "log.h"
#include "main_pref_window.h"
#include "plugin.h"
#include "user_prefs.h"

/**
 * @fn int libheraia_test(void)
 * A simple test function
 * @return Always returns 0
 */

int libheraia_test(void)
{
    fprintf(stdout, Q_("Libheraia library test OK\n"));
    return 0;
}

void libheraia_initialize(void)
{

}

void libheraia_finalize(void)
{

}


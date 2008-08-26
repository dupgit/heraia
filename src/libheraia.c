/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  libheraia.c
  Heraia's library
 
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

#include <stdio.h>

#include "libheraia.h"
#include "heraia_types.h"

int library_test(void)
{
	heraia_window_t *main_struct = NULL;
	
	
	main_struct = get_main_struct();
	fprintf(stdout, "main_struct : %p\n", main_struct);
	
	return 0;
}

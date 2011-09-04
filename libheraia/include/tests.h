/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  tests.h
 *  tests for code coverage and bugs
 *
 *  (C) Copyright 2011 Olivier Delhomme
 *  e-mail : heraia@delhomme.org
 *  URL    : http://heraia.tuxfamily.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or  (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *  MERCHANTABILITY  or  FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
 /**
 * @file tests.h
 * Header file for tests (code coverage and bug tracking)
 */
#ifndef _LIBHERAIA_TESTS_H_
#define _LIBHERAIA_TESTS_H_

extern gboolean test_decode_functions(void);
extern gboolean tests_ui(heraia_struct_t *main_struct);

#endif /* _LIBHERAIA_TESTS_H_ */

#!/bin/sh
#
#  heraia_self_tests.sh
#  some self tests for heraia (coverage tests)
#
#  (C) Copyright 2007 - 2010 Olivier Delhomme
#  e-mail : heraia@delhomme.org
#  URL    : http://heraia.tuxfamily.org
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2, or  (at your option)
#  any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY;  without even the implied warranty of
#  MERCHANTABILITY  or  FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

../src/heraia --tests=2 ../docs/zero_length_file
../src/heraia --tests=2 ../docs/test_file
../src/heraia --tests=2 ../docs/does_not_exists.file
../src/heraia --tests=2 --a_false_param
../src/heraia -v --tests=2
../src/heraia -h --tests=2
../src/heraia --tests=1
../src/heraia ../src/heraia

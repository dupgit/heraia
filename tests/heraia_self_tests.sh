#!/bin/sh
#
#  heraia_self_tests.sh
#  some self tests for heraia (coverage tests)
#
#  (C) Copyright 2010 Olivier Delhomme
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

echo "Heraia testing suite..."

if test -f "$1";
then
    echo "$1";
else
    echo "Usage : ./heraia_self_tests.sh path_to_the_heraia_executable_where_installed";
    echo ""
    exit;
fi

export HERAIA="$1"

echo ""
echo "---> Testing the zero length file"
$HERAIA --tests=2 ../docs/zero_length_file

echo ""
echo "---> Testing a non empty existing file"
$HERAIA --tests=2 ../docs/test_file

echo ""
echo "---> Testing a non existing file"
$HERAIA --tests=2 ../docs/does_not_exists.file

echo ""
echo "---> Testing a false parameter in the options"
$HERAIA --tests=2 --a_false_param

echo ""
echo "---> Testing options"
$HERAIA -v --tests=2
$HERAIA -h --tests=2

echo ""
echo "---> Doing some naive coverage tests"
$HERAIA --tests=1

echo ""
echo "---> Testing loading heraia itself"
$HERAIA ../src/heraia

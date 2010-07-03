#!/bin/sh
#
#  make_devel_docs.sh
#  A simple script to generate devel docs
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

# PATH guessed from heraia.doxygen
DOC_PATH=$(cat heraia.doxygen | grep OUTPUT_DIRECTORY | cut -d'=' -f 2 | sed s/\ //g)

# Cleanning
rm -rf $DOC_PATH

# Invoking doxygen !
doxygen heraia.doxygen


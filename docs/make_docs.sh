#!/bin/bash
#
#  make_docs.sh
#  A simple script to generate documentation from the files from the project
#
#  (C) Copyright 2011 Olivier Delhomme
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

DOC_FILES="README NEWS HISTORY RoadMap TODO ChangeLog"
HTML_DIR="html"
DOC_DIRS="$HTML_DIR"

# Making the directories if necessary (and cleaning them)
for d in $DOC_DIRS; do
    mkdir -p $d
    rm -f $d/*
done;

# Generating html documentation
cd $HTML_DIR;
for f in $DOC_FILES; do
    echo "Making $f to $f.html"
    rst2html -q ../../$f $f.html;
done;
cd ../


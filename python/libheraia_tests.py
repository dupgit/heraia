#!/usr/bin/env python
# -*- encoding: utf8 -*-
#
#  Heraia in python !
#
#  (C) Copyright 2010 Olivier Delhomme
#  e-mail : heraia@delhomme.org
#  URL    : http://heraia.tuxfamily.org
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2, or (at your option)
#  any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#


from ctypes import cdll
from ctypes import Structure, Union, c_ulong, c_int, c_ushort
import pygtk
pygtk.require('2.0')
import gtk


libheraia = cdll.LoadLibrary('libheraia.so')
libheraia.libheraia_test()

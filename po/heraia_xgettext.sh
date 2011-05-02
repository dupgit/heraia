#!/bin/sh
#
#  heraia_xgettext.sh
#  extracts all translatable strings from heraia's code
#
#  (C) Copyright 2010 - 2011 Olivier Delhomme
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

# heraia's sources (from POTFILES)
xgettext -j --default-domain=heraia --add-comments --keyword=Q_:1g --keyword=N_:1g --from-code=UTF-8 --flag=g_strdup_printf:1:c-format --flag=g_string_printf:2:c-format  --flag=g_string_append_printf:2:c-format --flag=g_error_new:3:c-format --flag=g_set_error:4:c-format --flag=g_markup_printf_escaped:1:c-format --flag=g_log:3:c-format --flag=g_print:1:c-format --flag=g_printerr:1:c-format --flag=g_printf:1:c-format --flag=g_fprintf:2:c-format --flag=g_sprintf:2:c-format --flag=g_snprintf:3:c-format --flag=g_scanner_error:2:c-format --flag=g_scanner_warn:2:c-format --files-from=./POTFILES.in

# GtkBuilder's interfaces
xgettext -j --default-domain=heraia --add-comments --from-code=UTF-8 -Lglade ../src/heraia.gtkbuilder
xgettext -j --default-domain=heraia --add-comments --from-code=UTF-8 -Lglade ../plugins/stat/stat.gtkbuilder

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  io.h
  io.h - input and output functions for heraia
 
  (C) Copyright 2005 - 2007 Olivier Delhomme
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
#ifndef _HERAIA_IO_H_
#define _HERAIA_IO_H_

extern gboolean load_file_to_analyse(heraia_window_t *main_window, gchar *filename);
extern GladeXML *load_glade_xml_file(GList *location_list, gchar *filename);
extern gboolean save_preferences_to_file(prefs_t *prefs);
extern gboolean load_preference_file(heraia_window_t *main_window);

#endif /* _HERAIA_IO_H_ */

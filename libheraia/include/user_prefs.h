/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  user_prefs.h
  heraia - an hexadecimal file editor and analyser based on ghex
 
  (C) Copyright 2008 - 2008 Olivier Delhomme
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

#ifndef _LIBHERAIA_USER_PREFS_H_
#define _LIBHERAIA_USER_PREFS_H_

/**
 *  GN == Group Name
 *  KN == Key Name
 *  Thoses variables are the one to use in the main preference file
 */
#define GN_GLOBAL_PREFS "Global Preferences"
#define KN_SAVE_WINDOW_PREFS "Save Window Preferences"
#define KN_ABOUT_BOX "About Box"
#define KN_DATA_INTERPRETOR "Data Interpretor"
#define KN_LOG_BOX "Log Box"
#define KN_MAIN_DIALOG "Main Dialog"
#define KN_PLUGIN_LIST "Plugin List"
#define KN_LDT "List Data Types"
#define KN_MAIN_PREFS "Main Preferences"

extern void verify_preference_file(gchar *pathname, gchar *filename);
extern void save_main_preferences(heraia_window_t *main_window);
extern void setup_preferences(heraia_window_t *main_window);

#endif /* _LIBHERAIA_USER_PREFS_H_ */

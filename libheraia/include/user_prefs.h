/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  user_prefs.h
  heraia - an hexadecimal file editor and analyser based on ghex

  (C) Copyright 2008 - 2011 Olivier Delhomme
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */
/**
 * @file user_prefs.h
 * Header file for user preferences
 */
#ifndef _LIBHERAIA_USER_PREFS_H_
#define _LIBHERAIA_USER_PREFS_H_

/**
 * @note
 *  GN == Group Name
 *  KN == Key Name
 *  Thoses variables are the ones to use in main preference file and projects
 *  Please do not translate them !
 */
#define GN_GLOBAL_PREFS "Global Preferences"
#define GN_DISPLAY_PREFS "Display Preferences"
#define GN_DI_PREFS "Data Interpretor Preferences"
#define GN_MPWP_PREFS "Main Preferences Window Preferences"

#define KN_SAVE_WINDOW_PREFS "Save Window Preferences"
#define KN_ABOUT_BOX "About Box"
#define KN_DATA_INTERPRETOR "Data Interpretor"
#define KN_LOG_BOX "Log Box"
#define KN_MAIN_DIALOG "Main Dialog"
#define KN_PLUGIN_LIST "Plugin List"
#define KN_LDT "List Data Types"
#define KN_MAIN_PREFS "Main Preferences"
#define KN_GOTO_DIALOG "Goto Dialog"
#define KN_RESULT_WINDOW "Result Window"
#define KN_FIND_WINDOW "Find Window"
#define KN_FR_WINDOW "Find and Replace Window"
#define KN_FDFT_WINDOW "Find data from type Window"

#define KN_SAVE_OPENED_FILES_FILENAMES "Opened files filenames"
#define KN_FILES_FILENAMES "Files filenames"
#define KN_FILES_CURSOR_POSITIONS "Files cursor positions"
#define KN_CURRENT_TAB "Main current tab"


#define KN_DISP_THOUSAND "Thousand"
#define KN_DISP_OFFSETS "Offsets"

#define KN_DI_SELECTED_TAB "Selected Tab"
#define KN_DI_STREAM_SIZE "Stream Size"
#define KN_DI_ENDIANNESS "Endianness"

#define KN_MPWP_SELECTED_TAB "Selected Tab"

extern void verify_preference_file(prefs_t *prefs);

extern prefs_t *init_preference_struct(gchar *pathname, gchar *filename);
extern void free_preference_struct(prefs_t *prefs);

extern void save_preferences(heraia_struct_t *main_struct, prefs_t *prefs);
extern void load_preferences(heraia_struct_t *main_struct, prefs_t *prefs);

#endif /* _LIBHERAIA_USER_PREFS_H_ */

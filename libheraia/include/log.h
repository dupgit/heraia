/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  log.h
  log functions for heraia

  (C) Copyright 2006 - 2011 Olivier Delhomme
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
 * @file log.h
 *  Header file for the logging subsystem
 */
#ifndef _HERAIA_LOG_H_
#define _HERAIA_LOG_H_

/**
 * @def HERAIA_LOG_DOMAIN
 *  Defines heraia log domain (the one printed out on the logging window)
 */
#define HERAIA_LOG_DOMAIN "Heraia"

/* Messages */
extern void print_message(const char *format, ...);
extern void log_message(heraia_struct_t *main_struct, GLogLevelFlags log_level, const char *format, ...);
extern void log_window_init_interface(heraia_struct_t *main_struct);
extern void mw_cmi_show_logw_toggle(GtkWidget *widget, gpointer data);

#endif /* _HERAIA_LOG_H_ */

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  log.h
  log functions for heraia
 
  (C) Copyright 2006 Olivier Delhomme
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
#ifndef _HERAIA_LOG_H_
#define _HERAIA_LOG_H_

#define HERAIA_LOG_DOMAIN "Heraia"

/* Messages */
extern void print_message(const char *format, ...);
extern void log_message(heraia_window_t *main_window, GLogLevelFlags log_level, const char *format, ...);
extern void log_window_init_interface(heraia_window_t *main_window);
extern void show_hide_log_window(heraia_window_t *main_window, gboolean show);
extern void mw_cmi_affiche_logw_toggle(GtkWidget *widget, gpointer data);

#endif /* _HERAIA_LOG_H_ */

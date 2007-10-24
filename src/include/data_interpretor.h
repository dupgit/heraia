/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  data_interpretor.h
  heraia - an hexadecimal file editor and analyser based on ghex
 
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


#ifndef _DATA_INTERPRETOR_H_
#define _DATA_INTERPRETOR_H_

/** 
 *  Naming : 
 *   H stands for Heraia
 *   DI stands for Data_Interpretor
 */

#define H_DI_DISPLAYED FALSE  /* By default the Data Interpretor Window is not displayed */

/* Little Endian, Middle Endian and Big Endian in a reverse order ;) */
#define H_DI_BIG_ENDIAN 2
#define H_DI_MIDDLE_ENDIAN 4
#define H_DI_LITTLE_ENDIAN 8

extern void data_interpretor_init_interface(heraia_window_t *main_window);
extern void refresh_data_interpretor_window(GtkWidget *hexwidget, gpointer data);

#endif /* _DATA_INTERPRETOR_H_ */

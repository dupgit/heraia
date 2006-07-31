/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  data_interpretor.h
  heraia - an hexadecimal file editor and analyser based on ghex
 
  (C) Copyright 2005 Olivier Delhomme
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

/* 
Naming : 
  H stands for Heraia
  DI stands for Data_Interpretor
*/

#define H_DI_MAX_ENTRY 32   /* Max characters in an entry box                      */
#define H_DI_H_SPACE 2      /* Horizontal space between widgets                    */
#define H_DI_V_SPACE 2      /* Vertical space between widgets                      */
#define H_DI_BORDER_WIDTH 2 /* Object's border width in containers                 */
#define H_DI_LINES 6        /* Number of lines in the DI window by default         */
#define H_DI_COLUMNS 3      /* Same but for columns                                */
#define H_DI_DISPLAYED TRUE /* By default the Data Interpretor Window is displayed */


/* defaults values to display the data_interpretor window */
#define H_DI_FONT_FAMILY "FreeMono"
#define H_DI_FONT_STYLE "Bold"
#define H_DI_FONT_WIDTH "9"

extern void data_interpret(data_window_t *DW);
extern void refresh_data_window(GtkWidget *hexwidget, gpointer data);

#endif /* _DATA_INTERPRETOR_H_ */

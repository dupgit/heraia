/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  treatments.h
  Treatments that may be applied to make a new data type
 
  (C) Copyright 2007 - 2008 Olivier Delhomme
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/ 

#ifndef _TREATMENTS_H_
#define _TREATMENTS_H_

typedef struct
{
	guint length;   /* length of the field bytes below */
	guchar *bytes;   /* bytes extracted and/or parsed   */
} value_t;

extern value_t *new_value_t(guint length, guchar *bin_data);
extern treatment_t *copy_treatment(treatment_t *tment);
extern treatment_t *find_treatment(GList *tment_list, gchar *tment_name);
extern GList *init_treatments(void);

#endif /* _TREATMENTS_H_ */

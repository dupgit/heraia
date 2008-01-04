/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  treatments.c
  Treatments that may be applied to make a new data type
  
  (C) Copyright 2007 - 2007 Olivier Delhomme
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

#include "heraia_types.h"

/**
 *  General background : 
 *  Each treatment has to provide 4 functions :
 *   - one that will do the treatment (execute it) with the treatment parameters (if any) [do_it]
 *     take a GList of values_t * and return a GList of values_t *
 *   - one that will init the treatment with defaults values (widget creation included, if any) [init]
 *   - one that will delete the treatment, widgets included (if any) [kill] 
 *   - one that will copy the internal structures (widgets will be created by the init function) [copy]
 */

static GList *treatment_reverse_do_it(GList *gchar_list);
static gpointer treatment_reverse_copy(gpointer data);
static void treatment_reverse_init(gpointer data);
static void treatment_reverse_kill(gpointer data);


value_t *new_value_t(guint length, guchar *bin_data)
{
	value_t *a_value = NULL;

	a_value = (value_t*) g_malloc0(sizeof(value_t));
	
	a_value->length = length;

	a_value->bytes = (guchar *) g_malloc0(length * sizeof(guchar));

	memcpy(a_value->bytes, bin_data, length);

	return a_value;
}

/**
 *  Copies the treatment and calls the treatment copy function that
 *  copies the data of the treatment itself
 */
treatment_t *copy_treatment(treatment_t *tment)
{
	treatment_t *new_tment = NULL;

	new_tment = (treatment_t *) g_malloc0(sizeof(treatment_t));

	if (tment != NULL)
		{
			if (tment->name != NULL)
				{
					new_tment->name = g_strdup(tment->name);
				}
			else
				{
					new_tment->name = NULL;
				}
			new_tment->do_it = tment->do_it;
			new_tment->init = tment->init;
			new_tment->kill = tment->kill;
			new_tment->copy = tment->copy;

			if (tment->copy != NULL)
				{
					new_tment->data = tment->copy(tment->data);
				}
    
			return new_tment;
		}
	else 
		{
			return NULL;
		}
}

/**
 *  Finds a treatment, by name, in the treatment list
 *  returns the treatment if found, NULL otherwise
 */
treatment_t *find_treatment(GList *tment_list, gchar *tment_name) 
{
	treatment_t *tment = NULL;

	while (tment_list != NULL)
		{
			tment = (treatment_t *) tment_list->data;
			if (g_ascii_strcasecmp(tment->name, tment_name) == 0)
				{
					return tment;
				}
			else
				{
					tment_list = g_list_next(tment_list);
				}
		}

	return NULL;
}

/**
 *  Here we do init the available treatment list
 *  Should only be called once at init time
 */
GList *init_treatments(void)
{
	treatment_t *new_tment = NULL;
	GList *list = NULL;
  
	/* reverse treatment */
	new_tment = (treatment_t *) g_malloc0(sizeof(treatment_t));
	new_tment->name = g_strdup("reverse");
	new_tment->init = treatment_reverse_init;
	new_tment->do_it = treatment_reverse_do_it;
	new_tment->kill = treatment_reverse_kill;
	new_tment->copy = treatment_reverse_copy;
	list = g_list_prepend(list, new_tment);
 
	return list;
}

/******************* Here begins the treatments definitions *******************/

/**
 *  Treatment initialisation (called at runtime when selected by the user)
 */
static void treatment_reverse_init(gpointer data)
{
	treatment_t *tment = (treatment_t *) data;
	/**
	 *  We may init :
	 *  . structures and widgets (if any) an put all this in 'data'
	 */
  
	/* In this treatment there is no need for any widget nor structures */
	tment->data = NULL;
}

/**
 *  Deletes treatment (itself)
 */
static void treatment_reverse_kill(gpointer data)
{
	treatment_t *tment = (treatment_t *) data;
	/**
	 *  As there is no data nor structure, there is nothing to free !!
	 */
  
	if (tment != NULL)
		{
			g_free(tment);
		}
}

/**
 *  Copy treatment's datas 
 */
static gpointer treatment_reverse_copy(gpointer data)
{
	/* As there is no data, there is nothing to do ! */

	return data;
}

/**
 *  Reverse every lists that contains bytes from the hexwidget
 */
static GList *treatment_reverse_do_it(GList *values_list)
{
	GList *head = NULL;
	value_t *extracted = NULL;
	guchar *reversed = NULL;
	guint i = 0;

	head = values_list;

	while (values_list != NULL)
		{
			/* here we reverse the bytes in the values_t * structure */
			extracted = (value_t *) values_list->data;
			reversed = (guchar *) g_malloc0(sizeof(guchar) * extracted->length);

			for (i=0; i<extracted->length; i++)
				{
					reversed[i] = extracted->bytes[extracted->length-1-i];
				}
			g_free(extracted->bytes);
			extracted->bytes = reversed;

			values_list = g_list_next(values_list);
		}

	return head;
}




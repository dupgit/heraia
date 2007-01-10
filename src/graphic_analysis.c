/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  graphic_analysis.c
  graphic analysis functions
  
  (C) Copyright 2006 - 2007 Olivier Delhomme
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "types.h"
#include "graphic_analysis.h"

static gboolean delete_ga_window_event( GtkWidget *widget, GdkEvent  *event, gpointer data );
static void destroy_ga_window( GtkWidget *widget, GdkEvent  *event, gpointer data );
static void on_graph_analysis_show( GtkWidget *widget, gpointer data );
static void on_graph_analysis_expose_event( GtkWidget *widget,  GdkEventExpose *event, gpointer data );
static void on_analyse_graphique_activate( GtkWidget *widget, gpointer data );

/* Call only Once ! */
void init_graph_analysis(heraia_window_t *main_window)
{
	graph_analysis_t *ga;

	/* Connection des signaux */
	log_message(main_window, G_LOG_LEVEL_INFO, "init_graph_analysis(%p) !", main_window);
	g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "analyse_graphique")), "activate",
					  G_CALLBACK (on_analyse_graphique_activate), main_window);

	g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "graph_analysis")), "show",
					  G_CALLBACK (on_graph_analysis_show), main_window);
			
	g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "graph_analysis")), "expose_event",
					  G_CALLBACK (on_graph_analysis_expose_event), main_window);

	g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "graph_analysis")), "delete_event", 
					  G_CALLBACK (delete_ga_window_event), main_window);

	g_signal_connect (G_OBJECT (glade_xml_get_widget(main_window->xml, "graph_analysis")), "destroy", 
					  G_CALLBACK (destroy_ga_window), main_window);
	log_message(main_window, G_LOG_LEVEL_INFO, "init_graph_analysis(%p) !", main_window);
	/* fin de la connexion des signaux */
    ga = (graph_analysis_t *) g_malloc0(sizeof(graph_analysis_t));
	ga->running = 0;
	main_window->ga = ga;
	log_message(main_window, G_LOG_LEVEL_INFO, "main_window->ga == %p !", main_window->ga);
	/* Connexion de la structure */
}


/**** call back functions :  for the graphic analysis window ****/

void on_analyse_graphique_activate( GtkWidget *widget, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	
	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(main_window->xml, "analyse_graphique"))))
		gtk_widget_show_all(glade_xml_get_widget(main_window->xml, "graph_analysis"));
	else
		gtk_widget_hide_all(glade_xml_get_widget(main_window->xml, "graph_analysis"));
}

gboolean delete_ga_window_event( GtkWidget *widget, GdkEvent  *event, gpointer data )
{	
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name (glade_xml_get_widget(main_window->xml, "analyse_graphique"), "activate");

	return TRUE;
}

void destroy_ga_window( GtkWidget *widget, GdkEvent  *event, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;

	g_signal_emit_by_name (glade_xml_get_widget(main_window->xml, "analyse_graphique"), "activate");
}


/* Called when the graph analysis window is shown */
void on_graph_analysis_show( GtkWidget *widget,  gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	graph_analysis_t *ga = NULL;

	if (main_window != NULL)
		ga = (graph_analysis_t *) main_window->ga;

	if (ga != NULL)
		{
			if (ga->running == 0)
				{
					ga->running = 1;
					do_the_histogram(main_window);
					ga->running = 0;
				}
		}
	log_message(main_window, G_LOG_LEVEL_INFO, "Done !!");
}


/* Called when the graph analysis window is exposed (redrawn) */
void on_graph_analysis_expose_event( GtkWidget *widget, GdkEventExpose *event, gpointer data )
{
	heraia_window_t *main_window = (heraia_window_t *) data;
	graph_analysis_t *ga = NULL;

	if (main_window != NULL)
		ga = (graph_analysis_t *) main_window->ga;
	
	if (ga != NULL)
		{
				if (ga->running == 0)
				{
					ga->running = 1;
					do_the_histogram(main_window);
					ga->running = 0;
				}
		}
}
/**** End of call back functions that handle the  graphic analysis window ****/




void do_the_histogram(heraia_window_t *main_window)
{
	/*
	  guint histo[256];
	guint pos = 0;
	guint size = 0;
	guint c = 0;
	guint max = 0;
	data_window_t *DW = NULL;
	GtkHex *gh = NULL;
	GtkDrawingArea *graph_area = NULL;
	GdkGC* GC = NULL;

	pos = 0;
	while (pos < 255)
		histo[pos++] = 0;
		
	pos = 0;

	if (main_window != NULL)
		{
 
			graph_area = glade_xml_get_widget(main_window->xml, "graph_area");
			GC = gdk_gc_new(graph_area);
		
			DW = main_window->current_DW;
			if (DW != NULL)
				{
					
					gh = GTK_HEX (DW->current_hexwidget); 
  
					if (gh != NULL)
						{
							
							pos = 0;
							size = gh->document->file_size;
	     
							while (pos < size)
								{
									c = (guint) gtk_hex_get_byte(gh, pos);
									pos++;
									histo[c]++;
								}

							pos = 0;
							while (pos < 255)
								{
									fprintf(stdout, "%d ", histo[pos]);
									if (histo[pos] > max)
										max = histo[pos];
									pos++;
								}
							gtk_widget_set_size_request(graph_area, 256, max);	
							pos = 0;
							
							  while (pos < 255)
								{
									gdk_draw_line (graph_area, GC, pos, 0, pos, histo[pos]);
									pos++;
								}
							
						}
				}
		}

*/
}


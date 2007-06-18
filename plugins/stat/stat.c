/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  stat.c
  an heraia plugin to calculate some stats on the opened file
 
  (C) Copyright 2007 Olivier Delhomme
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
#include "stat.h"

/* The functions for the plugin's usage */
static void stat_window_connect_signals(heraia_plugin_t *plugin);
static void statw_close_clicked(GtkWidget *widget, gpointer data);
static void destroy_stat_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void histo_radiobutton_toggled(GtkWidget *widget, gpointer data);
static gboolean delete_stat_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data );
static void realize_some_numerical_stat(heraia_window_t *main_struct, heraia_plugin_t *plugin);
static void init_stats_histos(heraia_plugin_t *plugin);
static void populate_stats_histos(heraia_window_t *main_struct, heraia_plugin_t *plugin);
static guint max_histo_1D(stat_t *extra);
static guint max_histo_2D(stat_t *extra);
static guint init_stats_pixbufs(stat_t *extra);
static void make_pixbufs_from_histos(stat_t *extra);
static void plot_in_pixbuf(GdkPixbuf *pixbuf, gint64 x, gint64 y, guchar red, guchar green, guchar blue, guchar alpha);
static void do_pixbuf_1D_from_histo1D(stat_t *extra, guint max_1D);
static void do_pixbuf_2D_from_histo2D(stat_t *extra, guint max_2D);


/**
 *  Initialisation plugin called when the plugin is loaded (some sort of pre-init)
 */
heraia_plugin_t *heraia_plugin_init(heraia_plugin_t *plugin)
{
	stat_t *extra;

	plugin->state             = PLUGIN_STATE_INITIALIZING;
	plugin->xml = NULL;

  	plugin->info->api_version = API_VERSION;
	plugin->info->type        = PLUGIN_TYPE;
	plugin->info->priority    = HERAIA_PRIORITY_DEFAULT;
	plugin->info->name        = PLUGIN_NAME;
	plugin->info->version     = PLUGIN_VERSION;
	plugin->info->summary     = PLUGIN_SUMMARY;
	plugin->info->description = PLUGIN_DESCRIPTION;
	plugin->info->author      = PLUGIN_AUTHOR;
	plugin->info->homepage    = PLUGIN_HOMEPAGE;

	plugin->init_proc    = init;
	plugin->quit_proc    = quit;
	plugin->run_proc     = run;
	plugin->refresh_proc = refresh;

	plugin->filter->extensions = NULL;
	plugin->filter->import     = NULL;
	plugin->filter->export     = NULL;

	/* add the extra struct to the plugin one */
	extra = (stat_t *) g_malloc0 (sizeof(stat_t));
	plugin->extra = extra;

	return plugin;
}

/* the plugin interface functions */

/**
 *  The real init function of the plugin
 */
void init(heraia_window_t *main_struct)
{
	heraia_plugin_t *plugin = NULL;

	log_message(main_struct, G_LOG_LEVEL_INFO, "Initializing plugin %s", PLUGIN_NAME);
	/* first, know who we are ! */
	plugin = find_plugin_by_name(main_struct->plugins_list, PLUGIN_NAME);
	
	if (plugin != NULL)
		{
			/* load the xml interface */
			log_message(main_struct, G_LOG_LEVEL_INFO, "Plugin from %s found !", plugin->info->author);
			if (load_plugin_glade_xml(main_struct, plugin) == TRUE)
				log_message(main_struct, G_LOG_LEVEL_INFO, "%s xml interface loaded.", plugin->info->name);
			else
				log_message(main_struct, G_LOG_LEVEL_WARNING, "Unable to load %s xml interface.", plugin->info->name);
			
			/* hide the interface */
			gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(plugin->xml, "stat_window")));

			/* connect some signals handlers */
			stat_window_connect_signals(plugin);

		}
}

/**  
 *  Normaly this is called when the plugin is unloaded
 *  One may wait it's entire life for this to be called !! ;)
 */ 
void quit(void)
{
	g_print("Quitting %s\n", PLUGIN_NAME);
}

/**
 *  This function is called via a signal handler when the menu entry is toggled
 */
void run(GtkWidget *widget, gpointer data)
{
	heraia_window_t *main_struct = (heraia_window_t *) data; /* the signal send the pointer to this structure */
	heraia_plugin_t *plugin = NULL;

	/* first, know who we are ! */
	plugin = find_plugin_by_name(main_struct->plugins_list, PLUGIN_NAME);
	
	if (plugin != NULL)
		{
			show_hide_widget(GTK_WIDGET(glade_xml_get_widget(plugin->xml, "stat_window")),
							 gtk_check_menu_item_get_active(plugin->cmi_entry));
			if (gtk_check_menu_item_get_active(plugin->cmi_entry) == TRUE)
				{
					plugin->state = PLUGIN_STATE_RUNNING;
					realize_some_numerical_stat(main_struct, plugin);
				}
			else
				plugin->state = PLUGIN_STATE_NONE;
		}

}

/**
 *  The refresh function Called when a new file is loaded or when the cursor is moved
 *  Here we want to refresh the plugin only if a new file is loaded AND if the plugin
 *  is allready displayed (running)
 */
void refresh(heraia_window_t *main_struct, void *data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;

	if (main_struct != NULL && plugin != NULL)
		{
			if (main_struct->event == HERAIA_REFRESH_NEW_FILE && plugin->state == PLUGIN_STATE_RUNNING)
				{
					plugin->run_proc(NULL, (gpointer) main_struct);
				}
		}
}

/* end of the plugin interface functions */

/**
 *  Usefull functions for the stat plugin
 *  those may be included in an another .c source file ?!
 */

/** 
 *  Signals handlers
 */
static gboolean delete_stat_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data )
{
	statw_close_clicked(widget, data);

	return TRUE;
}

static void destroy_stat_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	statw_close_clicked(widget, data);
}

/* What to do when the window is closed */
static void statw_close_clicked(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;

	if (plugin != NULL)
		{
			show_hide_widget(GTK_WIDGET(glade_xml_get_widget(plugin->xml, "stat_window")), FALSE);
			gtk_check_menu_item_set_active(plugin->cmi_entry, FALSE);
		}
}

/* What to do when the user chooses a 1D or 2D histo */
static void histo_radiobutton_toggled(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;
	
	if (plugin != NULL)
		{
			GtkImage *image = GTK_IMAGE(glade_xml_get_widget(plugin->xml, "histo_image"));
			stat_t *extra = (stat_t *) plugin->extra;

			if (gtk_toggle_button_get_active(glade_xml_get_widget(plugin->xml, "rb_1D")) == TRUE)
				gtk_image_set_from_pixbuf(image, extra->pixbuf_1D);
			else
				if (gtk_toggle_button_get_active(glade_xml_get_widget(plugin->xml, "rb_2D")) == TRUE)
					gtk_image_set_from_pixbuf(image, extra->pixbuf_2D);
		}
}


/**
 *  Connects all the signals to the correct functions
 */
static void stat_window_connect_signals(heraia_plugin_t *plugin)
{
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "stat_window")), "delete_event", 
					 G_CALLBACK(delete_stat_window_event), plugin);

	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "stat_window")), "destroy", 
					 G_CALLBACK(destroy_stat_window), plugin);
	
	/* Close Button */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "statw_close_b")), "clicked", 
					 G_CALLBACK(statw_close_clicked), plugin);
	
	/* RadioButton */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "rb_1D")), "toggled", 
					 G_CALLBACK(histo_radiobutton_toggled), plugin);

	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "rb_2D")), "toggled", 
					 G_CALLBACK(histo_radiobutton_toggled), plugin);

	/* the toogle button is already connected to the run_proc function ! */
}


/**
 *  Do some stats on the selected file (entire file is used)
 */
static void realize_some_numerical_stat(heraia_window_t *main_struct, heraia_plugin_t *plugin)
{
	struct stat *stat_buf;
	gchar buf[42];
	guint i = 0;
	guint j = 0;
	guint nbval1D = 0;
	guint nbval2D = 0;
	stat_t *extra = NULL;
	GtkTextView *textview = GTK_TEXT_VIEW(glade_xml_get_widget(plugin->xml, "statw_textview"));

	if (main_struct->filename != NULL)
		{
			log_message(main_struct, G_LOG_LEVEL_INFO, "Calculating stats on %s", main_struct->filename);

			stat_buf = (struct stat *) g_malloc0 (sizeof(struct stat));
			lstat(main_struct->filename, stat_buf);
			if (S_ISREG(stat_buf->st_mode))
				{
					kill_text_from_textview(textview);
					add_text_to_textview(textview, "Taille du Fichier  : %Ld octets\n", stat_buf->st_size);
					ctime_r(&(stat_buf->st_mtime), buf);
					add_text_to_textview(textview, "Dernière modification interne : %s", buf);
					ctime_r(&(stat_buf->st_atime), buf);
					add_text_to_textview(textview, "Dernier accès au fichier      : %s", buf);
					ctime_r(&(stat_buf->st_ctime), buf);
					add_text_to_textview(textview, "Dernier changement externe    : %s", buf);
					
					populate_stats_histos(main_struct, plugin);

					extra = (stat_t *) plugin->extra;
					for (i=0; i<=255; i++)
						{
							if (extra->histo1D[i] > 0)
								nbval1D++;
							for (j=0; j<=255; j++)
								if (extra->histo2D[i][j] > 0)
									nbval2D++;
						}
					add_text_to_textview(textview, "Nombre d'octets différents : %d\n", nbval1D);
					add_text_to_textview(textview, "Nombre de paires d'octets  : %d\n", nbval2D);

					log_message(main_struct, G_LOG_LEVEL_INFO, "Histos calculated !");
				}		
		}
}


/**
 *  Inits the histograms
 */
static void init_stats_histos(heraia_plugin_t *plugin)
{
	guint i = 0;
	guint j = 0;
	stat_t *extra = NULL;

	/* inits the structures */
	extra = (stat_t *) plugin->extra;
	for (i=0; i<=255; i++)
		{
			extra->histo1D[i] = 0 ;
			for (j=0; j<=255; j++)
				extra->histo2D[i][j] = 0 ;
		}
}




/**
 *  Populates the histograms
 */
static void populate_stats_histos(heraia_window_t *main_struct, heraia_plugin_t *plugin)
{
	GtkHex *gh = GTK_HEX(main_struct->current_DW->current_hexwidget);
	guint64 i = 0;
	guint64 taille = ghex_file_size(gh);
	guchar c1, c2;
	stat_t *extra = (stat_t *) plugin->extra;
	GtkImage *image = GTK_IMAGE(glade_xml_get_widget(plugin->xml, "histo_image"));
	GtkToggleButton *rb_1D = GTK_TOGGLE_BUTTON(glade_xml_get_widget(plugin->xml, "rb_1D"));
	GtkToggleButton *rb_2D = GTK_TOGGLE_BUTTON(glade_xml_get_widget(plugin->xml, "rb_2D"));

	init_stats_histos(plugin);

	while (i < taille)
		{
			c1 = gtk_hex_get_byte(gh, i);
			extra->histo1D[c1]++;
			if (i+1 < taille)
				{
					i++;
					c2 = gtk_hex_get_byte(gh, i);
					extra->histo1D[c2]++;
					extra->histo2D[c1][c2]++;
				}
			i++;
		}

	make_pixbufs_from_histos(extra);

	if (gtk_toggle_button_get_active(rb_1D) == TRUE)
		gtk_image_set_from_pixbuf(image, extra->pixbuf_1D);
	else
		if (gtk_toggle_button_get_active(rb_2D) == TRUE)
			gtk_image_set_from_pixbuf(image, extra->pixbuf_2D);
}


/**
 *  Seeks the histo2D struct to find the maximum value
 */
static guint max_histo_2D(stat_t *extra)
{
	guint i = 0;
	guint j = 0;
	guint max = 0;

	for (i=0; i<=255; i++)
		{
			for (j=0; j<=255; j++)
				{
					if (extra->histo2D[i][j] > max)
						max = extra->histo2D[i][j];
				}
		}

	return max;
}


/**
 *  Seeks the histo1D struct to find the maximum value
 */
static guint max_histo_1D(stat_t *extra)
{
	guint i = 0;
	guint max = 0;

	for (i=0; i<=255; i++)
		{
			if (extra->histo1D[i] > max)
				max = extra->histo1D[i];
		}

	return max;
}


/**
 *  Inits the image buffers
 */
static guint init_stats_pixbufs(stat_t *extra)
{
	guint max_1D = 0;
		
	max_1D = max_histo_1D(extra);

	extra->pixbuf_1D = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 255, (gint) max_1D);
	gdk_pixbuf_fill(extra->pixbuf_1D, 0xFFFFFF00);
	gdk_pixbuf_add_alpha(extra->pixbuf_1D, TRUE, (guchar) 255, (guchar) 255, (guchar) 255);

	extra->pixbuf_2D = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 255, 255);
	gdk_pixbuf_fill(extra->pixbuf_2D, 0xFFFFFF00);
	gdk_pixbuf_add_alpha(extra->pixbuf_2D, TRUE, (guchar) 255, (guchar) 255, (guchar) 255);

	return max_1D;
}


/**
 *  Makes the pixbufs from the histograms values
 */
static void make_pixbufs_from_histos(stat_t *extra)
{
	guint max_1D = 0;
	guint max_2D = 0;

	max_1D = init_stats_pixbufs(extra);
	max_2D = max_histo_2D(extra);
	do_pixbuf_1D_from_histo1D(extra, max_1D);
	do_pixbuf_2D_from_histo2D(extra, max_2D);
}


/**
 *  Prints a pixel in the corresponding pixbuf
 */
static void plot_in_pixbuf(GdkPixbuf *pixbuf, gint64 x, gint64 y, guchar red, guchar green, guchar blue, guchar alpha)
{
  guchar *pixels = NULL;
  guchar *p = NULL;

  pixels = gdk_pixbuf_get_pixels(pixbuf);

  p = pixels + y * gdk_pixbuf_get_rowstride(pixbuf) + x * gdk_pixbuf_get_n_channels(pixbuf);

  p[0] = red;
  p[1] = green;
  p[2] = blue;
  p[3] = alpha;

}

/**
 *  Fills the pixbuf with the corresponding data from the
 *  histo1D struct
 */
static void do_pixbuf_1D_from_histo1D(stat_t *extra, guint max_1D)
{
	guint i = 0;

	for (i=0; i<=255; i++)
		{
			plot_in_pixbuf(extra->pixbuf_1D, i, max_1D - extra->histo1D[i], (guchar) 255, (guchar) 0, (guchar) 0, (guchar) 255);
		}
}


/**
 *  Fills the pixbuf with the corresponding data from the
 *  histo2D struct
 */
static void do_pixbuf_2D_from_histo2D(stat_t *extra, guint max_2D)
{
	guint i = 0;
	guint j = 0;
	guchar red;
	guchar green;
	guchar blue;

	/* A sort of color 'normalization' */
	gdouble correction = (gdouble)255/(gdouble)max_2D ;

	for (i=0; i<=255; i++)
		{
			for (j=0; j<=255; j++)
				{
					if (extra->histo2D[i][j] > 255)
						red = (guchar) 255;
					else
						red = (guchar) extra->histo2D[i][j] * correction;

					if (extra->histo2D[i][j] > 65025)
						green = (guchar) 255;
					else
						green = extra->histo2D[i][j] / 255 * correction;

					if (extra->histo2D[i][j] > 16581375)
						blue = (guchar) 255;
					else
						blue = extra->histo2D[i][j] / 65025 * correction;
				
					plot_in_pixbuf(extra->pixbuf_2D, i, 255-j, red, green, blue, (guchar) 0);

				}
		}
}

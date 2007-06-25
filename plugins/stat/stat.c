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
static void statw_save_as_clicked(GtkWidget *widget, gpointer data);
static gchar *stat_select_file_to_save(void);
static void histo_radiobutton_toggled(GtkWidget *widget, gpointer data);
static gboolean delete_stat_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data );
static void realize_some_numerical_stat(heraia_window_t *main_struct, heraia_plugin_t *plugin);
static void init_stats_histos(heraia_plugin_t *plugin);
static void populate_stats_histos(heraia_window_t *main_struct, heraia_plugin_t *plugin);
static void calc_infos_histo_1D(stat_t *extra);
static void calc_infos_histo_2D(stat_t *extra);
static void init_stats_pixbufs(stat_t *extra);
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
	extra->infos_1D = (histo_infos_t *) g_malloc0 (sizeof(histo_infos_t));
	extra->infos_2D = (histo_infos_t *) g_malloc0 (sizeof(histo_infos_t));

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

/* What to do when the save as button is clicked */
static void statw_save_as_clicked(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;

	if (plugin != NULL)
		{
			GtkImage *image = GTK_IMAGE(glade_xml_get_widget(plugin->xml, "histo_image"));
			GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);
			gchar *filename = NULL;
			GError **error = NULL;

			filename = stat_select_file_to_save();
			gdk_pixbuf_save(pixbuf, filename, "png", error, "compression", "9", NULL);
			
			if (filename != NULL)
				g_free(filename);
		}
}

static gchar *stat_select_file_to_save(void)
{
	GtkFileSelection *file_selector = NULL;
	gint response_id = 0;
	gchar *filename;

	file_selector = GTK_FILE_SELECTION (gtk_file_selection_new ("Entrez le nom du fichier image"));

	/* for the moment we do not want to retrieve multiples selections */
	gtk_file_selection_set_select_multiple(file_selector, FALSE);

	response_id = gtk_dialog_run(GTK_DIALOG (file_selector));

	switch (response_id) 
		{
		case GTK_RESPONSE_OK:
			filename = g_strdup(gtk_file_selection_get_filename (GTK_FILE_SELECTION (file_selector)));
			break;
		case GTK_RESPONSE_CANCEL:
		default:
			filename = NULL;
			break;
		}

	gtk_widget_destroy (GTK_WIDGET(file_selector));

	return filename;
}

/* What to do when the user chooses a 1D or 2D histo */
static void histo_radiobutton_toggled(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;
	
	if (plugin != NULL)
		{
			GtkImage *image = GTK_IMAGE(glade_xml_get_widget(plugin->xml, "histo_image"));
			stat_t *extra = (stat_t *) plugin->extra;

			if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(plugin->xml, "rb_1D"))) == TRUE)
				gtk_image_set_from_pixbuf(image, extra->pixbuf_1D);
			else
				{
					if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(plugin->xml, "rb_2D"))) == TRUE)
						gtk_image_set_from_pixbuf(image, extra->pixbuf_2D);
				}
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

	/* Save As Button */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "statw_save_as")), "clicked", 
					 G_CALLBACK(statw_save_as_clicked), plugin);


	/* the toogle button is already connected to the run_proc function ! */
}


/**
 *  Do some stats on the selected file (entire file is used)
 */
static void realize_some_numerical_stat(heraia_window_t *main_struct, heraia_plugin_t *plugin)
{
	struct stat *stat_buf;
	gchar buf[42];
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
					
					add_text_to_textview(textview, "Nombre d'octets différents : %d\n", extra->infos_1D->nb_val);
					add_text_to_textview(textview, "Nombre de paires d'octets différentes : %d\n", extra->infos_2D->nb_val);

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
 *  Seeks the histo1D struct to find the maximum value 
 */
static void calc_infos_histo_1D(stat_t *extra)
{
	guint i = 0;
	gint64 n = 1;
	guint64 max = 0;
	guint64 min = G_MAXUINT64;
	gint64 mean = extra->histo1D[0];
	gint64 diff = 0;

	for (i=0; i<=255; i++)
		{
			/* maximum value */
			if (extra->histo1D[i] > max)
				max = extra->histo1D[i];

			/* minimum value */
			if (extra->histo1D[i] < min)
				min = extra->histo1D[i];
			
			/* number of different values */
			if (extra->histo1D[i] > 0)
				extra->infos_1D->nb_val++;

			/* mean calculation */
			diff = extra->histo1D[i] - mean;
			mean = mean + diff/n;
			n++;
		}
	extra->infos_1D->min = min;
	extra->infos_1D->max = max;
	extra->infos_1D->mean = (guint64) mean;
}


/**
 *  Seeks the histo2D struct to find the maximum value
 */
static void calc_infos_histo_2D(stat_t *extra)
{
	guint i = 0;
	guint j = 0;
	gint64 n = 1;
	guint64 max = 0;
	guint64 min = G_MAXUINT;
	gint64 mean = extra->histo2D[0][0];
	gint64 diff = 0;

	for (i=0; i<=255; i++)
		{
			for (j=0; j<=255; j++)
				{
					/* maximum value */
					if (extra->histo2D[i][j] > max)
						max = extra->histo2D[i][j];

					/* minimum value */
					if (extra->histo2D[i][j] < min)
						min = extra->histo2D[i][j];

					/* number of different values */
					if (extra->histo2D[i][j] > 0)
						extra->infos_2D->nb_val++;

					/* mean calculation */
					diff = extra->histo1D[i] - mean;
					mean = mean + diff/n;
					n++;
				}
		}
	extra->infos_2D->min = min;
	extra->infos_2D->max = max;
	extra->infos_2D->mean = (guint64) mean;
}


/**
 *  Inits the image buffers
 */
static void init_stats_pixbufs(stat_t *extra)
{
 
	extra->pixbuf_1D = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 255, 255);
	gdk_pixbuf_fill(extra->pixbuf_1D, 0xFFFFFF00);
	gdk_pixbuf_add_alpha(extra->pixbuf_1D, TRUE, (guchar) 255, (guchar) 255, (guchar) 255);

	extra->pixbuf_2D = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 255, 255);
	gdk_pixbuf_fill(extra->pixbuf_2D, 0xFFFFFF00);
	gdk_pixbuf_add_alpha(extra->pixbuf_2D, TRUE, (guchar) 255, (guchar) 255, (guchar) 255);

}


/**
 *  Makes the pixbufs from the histograms values
 */
static void make_pixbufs_from_histos(stat_t *extra)
{
	init_stats_pixbufs(extra);
	calc_infos_histo_1D(extra);
	calc_infos_histo_2D(extra);

	if (extra->infos_1D->max > 0)
		do_pixbuf_1D_from_histo1D(extra, extra->infos_1D->max);
	if (extra->infos_2D->max > 0)
		do_pixbuf_2D_from_histo2D(extra, extra->infos_2D->max);
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
 *  Prints a line of pixels in the corresponding pixbuf (1D histo)
 */
static void line_in_pixbuf(GdkPixbuf *pixbuf, gint64 x, gint64 y)
{
	guchar *pixels = NULL;
	guchar *p = NULL;

	if (pixbuf != NULL)
		{

			gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
			gint n_channels = gdk_pixbuf_get_n_channels(pixbuf);
			
			pixels = gdk_pixbuf_get_pixels(pixbuf);
	
			while (y<255)
				{
					p = pixels + y * rowstride + x * n_channels;
					p[0] = (guchar) 255-(y/2);
					p[1] = (guchar) 16;
					p[2] = (guchar) y/2;
					p[3] = (guchar) 255;
					y++;
				}
		}
}


/**
 *  Fills the pixbuf with the corresponding data from the
 *  histo1D struct
 */
static void do_pixbuf_1D_from_histo1D(stat_t *extra, guint max_1D)
{
	guint i = 0;
	gint64 y = 0;

	for (i=0; i<=255; i++)
		{	
			/* normalisation (here we know that max != 0 (cf make_pixbufs_from_histos) */
			y = (gint64) (extra->infos_1D->max - (extra->histo1D[i]*extra->infos_1D->nb_val) / (extra->infos_1D->max));	
			line_in_pixbuf(extra->pixbuf_1D, i, y);
		}
}


/**
 *  Fills the pixbuf with the corresponding data from the
 *  histo2D struct
 *  It is really hard to make something very visible (to make colors
 *  look really different between to height values)
 */
static void do_pixbuf_2D_from_histo2D(stat_t *extra, guint max_2D)
{
	guint i = 0;
	guint j = 0;
	guchar red;
	guchar green;
	guchar blue;
	gdouble height = 0;
	/* A sort of color 'normalization' */
	
	for (i=0; i<=255; i++)
		{
			for (j=0; j<=255; j++)
				{
					height = (gdouble) (extra->histo2D[i][j]*255) / (gdouble) max_2D ;
					if (height >0)
						{
							red = (guchar)  height;
							green = (guchar) height/2;
							blue = (guchar) 255 - (height);
				
							plot_in_pixbuf(extra->pixbuf_2D, i, 255-j, red, green, blue, (guchar) 255);
						}
				}
		}
}



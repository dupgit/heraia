/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  stat.c
  an heraia plugin to calculate some stats on the opened file
 
  (C) Copyright 2007 - 2009 Olivier Delhomme
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
/**
 * @file stat.c
 * does some basic statistics on the file and displays them
 * in a numerical or graphical way (histograms : 1D and 2D)
 */
#include "stat.h"

/* The functions for the plugin's usage */
static void stat_window_connect_signals(heraia_plugin_t *plugin);

static void statw_close_clicked(GtkWidget *widget, gpointer data);
static void destroy_stat_window(GtkWidget *widget, GdkEvent  *event, gpointer data);

static void statw_save_as_clicked(GtkWidget *widget, gpointer data);
static void statw_export_to_csv_clicked(GtkWidget *widget, gpointer data);
static void statw_export_to_gnuplot_clicked(GtkWidget *widget, gpointer data);
static void statw_export_to_pcv_clicked(GtkWidget *widget, gpointer data);

static gchar *stat_select_file_to_save(gchar *window_text);
static void histo_radiobutton_toggled(GtkWidget *widget, gpointer data);
static gboolean delete_stat_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data );
static void realize_some_numerical_stat(heraia_window_t *main_struct, heraia_plugin_t *plugin);
static void init_stats_histos(heraia_plugin_t *plugin);
static void set_statw_button_state(GladeXML *xml, gboolean sensitive);
static void populate_stats_histos(heraia_window_t *main_struct, heraia_plugin_t *plugin);
static void calc_infos_histo_1D(stat_t *extra);
static void calc_infos_histo_2D(stat_t *extra);
static void init_stats_pixbufs(stat_t *extra);
static void make_pixbufs_from_histos(stat_t *extra);
static void plot_in_pixbuf(GdkPixbuf *pixbuf, gint64 x, gint64 y, guchar red, guchar green, guchar blue, guchar alpha);
static void do_pixbuf_1D_from_histo1D(stat_t *extra);
static void do_pixbuf_2D_from_histo2D(stat_t *extra, guint max_2D);


/**
 *  Initialisation plugin function called when the plugin is loaded
 *  (some sort of pre-init)
 *  @param[in,out] plugin : plugin's structure
 *  @return returns the plugin structure
 */
heraia_plugin_t *heraia_plugin_init(heraia_plugin_t *plugin)
{
	stat_t *extra = NULL;  /**< extra structure specific to this plugin */
	window_prop_t *stat_prop = NULL; /**< window properties */

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

	/* window properties */
	stat_prop = (window_prop_t *) g_malloc0(sizeof(window_prop_t));
	stat_prop->displayed = FALSE; /* by default, it might be anything else */
	stat_prop->x = 0;
	stat_prop->y = 0;
	
	plugin->win_prop = stat_prop;
	
	plugin->extra = extra;
	

	return plugin;
}

/* the plugin interface functions */


/**
 *  The real init function of the plugin (called at init time)
 * @param main_struct : main structure
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
			{
				log_message(main_struct, G_LOG_LEVEL_INFO, "%s xml interface loaded.", plugin->info->name);
			}
			else
			{
				log_message(main_struct, G_LOG_LEVEL_WARNING, "Unable to load %s xml interface.", plugin->info->name);
			}
			
			/* greyed save as button and others */
			set_statw_button_state(plugin->xml, FALSE);
			
			/* shows or hide the interface (hides it at first as all windows shows up) */
			if (plugin->win_prop->displayed == FALSE)
			{
				gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(plugin->xml, "stat_window")));
			}
			else
			{
				gtk_check_menu_item_set_active(plugin->cmi_entry, TRUE);
			}

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
 * @param widget : widget which called the function (unused)
 * @param data : user data for the plugin, here MUST be heraia_window_t * main 
 *        structure
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
							 gtk_check_menu_item_get_active(plugin->cmi_entry), plugin->win_prop);
			if (gtk_check_menu_item_get_active(plugin->cmi_entry) == TRUE)
				{
					plugin->state = PLUGIN_STATE_RUNNING;
					realize_some_numerical_stat(main_struct, plugin);
				}
			else
				{
					plugin->state = PLUGIN_STATE_NONE;
				}
		}
}


/**
 * Sets stat window's button's sensitive property 
 * @param xml : The plugin's xml description
 * @param sensitive : whether the buttons are greyed (FALSE) or not (TRUE)
 */
static void set_statw_button_state(GladeXML *xml, gboolean sensitive)
{
	if (xml != NULL)
	{
		gtk_widget_set_sensitive(glade_xml_get_widget(xml, "statw_save_as"), sensitive);
		gtk_widget_set_sensitive(glade_xml_get_widget(xml, "statw_export_to_csv"), sensitive);
		gtk_widget_set_sensitive(glade_xml_get_widget(xml, "statw_export_to_gnuplot"), sensitive);
	}
}

/**
 *  The refresh function is called when a new file is loaded or when the cursor is moved
 *  Here we want to refresh the plugin only if a new file is loaded AND if the plugin
 *  is already displayed (running)
 * @param main_struct : main structure
 * @param data : user data (the plugin itself) MUST be heraia_plugin_t *plugin
 *        structure
 */
void refresh(heraia_window_t *main_struct, void *data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;

	if (main_struct != NULL && plugin != NULL)
		{
			if (main_struct->event == HERAIA_REFRESH_NEW_FILE) /* && plugin->state == PLUGIN_STATE_RUNNING) */
				{
					set_statw_button_state(plugin->xml, TRUE);
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
 * Closes stat window
 * @param widget : the widget which called this function
 * @param event : the event that issued the signal (unused here)
 * @param data : user data, MUST be heraia_plugin_t *plugin
 * @return resturns always FALSE (does not propagate the signal)
 */
static gboolean delete_stat_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	statw_close_clicked(widget, data);

	return TRUE;
}


/**

 * Closes stat window
 * @param widget : the widget which called this function
 * @param event : the event that issued the signal (unused here)
 * @param data : user data, MUST be heraia_plugin_t *plugin
 */
static void destroy_stat_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
	statw_close_clicked(widget, data);
}


/**
 *  What to do when the window is closed
 * @param widget : the widget which called this function (unused here)
 * @param data : user data, MUST be heraia_plugin_t *plugin
 */
static void statw_close_clicked(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;

	if (plugin != NULL)
		{
			show_hide_widget(GTK_WIDGET(glade_xml_get_widget(plugin->xml, "stat_window")), FALSE, plugin->win_prop);
			gtk_check_menu_item_set_active(plugin->cmi_entry, FALSE);
		}
}


/**
 *  What to do when the save as button is clicked
 * @param widget : the widget which called this function (unused here)
 * @param data : user data, MUST be heraia_plugin_t *plugin
 */
static void statw_save_as_clicked(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;
	GtkImage *image = NULL;
	GdkPixbuf *pixbuf = NULL;
	gchar *filename = NULL;
	GError **error = NULL;

	if (plugin != NULL)
		{
			image = GTK_IMAGE(glade_xml_get_widget(plugin->xml, "histo_image"));
			pixbuf = gtk_image_get_pixbuf(image);
			
			filename = stat_select_file_to_save("Enter filename's to save the image to");
			if (filename != NULL)
			{
				gdk_pixbuf_save(pixbuf, filename, "png", error, "compression", "9", NULL);
				g_free(filename);
			}
		}
}

/**
 *  Selecting the file filename where to save the file
 * @param window_text : text to be displayed in the selection window
 * @todo remember the last directory where we saved stuff
 * @return returns the new filename where to save a file
 */
static gchar *stat_select_file_to_save(gchar *window_text)
{
	/* GtkFileSelection *file_selector = NULL; */
	GtkFileChooser *file_chooser = NULL;
	gint response_id = 0;
	gchar *filename;

	file_chooser = GTK_FILE_CHOOSER(gtk_file_chooser_dialog_new(window_text, NULL,
																GTK_FILE_CHOOSER_ACTION_SAVE, 
																GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
																GTK_STOCK_OPEN, GTK_RESPONSE_OK, 
																NULL));
																
	/* file_selector = GTK_FILE_SELECTION (gtk_file_selection_new(window_text)); */

	/* for the moment we do not want to retrieve multiples selections */
	/* gtk_file_selection_set_select_multiple(file_selector, FALSE); */
	gtk_window_set_modal(GTK_WINDOW(file_chooser), TRUE);
	gtk_file_chooser_set_select_multiple(file_chooser, FALSE);

	/* response_id = gtk_dialog_run(GTK_DIALOG (file_selector)); */
	response_id = gtk_dialog_run(GTK_DIALOG(file_chooser));
	
	switch (response_id) 
		{
		case GTK_RESPONSE_OK:
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
			/* filename = g_strdup(gtk_file_selection_get_filename (GTK_FILE_SELECTION (file_selector))); */
			break;
		case GTK_RESPONSE_CANCEL:
		default:
			filename = NULL;
			break;
		}

	/* gtk_widget_destroy (GTK_WIDGET(file_selector)); */
	
	gtk_widget_destroy(GTK_WIDGET(file_chooser));
	return filename;
}

/**
 * What to do when "export to csv" button is clicked
 * @param widget : the widget which called this function
 * @param data : user data, MUST be heraia_plugin_t *plugin
 */
static void statw_export_to_csv_clicked(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;
	stat_t *extra = NULL;
	gchar *filename = NULL;
	FILE *fp = NULL;
	guint i = 0;
	guint j = 0;
	
	if (plugin != NULL)
		{
			extra = (stat_t *) plugin->extra;
			
			filename = stat_select_file_to_save("Enter filename to export data as CSV to");
			
			if (filename != NULL)
			{
				fp = g_fopen(filename, "w+");
			}
			
			if (fp != NULL && extra != NULL)
			{
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(plugin->xml, "rb_1D"))) == TRUE)
				{ 
					/* 1D display */
					fprintf(fp, "\"Byte\";\"Count\"\n");
					
					for (i=0; i<=255; i++)
					{
						fprintf(fp, "%d;%Ld\n", i, extra->histo1D[i]);
					}
					
				}
				else
				{ 
					/* 2D display */
					fprintf(fp, "\"Byte/Byte\";");
					for (j=0; j<255; j++)
					{
						fprintf(fp, "\"%d\";", j);
					}
					fprintf(fp, "\"%d\"\n", 255);
			
					for (i=0; i<=255; i++)
					{
						fprintf(fp, "\"%d\";", i);
						for (j=0 ; j<255; j++)
						{
							fprintf(fp, "\"%Ld\";", extra->histo2D[i][j]);
						}
						fprintf(fp, "\"%Ld\"\n", extra->histo2D[i][255]);
					}
				}
				fclose(fp);
			}
			if (filename != NULL)
			{
				g_free(filename);
			}
		}
}

/**
 * What to do when "export to gnuplot" button is clicked
 * @param widget : the widget which called this function
 * @param data : user data, MUST be heraia_plugin_t *plugin
 */
static void statw_export_to_gnuplot_clicked(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;
	stat_t *extra = NULL;
	gchar *filename = NULL;
	FILE *fp = NULL;
	guint i = 0;
	guint j = 0;

	if (plugin != NULL)
		{
			extra = (stat_t *) plugin->extra;
			
			filename = stat_select_file_to_save("Enter filename to export data as gnuplot to");
			
			if (filename != NULL)
			{
				fp = g_fopen(filename, "w+");
			}
			
			if (fp != NULL && extra != NULL)
			{
				/* common settings */
				fprintf(fp, "set terminal png transparent nocrop enhanced small size 1280,960\n");
				fprintf(fp, "set output '%s.png'\n", g_path_get_basename(filename));
				fprintf(fp, "set xrange [-10:265]\n");
				fprintf(fp, "set xlabel 'Bytes'\n");
				
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(plugin->xml, "rb_1D"))) == TRUE)
				{ 
					/* 1D display */
					fprintf(fp, "set title 'Classical histogram'\n");  /**< @todo we might add here the name of the file being edited */
					fprintf(fp, "set ylabel 'Count'\n");
					fprintf(fp, "plot '-' title 'Byte count' with impulses\n");
					
					for (i=0; i<=255; i++)
					{
						fprintf(fp, "%Ld\n", extra->histo1D[i]);
					}
					fprintf(fp, "e\n");
				}
				else
				{ 
					/* 2D display */
					fprintf(fp, "set title 'Heatmap histogram'\n");  /**< @todo we might add here the name of the file being edited */
					fprintf(fp, "set bar 1.000000\n");
					fprintf(fp, "set style rectangle back fc lt -3 fillstyle solid 1.00 border -1\n");
					fprintf(fp, "unset key\n");
					fprintf(fp, "set view map\n");
					fprintf(fp, "set yrange [-10:265]\n");
					fprintf(fp, "set ylabel 'Bytes'\n");
					fprintf(fp, "set palette rgbformulae 36, 13, 15\n");
					fprintf(fp, "splot '-' matrix with image\n");
					
					for (i=0; i<=255; i++)
					{
						for (j=0; j<=255; j++)
						{
							fprintf(fp, "%Ld ", extra->histo2D[i][j]);
						}
						fprintf(fp, "\n");
					}

					fprintf(fp, "e\n");
					fprintf(fp, "e\n");	
				}
				fclose(fp);
			}
			if (filename != NULL)
			{
				g_free(filename);
			}
		}
}

/**
 * What to do when "export to pcv" button is clicked
 * @param widget : the widget which called this function
 * @param data : user data, MUST be heraia_plugin_t *plugin
 */
static void statw_export_to_pcv_clicked(GtkWidget *widget, gpointer data)
{
	heraia_plugin_t *plugin = (heraia_plugin_t *) data;
	stat_t *extra = NULL;
	gchar *filename = NULL;
	FILE *fp = NULL;
	guint i = 0;
	guint j = 0;

	if (plugin != NULL)
		{
			extra = (stat_t *) plugin->extra;
			
			filename = stat_select_file_to_save("Enter filename to export data as PCV to");
			
			if (filename != NULL)
			{
				fp = g_fopen(filename, "w+");
			}
			
			if (fp != NULL && extra != NULL)
			{			
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(plugin->xml, "rb_1D"))) == TRUE)
				{ 
					/* 1D display */
					fprintf(fp, "header {\n");
					fprintf(fp, "\theight = \"960\";\n");
					fprintf(fp, "\twidth = \"1280\";\n");
					fprintf(fp, "\ttitle = \"Classical histogram\";\n");
					fprintf(fp, "}\n");
					fprintf(fp, "axes {\n");
					fprintf(fp, "\tinteger b [label=\"Bytes\"];\n");
					fprintf(fp, "\tinteger c [label=\"Byte count\"];\n");
					fprintf(fp, "}\n");
					fprintf(fp, "data {\n");
					
					for (i=0; i<=255; i++)
					{
						fprintf(fp, "\tb=\"%d\", c=\"%Ld\";\n", i, extra->histo1D[i]);
					}
					fprintf(fp, "}\n");
				}
				else
				{ 
					/* 2D display */
					fprintf(fp, "header {\n");
					fprintf(fp, "\theight = \"960\";\n");
					fprintf(fp, "\twidth = \"1280\";\n");
					fprintf(fp, "\ttitle = \"Classical histogram\";\n");
					fprintf(fp, "}\n");
					fprintf(fp, "axes {\n");
					fprintf(fp, "\tinteger a [label=\"Bytes\"];\n");
					fprintf(fp, "\tinteger b [label=\"Bytes\"];\n");
					fprintf(fp, "\tinteger c [label=\"Byte count\"];\n");
					fprintf(fp, "}\n");
					fprintf(fp, "data {\n");
					
					for (i=0; i<=255; i++)
					{
						for (j=0; j<=255; j++)
						{
							fprintf(fp, "\ta=\"%d\", b=\"%d\", c=\"%Ld\";\n", i, j, extra->histo2D[i][j]);
						}
					}
					fprintf(fp, "}\n");
				}
				fclose(fp);
			}
			if (filename != NULL)
			{
				g_free(filename);
			}
		}
}



/**
 *  What to do when the user chooses a 1D or 2D histo 
 * @param widget : the widget which called this function (unused here)
 * @param data : user data, MUST be heraia_plugin_t *plugin
 */
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
 * @param plugin : main plugin structure
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

	/* CVS button */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "statw_export_to_csv")), "clicked", 
					 G_CALLBACK(statw_export_to_csv_clicked), plugin);
	
	/* Gnuplot button */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "statw_export_to_gnuplot")), "clicked", 
					 G_CALLBACK(statw_export_to_gnuplot_clicked), plugin);

	/* Gnuplot button */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(plugin->xml, "statw_export_to_pcv")), "clicked", 
					 G_CALLBACK(statw_export_to_pcv_clicked), plugin);

	/* the toogle button is already connected to the run_proc function ! */
}


/**
 *  Do some stats on the selected file (entire file is used)
 * @param main_struct : main structure from heraia
 * @param plugin : main plugin structure (the plugin itself in fact)
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
					add_text_to_textview(textview, "File size : %Ld bytes\n\n", stat_buf->st_size);
					ctime_r(&(stat_buf->st_mtime), buf);
					add_text_to_textview(textview, "Last intern modification : %s", buf);
					ctime_r(&(stat_buf->st_atime), buf);
					add_text_to_textview(textview, "Last acces to the file   : %s", buf);
					ctime_r(&(stat_buf->st_ctime), buf);
					add_text_to_textview(textview, "Last extern modification : %s", buf);
					
					populate_stats_histos(main_struct, plugin);

					extra = (stat_t *) plugin->extra;
					
					add_text_to_textview(textview, "\n1D histogram statistics :\n");
					add_text_to_textview(textview, "     . minimum          : %lld\n", extra->infos_1D->min);
					add_text_to_textview(textview, "     . maximum          : %lld\n", extra->infos_1D->max);
					add_text_to_textview(textview, "     . mean             : %lld\n", extra->infos_1D->mean);
					add_text_to_textview(textview, "     . number of values : %lld\n", extra->infos_1D->nb_val);
					add_text_to_textview(textview, "\n2D histogram statistics :\n");
					add_text_to_textview(textview, "     . minimum          : %lld\n", extra->infos_2D->min);
					add_text_to_textview(textview, "     . maximum          : %lld\n", extra->infos_2D->max);
					add_text_to_textview(textview, "     . mean             : %lld\n", extra->infos_2D->mean);
					add_text_to_textview(textview, "     . number of values : %lld\n", extra->infos_2D->nb_val);
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

	extra->infos_1D->nb_val = 0;

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

	extra->infos_2D->nb_val = 0;

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
					diff = extra->histo2D[i][j] - mean;
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
		do_pixbuf_1D_from_histo1D(extra);
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
static void do_pixbuf_1D_from_histo1D(stat_t *extra)
{
	guint i = 0;
	gint64 y = 0;
	gdouble inter = 0;
	gdouble y_norm = 0;

	for (i=0; i<=255; i++)
		{	
			/* normalisation (here we know that max != 0 (cf make_pixbufs_from_histos) */
			y_norm = (gdouble) extra->infos_1D->max - (gdouble) extra->histo1D[i];
			inter = (gdouble) (y_norm*255) / (gdouble)(extra->infos_1D->max);
			y = (gint64) inter;	
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
	/* A sort of color 'normalization' */
	guint i = 0;
	guint j = 0;
	guchar red;
	guchar green;
	guchar blue;
	gdouble height = 0;
	gdouble max = 0;
	gdouble min = 0;
	gdouble mean = 0;
	gdouble threshold1 = 0;
	gdouble threshold2 = 0;
	guchar ceill;
	guchar floor;

	max = extra->infos_2D->max;
	min = extra->infos_2D->min;
	mean = extra->infos_2D->mean;
	
	threshold1 = min + (mean - min) / 2;
	threshold2 = mean + (max - mean) / 2;
	
	floor = (guchar) 50;
	ceill = (guchar) 200;
	
	for (i=0; i<=255; i++)
	{
			for (j=0; j<=255; j++)
			{
					height = extra->histo2D[i][j];  /* min .. max */
					
				if (height > 0)
				{
					
					if (height >= min && height <= threshold1)
					{
						red = floor;
						green = floor;
						blue = (guchar) (height - min)*(ceill-floor) / threshold1;
						/*
						 * height = (gdouble) (height*255) / (gdouble) extra->infos_2D->max;
						 * red = (guchar)  height;
						 * green = (guchar) 255 - (height);
						 * blue = (guchar) height/2;
						 */
						plot_in_pixbuf(extra->pixbuf_2D, i, 255-j, red, green, blue, (guchar) 255);
					}
					else if (height > threshold1 && height <= threshold2)
					{
						red = (guchar) floor;
						green = (guchar) (height - threshold1)*(ceill-floor) / (threshold2 - threshold1);
						blue = (guchar) floor; /* ceill - green;*/
						plot_in_pixbuf(extra->pixbuf_2D, i, 255-j, red, green, blue, (guchar) 255);
					}
					else if (height > threshold2 && height <= max)
					{
						red = (guchar) (height - threshold2)*(ceill-floor) / (max - threshold2);
						green = floor; /* ceill - red; */
						blue = floor;
						/*
						 * height = (gdouble) height*255 / (gdouble) extra->infos_2D->max;
						 * red = (guchar)  255 - (height);
						 * green = (guchar) height/2;
						 * blue = (guchar) height;
						 */
						plot_in_pixbuf(extra->pixbuf_2D, i, 255-j, red, green, blue, (guchar) 255);
					}
				}
			}
	}
}



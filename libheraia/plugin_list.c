/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  plugin_list.c
 *  manage the plugin_list_window window
 *
 *  (C) Copyright 2007 - 2010 Olivier Delhomme
 *  e-mail : heraia@delhomme.org
 *  URL    : http://heraia.tuxfamily.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or  (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *  MERCHANTABILITY  or  FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/**
 * @file plugin_list.c
 * This file manage plugin list window's behavior
 */
#include <libheraia.h>

static gboolean delete_plw_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void destroy_plw_window(GtkWidget *widget, GdkEvent  *event, gpointer data);
static void plw_close_clicked(GtkWidget *widget, gpointer data);
static void plw_refresh_clicked(GtkWidget *widget, gpointer data);
static void mw_cmi_plw_toggle(GtkWidget *widget, gpointer data);
static void pn_treeview_selection_changed_cb(GtkTreeSelection *selection, gpointer data);
static void pn_treeview_double_click(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data);

static void plugin_list_window_connect_signals(heraia_struct_t *main_struct);

static void init_plugin_name_tv(heraia_struct_t *main_struct);
static void print_plugin_basics(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_interface(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_filter_structure(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_functions(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_info_structure(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_extra_structure(GtkTextView *textview, heraia_plugin_t *plugin);
static void print_plugin_state(GtkTextView *textview, heraia_plugin_t *plugin);


/*************** call back function for the plugins_window ********************/
/**
 * @fn gboolean delete_plw_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
 *  Signal handler called when the user closes the window
 * @param widget : calling widget
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 * @return Always returns TRUE in order to propagate the signal
 */
static gboolean delete_plw_window_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    plw_close_clicked(widget, data);

    return TRUE;
}


/**
 * @fn void destroy_plw_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
 * When the window is destroyed (Gtk's doc says that we may never get there)
 * @param widget : calling widget
 * @param event : event associated (may be NULL as we don't use this here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void destroy_plw_window(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
    plw_close_clicked(widget, data);
}


/**
 * @fn void plw_close_clicked(GtkWidget *widget, gpointer data)
 *  Closing the window effectively
 * @param widget : calling widget
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void plw_close_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkCheckMenuItem *cmi = GTK_CHECK_MENU_ITEM(heraia_get_widget(main_struct->xmls->main, "mw_cmi_plugin_list"));

    record_and_hide_dialog_box(GTK_WIDGET(heraia_get_widget(main_struct->xmls->main, "plugin_list_window")), main_struct->win_prop->plugin_list);
    gtk_check_menu_item_set_active(cmi, FALSE);
}


/**
 * @fn void plw_refresh_clicked(GtkWidget *widget, gpointer data)
 *  Refreshing the window effectively
 * @param widget : calling widget
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void plw_refresh_clicked(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkTextView *textview = GTK_TEXT_VIEW(heraia_get_widget(main_struct->xmls->main, "plugin_info_textview"));

    init_plugin_name_tv(main_struct);
    kill_text_from_textview(textview);
}


/**
 * @fn void mw_cmi_plw_toggle(GtkWidget *widget, gpointer data)
 *  When the toggle button 'Liste des plugins' is toggled !
 * @param widget : calling widget (may be NULL as we do not even bother here)
 * @param data : MUST be heraia_struct_t *main_struct main structure and not NULL
 */
static void mw_cmi_plw_toggle(GtkWidget *widget, gpointer data)
{
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    GtkCheckMenuItem *cmi = GTK_CHECK_MENU_ITEM(heraia_get_widget(main_struct->xmls->main, "mw_cmi_plugin_list"));
    GtkPaned *paned = GTK_PANED(heraia_get_widget(main_struct->xmls->main, "plw_hpaned"));
    gint pos = 0;

    if (gtk_check_menu_item_get_active(cmi) == TRUE)
        {
            pos = gtk_paned_get_position(paned);
            if (pos < 15)
                {
                    pos = 100;
                    gtk_paned_set_position(paned, pos);
                }
            move_and_show_dialog_box(heraia_get_widget(main_struct->xmls->main, "plugin_list_window"), main_struct->win_prop->plugin_list);
        }
    else
        {
            record_and_hide_dialog_box(GTK_WIDGET(heraia_get_widget(main_struct->xmls->main, "plugin_list_window")), main_struct->win_prop->plugin_list);
        }
}


/**
 * @fn void print_plugin_info_structure(GtkTextView *textview, heraia_plugin_t *plugin)
 *  adds to the textview the relevant informations about the plugin info structure !
 * @param textview : the specified textview (the one in the window)
 * @param plugin : the plugin we want to print information on
 */
static void print_plugin_info_structure(GtkTextView *textview, heraia_plugin_t *plugin)
{
    if (plugin->info != NULL)
        {
            switch (plugin->info->type)
                {
                    case HERAIA_PLUGIN_UNKNOWN:
                        add_text_to_textview(textview, Q_("Type        : Unknown\n"));
                        break;

                    case HERAIA_PLUGIN_FILTER:
                        add_text_to_textview(textview, Q_("Type        : Filter\n"));
                        break;

                    case HERAIA_PLUGIN_ACTION:
                        add_text_to_textview(textview, Q_("Type        : Action\n"));
                        break;

                    default:
                        add_text_to_textview(textview, Q_("Type        : Unknown\n"));
                }

            add_text_to_textview(textview, Q_("Priority    : %d\n"), plugin->info->priority);
            add_text_to_textview(textview, Q_("Id          : %d\n"), plugin->info->id);

            if (plugin->info->name != NULL)
                {
                    add_text_to_textview(textview, Q_("Name        : %s\n"), plugin->info->name);
                }

            if (plugin->info->version != NULL)
                {
                    add_text_to_textview(textview, Q_("Version     : %s\n"), plugin->info->version);
                }

            if (plugin->info->summary != NULL)
                {
                    add_text_to_textview(textview, Q_("Summary     : %s\n"), plugin->info->summary);
                }

            if (plugin->info->description != NULL)
                {
                    add_text_to_textview(textview, Q_("Description : %s\n"), plugin->info->description);
                }

            if (plugin->info->author != NULL)
                {
                    add_text_to_textview(textview, Q_("Author      : %s\n"), plugin->info->author);
                }

            if (plugin->info->homepage != NULL)
                {
                    add_text_to_textview(textview, Q_("Web site    : %s\n"), plugin->info->homepage);
                }
        }
    else
        {
            add_text_to_textview(textview, Q_("The 'info' structure is not initialized !\n"));
        }
}


/**
 * @fn void print_plugin_functions(GtkTextView *textview, heraia_plugin_t *plugin)
 *  adds to the textview the relevant informations about the plugin functions !
 * @param textview : the specified textview (the one in the window)
 * @param plugin : the plugin we want to print information on
 */
static void print_plugin_functions(GtkTextView *textview, heraia_plugin_t *plugin)
{

    if (plugin->init_proc != NULL ||
        plugin->run_proc != NULL  ||
        plugin->quit_proc != NULL ||
        plugin->refresh_proc != NULL)
        {
            add_text_to_textview(textview, Q_("\nPlugin's defined functions :\n"));

            if (plugin->init_proc != NULL)
                {
                    add_text_to_textview(textview, Q_("    - Initialization function : %p\n"), plugin->init_proc);
                }

            if (plugin->run_proc != NULL)
                {
                    add_text_to_textview(textview, Q_("    - Main function           : %p\n"), plugin->run_proc);
                }

            if (plugin->quit_proc != NULL)
                {
                    add_text_to_textview(textview, Q_("    - Exit function           : %p\n"), plugin->quit_proc);
                }

            if (plugin->refresh_proc != NULL)
                {
                    add_text_to_textview(textview, Q_("    - Refresh function        : %p\n"), plugin->refresh_proc);
                }
        }
    else
        {
            add_text_to_textview(textview, Q_("\nThis plugin does not provide any function !!\n"));
        }
}


/**
 * @fn void print_plugin_filter_structure(GtkTextView *textview, heraia_plugin_t *plugin)
 *  adds to the textview the relevant informations about the plugin filter structure !
 * @param textview : the specified textview (the one in the window)
 * @param plugin : the plugin we want to print information on
 */
static void print_plugin_filter_structure(GtkTextView *textview, heraia_plugin_t *plugin)
{
    if (plugin->filter != NULL)
        {
            if (plugin->filter->import != NULL ||
                plugin->filter->export != NULL)
                {
                    add_text_to_textview(textview, Q_("\nFilter functions :\n"));

                    if (plugin->filter->import != NULL)
                        {
                            add_text_to_textview(textview, Q_("    - Import function : %p\n"), plugin->filter->import);
                        }

                    if (plugin->filter->export != NULL)
                        {
                            add_text_to_textview(textview, Q_("    - Export function : %p\n"), plugin->filter->export);
                        }
                }
            else
                {
                    add_text_to_textview(textview, Q_("\nThis plugin does not provide any filter function\n"));
                }
        }
    else
        {
            add_text_to_textview(textview, Q_("\nThe structure 'filter' is not initialized !"));
        }
}


/**
 * @fn void print_plugin_interface(GtkTextView *textview, heraia_plugin_t *plugin)
 *  adds to the textview the relevant informations about the plugin interface (xml) !
 * @param textview : the specified textview (the one in the window)
 * @param plugin : the plugin we want to print information on
 */
static void print_plugin_interface(GtkTextView *textview, heraia_plugin_t *plugin)
{
    add_text_to_textview(textview, Q_("\nThis plugin provides :\n"));

    if (plugin->cmi_entry != NULL)
        {
            add_text_to_textview(textview, Q_("  - a menu entry in the plugins menu.\n"));
        }
    else
        {
            add_text_to_textview(textview, Q_("  - no menu entry.\n"));
        }

    if (plugin->xml != NULL)
        {
            add_text_to_textview(textview, Q_("  - an xml interface.\n"));
        }
    else
        {
            add_text_to_textview(textview, Q_("  - no xml interface.\n"));
        }
}


/**
 * @fn void print_plugin_basics(GtkTextView *textview, heraia_plugin_t *plugin)
 *  adds to the textview the relevant informations about the plugin basics !
 * @param textview : the specified textview (the one in the window)
 * @param plugin : the plugin we want to print information on
 */
static void print_plugin_basics(GtkTextView *textview, heraia_plugin_t *plugin)
{
    if (plugin->info != NULL)
        {
            add_text_to_textview(textview, Q_("API version : %d\n"), plugin->info->api_version);
        }

    if (plugin->filename != NULL)
        {
            add_text_to_textview(textview, Q_("File        : %s\n"), plugin->filename);
        }

    if (plugin->path != NULL)
        {
            add_text_to_textview(textview, Q_("Directory  : %s\n"), plugin->path);
        }

    if (plugin->handle != NULL)
        {
            add_text_to_textview(textview, Q_("Handle      : %p\n"), plugin->handle);
        }
    else
        {
            add_text_to_textview(textview, Q_("Handle      : NONE <-- Is there anything normal ?\n"));
        }
}


/**
 * @fn void print_plugin_extra_structure(GtkTextView *textview, heraia_plugin_t *plugin)
 *  adds to the textview the relevant informations about the plugin extra structure !
 * @param textview : the specified textview (the one in the window)
 * @param plugin : the plugin we want to print information on
 */
static void print_plugin_extra_structure(GtkTextView *textview, heraia_plugin_t *plugin)
{

    if (plugin->extra != NULL)
        {
            add_text_to_textview(textview, Q_("\nThis plugin has an additionnal 'extra' structure (%p) sized %d bytes.\n"),
                                 plugin->extra, sizeof(*(plugin->extra)));
        }
    else
        {
            add_text_to_textview(textview, Q_("\nThis plugin does not have any additionnal structure.\n"));
        }

}


/**
 * @fn void print_plugin_state(GtkTextView *textview, heraia_plugin_t *plugin)
 *  adds to the textview the relevant informations about the plugin state !
 * @param textview : the specified textview (the one in the window)
 * @param plugin : the plugin we want to print information on
 */
static void print_plugin_state(GtkTextView *textview, heraia_plugin_t *plugin)
{

    add_text_to_textview(textview, Q_("Plugin's state : "));
    switch (plugin->state)
        {
            case PLUGIN_STATE_RUNNING:
                add_text_to_textview(textview, Q_("Running\n"));
                break;

            case PLUGIN_STATE_INITIALIZING:
                add_text_to_textview(textview, Q_("Initialiazing or initialized\n"));
                break;
            case PLUGIN_STATE_LOADED:
                add_text_to_textview(textview, Q_("Loaded\n"));
                break;

            case PLUGIN_STATE_NEW:
                add_text_to_textview(textview, Q_("Creating itself\n"));
                break;

            case PLUGIN_STATE_EXITING:
                add_text_to_textview(textview, Q_("Exiting\n"));
                break;

            case PLUGIN_STATE_NONE:
                add_text_to_textview(textview, Q_("Waiting\n"));
                break;

            default:
                add_text_to_textview(textview, Q_("Unknown\n"));
        }
}


/**
 * @fn void pn_treeview_selection_changed_cb(GtkTreeSelection *selection, gpointer data)
 *  Function called when the selection changes in the treeview
 *  Displays informations about the selected plugin
 * @param selection : user selection in the treeview
 * @param data : MUST be heraia_struct_t *main_struct main structure (must not be NULL)
 */
static void pn_treeview_selection_changed_cb(GtkTreeSelection *selection, gpointer data)
{
    GtkTreeIter iter;
    GtkTreeModel *model = NULL;
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    gchar *name = NULL;
    heraia_plugin_t *plugin = NULL;
    GtkTextView *textview = GTK_TEXT_VIEW(heraia_get_widget(main_struct->xmls->main, "plugin_info_textview"));

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
        {
            gtk_tree_model_get(model, &iter, PNTV_COLUMN_NAME, &name, -1);
            plugin = find_plugin_by_name(main_struct->plugins_list, name);

            if (plugin != NULL)
                {
                    kill_text_from_textview(textview);

                    print_plugin_basics(textview, plugin);

                    print_plugin_info_structure(textview, plugin);

                    print_plugin_functions(textview, plugin);

                    print_plugin_filter_structure(textview, plugin);

                    print_plugin_interface(textview, plugin);

                    print_plugin_extra_structure(textview, plugin);

                    print_plugin_state(textview, plugin);
                }
        }
}


/**
 * In case of a double click on a plugin name in the treeview
 * @param treeview : concerned treeview's widget
 * @param path : Something related to a GtkTreePath ! (not used here)
 * @param col : concerned column (not used here)
 * @param data : user data MUST be a pointer to the heraia_struct_t * structure
 */
static void pn_treeview_double_click(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *plugin_name;
    heraia_struct_t *main_struct = (heraia_struct_t *) data;
    heraia_plugin_t *plugin = NULL;
    gboolean active = FALSE;


    model = gtk_tree_view_get_model(treeview);

    if (gtk_tree_model_get_iter(model, &iter, path))
        {
            gtk_tree_model_get(model, &iter, PNTV_COLUMN_NAME, &plugin_name, -1);
            plugin = find_plugin_by_name(main_struct->plugins_list, plugin_name);

            if (plugin != NULL)
                {
                    active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(plugin->cmi_entry));

                    if (active == FALSE)
                        {
                            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(plugin->cmi_entry), TRUE);
                            plugin->run_proc(GTK_WIDGET(treeview), main_struct);
                        }
                }
        }
}
/********** End of callback functions that handle the plugins window **********/


/**
 * @fn void plugin_list_window_connect_signals(heraia_struct_t *main_struct)
 *  Connecting all signals to the right functions
 * @param main_struct : main structure
 */
static void plugin_list_window_connect_signals(heraia_struct_t *main_struct)
{
    GtkTreeSelection *select = NULL;

    if (main_struct != NULL)
        {
            /* When the plugin list window is destroyed or killed */
            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "plugin_list_window")), "delete_event",
                             G_CALLBACK(delete_plw_window_event), main_struct);

            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "plugin_list_window")), "destroy",
                             G_CALLBACK(destroy_plw_window), main_struct);

            /* Close button */
            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "plw_close_b")), "clicked",
                             G_CALLBACK(plw_close_clicked), main_struct);

            /* The toogle button */
            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "mw_cmi_plugin_list")), "toggled",
                             G_CALLBACK(mw_cmi_plw_toggle), main_struct);

            /* Selection has changed for the pn_treeview */
            select = gtk_tree_view_get_selection(GTK_TREE_VIEW(heraia_get_widget(main_struct->xmls->main, "pn_treeview")));
            gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
            g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK (pn_treeview_selection_changed_cb), main_struct);

            /* Double Click */
            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "pn_treeview")), "row-activated",
                             G_CALLBACK(pn_treeview_double_click), main_struct);

            /* Refresh button */
            g_signal_connect(G_OBJECT(heraia_get_widget(main_struct->xmls->main, "plw_refresh_b")), "clicked",
                             G_CALLBACK(plw_refresh_clicked), main_struct);
        }
}


/**
 * @fn void init_plugin_name_tv(heraia_struct_t *main_struct)
 *  Function to init the first treeview (plugin names)
 * @param main_struct : main structure
 */
static void init_plugin_name_tv(heraia_struct_t *main_struct)
{
    GtkListStore *list_store = NULL;  /**< Treeview Stuff for rendering */
    GtkTreeIter iter;                 /**< the text in it.              */
    GtkCellRenderer *renderer = NULL;

    GtkTreeViewColumn *column = NULL;
    heraia_plugin_t *plugin = NULL;   /**< plugin interface structure   */
    GList *p_list = NULL;             /**< plugin list                  */
    GtkTreeView *treeview = NULL;     /**< Treeview where plugin names are to be displayed */

    if (main_struct != NULL)
        {
            treeview = GTK_TREE_VIEW(heraia_get_widget(main_struct->xmls->main, "pn_treeview"));

            p_list = g_list_first(main_struct->plugins_list);

            list_store = gtk_list_store_new(PNTV_N_COLUMNS, G_TYPE_STRING);

            while (p_list != NULL)
                {
                    plugin = (heraia_plugin_t *) p_list->data;
                    log_message(main_struct, G_LOG_LEVEL_INFO, "%s", plugin->info->name);

                    gtk_list_store_append(list_store, &iter);
                    gtk_list_store_set(list_store, &iter, PNTV_COLUMN_NAME, plugin->info->name, -1);

                    p_list = p_list->next;
                }

            gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(list_store));

            column = gtk_tree_view_get_column(treeview, PNTV_COLUMN_NAME);

            if (column != NULL)
                {
                    gtk_tree_view_remove_column(treeview, column);
                }

            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", PNTV_COLUMN_NAME, NULL);
            gtk_tree_view_append_column(treeview, column);
        }
}


/**
 * @fn plugin_list_window_init_interface(heraia_struct_t *main_struct)
 *  the function to init the plugin_list_window interface
 * @param main_struct : main structure
 */
void plugin_list_window_init_interface(heraia_struct_t *main_struct)
{

    plugin_list_window_connect_signals(main_struct);

    init_plugin_name_tv(main_struct);
}







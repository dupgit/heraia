/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
  ghex_heraia_interface.h
  heraia - an hexadecimal file editor and analyser based on ghex

  (C) Copyright 2005 - 2011 Olivier Delhomme
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */
/**
 * @file ghex_heraia_interface.h
 * Header file providing an interface to Heraia_Hex
 */
#ifndef _GHEX_HERAIA_INTERFACE_H_
#define _GHEX_HERAIA_INTERFACE_H_

extern gboolean ghex_memcpy(Heraia_Hex *gh, guint64 pos, guint len, guint endianness, guchar *result);
extern gboolean ghex_get_data(GtkWidget *hex_widget, guint length, guint endianness, guchar *c);
extern void ghex_set_data(doc_t *doc, guint64 position, guint rep_len, guint len, guchar *data);

extern gboolean ghex_get_data_position(GtkWidget *hex_widget, guint64 pos, guint length, guint endianness, guchar *c);

extern guchar *ghex_get_data_to_ascii(GtkWidget *hex_widget, guint64 pos, guint length, guint endianness);
extern guchar *ghex_get_data_to_hex(GtkWidget *hex_widget, guint64 pos, guint length, guint endianness);

extern gint ghex_compare_data(doc_t *doc, guchar *string,  guint buffer_size, guint64 position);

extern doc_t *heraia_hex_document_new(heraia_struct_t *main_struct, char *filename);
extern HERAIA_ERROR heraia_hex_document_save(doc_t *current_doc);
extern HERAIA_ERROR heraia_hex_document_save_as(doc_t *current_doc, gchar *filename);

extern gchar *heraia_hex_document_get_filename(Heraia_Document *hex_doc);
extern gchar *doc_t_document_get_filename(doc_t *doc);

extern guint64 ghex_file_size(Heraia_Hex *gh);

extern guint64 ghex_get_cursor_position(GtkWidget *hex_widget);
extern void ghex_set_cursor_position(GtkWidget *hex_widget, guint64 position);

extern gboolean ghex_find_forward(doc_t *doc, guchar *search_buffer, guint buffer_size, guint64 *position);
extern gboolean ghex_find_backward(doc_t *doc, guchar *search_buffer, guint buffer_size, guint64 *position);

extern selection_t *ghex_get_selection(GtkWidget *hex_widget);

extern doc_t *new_doc_t(Heraia_Document *hex_doc, GtkWidget *hex_widget);
extern void close_doc_t(doc_t *current_doc);

gboolean ghex_find_decode(gint direction, doc_t *doc, DecodeFunc decode_it, decode_parameters_t *decode_parameters, guint data_size, gchar *search_buffer, guint64 *position);


/* From libgtkhex */
#define is_displayable(c) (((c) >= 0x20) && ((c) < 0x7f))

#endif /* _GHEX_HERAIA_INTERFACE_H_ */

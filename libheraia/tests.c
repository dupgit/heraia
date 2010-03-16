/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 *  tests.c
 *  tests for code coverage and bugs
 *
 *  (C) Copyright 2010 Olivier Delhomme
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
 * @file tests.c
 * tests (code coverage and bug tracking)
 */
#include <libheraia.h>

static gboolean test_one_function(DecodeFunc a_function, gchar *function_name, guchar *data, gpointer data_struct, gchar *expected_result);

/**
 *  test one decoding function from the decode.c module
 *  @param a_function : a decoding function that responds to the DecodeFunc template
 *  @param function_name : a gchar * representing the name of the function
 *  @param data : the data to be decoded
 *  @param data_struct : a structure related to the decoding function that may
 *                       help to decode data correctly
 *  @param expected_result : the result that may give the decoding function with
 *                           those specific parameters
 */
static gboolean test_one_function(DecodeFunc a_function, gchar *function_name, guchar *data, gpointer data_struct, gchar *expected_result)
{
    gchar *ret_code = NULL;

    ret_code = a_function(data, data_struct);

    if (g_strcmp0(ret_code, expected_result) == 0)
        {
            return TRUE;
        }
    else
        {
            fprintf(stderr, "WARNING : expected result was not good with %s\n", function_name);
            return FALSE;
        }
}


/**
 *  test decode functions
 *  No parameters
 */
gboolean test_decode_functions(void)
{

    gboolean exit_value = TRUE;

    exit_value = exit_value && test_one_function(decode_8bits_signed, "decode_8bits_signed", NULL, NULL, NULL);
    exit_value = exit_value && test_one_function(decode_8bits_unsigned, "decode_8bits_unsigned", NULL, NULL, NULL);

    exit_value = exit_value && test_one_function(decode_16bits_signed, "decode_16bits_signed", NULL, NULL, NULL);
    exit_value = exit_value && test_one_function(decode_16bits_unsigned, "decode_16bits_unsigned", NULL, NULL, NULL);

    exit_value = exit_value && test_one_function(decode_32bits_signed, "decode_32bits_signed", NULL, NULL, NULL);
    exit_value = exit_value && test_one_function(decode_32bits_unsigned, "decode_32bits_unsigned", NULL, NULL, NULL);

    exit_value = exit_value && test_one_function(decode_64bits_signed, "decode_64bits_signed", NULL, NULL, NULL);
    exit_value = exit_value && test_one_function(decode_64bits_unsigned, "decode_64bits_unsigned", NULL, NULL, NULL);

    exit_value = exit_value && test_one_function(decode_float_normal, "decode_float_normal", NULL, NULL, NULL);
    exit_value = exit_value && test_one_function(decode_float_scientific, "decode_float_scientific", NULL, NULL, NULL);

    exit_value = exit_value && test_one_function(decode_double_normal, "decode_double_normal", NULL, NULL, NULL);
    exit_value = exit_value && test_one_function(decode_double_scientific, "decode_double_scientific", NULL, NULL, NULL);

    return exit_value;
}

/**
 * functions to tests heraia's UI
 * @param main_struct : the main structure (the whole one)
 */
gboolean tests_ui(heraia_struct_t main_struct)
{




}

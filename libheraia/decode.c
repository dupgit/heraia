#include <libheraia.h>

/**
 *  general purpose of this function is to take a 1 byte data stream
 *  and convert it as if it is an 8 bits unsigned number
 *  data : 1 guchar
 *  returns a gchar* that may be freed when no longer needed
*/
gchar *decode_8bits_unsigned(guchar *data)
{
	guint8 total = 0;

	if (data == NULL)
		{
			return NULL;
		}
	else
		{
			memcpy(&total, data, sizeof (guchar));
			return g_strdup_printf("%u", total);
		}
}



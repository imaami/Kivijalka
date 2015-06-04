/** \file img_data.c
 *
 * Image manipulation.
 */

#define _GNU_SOURCE

#include "img_data.h"
#include "read-file.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

size_t
img_data_size (img_data_t *imd)
{
	return (imd) ? sizeof (size_t) + imd->size : 0;
}

img_data_t *
img_data_alloc (size_t size)
{
	img_data_t *data;
	if ((data = malloc (sizeof (size_t) + size))) {
		data->size = size;
	} else {
		fprintf (stderr, "%s: malloc failed: %s\n",
		                 __func__, strerror (errno));
	}
	return data;
}

void
img_data_free (img_data_t *imd)
{
	size_t size;
	if ((size = img_data_size (imd))) {
		(void) memset ((void *) imd, 0, size);
		free (imd);
		imd = NULL;
	}
}

img_data_t *
img_data_new_from_buffer (size_t  size,
                          char   *data)
{
	if (data) {
		img_data_t *imd;
		if ((imd = img_data_alloc (size))) {
			memcpy ((void *) imd->data, (const void *) data, size);
		}
		return imd;
	}
	return NULL;
}

img_data_t *
img_data_new_from_file (const char *path)
{
	if (path) {
		size_t size;
		char *data;
		if ((data = read_binary_file (path, &size))) {
//			printf ("%s: read %zu bytes\n", __func__, size);
			img_data_t *imd = img_data_new_from_buffer (size, data);
			free (data);
			data = NULL;
			size = 0;
			return imd;
		}
	}
	return NULL;
}

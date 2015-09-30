/* Copyright © 2015 Koneet Kiertoon and Juuso Alasuutari.
 * Written by Juuso Alasuutari.
 *
 * This file is part of Kivijalka.
 *
 * Kivijalka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kivijalka is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kivijalka.  If not, see <http://www.gnu.org/licenses/>.
 */
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
	return (imd) ? offsetof (struct img_data, data) + imd->size : 0;
}

img_data_t *
img_data_alloc (size_t size)
{
	img_data_t *data;
	if ((data = malloc (offsetof (struct img_data, data) + size))) {
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
img_data_new_from_buffer (size_t      size,
                          const char *data)
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
		img_data_t *data;
		size_t size;
		if ((data = (img_data_t *) read_binary_file (path, &size, offsetof (struct img_data, data)))) {
			data->size = size;
//			printf ("%s: read %zu bytes\n", __func__, size);
			size = 0;
			return data;
		}
	}
	return NULL;
}

bool
img_data_cmp (img_data_t *imd,
              img_data_t *imd2)
{
	if (imd) {
		if (imd == imd2) {
			return true;
		}
		if (imd2) {
			size_t c = imd2->size;
			if (c == imd->size) {
				size_t s = c / sizeof (size_t), i = 0;
				union {
					size_t *s;
					char   *c;
				} p[2] = {
					{.s = (size_t *) imd->data},
					{.s = (size_t *) imd2->data}
				};
				for (; i < s; ++i) {
					if (p[0].s[i] != p[1].s[i]) {
						return false;
					}
				}
				p[0].c = imd->data;
				p[1].c = imd2->data;
				for (i *= sizeof (size_t); i < c; ++i) {
					if (p[0].c[i] != p[1].c[i]) {
						return false;
					}
				}
				return true;
			}
		} else {
			fprintf (stderr, "%s: 2nd param is null\n", __func__);
		}
	} else {
		fprintf (stderr, "%s: 1st param is null\n", __func__);
	}
	return false;
}

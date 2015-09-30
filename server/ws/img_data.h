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
/** \file img_data.h
 *
 * Image manipulation.
 */

#ifndef __IMG_DATA_H__
#define __IMG_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef struct img_data img_data_t;

struct img_data {
	size_t size;
	char   data[];
} __attribute__((gcc_struct,packed));

/**
 * Return the total size of an img_data_t object in bytes.
 */
extern size_t
img_data_size (img_data_t *imd);

/**
 * Allocate a new img_data_t object.
 */
extern img_data_t *
img_data_alloc (size_t size);

/**
 * Wipe (overwrite with zero) and free an img_data_t object.
 */
extern void
img_data_free (img_data_t *imd);

/**
 * Read the contents of a buffer into a newly allocated img_data_t object.
 */
extern img_data_t *
img_data_new_from_buffer (size_t      size,
                          const char *data);

/**
 * Read a file into a newly allocated img_data_t object.
 */
extern img_data_t *
img_data_new_from_file (const char *path);

/**
 * @brief Compare two img_data_t objects.
 * @param imd Pointer to first img_data_t object.
 * @param imd2 Pointer to second img_data_t object.
 * @return true if the object pointers are the same or if the data
 *         are identical, false otherwise.
 */
extern bool
img_data_cmp (img_data_t *imd,
              img_data_t *imd2);

#ifdef __cplusplus
}
#endif

#endif // __IMG_DATA_H__

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
/** \file img_file.h
 *
 * Image manipulation.
 */

#ifndef __IMG_FILE_H__
#define __IMG_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

#include "img_data.h"

typedef struct img_file img_file_t;

struct img_file {
	sem_t       lock;
	const char *path;
#ifdef __cplusplus
	img_data_t *data;
#else
	img_data_t *_Atomic data;
#endif
} __attribute__((gcc_struct,packed));

extern void
img_file_init (img_file_t *imf);

extern void
img_file_fini (img_file_t *imf);

extern bool
img_file_set_path (img_file_t *imf,
                   const char *path);

/**
 * Atomically grab the img_data_t pointer from an img_file_t object and
 * replace the pointer with NULL. This function provides a means for any
 * thread accessing the img_file_t object to be certain that it is the
 * sole owner of the pointer. Notice, however, that this implies that
 * the thread stealing the img_data_t pointer must also free it later on.
 *
 * @param imf Pointer to the img_file_t object to steal the pointer from.
 * @return The stolen pointer.
 */
extern img_data_t *
img_file_steal_data (img_file_t *imf);

extern void
img_file_replace_data (img_file_t *imf,
                       img_data_t *imd);

extern bool
img_file_wait (img_file_t *imf);

extern bool
img_file_post (img_file_t *imf);

extern bool
img_file_update (img_file_t *imf);

#ifdef __cplusplus
}
#endif

#endif // __IMG_FILE_H__

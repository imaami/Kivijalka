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
/** \file file.h
 *
 * File read/write.
 */

#ifndef __KIVIJALKA_FILE_H__
#define __KIVIJALKA_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "buf.h"

typedef struct file file_t;

extern file_t *
file_create (const char *path);

extern void
file_destroy (file_t **f);

extern const char *
file_path (file_t *f);

extern bool
file_open (file_t     *f,
           const char *mode);

extern bool
file_close (file_t *f);

extern bool
file_size (file_t *f,
           size_t *size);

extern bool
file_read (file_t  *f,
           size_t   size,
           uint8_t *data,
           size_t  *count);

extern bool
file_write (file_t        *f,
            size_t         size,
            const uint8_t *data);

extern bool
file_read_to_buf (file_t *f,
                  buf_t  *buf,
                  size_t *count);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_FILE_H__

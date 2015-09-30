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
/** \file global.h
 *
 * Global variables.
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <semaphore.h>

#include "img_file.h"
#include "banner.h"

extern size_t thumb_w, thumb_h;
extern sem_t process_sem;
extern img_file_t capture_file, banner_file, output_file, thumb_file;
extern banner_t *cur_banner;

extern void
global_init (size_t thumb_width,
             size_t thumb_height);

extern void
global_fini (void);

#ifdef __cplusplus
}
#endif

#endif // __GLOBAL_H__

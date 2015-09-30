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
/** \file watcher.h
 *
 * File change watcher.
 */

#ifndef __WATCHER_H__
#define __WATCHER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <semaphore.h>

typedef struct watcher watcher_t;

extern watcher_t *
watcher_create (const char *path);

extern bool
watcher_prepare (watcher_t *w);

extern void
watcher_start (watcher_t *w);

extern bool
watcher_wait (watcher_t *w);

extern void
watcher_stop (watcher_t *w);

extern void
watcher_destroy (watcher_t *w);

#ifdef __cplusplus
}
#endif

#endif // __WATCHER_H__

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
#ifndef __KIVIJALKA_GEO2D_H__
#define __KIVIJALKA_GEO2D_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct geo2d geo2d_t;

extern void
geo2d_init (geo2d_t *g);

extern void
geo2d_set_width (geo2d_t  *g,
                 uint32_t  width);

extern void
geo2d_set_height (geo2d_t  *g,
                  uint32_t  height);

extern void
geo2d_set (geo2d_t  *g,
           uint32_t  width,
           uint32_t  height);

extern uint32_t
geo2d_width (geo2d_t *g);

extern uint32_t
geo2d_height (geo2d_t *g);

extern void
geo2d_cpy (geo2d_t *g,
           geo2d_t *dest);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_GEO2D_H__

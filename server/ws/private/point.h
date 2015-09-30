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
#ifndef __KIVIJALKA_PRIVATE_POINT_H__
#define __KIVIJALKA_PRIVATE_POINT_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../point.h"

__attribute__((always_inline))
static inline void
_point_cpy (point_t *src,
            point_t *dest)
{
	dest->u64 = src->u64;
}

#endif // __KIVIJALKA_PRIVATE_POINT_H__

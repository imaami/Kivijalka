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
#ifndef __KIVIJALKA_POINT_H__
#define __KIVIJALKA_POINT_H__

#ifdef __cplusplus
extern "C" {
#else
#include <stdatomic.h>
#endif

#include <stdint.h>

typedef union {
	uint64_t u64;
	#ifndef __cplusplus
	_Atomic uint64_t a64;
	#endif
	struct {
		int32_t x : 32;
		int32_t y : 32;
	} __attribute__((gcc_struct,packed));
} point_t;

#define POINT_INIT(a, b) { .x = (a), .y = (b) }

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_POINT_H__

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
#ifndef __KIVIJALKA_PRIVATE_MEM_H__
#define __KIVIJALKA_PRIVATE_MEM_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <limits.h> // CHAR_BIT
#include <stddef.h> // NULL, size_t
#include <stdlib.h> // aligned_alloc(), free()
#include <stdint.h>

typedef union __attribute__((transparent_union)) {
	void          **v;
	char          **c;
	unsigned char **uc;
	int8_t        **i8;
	uint8_t       **u8;
	int16_t       **i16;
	uint16_t      **u16;
	int32_t       **i32;
	uint32_t      **u32;
	int64_t       **i64;
	uint64_t      **u64;
} mem_del_t;

#define MEM_BIT_SHIFT_MAX (CHAR_BIT * sizeof (size_t) - 1)

__attribute__((always_inline))
static inline void *
_mem_new (size_t  log2_align,
          size_t  minimum_bytes,
          size_t *allocated)
{
	void *m;

	if (log2_align > MEM_BIT_SHIFT_MAX) {
		fprintf (stderr, "%s: invalid alignment parameter\n", __func__);
		m = NULL;
	} else {
		size_t alignment = ((size_t) 1) << log2_align;
		size_t alloc_size = minimum_bytes;
		size_t padding = minimum_bytes & (alignment - 1);

		if (padding) {
			alloc_size += (alignment - padding);
		}

		if (!(m = aligned_alloc (alignment, alloc_size))) {
			fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
		} else {
			*allocated = alloc_size;
//			printf ("%s: allocated %zu B aligned to %zu B\n",
//			        __func__, alloc_size, alignment);
		}
	}

	return m;
}

__attribute__((always_inline))
static inline void
_mem_del (mem_del_t m)
{
	if (m.v) {
		free (*(m.v));
		*(m.v) = NULL;
	}
}

#endif // __KIVIJALKA_PRIVATE_MEM_H__

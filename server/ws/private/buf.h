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
/** \file private/buf.h
 *
 * Memory buffer. Private parts.
 */

#ifndef __KIVIJALKA_PRIVATE_BUF_H__
#define __KIVIJALKA_PRIVATE_BUF_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../buf.h"

/** Data buffer object. */
struct buf {
	size_t  size;   //!< Size in bytes of \a data array
	size_t  used;   //!< Bytes in use counting from the beginning
	uint8_t data[]; //!< Buffer contents
} __attribute__((packed,gcc_struct));

__attribute__((always_inline))
static inline struct buf *
_buf_create (size_t size)
{
	if (SIZE_MAX - offsetof (struct buf, data) >= size) {
		struct buf *b;
		if ((b = aligned_alloc (64, offsetof (struct buf, data) + size))) {
			b->size = size;
			b->used = 0;
			return b;
		} else {
			fprintf (stderr, "%s: aligned_alloc failed\n",
			         __func__);
		}
	} else {
		fprintf (stderr, "%s: requested size %zu is too large\n",
		         __func__, size);
	}
	return NULL;
}

__attribute__((always_inline))
static inline void
_buf_destroy (struct buf **b)
{
	struct buf *_b = *b;
	if (_b) {
		size_t s = offsetof (struct buf, data) + _b->size;
		(void) memset ((void *) _b, 0, s);
		free (_b);
		*b = _b = NULL;
	}
}

__attribute__((always_inline))
static inline size_t
_buf_size (struct buf *b)
{
	return b->size;
}

__attribute__((always_inline))
static inline size_t
_buf_used (struct buf *b)
{
	return b->used;
}

__attribute__((always_inline))
static inline size_t
_buf_unused (struct buf *b)
{
	return b->size - b->used;
}

__attribute__((always_inline))
static inline uint8_t *
_buf_ptr (struct buf *b)
{
	return b->data + b->used;
}

__attribute__((always_inline))
static inline uint8_t *
_buf_alloc (struct buf *b,
            size_t      size)
{
	uint8_t *p;
	if (_buf_unused (b) >= size) {
		p = _buf_ptr (b);
		b->used += size;
	} else {
		p = NULL;
	}
	return p;
}

__attribute__((always_inline))
static inline char *
_buf_reset (struct buf *b)
{
	b->used = 0;
	return (char *) b->data;
}

#endif // __KIVIJALKA_PRIVATE_BUF_H__

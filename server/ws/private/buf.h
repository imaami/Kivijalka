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
static inline uint8_t *
_buf_alloc (struct buf *b,
            size_t      size)
{
	uint8_t *p;
	if (b->size - b->used >= size) {
		p = b->data + b->used;
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

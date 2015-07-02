/** \buf buf.h
 *
 * Memory buffer.
 */

#ifndef __KIVIJALKA_BUF_H__
#define __KIVIJALKA_BUF_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct buf buf_t;

/** Data buffer object. */
struct buf {
	size_t  size;   //!< Size in bytes of \a data array
	size_t  used;   //!< Bytes in use counting from the beginning
	uint8_t data[]; //!< Buffer contents
} __attribute__((packed,gcc_struct));

/**
 * @brief Create (allocate) a new data buffer.
 * @param size Number of bytes the buffer should have available.
 * @return Pointer to newly allocated buffer object if the operation
 *         succeeded, NULL otherwise.
 */
__attribute__((always_inline))
static inline struct buf *
buf_create (size_t size)
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

/**
 * @brief Destroy a buffer object.
 * @param b Pointer to the buffer pointer.
 */
__attribute__((always_inline))
static inline void
buf_destroy (struct buf **b)
{
	struct buf *_b = *b;
	if (_b) {
		size_t s = offsetof (struct buf, data) + _b->size;
		(void) memset ((void *) _b, 0, s);
		free (_b);
		*b = _b = NULL;
	}
}

/**
 * @brief Allocate (reserve) space within an existing buffer object.
 * @param b Pointer to the buffer pointer.
 * @param size Number of bytes to allocate.
 * @return Pointer to the first byte of the allocated space
 *         if the operation succeeded, NULL otherwise. Do not
 *         free() the returned pointer - it is just a pointer
 *         into the buffer object's internal data array, not
 *         an allocated object.
 */
__attribute__((always_inline))
static inline char *
buf_alloc (struct buf *b,
           size_t      size)
{
	char *p;
	if (b->size - b->used >= size) {
		p = (char *) &b->data[b->used];
		b->used += size;
	} else {
		p = NULL;
	}
	return p;
}

/**
 * @brief Reset a buffer's used bytes counter. No data is zeroed out,
 *        but the next call to \a buf_alloc will return a pointer
 *        which points the beginning of the data array, and writing to
 *        it will therefore overwrite previously written bytes.
 * @param b Pointer to the buffer pointer.
 * @return Pointer to the first byte of the buffer's data array.
 */
__attribute__((always_inline))
static inline char *
buf_reset (struct buf *b)
{
	b->used = 0;
	return (char *) b->data;
}

#endif // __KIVIJALKA_BUF_H__

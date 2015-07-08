/** \file buf.c
 *
 * Memory buffer object.
 */

#include "private/buf.h"

struct buf *
buf_create (size_t size)
{
	return _buf_create (size);
}

void
buf_destroy (struct buf **b)
{
	if (b) {
		_buf_destroy (b);
	}
}

size_t
buf_size (struct buf *b)
{
	return (b) ? _buf_size (b) : 0;
}

size_t
buf_used (struct buf *b)
{
	return (b) ? _buf_used (b) : SIZE_MAX;
}

size_t
buf_unused (struct buf *b)
{
	return (b) ? _buf_unused (b) : 0;
}

uint8_t *
buf_ptr (struct buf *b)
{
	return (b) ? _buf_ptr (b) : NULL;
}

uint8_t *
buf_alloc (struct buf *b,
           size_t      size)
{
	return (b) ? _buf_alloc (b, size) : NULL;
}

char *
buf_reset (struct buf *b)
{
	return (b) ? _buf_reset (b) : NULL;
}

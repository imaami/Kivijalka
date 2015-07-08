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

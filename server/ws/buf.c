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

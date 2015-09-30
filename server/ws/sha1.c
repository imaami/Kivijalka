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
#include "private/sha1.h"

void
sha1_init (sha1_t *hash)
{
	if (hash) {
		_sha1_init (hash);
	}
}

void
sha1_gen (sha1_t  *hash,
          size_t   size,
          uint8_t *data)
{
	if (hash && data) {
		_sha1_gen (hash, size, data);
	}
}

bool
sha1_cmp (sha1_t *h1,
          sha1_t *h2)
{
	return (h1 && h2) ? _sha1_cmp (h1, h2) : false;
}

void
sha1_cpy (sha1_t *src,
          sha1_t *dest)
{
	if (src && dest) {
		_sha1_cpy (src, dest);
	}
}

void
sha1_str (sha1_t *hash,
          char   *dest)
{
	if (hash && dest) {
		_sha1_str (hash, dest);
	}
}

bool
sha1_parse (const char *src,
            sha1_t     *dest)
{
	if (src && dest) {
		_sha1_parse (src, dest);
	}
}

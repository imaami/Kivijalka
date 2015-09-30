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
#ifndef __KIVIJALKA_SHA1_H__
#define __KIVIJALKA_SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef union {
	uint32_t u32[5];
	uint16_t u16[10];
	uint8_t   u8[20];
} sha1_t;

#define SHA1_INITIALIZER {.u32 = {0, 0, 0, 0, 0}}

extern void
sha1_init (sha1_t *hash);

extern void
sha1_gen (sha1_t  *hash,
          size_t   size,
          uint8_t *data);

extern bool
sha1_cmp (sha1_t *h1,
          sha1_t *h2);

extern void
sha1_cpy (sha1_t *src,
          sha1_t *dest);

extern void
sha1_str (sha1_t *hash,
          char   *dest);

extern bool
sha1_parse (const char *src,
            sha1_t     *dest);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_SHA1_H__

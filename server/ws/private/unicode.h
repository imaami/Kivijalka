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
#ifndef __KIVIJALKA_PRIVATE_UNICODE_H__
#define __KIVIJALKA_PRIVATE_UNICODE_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stdint.h>

/** Struct representing a UTF-8 continuation character. */
struct utf8_cc {
	uint8_t bits : 6;
	uint8_t      : 2;
} __attribute__((gcc_struct,packed));

/*
 * Single-byte UTF-8
 */

/** Struct representing the leading and only byte of a single-byte UTF-8 character. */
struct utf8_l1 {
	uint8_t bits : 7;
	uint8_t      : 1;
} __attribute__((gcc_struct,packed));

/** Union representing a single-byte UTF-8 character. */
union utf8_1b {
	uint8_t        u8;
	struct utf8_l1 _0;
} __attribute__((gcc_struct,packed));

/*
 * Two-byte UTF-8
 */

/** Struct representing the leading byte of a two-byte UTF-8 character. */
struct utf8_l2 {
	uint8_t bits : 5;
	uint8_t      : 3;
} __attribute__((gcc_struct,packed));

/** Union representing any byte of a two-byte UTF-8 character. */
union utf8_2b {
	uint8_t        u8;
	struct utf8_l2 _0;
	struct utf8_cc _1;
} __attribute__((gcc_struct,packed));

/*
 * Three-byte UTF-8
 */

/** Struct representing the leading byte of a three-byte UTF-8 character. */
struct utf8_l3 {
	uint8_t bits : 4;
	uint8_t      : 4;
} __attribute__((gcc_struct,packed));

/** Union representing any byte of a three-byte UTF-8 character. */
union utf8_3b {
	uint8_t        u8;
	struct utf8_l3 _0;
	struct utf8_cc _1;
	struct utf8_cc _2;
} __attribute__((gcc_struct,packed));

/*
 * Four-byte UTF-8
 */

/** Struct representing the leading byte of a four-byte UTF-8 character. */
struct utf8_l4 {
	uint8_t bits : 3;
	uint8_t      : 5;
} __attribute__((gcc_struct,packed));

/** Union representing any byte of a four-byte UTF-8 character. */
union utf8_4b {
	uint8_t        u8;
	struct utf8_l4 _0;
	struct utf8_cc _1;
	struct utf8_cc _2;
	struct utf8_cc _3;
} __attribute__((gcc_struct,packed));

/** Union representing any UTF-8 byte. */
union utf8_byte {
	uint8_t        u8;
	struct utf8_l1 l1;
	struct utf8_l2 l2;
	struct utf8_l3 l3;
	struct utf8_l4 l4;
	struct utf8_cc cc;
} __attribute__((gcc_struct,packed));

/** Union representing any UTF-8-encoded character (1 to 3 bytes long). */
union utf8_char {
	uint32_t       u32;
	uint8_t        u8[4];
	struct {
		struct utf8_l1 _0;
	} _1 __attribute__((gcc_struct,packed));
	struct {
		struct utf8_l2 _0;
		struct utf8_cc _1;
	} _2 __attribute__((gcc_struct,packed));
	struct {
		struct utf8_l3 _0;
		struct utf8_cc _1;
		struct utf8_cc _2;
	} _3 __attribute__((gcc_struct,packed));
	struct {
		struct utf8_l4 _0;
		struct utf8_cc _1;
		struct utf8_cc _2;
		struct utf8_cc _3;
	} _4 __attribute__((gcc_struct,packed));
} __attribute__((gcc_struct,packed));

union code_point {
	uint32_t u32;
	struct {
		uint8_t  _0 :  7;
		uint32_t    : 25;
	} _1b __attribute__((gcc_struct,packed));
	struct {
		uint8_t  _1 :  6;
		uint8_t  _0 :  5;
		uint32_t    : 21;
	} _2b __attribute__((gcc_struct,packed));
	struct {
		uint8_t  _2 :  6;
		uint8_t  _1 :  6;
		uint8_t  _0 :  4;
		uint16_t    : 16;
	} _3b __attribute__((gcc_struct,packed));
	struct {
		uint8_t _3 :  6;
		uint8_t _2 :  6;
		uint8_t _1 :  6;
		uint8_t _0 :  3;
		uint16_t    : 11;
	} _4b __attribute__((gcc_struct,packed));
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline size_t
_utf8_size (union utf8_byte b)
{
	size_t r;

	switch (b.u8) {
	case 0x00 ... 0x7f:
		r = 1;
		break;

	case 0xc0 ... 0xdf:
		r = 2;
		break;

	case 0xe0 ... 0xef:
		r = 3;
		break;

	case 0xf0 ... 0xf7:
		r = 4;
		break;

	default:
		r = 0;
		break;
	}

	return r;
}

__attribute__((always_inline))
static inline bool
_utf8_next (uint8_t         *buf,
            size_t           len,
            size_t           pos,
            size_t          *size,
            union utf8_char *dest)
{
	bool r;
	union utf8_char c = {.u32 = 0};

	if (pos < len) {
		union utf8_byte b = {.u8 = buf[pos]};
		size_t l = len - pos, s = _utf8_size (b);

		if (0 == s || l < s) {
			r = false;
			goto _at_end;
		}

		switch (s) {
		case 1:
			c._1._0.bits = b.l1.bits;
			break;

		case 2:
			c._2._0.bits = b.l2.bits;
			break;

		case 3:
			c._3._0.bits = b.l3.bits;
			break;

		case 4:
			c._4._0.bits = b.l4.bits;
		}

	} else {
		dest->u32 = c.u32;
	_at_end:
		*size = s;
	}

	return r;
}

	__attribute__((always_inline))
static inline size_t
_utf8_to_code_point (uint8_t  *buf,
                     size_t    len,
                     size_t    pos,
                     uint32_t *cp)
{
	union code_point cp;
	uint8_t byte = buf[pos];
	size_t i = pos, n;
	union utf8_leading byte = 

	switch ((n = _utf8_size (byte))) {
	case 1:
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:
		break;

	default:
		cp.u32 = 0;
		break;
	}

	return cp;
}


#endif // __KIVIJALKA_PRIVATE_UNICODE_H__

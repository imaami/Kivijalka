#ifndef __KIVIJALKA_PRIVATE_JSON_H__
#define __KIVIJALKA_PRIVATE_JSON_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

__attribute__((always_inline))
static inline size_t
_u32_to_str (uint32_t  val,
             char     *dest)
{
	uint32_t v = val;
	char c = '0';
	size_t i = 0;

	if (v >= 1000000000) {
		do {
			v -= 1000000000;
			++c;
		} while (v >= 1000000000);
		dest[i++] = c;

		for (c = '0'; v >= 100000000; ++c) {
		_8:
			v -= 100000000;
		}
		dest[i++] = c;

		for (c = '0'; v >= 10000000; ++c) {
		_7:
			v -= 10000000;
		}
		dest[i++] = c;

		for (c = '0'; v >= 1000000; ++c) {
		_6:
			v -= 1000000;
		}
		dest[i++] = c;

		for (c = '0'; v >= 100000; ++c) {
		_5:
			v -= 100000;
		}
		dest[i++] = c;

		for (c = '0'; v >= 10000; ++c) {
		_4:
			v -= 10000;
		}
		dest[i++] = c;

		for (c = '0'; v >= 1000; ++c) {
		_3:
			v -= 1000;
		}
		dest[i++] = c;

		for (c = '0'; v >= 100; ++c) {
		_2:
			v -= 100;
		}
		dest[i++] = c;

		for (c = '0'; v >= 10; ++c) {
		_1:
			v -= 10;
		}
		dest[i++] = c;

	} else {
		if (v >= 100000000) {
			goto _8;
		}

		if (v >= 10000000) {
			goto _7;
		}

		if (v >= 1000000) {
			goto _6;
		}

		if (v >= 100000) {
			goto _5;
		}

		if (v >= 10000) {
			goto _4;
		}

		if (v >= 1000) {
			goto _3;
		}

		if (v >= 100) {
			goto _2;
		}

		if (v >= 10) {
			goto _1;
		}
	}

	dest[i++] = '0' + v;
	dest[i] = '\0';

	return i;
}

__attribute__((always_inline))
static inline size_t
_i32_to_str (int32_t  val,
             char    *dest)
{
	size_t i;

	if (val >= 0) {
		i = _u32_to_str ((uint32_t) val, dest);
	} else {
		int32_t v = val;
		char c = '0';

		dest[0] = '-';
		i = 1;

		if (v <= -1000000000) {
			do {
				v += 1000000000;
				++c;
			} while (v <= -1000000000);
			dest[i++] = c;

			for (c = '0'; v <= -100000000; ++c) {
			_8:
				v += 100000000;
			}
			dest[i++] = c;

			for (c = '0'; v <= -10000000; ++c) {
			_7:
				v += 10000000;
			}
			dest[i++] = c;

			for (c = '0'; v <= -1000000; ++c) {
			_6:
				v += 1000000;
			}
			dest[i++] = c;

			for (c = '0'; v <= -100000; ++c) {
			_5:
				v += 100000;
			}
			dest[i++] = c;

			for (c = '0'; v <= -10000; ++c) {
			_4:
				v += 10000;
			}
			dest[i++] = c;

			for (c = '0'; v <= -1000; ++c) {
			_3:
				v += 1000;
			}
			dest[i++] = c;

			for (c = '0'; v <= -100; ++c) {
			_2:
				v += 100;
			}
			dest[i++] = c;

			for (c = '0'; v <= -10; ++c) {
			_1:
				v += 10;
			}
			dest[i++] = c;

		} else {
			if (v <= -100000000) {
				goto _8;
			}

			if (v <= -10000000) {
				goto _7;
			}

			if (v <= -1000000) {
				goto _6;
			}

			if (v <= -100000) {
				goto _5;
			}

			if (v <= -10000) {
				goto _4;
			}

			if (v <= -1000) {
				goto _3;
			}

			if (v <= -100) {
				goto _2;
			}

			if (v <= -10) {
				goto _1;
			}
		}

		dest[i++] = '0' - v;
		dest[i] = '\0';
	}

	return i;
}

__attribute__((always_inline))
static inline size_t
_json_utf8_escaped_size (const char *str)
{
	size_t n = 0, i = 0;

	for (; str[i]; ++i) {
		switch (str[i]) {
		case 1 ... 7: case 11: case 14 ... 31:
			n += 5;
			break;
		case 8 ... 10: case 12 ... 13: case 34: case 47: case 92:
			++n;
		default:
			break;
		}
	}

	return i + n;
}

__attribute__((always_inline))
static inline bool
_json_stringify (const char  *str,
                 size_t      *pos,
                 size_t      *len,
                 uint8_t    **buf)
{
	uint8_t *b = *buf;
	size_t l = *len, p = *pos, i = 0;

	for (; str[i]; ++i, ++p) {
		/* make sure the buffer has enough space for any possible
		 * serialized char; the maximum amount we really need is
		 * six bytes, but 16 is a nice binarily-round number
		 */
		if (l - p < 16) {
			uint8_t *_b;
			if (!(_b = realloc (b, (l += BUFSIZ)))) {
				fprintf (stderr, "%s: realloc: %s\n",
				         __func__, strerror (errno));
				return false;
			}
			b = _b;
		}

		uint8_t c = ((uint8_t *) str)[i];

		switch (c) {
		case 0x01 ... 0x07:
			b[p++] = '\\';
			b[p++] = 'u';
			b[p++] = '0';
			b[p++] = '0';
			b[p++] = '0';
			c += '0';
			break;

		case 0x0b: case 0x0e ... 0x0f:
			b[p++] = '\\';
			b[p++] = 'u';
			b[p++] = '0';
			b[p++] = '0';
			b[p++] = '0';
			c += ('a' - 0x0a);
			break;

		case 0x10 ... 0x19:
			b[p++] = '\\';
			b[p++] = 'u';
			b[p++] = '0';
			b[p++] = '0';
			b[p++] = '1';
			c += '0';
			break;

		case 0x1a ... 0x1f:
			b[p++] = '\\';
			b[p++] = 'u';
			b[p++] = '0';
			b[p++] = '0';
			b[p++] = '1';
			c += ('a' - 0x0a);
			break;

		case '\b': // 0x08
			c = 'b';
			goto escape;

		case '\t': // 0x09
			c = 't';
			goto escape;

		case '\n': // 0x0a
			c = 'n';
			goto escape;

		case '\f': // 0x0c
			c = 'f';
			goto escape;

		case '\r': // 0x0d
			c = 'r';

		case '"': // 0x22
		case '\\': // 0x5c
		escape:
			b[p++] = '\\';

		default:
			break;
		}

		b[p] = c;
	}

	*buf = b;
	*len = l;
	*pos = p;

	return true;
}

#endif // __KIVIJALKA_PRIVATE_JSON_H__

#ifndef __KIVIJALKA_PRIVATE_JSON_H__
#define __KIVIJALKA_PRIVATE_JSON_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stddef.h>
#include <stdint.h>

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

#endif // __KIVIJALKA_PRIVATE_JSON_H__

#ifndef __KIVIJALKA_PRIVATE_JSON_H__
#define __KIVIJALKA_PRIVATE_JSON_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stddef.h>
#include <stdint.h>

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

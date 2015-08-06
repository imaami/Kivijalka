#ifndef __KIVIJALKA_PRIVATE_PARSE_H__
#define __KIVIJALKA_PRIVATE_PARSE_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

__attribute__((always_inline))
static inline bool
_parse_u32 (char         *str,
            unsigned int *pos,
            uint32_t     *dest)
{
	uint32_t v;
	unsigned int i, p = *pos;
	unsigned int c = str[p];

	switch (c) {
	case '0':
		*pos = p + 1;
		*dest = 0;
		return true;

	case '1' ... '9':
		v = c - '0';
		i = p + 1;
		for (;;) {
			switch ((c = str[i])) {
			case '0' ... '9':
				v = (v << 1) + (v << 3) + (c - '0');

				if (++i - p < 9) {
					continue;
				}

				switch ((c = str[i])) {
				case '0' ... '9':
					if (v > 429496729
					    || (v == 429496729 && c > '5')) {
						goto _overflow_u32;
					}

					v = (v << 1) + (v << 3) + (c - '0');

					switch ((c = str[++i])) {
					case '0' ... '9':
					_overflow_u32:
						*dest = UINT32_MAX;
						return false;
					}
				}
			}

			break;
		}

		*pos = i;
		*dest = v;
		return true;
	}

	*dest = 0;
	return false;
}

__attribute__((always_inline))
static inline bool
_parse_i32 (char         *str,
            unsigned int *pos,
            int32_t      *dest)
{
	int32_t v;
	unsigned int i, p = *pos;
	unsigned int c = str[p];

	switch (c) {
	case '-':
		switch ((c = str[++p])) {
		case '0':
			goto _zero;

		case '1' ... '9':
			v = 0 - (int32_t) (c - '0');
			i = p + 1;
			for (;;) {
				switch ((c = str[i])) {
				case '0' ... '9':
					v *= 10;
					v -= (c - '0');

					if (++i - p < 9) {
						continue;
					}

					switch ((c = str[i])) {
					case '0' ... '9':
						if (v < -214748364
						    || (v == -214748364 && c > '8')) {
							goto _underflow_i32;
						}

						v *= 10;
						v -= (c - '0');

						switch ((c = str[++i])) {
						case '0' ... '9':
						_underflow_i32:
							*dest = INT32_MIN;
							return false;
						}
					}
				}

				break;
			}

			*pos = i;
			*dest = v;
			return true;
		}

		break;

	case '0':
	_zero:
		*pos = p + 1;
		*dest = 0;
		return true;

	case '1' ... '9':
		v = c - '0';
		i = p + 1;
		for (;;) {
			switch ((c = str[i])) {
			case '0' ... '9':
				v = (v << 1) + (v << 3) + (c - '0');

				if (++i - p < 9) {
					continue;
				}

				switch ((c = str[i])) {
				case '0' ... '9':
					if (v > 214748364
					    || (v == 214748364 && c > '7')) {
						goto _overflow_i32;
					}

					v = (v << 1) + (v << 3) + (c - '0');

					switch ((c = str[++i])) {
					case '0' ... '9':
					_overflow_i32:
						*dest = INT32_MAX;
						return false;
					}
				}
			}

			break;
		}

		*pos = i;
		*dest = v;
		return true;
	}

	*dest = 0;
	return false;
}

#endif // __KIVIJALKA_PRIVATE_PARSE_H__

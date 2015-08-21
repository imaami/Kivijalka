#ifndef __KIVIJALKA_PRIVATE_PARSE_H__
#define __KIVIJALKA_PRIVATE_PARSE_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <uuid/uuid.h>

__attribute__((always_inline))
static inline bool
_parse_u8 (char         *str,
           unsigned int *pos,
           uint8_t      *dest)
{
	uint8_t v;
	unsigned int p = (pos) ? *pos : 0;
	unsigned int c = str[p];

	switch (c) {
	case '0':
		v = 0;
		++p;
		break;

	case '1':
		v = 1;
		c = str[++p];
		switch (c) {
		case '0' ... '9':
			v = (v << 1) + (v << 3) + (c - '0');
			c = str[++p];
			switch (c) {
			case '0' ... '9':
				goto _must_end_u8;
			}
		}
		break;

	case '2':
		v = 2;
		c = str[++p];
		switch (c) {
		case '0' ... '4':
			v = (v << 1) + (v << 3) + (c - '0');
			c = str[++p];
			switch (c) {
			case '0' ... '9':
				goto _must_end_u8;
			}
			break;

		case '5':
			v = (v << 1) + (v << 3) + (c - '0');
			c = str[++p];
			switch (c) {
			case '0' ... '5':
				goto _must_end_u8;
			case '6' ... '9':
				goto _overflow_u8;
			}
			break;

		case '6' ... '9':
			goto _must_end_u8;
		}
		break;

	case '3' ... '9':
		v = c - '0';
		c = str[++p];
		switch (c) {
		case '0' ... '9':
		_must_end_u8:
			v = (v << 1) + (v << 3) + (c - '0');
			c = str[++p];
			switch (c) {
			case '0' ... '9':
			_overflow_u8:
				*dest = UINT8_MAX;
				return false;
			}
		}
		break;

	default:
		*dest = 0;
		return false;
	}

	if (pos) {
		*pos = p;
	}
	*dest = v;
	return true;
}

__attribute__((always_inline))
static inline bool
_parse_i8 (char         *str,
           unsigned int *pos,
           int8_t       *dest)
{
	int8_t v;
	unsigned int p = (pos) ? *pos : 0;
	unsigned int c = str[p];

	switch (c) {
	case '-':
		c = str[++p];
		switch (c) {
		case '0':
			goto _zero_i8;

		case '1' ... '9':
			v = 0 - (int8_t) (c - '0');
			c = str[++p];
			switch (c) {
			case '0' ... '1':
				break;
			case '2':
				break;
			case '3' ... '9':
				break;
			}
		}
		goto _error_i8;

	case '0':
	_zero_i8:
		v = 0;
		++p;
		break;

	case '1':
		c = str[++p];
		switch (c) {
		case '0' ... '1':
			v = 10 + (c - '0');
			c = str[++p];
			switch (c) {
			case '0' ... '9':
				goto _must_end_i8;
			}
			break;

		case '2':
			v = 12;
			c = str[++p];
			switch (c) {
			case '0' ... '7':
				goto _must_end_i8;
			case '8' ... '9':
				goto _overflow_i8;
			}
			break;

		case '3' ... '9':
			goto _must_end_i8;
		}

		v = 1;
		break;

	case '2' ... '9':
		v = c - '0';
		c = str[++p];
		switch (c) {
		case '0' ... '9':
		_must_end_i8:
			v = (v << 1) + (v << 3) + (c - '0');
			c = str[++p];
			switch (c) {
			case '0' ... '9':
			_overflow_i8:
				*dest = INT8_MAX;
				return false;
			}
		}
		break;

	default:
	_error_i8:
		*dest = 0;
		return false;
	}

	if (pos) {
		*pos = p;
	}
	*dest = v;
	return true;
}

__attribute__((always_inline))
static inline bool
_parse_u16 (char         *str,
            unsigned int *pos,
            uint16_t     *dest)
{
	uint16_t v;
	unsigned int i, p = (pos) ? *pos : 0;
	unsigned int c = str[p];

	switch (c) {
	case '0':
		if (pos) {
			*pos = p + 1;
		}
		*dest = 0;
		return true;

	case '1' ... '9':
		v = c - '0';
		i = p + 1;
		for (;;) {
			switch ((c = str[i])) {
			case '0' ... '9':
				v = (v << 1) + (v << 3) + (c - '0');

				if (++i - p < 4) {
					continue;
				}

				switch ((c = str[i])) {
				case '0' ... '9':
					if (v > 6553
					    || (v == 6553 && c > '5')) {
						goto _overflow_u16;
					}

					v = (v << 1) + (v << 3) + (c - '0');

					switch ((c = str[++i])) {
					case '0' ... '9':
					_overflow_u16:
						*dest = UINT16_MAX;
						return false;
					}
				}
			}

			break;
		}

		if (pos) {
			*pos = i;
		}
		*dest = v;
		return true;
	}

	*dest = 0;
	return false;
}

__attribute__((always_inline))
static inline bool
_parse_i16 (char         *str,
            unsigned int *pos,
            int16_t      *dest)
{
	int16_t v;
	unsigned int i, p = (pos) ? *pos : 0;
	unsigned int c = str[p];

	switch (c) {
	case '-':
		switch ((c = str[++p])) {
		case '0':
			goto _zero;

		case '1' ... '9':
			v = 0 - (int16_t) (c - '0');
			i = p + 1;
			for (;;) {
				switch ((c = str[i])) {
				case '0' ... '9':
					v *= 10;
					v -= (c - '0');

					if (++i - p < 4) {
						continue;
					}

					switch ((c = str[i])) {
					case '0' ... '9':
						if (v < -3276
						    || (v == -3276 && c > '8')) {
							goto _underflow_i16;
						}

						v *= 10;
						v -= (c - '0');

						switch ((c = str[++i])) {
						case '0' ... '9':
						_underflow_i16:
							*dest = INT16_MIN;
							return false;
						}
					}
				}

				break;
			}

			if (pos) {
				*pos = i;
			}
			*dest = v;
			return true;
		}

		break;

	case '0':
	_zero:
		if (pos) {
			*pos = p + 1;
		}
		*dest = 0;
		return true;

	case '1' ... '9':
		v = c - '0';
		i = p + 1;
		for (;;) {
			switch ((c = str[i])) {
			case '0' ... '9':
				v = (v << 1) + (v << 3) + (c - '0');

				if (++i - p < 4) {
					continue;
				}

				switch ((c = str[i])) {
				case '0' ... '9':
					if (v > 3276
					    || (v == 3276 && c > '7')) {
						goto _overflow_i16;
					}

					v = (v << 1) + (v << 3) + (c - '0');

					switch ((c = str[++i])) {
					case '0' ... '9':
					_overflow_i16:
						*dest = INT16_MAX;
						return false;
					}
				}
			}

			break;
		}

		if (pos) {
			*pos = i;
		}
		*dest = v;
		return true;
	}

	*dest = 0;
	return false;
}

__attribute__((always_inline))
static inline bool
_parse_u32 (char         *str,
            unsigned int *pos,
            uint32_t     *dest)
{
	uint32_t v;
	unsigned int i, p = (pos) ? *pos : 0;
	unsigned int c = str[p];

	switch (c) {
	case '0':
		if (pos) {
			*pos = p + 1;
		}
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

		if (pos) {
			*pos = i;
		}
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
	unsigned int i, p = (pos) ? *pos : 0;
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

			if (pos) {
				*pos = i;
			}
			*dest = v;
			return true;
		}

		break;

	case '0':
	_zero:
		if (pos) {
			*pos = p + 1;
		}
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

		if (pos) {
			*pos = i;
		}
		*dest = v;
		return true;
	}

	*dest = 0;
	return false;
}

__attribute__((always_inline))
static inline bool
_parse_u64 (char         *str,
            unsigned int *pos,
            uint64_t     *dest)
{
	uint64_t v;
	unsigned int i, p = (pos) ? *pos : 0;
	unsigned int c = str[p];

	switch (c) {
	case '0':
		if (pos) {
			*pos = p + 1;
		}
		*dest = 0;
		return true;

	case '1' ... '9':
		v = c - '0';
		i = p + 1;
		for (;;) {
			switch ((c = str[i])) {
			case '0' ... '9':
				v = (v << 1) + (v << 3) + (c - '0');

				if (++i - p < 19) {
					continue;
				}

				switch ((c = str[i])) {
				case '0' ... '9':
					if (v > 1844674407370955161
					    || (v == 1844674407370955161
					        && c > '5')) {
						goto _overflow_u64;
					}

					v = (v << 1) + (v << 3) + (c - '0');

					switch ((c = str[++i])) {
					case '0' ... '9':
					_overflow_u64:
						*dest = UINT64_MAX;
						return false;
					}
				}
			}

			break;
		}

		if (pos) {
			*pos = i;
		}
		*dest = v;
		return true;
	}

	*dest = 0;
	return false;
}

__attribute__((always_inline))
static inline bool
_parse_i64 (char         *str,
            unsigned int *pos,
            int64_t      *dest)
{
	int64_t v;
	unsigned int i, p = (pos) ? *pos : 0;
	unsigned int c = str[p];

	switch (c) {
	case '-':
		switch ((c = str[++p])) {
		case '0':
			goto _zero;

		case '1' ... '9':
			v = 0 - (int64_t) (c - '0');
			i = p + 1;
			for (;;) {
				switch ((c = str[i])) {
				case '0' ... '9':
					v *= 10;
					v -= (c - '0');

					if (++i - p < 18) {
						continue;
					}

					switch ((c = str[i])) {
					case '0' ... '9':
						if (v < -922337203685477580
						    || (v == -922337203685477580
						        && c > '8')) {
							goto _underflow_i64;
						}

						v *= 10;
						v -= (c - '0');

						switch ((c = str[++i])) {
						case '0' ... '9':
						_underflow_i64:
							*dest = INT64_MIN;
							return false;
						}
					}
				}

				break;
			}

			if (pos) {
				*pos = i;
			}
			*dest = v;
			return true;
		}

		break;

	case '0':
	_zero:
		if (pos) {
			*pos = p + 1;
		}
		*dest = 0;
		return true;

	case '1' ... '9':
		v = c - '0';
		i = p + 1;
		for (;;) {
			switch ((c = str[i])) {
			case '0' ... '9':
				v = (v << 1) + (v << 3) + (c - '0');

				if (++i - p < 18) {
					continue;
				}

				switch ((c = str[i])) {
				case '0' ... '9':
					if (v > 922337203685477580
					    || (v == 922337203685477580
					        && c > '7')) {
						goto _overflow_i64;
					}

					v = (v << 1) + (v << 3) + (c - '0');

					switch ((c = str[++i])) {
					case '0' ... '9':
					_overflow_i64:
						*dest = INT64_MAX;
						return false;
					}
				}
			}

			break;
		}

		if (pos) {
			*pos = i;
		}
		*dest = v;
		return true;
	}

	*dest = 0;
	return false;
}

__attribute__((always_inline))
static inline bool
_parse_xgeo (char         *str,
             unsigned int *pos,
             uint32_t     *w,
             uint32_t     *h,
             int32_t      *x,
             int32_t      *y)
{
	unsigned int p = (pos) ? *pos : 0;
	uint32_t _w, _h;
	int32_t _x, _y;

	if (_parse_u32 (str, &p, &_w)) {
		if (str[p] == 'x') {
			++p;
			if (_parse_u32 (str, &p, &_h)) {
				switch (str[p]) {
				case '\0':
					_x = 0;
					goto _xgeo_zero_y;
				case '+':
					++p;
				}

				if (_parse_i32 (str, &p, &_x)) {
					switch (str[p]) {
					case '\0':
					_xgeo_zero_y:
						_y = 0;
						goto _xgeo_end;
					case '+':
						++p;
					}

					if (_parse_i32 (str, &p, &_y)) {
					_xgeo_end:
						if (pos) {
							*pos = p;
						}

						*w = _w;
						*h = _h;
						*x = _x;
						*y = _y;

						return true;
					}
				}
			}
		}
	}

	return false;
}

typedef union __attribute__((transparent_union)) {
	const char *cc;
	uint8_t    *u8;
} id_str_t;

/**
 * @brief Parse an arbitrary-length hexadecimal ID string.
 * @param src Pointer to input string.
 * @param dest Pointer to destination buffer to write the resulting bytes to.
 * @param req The number of characters requested to be parsed, beginning at
 *            \a src.
 * @return The number of characters parsed. If this is less than \a req it
 *         means that an error occurred whilst parsing the input character
 *         found at the offset indicated by this return value.
 */
__attribute__((always_inline))
static inline size_t
_parse_id (id_str_t  src,
           uint8_t  *dest,
           size_t    req)
{
	size_t i = 0;

	if (req > 0) {
		for (uint8_t *ptr = dest, x;; ++ptr) {
			switch ((x = src.u8[i])) {
			case '0' ... '9':
				x -= '0';
				break;
			case 'a' ... 'f':
				x -= ('a' - 10);
				break;
			case 'A' ... 'F':
				x -= ('A' - 10);
				break;
			default:
				goto _end;
			}

			*ptr = x << 4;

			if (++i >= req) {
				break;
			}

			switch ((x = src.u8[i])) {
			case '0' ... '9':
				x -= '0';
				break;
			case 'a' ... 'f':
				x -= ('a' - 10);
				break;
			case 'A' ... 'F':
				x -= ('A' - 10);
				break;
			default:
				goto _end;
			}

			*ptr |= x;

			if (++i >= req) {
				break;
			}
		}
	}

_end:
	return i;
}

__attribute__((always_inline))
static inline bool
_parse_uuid (const char *src,
             uuid_t      dest,
             size_t     *pos)
{
	uint8_t *d = (uint8_t *) dest;
	size_t n;

	switch ((n = _parse_id (src, d, 32))) {
	case 8:
		if (src[8] != '-'     || _parse_id (src + 9, d + 4, 4) != 4
		    || src[13] != '-' || _parse_id (src + 14, d + 6, 4) != 4
		    || src[18] != '-' || _parse_id (src + 19, d + 8, 4) != 4
		    || src[23] != '-' || _parse_id (src + 24, d + 10, 12) != 12) {
			break;
		}
		n = 36;
	case 32:
		*pos = n;
		return (!src[n]);
	}

	*pos = n;
	return false;
}

#endif // __KIVIJALKA_PRIVATE_PARSE_H__

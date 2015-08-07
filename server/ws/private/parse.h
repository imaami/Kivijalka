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
_parse_u8 (char         *str,
           unsigned int *pos,
           uint8_t      *dest)
{
	uint8_t v;
	unsigned int p = *pos;
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

	*pos = p;
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

		*pos = i;
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
	unsigned int i, p = *pos;
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

		*pos = i;
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

__attribute__((always_inline))
static inline bool
_parse_u64 (char         *str,
            unsigned int *pos,
            uint64_t     *dest)
{
	uint64_t v;
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

		*pos = i;
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
	unsigned int i, p = *pos;
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

		*pos = i;
		*dest = v;
		return true;
	}

	*dest = 0;
	return false;
}

#endif // __KIVIJALKA_PRIVATE_PARSE_H__

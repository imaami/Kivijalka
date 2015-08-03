#ifndef __KIVIJALKA_PRIVATE_ARGS_H__
#define __KIVIJALKA_PRIVATE_ARGS_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct args {
	struct __attribute__((gcc_struct,packed)) {
		uint32_t width;
		uint32_t height;
	} display;
	struct __attribute__((gcc_struct,packed)) {
		char *path;
	} banner_cache;
	struct __attribute__((gcc_struct,packed)) {
		char     *addr;
		uint16_t  port;
	} server;
} __attribute__((gcc_struct,packed));

#define ARGS_INIT {\
	.display = {\
		.width = 0,\
		.height = 0\
	},\
	.banner_cache = {\
		.path = NULL\
	},\
	.server = {\
		.addr = NULL,\
		.port = 0\
	}\
}

__attribute__((always_inline))
static inline bool
_u16_arg (char     *str,
          uint16_t *dest)
{
	uint16_t v;
	unsigned int i, c = str[0];

	switch (c) {
	case '1' ... '9':
		v = c - '0';
		for (i = 1; (c = str[i]);) {
			switch (c) {
			case '0' ... '9':
				v = (v << 1) + (v << 3) + (c - '0');

				if (++i < 4) {
					continue;
				}

				c = str[4];

				switch (c) {
				case '0' ... '9':
					if (v > 6553
					    || (v == 6553 && c > '5')) {
						*dest = UINT16_MAX;
						goto fail;
					}

					v = (v << 1) + (v << 3) + (c - '0');

				case '\0':
					goto done;

				default:
					break;
				}

			default:
				goto zero;
			}
		}

	done:
		*dest = v;
		return true;

	default:
	zero:
		*dest = 0;
	fail:
		return false;
	}
}

__attribute__((always_inline))
static inline bool
_u32_arg (char     *str,
          uint32_t *dest)
{
	uint32_t v;
	unsigned int i, c = str[0];

	switch (c) {
	case '1' ... '9':
		v = c - '0';
		for (i = 1; (c = str[i]);) {
			switch (c) {
			case '0' ... '9':
				v = (v << 1) + (v << 3) + (c - '0');

				if (++i < 9) {
					continue;
				}

				c = str[9];

				switch (c) {
				case '0' ... '9':
					if (v > 429496729
					    || (v == 429496729 && c > '5')) {
						*dest = UINT32_MAX;
						goto fail;
					}

					v = (v << 1) + (v << 3) + (c - '0');

				case '\0':
					goto done;

				default:
					break;
				}

			default:
				goto zero;
			}
		}

	done:
		*dest = v;
		return true;

	default:
	zero:
		*dest = 0;
	fail:
		return false;
	}
}

__attribute__((always_inline))
static inline bool
_args_parse (struct args  *a,
             int           argc,
             char        **argv)
{
	char *p;
	uint32_t *pu32;

	for (int i = 1; i < argc; ++i) {
		switch (*(p = argv[i])) {
			case '-':
				switch (*++p) {
				case 'a':
					if (*++p == '\0') {
						if (++i == argc) {
							return false;
						}
						a->server.addr = argv[i];
					} else {
						a->server.addr = p;
					}
					break;

				case 'b':
					if (*++p == '\0') {
						if (++i == argc) {
							return false;
						}
						a->banner_cache.path = argv[i];
					} else {
						a->banner_cache.path = p;
					}
					break;

				case 'p':
					if (*++p == '\0') {
						if (++i == argc) {
							return false;
						}
						p = argv[i];
					}
					if (!_u16_arg (p, &a->server.port)) {
						return false;
					}
					break;

				case 'h':
					pu32 = &a->display.height;
					goto parse_u32_arg;

				case 'w':
					pu32 = &a->display.width;
				parse_u32_arg:
					if (*++p == '\0') {
						if (++i == argc) {
							return false;
						}
						p = argv[i];
					}
					if (!_u32_arg (p, pu32)) {
						return false;
					}
					break;
				}
				break;

			default:
				return false;
		}
	}

	return true;
}

__attribute__((always_inline))
static inline uint32_t
_args_get_display_width (struct args *a)
{
	return a->display.width;
}

__attribute__((always_inline))
static inline uint32_t
_args_get_display_height (struct args *a)
{
	return a->display.height;
}

__attribute__((always_inline))
static inline const char *
_args_get_banner_cache_path (struct args *a)
{
	return (a->banner_cache.path)
	       ? (const char *) a->banner_cache.path
	       : "/usr/share/kivijalka/banners";
}

__attribute__((always_inline))
static inline const char *
_args_get_server_addr (struct args *a)
{
	return (a->server.addr) ? (const char *) a->server.addr : "0.0.0.0";
}

__attribute__((always_inline))
static inline uint16_t
_args_get_server_port (struct args *a)
{
	return (a->server.port > 0) ? a->server.port : 8001;
}

#endif // __KIVIJALKA_PRIVATE_ARGS_H__

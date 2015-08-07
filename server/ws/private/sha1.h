#ifndef __KIVIJALKA_PRIVATE_SHA1_H__
#define __KIVIJALKA_PRIVATE_SHA1_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../sha1.h"
#include "hex.h"

#include <openssl/sha.h>

__attribute__((always_inline))
static inline void
_sha1_init (sha1_t *hash)
{
	hash->u32[0] = 0;
	hash->u32[1] = 0;
	hash->u32[2] = 0;
	hash->u32[3] = 0;
	hash->u32[4] = 0;
}

__attribute__((always_inline))
static inline void
_sha1_gen (sha1_t  *hash,
           size_t   size,
           uint8_t *data)
{
	(void) SHA1 ((const unsigned char *) data, (unsigned long) size,
	             (unsigned char *) hash->u8);
}

__attribute__((always_inline))
static inline bool
_sha1_cmp (sha1_t *h1,
           sha1_t *h2)
{
	return (
		h1->u32[0] == h2->u32[0] &&
		h1->u32[1] == h2->u32[1] &&
		h1->u32[2] == h2->u32[2] &&
		h1->u32[3] == h2->u32[3] &&
		h1->u32[4] == h2->u32[4]
	);
}

__attribute__((always_inline))
static inline void
_sha1_cpy (sha1_t *src,
           sha1_t *dest)
{
	dest->u32[0] = src->u32[0];
	dest->u32[1] = src->u32[1];
	dest->u32[2] = src->u32[2];
	dest->u32[3] = src->u32[3];
	dest->u32[4] = src->u32[4];
}

__attribute__((always_inline))
static inline void
_sha1_str (sha1_t *hash,
           char   *dest)
{
	for (size_t i = 0, j = 0; i < 20; ++i, ++j) {
		uint8_t x = hash->u8[i];
		dest[j] = _hex_char (x >> 4);
		dest[++j] = _hex_char (x & 15);
	}
	dest[40] = '\0';
}

__attribute__((always_inline))
static inline bool
_sha1_parse (const char *src,
             sha1_t     *dest)
{
	unsigned int i = 0, j = 0;

	do {
		uint8_t c = src[i++], d;

		switch (c) {
		case '0' ... '9':
			d = c - '0';
			break;
		case 'A' ... 'F':
			d = 10 + (c - 'A');
			break;
		case 'a' ... 'f':
			d = 10 + (c - 'a');
			break;
		default:
			return false;
		}

		d <<= 4;
		c = src[i++];

		switch (c) {
		case '0' ... '9':
			d |= c - '0';
			break;
		case 'A' ... 'F':
			d |= 10 + (c - 'A');
			break;
		case 'a' ... 'f':
			d |= 10 + (c - 'a');
			break;
		default:
			return false;
		}

		dest->u8[j] = d;
	} while (++j < 20);

	return true;
}

#endif // __KIVIJALKA_PRIVATE_SHA1_H__

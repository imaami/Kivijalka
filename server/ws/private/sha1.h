#ifndef __KIVIJALKA_PRIVATE_SHA1_H__
#define __KIVIJALKA_PRIVATE_SHA1_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../sha1.h"

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
_sha1_str (sha1_t *hash,
           char   *dest)
{
	static const char hex[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};
	for (size_t i = 0, j = 0; i < 20; ++i, ++j) {
		uint8_t x = hash->u8[i];
		dest[j] = hex[x >> 4];
		dest[++j] = hex[x & 15];
	}
	dest[40] = '\0';
}

#endif // __KIVIJALKA_PRIVATE_SHA1_H__

#ifndef __KIVIJALKA_SHA1_H__
#define __KIVIJALKA_SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef union {
	uint32_t u32[5];
	uint16_t u16[10];
	uint8_t   u8[20];
} sha1_t;

#define SHA1_INITIALIZER {.u32 = {0, 0, 0, 0, 0}}

__attribute__((always_inline))
static inline void
sha1_init (sha1_t *hash)
{
	hash->u32[0] = 0;
	hash->u32[1] = 0;
	hash->u32[2] = 0;
	hash->u32[3] = 0;
	hash->u32[4] = 0;
}

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_SHA1_H__

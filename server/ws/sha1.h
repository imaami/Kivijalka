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

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_SHA1_H__

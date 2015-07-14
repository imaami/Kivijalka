#ifndef __KIVIJALKA_POINT_H__
#define __KIVIJALKA_POINT_H__

#ifdef __cplusplus
extern "C" {
#else
#include <stdatomic.h>
#endif

#include <stdint.h>

typedef union {
	uint64_t u64;
	#ifndef __cplusplus
	_Atomic uint64_t a64;
	#endif
	struct {
		int32_t x : 32;
		int32_t y : 32;
	} __attribute__((gcc_struct,packed));
} point_t;

#define POINT_INIT(a, b) { .x = (a), .y = (b) }

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_POINT_H__

#ifndef __KIVIJALKA_PRIVATE_POINT_H__
#define __KIVIJALKA_PRIVATE_POINT_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../point.h"

__attribute__((always_inline))
static inline void
_point_cpy (point_t *src,
            point_t *dest)
{
	dest->u64 = src->u64;
}

#endif // __KIVIJALKA_PRIVATE_POINT_H__

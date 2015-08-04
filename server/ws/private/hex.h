#ifndef __KIVIJALKA_PRIVATE_HEX_H__
#define __KIVIJALKA_PRIVATE_HEX_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../hex.h"

__attribute__((always_inline))
static inline char
_hex_char (unsigned int i)
{
	return HEX_CHAR[i];
}

#endif // __KIVIJALKA_PRIVATE_HEX_H__

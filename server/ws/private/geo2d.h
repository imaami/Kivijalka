#ifndef __KIVIJALKA_PRIVATE_GEO2D_H__
#define __KIVIJALKA_PRIVATE_GEO2D_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../geo2d.h"

struct geo2d {
	union {
		uint64_t u64;
		struct {
			uint32_t w : 32;
			uint32_t h : 32;
		} __attribute__((gcc_struct,packed));
	} __attribute__((packed));
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline void
_geo2d_init (struct geo2d *g)
{
	g->u64 = 0;
}

__attribute__((always_inline))
static inline void
_geo2d_set_width (struct geo2d  *g,
                  uint32_t       width)
{
	g->w = width;
}

__attribute__((always_inline))
static inline void
_geo2d_set_height (struct geo2d  *g,
                   uint32_t       height)
{
	g->h = height;
}

__attribute__((always_inline))
static inline void
_geo2d_set (struct geo2d  *g,
            uint32_t       width,
            uint32_t       height)
{
	_geo2d_set_width (g, width);
	_geo2d_set_height (g, height);
}

__attribute__((always_inline))
static inline uint32_t
_geo2d_width (struct geo2d *g)
{
	return g->w;
}

__attribute__((always_inline))
static inline uint32_t
_geo2d_height (struct geo2d *g)
{
	return g->h;
}

#endif // __KIVIJALKA_PRIVATE_GEO2D_H__

#ifndef __KIVIJALKA_GEO2D_H__
#define __KIVIJALKA_GEO2D_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct geo2d geo2d_t;

extern void
geo2d_init (geo2d_t *g);

extern void
geo2d_set_width (geo2d_t  *g,
                 uint32_t  width);

extern void
geo2d_set_height (geo2d_t  *g,
                  uint32_t  height);

extern void
geo2d_set (geo2d_t  *g,
           uint32_t  width,
           uint32_t  height);

extern uint32_t
geo2d_width (geo2d_t *g);

extern uint32_t
geo2d_height (geo2d_t *g);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_GEO2D_H__

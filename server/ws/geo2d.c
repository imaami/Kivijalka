#include "private/geo2d.h"

void
geo2d_init (geo2d_t *g)
{
	if (g) {
		_geo2d_init (g);
	}
}

void
geo2d_set_width (geo2d_t  *g,
                 uint32_t  width)
{
	if (g) {
		_geo2d_set_width (g, width);
	}
}

void
geo2d_set_height (geo2d_t  *g,
                  uint32_t  height)
{
	if (g) {
		_geo2d_set_height (g, height);
	}
}

void
geo2d_set (geo2d_t  *g,
           uint32_t  width,
           uint32_t  height)
{
	if (g) {
		_geo2d_set (g, width, height);
	}
}

uint32_t
geo2d_width (geo2d_t *g)
{
	return (g) ? _geo2d_width (g) : 0;
}

uint32_t
geo2d_height (geo2d_t *g)
{
	return (g) ? _geo2d_height (g) : 0;
}

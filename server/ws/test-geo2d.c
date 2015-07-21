#include "geo2d.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	uint64_t _g;
	geo2d_t *g = (geo2d_t *) &_g;
	geo2d_init (g);
	printf ("w=%u, h=%u\n", geo2d_width (g), geo2d_height (g));
	geo2d_set_width (g, 1280);
	geo2d_set_height (g, 1024);
	printf ("w=%u, h=%u\n", geo2d_width (g), geo2d_height (g));
	return EXIT_SUCCESS;
}

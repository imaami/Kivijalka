#include "display.h"

#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	display_t *d;
	if (!(d = display_create (1280, 1024))) {
		return EXIT_FAILURE;
	}
	display_destroy (d);
	d = NULL;
	return EXIT_SUCCESS;
}

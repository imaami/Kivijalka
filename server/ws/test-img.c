#include "img.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	if (argc < 2) {
		return EXIT_FAILURE;
	}

	img_t *im;

	if (!img_init (im)) {
		return EXIT_FAILURE;
	}

	if (img_load_screen (im, argv[1])) {
		return EXIT_FAILURE;
	}

	img_destroy (im);

	return EXIT_SUCCESS;
}

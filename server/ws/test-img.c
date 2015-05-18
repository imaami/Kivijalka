#include "img.h"
#include "read-file.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	if (argc < 3) {
		return EXIT_FAILURE;
	}

	char *pngdata;
	size_t pngsize;
	img_t im;

	if (!(pngdata = read_binary_file (argv[2], &pngsize))) {
		return EXIT_FAILURE;
	}

	if (!pngsize || !img_init (&im)) {
		return EXIT_FAILURE;
	}

	if (!img_load_screen (&im, argv[1])
	    || !img_load_banner (&im, pngdata, pngsize)) {
		return EXIT_FAILURE;
	}

	if (img_render_screen (&im, 0, 0)) {
		if (argc > 3 && argv[3]) {
			(void) img_write (&im, argv[3]);
		}
	}

	img_destroy (&im);
	free (pngdata);
	pngdata = NULL;

	return EXIT_SUCCESS;
}

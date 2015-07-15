#include "banner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int
main (int    argc,
      char **argv)
{
	banner_t *b;

	if (argc < 2 || !argv[1]) {
		return EXIT_FAILURE;
	}

	if (!(b = banner_create_from_path (argv[1]))) {
		return EXIT_FAILURE;
	}

	point_t offset = POINT_INIT(100, 200);
	banner_set_offset (b, offset);

	printf ("banner name is '%s'\n", banner_name (b));
	banner_print_hash (b);

	banner_destroy (&b);
	return EXIT_SUCCESS;
}

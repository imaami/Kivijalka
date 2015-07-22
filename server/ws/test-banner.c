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

	char str[41];
	banner_hash_unparse (b, str);
	printf ("banner name='%s'\n       hash=%s\n", banner_name (b), str);

	banner_destroy (&b);
	return EXIT_SUCCESS;
}

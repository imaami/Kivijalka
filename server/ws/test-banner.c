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

	if (!(b = banner_create ())) {
		fprintf (stderr, "failed to create banner object\n");
		return EXIT_FAILURE;
	}

	point_t offset = POINT_INIT(100, 200);
	banner_set_offset (b, offset);

	if (argc > 1 && argv[1]) {
		if (!banner_set_name (b, (const char *) argv[1])) {
			fprintf (stderr, "failed to set banner name\n");
		}
	}

	char str[41];
	banner_hash_unparse (b, str);
	printf ("banner name='%s'\n       hash=%s\n", banner_name (b), str);

	banner_destroy (&b);
	return EXIT_SUCCESS;
}

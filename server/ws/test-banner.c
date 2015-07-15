#include "banner.h"
#include "sha1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int
main (int    argc,
      char **argv)
{
	banner_t *b;
	if (argc > 1 && argv[1]) {
		if (!(b = banner_create_from_path (argv[1]))) {
			return EXIT_FAILURE;
		}
	} else {
		if (!(b = banner_create ())) {
			return EXIT_FAILURE;
		}
		banner_set_name (b, "testibanneri");
	}
	printf ("banner name is '%s'\n", banner_name (b));
	sha1_t hash;
	sha1_gen (&hash, strlen (banner_name (b)),
	          (uint8_t *) banner_name (b));
	point_t offset = POINT_INIT(100, 200);
	banner_set_offset (b, offset);
	banner_set_hash (b, hash);
	banner_print_hash (b);
	banner_destroy (&b);
	return EXIT_SUCCESS;
}

#include "banner.h"

#include <stdio.h>
#include <stdlib.h>

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
	sha1_t hash = {.u32 = {3999721,4023425,3938532,2227354,1124725}};
	point_t offset = POINT_INIT(100, 200);
	banner_set_offset (b, offset);
	banner_set_hash (b, hash);
	banner_print_hash (b);
	banner_destroy (&b);
	return EXIT_SUCCESS;
}

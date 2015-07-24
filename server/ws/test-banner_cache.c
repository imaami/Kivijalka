#include "banner_cache.h"
#include "banner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int
main (int    argc,
      char **argv)
{
	banner_cache_t *bc;
	banner_t *b;
	char *json;

	if (argc < 2 || !argv[1]) {
		return EXIT_FAILURE;
	}

	if (!(bc = banner_cache_create ("/"))) {
		return EXIT_FAILURE;
	}

	for (unsigned int i = 1; i < argc; ++i) {
		if ((b = banner_create_from_path (argv[i]))
		    && !banner_cache_add_banner (bc, b)) {
			banner_destroy (&b);
		}
	}

	if ((json = banner_cache_json (bc))) {
		puts (json);
		free (json);
	}

	banner_cache_destroy (&bc);
	return EXIT_SUCCESS;
}

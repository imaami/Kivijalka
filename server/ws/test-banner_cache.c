#include "banner_cache.h"
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
	banner_cache_t *bc;
	banner_t *b;
	sha1_t hash;

	if (argc < 2 || !argv[1]) {
		return EXIT_FAILURE;
	}

	if (!(bc = banner_cache_create ("/"))) {
		return EXIT_FAILURE;
	}
	if (!(b = banner_create_from_path (argv[1]))) {
		banner_cache_destroy (&bc);
		return EXIT_FAILURE;
	}

	fputs ("loaded banner: ", stdout);
	banner_print_hash (b);

	if (!banner_cache_add_banner (bc, b)) {
		fprintf (stderr, "failed to add banner\n");
	} else {
		puts ("added banner to cache");
		banner_hash_cpy (b, &hash);
		b = NULL;
		banner_t *b2 = banner_cache_find_banner (bc, &hash);
		if (!b2) {
			fprintf (stderr, "failed to find banner\n");
		} else {
			fputs ("found banner: ", stdout);
			banner_print_hash (b2);
			puts ("trying to incorrectly add duplicate banner...");
			if (!banner_cache_add_banner (bc, b2)) {
				puts ("banner_cache_add_banner() refused our stupid request, things seem to work");
			} else {
				puts ("whoops! banner_cache_add_banner() seem to be broken");
			}
		}
	}

	banner_cache_destroy (&bc);
	return EXIT_SUCCESS;
}

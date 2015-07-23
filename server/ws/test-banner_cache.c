#include "banner_cache.h"
#include "banner.h"
#include "sha1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <uuid/uuid.h>

int
main (int    argc,
      char **argv)
{
	banner_cache_t *bc;
	banner_t *b, *b2;
	sha1_t hash;
	uuid_t uuid;
	char str[41];

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

	banner_hash_unparse (b, str);
	printf ("created banner, SHA1=%s\n", str);

	if (!banner_cache_add_banner (bc, b)) {
		fprintf (stderr, "failed to add banner\n");
		banner_destroy (&b);
	} else {
		banner_uuid_cpy (b, uuid);
		uuid_unparse_lower (uuid, str);
		printf ("cached banner, UUID=%s\n", str);
		banner_hash_cpy (b, &hash);
		b = NULL;
		puts ("testing whether the most recent banner is the correct one...");
		b2 = banner_cache_most_recent (bc);
		if (b2 && banner_hash_cmp (b2, &hash)) {
			puts ("yes it is, the hashes appear to match");
		} else {
			fprintf (stderr, "failed to retrieve most recently added banner\n");
		}
		fputs ("testing whether banner_cache_find_by_uuid() works...", stdout);
		b2 = banner_cache_find_by_uuid (bc, uuid);
		printf (" it does%s", (b2) ? "\n" : "n't!\n");
		fputs ("testing whether banner_cache_find_by_hash() works...", stdout);
		b2 = banner_cache_find_by_hash (bc, &hash);
		if (!b2) {
			puts (" it doesn't!");
		} else {
			puts (" it does");
			banner_hash_unparse (b2, str);
			printf ("found banner, SHA1=%s\ntrying to incorrectly add duplicate banner...\n", str);
			if (!banner_cache_add_banner (bc, b2)) {
				puts ("banner_cache_add_banner() refused our stupid request, things seem to work");
			} else {
				puts ("whoops! banner_cache_add_banner() seem to be broken");
			}
		}
	}

	for (unsigned int i = 2; i < argc; ++i) {
		if ((b = banner_create_from_path (argv[i]))) {
			banner_hash_unparse (b, str);
			printf ("created banner, SHA1=%s\n", str);
			if (!banner_cache_add_banner (bc, b)) {
				fprintf (stderr, "failed to add banner\n");
				banner_destroy (&b);
			} else {
				banner_uuid_cpy (b, uuid);
				uuid_unparse_lower (uuid, str);
				printf ("cached banner, UUID=%s\n", str);
			}
		}
	}

	banner_cache_destroy (&bc);
	return EXIT_SUCCESS;
}

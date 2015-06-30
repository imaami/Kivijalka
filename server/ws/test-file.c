#include "file.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int
main (int    argc,
      char **argv)
{
	if (argc < 2 || !argv || !argv[1]) {
		fprintf (stderr, "%s: invalid arguments\n", __func__);
		return EXIT_FAILURE;
	}

	size_t size = (size_t) 1 << 24;
	uint8_t *data = malloc (size);

	if (!data) {
		fprintf (stderr, "%s: malloc: %s\n",
		         __func__, strerror (errno));
		return EXIT_FAILURE;
	}

	printf ("%s: allocated buffer of %zu B\n", __func__, size);

	size_t count;
	bool r = file_read (argv[1], size, data, &count);

	printf ("%s: file_read returned %s, count==%zu\n",
	        __func__, r ? "true" : "false", count);

	// let's print out the buffer without regard to whether it
	// contains binary data, because what could possibly go wrong?
	if (r) {
		(void) fputs ((const char *) data, stdout);
	}

	free (data);
	data = NULL;

	return EXIT_SUCCESS;
}

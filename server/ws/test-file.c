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
	if (argc < 3 || !argv || !argv[1] || !argv[2]) {
		fprintf (stderr, "%s: invalid arguments\n", __func__);
		return EXIT_FAILURE;
	}

	size_t size = (size_t) 1 << 24;
	uint8_t *data = malloc (size);
	file_t *f;
	size_t count;

	if (!data) {
		fprintf (stderr, "%s: malloc: %s\n",
		         __func__, strerror (errno));
		return EXIT_FAILURE;
	}

	printf ("%s: allocated buffer of %zu B\n", __func__, size);

	if (!(f = file_create (argv[1]))) {
		goto fail_file_create;
	}

	printf ("%s: created file object for path '%s'\n",
	        __func__, file_path (f));

	if (!file_open (f, "rb")) {
		fprintf (stderr, "%s: file_open failed\n", __func__);
	} else {
		printf ("%s: opened '%s'\n"
		        "%s: testing file_open by attempting to reopen "
		        "the same file...\n",
		        __func__, file_path (f), __func__);

		if (!file_open (f, "rb")) {
			printf ("%s: file_open failed; this is a good "
			        "thing because that's what was supposed "
			        "to happen here\n", __func__);
		}

		bool r = file_read (f, size, data, &count);
		printf ("%s: file_read returned %s, count==%zu\n",
			__func__, r ? "true" : "false", count);

		if (!file_close (f)) {
			fprintf (stderr, "%s: file_close failed\n",
			         __func__);
		}
	}

	file_destroy (&f);

	if (!(f = file_create (argv[2]))) {
	fail_file_create:
		fprintf (stderr, "%s: file_create failed\n", __func__);
		free (data);
		data = NULL;
		return EXIT_FAILURE;
	}

	printf ("%s: created file object for path '%s'\n",
	        __func__, file_path (f));

	if (!file_open (f, "wb")) {
		fprintf (stderr, "%s: file_open failed\n", __func__);
	} else {
		printf ("%s: opened '%s'\n", __func__, file_path (f));

		if (!file_write (f, count, data)) {
			fprintf (stderr, "%s: file_write failed\n",
			         __func__);
		}

		if (!file_close (f)) {
			fprintf (stderr, "%s: file_close failed\n",
			         __func__);
		}
	}

	file_destroy (&f);

	free (data);
	data = NULL;

	return EXIT_SUCCESS;
}

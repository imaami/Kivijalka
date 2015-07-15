#include "sha1.h"
#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

int
main (int    argc,
      char **argv)
{
	sha1_t hash;
	file_t *f;
	size_t size;
	uint8_t *data;
	size_t count;

	if (argc < 2 || !argv[1]) {
		fprintf (stderr, "Usage: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!(f = file_create (argv[1]))) {
		fprintf (stderr, "%s: file_create failed\n", __func__);
		goto fail;
	}

	if (!file_open (f, "rb")) {
		fprintf (stderr, "%s: file_open failed\n", __func__);
		goto fail_destroy_file;
	}

	if (!file_size (f, &size)) {
		printf ("%s: file_size failed\n", __func__);
		goto fail_close_destroy_file;
	}

	if (size < 40) {
		size = 40;
	}

	if (!(data = aligned_alloc (64, size + 1))) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
		goto fail_close_destroy_file;
	}

	count = 0;
	if (!file_read (f, size + 1, data, &count)) {
		if (count > 0) {
			fprintf (stderr, "%s: file_read failed: buffer "
			         "needs to be at least %zu B\n",
			         __func__, count);
			count = 0;
		} else {
			fprintf (stderr, "%s: file_read failed\n", __func__);
		}
		free (data);
		data = NULL;
		size = 0;
	fail_close_destroy_file:
		(void) file_close (f);
	fail_destroy_file:
		file_destroy (&f);
	fail:
		return EXIT_FAILURE;
	}

	if (!file_close (f)) {
		fprintf (stderr, "%s: file_close failed\n", __func__);
	}

	file_destroy (&f);

	sha1_gen (&hash, count, data);
	sha1_str (&hash, (char *) data);

	printf ("%s  %s\n", data, argv[1]);

	free (data);
	data = NULL;
	size = 0;
	count = 0;

	return EXIT_SUCCESS;
}

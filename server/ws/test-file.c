#include "file.h"
#include "buf.h"

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

	file_t *f;
	size_t size;
	buf_t *buf;
	uint8_t *data;
	size_t count;

	if (!(f = file_create (argv[1]))) {
		goto fail_file_create;
	}

	printf ("%s: created file object for path '%s'\n",
	        __func__, file_path (f));

	if (!file_open (f, "rb")) {
		fprintf (stderr, "%s: file_open failed\n", __func__);
		goto fail_destroy_file;
	}

	printf ("%s: opened '%s'\n", __func__, file_path (f));

	if (!file_size (f, &size)) {
		printf ("%s: file_size failed\n", __func__);
		goto fail_close_destroy_file;
	}

	printf ("%s: file size is %zu B, trying to create buffer\n",
	        __func__, size);

	if (!(buf = buf_create (size + 1))) {
		fprintf (stderr, "%s: buf_create failed\n", __func__);
		goto fail_close_destroy_file;
	}

	printf ("%s: created buffer of %zu B, trying to reserve %zu B\n",
	        __func__, buf_size (buf), size + 1);

	if (!(data = buf_alloc (buf, size + 1))) {
		fprintf (stderr, "%s: buf_alloc failed\n", __func__);
		goto fail_destroy_buf;
	}

	count = 0;
	if (!file_read (f, size + 1, data, &count)) {
		if (count > 0) {
			fprintf (stderr, "%s: file_read failed: buffer "
			         "needs to be at least %zu B\n",
			         __func__, count);
		} else {
			fprintf (stderr, "%s: file_read failed\n", __func__);
		}
	fail_destroy_buf:
		buf_destroy (&buf);
	fail_close_destroy_file:
		(void) file_close (f);
	fail_destroy_file:
		file_destroy (&f);
		return EXIT_FAILURE;
	}

	printf ("%s: read %zu B\n", __func__, count);

	if (!file_close (f)) {
		fprintf (stderr, "%s: file_close failed\n", __func__);
	}

	file_destroy (&f);

	if (!(f = file_create (argv[2]))) {
	fail_file_create:
		fprintf (stderr, "%s: file_create failed\n", __func__);
		buf_destroy (&buf);
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
	buf_destroy (&buf);

	return EXIT_SUCCESS;
}

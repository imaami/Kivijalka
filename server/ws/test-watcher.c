#include "watcher.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	if (argc < 2 || !argv[1]) {
		printf ("Usage: %s PATH\n", argv[0]);
		return EXIT_FAILURE;
	}

	watcher_t *w;

	if (!(w = watcher_create ((const char *) argv[1]))) {
		fprintf (stderr, "%s: watcher_create failed\n", __func__);
		return EXIT_FAILURE;
	}

	if (!watcher_prepare (w)) {
		fprintf (stderr, "%s: watcher_prepare failed\n", __func__);
	} else {
		watcher_start (w);
		for (unsigned int x = 0; x < 4;) {
			if (watcher_wait (w)) {
				++x;
				printf ("%s: got watcher event\n", __func__);
			}
		}
		watcher_stop (w);
	}

	watcher_destroy (w);

	return EXIT_SUCCESS;
}

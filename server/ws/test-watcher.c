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

	if ((w = watcher_create ((const char *) argv[1]))) {
		int64_t r;
		do {
			if ((r = watcher_run_once (w)) > 0) {
				printf ("%s: got watcher event\n", __func__);
				r = 0;
			}
		} while (!r);
		watcher_destroy (w);
	}

	return EXIT_SUCCESS;
}

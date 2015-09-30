/* Copyright © 2015 Koneet Kiertoon and Juuso Alasuutari.
 * Written by Juuso Alasuutari.
 *
 * This file is part of Kivijalka.
 *
 * Kivijalka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kivijalka is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kivijalka.  If not, see <http://www.gnu.org/licenses/>.
 */
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

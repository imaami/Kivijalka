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
#include "banner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int
main (int    argc,
      char **argv)
{
	banner_t *b;

	if (!(b = banner_create ())) {
		fprintf (stderr, "failed to create banner object\n");
		return EXIT_FAILURE;
	}

	point_t offset = POINT_INIT(100, 200);
	banner_set_offset (b, offset);

	if (argc > 1 && argv[1]) {
		if (!banner_set_name (b, (const char *) argv[1])) {
			fprintf (stderr, "failed to set banner name\n");
		}
	}

	char str[41];
	banner_hash_unparse (b, str);
	printf ("banner name='%s'\n       hash=%s\n", banner_name (b), str);

	banner_destroy (&b);
	return EXIT_SUCCESS;
}

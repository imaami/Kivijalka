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
#include "path.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	if (argv[1]) {
		path_head_t path;
		char *str;

		path_create (&path, argv[1]);

		if ((str = path_strdup (&path))) {
			printf ("%s (%zu)\n", str, path_strlen (&path));
			free (str);
		}

		path_destroy (&path);
	}
	return 0;
}

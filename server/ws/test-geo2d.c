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
#include "geo2d.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	uint64_t _g, _g2;
	geo2d_t *g = (geo2d_t *) &_g, *g2 = (geo2d_t *) &_g2;
	geo2d_init (g);
	printf ("w=%u, h=%u\n", geo2d_width (g), geo2d_height (g));
	geo2d_set_width (g, 1280);
	geo2d_set_height (g, 1024);
	geo2d_cpy (g, g2);
	printf ("w=%u, h=%u\n", geo2d_width (g2), geo2d_height (g2));
	return EXIT_SUCCESS;
}

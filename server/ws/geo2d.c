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
#include "private/geo2d.h"

void
geo2d_init (struct geo2d *g)
{
	if (g) {
		_geo2d_init (g);
	}
}

void
geo2d_set_width (struct geo2d  *g,
                 uint32_t       width)
{
	if (g) {
		_geo2d_set_width (g, width);
	}
}

void
geo2d_set_height (struct geo2d  *g,
                  uint32_t       height)
{
	if (g) {
		_geo2d_set_height (g, height);
	}
}

void
geo2d_set (struct geo2d  *g,
           uint32_t       width,
           uint32_t       height)
{
	if (g) {
		_geo2d_set (g, width, height);
	}
}

uint32_t
geo2d_width (struct geo2d *g)
{
	return (g) ? _geo2d_width (g) : 0;
}

uint32_t
geo2d_height (struct geo2d *g)
{
	return (g) ? _geo2d_height (g) : 0;
}

void
geo2d_cpy (struct geo2d *g,
           struct geo2d *dest)
{
	if (g && dest) {
		_geo2d_cpy (g, dest);
	}
}

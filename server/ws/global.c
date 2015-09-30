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
/** \file global.c
 *
 * Global variables.
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "global.h"

size_t thumb_w, thumb_h;
sem_t process_sem;
img_file_t capture_file, banner_file, output_file, thumb_file;
banner_t *cur_banner;

void
global_init (size_t thumb_width,
             size_t thumb_height)
{
	thumb_w = thumb_width;
	thumb_h = thumb_height;
	if (sem_init (&process_sem, 0, 0)) {
		fprintf (stderr, "%s: sem_init failed: %s\n",
		                 __func__, strerror (errno));
	}
	img_file_init (&capture_file);
	img_file_init (&banner_file);
	img_file_init (&output_file);
	img_file_init (&thumb_file);
	cur_banner = NULL;
}

void
global_fini (void)
{
	cur_banner = NULL;
	img_file_fini (&thumb_file);
	img_file_fini (&output_file);
	img_file_fini (&banner_file);
	img_file_fini (&capture_file);
	if (sem_destroy (&process_sem)) {
		fprintf (stderr, "%s: sem_destroy failed: %s\n",
		                 __func__, strerror (errno));
	}
	thumb_h = 0;
	thumb_w = 0;
}

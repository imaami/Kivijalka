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
}

void
global_fini (void)
{
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

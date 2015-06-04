/** \file global.c
 *
 * Global variables.
 */

#include <errno.h>
#include <string.h>

#include "global.h"

size_t banner_x, banner_y, thumb_w, thumb_h;
sem_t process_sem;
img_file_t capture_file, banner_file, output_file, thumb_file;
img_t img;

void
global_init (size_t bx,
             size_t by,
             size_t tw,
             size_t th)
{
	banner_x = bx;
	banner_y = by;
	thumb_w = tw;
	thumb_h = th;
	if (sem_init (&process_sem, 0, 0)) {
		fprintf (stderr, "%s: sem_init failed: %s\n",
		                 __func__, strerror (errno));
	}
	img_file_init (&capture_file);
	img_file_init (&banner_file);
	img_file_init (&output_file);
	img_file_init (&thumb_file);
	(void) img_init (&img);
}

void
global_fini (void)
{
	img_destroy (&img);
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
	banner_y = 0;
	banner_x = 0;
}

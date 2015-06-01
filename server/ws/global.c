/** \file global.c
 *
 * Global variables.
 */

#include "global.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

sem_t capture_sem, process_sem, output_sem;
img_file_t capture_file, banner_file, output_file, thumb_file;
img_t img;

void
global_init (void)
{
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
}

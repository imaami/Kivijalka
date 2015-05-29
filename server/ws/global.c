/** \file global.c
 *
 * Global variables.
 */

#include "global.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

sem_t capture_sem, process_sem, output_sem;
const char *capture_file, *output_file;
char *capture_data, *banner_data, *output_data, *thumb_data;
size_t capture_size, banner_size, output_size, thumb_size;
img_t img;

bool
global_init (void)
{
	capture_file = NULL;
	output_file = NULL;
	capture_data = NULL;
	banner_data = NULL;
	output_data = NULL;
	thumb_data = NULL;
	capture_size = 0;
	banner_size = 0;
	output_size = 0;
	thumb_size = 0;
	(void) img_init (&img);
	if (sem_init (&capture_sem, 0, 0)
	    || sem_init (&process_sem, 0, 0)
	    || sem_init (&output_sem, 0, 0)) {
		fprintf (stderr, "%s: sem_init failed: %s\n",
		                 __func__, strerror (errno));
		return false;
	}
	return true;
}

void
global_fini (void)
{
	(void) sem_destroy (&capture_sem);
	(void) sem_destroy (&process_sem);
	(void) sem_destroy (&output_sem);
	img_destroy (&img);
	if (capture_file) {
		free ((void *) capture_file);
		capture_file = NULL;
	}
	if (output_file) {
		free ((void *) output_file);
		output_file = NULL;
	}
	if (capture_data) {
		free (capture_data);
		capture_data = NULL;
	}
	if (banner_data) {
		free (banner_data);
		banner_data = NULL;
	}
	if (output_data) {
		free (output_data);
		output_data = NULL;
	}
	if (thumb_data) {
		free (thumb_data);
		thumb_data = NULL;
	}
	capture_size = 0;
	banner_size = 0;
	output_size = 0;
	thumb_size = 0;
}

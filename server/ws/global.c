/** \file global.c
 *
 * Global variables.
 */

#include "global.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

sem_t capture_sem, process_sem, output_sem;

bool
global_init (void)
{
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
}

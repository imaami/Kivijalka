/** \file img_file.c
 *
 * Image manipulation.
 */

#define _GNU_SOURCE

#include "img_file.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

void
img_file_init (img_file_t *imf)
{
	if (imf) {
		if (sem_init (&(imf->lock), 0, 0)) {
			fprintf (stderr, "%s: sem_init failed: %s\n",
			                 __func__, strerror (errno));
			abort();
		}
		imf->path = NULL;
		imf->data = NULL;
	}
}

void
img_file_fini (img_file_t *imf)
{
	if (imf) {
		if (imf->data) {
			img_data_t *imd;
			if (img_file_steal_data (imf, &imd)) {
				img_data_free (imd);
			}
			imd = NULL;
		}
		if (imf->path) {
			free ((void *) imf->path);
			imf->path = NULL;
		}
		if (sem_destroy (&(imf->lock))) {
			fprintf (stderr, "%s: sem_destroy failed: %s\n",
			                 __func__, strerror (errno));
		}
	}
}

bool
img_file_wait (img_file_t *imf)
{
	if (imf) {
		for (;;) {
			if (!sem_wait (&(imf->lock))) {
				return true;
			} else if (errno != EINTR) {
				fprintf (stderr, "%s: sem_wait failed: %s\n",
				                 __func__, strerror (errno));
				break;
			}
		}
	}
	return false;
}

bool
img_file_post (img_file_t *imf)
{
	if (sem_post (&(imf->lock))) {
		fprintf (stderr, "%s: sem_post failed: %s\n",
		                 __func__, strerror (errno));
		return false;
	}
	return true;
}

bool
img_file_set_path (img_file_t *imf,
                   const char *path)
{
	if (imf && path) {
		if (imf->path) {
			free ((void *) imf->path);
		}
		if ((imf->path = (const char *) strdup (path))) {
			return true;
		}
		fprintf (stderr, "%s: strdup failed: %s\n",
		                 __func__, strerror (errno));
	}
	return false;
}

bool
img_file_swap_data (img_file_t  *imf,
                    img_data_t  *imd,
                    img_data_t **old)
{
	if (imf && old) {
		volatile img_data_t *_Atomic *ptr = (volatile img_data_t *_Atomic *) &imf->data;
		volatile img_data_t *_Atomic *_old = (volatile img_data_t *_Atomic *) old;
		volatile img_data_t *_Atomic cur;
		do {
			cur = atomic_load (ptr);
		} while (!atomic_compare_exchange_weak (ptr, &cur, imd));
		*_old = cur;
		return true;
	}
	return false;
}

bool
img_file_replace_data (img_file_t *imf,
                       img_data_t *imd)
{
	img_data_t *old;
	if (img_file_swap_data (imf, imd, &old)) {
		img_data_free (old);
		old = NULL;
		return true;
	}
	return false;
}

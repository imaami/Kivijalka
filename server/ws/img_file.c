/** \file img_file.c
 *
 * Image manipulation.
 */

#define _GNU_SOURCE

#include "img_file.h"

#include <stdio.h>
#include <stdlib.h>
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

/**
 * Atomically grab the img_data_t pointer from an img_file_t object and
 * replace it with a new one, and write the replaced pointer into \a old.
 * This function provides a means for any thread accessing the img_file_t
 * object to be certain that it is the sole owner of the retrieved pointer.
 * Notice, however, that this implies that the thread replacing the img_data_t
 * pointer must also free the retrieved old pointer later on.
 *
 * @param imf Pointer to the img_file_t object whose img_data_t pointer
 *            to replace.
 * @param imd New pointer to replace the previous one with. Can be NULL.
 * @return Old pointer.
 */
__attribute__((always_inline))
static inline img_data_t *
img_file_swap_data (img_file_t *imf,
                    img_data_t *imd)
{
	return atomic_exchange (&imf->data, imd);
}

img_data_t *
img_file_steal_data (img_file_t *imf)
{
	return img_file_swap_data (imf, NULL);
}

void
img_file_replace_data (img_file_t *imf,
                       img_data_t *imd)
{
	img_data_t *old;
	if ((old = img_file_swap_data (imf, imd))) {
		free (old);
	}
	old = NULL;
}

void
img_file_fini (img_file_t *imf)
{
	if (imf) {
		if (imf->data) {
			img_file_replace_data (imf, NULL);
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
	for (;;) {
		if (!sem_wait (&(imf->lock))) {
			return true;
		} else if (errno != EINTR) {
			fprintf (stderr, "%s: sem_wait failed: %s\n",
			                 __func__, strerror (errno));
			break;
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
img_file_update (img_file_t *imf)
{
	img_data_t *imd;

	if (!(imd = img_data_new_from_file (imf->path))) {
		fprintf (stderr, "%s: failed to read file\n", __func__);
		return false;
	}

	if (imd->size > 0) {
		img_file_replace_data (imf, imd);
		imd = NULL;
		return true;
	}

	fprintf (stderr, "%s: file empty\n", __func__);
	img_data_free (imd);
	imd = NULL;
	return false;
}

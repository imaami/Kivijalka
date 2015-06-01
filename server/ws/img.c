/** \file img.c
 *
 * Image manipulation.
 */

#define _GNU_SOURCE

#include "img.h"
#include "read-file.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define img_exception(_w) { \
	char *_d; \
	ExceptionType _s; \
	_d = MagickGetException (_w, &_s); \
	(void) fprintf (stderr, "%s %s %lu %s\n", \
	                GetMagickModule(), _d); \
	_d = (char *) MagickRelinquishMemory(_d); \
}

bool
img_init (img_t *im)
{
	MagickWandGenesis();

	if (im) {
		im->layers[0] = NewMagickWand();
		im->layers[1] = NewMagickWand();
		im->layers[2] = NewMagickWand();
		return true;
	}

	return false;
}

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
		imf->size = 0;
		imf->data = NULL;
	}
}

void
img_file_fini (img_file_t *imf)
{
	if (imf) {
		if (imf->data) {
			free (imf->data);
			imf->data = NULL;
		}
		imf->size = 0;
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
img_file_set_data (img_file_t *imf,
                   size_t      size,
                   char       *data)
{
	if (imf && size > 0 && data) {
		if (imf->data) {
			free (imf->data);
		}
		imf->size = size;
		imf->data = data;
		return true;
	}
	return false;
}

bool
img_file_read_data (img_file_t *imf)
{
	if (imf && imf->path) {
		if (imf->data) {
			free (imf->data);
		}
		if ((imf->data = read_binary_file (imf->path, &(imf->size)))) {
			return true;
		}
		imf->size = 0;
		fprintf (stderr, "%s: read_binary_file failed\n", __func__);
	}
	return false;
}

bool
img_load_file (img_t        *im,
               unsigned int  layer,
               const char   *path)
{
	MagickWand *w = im->layers[layer];

	ClearMagickWand (w);

	if (MagickReadImage (w, path) == MagickFalse) {
		img_exception (w);
		return false;
	}

	printf ("loaded image file\n");

	return true;
}

size_t
img_get_width (img_t    *im,
               unsigned  int layer)
{
	return (im) ? MagickGetImageWidth (im->layers[layer]) : 0;
}

size_t
img_get_height (img_t    *im,
                unsigned  int layer)
{
	return (im) ? MagickGetImageHeight (im->layers[layer]) : 0;
}

bool
img_load_data (img_t        *im,
               unsigned int  layer,
               const char   *data,
               const size_t  size)
{
	MagickWand *w = im->layers[layer];

	ClearMagickWand (w);

	if (MagickReadImageBlob (w, (const void *) data, size) == MagickFalse) {
		img_exception (w);
		return false;
	}

	printf ("loaded image buffer\n");

	return true;
}

bool
img_file_import_layer (img_file_t   *imf,
                       img_t        *im,
                       unsigned int  layer)
{
	if (imf && im) {
		MagickWand *w = im->layers[layer];

		if (imf->data) {
			free (imf->data);
			imf->data = NULL;
			imf->size = 0;
		}

		MagickResetIterator (w);

		if (MagickSetImageFormat (w, "PNG") == MagickTrue
		    && (imf->data = (char *) MagickGetImageBlob (w, &(imf->size)))) {
			return true;
		}
	}

	return false;
}

bool
img_render (img_t         *im,
            const ssize_t  x,
            const ssize_t  y)
{
	if (im) {
		MagickWand *l0 = im->layers[0], *l1 = im->layers[1], *l2 = im->layers[2];

		// TODO: clone l1 to l0

		if (MagickCompositeImage (l0, l2, OverCompositeOp, x, y) == MagickTrue) {
			printf ("composited image\n");
			return true;
		}

		img_exception (l0);
	}

	return false;
}

bool
img_scale (img_t        *im,
           unsigned int  layer,
           const size_t  width,
           const size_t  height)
{
	if (im) {
		MagickWand *w = im->layers[layer];

		if (MagickScaleImage (w, width, height) == MagickTrue) {
			printf ("scaled image\n");
			return true;
		}

		img_exception (w);
	}

	return false;
}

bool
img_write (img_t        *im,
           unsigned int  layer,
           const char   *file)
{
	if (!im) {
		return false;
	}

	MagickWand *w = im->layers[layer];

	if (MagickWriteImage (w, file) == MagickFalse) {
		img_exception (w);
		return false;
	}

	printf ("wrote image file\n");

	return true;
}

void
img_destroy (img_t *im)
{
	if (im) {
		MagickWand *l0 = im->layers[0], *l1 = im->layers[1], *l2 = im->layers[2];

		if (l2) {
			l2 = DestroyMagickWand (l2);
		}
		l2 = NULL;

		if (l1) {
			l1 = DestroyMagickWand (l1);
		}
		l1 = NULL;

		if (l0) {
			l0 = DestroyMagickWand (l0);
		}
		l0 = NULL;
	}

	MagickWandTerminus();
}

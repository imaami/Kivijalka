/** \file img.c
 *
 * Image manipulation.
 */

#define _GNU_SOURCE

#include "img.h"

#include <stdio.h>

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
		im->thumb.data = NULL;
		im->thumb.size = 0;
		im->layers[0] = NewMagickWand();
		im->layers[1] = NewMagickWand();
		im->layers[2] = NewMagickWand();
		return true;
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
img_export (img_t         *im,
            unsigned int   layer,
            char         **buf,
            size_t        *len)
{
	MagickWand *w = im->layers[layer];
	unsigned char *b;
	size_t l;
	bool r;

	MagickResetIterator (w);

	if (MagickSetImageFormat (w, "PNG") == MagickFalse
	    || !(b = MagickGetImageBlob (w, &l))) {
		*buf = NULL;
		*len = 0;
		r = false;
	} else {
		*buf = (char *) b;
		*len = l;
		r = true;
		printf ("exported image data\n");
	}

	b = NULL;
	l = 0;

	return r;
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

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
	if (im) {
		MagickWandGenesis();
		im->screen = NewMagickWand();
		im->banner = NewMagickWand();
	}
}

bool
img_load_screen (img_t      *im,
                 const char *path)
{
	ClearMagickWand (im->screen);

	if (MagickReadImage (im->screen, path) == MagickFalse) {
		img_exception (im->screen);
		return false;
	}

	return true;
}

bool
img_load_banner (img_t         *im,
                 const uint8_t *data,
                 const size_t   size)
{
	MagickBooleanType r;

	ClearMagickWand (im->banner);

	r = MagickReadImageBlob (im->banner,
				 (const void *) data,
				 size);
	if (r == MagickFalse) {
		img_exception (im->banner);
		return false;
	}

	return true;
}

bool
img_render_screen (img_t         *im,
                   const ssize_t  banner_x,
                   const ssize_t  banner_y)
{
	if (!im) {
		return false;
	}

	MagickBooleanType r;

	r = MagickCompositeImage (im->screen, im->banner,
	                          OverCompositeOp,
	                          banner_x, banner_y);
	if (r == MagickFalse) {
		img_exception (im->screen);
		return false;
	}

	return true;
}


void
img_destroy (img_t *im)
{
	if (im) {
		if (im->banner) {
			im->banner = DestroyMagickWand(im->banner);
		}
		if (im->screen) {
			im->screen = DestroyMagickWand(im->screen);
		}
		MagickWandTerminus();
		free (im);
	}
}

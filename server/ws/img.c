/** \file img.c
 *
 * Image manipulation.
 */

#define _GNU_SOURCE

#include "img.h"

#include <stdio.h>
#include <wand/MagickWand.h>

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
		im->screen = (void *) NewMagickWand();
		im->banner = (void *) NewMagickWand();
	}
}

bool
img_load_screen (img_t      *im,
                 const char *path)
{
	ClearMagickWand ((MagickWand *) im->screen);

	if (MagickReadImage ((MagickWand *) im->screen, path)
	    == MagickFalse) {
		img_exception ((MagickWand *) im->screen);
		return false;
	}

	return true;
}

bool
img_load_banner (img_t         *im,
                 const uint8_t *data,
                 const size_t   size)
{
	ClearMagickWand ((MagickWand *) im->banner);

	if (MagickReadImageBlob ((MagickWand *) im->banner,
	                         (const void *) data,
	                         size)
	    == MagickFalse) {
		img_exception ((MagickWand *) im->banner);
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

	if (MagickCompositeImage ((MagickWand *) im->screen,
	                          (MagickWand *) im->banner,
	                          OverCompositeOp,
	                          banner_x, banner_y)
	    == MagickFalse) {
		img_exception ((MagickWand *) im->screen);
		return false;
	}

	return true;
}

bool
img_write (img_t      *im,
           const char *file)
{
	if (!im) {
		return false;
	}

	if (MagickWriteImage ((MagickWand *) im->screen, file) == MagickFalse) {
		img_exception ((MagickWand *) im->screen);
		return false;
	}

	return true;
}

void
img_destroy (img_t *im)
{
	if (im) {
		if (im->banner) {
			im->banner = (void *) DestroyMagickWand((MagickWand *) im->banner);
		}
		im->banner = NULL;

		if (im->screen) {
			im->screen = (void *) DestroyMagickWand((MagickWand *) im->screen);
		}
		im->screen = NULL;
	}

	MagickWandTerminus();
}

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
		im->screen = NewMagickWand();
		im->banner = NewMagickWand();
		im->thumb.data = NULL;
		im->thumb.size = 0;
		return true;
	}

	return false;
}

bool
img_load_screen (img_t      *im,
                 const char *path)
{
	ClearMagickWand (im->screen);

	if (MagickReadImage (im->screen, path)
	    == MagickFalse) {
		img_exception (im->screen);
		return false;
	}

	return true;
}

size_t
img_get_screen_width (img_t *im)
{
	return (im) ? MagickGetImageWidth (im->screen) : 0;
}

size_t
img_get_screen_height (img_t *im)
{
	return (im) ? MagickGetImageHeight (im->screen) : 0;
}

bool
img_load_banner (img_t         *im,
                 const uint8_t *data,
                 const size_t   size)
{
	ClearMagickWand (im->banner);

	if (MagickReadImageBlob (im->banner,
	                         (const void *) data,
	                         size)
	    == MagickFalse) {
		img_exception (im->banner);
		return false;
	}

	return true;
}

bool
img_export (img_t    *im,
            uint8_t **buf,
            size_t   *len)
{
	unsigned char *b;
	size_t l;
	bool r;

	MagickResetIterator (im->screen);

	if (MagickSetImageFormat (im->screen, "PNG") == MagickFalse
	    || !(b = MagickGetImageBlob (im->screen, &l))) {
		*buf = NULL;
		*len = 0;
		r = false;
	} else {
		*buf = b;
		*len = l;
		r = true;
	}

	b = NULL;
	l = 0;

	return r;
}

bool
img_render_thumb (img_t         *im,
                  const ssize_t  banner_x,
                  const ssize_t  banner_y,
                  const size_t   thumb_w,
                  const size_t   thumb_h)
{
	if (im) {
		if (MagickCompositeImage (im->screen,
		                          im->banner,
		                          OverCompositeOp,
		                          banner_x, banner_y) == MagickTrue
		    && MagickScaleImage (im->screen,
		                         thumb_w, thumb_h) == MagickTrue
		    && img_export (im, &im->thumb.data, &im->thumb.size)) {
			return true;
		}

		img_exception (im->screen);
	}

	return false;
}

bool
img_write (img_t      *im,
           const char *file)
{
	if (!im) {
		return false;
	}

	if (MagickWriteImage (im->screen, file) == MagickFalse) {
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
			im->banner = DestroyMagickWand (im->banner);
		}
		im->banner = NULL;

		if (im->screen) {
			im->screen = DestroyMagickWand (im->screen);
		}
		im->screen = NULL;
	}

	MagickWandTerminus();
}

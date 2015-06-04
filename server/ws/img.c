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

#define img_exception(_w, _m) { \
	char *_d; \
	ExceptionType _s; \
	_d = MagickGetException (_w, &_s); \
	(void) fprintf (stderr, "%s:%s:%lu: %s: %s\n", \
	                GetMagickModule(), _m, _d); \
	_d = (char *) MagickRelinquishMemory(_d); \
}

__attribute__((always_inline))
static inline MagickWand *
img_layer (img_t        *im,
           const size_t  layer)
{
	return im->layers[(layer & 3)];
}

bool
img_init (img_t *im)
{
	if (im) {
		size_t i;
		MagickWandGenesis ();

		for (i = 0; i < 4; ++i) {
			if (!(im->layers[i] = NewMagickWand ())) {
				fprintf (stderr, "%s: NewMagickWand failed, cannot continue\n", __func__);

			cleanup:
				for (; i;) {
					--i;
					im->layers[i] = DestroyMagickWand (img_layer (im, i));
					im->layers[i] = NULL;
				}

				MagickWandTerminus ();
				goto fail;
			}
		}

		if (!(im->bgcolor = NewPixelWand ())) {
			fprintf (stderr, "%s: NewPixelWand failed, cannot continue\n", __func__);
			goto cleanup;
		}

		return true;
	}

fail:
	return false;
}

bool
img_import_data (img_t        *im,
                 const size_t  layer,
                 img_data_t   *imd)
{
	if (im && imd) {
		MagickWand *w = img_layer (im, layer);

		ClearMagickWand (w);

		if (MagickReadImageBlob (w, (const void *) imd->data,
		                         (const size_t) imd->size)
		    == MagickFalse) {
			img_exception (w, "MagickReadImageBlob failed");
		} else {
			return true;
		}
	}

	return false;
}

__attribute__((always_inline))
static inline bool
img_layer_empty (img_t        *im,
                 const size_t  layer)
{
	return (!MagickGetNumberImages (img_layer (im, layer)));
}

__attribute__((always_inline))
static inline bool
img_export_data (img_t         *im,
                 const size_t   layer,
                 img_data_t   **imd)
{
	if (im && imd) {
		MagickWand *w = img_layer (im, layer);

		MagickResetIterator (w);

		if (!MagickGetNumberImages (w)) {
			fprintf (stderr, "%s: layer empty\n", __func__);
		} else if (MagickSetImageFormat (w, "PNG") == MagickFalse) {
			img_exception (w, "MagickSetImageFormat failed");
		} else {
			char *blob;
			size_t size;
			if (!(blob = (char *) MagickGetImageBlob (w, &size))) {
				img_exception (w, "MagickGetImageBlob failed");
			} else {
				*imd = img_data_new_from_buffer (size, blob);
				blob = (char *) MagickRelinquishMemory (blob);
				return (*imd != NULL);
			}
		}
	}

	return false;
}

__attribute__((always_inline))
static inline bool
img_composite (img_t         *im,
               const size_t   dst,
               const size_t   src,
               const ssize_t  x,
               const ssize_t  y)
{
	if (im) {
		MagickWand *dw = img_layer (im, dst),
		           *sw = img_layer (im, src);

		MagickResetIterator (dw);
		MagickResetIterator (sw);

		if (MagickCompositeImage (dw, sw, OverCompositeOp, x, y)
		    == MagickFalse) {
			img_exception (dw, "MagickCompositeImage failed");
		} else {
			printf ("%s: composited image\n", __func__);
			return true;
		}
	}

	return false;
}

__attribute__((always_inline))
static inline bool
img_clone_layer (img_t        *im,
                 const size_t  dst,
                 const size_t  src)
{
	if (im) {
		MagickWand *dw = img_layer (im, dst),
		           *sw = img_layer (im, src);
		size_t w, h;

		MagickResetIterator (sw);

		w = MagickGetImageWidth (sw);
		h = MagickGetImageHeight (sw);

		if (MagickGetImageBackgroundColor (sw, im->bgcolor) == MagickFalse) {
			img_exception (sw, "MagickGetImageBackgroundColor failed");

		} else {
			ClearMagickWand (dw);

			if (MagickNewImage (dw, (const size_t) w, (const size_t) h,
			                    (const PixelWand *) im->bgcolor)
			    == MagickFalse) {
				img_exception (dw, "MagickNewImage failed");

			} else if (MagickCompositeImage (dw, sw,
			                                 OverCompositeOp,
			                                 0, 0)
			           == MagickFalse) {
				img_exception (dw, "MagickCompositeImage failed");

			} else {
				printf ("%s: cloned layer %zu to %zu\n",
				        __func__, (src & 3), (dst & 3));
				return true;
			}
		}
	}

	return false;
}

bool
img_scale (img_t        *im,
           const size_t  layer,
           const size_t  width,
           const size_t  height)
{
	if (im) {
		MagickWand *w = img_layer (im, layer);

		if (MagickScaleImage (w, width, height) == MagickFalse) {
			img_exception (w, "MagickScaleImage failed");
		} else {
			printf ("scaled image\n");
			return true;
		}
	}

	return false;
}

bool
img_render (img_t *im)
{
	if (im) {
		const size_t x = 1024, y = 768, tw = 640, th = 512;

		if (img_layer_empty (im, 0)) {
			fprintf (stderr, "%s: capture layer empty\n", __func__);
		} else if (!img_clone_layer (im, 2, 0)) {
			fprintf (stderr, "%s: img_clone_layer failed\n", __func__);
		} else {
			if (!img_layer_empty (im, 1)
			    && !img_composite (im, 2, 1, x, y)) {
				fprintf (stderr, "%s: img_composite failed\n", __func__);
			}
			if (!img_clone_layer (im, 3, 2)
			    || !img_scale (im, 3, tw, th)) {
				fprintf (stderr, "%s: thumbnail render failed\n", __func__);
			} else {
				// todo: export layer 3 to thumb_file
			}
			return true;
		}
	}

	return false;
}

bool
img_write (img_t        *im,
           const size_t  layer,
           const char   *file)
{
	if (!im) {
		return false;
	}

	MagickWand *w = img_layer (im, layer);

	if (MagickWriteImage (w, file) == MagickFalse) {
		img_exception (w, "MagickWriteImage failed");
		return false;
	}

	printf ("wrote image file\n");

	return true;
}

void
img_destroy (img_t *im)
{
	if (im) {
		size_t i = 4;
		do {
			--i;
			im->layers[i] = DestroyMagickWand (img_layer (im, i));
			im->layers[i] = NULL;
		} while (i);

		if (im->bgcolor) {
			im->bgcolor = DestroyPixelWand (im->bgcolor);
			im->bgcolor = NULL;
		}

		MagickWandTerminus ();
	}
}

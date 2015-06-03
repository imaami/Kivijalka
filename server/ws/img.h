/** \file img.h
 *
 * Image manipulation.
 */

#ifndef __IMG_H__
#define __IMG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <wand/MagickWand.h>

#include "img_data.h"
#include "img_file.h"

typedef struct img img_t;

struct img {
	MagickWand *layers[4];
	PixelWand  *bgcolor;
} __attribute__((gcc_struct,packed));

extern bool
img_init (img_t *im);

extern bool
img_import_data (img_t        *im,
                 const size_t  layer,
                 img_data_t   *imd);


extern bool
img_scale (img_t        *im,
           const size_t  layer,
           const size_t  width,
           const size_t  height);

extern bool
img_render (img_t *im);

extern bool
img_write (img_t        *im,
           const size_t  layer,
           const char   *file);

extern void
img_destroy (img_t *im);

#ifdef __cplusplus
}
#endif

#endif // __IMG_H__

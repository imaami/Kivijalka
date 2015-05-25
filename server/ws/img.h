/** \file img.h
 *
 * Image manipulation.
 */

#ifndef __IMG_H__
#define __IMG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <wand/MagickWand.h>

typedef struct img img_t;

struct img {
	struct {
		uint8_t *data;
		size_t   size;
	} thumb;
	MagickWand *layers[2];
} __attribute__((gcc_struct,packed));

extern bool
img_init (img_t *im);

extern size_t
img_get_width (img_t    *im,
               unsigned  int layer);

extern size_t
img_get_height (img_t    *im,
                unsigned  int layer);

extern bool
img_load_file (img_t        *im,
               unsigned int  layer,
               const char   *path);

extern bool
img_load_data (img_t         *im,
               unsigned int   layer,
               const uint8_t *data,
               const size_t   size);

extern bool
img_render (img_t         *im,
            const ssize_t  banner_x,
            const ssize_t  banner_y);

extern bool
img_scale (img_t        *im,
           unsigned int  layer,
           const size_t  width,
           const size_t  height);

extern bool
img_write (img_t        *im,
           unsigned int  layer,
           const char   *file);

extern bool
img_export (img_t         *im,
            unsigned int   layer,
            uint8_t      **buf,
            size_t        *len);

extern void
img_destroy (img_t *im);

#ifdef __cplusplus
}
#endif

#endif // __IMG_H__

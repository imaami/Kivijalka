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

typedef struct img img_t;

struct img {
	void *screen;
	void *banner;
} __attribute__((gcc_struct,packed));

extern bool
img_init (img_t *im);

extern bool
img_load_screen (img_t      *im,
                 const char *path);

extern size_t
img_get_screen_width (img_t *im);

extern size_t
img_get_screen_height (img_t *im);

extern bool
img_load_banner (img_t         *im,
                 const uint8_t *data,
                 const size_t   size);

extern bool
img_render_screen (img_t         *im,
                   const ssize_t  banner_x,
                   const ssize_t  banner_y);

extern bool
img_write (img_t      *im,
           const char *file);

extern void
img_destroy (img_t *im);

#ifdef __cplusplus
}
#endif

#endif // __IMG_H__

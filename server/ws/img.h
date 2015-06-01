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
#include <semaphore.h>
#include <wand/MagickWand.h>

typedef struct img img_t;
typedef struct img_file img_file_t;

struct img {
	MagickWand *layers[3];
} __attribute__((gcc_struct,packed));

struct img_file {
	sem_t       lock;
	const char *path;
	size_t      size;
	char       *data;
} __attribute__((gcc_struct,packed));

extern bool
img_init (img_t *im);

extern void
img_file_init (img_file_t *imf);

extern void
img_file_fini (img_file_t *imf);

extern bool
img_file_set_path (img_file_t *imf,
                   const char *path);

extern bool
img_file_set_data (img_file_t *imf,
                   size_t      size,
                   char       *data);

extern bool
img_file_read_data (img_file_t *imf);

extern bool
img_file_import_layer (img_file_t   *imf,
                       img_t        *im,
                       unsigned int  layer);

extern bool
img_file_wait (img_file_t *imf);

extern bool
img_file_post (img_file_t *imf);

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
img_load_data (img_t        *im,
               unsigned int  layer,
               const char   *data,
               const size_t  size);

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

extern void
img_destroy (img_t *im);

#ifdef __cplusplus
}
#endif

#endif // __IMG_H__

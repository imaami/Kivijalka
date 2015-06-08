/** \file img.h
 *
 * Image manipulation.
 */

#ifndef __IMG_H__
#define __IMG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <semaphore.h>

#include "img_file.h"

typedef struct img img_t;

struct img {
	unsigned int _;
} __attribute__((gcc_struct,packed));

extern bool
img_init (img_t *im);

extern void
img_destroy (img_t *im);

extern void
img_thread (img_t      *im,
            size_t      bx,
            size_t      by,
            size_t      tw,
            size_t      th,
            sem_t      *sem,
            img_file_t *capture_file,
            img_file_t *banner_file,
            img_file_t *output_file,
            img_file_t *thumb_file);

#ifdef __cplusplus
}
#endif

#endif // __IMG_H__

/** \file img_file.h
 *
 * Image manipulation.
 */

#ifndef __IMG_FILE_H__
#define __IMG_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

#include "img_data.h"

typedef struct img_file img_file_t;

struct img_file {
	sem_t       lock;
	const char *path;
	img_data_t *data;
} __attribute__((gcc_struct,packed));

extern void
img_file_init (img_file_t *imf);

extern void
img_file_fini (img_file_t *imf);

extern bool
img_file_set_path (img_file_t *imf,
                   const char *path);

/**
 * Atomically grab the img_data_t pointer from an img_file_t object and
 * replace it with a new one, and write the replaced pointer into \a old.
 * This function provides a means for any thread accessing the img_file_t
 * object to be certain that it is the sole owner of the retrieved pointer.
 * Notice, however, that this implies that the thread replacing the img_data_t
 * pointer must also free the retrieved old pointer later on.
 *
 * @param imf Pointer to the img_file_t object whose img_data_t pointer
 *            to replace.
 * @param imd New pointer to replace the previous one with. Can be NULL.
 * @param old Pointer to the memory location to put the replaced previous
 *            pointer in.
 */
extern bool
img_file_swap_data (img_file_t  *imf,
                    img_data_t  *imd,
                    img_data_t **old);

/**
 * Atomically grab the img_data_t pointer from an img_file_t object and
 * replace the pointer with NULL. This function provides a means for any
 * thread accessing the img_file_t object to be certain that it is the
 * sole owner of the pointer. Notice, however, that this implies that
 * the thread stealing the img_data_t pointer must also free it later on.
 *
 * @param imf Pointer to the img_file_t object to steal the pointer from.
 * @param imd Pointer to the memory location to put the stolen pointer in.
 */
__attribute__((always_inline))
static inline bool
img_file_steal_data (img_file_t  *imf,
                     img_data_t **imd)
{
	return img_file_swap_data (imf, NULL, imd);
}

extern bool
img_file_replace_data (img_file_t *imf,
                       img_data_t *imd);

extern bool
img_file_wait (img_file_t *imf);

extern bool
img_file_post (img_file_t *imf);

extern bool
img_file_update (img_file_t *imf);

#ifdef __cplusplus
}
#endif

#endif // __IMG_FILE_H__

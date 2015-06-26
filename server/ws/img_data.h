/** \file img_data.h
 *
 * Image manipulation.
 */

#ifndef __IMG_DATA_H__
#define __IMG_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct img_data img_data_t;

struct img_data {
	size_t size;
	char   data[];
} __attribute__((gcc_struct,packed));

/**
 * Return the total size of an img_data_t object in bytes.
 */
extern size_t
img_data_size (img_data_t *imd);

/**
 * Allocate a new img_data_t object.
 */
extern img_data_t *
img_data_alloc (size_t size);

/**
 * Wipe (overwrite with zero) and free an img_data_t object.
 */
extern void
img_data_free (img_data_t *imd);

/**
 * Read the contents of a buffer into a newly allocated img_data_t object.
 */
extern img_data_t *
img_data_new_from_buffer (size_t      size,
                          const char *data);

/**
 * Read a file into a newly allocated img_data_t object.
 */
extern img_data_t *
img_data_new_from_file (const char *path);

#ifdef __cplusplus
}
#endif

#endif // __IMG_DATA_H__

/** \file file.h
 *
 * File read/write.
 */

#ifndef __KIVIJALKA_FILE_H__
#define __KIVIJALKA_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "buf.h"

typedef struct file file_t;

extern file_t *
file_create (const char *path);

extern void
file_destroy (file_t **f);

extern const char *
file_path (file_t *f);

extern bool
file_open (file_t     *f,
           const char *mode);

extern bool
file_close (file_t *f);

extern bool
file_size (file_t *f,
           size_t *size);

extern bool
file_read (file_t  *f,
           size_t   size,
           uint8_t *data,
           size_t  *count);

extern bool
file_write (file_t        *f,
            size_t         size,
            const uint8_t *data);

extern bool
file_read_to_buf (file_t *f,
                  buf_t  *buf,
                  size_t *count);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_FILE_H__

/** \file file.h
 *
 * File read/write.
 */

#ifndef __FILE_H__
#define __FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

extern bool
file_write (const char    *path,
            size_t         size,
            const uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // __FILE_H__

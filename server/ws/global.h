/** \file global.h
 *
 * Global variables.
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <semaphore.h>

#include "img_file.h"

extern size_t thumb_w, thumb_h;
extern sem_t process_sem;
extern img_file_t capture_file, banner_file, output_file, thumb_file;

extern void
global_init (size_t thumb_width,
             size_t thumb_height);

extern void
global_fini (void);

#ifdef __cplusplus
}
#endif

#endif // __GLOBAL_H__

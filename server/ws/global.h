/** \file global.h
 *
 * Global variables.
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>

#include "img.h"

extern sem_t process_sem;
extern img_file_t capture_file, banner_file, output_file, thumb_file;
extern img_t img;

extern void
global_init (void);

extern void
global_fini (void);

#ifdef __cplusplus
}
#endif

#endif // __GLOBAL_H__

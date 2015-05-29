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

extern sem_t capture_sem, process_sem, output_sem;
extern const char *capture_file, *output_file;
extern char *capture_data, *banner_data, *output_data, *thumb_data;
extern size_t capture_size, banner_size, output_size, thumb_size;
extern img_t img;

extern bool
global_init (void);

extern void
global_fini (void);

#ifdef __cplusplus
}
#endif

#endif // __GLOBAL_H__

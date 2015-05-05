/** \file watcher.h
 *
 * File change watcher.
 */

#ifndef __WATCHER_H__
#define __WATCHER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct watcher watcher_t;

extern watcher_t *
watcher_create (const char *path);

extern int64_t
watcher_run_once (watcher_t *w);

extern void
watcher_destroy (watcher_t *w);

#ifdef __cplusplus
}
#endif

#endif // __WATCHER_H__

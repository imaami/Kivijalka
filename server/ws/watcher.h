/** \file watcher.h
 *
 * File change watcher.
 */

#ifndef __WATCHER_H__
#define __WATCHER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <semaphore.h>

typedef struct watcher watcher_t;

extern watcher_t *
watcher_create (const char *path);

extern bool
watcher_prepare (watcher_t *w);

extern void
watcher_start (watcher_t *w);

extern bool
watcher_wait (watcher_t *w);

extern void
watcher_stop (watcher_t *w);

extern void
watcher_destroy (watcher_t *w);

#ifdef __cplusplus
}
#endif

#endif // __WATCHER_H__

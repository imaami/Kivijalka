#ifndef __KIVIJALKA_CACHE_H__
#define __KIVIJALKA_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <uuid/uuid.h>

#include "banner.h"

typedef struct cache cache_t;

extern cache_t *
cache_create (const char *path);

extern void
cache_destroy (cache_t **c);

extern void
cache_import (cache_t *c);

extern const char *
cache_path (cache_t *c);

extern bool
cache_add_banner (cache_t    *c,
                  banner_t   *banner,
                  const bool  write_to_disk);

extern banner_t *
cache_most_recent (cache_t *c);

//extern char *
//cache_json (cache_t *c);

extern banner_t *
cache_find_banner_by_uuid_str (cache_t    *c,
                               const char *uuid_str);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_CACHE_H__

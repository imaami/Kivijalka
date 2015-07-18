#ifndef __KIVIJALKA_BANNER_CACHE_H__
#define __KIVIJALKA_BANNER_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "banner.h"
#include "sha1.h"

typedef struct banner_cache banner_cache_t;

extern banner_cache_t *
banner_cache_create (const char *path);

extern void
banner_cache_destroy (banner_cache_t **bc);

extern const char *
banner_cache_path (banner_cache_t *bc);

extern banner_t *
banner_cache_find_banner (banner_cache_t *bc,
                          sha1_t         *hash);

extern bool
banner_cache_add_banner (banner_cache_t *bc,
                         banner_t       *banner);

extern banner_t *
banner_cache_most_recent (banner_cache_t *bc);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_BANNER_CACHE_H__

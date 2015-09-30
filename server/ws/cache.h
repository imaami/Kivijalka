#ifndef __KIVIJALKA_CACHE_H__
#define __KIVIJALKA_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <uuid/uuid.h>

#include "banner.h"
#include "display.h"

typedef struct cache cache_t;

extern cache_t *
cache_create (const char *path);

extern void
cache_destroy (cache_t **c);

extern void
cache_import (cache_t *c);

extern bool
cache_import_packet (cache_t         *c,
                     banner_packet_t *p);

extern bool
cache_apply_modpkt (cache_t          *c,
                    banner_modpkt_t  *p,
                    char            **r,
                    size_t           *s);
extern bool
cache_apply_display_modpkt (cache_t           *c,
                            display_modpkt_t  *p,
                            char             **r,
                            size_t            *s);

extern const char *
cache_path (cache_t *c);

extern banner_t *
cache_most_recent (cache_t *c);

extern bool
cache_info_packet (cache_t  *c,
                   uint8_t **data,
                   size_t   *size);

extern char *
cache_json (cache_t *c);

extern char *
cache_json_stringified (cache_t *c);

extern bool
cache_activate_banner (cache_t  *c,
                       banner_t *b);

extern banner_t *
cache_find_banner_by_uuid_str (cache_t    *c,
                               const char *uuid_str);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_CACHE_H__

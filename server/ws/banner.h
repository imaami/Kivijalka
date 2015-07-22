#ifndef __KIVIJALKA_BANNER_H__
#define __KIVIJALKA_BANNER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "point.h"
#include "sha1.h"
#include "img_data.h"

typedef struct banner banner_t;
typedef struct banner_packet banner_packet_t;

extern banner_t *
banner_create (void);

extern banner_t *
banner_create_from_path (const char *path);

extern banner_t *
banner_create_from_packet (banner_packet_t *pkt);

extern void
banner_destroy (banner_t **b);

extern bool
banner_set_name (banner_t   *b,
                 const char *name);

extern const char *
banner_name (banner_t *b);

extern void
banner_set_offset (banner_t *b,
                   point_t   offset);

extern bool
banner_hash_cmp (banner_t *b,
                 sha1_t   *hash);

extern bool
banner_hash_cpy (banner_t *b,
                 sha1_t   *dest);

extern void
banner_print_hash (banner_t *b);

extern img_data_t *
banner_remove_data (banner_t *b);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_BANNER_H__

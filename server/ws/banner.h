#ifndef __KIVIJALKA_BANNER_H__
#define __KIVIJALKA_BANNER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "point.h"
#include "sha1.h"
#include "list.h"

typedef struct banner banner_t;

extern banner_t *
banner_create (void);

extern banner_t *
banner_create_from_path (const char *path);

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

extern void
banner_print_hash (banner_t *b);

extern banner_t *
banner_next_in_list (banner_t    *b,
                     list_head_t *list);

extern void
banner_add_to_list (banner_t    *b,
                    list_head_t *list);

extern void
banner_del_from_list (banner_t *b);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_BANNER_H__

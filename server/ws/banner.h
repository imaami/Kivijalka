/* Copyright © 2015 Koneet Kiertoon and Juuso Alasuutari.
 * Written by Juuso Alasuutari.
 *
 * This file is part of Kivijalka.
 *
 * Kivijalka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kivijalka is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kivijalka.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __KIVIJALKA_BANNER_H__
#define __KIVIJALKA_BANNER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <uuid/uuid.h>

#include "point.h"
#include "sha1.h"
#include "img_data.h"
#include "packet.h"

typedef struct banner banner_t;
typedef struct banner_packet banner_packet_t;
typedef struct banner_modpkt banner_modpkt_t;

extern banner_t *
banner_create (void);

extern void
banner_destroy (banner_t **b);

extern banner_packet_t *
banner_packet_inspect (const char *buf,
                       int         len);

extern banner_modpkt_t *
banner_modpkt_inspect (const char *buf,
                       int         len);

extern bool
banner_set_name (banner_t   *b,
                 const char *name);

extern const char *
banner_name (banner_t *b);

extern void
banner_set_offset (banner_t *b,
                   point_t   offset);

extern int32_t
banner_offset_x (banner_t *b);

extern int32_t
banner_offset_y (banner_t *b);

extern bool
banner_hash_cmp (banner_t *b,
                 sha1_t   *hash);

extern bool
banner_hash_cpy (banner_t *b,
                 sha1_t   *dest);

extern void
banner_uuid_unparse (banner_t *b,
                     char     *str);

extern void
banner_hash_unparse (banner_t *b,
                     char     *str);

extern img_data_t *
banner_img (banner_t *b);

extern img_data_t *
banner_remove_data (banner_t *b);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_BANNER_H__

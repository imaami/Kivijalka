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
#ifndef __KIVIJALKA_ARGS_H__
#define __KIVIJALKA_ARGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

extern bool
args_parse (int    argc,
            char **argv);

extern uint32_t
args_get_display_width (void);

extern uint32_t
args_get_display_height (void);

extern const char *
args_get_cache_path (void);

extern const char *
args_get_cache_uuid (void);

extern const char *
args_get_capture_path (void);

extern uint32_t
args_get_capture_width (void);

extern uint32_t
args_get_capture_height (void);

extern const char *
args_get_output_path (void);

extern const char *
args_get_server_addr (void);

extern uint16_t
args_get_server_port (void);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_ARGS_H__

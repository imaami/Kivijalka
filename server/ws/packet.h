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
#ifndef __KIVIJALKA_PACKET_H__
#define __KIVIJALKA_PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum {
	PACKET_REQ     = 0x01,
	PACKET_ADD     = 0x02,
	PACKET_MOD     = 0x04,
	PACKET_DEL     = 0x08,
	PACKET_DISPLAY = 0x10,
	PACKET_BANNER  = 0x20,
	PACKET_IMG     = 0x40,
	PACKET_DATA    = 0x80
};

extern uint32_t
packet_inspect (const char **buf,
                int         *len);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_PACKET_H__

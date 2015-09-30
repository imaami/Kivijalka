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
#ifndef __KIVIJALKA_PRIVATE_DISPLAY_H__
#define __KIVIJALKA_PRIVATE_DISPLAY_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../display.h"
#include "geo2d.h"
#include "point.h"

#include <stdint.h>
#include <uuid/uuid.h>

/**
 * @class display
 * @author Juuso Alasuutari
 * @brief Graphical display.
 */
struct display {
	uint32_t  width;    //! Display width
	uint32_t  height;   //! Display height
	uint32_t  scaled_w; //! Width to scale to for output
	uint32_t  scaled_h; //! Height to scale to for output
	uint32_t *pixbuf;   //! Pixels
} __attribute__((packed,gcc_struct));

struct display_modpkt {
	uint64_t     time;   //! Timestamp
	uint32_t     id;     //! Display ID
	uuid_t       uuid;   //! UUID of displayed banner
	struct geo2d dims;   //! Dimensions
	point_t      offs;   //! Offset
	uint32_t     nsiz;   //! Display name length; 0 means don't change name
	uint8_t      name[]; //! Optional display name
} __attribute__((gcc_struct,packed));

struct display_modpkt_echo {
	uint32_t              type;
	struct display_modpkt orig;
} __attribute__((gcc_struct,packed));

#endif // __KIVIJALKA_PRIVATE_DISPLAY_H__

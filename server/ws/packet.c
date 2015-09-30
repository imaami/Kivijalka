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
/** \file packet.c
 *
 * Packet handling.
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "packet.h"

uint32_t
packet_inspect (const char **buf,
                int         *len)
{
	uint32_t flags;

	if (!buf || !*buf || !len) {
		fprintf (stderr, "%s: invalid parameters\n", __func__);
		flags = 0;

	} else if (*len < 4) {
		fprintf (stderr, "%s: illegal packet length\n", __func__);
		flags = 0;

	} else {
		flags = *(uint32_t *) *buf;

		switch (flags) {
		case PACKET_REQ|PACKET_IMG|PACKET_DATA:
			// request image data
			break;

		case PACKET_ADD|PACKET_BANNER|PACKET_IMG|PACKET_DATA:
			// upload new banner
			break;

		case PACKET_ADD|PACKET_DISPLAY|PACKET_BANNER|PACKET_IMG|PACKET_DATA:
			// upload new banner and activate
			break;

		case PACKET_MOD|PACKET_DISPLAY:
			// change active banner
			break;

		case PACKET_MOD|PACKET_BANNER:
			// modify banner
			break;

		case PACKET_DEL|PACKET_DISPLAY:
			// delete display
			break;

		case PACKET_DEL|PACKET_BANNER:
			// delete banner
			break;

		case PACKET_DEL|PACKET_IMG:
			// delete image
			break;

		default:
			fprintf (stderr, "%s: invalid packet flags %"PRIu32"\n",
			         __func__, flags);
			flags = 0;
			goto _end;
		}

		*buf += 4;
		*len -= 4;
	}

_end:
	return flags;
}

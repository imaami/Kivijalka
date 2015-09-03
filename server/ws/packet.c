/** \file packet.c
 *
 * Packet handling.
 */

#include <stdint.h>
#include <inttypes.h>

#include "private/banner.h"

uint32_t
packet_inspect (const char *buf,
                int         len)
{
	uint32_t type;

	if (!buf) {
		fprintf (stderr, "%s: null buffer pointer\n", __func__);
		type = 0;
	} else if (len < 4) {
		fprintf (stderr, "%s: illegal packet length\n", __func__);
		type = 0;
	} else {
		type = *(uint32_t *) buf;
		switch (type) {
		case 1: // banner + image upload packet
			break;
		default:
			fprintf (stderr, "%s: illegal packet type %"PRIu32"\n",
			         __func__, type);
			type = 0;
		}
	}

	return type;
}

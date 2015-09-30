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

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

#ifndef __KIVIJALKA_PACKET_H__
#define __KIVIJALKA_PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint32_t
packet_inspect (const char *buf,
                int         len);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_PACKET_H__

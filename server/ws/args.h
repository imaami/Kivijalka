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

/** \file display.h
 *
 * Graphical display.
 */

#ifndef __KIVIJALKA_DISPLAY_H__
#define __KIVIJALKA_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "packet.h"

typedef struct display display_t;
typedef struct display_modpkt display_modpkt_t;

/**
 * @brief Create display object.
 * @param width Width of display (number of horizontal pixels).
 * @param height Height of display (number of vertical pixels).
 * @return Pointer to newly allocated and initialized display object.
 */
extern display_t *
display_create (const uint32_t width,
                const uint32_t height,
                const uint32_t scaled_w,
                const uint32_t scaled_h);

/**
 * @brief Destroy display object.
 * @param d Pointer to display object.
 */
extern void
display_destroy (display_t *d);

/**
 * @brief Get display width.
 * @param d Pointer to display object.
 * @return Display width (number of horizontal pixels).
 */
extern uint32_t
display_width (display_t *d);

/**
 * @brief Get display height.
 * @param d Pointer to display object.
 * @return Display height (number of vertical pixels).
 */
extern uint32_t
display_height (display_t *d);

/**
 * @brief Get display's scaled output width.
 * @param d Pointer to display object.
 * @return Display's scaled output width (number of horizontal pixels).
 */
extern uint32_t
display_scaled_width (display_t *d);

/**
 * @brief Get display's scaled output height.
 * @param d Pointer to display object.
 * @return Display's scaled output height (number of vertical pixels).
 */
extern uint32_t
display_scaled_height (display_t *d);

/**
 * @brief Get display's pixel buffer.
 * @param d Pointer to display object.
 * @return Pointer to display's pixel buffer.
 */
extern uint32_t *
display_pixbuf (display_t *d);

/**
 * @brief Render display pixels.
 * @param d Pointer to display object.
 * @param bg_data Pointer to the background image's pixel data. The background
 *                image is assumed to be the same size as the display itself.
 * @param ol_data Pointer to the overlay image's pixel data.
 * @param ol_w Overlay image width.
 * @param ol_h Overlay image height.
 * @param ol_x Overlay horizontal offset from left side of display.
 * @param ol_y Overlay vertical offset from top of display.
 */
extern bool
display_render (display_t *d,
                uint32_t  *bg_data,
                uint32_t  *ol_data,
                uint32_t   ol_w,
                uint32_t   ol_h,
                int32_t    ol_x,
                int32_t    ol_y);

/**
 * @brief Render display background pixels.
 * @param d Pointer to display object.
 * @param bg_data Pointer to the background image's pixel data. The background
 *                image is assumed to be the same size as the display itself.
 */
extern bool
display_render_bg (display_t *d,
                   uint32_t  *bg_data);

extern display_modpkt_t *
display_modpkt_inspect (const char *buf,
                        int         len);

extern bool
display_banner_uuid_export (char   *path,
                            size_t  path_len);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_DISPLAY_H__

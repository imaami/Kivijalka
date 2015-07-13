/** \file display.h
 *
 * Graphical display.
 */

#ifndef __KIVIJALKA_DISPLAY_H__
#define __KIVIJALKA_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct display display_t;

/**
 * @brief Create display object.
 * @param width Width of display (number of horizontal pixels).
 * @param height Height of display (number of vertical pixels).
 * @return Pointer to newly allocated and initialized display object.
 */
extern display_t *
display_create (const uint32_t width,
                const uint32_t height);

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
 * @brief Get display's pixel buffer.
 * @param d Pointer to display object.
 * @return Pointer to display's pixel buffer.
 */
extern uint32_t *
display_pixbuf (display_t *d);

/**
 * @brief Write a rectangle of pixels to display.
 * @param d Pointer to display object.
 * @param dst_x Horizontal offset of the rectangle's
 *              top left corner on the display.
 * @param dst_y Vertical offset of the rectangle's
 *              top left corner on the display.
 * @param src Pointer to buffer containing the pixels to write.
 * @param src_w Total width of the source pixel buffer.
 * @param src_h Total height of the source pixel buffer.
 */
extern bool
display_write (display_t *d,
               uint32_t   dst_x,
               uint32_t   dst_y,
               uint32_t  *src,
               uint32_t   src_w,
               uint32_t   src_h,
               uint32_t   src_x,
               uint32_t   src_y,
               uint32_t   rect_w,
               uint32_t   rect_h);

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
                uint32_t   ol_x,
                uint32_t   ol_y);

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_DISPLAY_H__

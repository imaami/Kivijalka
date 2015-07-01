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

#ifdef __cplusplus
}
#endif

#endif // __KIVIJALKA_DISPLAY_H__

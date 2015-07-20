#ifndef __KIVIJALKA_PRIVATE_DISPLAY_H__
#define __KIVIJALKA_PRIVATE_DISPLAY_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../display.h"

#include <stdint.h>

/**
 * @class display
 * @author Juuso Alasuutari
 * @brief Graphical display.
 */
struct display {
	uint32_t  width;  //! Display width
	uint32_t  height; //! Display height
	uint32_t *pixbuf; //! Pixels
} __attribute__((packed,gcc_struct));

#endif // __KIVIJALKA_PRIVATE_DISPLAY_H__

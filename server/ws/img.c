/** \file img.c
 *
 * Image manipulation.
 */

#include "private/img.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_BMP
#define STBI_NO_TGA
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_LINEAR
#define STBI_ASSERT(x)
#include "stb_image.h"


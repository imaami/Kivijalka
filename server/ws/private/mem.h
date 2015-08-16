#ifndef __KIVIJALKA_PRIVATE_MEM_H__
#define __KIVIJALKA_PRIVATE_MEM_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <limits.h> // CHAR_BIT
#include <stddef.h> // NULL, size_t
#include <stdlib.h> // aligned_alloc(), free()
#include <stdint.h>

typedef union __attribute__((transparent_union)) {
	void          **v;
	char          **c;
	unsigned char **uc;
	int8_t        **i8;
	uint8_t       **u8;
	int16_t       **i16;
	uint16_t      **u16;
	int32_t       **i32;
	uint32_t      **u32;
	int64_t       **i64;
	uint64_t      **u64;
} mem_del_t;

#define MEM_BIT_SHIFT_MAX (CHAR_BIT * sizeof (size_t) - 1)

__attribute__((always_inline))
static inline void *
_mem_new (size_t  log2_align,
          size_t  minimum_bytes,
          size_t *allocated)
{
	void *m;

	if (log2_align > MEM_BIT_SHIFT_MAX) {
		fprintf (stderr, "%s: invalid alignment parameter\n", __func__);
		m = NULL;
	} else {
		size_t alignment = ((size_t) 1) << log2_align;
		size_t alloc_size = minimum_bytes;
		size_t padding = minimum_bytes & (alignment - 1);

		if (padding) {
			alloc_size += (alignment - padding);
		}

		if (!(m = aligned_alloc (alignment, alloc_size))) {
			fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
		} else {
			*allocated = alloc_size;
			printf ("%s: allocated %zu B aligned to %zu B\n",
			        __func__, alloc_size, alignment);
		}
	}

	return m;
}

__attribute__((always_inline))
static inline void
_mem_del (mem_del_t m)
{
	if (m.v) {
		free (*(m.v));
		*(m.v) = NULL;
	}
}

#endif // __KIVIJALKA_PRIVATE_MEM_H__

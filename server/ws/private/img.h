#ifndef __KIVIJALKA_PRIVATE_IMG_H__
#define __KIVIJALKA_PRIVATE_IMG_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../list.h"
#include "../file.h"
#include "sha1.h"

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct img {
	list_head_t  hook;
	sha1_t       hash;
	size_t       size;
	uint8_t      data[];
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline struct img *
_img_read_file (const char *path)
{
	struct img *im;
	file_t *f;
	size_t size;

	printf ("%s: path=%s\n", __func__, path);

	if (!(f = file_create (path))) {
		fprintf (stderr, "%s: file_create failed\n", __func__);
		im = NULL;
		goto end;
	}

	if (!file_open (f, "rb")) {
		fprintf (stderr, "%s: file_open failed\n", __func__);
		im = NULL;
		goto destroy_file;
	}

	if (!file_size (f, &size)) {
		fprintf (stderr, "%s: file_size failed\n", __func__);
		im = NULL;
		goto close_file;
	}

	size_t alloc_size = offsetof (struct img, data) + size + 1;
	size_t padding = alloc_size & 0x3f;

	if (padding) {
		alloc_size += (0x40 - padding);
		padding = 0;
	}

	if (!(im = aligned_alloc (64, alloc_size))) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
		goto close_file;
	}

	alloc_size -= offsetof (struct img, data);

	if (!file_read (f, alloc_size, im->data, &size)) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
		free (im);
		im = NULL;
		goto close_file;
	}

	(void) memset ((void *) (im->data + size), 0, alloc_size - size);

	list_init (&im->hook);
	im->size = size;
	_sha1_gen (&im->hash, im->size, im->data);

	char str[41];
	_sha1_str (&im->hash, str);
	printf ("%s: read %zu bytes, SHA1=%s\n", __func__, im->size, str);

close_file:
	if (!file_close (f)) {
		fprintf (stderr, "%s: file_close failed\n", __func__);
	}

destroy_file:
	file_destroy (&f);

end:
	alloc_size = 0;
	size = 0;
	return im;
}

__attribute__((always_inline))
static inline void
_img_destroy (struct img *im)
{
	size_t size = offsetof (struct img, data) + im->size;
	if (im->hook.next && im->hook.prev) {
		list_del (&im->hook);
	}
	(void) memset ((void *) im, 0, size);
	size = 0;
	free (im);
	im = NULL;
}

__attribute__((always_inline))
static inline bool
_img_hash_cmp (struct img *im,
               sha1_t     *hash)
{
	return _sha1_cmp (&im->hash, hash);
}

__attribute__((always_inline))
static inline void
_img_hash_cpy (struct img *im,
               sha1_t     *dest)
{
	_sha1_cpy (&im->hash, dest);
}

__attribute__((always_inline))
static inline void
_img_hash_unparse (struct img *im,
                   char       *str)
{
	_sha1_str (&im->hash, str);
}

__attribute__((always_inline))
static inline const uint8_t *
_img_data (struct img *im)
{
	return (const uint8_t *) im->data;
}

#endif // __KIVIJALKA_PRIVATE_IMG_H__

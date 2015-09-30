/* Copyright © 2015 Koneet Kiertoon and Juuso Alasuutari.
 * Written by Juuso Alasuutari.
 *
 * This file is part of Kivijalka.
 *
 * Kivijalka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kivijalka is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kivijalka.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __KIVIJALKA_PRIVATE_IMG_H__
#define __KIVIJALKA_PRIVATE_IMG_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../img.h"
#include "../list.h"
#include "../file.h"
#include "geo2d.h"
#include "sha1.h"
#include "json.h"
#include "mem.h"
#include "point.h"

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h> // memcpy, strncpy
#include <errno.h>
#include <stdio.h>

struct img {
	list_head_t  hook;   //! List hook
	sha1_t       hash;   //! Hash of this image
	size_t       nsiz;   //! Size of source file name
	char        *name;   //! Name of this image's source file
	list_head_t  refs;   //! List of all banners using this image
	size_t       size;   //! Size of image file in bytes
	uint8_t     *data;   //! Pointer to memory buffer containing image file
	size_t       width;  //! Width of this image in pixels
	size_t       height; //! Height of this image in pixels
	uint8_t     *pxdata; //! Pointer to memory buffer containing pixel data
} __attribute__((gcc_struct,packed));

struct img_info {
	sha1_t       hash;
	uint32_t     name_size;
	uint64_t     file_size;
	struct geo2d dims;
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline void
_img_info_init (struct img_info *ii,
                struct img      *im)
{
	_sha1_cpy (&im->hash, &ii->hash);
	ii->name_size = (im->nsiz < UINT32_MAX) ? im->nsiz : UINT32_MAX;
	ii->file_size = im->size;
	_geo2d_set_width (&ii->dims,
	                  (im->width < UINT32_MAX) ? im->width : UINT32_MAX);
	_geo2d_set_height (&ii->dims,
	                   (im->height < UINT32_MAX) ? im->height : UINT32_MAX);
}

__attribute__((always_inline))
static inline struct img_info *
_img_info_create (struct img *im)
{
	struct img_info *ii;
	char *p;
	size_t n;
	if ((p = _mem_new (6, sizeof (struct img_info), &n))) {
		ii = (struct img_info *) p;
		_img_info_init (ii, im);
		for (size_t k = sizeof (struct img_info); k < n; ++k) {
			p[k] = 0;
		}
	} else {
		ii = NULL;
	}
	return ii;
}

__attribute__((always_inline))
static inline struct img *
_img_alloc (void)
{
	struct img *im;
	size_t size;

	if (!(im = _mem_new (6, sizeof (struct img), &size))) {
		fprintf (stderr, "%s: memory allocation failed\n", __func__);
	} else {
		(void) memset ((void *) (((char *) im) + sizeof (struct img)),
		               0, size - sizeof (struct img));
	}

	return im;
}

__attribute__((always_inline))
static inline void
_img_init (struct img  *im,
           list_head_t *list,
           sha1_t      *hash,
           size_t       nsiz,
           char        *name,
           size_t       size,
           uint8_t     *data,
           size_t       width,
           size_t       height,
           uint8_t     *pxdata)
{
	im->pxdata = pxdata;
	im->height = height;
	im->width = width;

	im->data = data;
	im->size = size;

	list_init (&im->refs);

	im->nsiz = nsiz;
	im->name = name;

	if (hash) {
		_sha1_cpy (hash, &im->hash);
	} else {
		_sha1_init (&im->hash);
	}

	if (list) {
		list_add (&im->hook, list);
	} else {
		list_init (&im->hook);
	}
}

__attribute__((always_inline))
static inline char *
_img_name_from_path (const char *path,
                     size_t     *result_len)
{
	union {
		const char *cc;
		char       *c;
	} ptr;
	const char *begin;
	size_t size, alloc_size;

	for (ptr.cc = begin = path;;) {
		switch (*ptr.cc++) {
		case '\0':
			goto _path_at_end;
		case '/':
			begin = ptr.cc;
		}
	}

_path_at_end:
	size = (size_t) ((ptrdiff_t) ptr.cc - (ptrdiff_t) begin);

	if (size < 1 || !(ptr.c = _mem_new (3, size + 1, &alloc_size))) {
		return NULL;
	}

	(void) memcpy (ptr.c, begin, size);
	(void) memset (ptr.c + size, 0, alloc_size - size);
	*result_len = size;

	alloc_size = 0;
	size = 0;
	begin = NULL;

	return ptr.c;
}

__attribute__((always_inline))
static inline char *
_img_name_from_buf (uint8_t *buf,
                    size_t   len,
                    size_t  *result_len)
{
	union {
		uint8_t    *u8;
		const char *cc;
		char       *c;
	} ptr = {.u8 = buf};
	const char *begin = ptr.cc;
	size_t i = 0;

	for (; i < len && ptr.cc[i];) {
		putchar (ptr.cc[i]);
		if (ptr.cc[i++] == '/') {
			begin = ptr.cc + i;
		}
	}
	puts("");

	size_t size = (size_t) ((ptrdiff_t) (ptr.cc + i) - (ptrdiff_t) begin);

	if (size < 1 || !(ptr.c = _mem_new (3, size + 1, &i))) {
		return NULL;
	}

	(void) memcpy (ptr.c, begin, size);
	(void) memset (ptr.c + size, 0, i - size);
	*result_len = size;

	i = 0;
	size = 0;
	begin = NULL;

	return ptr.c;
}

__attribute__((always_inline))
static inline void
_img_set_name (struct img *im,
               char       *name,
               size_t      size)
{
	im->nsiz = size;
	if (im->name) {
		_mem_del (&im->name);
	}
	im->name = name;
}

__attribute__((always_inline))
static inline bool
_img_dup_name_from_path (struct img *im,
                         const char *path)
{
	size_t size;
	char *name;
	if (!(name = _img_name_from_path (path, &size))) {
		return false;
	}
	_img_set_name (im, name, size);
	size = 0;
	name = NULL;
	return true;
}

__attribute__((always_inline))
static inline struct img *
_img_create (list_head_t *list,
             sha1_t      *hash,
             const char  *path,
             size_t       size,
             uint8_t     *data,
             size_t       width,
             size_t       height,
             uint8_t     *pxdata)
{
	struct img *im;
	size_t name_size;
	char *name;

	if (!(im = _img_alloc())) {
		fprintf (stderr, "%s: _img_alloc failed\n", __func__);
		return NULL;
	}

	if (path) {
		if (!(name = _img_name_from_path (path, &name_size))) {
			fprintf (stderr, "%s: _img_name_from_path failed\n",
			         __func__);
			_mem_del ((void **)&im);
			return NULL;
		}
	} else {
		name_size = 0;
		name = NULL;
	}

	_img_init (im, list, hash, name_size, name, size,
	           data, width, height, pxdata);

	return im;
}

__attribute__((always_inline))
static inline void
_img_destroy (struct img *im)
{
	if (im->hook.next && im->hook.prev) {
		list_del (&im->hook);
	}
/*
	struct banner *b, *tmp;
	list_for_each_entry_safe (b, tmp, &im->refs, by_hash) {
		list_del (&b->by_hash);
		_sha1_init (&b->hash);
	}
*/
	im->refs.next = NULL;
	im->refs.prev = NULL;

	im->nsiz = 0;
	if (im->name) {
		free (im->name);
		im->name = NULL;
	}

	_sha1_init (&im->hash);

	im->size = 0;
	if (im->data) {
		free (im->data);
		im->data = NULL;
	}

	im->width = 0;
	im->height = 0;
	if (im->pxdata) {
		free (im->pxdata);
		im->pxdata = NULL;
	}

	free (im);
}

__attribute__((always_inline))
static inline const char *
_img_name (struct img *im)
{
	return (const char *) im->name;
}

__attribute__((always_inline))
static inline size_t
_img_name_length (struct img *im)
{
	return im->nsiz;
}

__attribute__((always_inline))
static inline void
_img_set_data (struct img *im,
               uint8_t    *data,
               size_t      size)
{
	if (im->data) {
		_mem_del (&im->data);
	}

	_sha1_gen (&im->hash, size, data);

	im->size = size;
	im->data = data;
}

__attribute__((always_inline))
static inline bool
_img_import_buffer (struct img    *im,
                    const uint8_t *data,
                    size_t         data_len)
{
	bool r;
	uint8_t *_data;
	size_t _size;

	if (!(_data = _mem_new (6, data_len + 1, &_size))) {
		r = false;
	} else {
		(void) memcpy (_data, data, data_len);
		(void) memset (_data + data_len, 0, _size - data_len);

		_img_set_data (im, _data, data_len);

		_data = NULL;
		r = true;
	}

	_size = 0;

	return r;
}

__attribute__((always_inline))
static inline struct img *
_img_create_from_packet (list_head_t *list,
                         sha1_t      *hash,
                         size_t       name_size,
                         size_t       im_size,
                         uint8_t     *name_ptr,
                         uint8_t     *im_ptr)
{
	uint8_t *data;
	size_t alloc_size, n;
	char *name;
	struct img *im;

	if (!(data = _mem_new (6, im_size + 1, &alloc_size))) {
		fprintf (stderr, "%s: failed to allocate memory for data\n",
		         __func__);
		return NULL;
	}

	(void) memcpy (data, im_ptr, im_size);
	(void) memset (data + im_size, 0, alloc_size - im_size);

	if (!(name = _img_name_from_buf (name_ptr, name_size, &n))) {
		fprintf (stderr, "%s: failed to copy filename from packet\n",
		         __func__);
		goto _fail_free_data;
	}

	if (!(im = _img_alloc())) {
		fprintf (stderr, "%s: _img_alloc failed\n", __func__);
		free (name);
	_fail_free_data:
		free (data);
		return NULL;
	}

	_img_init (im, list, hash, n, name, im_size, data, 0, 0, NULL);

	return im;
}

__attribute__((always_inline))
static inline bool
_read_img_file (const char  *path,
                uint8_t    **data,
                size_t      *size)
{
	bool r;
	file_t *f;
	uint8_t *_data;
	size_t _size, alloc_size;

	printf ("%s: path=%s\n", __func__, path);

	if (!(f = file_create (path))) {
		fprintf (stderr, "%s: file_create failed\n", __func__);
		r = false;
		goto _end;
	}

	if (!file_open (f, "rb")) {
		fprintf (stderr, "%s: file_open failed\n", __func__);
		r = false;
		goto _destroy_file;
	}

	if (!file_size (f, &_size)) {
		fprintf (stderr, "%s: file_size failed\n", __func__);
		goto _fail_close_file;
	}

	if (!(_data = _mem_new (6, _size + 1, &alloc_size))) {
		fprintf (stderr, "%s: memory allocation failed\n", __func__);
	_fail_close_file:
		r = false;
		goto _close_file;
	}

	if (!(r = file_read (f, alloc_size, _data, &_size))) {
		fprintf (stderr, "%s: file_read failed\n", __func__);
	}

_close_file:
	if (!file_close (f)) {
		fprintf (stderr, "%s: file_close failed\n", __func__);
	}

	if (!r) {
		goto _destroy_file;
	}

	(void) memset ((void *) (_data + _size), 0, alloc_size - _size);

	*data = _data;
	*size = _size;

	_data = NULL;
	_size = 0;
	alloc_size = 0;

	r = true;

_destroy_file:
	file_destroy (&f);

_end:
	return r;
}

__attribute__((always_inline))
static inline bool
_img_import_file (struct img *im,
                  const char *path)
{
	bool r;
	size_t size, name_size;
	uint8_t *data;
	char *name;

	if (_read_img_file (path, &data, &size)) {
		if ((name = _img_name_from_path (path, &name_size))) {
			_img_set_data (im, data, size);
			_img_set_name (im, name, name_size);
			data = NULL;
			name = NULL;
			r = true;
		} else {
			fprintf (stderr, "%s: _img_name_from_path failed\n", __func__);
			(void) memset ((void *) data, 0, size);
			_mem_del (&data);
			r = false;
		}
		size = 0;
	} else {
		fprintf (stderr, "%s: _read_img_file failed\n", __func__);
		r = false;
	}

	char str[41];
	_sha1_str (&im->hash, str);
	printf ("%s: read %zu bytes, SHA1=%s\n", __func__, im->size, str);

	return r;
}

__attribute__((always_inline))
static inline struct img *
_img_create_from_file (const char *path)
{
	struct img *im;

	if ((im = _img_create (NULL, NULL, path, 0, NULL, 0, 0, NULL))) {
		if (!_img_import_file (im, path)) {
			_img_destroy (im);
			im = NULL;
		}
	}

	return im;
}

__attribute__((always_inline))
static inline bool
_img_hash_cmp (struct img *im,
               sha1_t      hash)
{
	return _sha1_cmp (&im->hash, &hash);
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

__attribute__((always_inline))
static inline const uint8_t *
_img_pxdata (struct img *im)
{
	return (const uint8_t *) im->pxdata;
}

__attribute__((always_inline))
static inline struct img *
_img_find_in_list_by_hash (list_head_t *list,
                           sha1_t       hash)
{
	struct img *im;
	list_for_each_entry (im, list, hook) {
		if (_img_hash_cmp (im, hash)) {
			return im;
		}
	}
	return NULL;
}

__attribute__((always_inline))
static inline bool
_img_export (struct img *im,
             char       *path,
             size_t      pos)
{
	bool r;
	file_t *f;

	(void) strncpy (path + pos, _img_name (im), _img_name_length (im));
	path[pos + _img_name_length (im)] = '\0';

	if (!(f = file_create (path))) {
		fprintf (stderr, "%s: file_create failed\n", __func__);
		r = false;

	} else {
		if (!file_open (f, "wb")) {
			fprintf (stderr, "%s: file_open failed\n", __func__);
			r = false;

		} else {
			if (!file_write (f, im->size,
			                 (const uint8_t *) im->data)) {
				fprintf (stderr, "%s: file_write failed\n",
				         __func__);
				r = false;
			} else {
				r = true;
				printf ("%s: wrote %s\n", __func__, path);
			}

			if (!file_close (f)) {
				fprintf (stderr, "%s: file_close failed\n",
				         __func__);
			}
		}

		file_destroy (&f);
	}

	path[pos] = '\0';

	return r;
}

__attribute__((always_inline))
static inline bool
_img_serialize (struct img  *im,
                size_t      *pos,
                size_t      *len,
                uint8_t    **buf)
{
	union {
		uint8_t *u;
		char    *c;
	} b = {.u = *buf};
	size_t l = *len, p = *pos;
	size_t need = 40 + 10 + 10 + sizeof (
		"{\"hash\":\"\""
		",\"w\":,\"h\":"
		",\"file\":\"\"}");

	if (l - p < need) {
		uint8_t *_b;
		if (!(_b = realloc (b.u, (l += BUFSIZ)))) {
			fprintf (stderr, "%s: realloc: %s\n",
			         __func__, strerror (errno));
			return false;
		}
		b.u = _b;
	}

	(void) strncpy (b.c + p, "{\"hash\":\"", 9);
	p += 9;

	_img_hash_unparse (im, b.c + p);
	p += 40;

	(void) strncpy (b.c + p, "\",\"w\":", 6);
	p += 6;

	p += _u32_to_str (im->width, b.c + p);

	(void) strncpy (b.c + p, ",\"h\":", 5);
	p += 5;

	p += _u32_to_str (im->height, b.c + p);

	(void) strncpy (b.c + p, ",\"file\":\"", 9);
	p += 9;

	(void) _json_stringify ((im->name) ? (const char *) im->name : "",
	                        &p, &l, &b.u);

	(void) strncpy (b.c + p, "\"}", 2);
	p += 2;

	*pos = p;
	*len = l;
	*buf = b.u;

	return true;
}

#endif // __KIVIJALKA_PRIVATE_IMG_H__

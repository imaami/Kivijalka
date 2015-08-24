#ifndef __KIVIJALKA_PRIVATE_BANNER_H__
#define __KIVIJALKA_PRIVATE_BANNER_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../banner.h"
#include "../global.h"
#include "../list.h"
#include "../point.h"
#include "../file.h"
#include "../img_file.h"
#include "geo2d.h"
#include "sha1.h"
#include "json.h"
#include "img.h"
#include "mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

struct banner {
	list_head_t   by_uuid;
	list_head_t   by_hash;
	uuid_t        uuid;
	char         *name;
	point_t       offset;
	struct geo2d  dims;
	sha1_t        hash;
	img_data_t   *data;
};

struct banner_packet {
	uint32_t     type;
	uint64_t     time;
	point_t      offs;
	struct geo2d dims;
	sha1_t       hash;
	uint32_t     nsiz;   //! Banner name length without terminating NUL
	uint32_t     fsiz;   //! Image filename length without terminating NUL
	uint64_t     size;   //! Image data size
	uint8_t      data[]; //! Payload: banner name, filename, data
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline struct banner *
_banner_create (void)
{
	struct banner *b;

	if (!(b = aligned_alloc (64, sizeof (struct banner)))) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
	} else {
		list_init (&b->by_uuid);
		list_init (&b->by_hash);
		uuid_clear (b->uuid);
		b->name = NULL;
		b->offset.u64 = 0;
		_geo2d_init (&b->dims);
		_sha1_init (&b->hash);
		b->data = NULL;
	}

	return b;
}

__attribute__((always_inline))
static inline struct banner_packet *
_banner_packet_inspect (const char *buf,
                        int         len)
{
	size_t n;
	struct banner_packet *p;
	size_t size, nsiz, fsiz;
	sha1_t hash;

	if (len < 1) {
		fprintf (stderr, "%s: illegal packet length\n", __func__);
		return NULL;
	}

	n = len;

	if (n < 64) {
		goto _packet_too_short;
	}

	n -= 64;
	p = (struct banner_packet *) buf;
	size = p->size;

	if (n < size) {
		goto _packet_too_short;
	}

	n -= size;
	nsiz = p->nsiz;

	if (n < nsiz) {
		goto _packet_too_short;
	}

	n -= nsiz;
	fsiz = p->fsiz;

	if (n < fsiz) {
	_packet_too_short:
		fprintf (stderr, "%s: packet too short\n", __func__);
		return NULL;
	}

	if (n != fsiz) {
		fprintf (stderr, "%s: inconsistent packet length\n", __func__);
		return NULL;
	}

	_sha1_gen (&hash, size, p->data + nsiz + fsiz);

	char str[41];
	_sha1_str (&hash, str);
	printf ("payload hash: %s\n", str);

	if (!_sha1_cmp (&hash, &p->hash)) {
		fprintf (stderr, "%s: hash sum mismatch\n", __func__);
		return NULL;
	}

	return p;
}

__attribute__((always_inline))
static inline struct banner *
_banner_create_from_packet (struct banner_packet *p)
{
	size_t name_size, alloc_size;
	union {
		uint8_t    *u8;
		const char *cc;
	} name_ptr;
	struct banner *b;
	char *name;

	name_size = p->nsiz;
	name_ptr.u8 = p->data;

	if (!(b = _mem_new (6, sizeof (struct banner), &alloc_size))) {
		fprintf (stderr, "%s: failed to allocate banner\n", __func__);
		return NULL;
	}

	if (!(name = _mem_new (3, name_size + 1, &alloc_size))) {
		fprintf (stderr, "%s: failed to allocate banner name string\n",
		         __func__);
		free (b);
		return NULL;
	}

	(void) strncpy (name, name_ptr.cc, name_size);
	name[name_size] = '\0';

	list_init (&b->by_uuid);
	list_init (&b->by_hash);
	b->name = name;
	b->offset.u64 = p->offs.u64;
	_geo2d_cpy (&p->dims, &b->dims);
	_sha1_cpy (&p->hash, &b->hash);
	b->data = NULL;

	return b;
}

__attribute__((always_inline))
static inline void
_banner_destroy (struct banner *b)
{
	if (b->by_uuid.next && b->by_uuid.prev) {
		list_del (&b->by_uuid);
	}
	if (b->by_hash.next && b->by_hash.prev) {
		list_del (&b->by_hash);
	}
	uuid_clear (b->uuid);
	if (b->name) {
		free (b->name);
		b->name = NULL;
	}
	b->offset.u64 = 0;
	_sha1_init (&b->hash);
	if (b->data) {
		free (b->data);
		b->data = NULL;
	}
	free (b);
}

__attribute__((always_inline))
static inline void
_banner_uuid_cpy (struct banner *b,
                  uuid_t         dest)
{
	uuid_copy (dest, b->uuid);
}

__attribute__((always_inline))
static inline bool
_banner_set_name (struct banner *b,
                  const char    *name)
{
	if (b->name) {
		free (b->name);
	}
	if (!(b->name = strdup (name))) {
		fprintf (stderr, "%s: strdup failed: %s\n", __func__,
		         strerror (errno));
		return false;
	}
	return true;
}

__attribute__((always_inline))
static inline void
_banner_set_offset (struct banner *b,
                    point_t        offset)
{
	b->offset.u64 = offset.u64;
}

__attribute__((always_inline))
static inline int32_t
_banner_offset_x (struct banner *b)
{
	return b->offset.x;
}

__attribute__((always_inline))
static inline int32_t
_banner_offset_y (struct banner *b)
{
	return b->offset.y;
}

__attribute__((always_inline))
static inline bool
_banner_hash_cmp (struct banner *b,
                  sha1_t        *hash)
{
	return _sha1_cmp (&b->hash, hash);
}

__attribute__((always_inline))
static inline void
_banner_hash_cpy (struct banner *b,
                  sha1_t        *dest)
{
	_sha1_cpy (&b->hash, dest);
}

__attribute__((always_inline))
static inline void
_banner_uuid_unparse (struct banner *b,
                      char          *str)
{
	uuid_unparse_lower (b->uuid, str);
}

__attribute__((always_inline))
static inline void
_banner_hash_unparse (struct banner *b,
                      char          *str)
{
	_sha1_str (&b->hash, str);
}

__attribute__((always_inline))
static inline struct img_data *
_banner_img (struct banner *b)
{
	return b->data;
}

__attribute__((always_inline))
static inline img_data_t *
_banner_remove_data (struct banner *b)
{
	img_data_t *imd = b->data;
	b->data = NULL;
	_sha1_init (&b->hash);
	return imd;
}

__attribute__((always_inline))
static inline bool
_banner_serialize (struct banner  *ban,
                   size_t         *pos,
                   size_t         *len,
                   uint8_t       **buf)
{
	union {
		uint8_t *u;
		char    *c;
	} b = {.u = *buf};
	size_t l = *len, p = *pos;
	size_t need = 36 + 40 + 42 + sizeof (
		"{\"uuid\":\"\",\"hash\":\"\""
		",\"w\":,\"h\":,\"x\":,\"y\":"
		",\"name\":\"\"}");

	if (l - p < need) {
		uint8_t *_b;
		if (!(_b = realloc (b.u, (l += BUFSIZ)))) {
			fprintf (stderr, "%s: realloc: %s\n",
			         __func__, strerror (errno));
			return false;
		}
		b.u = _b;
	}

	(void) strncpy (b.c + p, "{\"uuid\":\"", 9);
	p += 9;

	_banner_uuid_unparse (ban, b.c + p);
	p += 36;

	(void) strncpy (b.c + p, "\",\"hash\":\"", 10);
	p += 10;

	_banner_hash_unparse (ban, b.c + p);
	p += 40;

	(void) strncpy (b.c + p, "\",\"w\":", 6);
	p += 6;

	p += _u32_to_str (ban->dims.w, b.c + p);

	(void) strncpy (b.c + p, ",\"h\":", 5);
	p += 5;

	p += _u32_to_str (ban->dims.h, b.c + p);

	(void) strncpy (b.c + p, ",\"x\":", 5);
	p += 5;

	p += _i32_to_str (ban->offset.x, b.c + p);

	(void) strncpy (b.c + p, ",\"y\":", 5);
	p += 5;

	p += _i32_to_str (ban->offset.y, b.c + p);

	(void) strncpy (b.c + p, ",\"name\":\"", 9);
	p += 9;

	(void) _json_stringify ((ban->name) ? (const char *) ban->name : "",
	                        &p, &l, &b.u);

	(void) strncpy (b.c + p, "\"}", 2);
	p += 2;

	*pos = p;
	*len = l;
	*buf = b.u;

	return true;
}

__attribute__((always_inline))
static inline bool
_banner_export (struct banner *b,
                char          *path,
                size_t         path_len)
{
	bool r;
	file_t *f;
	union {
		char     c[48];
		uint64_t u64[6];
	} buf = {.u64 = {0, 0, 0, 0, 0, 0}};
	char *fpath;
	union {
		char          *chr;
		const char    *cch;
		const uint8_t *cu8;
	} ptr;
	size_t len;

	path[path_len] = 'i';
	path[path_len + 1] = '\0';

	if (!(f = file_create (path))) {
		fprintf (stderr, "%s: file_create failed\n", __func__);
		r = false;
		goto _cleanup;
	}

	// image hash

	ptr.chr = buf.c;
	_sha1_str (&b->hash, ptr.chr);

	if (!file_open (f, "w")) {
		goto _fail_open;
	}

	if (!file_write (f, 40, ptr.cu8)) {
		goto _fail_write;
	}

	if (!file_close (f)) {
		goto _fail_close;
	}

	fpath = (char *) file_path (f);

	// width

	len = _u32_to_str (b->dims.w, ptr.chr);

	fpath[path_len] = 'w';
	if (!file_open (f, "w")) {
		goto _fail_open;
	}

	if (!file_write (f, len, ptr.cu8)) {
		goto _fail_write;
	}

	if (!file_close (f)) {
		goto _fail_close;
	}

	// height

	len = _u32_to_str (b->dims.h, ptr.chr);

	fpath[path_len] = 'h';
	if (!file_open (f, "w")) {
		goto _fail_open;
	}

	if (!file_write (f, len, ptr.cu8)) {
		goto _fail_write;
	}

	if (!file_close (f)) {
		goto _fail_close;
	}

	// x offset

	len = _i32_to_str (b->offset.x, ptr.chr);

	fpath[path_len] = 'x';
	if (!file_open (f, "w")) {
		goto _fail_open;
	}

	if (!file_write (f, len, ptr.cu8)) {
		goto _fail_write;
	}

	if (!file_close (f)) {
		goto _fail_close;
	}

	// y offset

	len = _i32_to_str (b->offset.y, ptr.chr);

	fpath[path_len] = 'y';
	if (!file_open (f, "w")) {
		goto _fail_open;
	}

	if (!file_write (f, len, ptr.cu8)) {
		goto _fail_write;
	}

	if (!file_close (f)) {
		goto _fail_close;
	}

	// name

	if (b->name) {
		ptr.chr = b->name;
		len = strlen (ptr.cch);
	} else {
		ptr.cch = "";
		len = 0;
	}

	fpath[path_len] = 'n';
	if (!file_open (f, "w")) {
	_fail_open:
		ptr.cch = "open";
		goto _fail;
	}

	if (!file_write (f, len, ptr.cu8)) {
	_fail_write:
		ptr.cch = "write";
		goto _fail;
	}

	if (!file_close (f)) {
	_fail_close:
		ptr.cch = "close";
	_fail:
		fprintf (stderr, "%s: file_%s failed\n", __func__, ptr.cch);
		r = false;
		goto _destroy_file;
	}

	r = true;

_destroy_file:
	file_destroy (&f);

_cleanup:
	buf.u64[0] = 0;
	buf.u64[1] = 0;
	buf.u64[2] = 0;
	buf.u64[3] = 0;
	buf.u64[4] = 0;
	buf.u64[5] = 0;
	fpath = NULL;
	ptr.chr = NULL;
	len = 0;

	path[path_len] = '\0';

	return r;
}

__attribute__((always_inline))
static inline struct banner *
_banner_find_in_list_by_uuid (list_head_t *list,
                              uuid_t       uuid)
{
	struct banner *b;
	list_for_each_entry (b, list, by_uuid) {
		if (uuid_compare (b->uuid, uuid) == 0) {
			return b;
		}
	}
	return NULL;
}

__attribute__((always_inline))
static inline bool
_banner_activate (struct banner *b,
                  struct img    *im)
{
	img_data_t *imd;

	if (!im->data) {
		fprintf (stderr, "%s: image has null data\n", __func__);
		return false;
	}

	if (!(imd = img_data_new_from_buffer (im->size,
	                                      (const char *) im->data))) {
		fprintf (stderr, "%s: img_data_new_from_buffer failed\n",
		         __func__);
		return false;
	}

	img_file_replace_data (&banner_file, imd);
	imd = NULL;

	cur_banner = b;

	if (sem_post (&process_sem)) {
		fprintf (stderr, "%s: sem_post failed: %s\n", __func__,
		         strerror (errno));
		return false;
	}

	return true;
}

#endif // __KIVIJALKA_PRIVATE_BANNER_H__

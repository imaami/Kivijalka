#ifndef __KIVIJALKA_PRIVATE_BANNER_H__
#define __KIVIJALKA_PRIVATE_BANNER_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../banner.h"
#include "../global.h"
#include "../list.h"
#include "../file.h"
#include "../img_file.h"
#include "point.h"
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
#include <inttypes.h>

struct banner {
	list_head_t   by_uuid;
	list_head_t   by_hash;
	uuid_t        uuid;
	size_t        name_len;
	char         *name;
	point_t       offset;
	struct geo2d  dims;
	sha1_t        hash;
	img_data_t   *data;
};

struct banner_packet {
	uint64_t     time;
	point_t      offs;
	struct geo2d dims;
	sha1_t       hash;
	uint32_t     nsiz;   //! Banner name length without terminating NUL
	uint32_t     fsiz;   //! Image filename length without terminating NUL
	uint64_t     size;   //! Image data size
	uint8_t      data[]; //! Payload: banner name, filename, data
} __attribute__((gcc_struct,packed));

struct banner_modpkt {
	uint64_t     time;   //! Timestamp
	uuid_t       uuid;   //! UUID of banner to modify
	point_t      offs;   //! Offset
	struct geo2d dims;   //! Dimensions
	sha1_t       hash;   //! Image hash
	uint32_t     nsiz;   //! Banner name length; 0 means don't change name
	uint8_t      name[]; //! Optional banner name
} __attribute__((gcc_struct,packed));

struct banner_modpkt_echo {
	uint32_t             type;
	struct banner_modpkt orig;
} __attribute__((gcc_struct,packed));

struct banner_info {
	uuid_t        uuid;
	point_t       offs;
	struct geo2d  dims;
	sha1_t        hash;
	uint32_t      nsiz;
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline void
_banner_info_init (struct banner_info *i,
                   struct banner      *b)
{
	uuid_copy (i->uuid, b->uuid);
	_point_cpy (&b->offset, &i->offs);
	_geo2d_cpy (&b->dims, &i->dims);
	_sha1_cpy (&b->hash, &i->hash);
	i->nsiz = (b->name_len < UINT32_MAX) ? b->name_len : UINT32_MAX;
}

__attribute__((always_inline))
static inline struct banner_modpkt_echo *
_banner_modpkt_echo_create (struct banner *b)
{
	union {
		struct banner_modpkt_echo *echo;
		struct {
			uint32_t           type;
			uint64_t           time;
			struct banner_info info;
			uint8_t            name[];
		} *_echo;
	} mem;
	size_t s, n;

	s = offsetof (struct banner_modpkt_echo, orig.name) + b->name_len;

	if (!(mem.echo = _mem_new (6, s, &n))) {
		fprintf (stderr, "%s: failed to allocate memory\n", __func__);

	} else {
		_banner_info_init (&mem._echo->info, b);
		for (uint8_t *ptr = (uint8_t *) mem.echo; s < n; ++s) {
			ptr[s] = 0;
		}
	}

	return mem.echo;
}

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
		b->name_len = 0;
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

	if (n < 60) {
		goto _packet_too_short;
	}

	n -= 60;
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
static inline struct banner_modpkt *
_banner_modpkt_inspect (const char *buf,
                        int         len)
{
	size_t n;
	struct banner_modpkt *p;

	n = len;

	if (n < offsetof (struct banner_modpkt, name)) {
		goto _packet_too_short;
	}

	n -= offsetof (struct banner_modpkt, name);
	p = (struct banner_modpkt *) buf;

	if (n < p->nsiz) {
	_packet_too_short:
		fprintf (stderr, "%s: packet too short\n", __func__);
		return NULL;
	}
/*
	if (p->dims.w < 1 || p->dims.h < 1) {
		fprintf (stderr,
		         "%s: packet defines invalid width and/or height\n",
		         __func__);
		return NULL;
	}
*/
	return p;
}

__attribute__((always_inline))
static inline void
_banner_name_trimmed_size (const char *str,
                           size_t      len,
                           size_t     *leading_ws,
                           size_t     *trim_len)
{
	const uint8_t *s = (const uint8_t *) str;
	size_t o = 0, t = 0;

	// skip leading whitespace
	for (;; ++o) {
		if (o >= len) {
			goto _at_end;
		}

		switch (s[o]) {
		case 0x09: case 0x20:
			continue;

		case 0x00 ... 0x08:
		case 0x0a ... 0x1f:
		case 0x7f:
		case 0xf8 ... 0xff:
			goto _at_end;
		}

		*leading_ws = o;
		goto _skip_to_next;
	}

	for (;;) {
		if (o >= len) {
			goto _at_end;
		}

		switch (s[o]) {
		case 0x09: case 0x20:
			for (;;) {
				if (++o >= len) {
					goto _at_end;
				}

				switch (s[o]) {
				case 0x09: case 0x20:
					continue;

				case 0x00 ... 0x08:
				case 0x0a ... 0x1f:
				case 0x7f:
				case 0xf8 ... 0xff:
					goto _at_end;
				}

				break;
			}

			++t;
			break;

		// C0 controls
		case 0x00 ... 0x08:
		case 0x0a ... 0x1f:
		// delete
		case 0x7f:
		// invalid UTF-8
		case 0xf8 ... 0xff:
			goto _at_end;
		}

	_skip_to_next:
		++o;
		++t;
	}

_at_end:
	*trim_len = t;

	return;
}

__attribute__((always_inline))
static inline void
_banner_name_copy_trimmed (const char *src,
                           size_t      src_len,
                           char       *dest,
                           size_t      dest_len)
{
	if (src_len < 1 || dest_len < 1) {
		return;
	}

	union {
		const char    *cc;
		const uint8_t *u8;
		char          *c;
	} _src = {.cc = src};
	size_t s = 0, d = 0;

	for (;;) {
		switch (_src.u8[s]) {
		case 0x00 ... 0x08:
		case 0x0a ... 0x1f:
		case 0x7f:
		case 0xf8 ... 0xff:
			goto _at_end;
		}

	_next_valid_char:
		dest[d++] = _src.c[s++];

		if (d >= dest_len) {
			d = dest_len - 1;
			goto _at_end;
		}

		if (s >= src_len) {
			goto _at_end;
		}

		switch (_src.u8[s]) {
		case 0x09: case 0x20:
			if (d + 1 >= dest_len) {
				goto _at_end;
			}

			for (;;) {
				if (++s >= src_len) {
					goto _at_end;
				}

				switch (_src.u8[s]) {
				case 0x09: case 0x20:
					continue;

				case 0x00 ... 0x08:
				case 0x0a ... 0x1f:
				case 0x7f:
				case 0xf8 ... 0xff:
					goto _at_end;
				}

				break;
			}

			dest[d++] = ' ';
			goto _next_valid_char;
		}
	}

_at_end:
	for (; d < dest_len; ++d) {
		dest[d] = '\0';
	}
}

__attribute__((always_inline))
static inline bool
_banner_name_dup (const char  *str,
                  size_t       len,
                  char       **dest,
                  size_t      *dest_len)
{
	size_t leading_ws, trim_len, alloc_size;
	char *name;

	if (len < 1) {
		fprintf (stderr, "%s: name is empty\n", __func__);
		return false;
	}

	leading_ws = 0;

	_banner_name_trimmed_size (str, len, &leading_ws, &trim_len);

	if (trim_len < 1) {
		fprintf (stderr, "%s: trimmed name is empty\n", __func__);
		return false;
	}

	if (!(name = _mem_new (3, trim_len + 1, &alloc_size))) {
		fprintf (stderr, "%s: _mem_new failed\n", __func__);
		return false;
	}

	if (trim_len == len) {
		(void) strncpy (name, str, len);
		name[len] = '\0';
	} else {
		printf ("%s: name trimmed from %zu B to %zu B\n", __func__,
		        len, trim_len);
		_banner_name_copy_trimmed (str + leading_ws, len - leading_ws,
		                           name, alloc_size);
	}

	for (size_t k = trim_len; ++k < alloc_size;) {
		name[k] = '\0';
	}

	*dest = name;
	*dest_len = trim_len;

	return true;
}

__attribute__((always_inline))
static inline struct banner *
_banner_create_from_packet (struct banner_packet *p)
{
	size_t size;
	union {
		uint8_t    *u8;
		const char *cc;
	} name_ptr;
	struct banner *b;
	char *name;

	if (!(b = _mem_new (6, sizeof (struct banner), &size))) {
		fprintf (stderr, "%s: failed to allocate banner\n", __func__);
		return NULL;
	}

	name_ptr.u8 = p->data;

	if (!_banner_name_dup (name_ptr.cc, p->nsiz, &name, &size)) {
		fprintf (stderr, "%s: failed to copy banner name\n", __func__);
		free (b);
		return NULL;
	}

	list_init (&b->by_uuid);
	list_init (&b->by_hash);
	b->name_len = size;
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
	b->name_len = 0;
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
_banner_uuid_copy_to_u32 (struct banner *b,
                          uint32_t      *dest)
{
	uint32_t *src = (uint32_t *) (((char *) b)
	                              + offsetof (struct banner, uuid));
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}

__attribute__((always_inline))
static inline bool
_banner_set_name (struct banner *b,
                  const char    *name)
{
	size_t len, _name_len;
	char *_name;

	if ((len = strlen (name)) < 1) {
		fprintf (stderr, "%s: name is empty\n", __func__);
		return false;
	}

	if (!_banner_name_dup (name, len, &_name, &_name_len)) {
		fprintf (stderr, "%s: failed to copy banner name\n", __func__);
		return false;
	}

	if (b->name) {
		free (b->name);
	}

	b->name_len = _name_len;
	b->name = _name;

	return true;
}

__attribute__((always_inline))
static inline size_t
_banner_name_length (struct banner *b)
{
	return b->name_len;
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

	if (b->name && b->name_len > 0) {
		ptr.chr = b->name;
		len = b->name_len;
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
static inline bool
_banner_export_modifications (sha1_t       *hash,
                              struct geo2d *dims,
                              point_t      *offs,
                              char         *name,
                              size_t        name_len,
                              char         *path,
                              size_t        path_len)
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

	path[path_len] = '_';
	path[path_len + 1] = '\0';

	if (!(f = file_create (path))) {
		fprintf (stderr, "%s: file_create failed\n", __func__);
		r = false;
		goto _cleanup;
	}

	fpath = (char *) file_path (f);

	if (hash) {
		// image hash

		ptr.chr = buf.c;
		_sha1_str (hash, ptr.chr);

		fpath[path_len] = 'i';
		if (!file_open (f, "w")) {
			goto _fail_open;
		}

		if (!file_write (f, 40, ptr.cu8)) {
			goto _fail_write;
		}

		if (!file_close (f)) {
			goto _fail_close;
		}
	}

	if (dims) {
		// width & height

		len = _u32_to_str (dims->w, ptr.chr);

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

		len = _u32_to_str (dims->h, ptr.chr);

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
	}

	if (offs) {
		// x offset & y offset

		len = _i32_to_str (offs->x, ptr.chr);

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

		len = _i32_to_str (offs->y, ptr.chr);

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
	}

	if (name) {
		// name

		if (name_len > 0) {
			ptr.chr = name;
			len = name_len;
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

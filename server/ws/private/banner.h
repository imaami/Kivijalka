#ifndef __KIVIJALKA_PRIVATE_BANNER_H__
#define __KIVIJALKA_PRIVATE_BANNER_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../banner.h"
#include "../list.h"
#include "../point.h"
#include "geo2d.h"
#include "sha1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct banner {
	list_head_t   hook;
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
	uint32_t     nsiz;
	uint64_t     size;
	uint8_t      data[];
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline struct banner *
_banner_create (void)
{
	struct banner *b;

	if (!(b = aligned_alloc (64, sizeof (struct banner)))) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
	} else {
		list_init (&b->hook);
		b->name = NULL;
		b->offset.u64 = 0;
		_geo2d_init (&b->dims);
		_sha1_init (&b->hash);
		b->data = NULL;
	}

	return b;
}

__attribute__((always_inline))
static inline struct banner *
_banner_create_from_path (const char *path)
{
	struct banner *b;

	if ((b = aligned_alloc (64, sizeof (struct banner)))) {
		if ((b->data = img_data_new_from_file (path))) {
			if ((b->name = strdup (path))) {
				_sha1_gen (&b->hash, b->data->size,
				           (uint8_t *) b->data->data);
				b->offset.u64 = 0;
				_geo2d_init (&b->dims);
				list_init (&b->hook);
			} else {
				fprintf (stderr, "%s: strdup failed: %s\n", __func__,
				         strerror (errno));
				free (b->data);
				b->data = NULL;
			}
		} else {
			fprintf (stderr, "%s: image loading failed\n", __func__);
			free (b);
			b = NULL;
		}
	} else {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
	}

	return b;
}

__attribute__((always_inline))
static inline struct banner *
_banner_create_from_packet (struct banner_packet *pkt)
{
	char str[41];
	_sha1_str (&pkt->hash, str);
	printf ("type: %u\ntime: %lu\nxpos: %d\nypos: %d\nwidth: %d\nheight: %d\nhash: %s\nsize: %lu\n",
	        pkt->type, pkt->time, pkt->offs.x, pkt->offs.y,
		_geo2d_width (&pkt->dims), _geo2d_height (&pkt->dims), str, pkt->size);

	struct banner *b;

	if ((b = aligned_alloc (64, sizeof (struct banner)))) {
		_sha1_gen (&b->hash, (size_t) pkt->size,
		           pkt->data + (size_t) pkt->nsiz);
		_sha1_str (&b->hash, str);
		printf ("payload hash: %s\n", str);
		if (!_sha1_cmp (&b->hash, &pkt->hash)) {
			fprintf (stderr, "%s: hash sum mismatch\n", __func__);
			goto fail;
		} else if (!(b->name = strndup ((const char *) pkt->data,
		                                (size_t) pkt->nsiz))) {
			fprintf (stderr, "%s: strndup: %s\n", __func__, strerror (errno));
			goto fail2;
		} else if (!(b->data = img_data_new_from_buffer ((size_t) pkt->size, (const char *) pkt->data + (size_t) pkt->nsiz))) {
			fprintf (stderr, "%s: data copy failed\n", __func__);
			free (b->name);
		fail:
			b->name = NULL;
		fail2:
			_geo2d_init (&b->dims);
			_sha1_init (&b->hash);
			free (b);
			b = NULL;
		} else {
			b->offset.u64 = pkt->offs.u64;
			_geo2d_cpy (&pkt->dims, &b->dims);
			list_init (&b->hook);
			printf ("name: \"%s\"\n", b->name);
		}
	} else {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
	}

	return b;
}

__attribute__((always_inline))
static inline void
_banner_destroy (struct banner *b)
{
	b->hook.next = NULL;
	b->hook.prev = NULL;
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
_banner_print_hash (struct banner *b)
{
	char str[41];
	_sha1_str (&b->hash, str);
	puts (str);
}

__attribute__((always_inline))
static inline void
_banner_add_to_list (struct banner *b,
                     list_head_t   *list)
{
	list_add_tail (&b->hook, list);
}

__attribute__((always_inline))
static inline void
_banner_del_from_list (struct banner *b)
{
	__list_del_entry (&b->hook);
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

#endif // __KIVIJALKA_PRIVATE_BANNER_H__

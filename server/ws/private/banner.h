#ifndef __KIVIJALKA_PRIVATE_BANNER_H__
#define __KIVIJALKA_PRIVATE_BANNER_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../banner.h"
#include "../list.h"
#include "sha1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct banner {
	list_head_t  hook;
	char        *name;
	point_t      offset;
	sha1_t       hash;
};

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
		_sha1_init (&b->hash);
	}

	return b;
}

__attribute__((always_inline))
static inline struct banner *
_banner_create_from_path (const char *path)
{
	struct banner *b;

	if (!(b = aligned_alloc (64, sizeof (struct banner)))) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
	} else {
		list_init (&b->hook);
		if (!(b->name = strdup (path))) {
			fprintf (stderr, "%s: strdup failed: %s\n", __func__,
			         strerror (errno));
		}
		b->offset.u64 = 0;
		_sha1_init (&b->hash);
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
static inline void
_banner_set_hash (struct banner *b,
                  sha1_t         hash)
{
	b->hash.u32[0] = hash.u32[0];
	b->hash.u32[1] = hash.u32[1];
	b->hash.u32[2] = hash.u32[2];
	b->hash.u32[3] = hash.u32[3];
	b->hash.u32[4] = hash.u32[4];
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

#endif // __KIVIJALKA_PRIVATE_BANNER_H__

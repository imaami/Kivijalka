#ifndef __KIVIJALKA_PRIVATE_BANNER_CACHE_H__
#define __KIVIJALKA_PRIVATE_BANNER_CACHE_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../banner_cache.h"
#include "../list.h"
#include "sha1.h"
#include "banner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct bucket {
	list_head_t hook;
	list_head_t list;
} __attribute__((gcc_struct,packed));

struct banner_cache {
	char          *root_path;
	list_head_t    in_use;
	struct bucket *lookup_table;
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline struct banner_cache *
_banner_cache_create (const char *path)
{
	struct banner_cache *bc;

	if (!(bc = aligned_alloc (sizeof (struct banner_cache),
	                         sizeof (struct banner_cache)))) {
		fprintf (stderr, "%s: object allocation failed\n", __func__);
	} else if (!(bc->lookup_table = aligned_alloc (sizeof (struct bucket),
	                                               256 * sizeof (struct bucket)))) {
		fprintf (stderr, "%s: lookup table allocation failed\n", __func__);
		goto fail_free_bc;
	} else if (!(bc->root_path = strdup (path))) {
		fprintf (stderr, "%s: strdup failed: %s\n", __func__,
		         strerror (errno));
		free (bc->lookup_table);
		bc->lookup_table = NULL;
	fail_free_bc:
		free (bc);
		bc = NULL;
	} else {
		list_init (&bc->in_use);
		for (unsigned int i = 0; i < 256; ++i) {
			list_init (&bc->lookup_table[i].hook);
			list_init (&bc->lookup_table[i].list);
		}
	}

	return bc;
}

__attribute__((always_inline))
static inline void
_banner_cache_destroy (struct banner_cache *bc)
{
	if (bc->root_path) {
		free (bc->root_path);
		bc->root_path = NULL;
	}

	struct bucket *bkt, *tmp;
	struct banner *b, *tmp2;
	list_for_each_entry_safe (bkt, tmp, &bc->in_use, hook) {
		list_for_each_entry_safe (b, tmp2, &bkt->list, hook) {
			_banner_destroy (b);
		}
		__list_del_entry (&bkt->hook);
	}
	bkt = NULL;
	tmp = NULL;
	b = NULL;
	tmp2 = NULL;
	bc->in_use.next = NULL;
	bc->in_use.prev = NULL;

	if (bc->lookup_table) {
		(void) memset (bc->lookup_table, 0,
		               256 * sizeof (struct bucket));
		free (bc->lookup_table);
		bc->lookup_table = NULL;
	}

	free (bc);
	bc = NULL;
}

__attribute__((always_inline))
static inline const char *
_banner_cache_path (struct banner_cache *bc)
{
	return (const char *) bc->root_path;
}

__attribute__((always_inline))
static inline struct banner *
_banner_cache_find_banner (struct banner_cache *bc,
                           sha1_t              *hash)
{
	list_head_t *list = &bc->lookup_table[hash->u8[0]].list;
	struct banner *b;
	list_for_each_entry (b, list, hook) {
		if (_sha1_cmp (&b->hash, hash)) {
			return b;
		}
	}
	return NULL;
}

__attribute__((always_inline))
static inline bool
_banner_cache_add_banner (struct banner_cache *bc,
                          struct banner       *banner)
{
	struct bucket *bkt = &bc->lookup_table[banner->hash.u8[0]];
	struct banner *b;
	list_for_each_entry (b, &bkt->list, hook) {
		if (_sha1_cmp (&banner->hash, &b->hash)) {
			fprintf (stderr, "%s: hash collision!\n", __func__);
			return false;
		}
	}
	list_add (&banner->hook, &bkt->list);
	if (bkt->hook.next == &bkt->hook) {
		list_add (&bkt->hook, &bc->in_use);
	}
	return true;
}

__attribute__((always_inline))
static inline struct banner *
_banner_cache_most_recent (struct banner_cache *bc)
{
	list_head_t *h;
	if ((h = bc->in_use.next) != &bc->in_use) {
		struct bucket *bkt = list_entry (h, struct bucket, hook);
		if ((h = bkt->list.next) != &bkt->list) {
			return list_entry (h, struct banner, hook);
		}
	}
	return NULL;
}

#endif // __KIVIJALKA_PRIVATE_BANNER_CACHE_H__

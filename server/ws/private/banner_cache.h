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

struct table {
	list_head_t    in_use;
	struct bucket *lookup_table;
} __attribute__((gcc_struct,packed));

struct banner_cache {
	char          *root_path;
	struct table   by_uuid;
	struct table   by_hash;
	struct bucket *data;
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline struct banner_cache *
_banner_cache_create (const char *path)
{
	struct banner_cache *bc;

	if (!(bc = aligned_alloc (sizeof (struct banner_cache),
	                          sizeof (struct banner_cache)))) {
		fprintf (stderr, "%s: object allocation failed\n", __func__);
	} else if (!(bc->data = aligned_alloc (sizeof (struct bucket),
	                                       512 * sizeof (struct bucket)))) {
		fprintf (stderr, "%s: lookup table allocation failed\n", __func__);
		goto fail_free_bc;
	} else if (!(bc->root_path = strdup (path))) {
		fprintf (stderr, "%s: strdup failed: %s\n", __func__,
		         strerror (errno));
		free (bc->data);
		bc->data = NULL;
	fail_free_bc:
		free (bc);
		bc = NULL;
	} else {
		list_init (&bc->by_uuid.in_use);
		bc->by_uuid.lookup_table = bc->data;
		list_init (&bc->by_hash.in_use);
		bc->by_hash.lookup_table = &bc->data[256];
		for (unsigned int i = 0; i < 512; ++i) {
			list_init (&bc->data[i].hook);
			list_init (&bc->data[i].list);
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

	list_for_each_entry_safe (bkt, tmp, &bc->by_uuid.in_use, hook) {
		list_for_each_entry_safe (b, tmp2, &bkt->list, by_uuid) {
			_banner_destroy (b);
		}
		__list_del_entry (&bkt->hook);
	}
	bc->by_uuid.in_use.next = NULL;
	bc->by_uuid.in_use.prev = NULL;

	list_for_each_entry_safe (bkt, tmp, &bc->by_hash.in_use, hook) {
		list_for_each_entry_safe (b, tmp2, &bkt->list, by_hash) {
			_banner_destroy (b);
		}
		__list_del_entry (&bkt->hook);
	}
	bc->by_hash.in_use.next = NULL;
	bc->by_hash.in_use.prev = NULL;

	bkt = NULL;
	tmp = NULL;
	b = NULL;
	tmp2 = NULL;

	if (bc->data) {
		(void) memset (bc->data, 0, 512 * sizeof (struct bucket));
		free (bc->data);
		bc->data = NULL;
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
static inline struct bucket *
_bucket_by_uuid (struct banner_cache *bc,
                 const uuid_t         uuid)
{
	return &bc->by_uuid.lookup_table[uuid[0]];
}

__attribute__((always_inline))
static inline struct bucket *
_bucket_by_hash (struct banner_cache *bc,
                 sha1_t              *hash)
{
	return &bc->by_hash.lookup_table[hash->u8[0]];
}

__attribute__((always_inline))
static inline struct banner *
_banner_by_uuid (list_head_t  *list,
                 const uuid_t  uuid)
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
static inline struct banner *
_banner_by_hash (list_head_t *list,
                 sha1_t      *hash)
{
	struct banner *b;
	list_for_each_entry (b, list, by_hash) {
		if (_sha1_cmp (&b->hash, hash)) {
			return b;
		}
	}
	return NULL;
}

__attribute__((always_inline))
static inline struct banner *
_banner_cache_find_by_uuid (struct banner_cache *bc,
                            const uuid_t         uuid)
{
	struct bucket *bkt = _bucket_by_uuid (bc, uuid);
	return _banner_by_uuid (&bkt->list, uuid);
}

__attribute__((always_inline))
static inline struct banner *
_banner_cache_find_by_hash (struct banner_cache *bc,
                            sha1_t              *hash)
{
	struct bucket *bkt = _bucket_by_hash (bc, hash);
	return _banner_by_hash (&bkt->list, hash);
}

__attribute__((always_inline))
static inline bool
_banner_cache_add_banner (struct banner_cache *bc,
                          struct banner       *banner)
{
	struct bucket *bkt = _bucket_by_hash (bc, &banner->hash);
	struct banner *b;

	if ((b = _banner_by_hash (&bkt->list, &banner->hash))) {
		char str[41], str2[41];
		_banner_hash_unparse (banner, str);
		_banner_hash_unparse (b, str2);
		fprintf (stderr,
		         "hash collision: %p SHA1=%s\n"
		         "                   name=%s\n"
		         "            vs. %p SHA1=%s\n"
		         "                   name=%s\n",
		         banner, str, banner_name (banner),
		         b, str2, banner_name (b));
		return false;
	}

	list_add (&banner->by_hash, &bkt->list);

	if (bkt->hook.next == &bkt->hook) {
		list_add (&bkt->hook, &bc->by_hash.in_use);
	}

	uuid_t uuid;

	do {
		uuid_generate (uuid);
		bkt = _bucket_by_uuid (bc, uuid);
	} while (_banner_by_uuid (&bkt->list, uuid));

	uuid_copy (banner->uuid, uuid);
	list_add (&banner->by_uuid, &bkt->list);

	if (bkt->hook.next == &bkt->hook) {
		list_add (&bkt->hook, &bc->by_uuid.in_use);
	}

	return true;
}

__attribute__((always_inline))
static inline struct banner *
_banner_cache_most_recent (struct banner_cache *bc)
{
	list_head_t *h;
	if ((h = bc->by_hash.in_use.next) != &bc->by_hash.in_use) {
		struct bucket *bkt = list_entry (h, struct bucket, hook);
		if ((h = bkt->list.next) != &bkt->list) {
			return list_entry (h, struct banner, by_hash);
		}
	}
	return NULL;
}

#endif // __KIVIJALKA_PRIVATE_BANNER_CACHE_H__

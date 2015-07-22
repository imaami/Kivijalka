/** \file banner_cache.c
 *
 * Banner cache object.
 */

#include "private/banner_cache.h"

struct banner_cache *
banner_cache_create (const char *path)
{
	return (path) ? _banner_cache_create (path) : NULL;
}

void
banner_cache_destroy (struct banner_cache **bc)
{
	if (bc && *bc) {
		_banner_cache_destroy (*bc);
		*bc = NULL;
	}
}

const char *
banner_cache_path (struct banner_cache *bc)
{
	return (bc) ? _banner_cache_path (bc) : NULL;
}

struct banner *
banner_cache_find_by_hash (struct banner_cache *bc,
                           sha1_t              *hash)
{
	return (bc && hash) ? _banner_cache_find_by_hash (bc, hash) : NULL;
}

struct banner *
banner_cache_find_by_uuid (struct banner_cache *bc,
                           uuid_t               uuid)
{
	return (bc && uuid) ? _banner_cache_find_by_uuid (bc, uuid) : NULL;
}

bool
banner_cache_add_banner (struct banner_cache *bc,
                         struct banner       *banner)
{
	return (bc && banner) ? _banner_cache_add_banner (bc, banner) : false;
}

struct banner *
banner_cache_most_recent (struct banner_cache *bc)
{
	return (bc) ? _banner_cache_most_recent (bc) : NULL;
}

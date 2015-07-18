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

struct banner *
banner_cache_find_banner (struct banner_cache *bc,
                          sha1_t              *hash)
{
	return (bc && hash) ? _banner_cache_find_banner (bc, hash) : NULL;
}

bool
banner_cache_add_banner (struct banner_cache *bc,
                         struct banner       *banner)
{
	return (bc && banner) ? _banner_cache_add_banner (bc, banner) : false;
}

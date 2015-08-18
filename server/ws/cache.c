/** \file cache.c
 *
 * Cache object.
 */

#include "private/cache.h"

struct cache *
cache_create (const char *path)
{
	return (path) ? _cache_create (path) : NULL;
}

void
cache_destroy (struct cache **c)
{
	if (c && *c) {
		_cache_destroy (*c);
		*c = NULL;
	}
}

void
cache_import (struct cache *c)
{
	if (c) {
		_cache_import (c);
	}
}

const char *
cache_path (struct cache *c)
{
	return (c) ? _cache_path (c) : NULL;
}

bool
cache_add_banner (struct cache  *c,
                  struct banner *banner,
                  const bool     write_to_disk)
{
	return (c && banner)
	       ? _cache_add_banner (c, banner, write_to_disk)
	       : false;
}

struct banner *
cache_most_recent (struct cache *c)
{
	return (c) ? _cache_most_recent (c) : NULL;
}

char *
cache_json (struct cache *c)
{
	return (c) ? _cache_json (c) : NULL;
}

bool
cache_activate_banner (struct cache  *c,
                       struct banner *b)
{
	return (c && b) ? _cache_activate_banner (c, b) : false;
}

struct banner *
cache_find_banner_by_uuid_str (struct cache *c,
                               const char   *uuid_str)
{
	if (c && uuid_str) {
		uuid_t uuid;
		if (_parse_uuid (uuid_str, uuid)) {
			return _cache_find_banner_by_uuid (c, uuid);
		}
	}
	return NULL;
}

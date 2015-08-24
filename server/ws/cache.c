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

bool
cache_import_packet (struct cache         *c,
                     struct banner_packet *p)
{
	return (c && p) ? _cache_import_packet (c, p) : false;
}

const char *
cache_path (struct cache *c)
{
	return (c) ? _cache_path (c) : NULL;
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

char *
cache_json_stringified (struct cache *c)
{
	union {
		char       *c;
		uint8_t    *u8;
		const char *cc;
	} json, str;
	size_t len, pos;

	if (!c || !(json.c = _cache_json (c))) {
		return NULL;
	}

	str.c = NULL;
	len = 0;
	pos = 0;

	if (!_json_stringify (json.cc, &pos, &len, &str.u8)) {
		if (str.c) {
			free (str.c);
			str.c = NULL;
		}
	}

	free (json.c);
	json.c = NULL;

	return str.c;
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
		size_t n;
		if (_parse_uuid (uuid_str, uuid, &n)) {
			return _cache_find_banner_by_uuid (c, uuid);
		} else {
			fprintf (stderr, "%s: invalid UUID string: syntax error"
			         " at offset %zu\n", __func__, n);
		}
	}
	return NULL;
}

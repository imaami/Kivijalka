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
		(void) _cache_import (c);
	}
}

bool
cache_import_packet (struct cache         *c,
                     struct banner_packet *p)
{
	return (c && p) ? _cache_import_packet (c, p) : false;
}

bool
cache_apply_modpkt (struct cache          *c,
                    struct banner_modpkt  *p,
                    char                 **r,
                    size_t                *s)
{
	return (c && p && r && s) ? _cache_apply_modpkt (c, p, r, s) : false;
}

bool
cache_apply_display_modpkt (struct cache           *c,
                            struct display_modpkt  *p,
                            char                  **r,
                            size_t                 *s)
{
	return (c && p && r && s)
	       ? _cache_apply_display_modpkt (c, p, r, s)
	       : false;
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

bool
cache_info_packet (struct cache  *c,
                   uint8_t      **data,
                   size_t        *size)
{
	return (c && data && size) ? _cache_info_packet (c, data, size) : false;
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
	if (!c || !uuid_str) {
		fprintf (stderr, "%s: null argument(s)\n", __func__);
		return NULL;
	}

	return _cache_find_banner_by_uuid_str (c, uuid_str);
}

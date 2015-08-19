#include "bannercache.h"
#include "banner.h"
#include <cstdio>

#include "global.h"

QT_USE_NAMESPACE

BannerCache::BannerCache(cache_t *cache,
                         QObject *parent) :
	QObject(parent)
{
	if (!cache) {
		return;
	}

	this->ptr = cache;
	cache_import (cache);

	char *json = cache_json (cache);
	if (json) {
		std::puts (json);
		std::free (json);
	}

	const char *uuid_str = "2e0dcdfc078c444f832717d1a54cf244";
	banner_t *b;

	if ((b = cache_find_banner_by_uuid_str (cache, uuid_str))) {
		if (!cache_activate_banner (cache, b)) {
			std::fprintf (stderr, "%s: failed to activate banner\n",
			              __func__);
		}
	}
	
}

BannerCache::~BannerCache()
{
}

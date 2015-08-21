#include "bannercache.h"

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
}

BannerCache::~BannerCache()
{
}

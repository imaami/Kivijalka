#ifndef BANNERCACHE_H
#define BANNERCACHE_H

#include <QtCore/QObject>

#include "cache.h"

class BannerCache : public QObject
{
	Q_OBJECT
public:
	explicit BannerCache(cache_t *cache,
	                     QObject *parent = Q_NULLPTR);
	~BannerCache();

private:
	cache_t *ptr;
};

#endif // BANNERCACHE_H

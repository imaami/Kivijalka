#ifndef BANNERCACHE_H
#define BANNERCACHE_H

#include <QtCore/QObject>

#include "banner_cache.h"

class BannerCache : public QObject
{
	Q_OBJECT
public:
	explicit BannerCache(banner_cache_t *bc,
	                     QObject *parent = Q_NULLPTR);
	~BannerCache();

private:
	banner_cache_t *bc;
};

#endif // BANNERCACHE_H

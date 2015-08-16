#include "bannercache.h"
#include "banner.h"
#include <cstdio>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#include "global.h"

QT_USE_NAMESPACE

BannerCache::BannerCache(cache_t *cache,
                         QObject *parent) :
	QObject(parent)
{
	if (!cache) {
		return;
	}

	cache_import (cache);

	this->ptr = cache;

	QString path(cache_path (cache));
	banner_t *b;

	if (QDir(path).isReadable()) {
		path = QDir(path).absolutePath();
		QDirIterator di(path,
		                {"*.png", "*.jpg", "*.jpeg", "*.gif"},
		                QDir::Files|QDir::Readable);
		while (di.hasNext()) {
			if ((b = banner_create_from_path (di.next().toUtf8().data()))) {
				cache_add_banner (cache, b, false);
			}
		}
	}

	if ((b = cache_most_recent (cache))) {
		img_data_t *imd = img_data_new_from_file (banner_name (b));
		if (imd) {
			img_file_replace_data (&banner_file, imd);
			imd = NULL;
		}
		b = NULL;
	}
}

BannerCache::~BannerCache()
{
}

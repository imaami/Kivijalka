#include "bannercache.h"
#include "banner.h"
#include <cstdio>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#include "global.h"

QT_USE_NAMESPACE

BannerCache::BannerCache(banner_cache_t *bc,
                         QObject *parent) :
	QObject(parent)
{
	if (!bc) {
		return;
	}

	this->bc = bc;

	QString path(banner_cache_path (bc));
	banner_t *b;

	if (QDir(path).isReadable()) {
		path = QDir(path).absolutePath();
		QDirIterator di(path,
		                {"*.png", "*.jpg", "*.jpeg", "*.gif"},
		                QDir::Files|QDir::Readable);
		while (di.hasNext()) {
			if ((b = banner_create_from_path (di.next().toUtf8().data()))) {
				banner_cache_add_banner (bc, b, false);
			}
		}
	}

	if ((b = banner_cache_most_recent (bc))) {
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

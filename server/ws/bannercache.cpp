#include "bannercache.h"
#include <cstdio>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#include "global.h"

QT_USE_NAMESPACE

BannerCache::BannerCache(const QString &dirPath,
                         QObject *parent) :
	QObject(parent)
{
	list_init (&list);

	banner_t *b;

	if (QDir(dirPath).isReadable()) {
		path = QDir(dirPath).absolutePath();
		QDirIterator di(path,
		                {"*.png", "*.jpg", "*.jpeg", "*.gif"},
		                QDir::Files|QDir::Readable);
		while (di.hasNext()) {
			if ((b = banner_create_from_path (di.next().toUtf8().data()))) {
				banner_add_to_list (b, &list);
			}
		}
	}

	for (b = NULL; (b = banner_next_in_list (b, &list));) {
		std::printf ("BannerCache::%s: %s\n", __func__, banner_name (b));
	}

	if ((b = banner_next_in_list (b, &list))) {
		img_data_t *imd = img_data_new_from_file (banner_name (b));
		if (imd) {
			img_file_replace_data (&banner_file, imd);
		}
	}
}

BannerCache::~BannerCache()
{
	for (banner_t *b = banner_next_in_list (NULL, &list); b;) {
		banner *next = banner_next_in_list (b, &list);
		banner_del_from_list (b);
		banner_destroy (&b);
		b = next;
	}
}

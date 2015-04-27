#include "bannercache.h"
#include <cstdio>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

QT_USE_NAMESPACE

BannerCache::BannerCache(const QString &dirPath,
                         QObject *parent) :
	QObject(parent)
{
	if (QDir(dirPath).isReadable()) {
		path = QDir(dirPath).absolutePath();
		QDirIterator di(path,
		                {"*.png", "*.jpg", "*.jpeg", "*.gif"},
		                QDir::Files|QDir::Readable);
		while (di.hasNext()) {
			Banner *b = new Banner(di.next(), this);
			this->list.append(b);
		}
	}

	for (int i = 0; i < this->list.size(); ++i) {
		const Banner *b = this->list.at(i);
		std::printf("%s: %d x %d\n",
		            b->file.toUtf8().data(),
		            b->img.width(), b->img.height());
	}
}

BannerCache::~BannerCache()
{
}

bool BannerCache::add(Banner &b)
{
	return true;
}

bool BannerCache::del(Banner &b)
{
	return true;
}

Banner *BannerCache::get(const QString &filePath)
{
	return NULL; // TODO
}

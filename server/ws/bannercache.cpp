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

__attribute__((unused))
bool BannerCache::add(__attribute__((unused)) Banner &b)
{
	return true;
}

__attribute__((unused))
bool BannerCache::del(__attribute__((unused)) Banner &b)
{
	return true;
}

__attribute__((unused))
Banner *BannerCache::get(__attribute__((unused)) const QString &filePath)
{
	return NULL; // TODO
}

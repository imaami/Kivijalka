#include "bannercache.h"
#include <cstdio>
#include <QtCore/QStringList>

QT_USE_NAMESPACE

BannerCache::BannerCache(const QString &path,
                         QObject *parent) :
	QObject(parent),
	dir(path)
{
	if (dir.isReadable()) {
		(void)dir.makeAbsolute();
		QStringList nf;
		nf << "*.png" << "*.jpg" << "*.jpeg" << "*.gif";
		dir.setNameFilters(nf);
		foreach (QString str, dir.entryList(QDir::Files|QDir::Readable)) {
			std::printf("%s\n", str.toUtf8().data());
		}
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

Banner *BannerCache::get(const QString &path)
{
	return NULL; // TODO
}

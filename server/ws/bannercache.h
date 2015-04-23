#ifndef BANNERCACHE_H
#define BANNERCACHE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QDir>
#include "banner.h"

class BannerCache : public QObject
{
	Q_OBJECT
public:
	explicit BannerCache(const QString &path,
	                     QObject *parent = Q_NULLPTR);
	~BannerCache();

	bool add(Banner &b);
	bool del(Banner &b);
	Banner *get (const QString &path);

Q_SIGNALS:
	void bannerAdded(Banner *);
	void bannerRemoved(const QString &);
	void bannerModified(Banner *);

private:
	QDir dir;
	QList<Banner> list;
};

#endif // BANNERCACHE_H

#ifndef BANNERCACHE_H
#define BANNERCACHE_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "banner.h"
#include "list.h"

class BannerCache : public QObject
{
	Q_OBJECT
public:
	explicit BannerCache(const QString &dirPath,
	                     QObject *parent = Q_NULLPTR);
	~BannerCache();

private:
	QString path;
	list_head_t list;
};

#endif // BANNERCACHE_H

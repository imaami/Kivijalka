#include "banner.h"

#include <QtCore/QDebug>

QT_USE_NAMESPACE

Banner::Banner(const QString &imageFile,
               quint16 x, quint16 y,
               QObject *parent) :
	QObject(parent),
	file(imageFile),
	json("{}")
{
	// TODO: open banner file, populate metadata
	placement.x = x;
	placement.y = y;
}

Banner::~Banner()
{
}

QString Banner::metadata()
{
	return json;
}

void Banner::setPlacement(quint16 x, quint16 y)
{
	if (x != placement.x || y != placement.y) {
		placement.x = x;
		placement.y = y;
		emit placementChanged();
	}
}

void Banner::constructMetadata()
{
	// TODO: write metadata JSON to this->json
}

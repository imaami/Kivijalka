#include "banner.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

QT_USE_NAMESPACE

Banner::Banner(const QString &imageFile,
               QObject *parent) :
	QObject(parent),
	json("{}")
{
	QFile f(imageFile);
	QFileInfo i(f);

	if (i.isReadable()) {
		file = i.absoluteFilePath();
/*
		std::printf("%s: %lli bytes\n",
		            file.toUtf8().data(),
		            i.size());
*/
		if (f.open(QIODevice::ReadOnly)) {
			QByteArray buf = f.readAll();
			f.close();
			img = QImage::fromData(buf);
		}
	} else {
		std::fprintf(stderr, "Cannot read %s\n",
		             imageFile.toUtf8().data());
	}

	placement.x = 0;
	placement.y = 0;
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

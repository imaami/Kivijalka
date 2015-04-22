#ifndef BANNER_H
#define BANNER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QImage>

class Banner : public QObject
{
	Q_OBJECT
public:
	explicit Banner(const QString &imageFile,
	                quint16 x, quint16 y,
	                QObject *parent = Q_NULLPTR);
	~Banner();

	QString metadata();

Q_SIGNALS:
	void placementChanged();

private:
	void constructMetadata();
	void setPlacement(quint16 x, quint16 y);
	struct { quint16 x, y; } placement;
	QString file, json;
	QImage img;
};

#endif // BANNER_H
